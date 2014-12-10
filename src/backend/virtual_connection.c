#include <stdbool.h>
#include "messaging.h"
#include "request.h"
#include "request_queue.h"
#include "virtual_connection.h"

// Create a new virtual connection.
virtual_connection *virtual_connection_create(int index, int pid, int sliding_window_size, int max_node_cache_size)
{
	virtual_connection *tmp = malloc(sizeof(virtual_connection));

	if (tmp == NULL) {
		ERROR(1, "Failed to allocate space for virtual connection for destination %d:%d",
				GET_CLUSTER_RANK(pid), GET_PROCESS_RANK(pid));
		return NULL;
	}

	tmp->index = index;
	tmp->peer_pid = pid;

	tmp->send_request_queue = request_queue_create(max_node_cache_size);

	tmp->active_sender = false;
	tmp->force_ack = false;

	tmp->sliding_window_size = sliding_window_size;

	tmp->current_send_request = NULL;

	tmp->send_message = NULL;
	tmp->send_position = 0;
	tmp->send_length = 0;
	tmp->send_opcode = OPCODE_MISC;

	tmp->receive_message = NULL;
	tmp->receive_position = 0;
	tmp->receive_length = 0;
	tmp->receive_opcode = OPCODE_MISC;

	tmp->transmit_sequence = 0;
	tmp->receive_sequence = 0;
	tmp->acknowledged_sequence_sent = 0;

	return tmp;
}

// Free a virtual connection.
void virtual_connection_free(virtual_connection *virtual_connection)
{
	// TODO
}

// Enqueue send request
int virtual_connection_enqueue_send(virtual_connection *virtual_connection, request *req)
{
	if (!request_queue_enqueue(virtual_connection->send_request_queue, req)) {
		ERROR(0, "Failed to enqueue send request!");
		return 1;
	}

	return 0;
}

// Cancel send request
int virtual_connection_cancel_send(virtual_connection *virtual_connection, request *req)
{
	// TODO: implement
}
