
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>

#include "linked_queue.h"
#include "sync_linked_queue.h"

sync_linked_queue *sync_linked_queue_create(int max_node_cache_size)
{
	int error = 0;

	sync_linked_queue *tmp = malloc(sizeof(sync_linked_queue));

	if (tmp == NULL) {
		return NULL;
	}

	tmp->queue = linked_queue_create(max_node_cache_size);

	error = pthread_mutex_init(&tmp->mutex, NULL);

	if (error != 0) {
		return NULL;
	}

	return tmp;
}

bool sync_linked_queue_destroy(sync_linked_queue *queue)
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

	linked_queue_destroy(queue->queue);

	free(queue);
	return true;
}


bool sync_linked_queue_enqueue(sync_linked_queue *queue, void *elt, size_t size)
{
	bool result;

	// Lock the queue first.
	pthread_mutex_lock(&queue->mutex);

	result = linked_queue_enqueue(queue->queue, elt, size);

	pthread_mutex_unlock(&queue->mutex);

	return result;
}

void *sync_linked_queue_dequeue(sync_linked_queue *queue)
{
	void *elt;
	size_t size;

	// Lock the queue first.
	pthread_mutex_lock(&queue->mutex);

	// Get element
	elt = linked_queue_dequeue(queue->queue);

	// Release the lock and return the element.
	pthread_mutex_unlock(&queue->mutex);
	return elt;
}

int sync_linked_queue_length(sync_linked_queue *queue)
{
	int length;

	pthread_mutex_lock(&queue->mutex);

	length = linked_queue_length(queue->queue);

	pthread_mutex_unlock(&queue->mutex);

	return length;
}

int sync_linked_queue_data_size(sync_linked_queue *queue)
{
	int data_size;

	pthread_mutex_lock(&queue->mutex);

	data_size = linked_queue_data_size(queue->queue);

	pthread_mutex_unlock(&queue->mutex);

	return data_size;
}
