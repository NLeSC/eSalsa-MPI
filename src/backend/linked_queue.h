#ifndef _LINKED_QUEUE_H_
#define _LINKED_QUEUE_H_

#include <stdbool.h>

// Nodes for the linked queue of data messages
typedef struct s_linked_queue_node linked_queue_node;

struct s_linked_queue_node {
   linked_queue_node *next;
   void *data;
};

// A generic linked queue.
typedef struct {
   int size;
   linked_queue_node *head;
   linked_queue_node *tail;
} linked_queue;

// Create a new queue.
linked_queue *linked_queue_create();

// Destroy an existing empty queue.
bool linked_queue_destroy(linked_queue *queue);

// Enqueue an element at the tail of the queue.
bool linked_queue_enqueue(linked_queue *queue, void *elt);

// Dequeue an element from the head of the queue.
void *linked_queue_dequeue(linked_queue *queue);

// Is the queue empty ?
bool linked_queue_isempty(linked_queue *queue);

// Size of the queue ?
int linked_queue_size(linked_queue *queue);

#endif // _LINKED_QUEUE_H_
