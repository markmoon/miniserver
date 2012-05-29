/*
 * controls.c
 *
 *  Created on: 2012-2-13
 *      Author: poseidon
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ioctl.h>

#include <sqlite3.h>

#include "types.h"
#include "define.h"
#include "net.h"
#include "proc_packet.h"
#include "seri.h"



/*
 * 全局数据区，因为暂时还没有编写硬件的驱动，现在为了测试，用这些数据来代替硬件
 */
uint8 turns[10] = {0,0,0,0,0,0,0,0,0,0};
uint8 curtains[4] = {0,0,0,0};
uint8 turnone = 0;
uint8 tevenum = 10;



void turnover(uint8 turn, uint8 stas) {
	int fdd;
	int turned = 0;
	int stased = 0;
	if (stas == 0) {
		stased = 1;
	}
	if (turn == 0) {
		turned = 3;
	} else if (turn == 1) {
		turned = 2;
	} else if (turn == 2) {
		turned = 5;
	} else if (turn == 3) {
		turned = 4;
	} else {
		turned = turn;
	}
	fdd = open("/dev/smartturn", 0);
	if (fdd < 0) {
		perror("open device smartturn");
	} else {
		ioctl(fdd, stased, turned);
	}
}

/*
 * 控制电灯，收到的数据：电灯号 存放在“ttl”中
 * 电灯状态存放在“mo”中
 * 最后返回当前灯的状态
 */
uint16 control_turn(char * buffer, int32 fd) {
	uint16 ret_code = 0;
	uint16 temp = 1;
	int i = 0;
	ctl_hdr * request = (ctl_hdr *)buffer;
	uint8 turn = request->ttl;
	uint8 stas = request->mo;
	if (turn == 101) {
		for (i = 0; i < 10; i ++) {
			ret_code += turns[i] * temp;
			temp *= 2;
		}
		DEBUG(" Getstatus ret_code:  %d\n", ret_code);
	} else if (turn == 30) {
		turnone = stas;
		ret_code = (uint16)turnone;
	} else {
		//控制turn
		turnover(turn, stas);
		turns[turn] = stas;
		ret_code = (uint16)(turns[turn]);
	}
	return ret_code;

}

/*
 * 控制窗帘 ， 收到的数据：窗帘ID，存放在“ttl”中；窗帘状态：存放在“mo”中
 * 最后返回当前窗帘的状态
 */
uint16 control_curtain(char * buffer, int32 fd) {
	uint16 ret_code = 0;
	uint16 temp = 1;
	int i = 0;
	ctl_hdr * request = (ctl_hdr *)buffer;
	uint8 curtainid = request->ttl;
	uint8 number = request->mo;
	if (curtainid == 102) {
		for (i = 0; i < 4; i ++) {
			ret_code += curtains[i] * temp;
			temp *= 3;
		}

	} else {
		curtains[curtainid] = number;
		ret_code = curtains[curtainid];
	}
	DEBUG(" Control curtain ret_code:  %d\n", ret_code);
	return ret_code;
}

uint16 control_curlight(char * buffer, int32 fd) {
	uint16 ret_code = 0;
	uint8 num1 = 255;
	uint8 num2 = 255;
	ctl_hdr * request = (ctl_hdr *)buffer;
	uint8 device = request->ttl;
	uint8 number = request->mo;
	if (device == 203) {
		switch(number) {
		//客厅
		case 1:
			num1 = turns[0];
			num2 = curtains[0];
			ret_code = num1 * 10 + num2;
			break;
		//主卧
		case 2:
			num1 = turns[3];
			num2 = curtains[1];
			ret_code = num1 * 10 + num2;
			break;
		//次卧
		case 4:
			num1 = turns[4];
			num2 = curtains[2];
			ret_code = num1 * 10 + num2;
			break;
		//客房
		case 8:
			num1 = turns[5];
			num2 = curtains[3];
			ret_code = num1 * 10 + num2;
			break;
		//餐厅
		case 16:
			num1 = turns[1];
			ret_code = num1 * 10;
			break;
		//洗漱
		case 32:
			num1 = turns[6];
			num2 = turnone;
			ret_code = num1 * 10 + num2;
			break;
		}
	}
	return ret_code;
}


/*
 * 控制电视 ，收到的数据：
 */
uint16 control_television(char * buffer, int32 fd) {
	uint16 ret_code = 0;
	ctl_hdr * request = (ctl_hdr *)buffer;
	uint8 id = request->ttl;
	uint8 mode = request->mo;
	char sendBuf[8];
	uint8 sendMode;
	DEBUG(" Control television id:  %d\tmode:  %d\n", id, mode);
	if (mode == 1) {
		sendMode = 0x80;
	} else if (mode == 0) {
		sendMode = 0x81;
	}
	int sfd;
	int ret;
	//open serial1
	sfd = open_port(sfd, 1);
	ret = set_opt(sfd, 9600, 8, 'N', 1);
	if (ret < 0) {
		perror("set opt");
		return RPL_CANCEL;
	}

	/*
	 * id代表遥控器的按键，mode代表是否是学习模式	1为学习，0为控制
	 */
	/*
	switch (id) {
		//开关
		case 0:
			sprintf(sendBuf, "%c%c%c%c%c%c%c%c",
					0x01, 0x03, 0x06, sendMode, id, (0x03 + 0x06 + sendMode + id), 0x0D, 0x0A);
			break;
		//静音
		case 1:
			sprintf(sendBuf, "%c%c%c%c%c%c%c%c",
					0x01, 0x03, 0x06, sendMode, id, (0x03 + 0x06 + sendMode + id), 0x0D, 0x0A);
			break;
		//节目—
		case 2:
			sprintf(sendBuf, "%c%c%c%c%c%c%c%c",
					0x01, 0x03, 0x06, sendMode, id, (0x03 + 0x06 + sendMode + id), 0x0D, 0x0A);
			break;
		//节目+
		case 3:
			sprintf(sendBuf, "%c%c%c%c%c%c%c%c",
					0x01, 0x03, 0x06, sendMode, id, (0x03 + 0x06 + sendMode + id), 0x0D, 0x0A);
			break;
		//音量—
		case 4:
			sprintf(sendBuf, "%c%c%c%c%c%c%c%c",
					0x01, 0x03, 0x06, sendMode, id, (0x03 + 0x06 + sendMode + id), 0x0D, 0x0A);
			break;
		//音量+
		case 5:
			sprintf(sendBuf, "%c%c%c%c%c%c%c%c",
					0x01, 0x03, 0x06, sendMode, id, (0x03 + 0x06 + sendMode + id), 0x0D, 0x0A);
			break;
		//菜单
		case 6:
			sprintf(sendBuf, "%c%c%c%c%c%c%c%c",
					0x01, 0x03, 0x06, sendMode, id, (0x03 + 0x06 + sendMode + id), 0x0D, 0x0A);
			break;
		//返回
		case 7:
			sprintf(sendBuf, "%c%c%c%c%c%c%c%c",
					0x01, 0x03, 0x06, sendMode, id, (0x03 + 0x06 + sendMode + id), 0x0D, 0x0A);
			break;
		//视频
		case 8:
			sprintf(sendBuf, "%c%c%c%c%c%c%c%c",
					0x01, 0x03, 0x06, sendMode, id, (0x03 + 0x06 + sendMode + id), 0x0D, 0x0A);
			break;
		case 9:
			sprintf(sendBuf, "%c%c%c%c%c%c%c%c",
					0x01, 0x03, 0x06, sendMode, id, (0x03 + 0x06 + sendMode + id), 0x0D, 0x0A);
			break;
		}
		*/
	sprintf(sendBuf, "%c%c%c%c%c%c%c%c",
			0x01, 0x03, 0x06, sendMode, id, (0x03 + 0x06 + sendMode + id), 0x0D, 0x0A);
	write(sfd, sendBuf, sizeof(sendBuf));
	int i = 0;
	for (i = 0; i < 8; i ++) {
		printf("%d  ", sendBuf[i]);
	}
	printf("\n");
	return ret_code;
}


uint16 control_air(char * buffer, int32 fd) {
	uint16 ret_code = 0;
	ctl_hdr * request = (ctl_hdr *)buffer;
	uint8 id = request->ttl;
	uint8 mode = request->mo;
	char sendBuf[8];
	uint8 sendMode;
	if (mode == 1) {
		sendMode = 0x80;
	} else if (mode == 0) {
		sendMode = 0x81;
	}
	int sfd;
	int ret;
	//open serial1
	sfd = open_port(sfd, 1);
	ret = set_opt(sfd, 9600, 8, 'N', 1);
	if (ret < 0) {
		perror("set opt");
		return RPL_CANCEL;
	}
	/*
	 * id代表遥控器的按键，mode代表是否是学习模式	1为学习，0为控制
	 */
	sprintf(sendBuf, "%c%c%c%c%c%c%c%c",
			0x01, 0x03, 0x06, sendMode, (id + tevenum), (0x03 + 0x06 + sendMode + id + tevenum), 0x0D, 0x0A);
	write(sfd, sendBuf, sizeof(sendBuf));
	int i = 0;
	for (i = 0; i < 8; i ++) {
		printf("%d  ", sendBuf[i]);
	}
	printf("\n");

	return ret_code;
}


uint16 control_sound(char * buffer, int32 fd) {
	uint16 ret_code = 0;
	ctl_hdr * request = (ctl_hdr *)buffer;
	uint8 id = request->ttl;
	switch (id) {
	//播放暂停
	case 0:
		ret_code = id + 1000;
		break;
	//下一曲
	case 1:
		ret_code = id + 1000;
		break;
	//上一曲
	case 2:
		ret_code = id + 1000;
		break;
	//清单
	case 3:
		ret_code = id + 1000;
		break;
	//设置
	case 4:
		ret_code = id + 1000;
		break;
	//音量+
	case 5:
		ret_code = id + 1000;
		break;
	//音量-
	case 6:
		ret_code = id + 1000;
		break;
	//停止
	case 7:
		ret_code = id + 1000;
		break;
	//静音
	case 8:
		ret_code = id + 1000;
		break;
	}
	return ret_code;
}

uint16 control_music(char * buffer, int32 fd) {
	uint16 ret_code = 0;

	return ret_code;
}

/*
 * 首先从数据库中读出数据，然后判断alarm_data中小时和分钟是不是为0，
 * 如果为0，判断数据库中该数据是否存在，存在就删除，如果不存在，就提示。
 * 如果小时和分钟有一个不为0，也要判断数据是否存在，存在就修改，不存在就插入
 */

uint16 control_alarm(char * buffer, int32 fd) {
	uint16 ret_code = 0;
	struct common_packet *request = (struct common_packet *)buffer;
	char * temp = request->data;
	//这是接收到的闹钟数据包
	struct alarm_data * data = (struct alarm_data *)temp;
	DEBUG("Set alarm data received:  %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %s\n",
			data->id, data->set, data->hour, data->minute, data->fangjianid, data->yinyue, data->sun, data->mon,
			data->tue, data->wed, data->thu, data->fri, data->sat, data->tishi);

	sqlite3 *db = NULL;
	int rc;
	char * Errormsg;
	int row;
	int col;
	char ** result;
	rc = sqlite3_open("/opt/smarthome/smarthome.db", &db);
	if (rc) {
		DEBUG("can not open: %s\n ", sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	} else {
		DEBUG("Open database successfuly!\n");
		char sql[300] = "";
		/*
		strcpy(sql, "create table alarm_data("
				"ID integer primary key, "
				"set integer,"
				"hour integer,"
				"minute integer,"
				"roomid integer,"
				"yinyue integer,"
				"sun integer,"
				"mon integer,"
				"tue integer,"
				"wed integer,"
				"thu integer,"
				"fri integer,"
				"sat integer,"
				"tishi varchar(40))");
		*/
		sqlite3_exec(db, sql, 0, 0, &Errormsg);
		//读出数据库表中所有数据
		strcpy(sql, "select * from alarm_data");
		sqlite3_get_table(db, sql, &result, &row, &col, &Errormsg);
		//此临时变量用来指示数据库中是否存在此记录
		int te = 0;
		if (row == 0) {
			DEBUG("Alarm_data is emety\n");
		} else {
			int i = 1;
			for (i = 1; i < row + 1; i ++) {
				//说明数据库中存在此数据
				if (data->id == atoi(result[i * col])) {
					te = 1;
					break;
				}
			}
		}
		//先删除闹钟
		if (te == 1) {
			sprintf(sql, "delete from alarm_data where ID=%d", data->id);
			sqlite3_exec(db, sql, 0, 0, &Errormsg);
			sprintf(sql, "insert into alarm_data values(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s')",
					data->id, data->set, data->hour, data->minute, data->fangjianid, data->yinyue, data->sun, data->mon,
					data->tue, data->wed, data->thu, data->fri, data->sat, data->tishi);
			//插入当前闹钟信息到数据库
			sqlite3_exec(db, sql, 0, 0, &Errormsg);
		} else {
			sprintf(sql, "insert into alarm_data values(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s')",
					data->id, data->set, data->hour, data->minute, data->fangjianid, data->yinyue, data->sun, data->mon,
					data->tue, data->wed, data->thu, data->fri, data->sat, data->tishi);
			//插入当前闹钟信息到数据库
			DEBUG("SQL is :%s\n", sql);
			sqlite3_exec(db, sql, 0, 0, &Errormsg);
		}
		if (data->set == 0) {
			ret_code = RPL_ALARM_CLOSE;
		} else if (data->set == 1) {
			ret_code = RPL_ALARM_OPEN;
		}
	}
	sqlite3_close(db);
	return ret_code;
}

uint16 control_getalarm(char * buffer, int32 fd) {
	uint16 ret_code = 0;
	ctl_hdr * request = (ctl_hdr *)buffer;
	struct common_packet replay;
	struct alarm_all_data alldata;
	struct alarm_data data;
	replay.head.len = sizeof(replay.data);
	replay.head.encrpyt = ENCRPYT_NO;
	replay.head.mo = MO_CLIENT;

	replay.head.ki = KI_REPLAY;
	uint8 device = request->ttl;
	uint8 number = request->mo;
	if (device == 104) {
		sqlite3 *db = NULL;
		int rc;
		char * Errormsg;
		int row;
		int col;
		char ** result;
		rc = sqlite3_open("/opt/smarthome/smarthome.db", &db);
		if (rc) {
			DEBUG("can not open: %s\n ", sqlite3_errmsg(db));
			sqlite3_close(db);
			return 1;
		} else {
			DEBUG("Open database successfuly!\n");
			char sql[300] = "";
			//读出数据库表中所有数据
			strcpy(sql, "select * from alarm_data");
			sqlite3_get_table(db, sql, &result, &row, &col, &Errormsg);
			if (row == 0) {
				DEBUG("Alarm_data is emety\n");
			} else {
				if (number == 100) {
					int i = 1;
					for (i = 1; i < row + 1; i ++) {
						alldata.data[i - 1].fangjian = atoi(result[i * col + 4]);
						alldata.data[i - 1].hour = atoi(result[i * col + 2]);
						alldata.data[i - 1].minute = atoi(result[i * col + 3]);
						alldata.data[i - 1].set = atoi(result[i * col + 1]);
					}
					memset(replay.data, 0, 256);
					memcpy(replay.data, &alldata, 256);
					writen(fd, (void *)&replay, sizeof(replay));
				} else {
					int i = 1;
					for (i = 1; i < row + 1; i ++) {
						//说明数据库中存在此数据
						if (number == atoi(result[i * col])) {
							data.id = number;
							data.set = atoi(result[i * col + 1]);
							data.hour = atoi(result[i * col + 2]);
							data.minute = atoi(result[i * col + 3]);
							data.fangjianid = atoi(result[i * col + 4]);
							data.yinyue = atoi(result[i * col + 5]);
							data.sun = atoi(result[i * col + 6]);
							data.mon = atoi(result[i * col + 7]);
							data.tue = atoi(result[i * col + 8]);
							data.wed = atoi(result[i * col + 9]);
							data.thu = atoi(result[i * col + 10]);
							data.fri = atoi(result[i * col + 11]);
							data.sat = atoi(result[i * col + 12]);
							memcpy(data.tishi, result[i * col + 13], 244);
							memset(replay.data, 0, 256);
							memcpy(replay.data, &data, 256);
							writen(fd, (void *)&replay, sizeof(replay));
							break;
						}
					}
				}
			}
		}
		sqlite3_close(db);
	}
	return ret_code;
}





















