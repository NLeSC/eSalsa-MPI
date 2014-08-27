#ifndef _BLOCKING_LINKED_QUEUE_H_
#define _BLOCKING_LINKED_QUEUE_H_

#include <stdbool.h>
#include <pthread.h>

#include "linked_queue.h"

#define DEFAULT_MAX_DATA_SIZE (1024*1024)

// A linked queue that can block on a maximum size.
typedef struct {

	// The actual queue.
	linked_queue *queue;

	// The maximum allowed sum of all size fields in all queued elements.
	size_t max_data_size;

	// A mutex to make the queue thread safe.
	pthread_mutex_t mutex;

	// A condition variable that allows dequeue to block.
	pthread_cond_t cond;

} blocking_linked_queue;

// Create a new queue.
blocking_linked_queue *blocking_linked_queue_create(int max_node_cache_size, size_t max_data_size);

// Destroy an existing empty queue.
bool blocking_linked_queue_destroy(blocking_linked_queue *queue);

// Enqueue an element at the tail of the queue.
bool blocking_linked_queue_enqueue(blocking_linked_queue *queue, void *elt, size_t size, int64_t timeout_usec);

// Will an enqueue block ?
bool blocking_linked_queue_enqueue_will_block(blocking_linked_queue *queue);

// Dequeue a single element from the head of the queue. If the queue is empty,
// wait for at most timeout_usec for an element to appear. If no element
// is available within the specified timeout, NULL is returned.
//
// When timeout_usec is set to 0, linked_queue_dequeue will always return
// immediately.
//
// When timeout_usec is set to -1, no timeout is used and linked_queue_dequeue
// will block until an element becomes available.
void *blocking_linked_queue_dequeue(blocking_linked_queue *queue, int64_t timeout_usec);

// Dequeue multiple elements from the head of the queue. If the queue is empty,
// wait for at most timeout_usec for an element to appear. If no element
// is available within the specified timeout, 0 is returned.
//
// When timeout_usec is set to 0, linked_queue_bulk_dequeue will always return
// immediately.
//
// When timeout_usec is set to -1, no timeout is used and linked_queue_bulk_dequeue
// will block until an element becomes available.
int blocking_linked_queue_bulk_dequeue(blocking_linked_queue *queue, void **buffer, int count, int64_t timeout_usec);

// Is the queue empty ?
bool blocking_linked_queue_isempty(blocking_linked_queue *queue);

// Returns the length of the queue.
int blocking_linked_queue_length(blocking_linked_queue *queue);

// Returns the total size of all queue data elements.
int blocking_linked_queue_data_size(blocking_linked_queue *queue);

#endif // _BLOCKING_LINKED_QUEUE_H_
