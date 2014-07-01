#include <stdlib.h>
#include "linked_queue.h"

linked_queue *linked_queue_create()
{
   linked_queue *tmp = malloc(sizeof(linked_queue));

   if (tmp == NULL) {
      return NULL;
   }

   tmp->size = 0;
   tmp->head = NULL;
   tmp->tail = NULL;
   return tmp;
}

bool linked_queue_destroy(linked_queue *queue)
{
   if (queue == NULL) {
      // We need a queue to destroy!
      return false;
   }

   if (queue->head != NULL) {
      // We can only destroy an empty queue!
      return false;
   }

   free(queue);
   return true;
}

bool linked_queue_enqueue(linked_queue *queue, void *elt)
{
   linked_queue_node *n = malloc(sizeof(linked_queue_node));

   if (n == NULL) {
      return false;
   }

   n->next = NULL;
   n->data = elt;

   if (queue->head == NULL) {
      queue->head = queue->tail = n;
   } else {
      queue->tail->next = n;
      queue->tail = n;
   }
   queue->size++;
   return true;
}

void *linked_queue_dequeue(linked_queue *queue)
{
   linked_queue_node *current;
   void *elt;

   if (queue->head == NULL) {
      return NULL;
   }

   current = queue->head;
   elt = current->data;

   if (queue->head == queue->tail) {
      queue->head = queue->tail = NULL;
   } else {
      queue->head = queue->head->next;
   }

   free(current);
   queue->size--;
   return elt;
}

bool linked_queue_isempty(linked_queue *queue)
{
   return (queue->head == NULL);
}

int linked_queue_size(linked_queue *queue)
{
   return queue->size;
}
