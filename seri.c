/*
 * seri.c
 *
 *  Created on: 2012-5-15
 *      Author: poseidon
 */

#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<errno.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>
#include<stdlib.h>

#include"types.h"
#include"define.h"
#include "seri.h"

int set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop) {
	struct termios newtio, oldtio;
	//Save the old settings
	if (tcgetattr(fd, &oldtio) != 0) {
		perror("Setup Serial");
		return -1;
	}
	bzero(&newtio, sizeof(newtio));
	//激活本地连接和接受使能
	newtio.c_cflag |=CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;

	switch (nBits) {
		case 7:
			newtio.c_cflag |= CS7;
			break;
		case 8:
			newtio.c_cflag |= CS8;
			break;
	}

	switch (nEvent) {
		//奇校验
		case 'O':
			newtio.c_cflag |= PARENB;
			newtio.c_cflag |= PARODD;
			newtio.c_iflag |= (INPCK | ISTRIP);
			break;
		//偶校验
		case 'E':
			newtio.c_iflag |= (INPCK | ISTRIP);
			newtio.c_cflag |= PARENB;
			newtio.c_cflag &= ~PARODD;
			break;
		//无校验
		case 'N':
			newtio.c_cflag &= ~PARENB;
			break;
	}

	switch (nSpeed) {
		case 2400:
			cfsetispeed(&newtio, B2400);
			cfsetospeed(&newtio, B2400);
			break;
		case 4800:
			cfsetispeed(&newtio, B4800);
			cfsetospeed(&newtio, B4800);
			break;
		case 9600:
			cfsetispeed(&newtio, B9600);
			cfsetospeed(&newtio, B9600);
			break;
		case 115200:
			cfsetispeed(&newtio, B115200);
			cfsetospeed(&newtio, B115200);
			break;
		case 460800:
			cfsetispeed(&newtio, B460800);
			cfsetospeed(&newtio, B460800);
			break;
		default:
			cfsetispeed(&newtio, B9600);
			cfsetospeed(&newtio, B9600);
			break;
	}

	if(nStop == 1) {
		newtio.c_cflag &= ~CSTOPB;
	} else if(nStop == 2) {
		newtio.c_cflag |= CSTOPB;
	}
	//等待时间
	newtio.c_cc[VTIME] = 0;
	//最小接受字符
	newtio.c_cc[VMIN] = 0;
	//处理未接受字符
	tcflush(fd, TCIFLUSH);
	//激活配置
	if ((tcsetattr(fd, TCSANOW, &newtio)) != 0) {
		perror("Com set error");
		return -1;
	}
	DEBUG("Set done!\n");
	return 0;
}
int open_port(int fd, int comport) {
	if (comport == 0) {
		fd = open("/dev/tq2440_serial0", O_RDWR | O_NOCTTY | O_NDELAY);
		if (fd == -1) {
			perror("Can't Open Serial Port");
			return -1;
		}
	} else if (comport == 1) {
		fd = open("/dev/tq2440_serial1", O_RDWR | O_NOCTTY | O_NDELAY);
		if (fd == -1) {
			perror("Can't Open Serial Port");
			return -1;
		}
	} else if (comport == 2) {
		fd = open("/dev/tq2440_serial2", O_RDWR | O_NOCTTY | O_NDELAY);
		if (fd == -1) {
			perror("Can't Open Serial Port");
			return -1;
		}
	}
	if(fcntl(fd, F_SETFL, 0)<0)	//恢复串口阻塞
		printf("fcntl failed!\n");
	else
		printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));

	if(isatty(STDIN_FILENO)==0)	//测试是否为终端设备
		printf("standard input is not a terminal device\n");
	else
		printf("isatty success!\n");
	printf("fd-open=%d\n",fd);
	return fd;
}






















