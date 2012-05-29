/*
 * proc_packet.c
 *
 *  Created on: 2011-12-19
 *      Author: poseidon
 */

#include <stdio.h>
#include <string.h>

#include "types.h"
#include "define.h"
#include "net.h"
#include "proc_packet.h"
#include "controls.h"

/*
 * 处理用户发来的数据包
 */
uint16 proc_packet(char * buffer, int32 fd) {
	uint16 ret_code = 0;
	ctl_hdr *hdr;
	hdr = (ctl_hdr *)buffer;
	DEBUG("hdr->ki is %d\n", hdr->ki);

	switch(hdr->ki) {
	case KI_DEL_FILE:
		ret_code = RPL_OK;
		break;
	case KI_DEVICE:
		ret_code = RPL_OK;
		break;
	case KI_DOWNLOAD:
		ret_code = RPL_OK;
		break;
	case KI_IP_SET:
		ret_code = RPL_OK;
		break;
		//请求对灯光进行控制
	case KI_LIGHT:
		ret_code = control_turn(buffer, fd);
		break;
		//请求对窗帘进行控制
	case KI_CURTAINS:
		ret_code = control_curtain(buffer, fd);
		break;
		//请求对电视进行控制
	case KI_TELEVISION:
		ret_code = control_television(buffer, fd);
		break;
		//请求对空调进行控制
	case KI_AIRCONDITIONER:
		ret_code = control_air(buffer, fd);
		break;
		//请求音响进行控制
	case KI_SOUND:
		ret_code = control_sound(buffer, fd);
		break;
		//请求对闹钟进行控制
	case KI_ALARM:
		ret_code = control_alarm(buffer, fd);
		break;
	case KI_GETALARM:
		ret_code = control_getalarm(buffer, fd);
		break;
		//请求对背景音乐进行控制
	case KI_BACK_MUSIC:
		ret_code = control_music(buffer, fd);
		break;
	case KI_CURLIGHT:
		ret_code = control_curlight(buffer,fd);
		break;
	case KI_LIST_FILE:
		ret_code = RPL_OK;
		break;
	case KI_LOGCAT:
		ret_code = RPL_OK;
		break;
	case KI_NOTIFICATION:
		ret_code = RPL_OK;
		break;
	case KI_RESET:
		ret_code = RPL_OK;
		break;
	case KI_SECURE:
		ret_code = RPL_OK;
		break;
	case KI_SHUTDOWN:
		ret_code = RPL_OK;
		break;
	case KI_TIME_SERVICE:
		ret_code = RPL_OK;
		break;
	case KI_UPLOAD:
		ret_code = RPL_OK;
		break;
	case KI_VIDEO_SHARE:
		ret_code = RPL_OK;
		break;
	default:
		ret_code = RPL_OK;
		break;
	}

	return ret_code;

}




























