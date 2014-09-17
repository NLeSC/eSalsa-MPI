#ifndef _ARRAY_QUEUE_H_
#define _ARRAY_QUEUE_H_

#include <stdbool.h>

#define ARRAY_QUEUE_INITIAL_SIZE 32

// A generic array queue.
typedef struct {
   int capacity;
   int size;
   int head;
   int tail;
   void **entries;
} array_queue;

array_queue *array_queue_create();
bool array_queue_destroy(array_queue *queue);

bool array_queue_enqueue(array_queue *queue, void *elt);
void *array_queue_dequeue(array_queue *queue);

bool array_queue_isempty(array_queue *queue);
int array_queue_size(array_queue *queue);

#endif // _ARRAY_QUEUE_H_
