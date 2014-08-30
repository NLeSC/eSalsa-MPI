#ifndef _UDT4_H_
#define _UDT4_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/* This is C interface to the C++ UDT4 library. */

typedef enum
{
   UDT4_MSS,             // the Maximum Transfer Unit
   UDT4_SNDSYN,          // if sending is blocking
   UDT4_RCVSYN,          // if receiving is blocking
   UDT4_CC,              // custom congestion control algorithm
   UDT4_FC,				 // Flight flag size (window size)
   UDT4_SNDBUF,          // maximum buffer in sending queue
   UDT4_RCVBUF,          // UDT receiving buffer size
   UDT4_LINGER,          // waiting for unsent data when closing
   UDT4_UDP_SNDBUF,      // UDP sending buffer size
   UDT4_UDP_RCVBUF,      // UDP receiving buffer size
   UDT4_MAXMSG,          // maximum datagram message size
   UDT4_MSGTTL,          // time-to-live of a datagram message
   UDT4_RENDEZVOUS,      // rendezvous connection mode
   UDT4_SNDTIMEO,        // send() timeout
   UDT4_RCVTIMEO,        // recv() timeout
   UDT4_REUSEADDR,		 // reuse an existing port or create a new one
   UDT4_MAXBW,		     // maximum bandwidth (bytes per second) that the connection can use
   UDT4_STATE,		     // current socket state, see UDTSTATUS, read only
   UDT4_EVENT,		     // current avalable events associated with the socket
   UDT4_SNDDATA,		 // size of data in the sending buffer
   UDT4_RCVDATA		     // size of data available for recv

} UDT4_Option;

typedef struct
{
   // global measurements
   int64_t msTimeStamp;                 // time since the UDT entity is started, in milliseconds
   int64_t pktSentTotal;                // total number of sent data packets, including retransmissions
   int64_t pktRecvTotal;                // total number of received packets
   int pktSndLossTotal;                 // total number of lost packets (sender side)
   int pktRcvLossTotal;                 // total number of lost packets (receiver side)
   int pktRetransTotal;                 // total number of retransmitted packets
   int pktSentACKTotal;                 // total number of sent ACK packets
   int pktRecvACKTotal;                 // total number of received ACK packets
   int pktSentNAKTotal;                 // total number of sent NAK packets
   int pktRecvNAKTotal;                 // total number of received NAK packets
   int64_t usSndDurationTotal;			// total time duration when UDT is sending data (idle time exclusive)

   // local measurements
   int64_t pktSent;                     // number of sent data packets, including retransmissions
   int64_t pktRecv;                     // number of received packets
   int pktSndLoss;                      // number of lost packets (sender side)
   int pktRcvLoss;                      // number of lost packets (receiver side)
   int pktRetrans;                      // number of retransmitted packets
   int pktSentACK;                      // number of sent ACK packets
   int pktRecvACK;                      // number of received ACK packets
   int pktSentNAK;                      // number of sent NAK packets
   int pktRecvNAK;                      // number of received NAK packets
   double mbpsSendRate;                 // sending rate in Mb/s
   double mbpsRecvRate;                 // receiving rate in Mb/s
   int64_t usSndDuration;				// busy sending time (i.e., idle time exclusive)

   // instant measurements
   double usPktSndPeriod;               // packet sending period, in microseconds
   int pktFlowWindow;                   // flow window size, in number of packets
   int pktCongestionWindow;             // congestion window size, in number of packets
   int pktFlightSize;                   // number of packets on flight
   double msRTT;                        // RTT, in milliseconds
   double mbpsBandwidth;                // estimated bandwidth, in Mb/s
   int byteAvailSndBuf;                 // available UDT sender buffer size
   int byteAvailRcvBuf;                 // available UDT receiver buffer size
} UDT4_Statistics;

int udt4_startup();
int udt4_cleanup();
int udt4_create_socket(int af, int type, int protocol);
int udt4_setsockopt(int udt4_socket, int level, UDT4_Option optname, const void* optval, int optlen);
int udt4_getsockopt(int udt4_socket, int level, UDT4_Option optname, void* optval, int* optlen);
int udt4_connect(int udt4_socket, const struct sockaddr* name, int namelen);
int udt4_bind(int udt4_socket, const struct sockaddr* name, int namelen);
int udt4_listen(int udt4_socket, int backlog);
int udt4_accept(int udt4_socket, struct sockaddr* addr, int* addrlen);
int udt4_send(int udt4_socket, const char* buf, int len, int flags);
int udt4_recv(int udt4_socket, char* buf, int len, int flags);
int udt4_close(int udt4_socket);
int udt4_perfmon(int udt4_socket, UDT4_Statistics* perf, bool clear);

#ifdef __cplusplus
}
#endif

#endif // _UDT4_H_
