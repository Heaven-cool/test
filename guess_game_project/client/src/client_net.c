#include "client_net.h"

// 连接服务器
int client_init(char *ip, unsigned short port)
{
    // printf("进入连接\n");
    int connfd, ret;
    connfd = socket(AF_INET, SOCK_STREAM, 0);
    // printf("sokcket 结束\n");
    if (-1 == connfd)
    {
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(ip);

    // printf("准备connection\n");
    ret = connect(connfd, (struct sockaddr *)&saddr, sizeof(saddr));
    // printf("connnection 结束\n");
    if (-1 == ret)
    {
        perror("connect");
        return -1;
    }
    printf("服务器连接成功！\n");
    return connfd;
}

// 创建消息队列，用来游戏界面交互
int msgqueue_create()
{
    // 创建消息队列，权限为 0666
    int msqid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if (msqid == -1)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    return msqid;
}

// 游戏房间准备线程函数
void *game_room(void *arg)
{
    LOG_MSG("进入房间线程");
    // sleep(3);
    char res_type[20] = {0};
    int i;
    Server_Msg smsg = {0};
    char wait_player[3][10];
    MB.msgid = msgqueue_create(); // 创建消息队列
    while (USER_STATUS == 1)
    {
        // 接收队列中的第一条消息
        ssize_t bytes_received = msgrcv(MB.msgid, &mb, sizeof(mb.mtext), 0, IPC_NOWAIT);
        if (bytes_received == -1)
        {
            if (errno == ENOMSG)
            {
                // 当前没有消息，稍等后重试
                usleep(100000); // 100ms
                continue;
            }
            else
            {
                perror("msgrcv failed");
                break;
            }
        }
        memset(&smsg, 0, sizeof(smsg));
        memset(res_type, 0, sizeof(res_type));
        memset(wait_player, 0, sizeof(wait_player));
        if (strncmp(mb.mtext, "quit_realy", 10) == 0) // 如果是直接的一个请求退出准备,则直接退出结束线程
        {
            // 请求退出准备阶段，直接结束房间线程，将状态变为1，回到主页
            MB.msgid = 0;
            USER_STATUS = 1;
            tid = 0;
            pthread_exit(NULL);
        }
        LOG_MSG("准备解包");
        json_unpack(mb.mtext, res_type, &smsg); // 解包
        LOG_MSG("解包完成");
        printf("解包后的res_type为：%s\n", res_type);
        for (i = 0; i < 3; i++)
        {
            strcpy(wait_player[i], smsg.data.wait_player[i]);
        }

        if (strncmp(res_type, "res_realy", 9) == 0)
        {
            system("clear");
            if (smsg.status == 0)
            {
                printf("房间等待\n");
                printf("%s\n", smsg.message);
                printf("\n\n\n输入0,可退出房间：");
                fflush(stdout);
            }
            else if (smsg.status == 1)
            {
                printf("房间等待\n");
                printf("已准备玩家：%s\t%s\t%s\n", wait_player[0], wait_player[1], wait_player[2]);
                printf("\n\n\n输入0,可退出房间：");
                fflush(stdout);
            }
        }
        else if (strncmp(res_type, "quit_realy", 10) == 0)
        {
            // 请求退出准备阶段，直接结束房间线程，将状态变为1，回到主页
            USER_STATUS = 1;
            tid = 0;
            pthread_exit(NULL);
        }
        else if (strncmp(res_type, "start_game", 10) == 0)
        {
            LOG_MSG("进入条件");
            // 开始游戏，将状态变为2，进入游戏循环，并直接结束线程
            USER_STATUS = 2;
            MB.msgid = 0;
            tid = 0;
            LOG_MSG("收到退出请求，准备调用 pthread_exit");
            pthread_exit(NULL);
            // LOG_MSG("return返回后");
        }
    }
    pthread_exit(NULL);
}

// 待登录页面，使用IO多路复用（select）,进行循环操作
int client_wait_page(int connfd)
{
    char buf[1024] = {0};
    int n, ret;
    char res_type[20];      // 响应类型
    Server_Msg smsg;        // 接收响应数据
    fd_set rfds, fds;       // 定义读结合和监听集合
    FD_ZERO(&rfds);         // 清空集合
    FD_SET(0, &rfds);       // 将标准输入的文件描述符加入读集合
    FD_SET(connfd, &rfds);  // 将通信套结字 加入集合
    int maxfd = connfd + 1; // maxfd的值是最大文件描述符加 1
    do
    {
        wait_show(); // 显示选项
        fds = rfds;
        ret = select(maxfd, &fds, NULL, NULL, NULL);
        if (-1 == ret)
        {
            perror("select");
            break;
        }
        if (FD_ISSET(0, &fds))
        {
            // 监听客户输入的信息
            memset(buf, 0, sizeof(buf));
            fgets(buf, sizeof(buf), stdin);
            // 校验输入选项
            ret = check_input_option(buf);
            if (-1 == ret)
            {
                printf("无效参数！");
            }
            else if (buf[0] == '1')
            {

                memset(buf, 0, sizeof(buf));
                // 登录函数
                login_function(buf, sizeof(buf));
                //printf("登录JSON：%s\n", buf);
                // sleep(20);
            }
            else if (buf[0] == '2')
            {
                memset(buf, 0, sizeof(buf));
                // 注册函数
                register_function(buf, sizeof(buf));
            }
            else if (buf[0] == '3')
            {
                // 退出
                quit_function();
            }
            ret = send(connfd, buf, strlen(buf), 0);
            if (ret == -1)
            {
                perror("write");
                break;
            }
        }
        else if (FD_ISSET(connfd, &fds))
        {
            memset(buf, 0, sizeof(buf));
            n = recv(connfd, buf, sizeof(buf), 0);
            if (-1 == n)
            {
                perror("read");
                break;
            }
            else if (0 == n) // 服务器关闭
            {
                printf("服务器异常！\n");
                FD_CLR(connfd, &fds);
                USER_STATUS = -1;
                break;
            }
            else
            {
                // 收到服务器发来的信息，需要进行处理
                // 解包
                //printf("收到的JSON数据%s\n", buf);
                //sleep(3);
                json_unpack(buf, res_type, &smsg);
                if (strncmp(res_type, "res_login", 9) == 0)
                {
                    // 登录响应处理函数
                    res_login_function(&smsg);
                }
                else if (strncmp(res_type, "res_register", 13) == 0)
                {
                    res_register_function(&smsg);
                }
            }
        }
    } while (USER_STATUS == 0); // 若为未登录状态

    return 0;
}

// 主页循环
int client_home_page(int connfd)
{
    char buf[1024] = {0};
    int n, ret;
    char res_type[20]; // 响应类型
    Server_Msg smsg;
    Client_Msg cmsg;        // 接收服务器响应信息
    fd_set rfds, fds;       // 定义读结合和监听集合
    FD_ZERO(&rfds);         // 清空集合
    FD_SET(0, &rfds);       // 将标准输入的文件描述符加入读集合
    FD_SET(connfd, &rfds);  // 将通信套结字 加入集合
    int maxfd = connfd + 1; // maxfd的值是最大文件描述符加 1
    do
    {
        home_show(); // 显示主页选项
        fds = rfds;
        ret = select(maxfd, &fds, NULL, NULL, NULL);
        if (-1 == ret)
        {
            perror("select");
            break;
        }
        if (FD_ISSET(0, &fds))
        {
            // 监听客户输入的信息
            memset(buf, 0, sizeof(buf));
            fgets(buf, sizeof(buf), stdin);
            // 校验输入选项
            ret = check_input_option(buf);
            printf("输入选项参数为：%s\n", buf);
            if (-1 == ret)
            {
                printf("无效参数！");
                continue;
            }
            else if (buf[0] == '0')
            {
                // 退出房间准备
                memset(buf, 0, sizeof(buf));
                memset(mb.mtext, 0, sizeof(mb.mtext));
                memset(&cmsg, 0, sizeof(cmsg));
                strcpy(mb.mtext, "quit_realy");
                strcpy(cmsg.type, "quit_realy");
                strcpy(cmsg.name, USERNAME);
                msgsnd(MB.msgid, &mb, sizeof(buf), 0);
                // 打包发送给服务器，请求退出
                json_pack(buf, sizeof(buf), &cmsg);
                send(connfd, buf, strlen(buf), 0); // 发送
            }
            else if (buf[0] == '1')
            {
                memset(buf, 0, sizeof(buf));
                // 发送请求，准备游戏
                realy_function(connfd);
                // 创建线程 专门游戏房间
                LOG_MSG("准备创建房间线程");
                ret = pthread_create(&tid, NULL, game_room, NULL);
                if (ret != 0)
                {
                    perror("pthread_create fail");
                    return -1;
                }
                ret = pthread_detach(tid); // 线程分离
            }
            else if (buf[0] == '2')
            {
                memset(buf, 0, sizeof(buf));
                // 发送战绩查询请求函数
                sore_function(connfd);
            }
            else if (buf[0] == '3')
            {
                // 游戏说明
                instructions_function();
            }
            else if (buf[0] == '4')
            {
                // 退出登录
                log_out();
            }
        }
        else if (FD_ISSET(connfd, &fds))
        {
            memset(buf, 0, sizeof(buf));
            n = read(connfd, buf, sizeof(buf));
            if (-1 == n)
            {
                perror("read");
                break;
            }
            else if (0 == n) // 服务器关闭
            {
                printf("服务器异常！\n");
                FD_CLR(connfd, &fds);
                USER_STATUS = -1;
                system("clear");
                break;
            }
            else
            {
                // 收到服务器发来的信息，需要进行处理
                // 解包
                LOG_MSG("解包之前的buf");
                printf("收到的JSON数据%s\n", buf);
                LOG_MSG("解包之前的buf");
                // sleep(3);
                json_unpack(buf, res_type, &smsg);
                if (strncmp(res_type, "res_sore", 8) == 0)
                {
                    // 接收查战绩响应处理函数
                    res_sore_function(&smsg);
                }
                else if (strncmp(res_type, "res_realy", 9) == 0)
                {
                    // 接收请求准备响应处理,直接将接受的消息通过消息队列发送给游游戏线程
                    memset(mb.mtext, 0, sizeof(mb.mtext));
                    strcpy(mb.mtext, buf);
                    msgsnd(MB.msgid, &mb, sizeof(buf), 0);
                }
                else if (strncmp(res_type, "game_over", 9) == 0)
                {
                    // 接收请求准备响应处理,直接将接受的消息通过消息队列发送给游游戏线程
                    memset(mb.mtext, 0, sizeof(mb.mtext));
                    strcpy(mb.mtext, buf);
                    msgsnd(MB.msgid, &mb, sizeof(buf), 0);
                }
                else if (strncmp(res_type, "start_game", 10) == 0)
                {
                    // 接收请求准备响应处理,直接将接受的消息通过消息队列发送给游游戏线程
                    strcpy(mb.mtext, buf);
                    msgsnd(MB.msgid, &mb, sizeof(buf), 0);
                    LOG_MSG("更改状态值");
                    USER_STATUS = 2;
                }
            }
        }
    } while (USER_STATUS == 1); // 主页循环

    return 0;
}

// 游戏中页面循环
int runing_game_page(int connfd)
{
    system("clear");
    char buf[1024] = {0};
    int n, ret;
    char res_type[20];      // 响应类型
    Server_Msg smsg;        // 接收服务器响应信息
    Client_Msg cmsg;        // 客户端要发送的信息
    fd_set rfds, fds;       // 定义读结合和监听集合
    FD_ZERO(&rfds);         // 清空集合
    FD_SET(0, &rfds);       // 将标准输入的文件描述符加入读集合
    FD_SET(connfd, &rfds);  // 将通信套结字 加入集合
    int maxfd = connfd + 1; // maxfd的值是最大文件描述符加 1

    // 游戏倒计时
    timer();
    printf("开始！\n");
    do
    {
        fds = rfds;
        ret = select(maxfd, &fds, NULL, NULL, NULL);
        if (-1 == ret)
        {
            perror("select");
            break;
        }
        if (FD_ISSET(0, &fds))
        {
            // 监听客户输入的信息
            memset(buf, 0, sizeof(buf));
            fgets(buf, sizeof(buf), stdin);
            // 校验输入选项
            LOG_MSG("准备数据校验！");
            ret = check_input_number(buf);
            if (-1 == ret)
            {
                printf("无效数据！\n");
            }
            else
            {
                // 发送猜测的数据
                memset(&cmsg, 0, sizeof(cmsg));
                cmsg.data.number = atoi(buf); // 放数据
                strcpy(cmsg.name, USERNAME);  // 放用户名
                strcpy(cmsg.type, "guess");   // 放请求类型
                memset(buf, 0, sizeof(buf));
                json_pack(buf, sizeof(buf), &cmsg);
                // printf("猜数请求发送:%s\n", buf);
                ret = send(connfd, buf, sizeof(buf), 0);
            }
        }
        else if (FD_ISSET(connfd, &fds))
        {
            memset(buf, 0, sizeof(buf));
            n = read(connfd, buf, sizeof(buf));
            if (-1 == n)
            {
                perror("read");
                break;
            }
            else if (0 == n) // 服务器关闭
            {
                printf("服务器异常！\n");
                FD_CLR(connfd, &fds);
                USER_STATUS = -1;
                system("clear");
                break;
            }
            else
            {
                // 收到服务器发来的信息，需要进行处理
                // 解包
                // printf("收到的JSON数据%s\n", buf);
                // sleep(3);
                json_unpack(buf, res_type, &smsg);
                if (strncmp(res_type, "game_over", 9) == 0)
                {
                    // 游戏结束，进入结算循环
                    game_out_loop(&smsg);
                }
                else if (strncmp(res_type, "res_guess", 9) == 0)
                {
                    // 接收请求准备响应处理,直接将接受的消息通过消息队列发送给游游戏线程
                    if (smsg.status == 1)
                    {
                        printf("%s\n", smsg.message);
                    }
                    else
                    {
                        printf("服务器开小差！\n");
                    }
                }
            }
        }
    } while (USER_STATUS == 2); // 游戏页面循环

    return 0;
}
