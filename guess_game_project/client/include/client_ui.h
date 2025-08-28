#ifndef _CLIENT_UI_H
#define _CLIENT_UI_H

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdlib.h>
#include "client_utils.h"
#include "common.h"
#include "json_utils.h"
#include "log_utils.h"
#define N 100

#define WAIT_OPTION "请选择操作：\n(1):登录\n(2):注册\n(3):退出\n----------------------------\n"
#define HOME_OPTION "**主页**\n功能选项：\n(1)：开始游戏\n(2)：战绩查询\n(3)：游戏说明\n(4)：退出登录\n----------------------------\n"

// 游戏开始倒计时
void timer();

// 进度条
void progress_bar();

// 登录等待页面
void wait_show();

// 主页展示
void home_show();

// 游戏结束，等待退出
void game_out_loop(Server_Msg *message);

// 登录处理
int login_function(char *buf, size_t buf_size);

// 登录响应处理函数
void res_login_function(Server_Msg *smsg);

// 注册处理
int register_function(char *buf, size_t buf_size);
// 注册响应处理
void res_register_function(Server_Msg *smsg);

// 退出处理
int quit_function();

// 退出登录
int log_out();

// 游戏说明，页面循环
int instructions_function();

// 发送战绩查询请求函数
int sore_function(int connfd);

// 响应战绩查询
void res_sore_function(Server_Msg *smsg);

// 请求准备游戏函数
int realy_function(int connfd);

// 请求准备响应函数
void res_realy_function(Server_Msg *smsg);

#endif
