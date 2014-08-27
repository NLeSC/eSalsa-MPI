
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>

#include "linked_queue.h"
#include "blocking_linked_queue.h"

blocking_linked_queue *blocking_linked_queue_create(int max_node_cache_size, size_t max_data_size)
{
	int error = 0;

	blocking_linked_queue *tmp = malloc(sizeof(blocking_linked_queue));

	if (tmp == NULL) {
		return NULL;
	}

	tmp->queue = linked_queue_create(max_node_cache_size);
	tmp->max_data_size = max_data_size;

	error = pthread_cond_init(&tmp->cond, NULL);

	if (error != 0) {
		return NULL;
	}

	error = pthread_mutex_init(&tmp->mutex, NULL);

	if (error != 0) {
		pthread_cond_destroy(&tmp->cond);
		return NULL;
	}

	return tmp;
}

bool blocking_linked_queue_destroy(blocking_linked_queue *queue)
{
	// NOTE: This cleanup assumes there is NO contention
	// on the queue anymore!

	if (queue == NULL) {
		// We need a queue to destroy!
		return false;
	}

	if (!linked_queue_isempty(queue->queue)) {
		// We can only destroy an empty queue!
		return false;
	}

	pthread_mutex_destroy(&queue->mutex);
	pthread_cond_destroy(&queue->cond);

	linked_queue_destroy(queue->queue);

	free(queue);
	return true;
}

static bool wait_for_space(blocking_linked_queue *queue, int64_t timeout_usec)
{
	int error;
	struct timespec alarm;
	struct timeval now;

	// If the max_data_size is set, check if queue is already over sized.
	if (queue->max_data_size > 0) {

		// We need to recheck the data size, as there may be multiple threads waiting for this condition.
		while (linked_queue_data_size(queue->queue) > queue->max_data_size) {

			// We must wait until there is enough space in queue
			if (timeout_usec == 0) {
				// 0 timeout, so simply return false
				return false;
			}

			if (timeout_usec < 0) {
				// negative timeout, so perform a blocking wait.
				pthread_cond_wait(&queue->cond, &queue->mutex);

			} else { // if (timeout_usec > 0)
				gettimeofday(&now, NULL);
				alarm.tv_sec = now.tv_sec + (timeout_usec / 1000000);
				alarm.tv_nsec = (now.tv_usec + (timeout_usec % 1000000)) * 1000;

				if (alarm.tv_nsec >= 1000000000) {
					alarm.tv_sec++;
					alarm.tv_nsec -= 1000000000;
				}

				// positive timeout, perform timed wait.
				error = pthread_cond_timedwait(&queue->cond, &queue->mutex, &alarm);

				if (error == ETIMEDOUT) {
					// If the timeout expired we return false
					return false;
				}
			}
		}
	}

	return true;
}

bool blocking_linked_queue_enqueue(blocking_linked_queue *queue, void *elt, size_t size, int64_t timeout_usec)
{
	// Lock the queue first.
	pthread_mutex_lock(&queue->mutex);

	// If the max_data_size is set, check if queue is already over sized.
	if (!wait_for_space(queue, timeout_usec)) {
		pthread_mutex_unlock(&queue->mutex);
		return false;
	}

	// There is enough space in the queue, so add the element.

	if (!linked_queue_enqueue(queue->queue, elt, size)) {
		// Failed to add element...
		pthread_mutex_unlock(&queue->mutex);
		return false;
	}

	if (linked_queue_length(queue->queue) == 1) {
		// We added to an empty queue so wake up any blocked dequeues.
		pthread_cond_broadcast(&queue->cond);
	}

	// Unlock the queue.
	pthread_mutex_unlock(&queue->mutex);
	return true;
}

bool blocking_linked_queue_enqueue_will_block(blocking_linked_queue *queue)
{
	bool result = false;

	// Lock the queue first.
	pthread_mutex_lock(&queue->mutex);

	if (queue->max_data_size > 0) {
		result = linked_queue_data_size(queue->queue) > queue->max_data_size;
	}

	// Unlock the queue.
	pthread_mutex_unlock(&queue->mutex);
	return result;
}

static bool wait_for_element(blocking_linked_queue *queue, int64_t timeout_usec)
{
	int error;
	struct timespec alarm;
	struct timeval now;

	// We need to recheck the length, as there may be multiple threads waiting
	while (linked_queue_length(queue->queue) == 0) {

		if (timeout_usec == 0) {
			// 0 timeout, so simply return false
			return false;
		}

		if (timeout_usec < 0) {
			// negative timeout, so perform a blocking wait.
			pthread_cond_wait(&queue->cond, &queue->mutex);

		} else { // if (timeout_usec > 0)

			gettimeofday(&now, NULL);
			alarm.tv_sec = now.tv_sec + (timeout_usec / 1000000);
			alarm.tv_nsec = (now.tv_usec + (timeout_usec % 1000000)) * 1000;

			if (alarm.tv_nsec >= 1000000000) {
				alarm.tv_sec++;
				alarm.tv_nsec -= 1000000000;
			}

			// positive timeout, perform timed wait.
			error = pthread_cond_timedwait(&queue->cond, &queue->mutex, &alarm);

			if (error == ETIMEDOUT) {
				// If the timeout expired we return false
				return false;
			}
		}
	}

	return true;
}

void *blocking_linked_queue_dequeue(blocking_linked_queue *queue, int64_t timeout_usec)
{
	void *elt;
	size_t size;

	// Lock the queue first.
	pthread_mutex_lock(&queue->mutex);

	// Make sure there is an element in the queue, or wait for it.
	if (!wait_for_element(queue, timeout_usec)) {
		pthread_mutex_unlock(&queue->mutex);
		return NULL;
	}

	// Queue is no longer empty, so return the head.

	// Remember the data size before dequeue.
	size = linked_queue_data_size(queue->queue);

	// Get element
	elt = linked_queue_dequeue(queue->queue);

	// Check if we may need to wake up a blocked enqueue thread.
	if (queue->max_data_size > 0 && size > queue->max_data_size) {
		// We we're over the size limit before the dequeue.
		// See if we are under the limit now. If so, wakeup any blocked threads.
		if (linked_queue_data_size(queue->queue) <= queue->max_data_size) {
			pthread_cond_broadcast(&queue->cond);
		}
	}

	// Release the lock and return the element.
	pthread_mutex_unlock(&queue->mutex);
	return elt;
}

int blocking_linked_queue_bulk_dequeue(blocking_linked_queue *queue, void **buffer, int count, int64_t timeout_usec)
{
	int stored;
	size_t size;

	// Lock the queue first.
	pthread_mutex_lock(&queue->mutex);

	// Make sure there is an element in the queue, or wait for it.
	if (!wait_for_element(queue, timeout_usec)) {
		pthread_mutex_unlock(&queue->mutex);
		return 0;
	}

	// Queue is not empty, so return as many elements as we can.

	// Remember the data size before dequeue.
	size = linked_queue_data_size(queue->queue);

	stored = linked_queue_bulk_dequeue(queue->queue, buffer, count);

	// Check if we may need to wake up a blocked enqueue thread.
	if (queue->max_data_size > 0 && size > queue->max_data_size) {
		// We we're over the size limit before the dequeue.
		// See if we are under the limit now. If so, wakeup any blocked threads.
		if (linked_queue_data_size(queue->queue) <= queue->max_data_size) {
			pthread_cond_broadcast(&queue->cond);
		}
	}

	// Release the lock and return the element.
	pthread_mutex_unlock(&queue->mutex);

	return stored;
}

bool blocking_linked_queue_isempty(blocking_linked_queue *queue)
{
	bool empty;

	pthread_mutex_lock(&queue->mutex);

	empty = linked_queue_isempty(queue->queue);

	pthread_mutex_unlock(&queue->mutex);

	return empty;
}

int blocking_linked_queue_length(blocking_linked_queue *queue)
{
	int length;

	pthread_mutex_lock(&queue->mutex);

	length = linked_queue_length(queue->queue);

	pthread_mutex_unlock(&queue->mutex);

	return length;
}

int blocking_linked_queue_data_size(blocking_linked_queue *queue)
{
	int data_size;

	pthread_mutex_lock(&queue->mutex);

	data_size = linked_queue_data_size(queue->queue);

	pthread_mutex_unlock(&queue->mutex);

	return data_size;
}
