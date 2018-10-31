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
    //�ĸ�����
    if(argc!=4) 
    { 
        fprintf(stderr,"Usage:%s hostname \a\n",argv[0]); 
        exit(1); 
    } 
       //ʹ��hostname��ѯhost ���� 
    if((host=gethostbyname(argv[1]))==NULL) 
    { 
        fprintf(stderr,"Gethostname error\n"); 
        exit(1); 
    } 
        port=atoi(argv[2]);
        name=argv[3];
       /*�ͻ�����ʼ���� sockfd������ */ 
    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) 
    { 
        fprintf(stderr,"Socket Error:%s\a\n",strerror(errno)); 
        exit(1); 
    } else{
        printf("Socket successful!\n");
    }
        /*�ͻ�����������˵����� */ 
    bzero(&server_addr,sizeof(server_addr)); // ��ʼ��,��0
    server_addr.sin_family=AF_INET;          // IPV4
    server_addr.sin_port=htons(port);  // (���������ϵ�short����ת��Ϊ�����ϵ�short����)�˿ں�
    server_addr.sin_addr=*((struct in_addr *)host->h_addr); // IP��ַ
    /* �ͻ��������������� */ 
    if(confd=connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1) 
    { 
        fprintf(stderr,"Connect Error:%s\a\n",strerror(errno)); 
        exit(1); 
    }else{
        printf("Connect successful!\n");
    }
    /*���ͻ��˵����ַ��͵���������*/
    send(sockfd,name,20,0);
     /*�����ӽ��̣����ж�д����*/
    pid = fork();//�����ӽ���
     while(1)
    {
       /*���������ڷ�����Ϣ*/
           if(pid > 0)
           {       
          /*ʱ�亯��*/
           struct tm *p;
           time(&timep);
           p = localtime(&timep);
           strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", p);
           /*���ʱ��Ϳͻ��˵�����*/
           strcat(buffer," \n\t�ǳ� ->");
           strcat(buffer,name);
           strcat(buffer,":\n\t\t  ");
           memset(buf,0,SIZE);
           fgets(buf,SIZE,stdin);
       /*�Կͻ��˳�����й���*/
           if(strncmp("e",buf,1)==0)
           {
             printf("�ÿͻ�������...\n");
             strcat(buffer,"�˳������ң�");
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
             /*�ӽ������ڽ�����Ϣ*/
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
/*********�׽���������*******/
int get_sockfd()
{    struct sockaddr_in server_addr; if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)  {    fprintf(stderr,"Socket error:%s\n\a",strerror(errno));          exit(1); }else{
        printf("Socket successful!\n"); }     /*sockaddr�ṹ */ 
    bzero(&server_addr,sizeof(struct sockaddr_in)); 
    server_addr.sin_family=AF_INET;                
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY); 
    server_addr.sin_port=htons(PORT);  
    /*�󶨷�������ip�ͷ������˿ں�*/
    if(bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1)     
    {      fprintf(stderr,"Bind error:%s\n\a",strerror(errno));       
          exit(1);     
    } else{printf("Bind successful!\n");    }  
     /* �����������ӵ����ͻ����� */     
     if(listen(sockfd,BACKLOG)==-1)     
     {    fprintf(stderr,"Listen error:%s\n\a",strerror(errno)); exit(1);  } else{
          printf("Listening.....\n"); } 
     return sockfd;
}

/*��������洢��*/
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
    /***********�����ڴ�**************/
    shmid = shmid_create();
    //ӳ�乲���ڴ�
    shmadd = shmat(shmid, 0, 0);
    /*****�����׽���������***********/ 
    int sockfd = get_sockfd();
    /*ѭ�����տͻ���*/
    while(1)
    {   /* ����������,ֱ���ͻ����������� */         
      sin_size=sizeof(struct sockaddr_in);        
      if((new_fd=accept(sockfd,(struct sockaddr *)(&client_addr),&sin_size))==-1)         
      { fprintf(stderr,"Accept error:%s\n\a",strerror(errno));  exit1);   }else{printf("Accept successful!\n"); } 
     fd[i++] = new_fd;    
     printf("\n�������˿ͻ���%d : %s:%d \n",i , inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
       /*�ѽ��淢�͸��ͻ���*/
     memset(buffer,0,SIZE);
     strcpy(buffer,"\n������������������������������������Welecom come char ����������������������������������������������\n");
     send(new_fd,buffer,SIZE,0);
    //�����ӽ��̿ͻ���
    ppid = fork();  if(ppid == 0)
    {
        //��������¿ͻ����͸��������ߵĿͻ���/
       recv(new_fd,buffer,SIZE,0);
       strcat( buffer," ������������....");  
       for(i=0;i<BACKLOG;i++)
       {  
        if(fd[i]!=-1)
         {  
              send(fd[i],buffer,strlen(buffer),0);  
         }  
            }   
        //�����ӽ��̽��ж�д����/
      pid = fork();    
       while(1)
       {
         if(pid > 0)
         {
           //���������ڽ�����Ϣ/
           memset(buffer,0,SIZE);
           if((recv(new_fd,buffer,SIZE,0)) <= 0)
           {
              close(new_fd);
              exit(1); }
            memset(shmadd, 0, SIZE_SHMADD);
            strncpy(shmadd, buffer, SIZE_SHMADD);//���������Ŀͻ�����Ϣ���빲���ڴ���
            printf(" %s\n",buffer);
         }
         if(pid == 0)
         {
           //�ӽ������ڷ�����Ϣ/
           sleep(1);//��ִ�и�����
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
