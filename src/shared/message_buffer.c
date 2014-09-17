#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "message_buffer.h"
#include "settings.h"
#include "logging.h"

message_buffer *message_buffer_create(size_t size)
{
	message_buffer *b = malloc(sizeof(message_buffer));

	if (b == NULL) {
		ERROR(1, "Failed to allocate message buffer!");
		return NULL;
	}

	b->data = malloc(size);

	if (b->data == NULL) {
		free(b);
		ERROR(1, "Failed to allocate message buffer!");
		return NULL;
	}

	b->size = size;
	b->start = 0;
	b->end = 0;
	b->type = MESSAGE_BUFFER_NORMAL;

	return b;
}

int message_buffer_expand(message_buffer *buffer, size_t new_size)
{
	size_t used;
	unsigned char *tmp;

	if (new_size < buffer->size) {
		return 1;
	}

	used = buffer->end-buffer->start;

	tmp = malloc(new_size);

	if (tmp == NULL) {
		return 1;
	}

	memcpy(tmp, buffer->data+buffer->start, used);

	free(buffer->data);

	buffer->start = 0;
	buffer->end = used;
	buffer->size = new_size;
	buffer->data = tmp;
	return 0;
}


message_buffer *message_buffer_wrap(unsigned char *buffer, size_t size)
{
	message_buffer *b = malloc(sizeof(message_buffer));

	if (b == NULL) {
		ERROR(1, "Failed to allocate message buffer!");
		return NULL;
	}

	b->data = buffer;
	b->size = size;
	b->start = 0;
	b->end = 0;
	b->type = MESSAGE_BUFFER_WRAPPED;
	return b;
}

void message_buffer_destroy(message_buffer *buffer)
{
	if (buffer == NULL) {
		return;
	}

	if (buffer->type == MESSAGE_BUFFER_NORMAL) {
		free(buffer->data);
	}

	free(buffer);
}

size_t message_buffer_read(message_buffer *buffer, unsigned char *dest, size_t len)
{
	size_t avail = buffer->end - buffer->start;

	if (avail > len) {
		avail = len;
	}

	if (avail == 0) {
		return 0;
	}

	memcpy(dest, buffer->data+buffer->start, avail);

	buffer->start += avail;

	if (buffer->start == buffer->end) {
		buffer->start = buffer->end = 0;
	}

	return avail;
}

unsigned char *message_buffer_direct_read_access(message_buffer *buffer, size_t length)
{
	unsigned char *tmp;

	if ((buffer->end - buffer->start) < length) {
		return NULL;
	}

	tmp = buffer->data + buffer->start;
	buffer->start += length;

	if (buffer->start == buffer->end) {
		buffer->start = buffer->end = 0;
	}

	return tmp;
}

int message_buffer_skip(message_buffer *buffer, size_t length)
{
	size_t avail;

	avail = buffer->end - buffer->start;

	if (avail <= length) {
		buffer->start = buffer->end = 0;
		return avail;
	}

	buffer->start += length;
	return length;
}

unsigned char *message_buffer_direct_write_access(message_buffer *buffer, size_t length)
{
	unsigned char *tmp;

	if ((buffer->size - buffer->end) < length) {
		return NULL;
	}

	tmp = buffer->data + buffer->end;
	buffer->end += length;

	return tmp;
}


size_t message_buffer_peek(message_buffer *buffer, unsigned char *dest, size_t len)
{
	size_t avail = buffer->end - buffer->start;

	if (avail > len) {
		avail = len;
	}

	if (avail == 0) {
		return 0;
	}

	memcpy(dest, buffer->data+buffer->start, avail);
	return avail;
}

size_t message_buffer_write(message_buffer *buffer, unsigned char *src, size_t len)
{
	size_t space = buffer->size - buffer->end;

	if (space > len) {
		space = len;
	}

	if (space == 0) {
		return 0;
	}

	memcpy(buffer->data + buffer->end, src, space);
	buffer->end += space;
	return space;
}

size_t message_buffer_max_read(message_buffer *buffer)
{
	return (buffer->end - buffer->start);
}

size_t message_buffer_max_write(message_buffer *buffer)
{
	return (buffer->size - buffer->end);
}

size_t message_buffer_size(message_buffer *buffer)
{
	return buffer->size;
}


void message_buffer_compact(message_buffer *buffer)
{
	size_t used = buffer->end - buffer->start;

	if (buffer->start == 0) {
		// Already compacted
		return;
	}

	if (used == 0) {
		// Empty buffer
		buffer->end = buffer->start = 0;
		return;
	}

	// Copy the existing data to the start of the buffer.
	if (used < buffer->start) {
		// No overlapping copy, so memcpy is safe.
		memcpy(buffer->data, buffer->data + buffer->start, used);
	} else {
		// Overlapping copy, so must use memmove.
		memmove(buffer->data, buffer->data + buffer->start, used);
	}

	buffer->start = 0;
	buffer->end = used;
}

/*
size_t message_buffer_receive(int socketfd, message_buffer *buffer, bool blocking)
{
	int error;
	size_t space, bytes_read;

	space = message_buffer_max_write(buffer);

	if (space == 0) {
		return 0;
	}

	error = socket_receive(socketfd, buffer->data, space, blocking, &bytes_read);

	if (error != SOCKET_OK) {
		return -1;
	}

	buffer->end += bytes_read;

	return bytes_read;
}

size_t message_buffer_send(int socketfd, message_buffer *buffer, bool blocking)
{
	int error;
	size_t avail, bytes_sent;

	avail = message_buffer_max_read(buffer);

	if (avail == 0) {
		return 0;
	}

	error = socket_send(socketfd, buffer->data, avail, blocking, &bytes_sent);

	if (error != SOCKET_OK) {
		return -1;
	}

	buffer->start += bytes_sent;

	if (buffer->start == buffer->end) {
		buffer->start = buffer->end = 0;
	}

	return bytes_sent;
}
*/
