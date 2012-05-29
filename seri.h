/*
 * seri.h
 *
 *  Created on: 2012-5-15
 *      Author: poseidon
 */

#ifndef SERI_H_
#define SERI_H_

int open_port(int fd, int comport);
int set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop);

#endif /* SERI_H_ */
