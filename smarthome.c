/*
 * smarthome.c
 *
 *  Created on: 2011-11-6
 *      Author: poseidon
*/


#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>

#include "types.h"
#include "define.h"
#include "net.h"
#include "user_examine.h"
#include "proc_packet.h"
#include "alarm_server.h"

void loop_process(int cfd) {

	DEBUG(" Child=======>  In child!\n");
	int acceptfd;
	int recved;
	fd_set readfds;
	int delay = 300;
	struct timeval timeout;
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	struct sockaddr_in client_addr;
	socklen_t sock_len = sizeof(struct sockaddr);
	char buffer[512];
	ctl_hdr *hdr;
	uint16 ret_code = 0;

	//服务器对web服务器的回复报文
	struct replay_packet replay;
	replay.head.len = 2;
	replay.head.encrpyt = ENCRPYT_NO;
	replay.head.mo = MO_SERVER;
	replay.head.ttl = 0;
	replay.head.ki = KI_REPLAY;

	while (1) {
		DEBUG(" Child=======>  Waiting for connect! In child thread!\n");
		FD_ZERO(&readfds);
		FD_SET(cfd, &readfds);
		timeout.tv_sec = delay;
		timeout.tv_usec = 0;
		int tt;
		switch(tt = select(cfd + 1, &readfds, NULL, NULL, &timeout)) {
		case -1:
			//error
			continue;
			break;
		default:
			if (FD_ISSET(cfd, &readfds)) {
				acceptfd = accept(cfd,(struct sockaddr *) &client_addr, &sock_len);
				if (acceptfd <=0)  {
					DEBUG(" Child=======>  accept failed!\n");
					exit(-1);
				}
				recved = recv(acceptfd, buffer, sizeof(buffer), 0);
				DEBUG(" Child=======>  Listen socket:  %d\tAccept socket:  %d\n", cfd, acceptfd);
				struct common_packet * common = (struct common_packet *)buffer;

				DEBUG(" Child=======>  Received KI:  %d\n", common->head.ki);
				DEBUG(" Child=======>  Received data length:  %d\n", recved);

				if (recved > 0) {
					hdr = (ctl_hdr *)buffer;
					//如果是登录请求
					if (hdr->ki == KI_LOGIN) {
						//判断用户名和密码是否正确
						struct register_struct * user_struct = (struct register_struct *)buffer;
						if (user_check(&(user_struct->user_struct)) == 0) {
							//登录成功
							replay.data = RPL_LOGIN_SUCCESS;
							replay.head.extent = 111;
							writen(acceptfd, (void *)&replay, sizeof(replay));
							DEBUG(" Child=======>  Login successful!\n");
							tcp_close(acceptfd);
						} else {
							replay.data = RPL_LOGIN_FALLED;
							writen(acceptfd, (void *)&replay, sizeof(replay));
							DEBUG(" Child=======>  Login failed!\n");
							tcp_close(acceptfd);
						}
					} else if (hdr->ki == KI_REGISTER) {
						//用户注册
						struct register_struct * user_struct = (struct register_struct *)buffer;
						if (user_register(&(user_struct->user_struct)) == 0) {
							//注册成功
							replay.data = RPL_REGISTER_SUCCESS;
							writen(acceptfd, (void *)&replay, sizeof(replay));
							DEBUG("Register User Successful\n");
							tcp_close(acceptfd);
						} else {
							replay.data = RPL_REGISTER_FALLED;
							writen(acceptfd, (void *)&replay, sizeof(replay));
							DEBUG("Register User Failed\n");
							tcp_close(acceptfd);
						}
						//如果是发送的数据包，判断extent值，看是否已登录成功
					} else {
						hdr = (ctl_hdr *)buffer;
						DEBUG(" Child=======>  extent is : %d\tttl is : %d\n", hdr->extent, hdr->ttl);
						if ((hdr->extent) == 111) {
							ret_code = proc_packet(buffer, acceptfd);
							if (ret_code == RPL_NO_COMMON_REPLAY) {

							} else {
								replay.data = ret_code;
								writen(acceptfd, (void *)&replay, sizeof(replay));
							}
							tcp_close(acceptfd);
						}
					}
				}
			}
			break;
		}
		tcp_close(acceptfd);
		DEBUG(" Child=======>  tt is %d\n", tt);
	}
}



/*
 * 主函数
 */
int main() {
	pid_t pid;
	int fd;
	pid = fork();
	if (pid < 0) {
		perror("fork failed!");
		exit(1);
	}
	if (pid > 0) {
		//子进程，创建一个alarm服务器
		alarm_server();
	} else {
		DEBUG(" Child=======>  This is the child!\n");
		fd = tcp_init_server(8887);
		loop_process(fd);
	}
	return 0;
}

























