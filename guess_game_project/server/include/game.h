#ifndef _GAME_H
#define _GAME_H

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include "common.h"

// 游戏房间结构体
typedef struct
{
  int player_fds[3];       // 三个玩家的连接 fd
  char player_name[3][10]; // 三个玩家的用户名
  int ready[3];            // 每个玩家是否准备好了
  int count;               // 当前房间人数
  int target_number;       // 本局游戏要猜的数字
} GameRoom;

// 消息对列，收发消息的结构体
typedef struct msg_buffer
{
  long mtype;       // 消息类型，必须 > 0
  char mtext[1024]; // 消息正文（可自定义）
} Mb;

extern GameRoom WAITING;

// 房间广播函数
void room_broadcast(GameRoom *room, char *type, char *msg, Server_Msg *smsg);

// 将进入游戏准备的玩家加入等待池
int add_waite(int conn_fd, Client_Msg *cmsg);

// 退出准备
int remove_waite(int conn_fd, Client_Msg *cmsg);

#endif
