#ifndef _HANDLER_H
#define _HANDLER_H

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>                            //man 3 socket
#include <netinet/ip.h> /* superset of previous */ //man 7 ip
#include <arpa/inet.h>                             //man 3 inet_addr
#include <netinet/in.h>
#include <unistd.h>   //man 2 read
#include <stdlib.h>   //man 3 system
#include <signal.h>   //man 3 signal
#include <sys/wait.h> //man 3 waitpid
#include <ctype.h>    //man 3 toupper
#include <pthread.h>
#include "common.h"
#include "game.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "game.h"
#include "json_utils.h"
#include "db.h"
#include "log_utils.h"

extern int msg_queue[];

// 封装请求成功的通信
int send_success(int conn_fd, char *type, char *message, Server_Msg *smsg);

// 封装请求失败的通信
int send_error(int conn_fd, char *type, char *message);

// 通信线程函数
void *connection_handler(void *arg);

// 游戏房间的线程函数
void *game_handler(void *arg);

#endif
