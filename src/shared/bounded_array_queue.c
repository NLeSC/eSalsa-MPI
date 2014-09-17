#include <stdlib.h>
#include <string.h>
#include "bounded_array_queue.h"

bounded_array_queue *bounded_array_queue_create(int capacity)
{
   bounded_array_queue *tmp;

   if (capacity <= 0) {
      return NULL;
   }

   tmp = malloc(sizeof(bounded_array_queue));

   if (tmp == NULL) {
      return NULL;
   }

   tmp->capacity = capacity;
   tmp->size = 0;
   tmp->head = 0;
   tmp->tail = 0;
   tmp->entries = malloc(capacity * sizeof(void *));

   memset((void *) tmp->entries, 0, capacity * sizeof(void *));

   return tmp;
}

bool bounded_array_queue_destroy(bounded_array_queue *queue)
{
   if (queue == NULL) {
      return false;
   }

   if (queue->size != 0) {
      return false;
   }

   free(queue->entries);
   free(queue);
   return true;
}

bool bounded_array_queue_enqueue(bounded_array_queue *queue, void *elt)
{
   if (queue->size == queue->capacity) {
      return false;
   }

   queue->entries[queue->tail] = elt;
   queue->tail = (queue->tail+1) % queue->capacity;
   queue->size++;
   return true;
}

void *bounded_array_queue_dequeue(bounded_array_queue *queue)
{
   void *elt;

   if (queue->size == 0) {
      return NULL;
   }

   elt = queue->entries[queue->head];

   queue->size--;

   if (queue->size == 0) {
      queue->head = 0;
      queue->tail = 0;
   } else {
      queue->head = (queue->head+1) % queue->capacity;
   }

   return elt;
}

bool bounded_array_queue_isempty(bounded_array_queue *queue)
{
   return (queue->size == 0);
}

bool bounded_array_queue_isfull(bounded_array_queue *queue)
{
   return (queue->size == queue->capacity);
}

int bounded_array_queue_capacity(bounded_array_queue *queue)
{
   return queue->capacity;
}

int bounded_array_queue_size(bounded_array_queue *queue)
{
   return queue->size;
}

