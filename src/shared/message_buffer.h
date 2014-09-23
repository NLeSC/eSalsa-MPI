#ifndef _MESSAGE_BUFFER_H_
#define _MESSAGE_BUFFER_H_

#include <stdbool.h>

#define MESSAGE_BUFFER_NORMAL  0
#define MESSAGE_BUFFER_WRAPPED 1

typedef struct {
	size_t size;
	size_t start;
	size_t end;
	int type;
	unsigned char *data;
} message_buffer;

message_buffer *message_buffer_create(size_t size);
message_buffer *message_buffer_wrap(unsigned char *buffer, size_t size, bool nofree);
void message_buffer_destroy(message_buffer *buffer);
size_t message_buffer_read(message_buffer *buffer, unsigned char *dest, size_t len);
unsigned char *message_buffer_direct_read_access(message_buffer *buffer, size_t length);
int message_buffer_skip(message_buffer *buffer, size_t length);
unsigned char *message_buffer_direct_write_access(message_buffer *buffer, size_t length);
size_t message_buffer_peek(message_buffer *buffer, unsigned char *dest, size_t len);
size_t message_buffer_write(message_buffer *buffer, unsigned char *src, size_t len);
size_t message_buffer_max_read(message_buffer *buffer);
size_t message_buffer_max_write(message_buffer *buffer);
size_t message_buffer_size(message_buffer *buffer);
void message_buffer_compact(message_buffer *buffer);
int message_buffer_expand(message_buffer *buffer, size_t new_size);

// TODO: lots of missing stuff

#endif // _MESSAGE_BUFFER_H_
