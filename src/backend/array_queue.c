#include <stdlib.h>
#include <string.h>
#include "array_queue.h"

array_queue *array_queue_create()
{
   array_queue *queue = malloc(sizeof(array_queue));

   if (queue == NULL) {
      return NULL;
   }

   queue->capacity = ARRAY_QUEUE_INITIAL_SIZE;
   queue->size = 0;
   queue->head = 0;
   queue->tail = 0;
   queue->entries = malloc(ARRAY_QUEUE_INITIAL_SIZE * sizeof(void *));

//   memset((void *) queue->entries, 0, ARRAY_QUEUE_INITIAL_SIZE * sizeof(void *));

   return queue;
}

bool array_queue_destroy(array_queue *queue)
{
   if (queue == NULL) {
      // We need a queue to destroy!
      return false;
   }

   if (queue->size != 0) {
      // We can only destroy an empty queue!
      return false;
   }

   free(queue->entries);
   free(queue);
   return true;
}

static bool grow(array_queue *queue)
{
   void **tmp;

fprintf("REALLOC queue from %d to %d entries!\n", queue->capacity, queue->capacity*2);

   tmp = realloc(queue->entries, queue->capacity*2);

   if (tmp == NULL) {
      return false;
   }

   queue->entries = tmp;
   queue->capacity *= 2;
   return true;
}

static bool ensure_space(array_queue *queue)
{
   if (queue->size == queue->capacity) {
      return grow(queue);
   }

   return true;
}

bool array_queue_enqueue(array_queue *queue, void *elt)
{
   if (!ensure_space(queue)) {
      return false;
   }

   queue->entries[queue->tail] = elt;
   queue->tail = (queue->tail+1) % queue->capacity;
   queue->size++;
   return true;
}

void *array_queue_dequeue(array_queue *queue)
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

bool array_queue_isempty(array_queue *queue)
{
   return (queue->size == 0);
}

int array_queue_size(array_queue *queue)
{
   return queue->size;
}



