#include "json_utils.h"
#include "common.h"
#include "log_utils.h"

// 打包函数
int json_pack(char *buf, size_t buf_size, const Client_Msg *msg)
{
    LOG_MSG("进入打包！\n");
    cJSON *root = cJSON_CreateObject();
    cJSON *data = cJSON_CreateObject();

    if (!root || !data)
    {
        fprintf(stderr, "cJSON_CreateObject failed: root=%p, data=%p\n", root, data);
        return -1;
    }

    // 添加字段
    cJSON_AddStringToObject(root, "type", msg->type);
    cJSON_AddStringToObject(root, "name", msg->name);
    cJSON_AddStringToObject(data, "username", msg->data.username);
    cJSON_AddStringToObject(data, "password", msg->data.password);
    cJSON_AddNumberToObject(data, "number", msg->data.number);
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

// 解包函数
int json_unpack(char *buf, char *type, Server_Msg *msg)
{
    cJSON *json = cJSON_Parse(buf);
    if (!json)
    {
        fprintf(stderr, "json_unpack: JSON 解析失败！\n");
        return -1;
    }

    cJSON *msg_type = cJSON_GetObjectItem(json, "type");
    cJSON *status = cJSON_GetObjectItem(json, "status");
    cJSON *message = cJSON_GetObjectItem(json, "message");
    cJSON *data = cJSON_GetObjectItem(json, "data");

    // 必须字段 type
    if (!msg_type || !cJSON_IsString(msg_type))
    {
        fprintf(stderr, "json_unpack: 缺少 type 字段！\n");
        cJSON_Delete(json);
        return -1;
    }

    // type 赋值
    strncpy(type, msg_type->valuestring, sizeof(msg->type) - 1);
    type[sizeof(msg->type) - 1] = '\0';
    strncpy(msg->type, msg_type->valuestring, sizeof(msg->type) - 1);
    msg->type[sizeof(msg->type) - 1] = '\0';

    // status （可选）
    if (status && cJSON_IsNumber(status))
        msg->status = status->valueint;
    else
        msg->status = 0; // 默认值

    // message （可选）
    if (message && cJSON_IsString(message))
        strncpy(msg->message, message->valuestring, sizeof(msg->message) - 1);
    else
        msg->message[0] = '\0';

    // 如果 data 存在且是对象，提取字段
    if (data && cJSON_IsObject(data))
    {
        cJSON *username = cJSON_GetObjectItem(data, "username");
        cJSON *wins = cJSON_GetObjectItem(data, "wins");
        cJSON *loss = cJSON_GetObjectItem(data, "loss");
        cJSON *deuce = cJSON_GetObjectItem(data, "deuce");
        cJSON *win_rate = cJSON_GetObjectItem(data, "win_rate");
        cJSON *number = cJSON_GetObjectItem(data, "number");

        if (username && cJSON_IsString(username))
            strncpy(msg->data.username, username->valuestring, sizeof(msg->data.username) - 1);
        else
            msg->data.username[0] = '\0';

        msg->data.wins = (wins && cJSON_IsNumber(wins)) ? wins->valueint : -1;
        msg->data.loss = (loss && cJSON_IsNumber(loss)) ? loss->valueint : -1;
        msg->data.deuce = (deuce && cJSON_IsNumber(deuce)) ? deuce->valueint : -1;

        // win_rate 如果是数字（float），建议用 valueDouble
        msg->data.win_rate = (win_rate && cJSON_IsNumber(win_rate)) ? win_rate->valuedouble : -1.0;

        msg->data.number = (number && cJSON_IsNumber(number)) ? number->valueint : -1;
        // 解析 wait_player 字符串数组
        cJSON *wait_array = cJSON_GetObjectItem(data, "wait_player");
        if (wait_array && cJSON_IsArray(wait_array))
        {
            int len = cJSON_GetArraySize(wait_array);
            for (int i = 0; i < 3; ++i)
            {
                cJSON *item = (i < len) ? cJSON_GetArrayItem(wait_array, i) : NULL;
                if (item && cJSON_IsString(item))
                {
                    strncpy(msg->data.wait_player[i], item->valuestring, sizeof(msg->data.wait_player[i]) - 1);
                    msg->data.wait_player[i][sizeof(msg->data.wait_player[i]) - 1] = '\0';
                }
                else
                {
                    msg->data.wait_player[i][0] = '\0'; // 没有或无效则清空
                }
            }
        }
        else
        {
            // 如果 wait_player 字段不存在或不是数组，就清空所有
            for (int i = 0; i < 3; ++i)
            {
                msg->data.wait_player[i][0] = '\0';
            }
        }
    }
    else
    {
        // 若 data 不存在，初始化默认值
        msg->data.username[0] = '\0';
        msg->data.wins = -1;
        msg->data.loss = -1;
        msg->data.deuce = -1;
        msg->data.win_rate = -1.0;
        msg->data.number = -1;
    }

    cJSON_Delete(json);
    return 0;
}
