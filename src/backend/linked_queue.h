#ifndef _LINKED_QUEUE_H_
#define _LINKED_QUEUE_H_

#include <stdbool.h>

#define DEFAULT_NODE_CACHE_SIZE 1024

// Nodes for the linked queue of data messages
typedef struct s_linked_queue_node linked_queue_node;

struct s_linked_queue_node {
	linked_queue_node *next;
	void *data;
	size_t size;
};

// A generic linked queue.
typedef struct {
	// Head and tail point to the ends of the queue.
	linked_queue_node *head;
	linked_queue_node *tail;

	// The number of elements in the queue.
	int length;

	// The maximum allowed sum of all size fields in all queued elements.
	size_t max_data_size;

	// The sum of all size fields in all queued elements.
	size_t data_size;

	// A cache of queue nodes.
	linked_queue_node *node_cache;
	int node_cache_size;
	int max_node_cache_size;

} linked_queue;

// Create a new queue.
linked_queue *linked_queue_create(int max_node_cache_size);

// Destroy an existing empty queue.
bool linked_queue_destroy(linked_queue *queue);

// Enqueue an element at the tail of the queue.
bool linked_queue_enqueue(linked_queue *queue, void *elt, size_t size);

// Dequeue the element from the head of the queue.
void *linked_queue_dequeue(linked_queue *queue);

// Return the element from the head of the queue, without removeing it from the queue.
void *linked_queue_peek(linked_queue *queue);

// Dequeue up to count elements from the head of the queue.
int linked_queue_bulk_dequeue(linked_queue *queue, void **buffer, int size);

// Is the queue empty ?
bool linked_queue_isempty(linked_queue *queue);

// Length of the queue ?
int linked_queue_length(linked_queue *queue);

// Size of the queue ?
int linked_queue_data_size(linked_queue *queue);

#endif // _LINKED_QUEUE_H_
