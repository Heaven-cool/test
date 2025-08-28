#include "server_net.h"
#include "handler.h"

//初始化服务器
int server_init(char *ip, unsigned short port, int backlog)
{
    //1、创建套结字 socket 
    int listen_fd;   //用来保存监听套结字
    listen_fd = socket(AF_INET, SOCK_STREAM, 0); 
    if(-1 == listen_fd)
    {
        printf("socket fail");
        return -1;
    }
    printf("socket success!\n"); 
    
    //初始化ip和端口、协议等
    struct sockaddr_in server_addr = {0}; //用来保存服务器ip等信息
    server_addr.sin_family     = AF_INET;   //IPV4协议
    server_addr.sin_port       = htons(port);  //端口范围49152-65535 

    server_addr.sin_addr.s_addr =( NULL == ip ) ? (htonl(INADDR_ANY )) : inet_addr(ip);
    
    //1优化： 端口地址重用
    int opt = 1; 
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //2、绑定 bind 
    int ret ;
    ret = bind(listen_fd, ( struct sockaddr * )&server_addr,  sizeof(server_addr)); 
    if(-1 == ret)
    {
        perror("bind fail");
        return -1;
    }
    printf("bind success!\n"); 
    //3、建立监听 listen  
    ret = listen(listen_fd, backlog);   //最大连接数 1024 // 并发时可用 
    if(-1 == ret)
    {
        perror("listen fail");
        return -1;
    }
    printf("监听中....\n"); 
    return listen_fd ;  //返回监听套结字  
}

// 循环监听客户端连接
void server_listen_loop(int listen_fd)
{
    int conn_fd;
    pthread_t  tid;  //保存线程id 
    int ret; 
    //处理客户端连接请求 
    struct sockaddr_in client_addr  = {0};

    while(1)
    {
        memset(&client_addr, 0, sizeof(client_addr));
        socklen_t client_len = sizeof(client_addr); 

        conn_fd = accept(listen_fd,  (struct sockaddr *) &client_addr,&client_len); 
        if(-1 == conn_fd)
        {
            perror("accept fail");
            return ;
        }
        printf("accept connfd=%d success\n",conn_fd -3);
        printf("client <ip=%s--port=%d>\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port)); 
        //创建子线程 专门用来通信 
        ret = pthread_create(&tid, NULL, connection_handler, (void *)& conn_fd); 
        if( ret != 0 )
        {
            perror("pthread_create fail");
            return ;
        }
        ret = pthread_detach(tid);   //线程分离   
    }
    close(listen_fd);  //父进程结束 关闭监听套结字 //意味着服务器结束 
}


