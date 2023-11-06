/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2019
#       All rights reserved
#
#       @author       :lvzhe
#       @mail         :1750895316@qq.com
#       @file         :Udp_Subcontract.c
#       @date         :2020/04/24 14:30
#       @algorithm    :
=============================================================================*/

#include "subcontract.h"
#include <sys/time.h>
#include <errno.h>
#include <signal.h>
#define RECV_MAX   1044
int Socket1_fd, Socket2_fd;
char* SERVER1_PORT="6600"; //server1's port and ip adress
char* SERVER2_PORT="6601";//sever2's port and ip adress
int register_num=0;
int flag=NO_CONNECT;
int client1_timeout=0;//记录客户端1是否超时
int client2_timeout=0;//记录客户端2是否超时
struct sockaddr_in client1Adrr;
struct sockaddr_in client2Adrr;
char SERVERBUFF1[RECV_MAX];
char SERVERBUFF2[RECV_MAX];
void UdpServer1_th()
{
    struct sockaddr_in serveraddr;
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(SERVER1_PORT));
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if ( (Socket1_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("get socket error");
        exit(-1);
    }
     //port bind to server
    if (bind(Socket1_fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
        perror("bind error");
        close(Socket1_fd);
        exit(1);
    }
    int recv;
    memset(&client1Adrr,0,sizeof(client1Adrr));
    socklen_t len = sizeof(client1Adrr);
    recvfrom(Socket1_fd,SERVERBUFF1,RECV_MAX,0,(struct sockaddr*)&client1Adrr, &len);
    printf("客户端ip:%s 端口:%u 登录服务器\n", 
            inet_ntoa(client1Adrr.sin_addr), 
            ntohs(client1Adrr.sin_port));
    memset(SERVERBUFF1,0,RECV_MAX);
    register_num++;
    while(1){
        if(flag==CONNECT_OK)break;
    }
    while(1)
    {
       recv=recvfrom(Socket1_fd,SERVERBUFF1,RECV_MAX,0,(struct sockaddr*)&client1Adrr, &len);
       if(recv>0){
           client1_timeout=0;//没有超时
          sendto(Socket2_fd,SERVERBUFF1,recv,0,
                (struct sockaddr *)&client2Adrr,sizeof(client2Adrr));  //等待双方登录服务器后往对方发送数据
             memset(SERVERBUFF1,0,recv);
       }else if(recv<0)//接收超时
       {
           //printf("Socket1_fd等待超时\n");
           client1_timeout=1;//客户端1接收数据超时
       }

    if(flag==NO_CONNECT)
    {
        printf("结束客户端1的登录\n");
        close(Socket1_fd);
        break;
    }


    }

}


void UdpServer2_th()
{
    struct sockaddr_in serveraddr;
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(SERVER2_PORT));
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if ( (Socket2_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("get socket error");
        exit(-1);
    }
     //port bind to server
    if (bind(Socket2_fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
        perror("bind error");
        close(Socket2_fd);
        exit(1);
    }
    memset(&client2Adrr,0,sizeof(client2Adrr));
    socklen_t len = sizeof(client2Adrr);
    recvfrom(Socket2_fd,SERVERBUFF2,RECV_MAX,0,(struct sockaddr*)&client2Adrr, &len);
    printf("客户端ip:%s 端口:%u 登录服务器\n", 
            inet_ntoa(client2Adrr.sin_addr), 
            ntohs(client2Adrr.sin_port));
     memset(SERVERBUFF2,0,RECV_MAX);
    register_num++;
    int recv;
    while(1){
        if(flag==CONNECT_OK)break;
    }
    while(1)
    {
        recv=recvfrom(Socket2_fd,SERVERBUFF2,RECV_MAX,0,(struct sockaddr*)&client2Adrr, &len);
       if(recv>0){
        client2_timeout=0;//没有超时
          sendto(Socket1_fd,SERVERBUFF2,recv,0,
                (struct sockaddr *)&client1Adrr,sizeof(client1Adrr));  //等待双方登录服务器后往对方发送数据
             memset(SERVERBUFF2,0,recv);
       }
        else if(recv<0)//接收超时
       {
           //printf("Socket2_fd等待超时\n");
            client2_timeout=1;//客户端2接收数据超时
       }
    
    if(flag==NO_CONNECT)
    {
        printf("结束客户端2的登录\n");
        close(Socket2_fd);
        break;
    }

    }
    

}

void wait_server()
{   
    while(1)
    {
        if((client2_timeout==1)&&(client1_timeout==1))//双方都没有接收数据,结束客户端登录
        {
            flag=NO_CONNECT;
            break;
        }
    }
}

void *UdpServer1Thread(void *arg)
{
    UdpServer1_th();
    return (void*)0;

}

void *UdpServer2Thread(void *arg)
{
    UdpServer2_th();
    return (void*)0;
}

//信号处理函数,当服务结束收回资源
void signal_handler(int signao)
{
    if(signao==SIGINT)
    {
        printf("server over\n");
        exit(1);
    }
}

void *WaitServerThread(void *arg)
{
    wait_server();
    return (void*)0;
}

int main()
{   
    if(signal(SIGINT,signal_handler)==SIG_ERR)
    {
        perror("signal sigint error");
        exit(-1);
    }
    freopen("jpeg.log","w",stdout);
    setvbuf(stdout,NULL,_IONBF,0);
    printf("Welcome! This is a UDP server.\n");
                 //创建两个子线程负责两个客户端
     pthread_t server1ThreadId;
     pthread_t server2ThreadId;
     pthread_t waitThreadId;//等待线程
   struct timeval tv_out;
    tv_out.tv_sec = 30;//等待60秒
    tv_out.tv_usec = 0;
    printf("服务器端口:%s客户端1用来接收和发送数据\n",SERVER1_PORT);
    printf("服务器端口:%s客户端2用来发送和接收数据\n",SERVER2_PORT);
    while(1){
    if(flag==NO_CONNECT){
        printf("等待连接\n");
        client2_timeout=0;
        client1_timeout=0;
        pthread_create(&server1ThreadId,NULL,UdpServer1Thread,0);
        pthread_create(&server2ThreadId,NULL,UdpServer2Thread,0);
        flag=WAIT_CONNECT;
    }
    else if(flag==WAIT_CONNECT)
    {
        if(register_num==2)
        {
        printf("双方客户端登录成功\n");
        printf("在一分钟之内没有接收数据将会注销登录\n");
      /* sprintf((char*)SERVERBUFF1,"ready");
        sprintf((char*)SERVERBUFF2,"ready");
        sendto(Socket1_fd,SERVERBUFF1,strlen(SERVERBUFF1),0,
               (struct sockaddr *)&client1Adrr,sizeof(client1Adrr));
        sendto(Socket2_fd,SERVERBUFF2,strlen(SERVERBUFF2),0,
                (struct sockaddr *)&client2Adrr,sizeof(client2Adrr));
	*/ 
         memset(SERVERBUFF1,0,RECV_MAX);
        memset(SERVERBUFF2,0,RECV_MAX); 
        pthread_create(&waitThreadId,NULL,WaitServerThread,0);//创建等待线程
       setsockopt(Socket1_fd,SOL_SOCKET,SO_RCVTIMEO,&tv_out, sizeof(tv_out)); //设置套接字接收超时
       setsockopt(Socket2_fd,SOL_SOCKET,SO_RCVTIMEO,&tv_out, sizeof(tv_out)); 
        flag=CONNECT_OK;//成功连接
        register_num=0;
        pthread_join(waitThreadId,NULL);
        pthread_join(server1ThreadId, NULL);
        pthread_join(server2ThreadId, NULL);//用于阻塞该线程
        }
    }
    }


    return 0;
}
