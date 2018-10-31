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
#define SIZE 1024

int main(int argc, char *argv[])
{
    pid_t pid;
    int sockfd,confd;
    char buffer[SIZE],buf[SIZE]; 
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    struct hostent *host;
    short port;
    char *name;  
    //四个参数
    if(argc!=4) 
    { 
        fprintf(stderr,"Usage:%s hostname \a\n",argv[0]); 
        exit(1); 
    } 
       //使用hostname查询host 名字 
    if((host=gethostbyname(argv[1]))==NULL) 
    { 
        fprintf(stderr,"Gethostname error\n"); 
        exit(1); 
    } 
        port=atoi(argv[2]);
        name=argv[3];
       /*客户程序开始建立 sockfd描述符 */ 
    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) 
    { 
        fprintf(stderr,"Socket Error:%s\a\n",strerror(errno)); 
        exit(1); 
    } else{
        printf("Socket successful!\n");
    }
        /*客户程序填充服务端的资料 */ 
    bzero(&server_addr,sizeof(server_addr)); // 初始化,置0
    server_addr.sin_family=AF_INET;          // IPV4
    server_addr.sin_port=htons(port);  // (将本机器上的short数据转化为网络上的short数据)端口号
    server_addr.sin_addr=*((struct in_addr *)host->h_addr); // IP地址
    /* 客户程序发起连接请求 */ 
    if(confd=connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1) 
    { 
        fprintf(stderr,"Connect Error:%s\a\n",strerror(errno)); 
        exit(1); 
    }else{
        printf("Connect successful!\n");
    }
    /*将客户端的名字发送到服务器端*/
    send(sockfd,name,20,0);
     /*创建子进程，进行读写操作*/
    pid = fork();//创建子进程
     while(1)
    {
       /*父进程用于发送信息*/
           if(pid > 0)
           {       
          /*时间函数*/
           struct tm *p;
           time(&timep);
           p = localtime(&timep);
           strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", p);
           /*输出时间和客户端的名字*/
           strcat(buffer," \n\t昵称 ->");
           strcat(buffer,name);
           strcat(buffer,":\n\t\t  ");
           memset(buf,0,SIZE);
           fgets(buf,SIZE,stdin);
       /*对客户端程序进行管理*/
           if(strncmp("e",buf,1)==0)
           {
             printf("该客户端下线...\n");
             strcat(buffer,"退出聊天室！");
             if((send(sockfd,buffer,SIZE,0)) <= 0)
             {
               perror("error send");
             }
             close(sockfd);
             sockfd = -1;
             exit(0);
           }else 
        {
            strncat(buffer,buf,strlen(buf)-1);
            strcat(buffer,"\n");              
          if((send(sockfd,buffer,SIZE,0)) <= 0)
            {
                 perror("send");
            }
           }
        }     
        else if(pid == 0)
        {
             /*子进程用于接收信息*/
               memset(buffer,0,SIZE);
               if(sockfd > 0)
               {
             if((recv(sockfd,buffer,SIZE,0)) <= 0)
               {
                  close(sockfd);
                  exit(1);
               }
               printf("%s\n",buffer);
            }
        }
    }   close(sockfd);
    return 0;    
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
#define PORT 4395
#define SIZE 1024
#define SIZE_SHMADD 2048
#define BACKLOG 3
int sockfd;
int fd[BACKLOG];
int i=0;
/*********套接字描述符*******/
int get_sockfd()
{    struct sockaddr_in server_addr; if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)  {    fprintf(stderr,"Socket error:%s\n\a",strerror(errno));          exit(1); }else{
        printf("Socket successful!\n"); }     /*sockaddr结构 */ 
    bzero(&server_addr,sizeof(struct sockaddr_in)); 
    server_addr.sin_family=AF_INET;                
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY); 
    server_addr.sin_port=htons(PORT);  
    /*绑定服务器的ip和服务器端口号*/
    if(bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1)     
    {      fprintf(stderr,"Bind error:%s\n\a",strerror(errno));       
          exit(1);     
    } else{printf("Bind successful!\n");    }  
     /* 设置允许连接的最大客户端数 */     
     if(listen(sockfd,BACKLOG)==-1)     
     {    fprintf(stderr,"Listen error:%s\n\a",strerror(errno)); exit(1);  } else{
          printf("Listening.....\n"); } 
     return sockfd;
}

/*创建共享存储区*/
int shmid_create()
{    int shmid;     if((shmid = shmget(IPC_PRIVATE,SIZE_SHMADD,0777)) < 0)       { perror("shmid error!"); exit(1); }
    Else  printf("shmid success!\n");
    return shmid;
}
int main(int argc, char *argv[]) {   char shmadd_buffer[SIZE_SHMADD],buffer[SIZE];      struct sockaddr_in client_addr;  
    int sin_size;   
    pid_t ppid,pid;     int new_fd; 
    int shmid;
    char *shmadd;
    /***********共享内存**************/
    shmid = shmid_create();
    //映射共享内存
    shmadd = shmat(shmid, 0, 0);
    /*****创建套接字描述符***********/ 
    int sockfd = get_sockfd();
    /*循环接收客户端*/
    while(1)
    {   /* 服务器阻塞,直到客户程序建立连接 */         
      sin_size=sizeof(struct sockaddr_in);        
      if((new_fd=accept(sockfd,(struct sockaddr *)(&client_addr),&sin_size))==-1)         
      { fprintf(stderr,"Accept error:%s\n\a",strerror(errno));  exit1);   }else{printf("Accept successful!\n"); } 
     fd[i++] = new_fd;    
     printf("\n已连接了客户端%d : %s:%d \n",i , inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
       /*把界面发送给客户端*/
     memset(buffer,0,SIZE);
     strcpy(buffer,"\n——————————————————Welecom come char ———————————————————————\n");
     send(new_fd,buffer,SIZE,0);
    //创建子进程客户端
    ppid = fork();  if(ppid == 0)
    {
        //将加入的新客户发送给所有在线的客户端/
       recv(new_fd,buffer,SIZE,0);
       strcat( buffer," 进入了聊天室....");  
       for(i=0;i<BACKLOG;i++)
       {  
        if(fd[i]!=-1)
         {  
              send(fd[i],buffer,strlen(buffer),0);  
         }  
            }   
        //创建子进程进行读写操作/
      pid = fork();    
       while(1)
       {
         if(pid > 0)
         {
           //父进程用于接收信息/
           memset(buffer,0,SIZE);
           if((recv(new_fd,buffer,SIZE,0)) <= 0)
           {
              close(new_fd);
              exit(1); }
            memset(shmadd, 0, SIZE_SHMADD);
            strncpy(shmadd, buffer, SIZE_SHMADD);//将缓存区的客户端信息放入共享内存里
            printf(" %s\n",buffer);
         }
         if(pid == 0)
         {
           //子进程用于发送信息/
           sleep(1);//先执行父进程
           if(strcmp(shmadd_buffer,shmadd) != 0)
           {
              strcpy(shmadd_buffer,shmadd);
              if(new_fd  > 0)
              {
                 if(send(new_fd,shmadd,strlen(shmadd),0) == -1)
                 {
                   perror("send");
                 }                
                 memset(shmadd, 0, SIZE_SHMADD);
                 strcpy(shmadd,shmadd_buffer);                
              }
           }
         }
      }
     } 
   }    free(buffer);
   close(new_fd);
   close(sockfd);
   return 0;
}
