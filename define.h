/*
 * define.h
 *
 *  Created on: 2011-11-6
 *      Author: poseidon
 */


#ifndef DEFINE_H_
#define DEFINE_H_

#include "types.h"

#define __DEBUG__
#ifdef __DEBUG__
#define DEBUG(format,...) printf(__FILE__", %04dline: "format"\n", __LINE__, ##__VA_ARGS__)
#else
#define DEBUG(format,...)
#endif

struct _control_data_header {
	uint16		len;			/*数据包的长度，它只表示数据的长度（字节），不包括报文头部*/
	uint8		encrpyt;		/*报文的加密方式*/
	uint8		mo;				/*定义用户状态*/
	uint8		ttl;			/*定义报文生存周期*/
	uint8		extent;		/*用于以后的扩展*/
	uint16		ki;				/*定义控制命令*/
};

typedef struct _control_data_header	ctl_hdr;
//sizeof(ctl_hdr) = 8字节

struct replay_packet {
	ctl_hdr head;
	int16 data;
};
//size0f(replay_packet) = 10字节

struct user {
	int8 username[128];
	int8 password[128];
};

struct register_struct {
		ctl_hdr head;
		struct user user_struct;
	};


struct common_packet {
	ctl_hdr head;
	char data[256];
};

struct everyone {
	uint8 hour;
	uint8 minute;
	uint8 fangjian;
	uint8 set;
};

struct alarm_all_data {
	struct everyone data[5];
};

struct alarm_data {
	uint8 id;
	uint8 set;
	uint8 hour;
	uint8 minute;
	uint8 fangjianid;
	uint8 yinyue;
	uint8 sun;
	uint8 mon;
	uint8 tue;
	uint8 wed;
	uint8 thu;
	uint8 fri;
	uint8 sat;
	int8 tishi[243];
};


//加密类型
#define ENCRPYT_NO			0
#define ENCRPYT_MD5			1
#define ENCRPYT_AES			2

//用户状态
#define MO_SERVER				0
#define MO_CLIENT				1
#define MO_BROADCAST		2
#define MO_SET						3
#define MO_GET					4
#define MO_REPLAY				5

//控制类型
#define KI_REPLAY					100
#define KI_REGISTER				101
#define KI_LOGIN					102
#define KI_LOGOFF				103
#define KI_LOGCAT				104
#define KI_UPLOAD				105
#define KI_DOWNLOAD		106
#define KI_RESET					107
#define KI_SHUTDOWN		108
#define KI_LIST_FILE				109
#define KI_DEL_FILE				110
#define KI_SECURE					111
#define KI_LIGHT					112
#define KI_DEVICE					113
#define KI_BACK_MUSIC		114
#define KI_VIDEO_SHARE		115
#define KI_TIME_SERVICE		116
#define KI_IP_SET					117
#define KI_NOTIFICATION	118
#define KI_CURTAINS			119
#define KI_TELEVISION			120
#define KI_AIRCONDITIONER		121
#define KI_SOUND					122
#define KI_ALARM					123
#define KI_HEARTBEAT			124
#define KI_REPLAYHEART		125
#define KI_CURLIGHT			126
#define KI_GETALARM			127


//回复报文信息
#define RPL_OK									0x0001
#define RPL_CANCEL							0x0002
#define RPL_REGISTER_SUCCESS		0x0003
#define RPL_REGISTER_FALLED		0x0004
#define RPL_LOGIN_SUCCESS			0x0005
#define RPL_LOGIN_FALLED			0x0006
#define RPL_LOGOFF						0x0007
#define RPL_ALARM_OPEN				0x0008
#define RPL_ALARM_CLOSE				0x0009
#define RPL_NO_COMMON_REPLAY	0xFFFF



#endif /* DEFINE_H_ */




















