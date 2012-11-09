//
//  client.c
//  osue
//
//  Created by Martin Prebio on 08.11.12.
//  Copyright (c) 2012 Martin Prebio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include "gamelogic.h"

/* === Macros === */

#ifdef ENDEBUG
#define DEBUG(...) do { fprintf(stderr, __VA_ARGS__); } while(0)
#else
#define DEBUG(...)
#endif

/* === Global Variables === */
 
/* Name of the program */
static const char *progname = "client";

int connfd, round = 0;

extern int errno;

/* === Prototypes === */

int create_connection(const char *, const int);
static void bail_out(int eval, const char *fmt, ...);
void usage(void);


/* === Implementations === */

int main(int argc, const char * argv[])
{
	progname = argv[0];
	
	if (argc != 3) {
		errno = 0;
        fprintf(stderr, "Usage: %s [<server-hostname> | <server-ip>] <server-port>\n", progname);
		return EXIT_FAILURE;
	}

	int port_int = atoi(argv[2]);
	
    if (port_int < 1 || port_int > 65535) {
        bail_out(EXIT_FAILURE, "Use a valid TCP/IP port range (1-65535)");
    }
	
	connfd = create_connection(argv[1], port_int);

	uint8_t read_buffer;
	uint16_t next_tipp;
	
	do {
		round++;
		next_tipp = get_next_tipp();
		send(connfd, &next_tipp, 2, 0);
        DEBUG("Sent 0x%x\n", next_tipp);
		
		recv(connfd, &read_buffer, 1, 0);
        DEBUG("Got byte 0x%x\n", read_buffer);
		save_result(read_buffer);
		
		// Check buffer errors
		errno = 0;
		switch (read_buffer >> 6) {
			case 1:
				fprintf(stderr, "Parity error\n");
				return 2;
			case 2:
				fprintf(stderr, "Game lost\n");
				return 3;
			case 3:
				fprintf(stderr, "Parity error AND game lost, d'oh!\n");
				return 4;
			default:
				break;
		}
		
		// Check if game is won
		if ((read_buffer & 7) == 5 ) {
            printf("Runden: %d\n", round);
			return 0;
		}
		
	} while (1);
	
    return 1; // That should never happen
}


int create_connection(const char *hostname, const int port)
{
	int sock, success = 0;
	char port_string[6]; // Max port = 65xxx\0
	
	sprintf(port_string, "%i", port);
	
	// Get ze addr info
	struct addrinfo *ai = malloc(sizeof(struct addrinfo));
	struct addrinfo hints;
	
	hints.ai_flags = 0;
	hints.ai_family = AF_INET; /* IPv4 only, IPv6: AF_INET6  */
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_addrlen = 0;
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	
	if (getaddrinfo(hostname, port_string, &hints, &ai) < 0) {
		bail_out(EXIT_FAILURE, "getaddrinfo");
	}
	
	struct addrinfo *ai_head = ai;
	
	if (ai == NULL) {
		bail_out(EXIT_FAILURE, "Address infromation is empty");
	}
	
	do {
		sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
		if (sock >= 0) {
			if (connect(sock, ai->ai_addr, ai->ai_addrlen) >= 0){
				success = 1;
				break;
			}
		}
	} while ((ai = ai->ai_next) != NULL);
	
	freeaddrinfo(ai_head);
	
	if (success == 0) {
		bail_out(EXIT_FAILURE, "No adress information lead to a valid socket");
	}
	
	return sock;
}


static void free_resources(void)
{
    /* clean up resources */
    DEBUG("Shutting down client\n");
    if(connfd >= 0) {
        (void) close(connfd);
    }
}

static void bail_out(int eval, const char *fmt, ...)
{
    va_list ap;
	
    (void) fprintf(stderr, "%s: ", progname);
    if (fmt != NULL) {
        va_start(ap, fmt);
        (void) vfprintf(stderr, fmt, ap);
        va_end(ap);
    }
    if (errno != 0) {
        (void) fprintf(stderr, ": %s", strerror(errno));
    }
    (void) fprintf(stderr, "\n");
	
    free_resources();
    exit(eval);
}