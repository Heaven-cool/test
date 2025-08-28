CC = gcc
CFLAGS = -Wall -g -Iinclude -Isrc
LDFLAGS = -lsqlite3 -lpthread

ifeq ($(LOG), 1)
    CFLAGS += -DLOG
endif

# 所有源文件
SRCS = main.c \
       src/server_net.c \
       src/handler.c \
       src/game.c \
       src/db.c \
       src/json_utils.c \
       src/cJSON.c \
       src/server_utils.c

# 目标文件（.o）
OBJS = $(SRCS:.c=.o)

# 最终可执行文件名
TARGET = server_app

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# 自动生成 .o 文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)


