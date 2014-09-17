#ifndef _SYNC_LINKED_QUEUE_H_
#define _SYNC_LINKED_QUEUE_H_

#include <stdbool.h>
#include <pthread.h>

#include "linked_queue.h"

// A linked queue that can block on a maximum size.
typedef struct {

	// The actual queue.
	linked_queue *queue;

	// A mutex to make the queue thread safe.
	pthread_mutex_t mutex;

} sync_linked_queue;

// Create a new queue.
sync_linked_queue *sync_linked_queue_create(int max_node_cache_size);

// Destroy an existing empty queue.
bool sync_linked_queue_destroy(sync_linked_queue *queue);

// Enqueue an element at the tail of the queue.
bool sync_linked_queue_enqueue(sync_linked_queue *queue, void *elt, size_t size);

// Dequeue a single element from the head of the queue.
void *sync_linked_queue_dequeue(sync_linked_queue *queue);

// Returns the length of the queue.
int sync_linked_queue_length(sync_linked_queue *queue);

// Returns the total size of all queue data elements.
int sync_linked_queue_data_size(sync_linked_queue *queue);

#endif // _SYNC_LINKED_QUEUE_H_
