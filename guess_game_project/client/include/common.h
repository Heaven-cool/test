#ifndef _COMMON_H
#define _COMMON_H

#define INPUT_ERR "The input format is 2 parameters, such as './app ip port'\n"
#define IP_LEN 32
#include <string.h>

extern int USER_STATUS;   // 用户登录状态
extern char USERNAME[20]; // 存储用户名

// 客户端数据
typedef struct c_data
{
    char username[10];
    char password[15];
    int number;
} Client_Data;

typedef struct c_messaage
{
    char type[20];
    char name[10]; // 准备请求时需要携带的参数
    Client_Data data;
} Client_Msg;

// 服务器端的数据
typedef struct s_data
{
    char username[10];
    int wins;                // 胜利场次
    int loss;                // 失败场次
    int deuce;               // 平局场次
    float win_rate;          // 胜率
    char wait_player[3][10]; // 房间等待用户信息
    int number;              // 游戏数字
} Server_Data;

typedef struct s_message
{
    char type[20];    // 响应类型
    int status;       // 响应状态码 成功：1  失败：0
    char message[50]; // 响应信息
    Server_Data data; // 响应数据
} Server_Msg;

typedef struct msg_bind
{
    int fd;
    int msgid;
} Msg_Bind;

// 消息对列，收发消息的结构体
typedef struct msg_buffer
{
    long mtype;       // 消息类型，必须 > 0
    char mtext[1024]; // 消息正文（可自定义）
} Mb;

const static int MSG_LEN = sizeof(Client_Msg);

#endif
