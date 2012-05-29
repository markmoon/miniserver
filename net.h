/*
 * net.h
 *
 *  Created on: 2011-11-6
 *      Author: poseidon
 */


#ifndef NET_H_
#define NET_H_

#include <sys/types.h>

#include "types.h"
#include "define.h"

#define MAX_CONNECTED 		10
#define MAX_CONNECTED_NO		20

int tcp_init_server(int port);
int tcp_init_client(char ip[16], unsigned short port);
void tcp_close(int fd);
ssize_t readn(int fd, void * vptr, size_t n);
ssize_t writen(int fd, const void * vptr, size_t n);


#endif /* NET_H_ */
