#include<stdio.h>
#include<netinet/in.h>  
#include<sys/socket.h> 
#include<sys/types.h> #include<string.h>
#include<stdlib.h>
#include<netdb.h>
#include<unistd.h>
#include<signal.h>
#include<errno.h>
#include<time.h>
#define LISTENQ 5
#define MAXLINE 512
#define MAXMEM 10
#define NAMELEN 20

int listenfd,connfd[MAXMEM];//分别记录服务器端的套接字与连接的多个客户端的套接字

void quit();//服务器关闭函数
void rcv_snd(int n);//服务器接收并转发消息函数

int main()
{
    pthread_t thread;
    struct sockaddr_in servaddr,cliaddr;
    socklen_t len;
    time_t ticks;
    char buff[MAXLINE];

//调用socket函数创建服务器端的套接字
    printf("Socket...\n");
    listenfd=socket(AF_INET,SOCK_STREAM,0);
    if(listenfd<0)
    {
        printf("Socket created failed.\n");
        return -1;
    }

//调用bind函数使得服务器端的套接字与地址实现绑定
    printf("Bind...\n");
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(6666);
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
    {
        printf("Bind failed.\n");
        return -1;
    }

//调用listen函数，将一个主动连接套接字变为被动的倾听套接字
//在此过程中完成tcp的三次握手连接
    printf("listening...\n");
    listen(listenfd,LISTENQ);

//创建一个线程，对服务器程序进行管理（关闭）
    pthread_create(&thread,NULL,(void*)(&quit),NULL);

//记录空闲的客户端的套接字描述符（-1为空闲）
    int i=0;
    for(i=0;i
    {
        connfd[i]=-1;
    }

    while(1)
    {
        len=sizeof(cliaddr);
        for(i=0;i
        {
            if(connfd[i]==-1)
            {
                break;
            }
        }

//调用accept从listen接受的连接队列中取得一个连接
        connfd[i]=accept(listenfd,(struct sockaddr*)&cliaddr,&len);

        ticks=time(NULL);
        sprintf(buff,"% .24s \r \n",ctime(&ticks));
        printf("%s Connect from: %s,port %d\n\n",buff,inet_ntoa(cliaddr.sin_addr.s_addr),ntohs(cliaddr.sin_port));

//针对当前套接字创建一个线程，对当前套接字的消息进行处理
        pthread_create(malloc(sizeof(pthread_t)),NULL,(void*)(&rcv_snd),(void*)i);

    }
    return 0;
}

void quit()
{
    char msg[10];
    while(1)
    {
        scanf("%s",msg);
        if(strcmp("quit",msg)==0)
        {
            printf("Byebye...\n");
            close(listenfd);
            exit(0);
        }
    }
}

void rcv_snd(int n)
{
    char* ask="Your name please：";
    char buff[MAXLINE];
    char buff1[MAXLINE];
    char buff2[MAXLINE];
    char name[NAMELEN];
    time_t ticks;
    int i=0;
    int retval;

//获取此进程对应的套接字用户的名字
    write(connfd[n],ask,strlen(ask));
    int len;
    len=read(connfd[n],name,NAMELEN);
     if(len>0)
     {
         name[len]=0;
     }

//把当前用户的加入告知所有用户
    strcpy(buff,name);
    strcat(buff,"\tjoin in\0");
    for(i=0;i
    {
        if(connfd[i]!=-1)
        {
            write(connfd[i],buff,strlen(buff));
        }
    }

//接受当前用户的信息并将其转发给所有的用户
    while(1)
    {
        if((len=read(connfd[n],buff1,MAXLINE))>0)
        {
            buff1[len]=0;

//当当前用户的输入信息为“bye”时，当前用户退出
             if(strcmp("bye",buff)==0)
             {
                 close(connfd[n]);
                 connfd[n]=-1;
                 pthread_exit(&retval);
             }

             ticks=time(NULL);
             sprintf(buff2,"%.24s\r\n",ctime(&ticks));
             write(connfd,buff2,strlen(buff2));

             strcpy(buff,name);
             strcat(buff,"\t");
             strcat(buff,buff2);
             strcat(buff,buff1);

            for(i=0;i
            {
                 if(connfd[i]!=-1)
                 {
                      write(connfd[i],buff,strlen(buff));
                 }
            }
        }

    }
}


#include<stdio.h>   
#include<stdlib.h>
#include<sys/types.h> 
#include<sys/stat.h>
#include<netinet/in.h>  
#include<sys/socket.h> 
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<sys/ipc.h>
#include<errno.h>
#include<sys/shm.h>
#include<time.h>
#include<pthread.h>
#define MAXLINE 512
#define NAMELEN 20
#define PORT 6666
#define HOST_ADDR "127.0.0.1"

int sockfd;
void snd();

int main()
{
    pthread_t thread;
    struct sockaddr_in servaddr;

//调用socket函数创建客户端的套接字
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0)
    {
        printf("Socket create failed\n");
        return -1;
    }

//初始化服务器端的地址
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(PORT);
    if(inet_aton(HOST_ADDR,&servaddr.sin_addr)<0)
    {
        printf("inet_aton error.\n");
        return -1;
    }

//调用connect函数实现与服务器端建立连接
    printf("Connecting...\n");
    if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
    {
        printf("Connect server failed.\n");
        return  -1;
    }

//从此处开始程序分做两个线程

//创建发送消息的线程，调用了发送消息的函数snd
    pthread_create(&thread,NULL,(void*)(&snd),NULL);

//从此处开始向下为接收消息的线程
    char buff[MAXLINE];
    int len;

    while(1)
    {
        if((len=read(sockfd,buff,MAXLINE))>0)
        {
            buff[len]=0;
            printf("\n%s\n\n",buff);
        }
    }

    return 0;
}

//发送消息的函数snd
void snd()
{
    char name[NAMELEN];
    char buff[MAXLINE];
    gets(name);
    write(sockfd,name,strlen(name));
    while(1)
    {
        gets(buff);
        write(sockfd,buff,strlen(buff));
        if(strcmp("bye",buff)==0)
        {
            exit(0);
        }
    }
}
