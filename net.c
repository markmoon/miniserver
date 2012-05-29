/*
 * net.c
 *
 *  Created on: 2011-11-6
 *      Author: poseidon
 *
 *      修订：2011.12.22
 *      以前的代码只是实现了对发送数据和接受数据的函数封装，这样能保证在网络不好的
 *      情况下，接收到的数据也是有效的。
 *      新加功能：	增加了客户端的tcp初始化函数：tcp_init_client();
 *      						以下说法待测试，不知Linux的send函数和recv函数会不会自己转换字节序，还是需要手动转换
 *      						如果我用的文件IO函数write和read函数又是怎样的情况需要测试以后才能定。
 *      						增加了可移植性，由于开始的代码只是为了测试，都是在PC上，所以没有
 *      				考虑数据格式，即大小端格式，现在在接收和发送的封装函数里加入网络-本地数据转换
 *      				这样，在PC和ARM上的网络通信就成为可能。
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "types.h"
#include "define.h"
#include "net.h"



int tcp_init_server(int port) {
	int fd;
	struct sockaddr_in addr;
	//int addr_len = sizeof(struct sockaddr_in);
	int optval = 1;
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return -1;
	}
	DEBUG("Listen sock: %d\n", fd);
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		sleep(5);
		if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			perror("bind");
			return -1;
		}
	}
	if (listen(fd, MAX_CONNECTED) < 0) {
		perror("listen");
		return -1;
	}
	return fd;
}

int tcp_init_client(char ip[16], unsigned short port) {
	int fd;
	//char ip[16] = "127.0.0.1";
	struct hostent *host;
	if ((host = gethostbyname(ip)) == NULL) {
		DEBUG("Gethostname error\n");
		 exit(1);
	}
	struct sockaddr_in addr;
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket");
		return -1;
	}
	bzero((void *)&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr = *((struct in_addr *)host->h_addr);

	DEBUG("port:%x\n", port);
	DEBUG("htons(port) :%x\n", htons(port));

	DEBUG("server ip:port: %x : %d \n", addr.sin_addr.s_addr, addr.sin_port);

	if (connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) < 0) {
		perror("Connect");
		return -1;
	}
	return fd;

}


void tcp_close(int fd) {
	close(fd);
}

ssize_t readn(int fd, void * vptr, size_t n) {
	ssize_t nread;
	size_t nleft;
	char *ptr;
	ptr = vptr;
	nleft = n;
	while(nleft > 0) {
		if ((nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR) {
				nread = 0;
			} else {
				return -1;
			}
		} else if (nread == 0) {
			break;
		}
		nleft -= nread;
		ptr += nread;
	}
	/*
	uint16 * convertor = (uint16 *)vptr;
	uint16 temp = 0;
	int32 i = 0;
	for (i = 0; i < n; i ++) {
		temp = ntohs(*convertor);
		*convertor = temp;
		convertor ++;
	}
	*/
	return (n-nleft);

}

ssize_t writen(int fd, const void * vptr, size_t n) {
	size_t nleft;
	ssize_t nwriten;
	const char *ptr;
	ptr = vptr;
	nleft = n;
	while(nleft > 0) {
		if ((nwriten = write(fd, ptr, nleft)) <= 0) {
			if (nwriten < 0 && errno == EINTR) {
				nwriten = 0;
			} else {
				return -1;
			}
		}
		nleft -= nwriten;
		ptr += nwriten;
	}



	return n;
}





















