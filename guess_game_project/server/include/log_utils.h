#ifndef LOG_UTILS_H
#define LOG_UTILS_H

#ifdef LOG //  使用 LOG 作为开关
#define LOG_MSG(msg, ...) \
    fprintf(stderr, "[LOG] %s:%d: " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_MSG(msg, ...) // 日志关闭，什么也不输出
#endif

#endif // LOG_UTILS_H
