/*===============================================
*   文件名称：main.c
*   创 建 者：
*   创建日期：2025年07月24日
*   描    述：
================================================*/
#include "server_net.h"
#include "game.h"

// 进入游戏房间后，玩家应的消息队列
int msg_queue[512] = {0};
// 玩家等待房间池
GameRoom WAITING = {0};
int main(int argc, char *argv[])
{
    int listen_fd;
    // 初始化服务器监听套接字
    listen_fd = server_init(NULL, 6666, 1024);
    // 循环监听连接
    server_listen_loop(listen_fd);
    return 0;
}
