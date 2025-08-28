
# Makefile
CC = gcc
CFLAGS = -Wall -g -Iinclude -Isrc
LDFLAGS = -lsqlite3 -lpthread

ifeq ($(LOG), 1)
    CFLAGS += -DLOG
endif

# 源文件列表
SRCS = main.c \
       src/client_net.c \
       src/client_ui.c \
       src/json_utils.c \
       src/client_utils.c \
       src/cJSON.c

# 对应的 .o 文件
OBJS = $(SRCS:.c=.o)

# 最终可执行文件
TARGET = client_app

# 默认目标
.PHONY: all clean

all: $(TARGET)

# 链接
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# 单个源文件编译规则
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理
clean:
	rm -f $(OBJS) $(TARGET)


