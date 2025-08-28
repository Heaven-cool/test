/*===============================================
 *   文件名称：main.c
 *   创 建 者：
 *   创建日期：2025年07月24日
 *   描    述：
 ================================================*/
#include "client_net.h"
#include "client_ui.h"
#include "common.h"

int USER_STATUS = 0;     // 用户状态
char USERNAME[20] = {0}; // 用户名
Msg_Bind MB = {0};       // 全局的通信套接字和消息队列结构体
Mb mb = {0};             // 消息队列中存储消息结构
pthread_t tid = 0;       // 保存线程id

int main(int argc, char *argv[])
{
    int connfd = client_init("0.0.0.0", 6666);
    if (connfd < 0)
    {
        printf("服务器异常！\n");
        return -1;
    }
    // 将通信套接字放入全局,创建消息队列
    MB.fd = connfd;
    mb.mtype = connfd;

    // 加载进度条
    progress_bar();
    while (USER_STATUS != -1)
    {
        if (USER_STATUS == 0)
            client_wait_page(connfd); // 未登录页面循环
        else if (USER_STATUS == 1)
            client_home_page(connfd); // 主页循环
        else if (USER_STATUS == 2)
            runing_game_page(connfd); // 主页循环
    }
    close(connfd);
    return 0;
}
