#include "handler.h"

// 封装请求成功的通信
int send_success(int conn_fd, char *type, char *message, Server_Msg *smsg)
{
    smsg->status = 1;
    strcpy(smsg->type, type);
    strcpy(smsg->message, message);
    char buf[1024] = {0};
    // 打包
    json_pack(buf, sizeof(buf), smsg);

    // 发送
    send(conn_fd, buf, sizeof(buf), 0);
    return 0;
}

// 封装请求失败的通信
int send_error(int conn_fd, char *type, char *message) // 失败就不封装data了
{
    Server_Msg smsg;
    smsg.status = 0;
    strcpy(smsg.type, type);
    strcpy(smsg.message, message);
    char buf[1024] = {0};
    // 打包
    json_pack(buf, sizeof(buf), &smsg);

    // 发送
    send(conn_fd, buf, sizeof(buf), 0);
    return 0;
}

// 游戏房间的线程函数
void *game_handler(void *arg)
{
    GameRoom *room = (GameRoom *)arg;
    srand(time(NULL));
    room->target_number = rand() % 1000 + 1; // 1~1000的随机数
    int target = room->target_number;
    Server_Msg smsg;
    int i;
    Mb mb;
    char message[50];
    // 创建消息队列，权限为 0666
    int msqid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if (msqid == -1)
    {
        perror("msgget faile!");
    }

    // 将消息队列放入全局的，客户端绑定对应消息对列的数组中
    for (i = 0; i < 3; i++)
    {
        msg_queue[room->player_fds[i]] = msqid;
    }
    time_t start_time = time(NULL); // 开始计时

    while (1)
    {
        memset(&smsg, 0, sizeof(smsg));
        // 判断是否超时
        if (time(NULL) - start_time >= 69) // 多几秒，前端有开始前有倒计时
        {
            smsg.data.number = target;
            room_broadcast(room, "game_over", "游戏超时，未有人猜中", &smsg);

            // 存入结果
            for (i = 0; i < 3; i++)
            {
                add_deuce(room->player_name[i]);
            }

            // 清空消息队列绑定
            for (i = 0; i < 3; i++)
            {
                msg_queue[room->player_fds[i]] = 0;
            }
            msgctl(msqid, IPC_RMID, NULL); // 删除消息队列
            pthread_exit(NULL);            // 结束线程
        }

        // 接收队列中的第一条消息
        // IPC_NOWAIT非阻塞接收消息，避免死等导致超时无法处理
        // ssize_t bytes_received = msgrcv(msqid, &mb, sizeof(mb.mtext), 0, IPC_NOWAIT);
        ssize_t bytes_received = msgrcv(msqid, &mb, sizeof(mb.mtext), 0, 0);
        LOG_MSG("读取消息队列中的第一条信息");
        if (bytes_received == -1)
        {
            perror("msgrcv fail!");
            continue;
        }

        int guess = atoi(mb.mtext);
        if (guess == target)
        {
            // 找出猜对的客户端用户名
            for (i = 0; i < 3; i++)
            {

                if (room->player_fds[i] == mb.mtype)
                {
                    smsg.data.number = target;
                    sprintf(message, "游戏结束，胜利者为 %s", room->player_name[i]);
                    room_broadcast(room, "game_over", message, &smsg);
                    add_wins(room->player_name[i]);
                }
                else
                {
                    add_loss(room->player_name[i]);
                }
            }

            // 清空全局变量msg_queue中套接字对应的消息对列
            for (i = 0; i < 3; i++)
            {
                msg_queue[room->player_fds[i]] = 0;
            }
            msgctl(msqid, IPC_RMID, NULL); // 关闭删除消息队列
            pthread_exit(NULL);            // 直接结束线程
        }
        else if (guess > target)
        {
            send_success(mb.mtype, "res_guess", "猜大了", &smsg);
        }
        else
        {
            send_success(mb.mtype, "res_guess", "猜小了", &smsg);
        }
    }
}

// 通信线程处理函数
void *connection_handler(void *arg)
{
    int conn_fd;
    int ret, i;
    pthread_t tid; // 保存游戏线程id
    char buf[1024] = {0};
    char type[20] = {0}; // 接受请求类型
    conn_fd = *(int *)arg;
    Mb mb;
    char text[20];
    Client_Msg cmsg;

    mb.mtype = conn_fd; // 消息对列要发送的类型固定为当前通信套接字
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        ret = recv(conn_fd, buf, sizeof(buf), 0);
        if (-1 == ret)
        {
            perror("read fail");
            pthread_exit(NULL);
        }
        else if (0 == ret)
        {
            // 客户端退出
            // 将该该客户端的通信套接字在msg_queue中对应的消息队列清空（置为0）
            msg_queue[conn_fd] = 0;
            // 检查准备连接池WAITING，如果该通信套接字在里面，则将其置为去除
            for (i = 0; i < 3; i++)
            {
                if (conn_fd == WAITING.player_fds[i])
                {
                    WAITING.player_fds[i] = 0;
                    memset(WAITING.player_name[i], 0, 10);
                    WAITING.count--;
                }
            }
            printf("client quit!\n");
            close(conn_fd);
            pthread_exit(NULL);
        }
        else
        {
            memset(type, 0, sizeof(type));
            // 解包
            LOG_MSG("准备解包");
            json_unpack(buf, type, &cmsg);

            printf("请求类型:%s\n", type);
            if (strncmp(type, "login", 5) == 0)
            {
                // 登录处理函数
                login_function(conn_fd, &cmsg);
            }
            else if (strncmp(type, "register", 8) == 0)
            {
                // 注册处理函数
                register_function(conn_fd, &cmsg);
            }
            else if (strncmp(type, "sore", 4) == 0)
            {
                // 查战绩处理函数
                sore_function(conn_fd, &cmsg);
            }
            else if (strncmp(type, "realy", 5) == 0)
            {
                // 准备游戏
                // 加入等待池，,发送给客户端，房间用户信息
                ret = add_waite(conn_fd, &cmsg);
                if (3 == ret)
                {
                    // 等待池已满，可以创建游戏房间进程，并重置等待池
                    GameRoom gr = WAITING;
                    // 创建游戏线程
                    ret = pthread_create(&tid, NULL, game_handler, (void *)&gr);
                    if (ret != 0)
                    {
                        perror("game_room pthread create fail!");
                    }
                    Server_Msg smsg = {0};
                    room_broadcast(&WAITING, "start_game", "游戏开始，猜一个1~1000的数", &smsg);
                    // 重置等待池
                    memset(&WAITING, 0, sizeof(WAITING));
                }
            }
            else if (strncmp(type, "quit_realy", 10) == 0)
            {
                // 是退出准备的请求
                remove_waite(conn_fd, &cmsg);
            }
            else if (strncmp(type, "guess", 5) == 0)
            {
                // 是猜测数字请求，直接将猜测的数字发送给消息队列
                LOG_MSG("收到guess请求");
                memset(text, 0, sizeof(text));
                sprintf(text, "%d", cmsg.data.number);
                strcpy(mb.mtext, text);
                LOG_MSG("准备将guess请求发送到消息队列");
                if (msgsnd(msg_queue[conn_fd], &mb, sizeof(mb.mtext), 0) == -1)
                {
                    perror("msgsnd faile!");
                }
            }
            printf("打印buf--%s\n", buf);
        }
    }
    // 退出后关闭通信套接字
    close(conn_fd);

    return NULL;
}
