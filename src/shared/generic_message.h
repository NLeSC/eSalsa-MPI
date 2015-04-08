#ifndef _GENERIC_MESSAGE_H_
#define _GENERIC_MESSAGE_H_

#include <stdint.h>

// Magic bytes at start of each message fragment. Spells "Mg" in ASCII ...
#define MAGIC_BYTES (0x674D)

// Macro to filter out first magic byte from generic message header
#define GET_MAGIC0(X) (X & 0xFF)

// Macro to filter out second magic byte from generic message header
#define GET_MAGIC1(X) ((X & 0xFF00) >> 8)

// Macro to filter out opcode from generic message header
#define GET_OPCODE(X) ((X & 0xFF000000) >> 24)

// Macro to filter out flags from generic message header
#define GET_FLAGS(X) ((X & 0x00FF0000) >> 16)

// Macro to filter out flags from generic message header
// #define CHECK_MAGIC(X) ((X & 0xFFFF0000) == (MAGIC_BYTES << 16))

// Macro to set opcode X in existing field Y
// #define SET_OPCODE(X, Y) ((X & 0x000000FF) | (Y & 0xFFFFFF00))

// Macro to set flags X in existing field Y
// #define SET_FLAGS(X, Y) ((X & 0x0000FF00) << 8 | (Y & 0xFFFF00FF))

// Macro to set flags X in existing field Y
#define SET_FLAGS_FIELD(F, O) (((O & 0xFF) << 24) | ((F & 0xFF) << 16) | MAGIC_BYTES)

// #define SET_FLAGS_FIELD(F, O) ((O & 0xFF) | ((F & 0xFF) << 8) | MAGIC_BYTES << 16)

// #define SET_FLAGS_FIELD(F, O) ((MAGIC_BYTES << 16) | ((F & 0x0000FF) << 8) | (O & 0x000000FF))

// Macro to set magic in existing field Y
#define SET_MAGIC(Y) ((MAGIC_BYTES << 16) | (Y & 0x0000FFFF))

// These are the valid opcodes
#define OPCODE_MISC      0
#define OPCODE_DATA      1
#define OPCODE_SERVER    2
#define OPCODE_ACK       3
#define OPCODE_TERMINATE 4

// This is a generic message, consisting of a header plus some data.
typedef struct {
	uint32_t flags;  			// magic number (2 bytes: Mg = 4D67), flags (1 bytes), opcode (1 byte).
	uint32_t src_pid; 			// source PID
	uint32_t dst_pid; 			// destination PID
	uint32_t transmit_seq; 		// sequence number of this message fragment
	uint32_t ack_seq; 			// sequence number to acknowledge
	uint32_t length;  			// size of message, including this header.

	uint32_t padding1;    		// padding
	uint32_t padding2;  		// padding

	unsigned char payload[];  	// message pay load.
} generic_message;

#define GENERIC_MESSAGE_HEADER_SIZE (8 * sizeof(uint32_t))

#endif // _GENERIC_MESSAGE_H_
