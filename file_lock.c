/*
 * file_lock.c
 *
 *  Created on: 2012-2-29
 *      Author: poseidon
 */

#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include "file_lock.h"



int lock_set(int fd, int type) {
	struct flock lock;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	lock.l_type = type;
	if ((fcntl(fd, F_SETLK, &lock)) == 0) { //加锁成功
		return 0;
	}
	return -1;
}
