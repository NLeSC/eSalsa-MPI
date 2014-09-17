#ifndef _REQUEST_QUEUE_H_
#define _REQUEST_QUEUE_H_

#include <stdbool.h>
#include "request.h"

#define DEFAULT_NODE_CACHE_SIZE 1024

// Nodes for the linked queue of data messages
typedef struct s_request_queue_node request_queue_node;

struct s_request_queue_node {
	request_queue_node *next;
	request *data;
};

// A generic linked queue.
typedef struct {
	// Head and tail point to the ends of the queue.
	request_queue_node *head;
	request_queue_node *tail;

	// The number of elements in the queue.
	int length;

	// A cache of queue nodes.
	request_queue_node *node_cache;
	int node_cache_size;
	int max_node_cache_size;

} request_queue;

// Create a new queue.
request_queue *request_queue_create(int max_node_cache_size);

// Destroy an existing empty queue.
bool request_queue_destroy(request_queue *queue);

// Enqueue an element at the tail of the queue.
bool request_queue_enqueue(request_queue *queue, request *elt);

// Dequeue the element from the head of the queue.
request *request_queue_dequeue(request_queue *queue);

// Return the element from the head of the queue, without removeing it from the queue.
request *request_queue_dequeue_matching(request_queue *queue, int comm, int source, int tag);

// Return the element from the head of the queue, without removeing it from the queue.
request *request_queue_peek(request_queue *queue);

// Length of the queue ?
int request_queue_length(request_queue *queue);


#endif // _LINKED_QUEUE_H_
