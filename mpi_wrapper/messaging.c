#include "flags.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>

#include "empi.h"
#include "error.h"
#include "datatype.h"
#include "status.h"
#include "types.h"
#include "messaging.h"

#include "communicator.h"
#include "group.h"
#include "request.h"
#include "wa_sockets.h"
#include "logging.h"

#define BYTE_ORDER_UNDEFINED 0
#define BYTE_ORDER_HOST      1
#define BYTE_ORDER_NETWORK   2

static message_buffer *receive_message(int blocking, int *error);

static void free_message(message_buffer *m)
{
   if (m == NULL) {
      return;
   }

   if (m->data_buffer != NULL) {
      free(m->data_buffer);
   }

   free(m);
}

static message_buffer *probe_wa(communicator *c, int source, int tag, int blocking, int *error)
{
   message_buffer *m;

   DEBUG(4, "Probing socket for incoming messages from source=%d tag=%d blocking=%d", source, tag, blocking);

   do {
      m = receive_message(blocking, error);

      if (m == NULL) {
         DEBUG(5, "No message received");
         return NULL;
      }

      if (*error != EMPI_SUCCESS) {
         ERROR(1, "Failed to receive message error=%d", error);
         return NULL;
      }

      DEBUG(5, "Message received from source=%d tag=%d count=%d bytes=%d", m->header.source, m->header.tag, m->header.count, m->header.bytes);

      if (match_message(m, c->handle, source, tag)) {
         // we have a match!
         DEBUG(5, "Match! Returning message");
         return m;
      } else {
         DEBUG(5, "No match. Storing message");
         store_message(m);
      }

   } while (blocking);

   DEBUG(4, "No message received");

   return NULL;
}

static int unpack_message(void *buf, int count, datatype *t, communicator *c,
                           message_buffer *m, EMPI_Status *status)
{
  int error = 0;
  int position = 0;

/*
  This check is wrong! The sendnig and receiving count may differ if the sending and receiveing types differ!
 
  if (m->header.count != count) {
      ERROR(1, "Message size mismatch! (expected %d got %d)",
                  count, m->header.count);

      if (m->header.count < count) {
         // we have received less data
         count = m->header.count;
      }
      // else we have received too much data!
   }
*/

   error = TRANSLATE_ERROR(MPI_Unpack(m->data_buffer, m->header.bytes, &position, buf,
                        count, t->type, c->comm));

   if (error == EMPI_SUCCESS && status != NULL) {
      status->MPI_SOURCE = m->header.source;
      status->MPI_TAG = m->header.tag;
      status->MPI_ERROR = error;
      status->count = count;
      status->cancelled = 0;
   }

   free_message(m);
   return error;
}

static message_buffer *create_message_buffer()
{
   message_buffer *m = malloc(sizeof(message_buffer));

   if (m == NULL) {
       ERROR(1, "Failed to allocate message header\n");
       return NULL;
   }

   m->byte_order = BYTE_ORDER_UNDEFINED;
   m->data_buffer_size = 0;
   m->data_buffer = NULL;

   return m;
}

static int create_data_buffer(message_buffer *m, int max_size)
{
   m->data_buffer_size = max_size;
   m->data_buffer = malloc(max_size);

   if (m->data_buffer == NULL) {
       ERROR(1, "Failed to allocate message buffer of size %d", max_size);
       return CONNECT_ERROR_ALLOCATE;
   }

   return CONNECT_OK;
}

static message_buffer *create_message(int max_size)
{
   message_buffer *m = create_message_buffer();

   if (m == NULL) {
       return NULL;
   }

   if (create_data_buffer(m, max_size) != CONNECT_OK) {
       free_message(m);
       return NULL;
   }

   return m;
}

static void write_message_header(message_buffer *m, int opcode, int comm, int source, int dest, int tag, int count, int bytes)
{
   m->byte_order = BYTE_ORDER_HOST;

   m->header.opcode = opcode;
   m->header.comm = comm;
   m->header.source = source;
   m->header.dest = dest;
   m->header.tag = tag;
   m->header.count = count;
   m->header.bytes = bytes;
}

static int ensure_byte_order(message_buffer *m, int order)
{
   if (m->byte_order == order) {
      return CONNECT_OK;
   }

   if (order == BYTE_ORDER_HOST && m->byte_order == BYTE_ORDER_NETWORK) {
      m->byte_order = BYTE_ORDER_HOST;

      m->header.opcode = ntohl(m->header.opcode);
      m->header.comm = ntohl(m->header.comm);
      m->header.source = ntohl(m->header.source);
      m->header.dest = ntohl(m->header.dest);
      m->header.tag = ntohl(m->header.tag);
      m->header.count = ntohl(m->header.count);
      m->header.bytes = ntohl(m->header.bytes);

      return CONNECT_OK;
   }

   if (order == BYTE_ORDER_NETWORK && m->byte_order == BYTE_ORDER_HOST) {
      m->byte_order = BYTE_ORDER_NETWORK;

      m->header.opcode = htonl(m->header.opcode);
      m->header.comm = htonl(m->header.comm);
      m->header.source = htonl(m->header.source);
      m->header.dest = htonl(m->header.dest);
      m->header.tag = htonl(m->header.tag);
      m->header.count = htonl(m->header.count);
      m->header.bytes = htonl(m->header.bytes);

      return CONNECT_OK;
   }

   ERROR(1, "Failed to convert byte order %d to %d", order, m->byte_order);
   return CONNECT_ERROR_BYTE_ORDER;
}

static int send_message(message_buffer *m)
{
   int error;
   int size = m->header.bytes;

   if (m->byte_order == BYTE_ORDER_NETWORK) {
      size = ntohl(size);
   } else {
      ensure_byte_order(m, BYTE_ORDER_NETWORK);
   }

   error = wa_sendfully((unsigned char *) &(m->header), MSG_HEADER_SIZE);

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to send message header!");
      return EMPI_ERR_INTERN;
   }

   error = wa_sendfully(m->data_buffer, size);

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to send message body!");
      return EMPI_ERR_INTERN;
   }

   return EMPI_SUCCESS;
}

/*
static int receive_opcode(int* opcode, int *error, int blocking)
{
   DEBUG(1, "Receiving from socket (blocking=%d)", blocking);

   int result = wa_wait_for_data(blocking);

   DEBUG(1, "Result of receive: result=%d error=%d", result, *error);

   if (result == -1) {
      *error = MPI_ERR_INTERN;
      return 0;
   }

   if (result == 0) {
      if (blocking == 1) {
         *error = MPI_ERR_INTERN;
      } else {
         *error = MPI_SUCCESS;
      }
      return 0;
   }

   result = wa_receivefully((unsigned char *) opcode, 4);

   if (result != CONNECT_OK) {
      ERROR(1, "Failed to receive message opcode!");
      *error = MPI_ERR_INTERN;
      return 0;
   }

   *opcode = ntohl(*opcode);
   return 1;
}
*/

static int receive_opcode(int* opcode, int *error, int blocking)
{
   DEBUG(1, "Receiving from socket (blocking=%d)", blocking);

   int result = wa_wait_for_opcode(blocking, opcode);

   DEBUG(1, "Result of receive: result=%d error=%d", result, *error);

   if (result == -1) {
      *error = EMPI_ERR_INTERN;
      return 0;
   }

   if (result == 0) {
      if (blocking == 1) {
         *error = EMPI_ERR_INTERN;
      } else {
         *error = EMPI_SUCCESS;
      }
      return 0;
   }

   *opcode = ntohl(*opcode);
   return 1;
}

static int receive_message_header(message_buffer *m)
{
   // Shift the buffer 4 bytes to skip the opcode.
   unsigned char *tmp = (unsigned char *) &(m->header);
   tmp += 4;

   int error = wa_receivefully(tmp, MSG_HEADER_SIZE-4);

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to receive message header!");
      return error;
   }

   m->header.opcode = htonl(OPCODE_DATA);
   m->byte_order = BYTE_ORDER_NETWORK;
   return CONNECT_OK;
}

static int receive_message_data(message_buffer *m)
{
   int error = wa_receivefully(m->data_buffer, m->header.bytes);

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to receive message data!");
      return error;
   }

   return CONNECT_OK;
}

static message_buffer *receive_data_message(int *error)
{
   message_buffer *m = create_message_buffer();

   if (m == NULL) {
      *error = EMPI_ERR_INTERN;
      return NULL;
   }

   if (receive_message_header(m) != CONNECT_OK) {
      free_message(m);
      *error = EMPI_ERR_INTERN;
      return NULL;
   }

   ensure_byte_order(m, BYTE_ORDER_HOST);

   if (create_data_buffer(m, m->header.bytes) != CONNECT_OK) {
      free_message(m);
      *error = EMPI_ERR_INTERN;
      return NULL;
   }

   if (receive_message_data(m) != CONNECT_OK) {
      free_message(m);
      *error = EMPI_ERR_INTERN;
      return NULL;
   }

   *error = EMPI_SUCCESS;
   return m;
}

static message_buffer *receive_message(int blocking, int *error)
{
   int opcode;

   DEBUG(4, "Receiving message from socket (blocking=%d)", blocking);

   int result = receive_opcode(&opcode, error, blocking);

   DEBUG(4, "Result of receive: result=%d error=%d", result, *error);

   if (result == 0) {
      // Note: error will be set correctly if blocking was true
      return NULL;
   }

   if (opcode == OPCODE_DATA || opcode == OPCODE_COLLECTIVE_BCAST) {
      return receive_data_message(error);
   }

   ERROR(1, "Unexpected message opcode (RM) %d", opcode);
   *error = EMPI_ERR_INTERN;
   return NULL;
}

/*
static int pack_size(int count, datatype *d, int *bytes)
{
   if (count < 0) {
      ERROR(1, "Unexpected message opcode (RM) %d", opcode);
   }

FIXME!
}
*/


static int do_send(int opcode, void* buf, int count, datatype *t, int dest, int tag, communicator* c)
{
   // We have already checked the various parameters, so all we have to so is send the lot!
   int bytes, error;

   // FIXME ??
   error = TRANSLATE_ERROR(MPI_Pack_size(count, t->type, c->comm, &bytes));

   if (error != EMPI_SUCCESS) {
      return error;
   }

   message_buffer *m = create_message(bytes);

   if (m == NULL) {
      return EMPI_ERR_INTERN;
   }

   bytes = 0;

   // FIXME ??
   error = TRANSLATE_ERROR(MPI_Pack(buf, count, t->type, m->data_buffer, m->data_buffer_size, &bytes, c->comm));

   write_message_header(m, opcode, c->handle, c->global_rank, dest, tag, count, bytes);

   if (error == EMPI_SUCCESS) {
      error = send_message(m);
   }

   free_message(m);
   return error;
}

int messaging_send(void* buf, int count, datatype *t, int dest, int tag, communicator* c)
{
   return do_send(OPCODE_DATA, buf, count, t, dest, tag, c);
}

int messaging_bcast(void* buf, int count, datatype *t, int root, communicator* c)
{
   return do_send(OPCODE_COLLECTIVE_BCAST, buf, count, t, root, BCAST_TAG, c);
}

int messaging_bcast_receive(void *buf, int count, datatype *t, int root, communicator* c)
{
   return messaging_receive(buf, count, t, root, BCAST_TAG, EMPI_STATUS_IGNORE, c);
}

/*
int messaging_allreduce(void* buf, int count, MPI_Datatype datatype, communicator* c)
{
   return do_send(OPCODE_COLLECTIVE_ALLREDUCE, buf, count, datatype, 0, ALLREDUCE_TAG, c);
}

int messaging_allreduce_receive(void *buf, int count, MPI_Datatype datatype, communicator* c)
{
   return messaging_receive(buf, count, datatype, 0, ALLREDUCE_TAG, MPI_STATUS_IGNORE, c);
}
*/

/*
int messaging_probe_receive_request(request *r)
{
   int error = MPI_SUCCESS;
   message_buffer *m;

   // Check if the request was already completed.
   if (r->flags & REQUEST_FLAG_COMPLETED) {
      return 1;
   }

   // We now need to see if we can fullfill the waiting
   // receive request. There are 3 ways to do this:
   //
   // 1. Check to see if we have any pending messages which match.
   // 2. Check if the local MPI can fullfill the request.
   // 3. Check if the wide area connection can fullfill the request.
   //
   // NOTE: step 2 is only performed if the request refers to a MIXED
   //       operation, i.e., a receive from an communicator that contains
   //       both local and remote (wa) processes.
   // If the request is fullfilled, TRUE is returned, FALSE otherwise.

   m = find_pending_message(r->c, r->source_or_dest, r->tag);

   if (m == NULL) {
      m = probe_wa(r->c, r->source_or_dest, r->tag, 0, &error);
   }

   if (m != NULL) {
      r->flags |= REQUEST_FLAG_COMPLETED;
      r->message = m;
//      r->error = unpack_message(r->buf, r->count, r->type, r->c->comm, m, status);
      return 1;

   } else if (error != MPI_SUCCESS) {
      // Something happened in probe_wa!
      r->flags |= REQUEST_FLAG_COMPLETED;
      r->error = error;
      return 1;
   }

   return 0;
}

void messaging_complete_receive_request(request *r, MPI_Status *status)
{
   // Check if we still need to receive a message.
   if (!(r->flags & REQUEST_FLAG_COMPLETED)) {
      r->error = messaging_receive(r->buf, r->count, r->type, r->source_or_dest, r->tag, status, r->comm);
   } else {
      r->error = unpack_message(r->buf, r->count, r->type, r->c->comm, r->message, status);
   }
}
*/

int messaging_receive(void *buf, int count, datatype *t,
              int source, int tag, EMPI_Status *status, communicator* c)
{
   int error = EMPI_SUCCESS;

   message_buffer *m = find_pending_message(c, source, tag);

   while (m == NULL && error == EMPI_SUCCESS) {
      m = probe_wa(c, source, tag, 1, &error);
   }

   if (error == EMPI_SUCCESS) {
      error = unpack_message(buf, count, t, c, m, status);
   }

   return error;
}

int messaging_probe_receive(request *r, int blocking)
{
   if ((r->flags & REQUEST_FLAG_COMPLETED)) {
      return EMPI_SUCCESS;
   }

   r->message = find_pending_message(r->c, r->source_or_dest, r->tag);

   if (r->message != NULL) {
      r->flags |= REQUEST_FLAG_COMPLETED;
      return EMPI_SUCCESS;
   }

   r->error = EMPI_SUCCESS;

   do {
      r->message = probe_wa(r->c, r->source_or_dest, r->tag, blocking, &(r->error));
   } while (blocking && r->message == NULL && r->error == EMPI_SUCCESS);

   if (r->message != NULL || r->error != EMPI_SUCCESS) {
      r->flags |= REQUEST_FLAG_COMPLETED;
   }

   return EMPI_SUCCESS;
}

int messaging_finalize_receive(request *r, EMPI_Status *status)
{
   if (!(r->flags & REQUEST_FLAG_COMPLETED)) {
      return EMPI_ERR_INTERN;
   }

   if (r->flags & REQUEST_FLAG_UNPACKED) {
      return EMPI_SUCCESS;
   }

   r->error = unpack_message(r->buf, r->count, r->type, r->c, r->message, status);
   r->flags |= REQUEST_FLAG_UNPACKED;

   return r->error;
}

static int queue_pending_messages(int *next_opcode)
{
   int opcode;
   int error = 0;
   int result = 0;

   message_buffer *m;

   while (result == 0) {

      DEBUG(1, "Start blocking receive for opcode");

      result = receive_opcode(&opcode, &error, 1);

      if (result == 0) {
         ERROR(1, "Failed to receive opcode! (%d)", error);
         return error;
      }

      DEBUG(1, "Result of receive result=%d error=%d", result, error);

      if (opcode == OPCODE_DATA || opcode == OPCODE_COLLECTIVE_BCAST) {
         // There is a message blocking the stream!
         m = receive_data_message(&error);

         if (m == NULL) {
            ERROR(1, "Failed to receive message!");
            return error;
         }

         DEBUG(1, "Message received from src: %d opcode: %d tag: %d", m->header.source, opcode, m->header.tag);

         store_message(m);
         result = 0;
      } else {
         DEBUG(1, "Received non-message opcode %d", opcode);
         *next_opcode = opcode;
         result = 1;
      }
   }

   return EMPI_SUCCESS;
}

static int *alloc_and_receive_int_array(int len)
{
   int i, error;
   int *tmp = malloc(len * sizeof(int));

   if (tmp == NULL) {
      ERROR(1, "Failed to allocate int buffer!");
      return NULL;
   }

   error = wa_receivefully((unsigned char *)tmp, len * sizeof(int));

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to receive int buffer!");
      free(tmp);
      return NULL;
   }

   for (i=0;i<len;i++) {
      tmp[i] = ntohl(tmp[i]);
   }

   return tmp;
}

int messaging_receive_comm_reply(comm_reply *reply)
{
   // Since operations on communicators are collective operations, we can
   // assume here that the reply has not be received yet. There is a chance,
   // however, that other messages are on the stream before our reply message.
   int opcode;

   int error = queue_pending_messages(&opcode);

   if (error != EMPI_SUCCESS) {
      return error;
   }

   if (opcode != OPCODE_COMM_REPLY) {
      ERROR(1, "unexpected message opcode (RC) %d", opcode);
      return EMPI_ERR_INTERN;
   }

   error = wa_receivefully((unsigned char *) reply, COMM_REPLY_SIZE);

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to receive comm reply!");
      return EMPI_ERR_INTERN;
   }

   reply->comm = ntohl(reply->comm);
   reply->src = ntohl(reply->src);
   reply->newComm = ntohl(reply->newComm);
   reply->rank = ntohl(reply->rank);
   reply->size = ntohl(reply->size);
   reply->color = ntohl(reply->color);
   reply->key = ntohl(reply->key);
   reply->cluster_count = ntohl(reply->cluster_count);
   reply->flags = ntohl(reply->flags);

   DEBUG(1, "*Received comm reply (comm=%d src=%d newComm=%d rank=%d size=%d color=%d key=%d cluster_count=%d flag=%d)", 
           reply->comm, reply->src, reply->newComm, reply->rank, reply->size, reply->color, reply->key, reply->cluster_count, reply->flags);

   reply->coordinators = alloc_and_receive_int_array(reply->cluster_count);

   if (reply->coordinators == NULL) {
      ERROR(1, "Failed to allocate or receive coordinators");
      return EMPI_ERR_INTERN;
   }

   reply->cluster_sizes = alloc_and_receive_int_array(reply->cluster_count);

   if (reply->cluster_sizes == NULL) {
      ERROR(1, "Failed to allocate or receive cluster sizes");
      return EMPI_ERR_INTERN;
   }

   reply->cluster_ranks = alloc_and_receive_int_array(reply->cluster_count);

   if (reply->cluster_ranks == NULL) {
      ERROR(1, "Failed to allocate or receive cluster ranks");
      return EMPI_ERR_INTERN;
   }

   if (reply->size > 0) {
      reply->members = (uint32_t *) alloc_and_receive_int_array(reply->size);

      if (reply->members == NULL) {
         ERROR(1, "Failed to allocate or receive communicator members");
         return EMPI_ERR_INTERN;
      }

      reply->member_cluster_index = (uint32_t *) alloc_and_receive_int_array(reply->size);

      if (reply->member_cluster_index == NULL) {
         ERROR(1, "Failed to allocate or receive communicator member cluster index");
         return EMPI_ERR_INTERN;
      }

      reply->local_ranks = (uint32_t *) alloc_and_receive_int_array(reply->size);

      if (reply->local_ranks == NULL) {
         ERROR(1, "Failed to allocate or receive communicator member local ranks");
         return EMPI_ERR_INTERN;
      }
   }

   return EMPI_SUCCESS;
}

int messaging_send_comm_request(communicator* c, int color, int key)
{
   comm_request req;

   req.opcode = htonl(OPCODE_COMM);
   req.comm = htonl(c->handle);
   req.src = htonl(c->global_rank);
   req.color = htonl(color);
   req.key = htonl(key);

   int error = wa_sendfully((unsigned char *) &req, COMM_REQUEST_SIZE);

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to send comm request!");
      return EMPI_ERR_INTERN;
   }

   return EMPI_SUCCESS;
}

int messaging_send_group_request(communicator* c, group *g)
{
   int i;
   group_request req;

   req.opcode = htonl(OPCODE_GROUP);
   req.comm = htonl(c->handle);
   req.src = htonl(c->global_rank);
   req.size = htonl(g->size);

   int error = wa_sendfully((unsigned char *) &req, GROUP_REQUEST_SIZE);

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to send comm request!");
      return EMPI_ERR_INTERN;
   }

   for (i=0;i<g->size;i++) {
      g->members[i] = htonl(g->members[i]);
   }

   error = wa_sendfully((unsigned char *) g->members, g->size * sizeof(uint32_t));

   for (i=0;i<g->size;i++) {
      g->members[i] = ntohl(g->members[i]);
   }

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to send comm request!");
      return EMPI_ERR_INTERN;
   }

   return EMPI_SUCCESS;
}

int messaging_receive_group_reply(group_reply *reply)
{
   // Since operations on communicators are collective operations, we can
   // assume here that the reply has not be received yet. There is a chance,
   // however, that other messages are on the stream before our reply message.
   int opcode;

   int error = queue_pending_messages(&opcode);

   if (error != EMPI_SUCCESS) {
      return error;
   }

   if (opcode != OPCODE_GROUP_REPLY) {
      ERROR(1, "unexpected message opcode (RG) %d", opcode);
      return EMPI_ERR_INTERN;
   }

DEBUG(1, "*Receiving group reply %lu %lu %lu %lu", sizeof(group_reply), sizeof(unsigned char *), sizeof(group_reply)-sizeof(unsigned char *), GROUP_REPLY_SIZE);

   error = wa_receivefully((unsigned char *) reply, GROUP_REPLY_SIZE);

   if (error != CONNECT_OK) {
      ERROR(1, "INTERNAL ERROR: Failed to receive group reply!");
      return EMPI_ERR_INTERN;
   }

   reply->comm = ntohl(reply->comm);
   reply->src = ntohl(reply->src);
   reply->newComm = ntohl(reply->newComm);
   reply->rank = ntohl(reply->rank);
   reply->size = ntohl(reply->size);
   reply->type = ntohl(reply->type);
   reply->cluster_count = ntohl(reply->cluster_count);
   reply->flags = ntohl(reply->flags);

DEBUG(1, "*Received group reply (comm=%d src=%d newComm=%d rank=%d size=%d type=%d cluster_count=%d flags=%d)", reply->comm, reply->src, reply->newComm, reply->rank, reply->size, reply->type, reply->cluster_count, reply->flags);

   if (reply->type == GROUP_TYPE_ACTIVE) {

      reply->coordinators = alloc_and_receive_int_array(reply->cluster_count);

      if (reply->coordinators == NULL) {
         ERROR(1, "Failed to allocate or receive coordinators");
         return EMPI_ERR_INTERN;
      }

      reply->cluster_sizes = alloc_and_receive_int_array(reply->cluster_count);

      if (reply->cluster_sizes == NULL) {
         ERROR(1, "Failed to allocate or receive cluster sizes");
         return EMPI_ERR_INTERN;
      }

      reply->cluster_ranks = alloc_and_receive_int_array(reply->cluster_count);

      if (reply->cluster_ranks == NULL) {
         ERROR(1, "Failed to allocate or receive cluster ranks");
         return EMPI_ERR_INTERN;
      }

      if (reply->size > 0) {
         reply->members = (uint32_t *) alloc_and_receive_int_array(reply->size);

         if (reply->members == NULL) {
            ERROR(1, "Failed to allocate or receive communicator members");
            return EMPI_ERR_INTERN;
         }

         reply->member_cluster_index = (uint32_t *) alloc_and_receive_int_array(reply->size);

         if (reply->member_cluster_index == NULL) {
            ERROR(1, "Failed to allocate or receive communicator member cluster index");
            return EMPI_ERR_INTERN;
         }

         reply->local_ranks = (uint32_t *) alloc_and_receive_int_array(reply->size);

         if (reply->local_ranks == NULL) {
            ERROR(1, "Failed to allocate or receive communicator member local ranks");
            return EMPI_ERR_INTERN;
         }
      }
   }

   return EMPI_SUCCESS;
}

int messaging_send_dup_request(communicator* c)
{
   dup_request req;

   req.opcode = htonl(OPCODE_DUP);
   req.comm = htonl(c->handle);
   req.src = htonl(c->global_rank);

   int error = wa_sendfully((unsigned char *) &req, DUP_REQUEST_SIZE);

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to send dup request!");
      return EMPI_ERR_INTERN;
   }

   return EMPI_SUCCESS;
}

int messaging_send_terminate_request(communicator* c)
{
   terminate_request req;

   req.opcode = htonl(OPCODE_TERMINATE);
   req.comm = htonl(c->handle);
   req.src = htonl(c->global_rank);

   int error = wa_sendfully((unsigned char *) &req, TERMINATE_REQUEST_SIZE);

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to send terminate request!");
      return EMPI_ERR_INTERN;
   }

   return EMPI_SUCCESS;
}


int messaging_receive_dup_reply(dup_reply *reply)
{
   // Since operations on communicators are collective operations, we can
   // assume here that the reply has not be received yet. There is a chance,
   // however, that other messages are on the stream before our reply message.
   int opcode;

   int error = queue_pending_messages(&opcode);

   if (error != EMPI_SUCCESS) {
      return error;
   }

   if (opcode != OPCODE_DUP_REPLY) {
      ERROR(1, "INTERNAL ERROR: unexpected message opcode (RD) %d", opcode);
      return EMPI_ERR_INTERN;
   }

   error = wa_receivefully((unsigned char *) reply, DUP_REPLY_SIZE);

   if (error != CONNECT_OK) {
      ERROR(1, "INTERNAL ERROR: Failed to receive group reply!");
      return EMPI_ERR_INTERN;
   }

   reply->comm = ntohl(reply->comm);
   reply->src = ntohl(reply->src);
   reply->newComm = ntohl(reply->newComm);

DEBUG(1, "*Received dup reply (comm=%d src=%d newComm=%d)", reply->comm, reply->src, reply->newComm);

   return EMPI_SUCCESS;
}

