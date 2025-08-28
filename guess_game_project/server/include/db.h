#ifndef _DB_H
#define _DB_H

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
#include <sqlite3.h>
#include "common.h"
#include "json_utils.h"
#include "handler.h"

// 登录处理函数
void login_function(int conn_fd, Client_Msg *msg);

// 注册处理函数
void register_function(int conn_fd, Client_Msg *msg);

// 战绩查询函数
void sore_function(int conn_fd, Client_Msg *cmsg);

// 添加平局场次
void add_deuce(char *username);

// 添加胜利场次
void add_wins(char *username);

// 添加失败场次
void add_loss(char *usernaem);

#endif
