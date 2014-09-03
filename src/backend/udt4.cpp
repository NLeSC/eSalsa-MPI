#include "udt4.h"
#include <udt.h>

// This is a C interface to the UDT4 library. Note that this file contains C++ code!

UDT::SOCKOPT translate_optname(UDT4_Option optname)
{
	switch (optname) {
	case UDT4_MSS:        return UDT_MSS;
	case UDT4_SNDSYN:     return UDT_SNDSYN;
	case UDT4_RCVSYN:     return UDT_RCVSYN;
	case UDT4_CC:         return UDT_CC;
	case UDT4_FC:	      return UDT_FC;
	case UDT4_SNDBUF:     return UDT_SNDBUF;
	case UDT4_RCVBUF:     return UDT_RCVBUF;
	case UDT4_LINGER:     return UDT_LINGER;
	case UDT4_UDP_SNDBUF: return UDP_SNDBUF;
	case UDT4_UDP_RCVBUF: return UDP_RCVBUF;
	case UDT4_MAXMSG:     return UDT_MAXMSG;
	case UDT4_MSGTTL:     return UDT_MSGTTL;
	case UDT4_RENDEZVOUS: return UDT_RENDEZVOUS;
	case UDT4_SNDTIMEO:   return UDT_SNDTIMEO;
	case UDT4_RCVTIMEO:   return UDT_RCVTIMEO;
	case UDT4_REUSEADDR:  return UDT_REUSEADDR;
	case UDT4_MAXBW:	  return UDT_MAXBW;
	case UDT4_STATE:	  return UDT_STATE;
	case UDT4_EVENT:	  return UDT_EVENT;
	case UDT4_SNDDATA:	  return UDT_SNDDATA;
	case UDT4_RCVDATA:    return UDT_RCVDATA;
	}

	// Should be unreachable!
	return UDT_MSS;
}

extern "C" int udt4_startup()
{
	return UDT::startup();
}

extern "C" int udt4_cleanup()
{
	return UDT::cleanup();
}

extern "C" int udt4_create_socket(int af, int type, int protocol)
{
	return UDT::socket(af, type, protocol);
}

extern "C" int udt4_setsockopt(int udt4_socket, int level, UDT4_Option optname, const void* optval, int optlen)
{
	return UDT::setsockopt(udt4_socket, level, translate_optname(optname), optval, optlen);
}

extern "C" int udt4_getsockopt(int udt4_socket, int level, UDT4_Option optname, void* optval, int* optlen)
{
	return UDT::getsockopt(udt4_socket, level, translate_optname(optname), optval, optlen);
}

extern "C" int udt4_connect(int udt4_socket, const struct sockaddr* name, int namelen)
{
	return UDT::connect(udt4_socket, name, namelen);
}

extern "C" int udt4_bind(int udt4_socket, const struct sockaddr* name, int namelen)
{
	return UDT::bind(udt4_socket, name, namelen);
}

extern "C" int udt4_listen(int udt4_socket, int backlog)
{
	return UDT::listen(udt4_socket, backlog);
}

extern "C" int udt4_accept(int udt4_socket, struct sockaddr* addr, int* addrlen)
{
	return UDT::accept(udt4_socket, addr, addrlen);
}

extern "C" int udt4_send(int udt4_socket, const char* buf, int len, int flags)
{
	return UDT::send(udt4_socket, buf, len, flags);
}

extern "C" int udt4_recv(int udt4_socket, char* buf, int len, int flags)
{
	// Does this do anything ?
	int rcv_size;
	int var_size = sizeof(int);
	UDT::getsockopt(udt4_socket, 0, UDT_RCVDATA, &rcv_size, &var_size);

	return UDT::recv(udt4_socket, buf, len, flags);
}

extern "C" int udt4_close(int udt4_socket)
{
	return UDT::close(udt4_socket);
}

extern "C" int udt4_perfmon(int udt4_socket, UDT4_Statistics* perf, bool clear)
{
	UDT::TRACEINFO tmp;
	int error;

	error = UDT::perfmon(udt4_socket, &tmp, clear);

	if (error != 0) {
		return error;
	}

	// global measurements
	perf->msTimeStamp = tmp.msTimeStamp;
	perf->pktSentTotal = tmp.pktSentTotal;             // total number of sent data packets, including retransmissions
	perf->pktRecvTotal = tmp.pktRecvTotal;             // total number of received packets
	perf->pktSndLossTotal = tmp.pktSndLossTotal;       // total number of lost packets (sender side)
	perf->pktRcvLossTotal = tmp.pktRcvLossTotal;       // total number of lost packets (receiver side)
	perf->pktRetransTotal = tmp.pktRetransTotal;       // total number of retransmitted packets
	perf->pktSentACKTotal = tmp.pktSentACKTotal;       // total number of sent ACK packets
	perf->pktRecvACKTotal = tmp.pktRecvACKTotal;       // total number of received ACK packets
	perf->pktSentNAKTotal = tmp.pktSentNAKTotal;       // total number of sent NAK packets
	perf->pktRecvNAKTotal = tmp.pktRecvNAKTotal;       // total number of received NAK packets
	perf->usSndDurationTotal = tmp.usSndDurationTotal; // total time duration when UDT is sending data (idle time exclusive)

	// local measurements
	perf->pktSent = tmp.pktSent;                       // number of sent data packets, including retransmissions
	perf->pktRecv = tmp.pktRecv;                       // number of received packets
	perf->pktSndLoss = tmp.pktSndLoss;                 // number of lost packets (sender side)
	perf->pktRcvLoss = tmp.pktRcvLoss;                 // number of lost packets (receiver side)
	perf->pktRetrans = tmp.pktRetrans;                 // number of retransmitted packets
	perf->pktSentACK = tmp.pktSentACK;                 // number of sent ACK packets
	perf->pktRecvACK = tmp.pktRecvACK;                 // number of received ACK packets
	perf->pktSentNAK = tmp.pktSentNAK;                 // number of sent NAK packets
	perf->pktRecvNAK = tmp.pktRecvNAK;                 // number of received NAK packets
	perf->mbpsSendRate = tmp.mbpsSendRate;             // sending rate in Mb/s
	perf->mbpsRecvRate = tmp.mbpsRecvRate;             // receiving rate in Mb/s
	perf->usSndDuration = tmp.usSndDuration;		   // busy sending time (i.e., idle time exclusive)

	// instant measurements
	perf->usPktSndPeriod = tmp.usPktSndPeriod;         // packet sending period, in microseconds
	perf->pktFlowWindow = tmp.pktFlowWindow;           // flow window size, in number of packets
	perf->pktCongestionWindow = tmp.pktCongestionWindow; // congestion window size, in number of packets
	perf->pktFlightSize = tmp.pktFlightSize;           // number of packets on flight
	perf->msRTT = tmp.msRTT;                           // RTT, in milliseconds
	perf->mbpsBandwidth = tmp.mbpsBandwidth;           // estimated bandwidth, in Mb/s
	perf->byteAvailSndBuf = tmp.byteAvailSndBuf;       // available UDT sender buffer size
	perf->byteAvailRcvBuf = tmp.byteAvailRcvBuf;       // available UDT receiver buffer size

	return 0;
}
