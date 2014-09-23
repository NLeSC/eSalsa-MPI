#ifndef _GENERIC_MESSAGE_H_
#define _GENERIC_MESSAGE_H_

#include <stdint.h>

typedef struct {
   uint32_t opcode;  // type of message
   uint32_t src_pid; // source PID
   uint32_t dst_pid; // destination PID
   uint32_t length;  // size of message, including this header.
} message_header;

#define MESSAGE_HEADER_SIZE (4*sizeof(uint32_t))

// This is an ack message, used as a data acknowledgement in flow control.
typedef struct {
   message_header header;
   size_t bytes;
} ack_message;

#define ACK_MESSAGE_SIZE (MESSAGE_HEADER_SIZE + sizeof(size_t))

// This is a generic message, consisting of a header plus some data.
typedef struct {
   message_header header;
   unsigned char payload[];  // message data.
} generic_message;

#endif // _GENERIC_MESSAGE_H_
