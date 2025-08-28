#include "client_ui.h"

// 游戏开始倒计时
void timer()
{
    printf("游戏即将开始！");
    fflush(stdout);
    sleep(5);
    system("clear");
    printf("倒计时：3");
    fflush(stdout);
    sleep(1);
    system("clear");
    printf("倒计时：2");
    fflush(stdout);
    sleep(1);
    system("clear");
    printf("倒计时：1");
    fflush(stdout);
    sleep(1);
    system("clear");
}

// 打印进度条
void make(int n)
{
    printf("\n\n\n服务器已连接成功！\n系统加载中...");
    for (int i = 0; i < N; i++)
    {
        if (i < n)
        {
            printf("\033[1;34m=\033[0m");
        }
        else
        {
            printf(">");
        }
    }
    printf("[%%%.2f]", (n * 1.0 / N) * 100);
    printf("\n");
}
void progress_bar()
{
    int i = 1;
    // 进度条
    while (i < 100)
    {
        system("clear");
        make(i++);
        sleep(1);
        system("clear");
        make(i);
        if (i >= 100)
            break;
        i = i + 20;
        sleep(2);
        system("clear");
        make(i);
        i = i + 28;
        sleep(1);
    }
}

// 待登录页面
void wait_show()
{
    system("clear");
    printf("%s", WAIT_OPTION);
}

// 主页
void home_show()
{
    system("clear");
    printf("%s", HOME_OPTION);
}

// 游戏结束，等待退出
void game_out_loop(Server_Msg *msg)
{
    char flag[4]; // 大一些，能容纳字符 + \n + \0

    while (1)
    {
        system("clear");
        printf("游戏结束：\n\n");
        printf("%s\n", msg->message);
        printf("正确答案为：%d\n", msg->data.number);
        printf("输入0，退出结算：");

        // 读取一整行输入（含换行）
        if (fgets(flag, sizeof(flag), stdin) == NULL)
            break;

        // 判断是否是 '0'（注意是字符串比较）
        if (flag[0] == '0')
        {
            system("clear");
            USER_STATUS = 1;
            break;
        }
    }
}

// 登录
int login_function(char *buf, size_t buf_size)
{
    char username[12];
    char password[16];
    system("clear");
    printf("**登录操作**\n");
    printf("请输入用户名：");
    scanf("%s", username);

    if (check_username(username) < 0)
    {
        printf("用户名格式为：3-10位，由字母和数字组合成！\n");
        sleep(3);
        return -1;
    }
    printf("请输入密码：");
    scanf("%s", password);
    if (check_password(password) < 0)
    {
        printf("密码格式为：5-15位，由字母和数字组成！\n");
        sleep(3);
        return -1;
    }

    Client_Msg msg;
    strcpy(msg.type, "login");
    strcpy(msg.data.username, username);
    strcpy(msg.data.password, password);

    return json_pack(buf, buf_size, &msg); // 打包
}
// 登录响应处理
void res_login_function(Server_Msg *smsg)
{
    int status = smsg->status;
    if (0 == status)
    {
        printf("%s\n", smsg->message);
        sleep(3);
        return;
    }
    else if (1 == status)
    {
        printf("登录成功！\n");
        strcpy(USERNAME, smsg->data.username); // 存储用户名
        USER_STATUS = 1;                       // 更新登录状态
    }
    return;
}

// 注册
int register_function(char *buf, size_t buf_size)
{

    char username[12];
    char password[16];
    system("clear");
    printf("**注册操作**\n");
    printf("请输入用户名：");
    scanf("%s", username);

    if (check_username(username) < 0)
    {
        printf("用户名格式为：3-10位，由字母和数字组合成！\n");
        sleep(3);
        return -1;
    }
    printf("请输入密码：");
    scanf("%s", password);
    if (check_password(password) < 0)
    {
        printf("密码格式为：5-15位，由字母和数字组成！\n");
        sleep(3);
        return -1;
    }

    Client_Msg msg;
    strcpy(msg.type, "register");
    strcpy(msg.data.username, username);
    strcpy(msg.data.password, password);

    return json_pack(buf, buf_size, &msg); // 打包
}
// 注册响应处理
void res_register_function(Server_Msg *smsg)
{
    int status = smsg->status;
    if (0 == status)
    {
        printf("%s\n", smsg->message);
        sleep(3);
        return;
    }
    else if (1 == status)
    {
        printf("注册成功！\n");
        sleep(3);
    }
    return;
}

// 退出
int quit_function()
{
    USER_STATUS = -1;
    memset(USERNAME, 0, sizeof(USERNAME));
    return 0;
}

// 退出登录
int log_out()
{
    USER_STATUS = 0;
    memset(USERNAME, 0, sizeof(USERNAME));
    return 0;
}

// 游戏说明，页面循环
int instructions_function()
{
    char flag[4]; // 大一些，能容纳字符 + \n + \0

    while (1)
    {
        system("clear");
        printf("功能说明：\n\n");
        system("cat instruction_book");
        printf("输入0，结束阅读：");

        // 读取一整行输入（含换行）
        if (fgets(flag, sizeof(flag), stdin) == NULL)
            break;

        // 判断是否是 '0'（注意是字符串比较）
        if (flag[0] == '0')
        {
            system("clear");
            break;
        }
    }
    return 0;
}

// 发送战绩查询请求函数
int sore_function(int connfd)
{
    Client_Msg cmsg = {0};
    char buf[1024] = {0};
    strcpy(cmsg.type, "sore");
    strcpy(cmsg.name, USERNAME);
    json_pack(buf, sizeof(buf), &cmsg);
    int ret = send(connfd, buf, sizeof(buf), 0);
    if (ret < 0)
    {
        printf("sore send fail!");
        return -1;
    }
    return 0;
}

// 接收战绩响应处理
void res_sore_function(Server_Msg *smsg)
{
    // 循环打印战绩，直至输入0表示停止
    int status = smsg->status;
    char flag[4]; // 大一些，能容纳字符 + \n + \0

    if (status == 1)
    {
        while (1)
        {
            system("clear");

            printf("战绩查询\n\n");
            printf("+--------------+----------+---------+---------+----------+\n");
            printf("| %-12s | %-8s | %-7s | %-7s | %-8s |\n", "username", "wins", "loss", "deuce", "win_rate");
            printf("+--------------+----------+---------+---------+----------+\n");
            printf("| %-12s | %-8d | %-7d | %-7d | %-8.2f |\n",
                   USERNAME, smsg->data.wins, smsg->data.loss, smsg->data.deuce, smsg->data.win_rate);
            printf("+--------------+----------+---------+---------+----------+\n");

            printf("\n\n输入0，退出查询：");

            if (fgets(flag, sizeof(flag), stdin) == NULL)
                break;

            if (flag[0] == '0')
            {
                system("clear");
                break;
            }
        }
    }
    else if (status == 0)
    {
        while (1)
        {
            system("clear");
            printf("战绩查询：\n\n");
            printf("%s\n\n", smsg->message);
            printf("输入0，退出查询：");

            // 读取一整行输入（含换行）
            if (fgets(flag, sizeof(flag), stdin) == NULL)
                break;

            // 判断是否是 '0'（注意是字符串比较）
            if (flag[0] == '0')
            {
                system("clear");
                break;
            }
        }
    }

    return;
}

// 请求准备函数
int realy_function(int connfd)
{
    Client_Msg cmsg = {0};
    char buf[1024] = {0};
    strcpy(cmsg.type, "realy");
    strcpy(cmsg.name, USERNAME);
    json_pack(buf, sizeof(buf), &cmsg);
    int ret = send(connfd, buf, sizeof(buf), 0);
    if (ret < 0)
    {
        printf("realy send fail!");
        return -1;
    }
    return 0;
}

// 请求准备响应函数
void res_realy_function(Server_Msg *smsg)
{
    int status = smsg->status;
    char flag[4];
    if (0 == status)
    {
        while (1)
        {
            system("clear");
            printf("游戏准备：\n\n");

            printf("输入0，退出准备：");

            // 读取一整行输入（含换行）
            if (fgets(flag, sizeof(flag), stdin) == NULL)
                break;

            // 判断是否是 '0'（注意是字符串比较）
            if (flag[0] == '0')
            {
                system("clear");
                break;
            }
        }
    }
    else if (1 == status)
    {
        while (1)
        {
        }
    }
}
