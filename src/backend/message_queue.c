#include <stdint.h>
#include <stdlib.h>
#include "logging.h"
#include "messaging.h"
#include "message_queue.h"
#include "empi.h"

int message_queue_init(message_queue *q)
{
   if (q == NULL) {
      return -1;
   }

   q->head = NULL;
   q->tail = NULL;

   return 0;
}

int message_enqueue(message_queue *q, generic_message *m)
{
   message_info *info = malloc(sizeof(message_info));

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

generic_message *message_dequeue(message_queue *q)
{
   message_info *current;
   generic_message *message;

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

int message_queue_empty(message_queue *q)
{
   return (q->head == NULL);
}

int data_message_queue_init(data_message_queue *q)
{
   return message_queue_init(&(q->queue));
}

int data_message_enqueue(data_message_queue *q, data_message *m)
{
   return message_enqueue(&(q->queue), (generic_message *)m);
}

data_message *data_message_dequeue(data_message_queue *q)
{
   return (data_message *) message_dequeue(&(q->queue));
}

int data_message_queue_empty(data_message_queue *q)
{
   return message_queue_empty(&(q->queue));
}

/*
static int match_message(data_message *m, int comm, int source, int tag)
{
   int result = ((comm == m->comm) &&
                 (source == EMPI_ANY_SOURCE || source == m->source) &&
                 (tag == EMPI_ANY_TAG || tag == m->tag));

   DEBUG(5, "MATCH_MESSAGE: (comm=%d source=%d [any=%d] tag=%d [any=%d]) == (m.comm=%d m.source=%d m.tag=%d) => %d",
        comm, source, EMPI_ANY_SOURCE, tag, EMPI_ANY_TAG,
        m->comm, m->source, m->tag, result);

   return result;
}
*/

data_message *data_message_dequeue_matching(data_message_queue *q, int comm, int source, int tag)
{
   message_info *curr, *prev;
   data_message *result;

   DEBUG(4, "FIND_PENDING_MESSAGE: Checking for pending messages in comm=%d from source=%d tag=%d", comm, source, tag);

   if (q->queue.head == NULL) {
      DEBUG(4, "FIND_PENDING_MESSAGE: No pending messages");
      return NULL;
   }

   curr = q->queue.head;
   prev = NULL;

   while (curr != NULL) {

      if (match_message((data_message *)curr->message, comm, source, tag)) {
          if (curr == q->queue.head) {
              // delete head. check if list empty
              if (q->queue.head == q->queue.tail) {
                 q->queue.head = q->queue.tail = NULL;
              } else {
                 q->queue.head = q->queue.head->next;
              }
          } else if (curr == q->queue.tail) {
              // delete tail. set tail to prev
              q->queue.tail = prev;
              q->queue.tail->next = NULL;
          } else {
              // delete middle.
              prev->next = curr->next;
          }

          curr->next = NULL;
          result = (data_message *) curr->message;
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



