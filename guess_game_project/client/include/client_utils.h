#ifndef _CLIENT_UTILS_H
#define _CLIENT_UTILS_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <limits.h>
#include "log_utils.h"

extern pthread_t tid;

// 校验输入参数
int check_input_option(char *buf);

// 校验用户名，长度须为3-10，英文字母和数字组成
int check_username(char *buf);

// 校验密码，长度须为6-15，英文字母和数字组成
int check_password(char *buf);

// 校验输入数据
int check_input_number(char *buf);

#endif
