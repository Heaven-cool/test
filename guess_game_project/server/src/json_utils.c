#include "json_utils.h"
#include "common.h"

// 打包函数
int json_pack(char *buf, size_t buf_size, const Server_Msg *msg)
{
    int status;
    status = msg->status;
    if (status == 0)
    {
        cJSON *root = cJSON_CreateObject();
        if (!root)
        {
            fprintf(stderr, "cJSON_CreateObject failed: root=%p\n", root);
            return -1;
        }
        // 添加字段
        cJSON_AddStringToObject(root, "type", msg->type);
        cJSON_AddNumberToObject(root, "status", 0);
        cJSON_AddStringToObject(root, "message", msg->message);
        cJSON_AddItemToObject(root, "data", NULL);

        // 转换为无格式字符串
        char *json_str = cJSON_PrintUnformatted(root);
        if (!json_str)
        {
            cJSON_Delete(root);
            return -1;
        }

        // 拷贝到用户传入的 buf
        strncpy(buf, json_str, buf_size - 1);
        buf[buf_size - 1] = '\0'; // 确保结尾

        // 清理内存
        cJSON_Delete(root);
        free(json_str);
        return 0;
    }
    else if (status == 1)
    {
        cJSON *root = cJSON_CreateObject();
        cJSON *data = cJSON_CreateObject();
        // 创建 JSON 数组
        cJSON *wait_array = cJSON_CreateArray();

        // perror("57 行cJSON_CreateObject fail!");
        if (!root || !data)
        {
            fprintf(stderr, "cJSON_CreateObject failed: root=%p, data=%p\n", root, data);
            return -1;
        }

        for (int i = 0; i < 3; ++i)
        {
            if (strlen(msg->data.wait_player[i]) > 0)
            {
                cJSON_AddItemToArray(wait_array, cJSON_CreateString(msg->data.wait_player[i]));
            }
        }

        // 添加字段
        cJSON_AddStringToObject(root, "type", msg->type);
        cJSON_AddNumberToObject(root, "status", 1);
        cJSON_AddStringToObject(root, "message", msg->message);
        cJSON_AddStringToObject(data, "username", msg->data.username);
        cJSON_AddNumberToObject(data, "number", msg->data.number);
        cJSON_AddNumberToObject(data, "wins", msg->data.wins);
        cJSON_AddNumberToObject(data, "loss", msg->data.loss);
        cJSON_AddNumberToObject(data, "deuce", msg->data.deuce);
        cJSON_AddItemToObject(data, "wait_player", wait_array);
        cJSON_AddNumberToObject(data, "win_rate", msg->data.win_rate);
        cJSON_AddItemToObject(root, "data", data);

        // 转换为无格式字符串
        char *json_str = cJSON_PrintUnformatted(root);
        if (!json_str)
        {
            cJSON_Delete(root);
            return -1;
        }

        // 拷贝到用户传入的 buf
        strncpy(buf, json_str, buf_size - 1);
        buf[buf_size - 1] = '\0'; // 确保结尾

        // 清理内存
        cJSON_Delete(root);
        free(json_str);
        return 0;
    }

    return 0;
}

// 解包函数
int json_unpack(char *buf, char *type, Client_Msg *msg)
{
    cJSON *json = cJSON_Parse(buf);
    if (!json)
    {
        fprintf(stderr, "json_unpack: JSON 解析失败！\n");
        return -1;
    }

    cJSON *msg_type = cJSON_GetObjectItem(json, "type");
    cJSON *name = cJSON_GetObjectItem(json, "name");
    cJSON *data = cJSON_GetObjectItem(json, "data");

    // type 必须存在
    if (!msg_type || !cJSON_IsString(msg_type))
    {
        fprintf(stderr, "json_unpack: 缺少或错误的 type 字段\n");
        cJSON_Delete(json);
        return -1;
    }

    // 拷贝 type
    strncpy(type, msg_type->valuestring, sizeof(msg->type) - 1);
    type[sizeof(msg->type) - 1] = '\0';
    strncpy(msg->type, msg_type->valuestring, sizeof(msg->type) - 1);
    msg->type[sizeof(msg->type) - 1] = '\0';

    // 拷贝 name（可为空）
    if (name && cJSON_IsString(name))
        strncpy(msg->name, name->valuestring, sizeof(msg->name) - 1);
    else
        msg->name[0] = '\0';

    // 如果 data 不存在，就初始化为空并返回
    if (!data || !cJSON_IsObject(data))
    {
        memset(&msg->data, 0, sizeof(msg->data));
        cJSON_Delete(json);
        return 0;
    }

    // 解 data 中的字段
    cJSON *username = cJSON_GetObjectItem(data, "username");
    cJSON *password = cJSON_GetObjectItem(data, "password");
    cJSON *number = cJSON_GetObjectItem(data, "number");

    // 拷贝 username（可为空）
    if (username && cJSON_IsString(username))
        strncpy(msg->data.username, username->valuestring, sizeof(msg->data.username) - 1);
    else
        msg->data.username[0] = '\0';

    // 拷贝 password（可为空）
    if (password && cJSON_IsString(password))
        strncpy(msg->data.password, password->valuestring, sizeof(msg->data.password) - 1);
    else
        msg->data.password[0] = '\0';

    // 拷贝 number（可为空）
    if (number && cJSON_IsNumber(number))
        msg->data.number = number->valueint;
    else
        msg->data.number = -1; // 默认值

    cJSON_Delete(json);
    return 0;
}
