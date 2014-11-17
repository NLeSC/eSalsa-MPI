#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include "../shared/socket_util.h"

// Total time to send data (in seconds)
#define TOTAL_TIME  (30)

// Min message size used (must be power of two)
#define MIN_MSG_SIZE (32)

// Max message size used (must be power of two)
//#define MAX_MSG_SIZE (1024L*1024L)
#define MAX_MSG_SIZE (8*1024L*1024L)

static int socketfd = -1;

static uint64_t current_time_micros()
{
   uint64_t result;
   struct timeval t;

   gettimeofday(&t,NULL);

   result = (t.tv_sec * 1000000LU) + t.tv_usec;

   return result;
}

int run_send_test(int msgsize)
{
    uint64_t start, end, tpt, time, now, print, data, msg, totaldata, totalmsg;
    int i, j, error;
    unsigned char *buf;

    buf = malloc(msgsize);

    if (buf == NULL) {
       fprintf(stderr, "Failed to allocate buffer of size %d\n", msgsize);
       return 1;
    }

    buf[0] = 0;

    fprintf(stderr, "Will send messages of %d bytes\n", msgsize);

    print = now = start = current_time_micros();

    totalmsg = 0;
    totaldata = 0;
    data = 0;
    msg = 0;

    while (now <= start + TOTAL_TIME*1000000) {

    	error = socket_send(socketfd, buf, msgsize, msgsize, true);

    	if (error != msgsize) {
    		fprintf(stderr, "Test %d/%d failed! (error = %d)\n", i, j, error);
    		free(buf);
    		return 1;
    	}

    	data += msgsize;
    	msg++;

    	now = current_time_micros();

    	if (now > print + 1000000) {

    		// Time in micro seconds
    		time = (now-print);

    		// Total data volume in bits
    		tpt = (8L*data);

    		fprintf(stderr, "Test %d took %ld usec for %ld bytes in %ld messages = %ld Mbit/sec total\n", msgsize, time, data, msg, (tpt/time));

    		print = now;

    		totaldata += data;
    		totalmsg += msg;
    		data = 0;
    		msg = 0;
    	}
    }

    fprintf(stderr, "Done sending -- Barrier\n");

    buf[0] = 1;

	error = socket_send(socketfd, buf, msgsize, msgsize, true);

	if (error != msgsize) {
		fprintf(stderr, "Test %d/%d failed! (error = %d)\n", i, j, error);
		free(buf);
		return 1;
	}

	totaldata += msgsize;
	totalmsg++;

	error = socket_receive(socketfd, buf, 1, 1, true);

	if (error != 1) {
		fprintf(stderr, "Test %d/%d failed! (error = %d)\n", i, j, error);
		free(buf);
		return 1;
	}

	end = current_time_micros();

	// Time in micro seconds
	time = (end-start);

	// Total data volume in bits (as send by all senders)
	tpt = (8L*totaldata);

	fprintf(stderr, "OVERALL %d took %ld usec for %ld bytes in %ld messages = %ld Mbit/sec total\n", msgsize, time, totaldata, totalmsg, (tpt/time));

    free(buf);

    return 0;
}
int run_receive_test(int msgsize)
{
    long total, count;
    int i, j, error;
    unsigned char *buf;

    buf = malloc(msgsize);

    if (buf == NULL) {
       fprintf(stderr, "Failed to allocate buffer of size %d\n", msgsize);
       return 1;
    }

    fprintf(stderr, "I am a receiver\n");

    buf[0] = 0;


    while (buf[0] == 0) {

    	error = socket_receive(socketfd, buf,  msgsize, msgsize, true);

    	if (error != msgsize) {
    		fprintf(stderr, "Test %d/%d failed! (error = %d)\n", i, j, error);
    		free(buf);
    		return 1;
    	}
    }

    fprintf(stderr, "Done receiving -- Barrier\n");

    error = socket_send(socketfd, buf, 1, 1, true);

    if (error != 1) {
    	fprintf(stderr, "Test %d/%d failed! (error = %d)\n", i, j, error);
    	free(buf);
    	return 1;
    }

    free(buf);

    return 0;
}


static int run_server()
{
	int msgsize, error;

	msgsize = MIN_MSG_SIZE;

	while (msgsize <= MAX_MSG_SIZE) {

		error = run_receive_test(msgsize);

		if (error != 0) {
			return error;
		}

		msgsize *= 2;
	}

	return 0;
}

static int run_client()
{
	int msgsize, error;

	msgsize = MIN_MSG_SIZE;

	while (msgsize <= MAX_MSG_SIZE) {

		error = run_send_test(msgsize);

		if (error != 0) {
			return error;
		}

		msgsize *= 2;
	}

	return 0;
}

int main(int argc, char *argv[])
{
    int i, status;
    char *server_name;
    long server_ipv4;
    unsigned short server_port;

    server_name = NULL;
    server_port = 20000;

    i=1;

    while ( i < argc ){
    	if ( strcmp(argv[i],"--server") == 0 ) {
    		if ( i+1 < argc ){
    			server_name = malloc(strlen(argv[i+1])+1);
    			strcpy(server_name, argv[i+1]);
    			i++;
    		} else {
    			fprintf(stderr, "Missing option for --server\n");
    			return 1;
    		}
    	} else if ( strcmp(argv[i],"--server-port") == 0 ) {
    		if ( i+1 < argc ){
    			server_port = (unsigned short) atoi(argv[i+1]);
    			i++;
    		} else {
    			fprintf(stderr, "Missing option for --server-port\n");
    			return 1;
    		}
    	} else {
    		fprintf(stderr, "Unknown option %s\n", argv[i]);
    		return 1;
    	}

    	i++;
    }

    if (server_name != NULL) {
    	// I am the client!!
    	fprintf(stderr, "I am client connecting to server at %s %d\n", server_name, server_port);

    	status = socket_get_ipv4_address(server_name, &server_ipv4);

    	if (status != SOCKET_OK) {
    		fprintf(stderr, "Failed to convert server address to IP!");
    		return 1;
    	}

    	status = socket_connect(server_ipv4, server_port, 0, 0, &socketfd);

    	if (status != SOCKET_OK) {
    		fprintf(stderr, "Failed to connect to server!\n");
    		return 1;
    	}

    	status = run_client();

    } else {
    	// I am the server!

    	status = socket_accept_one(server_port, 0, 0, 0, &socketfd);

    	if (status != SOCKET_OK) {
    		fprintf(stderr, "Failed to connect to client!\n");
    		return 1;
    	}

    	status = run_server();

    }

	close(socketfd);

	if (status != 0) {
		fprintf(stderr, "Test failed!\n");
	}

	return status;
}

