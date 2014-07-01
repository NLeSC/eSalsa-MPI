#ifndef _BOUNDED_ARRAY_QUEUE_H_
#define _BOUNDED_ARRAY_QUEUE_H_

#include <stdbool.h>

// A generic bounded array queue.
typedef struct {
   int capacity;
   int size;
   int head;
   int tail;
   void **entries;
} bounded_array_queue;

bounded_array_queue *bounded_array_queue_create(int capacity);
bool bounded_array_queue_destroy(bounded_array_queue *queue);

bool bounded_array_queue_enqueue(bounded_array_queue *queue, void *elt);
void *bounded_array_queue_dequeue(bounded_array_queue *queue);

bool bounded_array_queue_isempty(bounded_array_queue *queue);
bool bounded_array_queue_isfull(bounded_array_queue *queue);

int bounded_array_queue_capacity(bounded_array_queue *queue);
int bounded_array_queue_size(bounded_array_queue *queue);

#endif // _BOUNDED_ARRAY_QUEUE_H_
