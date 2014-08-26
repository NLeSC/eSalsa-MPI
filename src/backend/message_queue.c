#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "logging.h"
#include "messaging.h"
#include "message_queue.h"
#include "empi.h"

message_queue *message_queue_create()
{
   message_queue *tmp = malloc(sizeof(message_queue));

   if (tmp == NULL) {
      return NULL;
   }

   tmp->size = MESSAGE_QUEUE_INITIAL_SIZE;
   tmp->used = 0;
   tmp->head = 0;
   tmp->tail = 0;

   tmp->entries = malloc(MESSAGE_QUEUE_INITIAL_SIZE * sizeof(generic_message *));

   memset((void *) tmp->entries, 0, MESSAGE_QUEUE_INITIAL_SIZE * sizeof(generic_message *));

   return tmp;
}

//message_queueint message_queue_init(message_queue *q)
//{
//   if (q == NULL) {
//      return -1;
//   }
//
//   q->head = NULL;
//   q->tail = NULL;
//
//   return 0;
//}

static int grow(message_queue *q)
{
   generic_message **tmp = realloc(q->entries, q->size*2);

   if (tmp == NULL) {
      return -1;
   }

   q->entries = tmp;
   return 0;
}


static int ensure_space(message_queue *q)
{
   if (q->used == q->size) {
      return grow(q);
   }

   return 0;
}

int message_enqueue(message_queue *q, generic_message *m)
{
   if (ensure_space(q) == -1) {
      return -1;
   }

   q->entries[q->tail] = m;
   q->tail = (q->tail+1) % q->size;
   q->used++;
   return 0;
}

generic_message *message_dequeue(message_queue *q)
{
   generic_message *message;

   if (q->used == 0) {
      return NULL;
   }

   message = q->entries[q->head];

   q->used--;

   if (q->used == 0) {
      q->head = 0;
      q->tail = 0;
   } else {
      q->head = (q->head+1) % q->size;
   }

   return message;
}

int message_queue_empty(message_queue *q)
{
   return (q->used == 0);
}

/*
data_message_queue *data_message_queue_create(data_message_queue *q)
{
   return (data_message_queue *) message_queue_create();
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
*/

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

/*

data_message *data_message_dequeue_matching(data_message_queue *q, int comm, int source, int tag)
{
   int curr, i, next;
   data_message *result;

   DEBUG(4, "FIND_PENDING_MESSAGE: Checking for pending messages in comm=%d from source=%d tag=%d", comm, source, tag);

   if (q->queue.used == 0) {
      DEBUG(4, "FIND_PENDING_MESSAGE: No pending messages");
      return NULL;
   }

   curr = q->queue.head;

   for (i=0;i<q->queue.used;i++) {

      if (match_message((data_message *) q->queue.entries[curr], comm, source, tag)) {

          result = (data_message *) q->queue.entries[curr];

          if (curr == q->queue.head) {

              q->queue.head = (q->queue.head+1) % q->queue.size;
              q->queue.used--;

              if (q->queue.used == 0) {
                 q->queue.head = 0;
                 q->queue.tail = 0;
              }

          } else if (curr == q->queue.tail) {

              // delete tail. set tail to prev
              q->queue.tail = (q->queue.tail + q->queue.size-1) % q->queue.size;
              q->queue.used--;

          } else {
              // delete middle by moving the tail part.
              while (curr != q->queue.tail) {
                 q->queue.entries[curr] = q->queue.entries[(curr+1) % q->queue.size];
                 curr = (curr+1) % q->queue.size;
              }

              q->queue.used--;
              q->queue.tail = (q->queue.tail + q->queue.size-1) % q->queue.size;
          }

          DEBUG(4, "FIND_PENDING_MESSAGE: Found pending message from %d", result->source);

          return result;
      }

      curr = (curr+1) % q->queue.size;
   }

   DEBUG(4, "FIND_PENDING_MESSAGE: No matching messages");

   return NULL;
}
*/


