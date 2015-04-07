#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>

#include "fragments_buffer.h"

fragment_buffer *fragment_buffer_create(int fragment_size, int fragment_count)
{
	int i;
	size_t allocsize;
	fragment_buffer *tmp;

	tmp = malloc(sizeof(fragment_buffer));

	if (tmp == NULL) {
		return NULL;
	}

	tmp->fragment_size = fragment_size;
	tmp->fragment_count = fragment_count;
	tmp->pages_per_fragment = fragment_size / PAGE_SIZE;

	if (fragment_size % PAGE_SIZE != 0) {
		tmp->pages_per_fragment++;
	}

	allocsize = PAGE_SIZE;
	allocsize *= (size_t) tmp->pages_per_fragment;
	allocsize *= (size_t) fragment_count;

	tmp->buffer = malloc(allocsize);

	if (tmp->buffer == NULL) {
		free(tmp);
		return NULL;
	}

	tmp->free_stack = (int *) malloc(fragment_count * sizeof(int));

	if (tmp->free_stack == NULL) {
		free(tmp->buffer);
		free(tmp);
		return NULL;
	}

	tmp->free_top = 0;

	for (i=0;i<fragment_count;i++) {
		tmp->free_stack[i] = i;
	}

	tmp->used_queue = (int *) malloc(fragment_count * sizeof(int));

	if (tmp->used_queue == NULL) {
		free(tmp->free_stack);
		free(tmp->buffer);
		free(tmp);
		return NULL;
	}

	tmp->used_size = 0;
	tmp->used_head = -1;
	tmp->used_tail = -1;

	for (i=0;i<fragment_count;i++) {
		tmp->used_queue[i] = -1;
	}

//	error = pthread_cond_init(&(tmp->cond), NULL);
//
//	if (error != 0) {
//		free(tmp->used_queue);
//		free(tmp->free_stack);
//		free(tmp->buffer);
//		free(tmp);
//		return NULL;
//	}
//
//	error = pthread_mutex_init(&(tmp->mutex), NULL);
//
//	if (error != 0) {
//		pthread_cond_destroy(&(tmp->cond));
//		free(tmp->used_queue);
//		free(tmp->free_stack);
//		free(tmp->buffer);
//		free(tmp);
//		return NULL;
//	}

	return tmp;
}

int fragment_buffer_destroy(fragment_buffer *fb)
{
	if (fb == NULL) {
		return -1;
	}

	if (fb->used_size != 0 || fb->free_top != 0) {
		return 1;
	}

//	pthread_cond_destroy(&(fb->cond));
//	pthread_mutex_destroy(&(fb->mutex));

	free(fb->used_queue);
	free(fb->free_stack);
	free(fb->buffer);
	free(fb);

	return 0;
}

int fragment_buffer_pop_free_fragment(fragment_buffer *fb, int *index)
{
	int result;

	if (fb == NULL || index == NULL) {
		return -1;
	}

	// Lock the stack first.
//	pthread_mutex_lock(&(fb->mutex));

	if (fb->free_top == fb->fragment_count) {
		// The stack is empty.
		result = 1;
	} else {
		*index = fb->free_stack[fb->free_top];
		fb->free_top++;
		result = 0;
	}

//	pthread_mutex_unlock(&(fb->mutex));

	return result;
}

/*
static uint64_t current_time_micros()
{
	uint64_t result;
	struct timeval t;

	gettimeofday(&t,NULL);

	result = (t.tv_sec * 1000000LU) + t.tv_usec;

	return result;
}
*/

//static int wait(pthread_cond_t *cond, pthread_mutex_t *mutex, uint64_t timeout_usec, uint64_t *left)
//{
//	int error;
//	struct timespec alarm;
//	struct timeval now, t;
//
//	if (timeout_usec <= 0) {
//		return 0;
//	}
//
//	// We have a positive timeout, so record the current time.
//	gettimeofday(&now, NULL);
//
//	alarm.tv_sec = now.tv_sec + (timeout_usec / 1000000);
//	alarm.tv_nsec = (now.tv_usec + (timeout_usec % 1000000)) * 1000;
//
//	if (alarm.tv_nsec >= 1000000000) {
//		alarm.tv_sec++;
//		alarm.tv_nsec -= 1000000000;
//	}
//
//	error = pthread_cond_timedwait(cond, mutex, &alarm);
//
//	gettimeofday(&t, NULL);
//
//	*left = ((t.tv_sec * 1000000LU) + t.tv_usec) - ((now.tv_sec * 1000000LU) + now.tv_usec);
//
//	return error;
//}

int fragment_buffer_pop_free_fragment_wait(fragment_buffer *fb, int *index, int64_t timeout_usec)
{
	int error;
//	uint64_t left;

	if (fb == NULL || index == NULL) {
		return -1;
	}

	// Lock the stack first.
//	pthread_mutex_lock(&(fb->mutex));

	error = 0;

	// No more fragments.
	if (fb->free_top == fb->fragment_count) {

//		if (timeout_usec == 0) {
			// We don't wait for an element to appear, but return immediately!
			return 1;
//		}

//		while (fb->free_top == fb->fragment_count && error == 0) {
//
//			if (timeout_usec < 0) {
//				// negative timeout, so perform a blocking wait.
//				error = pthread_cond_wait(&fb->cond, &fb->mutex);
//			} else {
//				// positive timeout, so perform a timed wait.
//				left = 0;
//
//				error = wait(&fb->cond, &fb->mutex, timeout_usec, &left);
//
//				// Check what happened.
//				if (error == 0) {
//					timeout_usec = left;
//				}
//			}
//		}
	}

	if (error == 0) {
		// We have a free fragment.
		*index = fb->free_stack[fb->free_top];
		fb->free_top++;
	} else if (error == ETIMEDOUT) {
		// We have a timeout
		error = 1;
	} else {
		// We have an error
		error = -1;
	}

//	pthread_mutex_unlock(&(fb->mutex));

	return error;
}


int fragment_buffer_push_free_fragment(fragment_buffer *fb, int index)
{
	if (fb == NULL || index < 0 || index >= fb->fragment_count) {
		return -1;
	}

	// Lock the queue first.
//	pthread_mutex_lock(&(fb->mutex));

	fb->free_top--;
	fb->free_stack[fb->free_top] = index;

	// Lock the queue first.
//	pthread_mutex_unlock(&(fb->mutex));

	return 0;
}

int fragment_buffer_enqueue_used_fragment(fragment_buffer *fb, int index)
{
	if (fb == NULL || index < 0 || index >= fb->fragment_count || fb->used_size == fb->fragment_count) {
		return -1;
	}

	if (fb->used_size == 0) {
		fb->used_size = 1;
		fb->used_head = fb->used_tail = 0;
		fb->used_queue[0] = index;
	} else {
		fb->used_tail = (fb->used_tail + 1) % fb->fragment_count;
		fb->used_queue[fb->used_tail] = index;
		fb->used_size++;
	}
	return 0;
}

int fragment_buffer_dequeue_used_fragment(fragment_buffer *fb, int *index)
{
	if (fb == NULL || index == NULL) {
		return -1;
	}

	if (fb->used_size == 0) {
		return 1;
	}

	*index = fb->used_queue[fb->used_head];

	fb->used_size--;

	if (fb->used_size == 0) {
		fb->used_head = fb->used_tail = -1;
	} else {
		fb->used_head = (fb->used_head + 1) % fb->fragment_count;
	}

	return 0;
}

unsigned char *fragment_buffer_get_fragment(fragment_buffer *fb, int index)
{
	size_t offset;

	if (fb == NULL || index < 0 || index >= fb->fragment_count) {
		return NULL;
	}

	offset = (size_t)index * (size_t)fb->pages_per_fragment * PAGE_SIZE;

	return (fb->buffer + offset);
}

