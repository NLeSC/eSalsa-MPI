#ifndef _FRAGMENTS_BUFFER_H_
#define _FRAGMENTS_BUFFER_H_

#include <stdbool.h>
#include <stdint.h>
//#include <pthread.h>

#define PAGE_SIZE 4096L

typedef struct {

	// A mutex to make the fragment buffer thread safe.
//	pthread_mutex_t mutex;

	// A condition variable that allows a pop on the output queue to block until a fragment is available.
//	pthread_cond_t cond;

	// The buffer containing all message fragments.
	unsigned char *buffer;

	// The size of each fragment.
	int fragment_size;

	// The number of fragments available in the buffer.
	int fragment_count;

	// The number of pages used by each fragment.
	int pages_per_fragment;

	// A stack containing all free message fragments.
	int *free_stack;

	// The index of the top of the free message fragment stack. This entry contains the first free message.
	int free_top;

	// A fifo queue containing all used message fragments.
	int *used_queue;

	// The index head of the queue.
	int used_head;

	// The index of the tail of the queue.
	int used_tail;

	// The size the queue.
	int used_size;

} fragment_buffer;


/* Create a new message fragment buffer */
fragment_buffer *fragment_buffer_create(int fragment_size, int fragment_count);

/* Destroy a message fragment buffer */
int fragment_buffer_destroy(fragment_buffer *fb);

/**
 * Get the fragment index of the next free message fragment in the given fragment buffer.
 *
 * Returns 0 if successful, 1 when there are no free fragments, -1 on error.
 */
int fragment_buffer_pop_free_fragment(fragment_buffer *fb, int *index);

/**
 * Get the fragment index of the next free message fragment in the given fragment buffer. If no fragments are available, wait up
 * to timeout_usec microseconds for a fragment to become available. A timeout of 0 will return immediately, a negative timeout
 * will block until a fragment appears.
 *
 * Returns 0 if successful, 1 when there are no free fragments, -1 on error.
 */
int fragment_buffer_pop_free_fragment_wait(fragment_buffer *fb, int *index, int64_t timeout_usec);

/**
 * Push the fragment index onto the free stack of the fragment buffer.
 *
 * Returns 0 if successful, -1 on error.
 */
int fragment_buffer_push_free_fragment(fragment_buffer *fb, int index);

/**
 * Adds the fragment index to the tail of the used queue of the fragment_buffer.
 *
 * Returns 0 if successful, -1 on error.
 */
//int fragment_buffer_enqueue_used_fragment(fragment_buffer *fb, int index);

/**
 * Dequeue a fragment index from the head of the used queue of the fragment_buffer
 *
 * Returns 0 if successful, 1 when the queue is empty, -1 on error.
 */
//int fragment_buffer_dequeue_used_fragment(fragment_buffer *fb, int *index);

/**
 * Returns a pointer for the message fragment with the given index.
 *
 * Returns pointer if successful, NULL on error.
 */
unsigned char *fragment_buffer_get_fragment(fragment_buffer *fb, int index);



#endif // _FRAGMENTS_BUFFER_H_
