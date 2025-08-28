#ifndef _JSON_UTILS_H
#define _JSON_UTILS_H
#include <stdio.h>
#include "cJSON.h"
#include <string.h>
#include <stdlib.h>
#include "common.h"


//json打包函数   接收传出json   buf的大小       需要打包的字段信息放在结构体中传入    
int json_pack(char *buf, size_t buf_size, const Client_Msg * msg);

//json解包函数   传入json字符串  传出请求类型  传出请求信息结构体
int json_unpack(char *buf,char * type,Server_Msg * msg);

//登录请求打包
int json_pack_login(char *buf, size_t buf_size, const Client_Msg * msg);

//注册请求打包
int json_pack_register(char *buf, size_t buf_size, const Client_Msg * msg);

//游戏准备请求打包
int json_pack_realy(char *buf, size_t buf_size, const Client_Msg * msg);


#endif

