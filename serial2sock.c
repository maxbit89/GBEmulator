/*
 * serial2sock.c
 *
 *  Created on: 22.05.2014
 *      Author: maxbit
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#include <stdio.h>
#include <SDL/SDL_timer.h>
#include <SDL/SDL_thread.h>

#include "serial2sock.h"

#include "memory.h"
#include "core.h"

static int sockfd = -1;
static SDL_Thread *tLink = NULL;

static int linkThread(void *arg);

void serial_listen(int port) {
	if (sockfd < 1) {
		int listenfd = -1;
		struct sockaddr_in serv_addr;
		int addrlen = sizeof(struct sockaddr);
		bzero(&serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(port);
		printf("Create Socket\n");
		if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			printf("Can't create listening socket.\n");
			return;
		}
		printf("Bind Socket to port.\n");
		if (bind(listenfd, (struct sockaddr *) &serv_addr, addrlen) < 0) {
			printf("Can't bind listening socket.\n");
			close(listenfd);
			listenfd = -1;
			return;
		}
		printf("Setup Listener sock\n");
		if (listen(listenfd, 3) < 0) {
			printf("Can't set listening mode to listener socket.\n");
			close(listenfd);
			listenfd = -1;
			return;
		}
		printf("Wait for socket connection on port: %d\n", port);
		if ((sockfd = accept(listenfd, (struct sockaddr *) &serv_addr,
				(socklen_t*) &addrlen)) < 0) {
			printf("Error Accepting Connection. %d\n", errno);
			close(sockfd);
			sockfd = -1;
		}
		close(listenfd);

		if (tLink != NULL) {
			; //TODO make tLink quit
		}
		tLink = SDL_CreateThread(linkThread, (void *) NULL);
	}
}

void serial_connect(char* hostname, int port) {
	if (sockfd < 0) {
		struct sockaddr_in serv_addr;
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		if (inet_pton(AF_INET, hostname, &serv_addr.sin_addr) <= 0) {
			printf("Invalid hostname: %s\n", hostname);
			return;
		}
		serv_addr.sin_port = htons(port);
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))
				< 0) {
			printf("Can't connect to Host:%s on Port: %d\n", hostname, port);
			close(sockfd);
			sockfd = -1;
			return;
		}

		if (tLink != NULL) {
			; //TODO make tLink quit
		}
		tLink = SDL_CreateThread(linkThread, (void *) NULL);
	}
}

static int linkThread(void *arg) {
	while (1) {
		Byte sbRemote;
		if (sockfd < 0) {
			printf("Invalid Socket!\n");
			return -1;
		}
		int n = read(sockfd, &sbRemote, sizeof(sbRemote));
		if (n == 0) {
			//Disconnected.
			printf("Disconnecte!\n");
			close(sockfd);
			sockfd = -1;
			return 0;
		} else if (n > 0) {
			Byte sc = readb(HWREG_SC);
			if ((sc & 0x80)) {
				printf("[SC]: %02x\n", sc);
				if (sc & 0x01) {
					//sending in progress
					//Receiving Reply from Send
					write_io(HWREG_SB, sbRemote);
					printf("[send] reply: 0x%02x\n", sbRemote);
					write_io(HWREG_SC, sc & (~0x80));
					raise_int(INT_SERIAL);
				} else {
					//Receive:
					Byte sb = readb(HWREG_SB);
					printf("[recv] : 0x%02x\n", sbRemote);
					write_io(HWREG_SB, sbRemote);
					printf("[recv] reply: 0x%02x\n", sb);
					usleep(200);
					write(sockfd, &sb, 1);
					write_io(HWREG_SC, sc & (~0x80));
					raise_int(INT_SERIAL);
				}
			}
		} else {
			printf("Error Receiving: %d\n", errno);
		}
		usleep(200);
	}
	return 0;
}

void serial_tx(Byte sb, Byte sc) {
	if ((sc & 0x80) && (sc & 0x01)) {
		//TX Data:
		if (sockfd < 0) {
			//No GameBoy Connected
			printf("Not Connected!\n");
			write_io(HWREG_SB, 0xff);
			write_io(HWREG_SC, sc & (~0x80));
			raise_int(INT_SERIAL);
		} else {
			printf("SC: %02x\n", sc);
			printf("[send] : %02x\n", sb);
			write(sockfd, &sb, 1);
			//usleep(200);
		}
	}
}
