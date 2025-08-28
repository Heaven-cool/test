#include "game.h"
#include "handler.h"

// 房间广播函数
void room_broadcast(GameRoom *room, char *type, char *msg, Server_Msg *smsg)
{
  for (int i = 0; i < 3; ++i)
  {
    int fd = room->player_fds[i];
    if (fd > 0)
    {
      send_success(fd, type, msg, smsg);
    }
  }
}

// 加入等待
int add_waite(int conn_fd, Client_Msg *cmsg)
{
  int i;
  // 找空插入
  for (i = 0; i < 3; i++)
  {
    if (WAITING.player_fds[i] == 0)
    {
      WAITING.player_fds[i] = conn_fd;
      strcpy(WAITING.player_name[i], cmsg->name);
      break;
    }
  }

  WAITING.count++;
  Server_Msg smsg;
  memset(&smsg, 0, sizeof(smsg));
  for (i = 0; i < 3; i++)
  {
    strcpy(smsg.data.wait_player[i], WAITING.player_name[i]);
  }
  // 广播游戏准备玩家信息
  room_broadcast(&WAITING, "res_realy", "等待其他玩家！", &smsg);
  return WAITING.count; // 返回等待池的人数
}

// 退出准备
int remove_waite(int conn_fd, Client_Msg *cmsg)
{
  int i;
  Server_Msg smsg;
  if (WAITING.count == 0)
  {
    send_success(conn_fd, "res_quit_realy", "退出失败，游戏已经开始！", &smsg);
    return 0;
  }
  for (i = 0; i < 3; i++)
  {
    if (WAITING.player_fds[i] == conn_fd)
    {
      WAITING.player_fds[i] = 0;
      strcpy(WAITING.player_name[i], " ");
    }
  }
  WAITING.count--;
  // 广播游戏准备玩家信息
  memset(&smsg, 0, sizeof(smsg));
  for (i = 0; i < 3; i++)
  {
    strcpy(smsg.data.wait_player[i], WAITING.player_name[i]);
  }
  room_broadcast(&WAITING, "res_realy", "等待其他玩家！", &smsg);
  return WAITING.count; // 返回等待池的人数
}
