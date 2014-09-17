#ifndef _DATA_MESSAGE_QUEUE_H_
#define _DATA_MESSAGE_QUEUE_H_

#include <stdint.h>
#include "messaging.h"

// Nodes for the linked list of data messages
typedef struct s_data_message_info data_message_info;

struct s_data_message_info {
   data_message_info *next; // Next message info in queue
   data_message *message;
};

// A generic data message queue.
typedef struct s_data_message_queue {
   data_message_info *head;
   data_message_info *tail;
} data_message_queue;

data_message_queue *data_message_queue_create();
int data_message_enqueue(data_message_queue *q, data_message *m);
int data_message_queue_empty(data_message_queue *q);

data_message *data_message_dequeue(data_message_queue *q);
data_message *data_message_dequeue_matching(data_message_queue *q, int comm, int source, int tag);
bool data_message_contains_matching(data_message_queue *q, int comm, int source, int tag);

#endif // _DATA_MESSAGE_QUEUE_H_
