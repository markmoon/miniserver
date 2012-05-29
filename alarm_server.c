/*
 * alarm_server.c
 *
 *  Created on: 2012-3-5
 *      Author: poseidon
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <string.h>

#include <sqlite3.h>

#include "types.h"
#include "define.h"
#include "user_examine.h"
#include "net.h"
#include "alarm_server.h"


//在相应的房间播放音乐，并显示提示语句。
void alarm_cycle(int roomid, int sound, char * notice) {
	DEBUG("Room id is: %d\tMusic id is:  %d\tNote is:  %s\n", roomid, sound, notice);


}


/*
 * 此函数主要是实现一个循环，每隔一分钟检测
 * 一次alarm.conf文件，根据文件中的数据定时执行命令
 */

void alarm_server (void) {
	time_t t, now;
	struct tm * nowtime;
	uint8 day[7] = {0};
	time(&t);
	nowtime = localtime(&t);
	if (nowtime->tm_sec > 0 && nowtime->tm_sec < 60) {
		sleep(60 - nowtime->tm_sec);
	}
	while(1) {
		DEBUG(" Alarm=======>  Alarm server is running!\n");
		time(&t);
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
			continue;
		} else {
			DEBUG("Open database successfuly!\n");
			char sql[300] = "create table alarm_data("
					"ID integer primary key, "
					"sett integer,"
					"hour integer, "
					"minute integer, "
					"roomid integer,"
					"yinyue integer,"
					"sun integer, "
					"mon integer, "
					"tue integer, "
					"wed integer, "
					"thu integer, "
					"fri integer, "
					"sat integer, "
					"tishi varchar(40))";
			sqlite3_exec(db, sql, 0, 0, &Errormsg);
			//读出数据库表中所有数据
			strcpy(sql, "select * from alarm_data");
			sqlite3_get_table(db, sql, &result, &row, &col, &Errormsg);
			sqlite3_close(db);

			time(&now);
			nowtime = localtime(&now);
			DEBUG("*******************************\n");
			DEBUG("The Time Now is  %d:%d\t%d\n", nowtime->tm_hour, nowtime->tm_min, nowtime->tm_wday);
			DEBUG("*******************************\n");
			int i = 0;
			for (i = 1; i < row + 1; i ++) {
				DEBUG("-----------------------------debug alarm!\n");
				int j = 0;
				for (j = 0; j < col; j ++) {
					printf("%s\t", result[i * col + j]);
				}
				DEBUG("\n");
				if (atoi(result[i * col + 3]) == nowtime->tm_min) {
					if (atoi(result[i * col + 2]) == nowtime->tm_hour) {
						day[0] = atoi(result[i * col + 6]);
						day[1] = atoi(result[i * col + 7]);
						day[2] = atoi(result[i * col + 8]);
						day[3] = atoi(result[i * col + 9]);
						day[4] = atoi(result[i * col + 10]);
						day[5] = atoi(result[i * col + 11]);
						day[6] = atoi(result[i * col + 12]);
						if (day[nowtime->tm_wday] == 1) {
							alarm_cycle(atoi(result[i * col + 4]), atoi(result[i * col + 5]), result[i * col + 13]);
						}
					}
				}
			}
		}
		//睡眠1分钟
		DEBUG(" Alarm=======> before sleep!\n");
		sleep(50);
		time(&now);
		t += 60;
		sleep(t - now);
	}
}





























