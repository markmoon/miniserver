/*
 * user_examine.c
 *
 *  Created on: 2011-11-6
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



#include "types.h"
#include "define.h"
#include "user_examine.h"
#include "net.h"
#include "file_lock.h"
#include <sqlite3.h>
static char userconf[] = "/opt/smarthome/smarthome_user.conf";

/*
 * 把用户信息的结构体写入用户配置信息文档。
 * 当前的实现是直接写入，但是这样不是很安全，因为密码在配置文件中是可以直接看见的。
 * 所以后期可以用密码文本的MD5值来代替。之需要在本函数中把密码文本求出MD5码再写入文件即可。
 * 因为在智能家居中的用户数目不会太大，所以直接使用的文件的方式来管理用户信息。
 * 如果数量太大，或者是在其他的应用场合，可以使用数据库。
 * 其实在我的嵌入式Linux平台上是已经移植好数据库的，我选择的是SQLite。详细的SQLite信息请Google it。
 * 对于这些改变，都之需要在下面两个函数改写即可。
 */

unsigned int user_register(struct user * user_struct) {
	FILE *file;
	ssize_t writen;
	file = fopen(userconf, "a");
	if (file == NULL) {
		perror("fopen");
		return -1;
	}
	DEBUG("For debug : %s : %s \n", user_struct->username, user_struct->password);
	lock_set(fileno(file), F_WRLCK);
	writen = fwrite(user_struct, sizeof(struct user), 1, file);
	lock_set(fileno(file), F_UNLCK);
	if (writen != 1) {
		perror("fwrite");
		fclose(file);
		return -1;
	}
	fclose(file);
	return 0;
}

/*
 * 检查登录的用户是否是已经注册的用户，它与注册的函数是大同小可的。
 * 详见user_register(struct user *user_struct);
 */
unsigned int user_check(struct user * user_struct) {
	FILE *file;
	//ssize_t readen;
	struct user users[MAX_CONNECTED];
	int user_num = 0;
	file = fopen(userconf, "r");
	if (file == NULL) {
		perror("fopen");
		return -1;
	}
	lock_set(fileno(file), F_RDLCK);
	while(!feof(file) && user_num < MAX_CONNECTED) {
		if (!feof(file)) {
			fread(&users[user_num], sizeof(struct user), 1, file);
			//printf("当前从文件中读出的结构体数量：%d\n", readen);
			//if (readen != 1) {
			//	perror("fread");
			//	return -1;
			//}
		} else {
			break;
		}
		user_num += 1;
	}
	lock_set(fileno(file), F_UNLCK);
	fclose(file);
	int i = 0;
	for (i = 0; i < user_num; i ++) {
		if ((strcmp(user_struct->username, users[i].username) == 0) &&
			   (strcmp(user_struct->password, users[i].password) == 0)) {
			DEBUG("User: %s auth passed\n", user_struct->username);
			return 0;
		}
	}
	return -1;
}

















