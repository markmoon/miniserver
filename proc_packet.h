/*
 * proc_packet.h
 *
 *  Created on: 2011-12-19
 *      Author: poseidon
 */

#ifndef PROC_PACKET_H_
#define PROC_PACKET_H_

#include "types.h"
#include "define.h"

/*
struct packet_proc_replay {
	ctl_hdr head;
	char data[256];
};
*/

uint16 proc_packet(char * buffer, int32 fd);

#endif /* PROC_PACKET_H_ */
