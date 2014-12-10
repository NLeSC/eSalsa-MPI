#ifndef _REQUEST_H_
#define _REQUEST_H_

#include "settings.h"

#include "empi.h"
#include "types.h"
#include "communicator.h"
#include "mpi.h"

#define REQUEST_FLAG_ACTIVE     (1 << 0)
#define REQUEST_FLAG_PERSISTENT (1 << 1)
#define REQUEST_FLAG_COMPLETED  (1 << 2)
#define REQUEST_FLAG_LOCAL      (1 << 3)
#define REQUEST_FLAG_SEND       (1 << 4)
#define REQUEST_FLAG_RECEIVE    (1 << 5)
#define REQUEST_FLAG_UNPACKED   (1 << 6)

struct s_request {

     // Status flags (see above)
     int flags;

     // These contain the call parameters and return value
     void *buf;
     datatype *type;
     int count;
     int source_or_dest;
     int tag;
     communicator *c;
     int error;

     int handle;

     // The source of the received message (set when MPI_ANY_SOURCE is used).
     int message_source;

     // The tag of the received message (set when MPI_ANY_TAG is used).
     int message_tag;

     // The count of the received message (set when MPI_ANY_TAG is used).
     int message_count;

     // This is the real MPI_Request
     MPI_Request req;

     // This is the matching message (used in receive operations).
     // data_message *message;

     // The sequence number of this send. Used to reassemble the message at the destination.
     uint32_t sequence_nr;

     // This a temporary buffer for all message fragments.
     void *fragment_buf;

     // Number of fragments received so far.
     int fragment_count;


};

int init_request();

request *create_request(int flags, void *buf, int count, datatype *d, int dest, int tag, communicator *c);
void free_request(request *r);

EMPI_Request request_to_handle(request *r);
request *handle_to_request(EMPI_Request handle);

int request_active(request *r);
int request_local(request *r);
int request_persistent(request *r);
int request_send(request *r);
int request_receive(request *r);
int request_completed(request *r);

// Utility function that allows us to retrieve which communicator a
// request is operation on (used in profiling).
EMPI_Comm request_get_comm(EMPI_Request *r, EMPI_Comm def);

#endif // _REQUEST_H_
