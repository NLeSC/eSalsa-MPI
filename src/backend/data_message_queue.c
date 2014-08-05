#include <stdint.h>
#include <stdlib.h>
#include "logging.h"
#include "messaging.h"
#include "data_message_queue.h"
#include "empi.h"

data_message_queue *data_message_queue_create()
{
   data_message_queue *tmp = malloc(sizeof(data_message_queue));

   if (tmp == NULL) {
      return NULL;
   }

   tmp->head = NULL;
   tmp->tail = NULL;

   return tmp;
}

int data_message_enqueue(data_message_queue *q, data_message *m)
{
   data_message_info *info = malloc(sizeof(data_message_info));

   if (info == NULL) {
      return -1;
   }

   info->message = m;
   info->next = NULL;

   if (q->head == NULL) {
      q->head = q->tail = info;
   } else {
      q->tail->next = info;
      q->tail = info;
   }

   return 0;
}

data_message *data_message_dequeue(data_message_queue *q)
{
   data_message_info *current;
   data_message *message;

   if (q->head == NULL) {
      return NULL;
   }

   current = q->head;
   message = current->message;

   if (q->head == q->tail) {
      q->head = q->tail = NULL;
   } else {
      q->head = q->head->next;
   }

   free(current);

   return message;
}

int data_message_queue_empty(data_message_queue *q)
{
   return (q->head == NULL);
}

data_message *data_message_dequeue_matching(data_message_queue *q, int comm, int source, int tag)
{
   data_message_info *curr, *prev;
   data_message *result;

   DEBUG(4, "FIND_PENDING_MESSAGE: Checking for pending messages in comm=%d from source=%d tag=%d", comm, source, tag);

   if (q->head == NULL) {
      DEBUG(4, "FIND_PENDING_MESSAGE: No pending messages");
      return NULL;
   }

   curr = q->head;
   prev = NULL;

   while (curr != NULL) {

      if (match_message(curr->message, comm, source, tag)) {
          if (curr == q->head) {
              // delete head. check if list empty
              if (q->head == q->tail) {
                 q->head = q->tail = NULL;
              } else {
                 q->head = q->head->next;
              }
          } else if (curr == q->tail) {
              // delete tail. set tail to prev
              q->tail = prev;
              q->tail->next = NULL;
          } else {
              // delete middle.
              prev->next = curr->next;
          }

          curr->next = NULL;
          result = curr->message;
          free(curr);

          DEBUG(4, "FIND_PENDING_MESSAGE: Found pending message from %d", result->source);

          return result;
      }

      prev = curr;
      curr = curr->next;
   }

   DEBUG(4, "FIND_PENDING_MESSAGE: No matching messages");

   return NULL;
}



