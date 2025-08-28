#include "db.h"

// 登录处理函数
void login_function(int conn_fd, Client_Msg *msg)
{
    sqlite3 *db;
    int ret;
    ret = sqlite3_open("db/game.db", &db); // 打开数据库
    if (ret != SQLITE_OK)
    {
        printf("sqlite open fail:%s\n", sqlite3_errmsg(db));
        return;
    }

    // 构造 SQL 查询语句
    char sql[256];
    snprintf(sql, sizeof(sql),
             "select * from users where username = '%s' and password = '%s';",
             msg->data.username, msg->data.password);

    printf("执行的sql:%s\n", sql);

    char **pResult;
    int pRow;
    int pColumn;
    char *errmsg;

    ret = sqlite3_get_table(db, sql, &pResult, &pRow, &pColumn, &errmsg);
    if (ret != SQLITE_OK)
    {
        printf("sqlite_get_table fail:%s\n", sqlite3_errmsg(db));
        sqlite3_free(errmsg); // 释放错误信息
        send_error(conn_fd, "res_login", "登录失败！服务异常");
        return;
    }

    if (pRow == 0)
    {
        send_error(conn_fd, "res_login", "登录失败！用户名或密码错误！");
        return;
    }
    Server_Msg smsg = {0};
    strcpy(smsg.data.username, msg->data.username);
    send_success(conn_fd, "res_login", "登录成功！", &smsg);
    return;
}

// 注册处理函数
void register_function(int conn_fd, Client_Msg *msg)
{
    sqlite3 *db;
    int ret;
    ret = sqlite3_open("db/game.db", &db); // 打开数据库
    if (ret != SQLITE_OK)
    {
        printf("sqlite open fail:%s\n", sqlite3_errmsg(db));
        return;
    }
    // 查询用户名是否已存在
    char sql1[256];
    snprintf(sql1, sizeof(sql1),
             "select * from users where username = '%s' ;",
             msg->data.username);
    printf("执行查重的sql:%s\n", sql1);

    char **pResult;
    int pRow;
    int pColumn;
    char *errmsg;

    ret = sqlite3_get_table(db, sql1, &pResult, &pRow, &pColumn, &errmsg);
    if (ret != SQLITE_OK)
    {
        printf("sqlite_get_table fail:%s\n", sqlite3_errmsg(db));
        sqlite3_free(errmsg); // 释放错误信息
        send_error(conn_fd, "res_login", "注册失败！服务异常");
        return;
    }

    if (pRow > 0)
    {
        send_error(conn_fd, "res_login", "注册失败！用户名已存在！");
        return;
    }

    // 构造 SQL 插入语句
    char sql2[256];
    snprintf(sql2, sizeof(sql2),
             "insert into users (username,password) values('%s','%s')",
             msg->data.username, msg->data.password);

    ret = sqlite3_exec(db, sql2, NULL, NULL, &errmsg);
    if (ret != SQLITE_OK)
    {
        printf("sqlite_get_table fail:%s\n", sqlite3_errmsg(db));
        sqlite3_free(errmsg); // 释放错误信息
        send_error(conn_fd, "res_register", "注册失败！服务异常");
        return;
    }
    Server_Msg smsg = {0};
    send_success(conn_fd, "res_register", "注册成功！", &smsg);
    return;
}

// 战绩查询函数
void sore_function(int conn_fd, Client_Msg *cmsg)
{
    sqlite3 *db;
    int ret;
    ret = sqlite3_open("db/game.db", &db); // 打开数据库
    if (ret != SQLITE_OK)
    {
        printf("sqlite open fail:%s\n", sqlite3_errmsg(db));
        return;
    }
    // 查询战绩
    char sql[256];
    snprintf(sql, sizeof(sql),
             "select * from users where username = '%s' ;",
             cmsg->name);
    printf("查战绩的sql:%s\n", sql);

    char **pResult;
    int pRow;
    int pColumn;
    char *errmsg;

    ret = sqlite3_get_table(db, sql, &pResult, &pRow, &pColumn, &errmsg);
    if (ret != SQLITE_OK)
    {
        printf("sqlite_get_table fail:%s\n", sqlite3_errmsg(db));
        sqlite3_free(errmsg); // 释放错误信息
        send_error(conn_fd, "res_sore", "查询失败，服务器异常!");
        return;
    }

    if (pRow == 0)
    {
        send_error(conn_fd, "res_sore", "用户不存在！");
        sqlite3_free_table(pResult);
        return;
    }

    if (pColumn < 6)
    {
        send_error(conn_fd, "res_sore", "数据列不足！");
        sqlite3_free_table(pResult);
        return;
    }

    Server_Msg smsg;
    int i = pColumn + 2;
    smsg.data.wins = atoi(pResult[i++]);
    smsg.data.loss = atoi(pResult[i++]);
    smsg.data.deuce = atoi(pResult[i++]);
    // 计算总场次和胜率
    int total = smsg.data.wins + smsg.data.loss + smsg.data.deuce;
    if (total == 0)
    {
        smsg.data.win_rate = 0.0;
    }
    else
    {
        smsg.data.win_rate = (float)smsg.data.wins / total;
    }
    send_success(conn_fd, "res_sore", "查询成功！", &smsg);

    return;
}

// 添加平局场次
void add_deuce(char *username)
{
    sqlite3 *db;
    int ret;
    char *errmsg;
    ret = sqlite3_open("db/game.db", &db); // 打开数据库
    if (ret != SQLITE_OK)
    {
        printf("sqlite open fail:%s\n", sqlite3_errmsg(db));
        return;
    }
    // 构造 SQL 插入语句
    char sql[256];
    snprintf(sql, sizeof(sql),
             "update users set deuce = deuce + 1 where username = '%s';",
             username);

    ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (ret != SQLITE_OK)
    {
        printf("sqlite_get_table fail:%s\n", sqlite3_errmsg(db));
        sqlite3_free(errmsg); // 释放错误信息
        return;
    }
    return;
}

// 添加胜利场次
void add_wins(char *username)
{
    sqlite3 *db;
    int ret;
    char *errmsg;
    ret = sqlite3_open("db/game.db", &db); // 打开数据库
    if (ret != SQLITE_OK)
    {
        printf("sqlite open fail:%s\n", sqlite3_errmsg(db));
        return;
    }
    // 构造 SQL 插入语句
    char sql[256];
    snprintf(sql, sizeof(sql),
             "update users set wins = wins + 1 where username = '%s';",
             username);

    ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (ret != SQLITE_OK)
    {
        printf("sqlite_get_table fail:%s\n", sqlite3_errmsg(db));
        sqlite3_free(errmsg); // 释放错误信息
        return;
    }
    return;
}

// 添加失败场次
void add_loss(char *username)
{
    sqlite3 *db;
    int ret;
    char *errmsg;
    ret = sqlite3_open("db/game.db", &db); // 打开数据库
    if (ret != SQLITE_OK)
    {
        printf("sqlite open fail:%s\n", sqlite3_errmsg(db));
        return;
    }
    // 构造 SQL 插入语句
    char sql[256];
    snprintf(sql, sizeof(sql),
             "update users set losses = losses + 1 where username = '%s';",
             username);

    ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (ret != SQLITE_OK)
    {
        printf("sqlite_get_table fail:%s\n", sqlite3_errmsg(db));
        sqlite3_free(errmsg); // 释放错误信息
        return;
    }
    return;
}
