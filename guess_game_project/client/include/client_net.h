#ifndef _CLIENT_NET_H
#define _CLIENT_NET_H

#include <stdio.h>
#include <string.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>
#include "client_ui.h"
#include "client_utils.h"
#include "common.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include "log_utils.h"

extern Msg_Bind MB;
extern Mb mb;
extern pthread_t tid;

int client_init(char *ip, unsigned short port);

// 创建消息队列，用来游戏界面交互
int msgqueue_create();

// 游戏房间准备进程函数
void *game_room(void *arg);

int client_wait_page(int connfd);

int client_home_page(int connfd);

// 游戏中页面循环
int runing_game_page(int connfd);
#endif
