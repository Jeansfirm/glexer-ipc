/***************************************************************************
  Copyright (C), 2009-2014 GuangdongGuanglian Electronic Technology Co.,Ltd.
  FileName:      main.c
  Author:        jiang
  Version :      1.0    
  Date:          2014-06-17
  Description:   ipc_rtsp程序的实现文件，包含rtsp协议会话的实现      
  History:         
      <author>  <time>   <version >   <desc> 
***************************************************************************/
#include<sys/ioctl.h>
#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<linux/if_ether.h>
#include<linux/if.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>
#include<fcntl.h>
#include<limits.h>
#include<sys/file.h>
#include<sys/time.h> 
#include<errno.h>
#include<signal.h> 
#include<string.h>

//#include <sys/types.h> 
//#include <sys/stat.h> 
//#include <fcntl.h>
//#include <unistd.h>

#include"online_detect.h"
#include"json.h"
#include"public.h"

#define SERV_PORT    5017
#define MAXLINE      512
#define LISTENQ      100

int client[LISTENQ];
fd_set rset,allset;
char buf[MAXLINE];
ssize_t n;
/***************************************************************************
  Function:       *ServerMctpListen(void* p) 
  Description:    监听客户端的链接请求，为请求的客户端创建socket
  Input:          
                  
  Output:         
  Return:         
  Others:         
***************************************************************************/
void *deal_thread(void* p) 
{
    int i;
    char temp[] = "test ipc online";
    int ipc_status[MAX_IPC_NUM];
    int open_ret = 0;
    int parse_ret = 0;
    int type = 0xff;
    printf("****ipc online detect deal thread start****\r\n");
    n = 0;
    init_data_online_detect();
    while(1)
    {
       //open_ret = get_ipc_status(ipc_status);
       for(i=0;i<LISTENQ;i++)
       {
         //if((sockfd = client[i]) < 0)
         if(client[i] < 0)
         {
             continue;
         }
         if(FD_ISSET(client[i],&rset))//检查集合中指定的文件描述符是否可以读写
         {
             if((n=read(client[i],buf,MAXLINE)) > 0)
             {
                 //printf("***read n = %d***\r\n",n);
                 parse_ret = parse_json(buf);

                 switch(parse_ret)
                 {
                     case PARSE_SUCCESS:
                     {
                          init_online_detect();
                          open_ret = get_ipc_status(ipc_status);
                          init_online_detect();
                          open_ret = get_ipc_status(ipc_status);

                          if(open_ret==0)
                          {
                              type = GET_IPC_STATUS_SUCCESS;
                          }
                          else if((open_ret>0)&&(open_ret<(MAX_IPC_NUM-1)))
                          {
                              type = GET_IPC_STATUS_NOT_ALL_SUCCESS;
                          }
                          else
                          {
                              type = GET_IPC_STATUS_FAILED;
                          }
                          break;
                     }
                     case JSON_PARSE_FAILED:type = JSON_PARSE_FAILED;break;
                     case INVALID_ACTION:   type = INVALID_ACTION;   break;
                     case UNKNOWN_ACTION:   type = UNKNOWN_ACTION;   break;
                     default:break;
                 }//switch(parse_ret)

                 response_msg(type,ipc_status,client[i]);
                 if(close(client[i])==0)
                 {
                     //printf("****close socket success****\r\n");
                 }
                 else
                 {
                     printf("****close socket failed !!!!\r\n");
                 }
                 FD_CLR(client[i],&allset);
                 client[i] = -1;

              }//if((n=read(client[i],buf,MAXLINE)) > 0)

         }//if(FD_ISST(sockfd,&rest))
     }//for(i=0;i<LISTENQ;i++)
     for(i=0;i<50;i++)//降低CPU利用率
     {
         usleep(1000);
     }
   }//while(1)
}

/***************************************************************************
  Function:       create_deal_thread()
  Description:    创建监听客户端的链接请求线程,在ipc_rtsp.cpp中使用
  Input:          
                  
  Output:         
  Return:         
  Others:         
***************************************************************************/
void create_deal_thread()
{
    //listen_thread_isover = 0;

    pthread_t deal_pid;

    pthread_create(&deal_pid, 0, deal_thread, NULL);
}

/***************************************************************************
  Function:       main(int argc, char** argv)
  Description:    主函数
  Input:          
                  
  Output:         
  Return:         
  Others:         
***************************************************************************/
int main(int argc, char** argv)
{
    int i,maxi,maxfd,listenfd,connfd,sockfd;
    int nready;
    //int client[LISTENQ];
    int ret = -1;
    int flags;
    //ssize_t n;
    //fd_set rset,allset;
    //char buf[MAXLINE];
    socklen_t clilen;
    struct sockaddr_in cliaddr,servaddr;
    
    listenfd = socket(AF_INET,SOCK_STREAM,0);
    if(listenfd == -1) 
    {
        perror("socket!");
        goto ERR;
    }

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    ret = bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
    if(ret < 0)
    {
        printf("**** ipc online detect listening connected  on port %d, bind error, %d !!!!\n", SERV_PORT,ret);
        goto ERR;
    }
    else
    {
        printf("**** ipc online detect listening connected  on port %d \n", SERV_PORT);
    }

    ret = listen(listenfd,LISTENQ);
    if (ret < 0) 
    {
        printf("**** ipc online detect listening on port %d, listen error, %d \n", SERV_PORT,ret);
	    goto ERR;
    }

    maxfd = listenfd;
    maxi = -1;
    for(i=0;i<LISTENQ;i++)
    {
        client[i] = -1;
    }
    
    FD_ZERO(&allset);         //清空集合
    FD_SET(listenfd,&allset); //将一个给定的文件描述符加入集合之中

    create_deal_thread();    

    for(;;)
    {
        rset = allset;
        //  select第一个参数，是一个整数值，是指集合中所有文件描述符的范围，即所有文件描述符的最大值加1
        //  select第二个参数，这个集合中应该包括文件描述符，我们是要监视这些文件描述符的读变化的，即我们关心是否可以从这些文件中读取数据了，
        //如果这个集合中有一个文件可读select就会返回一个大于0的值，表示有文件可读，
        //如果没有可读的文件，则根据timeout参数再判断是否超时，若超出timeout的时间，select返回0，若发生错误返回负值。
        //可以传入NULL值，表示不关心任何文件的读变化。
        nready = select(maxfd+1,&rset,NULL,NULL,NULL);
        if(FD_ISSET(listenfd,&rset))//检查集合中指定的文件描述符是否可以读写
        {
            clilen = sizeof(cliaddr);
            
            connfd = accept(listenfd,(struct sockaddr *)&cliaddr,&clilen);
            if(connfd < 0)
            {
             continue;
            }            

            //设置socket为非阻塞
            flags=fcntl(connfd,F_GETFL,0);
            fcntl(connfd,F_SETFL,flags|O_NONBLOCK);

            for(i=0;i<LISTENQ;i++)
            {
                if(client[i]<0)
                {
                    client[i] = connfd;
                    break;
                }
            }

            if(i == LISTENQ)
            {
                printf("too many clients\r\n");
                close(connfd);
                continue;
            }

            FD_SET(connfd,&allset);//将一个给定的文件描述符加入集合之中
            if(connfd > maxfd)
            {
                maxfd = connfd;
            }

            if(i > maxi)
            {
                maxi = i;
            }

            if(--nready <= 0)
            {
                continue;
            }
        }//if(FD_ISSET(listenfd,&rset))
 
    }//for(;;)
ERR:
    return 0;
}



