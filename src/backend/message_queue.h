#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_

#include <stdint.h>
#include "messaging.h"

#define MESSAGE_QUEUE_INITIAL_SIZE 256

//typedef struct s_message_info message_info;

//struct s_message_info {
//   message_info *next; // Next message info in queue
//  generic_message *message;
///}

// A generic message queue.
typedef struct s_message_queue {
   int size;
   int used;
   int head;
   int tail;
   generic_message **entries;
} message_queue;

// A data message queue.
//typedef struct s_data_message_queue {
//   message_queue queue;
//} data_message_queue;

message_queue *message_queue_create();
int message_enqueue(message_queue *q, generic_message *m);
generic_message *message_dequeue(message_queue *q);
int message_queue_empty(message_queue *q);

//data_message_queue *data_message_queue_create();
//int data_message_enqueue(data_message_queue *q, data_message *m);
//int data_message_queue_empty(data_message_queue *q);
//data_message *data_message_dequeue(data_message_queue *q);
//data_message *data_message_dequeue_matching(data_message_queue *q, int comm, int source, int tag);

#endif // _MESSAGE_QUEUE_H_
