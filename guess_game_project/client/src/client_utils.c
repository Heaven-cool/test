#include "client_utils.h"

// 校验输入选项,必须为1位整数
int check_input_option(char *buf)
{
    if (buf[1] == '\n')
    {
        buf[1] = '\0';
    }
    int len = strlen(buf);
    if (len != 1) // 非 1 位选项参数
    {
        LOG_MSG("进入1");
        return -1;
    }
    char opt = buf[0];
    if (tid != 0 && opt == '0')
    {
        LOG_MSG("进入2");
        return 0;
    }
    if (!(opt >= '0' && opt <= '9'))
    {
        LOG_MSG("进入3");
        return -1;
    }
    if (opt == '0' && tid == 0)
    {
        LOG_MSG("进入3");
        return -1;
    }

    return 0;
}

// 校验用户名，长度须为3-10，英文字母和数字组成
int check_username(char *buf)
{
    int len = strlen(buf);
    if (len < 3 || len > 10)
    {
        return -1;
    }

    for (int i = 0; i < len; i++)
    {
        if (!isalnum(buf[i])) // 如果不是字母或数字
        {
            return -1;
        }
    }

    return 0; // 合法
}

// 校验密码，长度须为6-15，英文字母和数字组成
int check_password(char *buf)
{
    int len = strlen(buf);
    if (len < 6 || len > 15)
    {
        return -1;
    }

    for (int i = 0; i < len; i++)
    {
        if (!isalnum(buf[i]))
        {
            return -1;
        }
    }

    return 0; // 合法
}

// 校验输入数据,必须为int范围的整数
int check_input_number(char *buf)
{
    if (buf == NULL || *buf == '\0')
        return -1; // 空指针或空字符串
    if (buf[strlen(buf) - 1] == '\n')
        buf[strlen(buf) - 1] = '\0';
    // 跳过前导空格
    while (isspace(*buf))
        buf++;

    if (!isdigit(*buf))
        return -1; // 第一个有效字符必须是数字

    // 检查每个字符是否都是数字
    const char *p = buf;
    while (*p)
    {
        if (!isdigit(*p))
            return -1;
        p++;
    }

    // 转换为整数并检查是否超出 int 范围
    long val = strtol(buf, NULL, 10);
    if (val > INT_MAX || val < INT_MIN)
        return -1;

    return 0; // 是合法整数
}