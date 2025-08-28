#ifndef _SERVER_NET_H
#define _SERVER_NET_H

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>
#include <pthread.h>
#include "game.h"

// 初始化服务器
int server_init(char *ip, unsigned short port, int backlog);

// 循环监听客户端连接
void server_listen_loop(int listen_fd);

#endif
