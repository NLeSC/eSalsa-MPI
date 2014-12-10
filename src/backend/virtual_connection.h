#ifndef _VIRTUAL_CONNECTION_H_
#define _VIRTUAL_CONNECTION_H_

#include <stdbool.h>

#include "messaging.h"
#include "request.h"
#include "request_queue.h"

typedef struct {

	//  **************** Information on this virtual connection  ****************

	// The index of this virtual connection in the connections array. Useful for reverse lookup.
	int index;

	// The pid of the peer process this virtual connection leads to. Needed as a target address in message fragments.
	int peer_pid;

	// The queue of send requests
	request_queue *send_request_queue;

	// Is this virtual connection an active sender ?
	bool active_sender;

	// Force an ACK message to be send, even if there is no actual data to send.
	bool force_ack;


	// **************** Message currently being send ****************

	// Current message being send to peer. May be NULL is no send is active.
	unsigned char *send_message;

	// Length of current send message
	size_t send_length;

	// Current position in send data message. Needed for fragmentation.
	size_t send_position;

	// Opcode of current message. Needed in message fragments.
	uint32_t send_opcode;

	// Current request for the send operation.  May be NULL is no send is active.
	request *current_send_request;



	// **************** Message currently being received ****************

	// Current data message being received from peer. May be NULL is no receive is active.
	unsigned char *receive_message;

	// Length of current message that is being received.
	size_t receive_length;

	// Current position in send data message. Needed for fragmentation.
	size_t receive_position;

	// Opcode of current message. Needed in message fragments.
	uint32_t receive_opcode;


	// **************** Sliding window information  ****************

	// The size of the sliding window (in message fragments, NOT bytes).
	int sliding_window_size;

	// Sequence number of next message fragment to transmit.
	uint32_t transmit_sequence;

	// Sequence number of next (!) fragment to be received
	uint32_t receive_sequence;

	// Sequence number of last fragment acknowledgment sent.
	uint32_t acknowledged_sequence_sent;

	// Sequence number of last acknowledgment received.
	uint32_t acknowledged_sequence_received;

} virtual_connection;

// Create a new virtual connection.
virtual_connection *virtual_connection_create(int index, int pid, int max_pending_messages, int max_node_cache_size);

// Free a virtual connection.
void virtual_connection_free(virtual_connection *virtual_connection);

// Enqueue send request
int virtual_connection_enqueue_send(virtual_connection *virtual_connection, request *req);

// Cancel send request
int virtual_connection_cancel_send(virtual_connection *virtual_connection, request *req);

#endif // _VIRTUAL_CONNECTION_H_
