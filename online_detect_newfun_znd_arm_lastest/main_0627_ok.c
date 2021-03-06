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
#include<linux/tcp.h>

#include<sys/time.h>
#include<time.h>

#include"online_detect.h"
#include"json.h"
#include"public.h"

#define SERV_PORT            5017
#define MAXLINE              512
#define LISTENQ              100
#define CONNECT_TIMEOUT      10

//int client[LISTENQ];

//链接结构体
struct connect
{
    int socket;
    long time;
};
struct connect client_connect[LISTENQ];

fd_set rset,allset;
char buf[MAXLINE];
//ssize_t n;

//int ipc_status[MAX_IPC_NUM] = {0};
//int open_ret = 0;

#define	 BUF_MAX		        2
static int save_index;
static int read_index;
static int buff_remain_count;
//static int data_buff[SENSOR_BUF_MAX];

struct status_msg
{
    int ipc_status[MAX_IPC_NUM];
    int open_ret;
    //int is_using;
};

//struct status_msg ipc_status_msg[2];
struct status_msg ipc_status_msg[BUF_MAX];

/***************************************************************************
  Function:       *read_ipc_status_thread(void* p) 
  Description:    读取ipc状态线程
  Input:          
                  
  Output:         
  Return:         
  Others:         
***************************************************************************/
void *read_ipc_status_thread(void* p) 
{
    int index = 0;
    int i;
    do
    {
        init_online_detect();
        //open_ret = get_ipc_status(ipc_status);
/*
        index = index % 2;
        if(ipc_status_msg[index].is_using == 0)
        {
            //ipc_status_msg[index].is_using = 1;
            ipc_status_msg[index].open_ret = get_ipc_status(ipc_status_msg[index].ipc_status);
            //ipc_status_msg[index].is_using = 0;
        }
        index++;
*/   

        if(buff_remain_count < (BUF_MAX))//缓冲数据没有满
        {
            //data_buff[save_index] = temp_data;//数据入队列
            ipc_status_msg[save_index].open_ret = get_ipc_status(ipc_status_msg[save_index].ipc_status);
            buff_remain_count++;//数据计数加
            if((BUF_MAX - 1)==save_index)//存满则从0位置开始存
            {
					save_index = 0x00;
            }
            else
            {
					save_index++;
            }
		}    

        sleep(1);
        //for(i=0;i<900;i++)//降低CPU利用率
        //{
        //    usleep(1000);
        //}

    }while(1);
}

/***************************************************************************
  Function:       create_deal_thread()
  Description:    创建读取ipc状态线程
  Input:          
                  
  Output:         
  Return:         
  Others:         
***************************************************************************/
void create_read_ipc_status_thread()
{
    //listen_thread_isover = 0;

    pthread_t read_pid;

    pthread_create(&read_pid, 0, read_ipc_status_thread, NULL);
}

/***************************************************************************
  Function:       *deal_thread(void* p) 
  Description:    处理客户端链接线程
  Input:          
                  
  Output:         
  Return:         
  Others:         
***************************************************************************/
void *deal_thread(void* p) 
{
    int i;
    char temp[] = "test ipc online";
    int ipc_status_tmp[MAX_IPC_NUM];
    int open_ret_tmp = 0;
    struct timeval stop;
    int parse_ret = 0;
    int type = 0xff;
    printf("****ipc online detect deal thread start****\r\n");
    int recv_len = 0;
    int index =0;
    int no_connect =0;

    init_data_online_detect();

    while(1)
    {
       //test
       if(buff_remain_count > 0)//有数据
	   {
		   open_ret_tmp  = ipc_status_msg[read_index].open_ret;
           memcpy(ipc_status_tmp,ipc_status_msg[read_index].ipc_status,MAX_IPC_NUM*sizeof(int));

		   if((BUF_MAX - 1)==read_index)
		   {
			   read_index = 0x00;
		   }
		   else
		   {
			   read_index++;
		   }
		
		   buff_remain_count--;//取出数据，数据计数减少
	  }
	  else//没有数据则读取最后一次的数据
	  {
		   //send_data = data_buff[read_index];
		   open_ret_tmp  = ipc_status_msg[read_index].open_ret;
           memcpy(ipc_status_tmp,ipc_status_msg[read_index].ipc_status,MAX_IPC_NUM*sizeof(int));
	  }
      //test


       for(i=0;i<LISTENQ;i++)
       {
         if(client_connect[i].socket < 0)
         {
             no_connect++;
             continue;
         }
   
         recv_len = recv(client_connect[i].socket,buf,MAXLINE, MSG_DONTWAIT);//非阻塞读
         if(recv_len > 0)
         {
              //printf("***recv_len = %d***\r\n",recv_len);
              parse_ret = parse_json(buf);

              switch(parse_ret)
              {
                  case PARSE_SUCCESS:
                  {
                       if(open_ret_tmp==0)
                       {
                           type = GET_IPC_STATUS_SUCCESS;
                       }
                       else if((open_ret_tmp>0)&&(open_ret_tmp<(MAX_IPC_NUM-1)))
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

              //发回应给客户端
              response_msg(type,ipc_status_tmp,client_connect[i].socket);
  
              //response_msg(type,ipc_status_msg[index].ipc_status,client_connect[i].socket);
              //ipc_status_msg[index].is_using = 0;
              
              if(close(client_connect[i].socket)==0)
              {
                   //printf("****close socket success****\r\n");
              }
              else
              {
                   printf("****close socket failed !!!!\r\n");
              }
                 //FD_CLR(client[i],&allset);
              FD_CLR(client_connect[i].socket,&allset);
              client_connect[i].socket = -1;
              client_connect[i].time = 0;

         }//if(n > 0)
		 else//判断是否超时没有收到请求命令
		 {
		      gettimeofday(&stop,0);
		      if((stop.tv_sec-client_connect[i].time)>CONNECT_TIMEOUT)
		      {
		          if(close(client_connect[i].socket)==0)
		          {
		              //printf("****close socket success****\r\n");
		          }
		          else
		          {
		              printf("****close socket failed !!!!\r\n");
		          }
		          printf("****get ipc status connect time out !!!!\r\n");    

		          FD_CLR(client_connect[i].socket,&allset);
		          client_connect[i].socket = -1;
		          client_connect[i].time = 0;
		      }
		      //printf("****gettimeofday(&stop,0); !!!!\r\n");
		}//else//判断是否超时没有收到请求命令
     }//for(i=0;i<LISTENQ;i++)
     if(no_connect >= LISTENQ)//没有客户端链接则sleep
     {
         for(i=0;i<3;i++)//降低CPU利用率
         {
             usleep(1000);
         }
         //sleep(2);
         no_connect = 0;
     }
   }//while(1)
}

/***************************************************************************
  Function:       create_deal_thread()
  Description:    创建处理客户端链接线程
  Input:          
                  
  Output:         
  Return:         
  Others:         
***************************************************************************/
void create_deal_thread()
{
    pthread_t deal_pid;

    pthread_create(&deal_pid, 0, deal_thread, NULL);
}

/***************************************************************************
  Function:       main(int argc, char** argv)
  Description:    主函数，监听客户端链接
  Input:          
                  
  Output:         
  Return:         
  Others:         
***************************************************************************/
int main(int argc, char** argv)
{
    int i,j,maxi,maxfd,listenfd,connfd,sockfd;
    int nready;
    struct timeval start;

    struct linger
    {
        int l_onoff;
        int l_linger;
    };   

    int ret = -1;
    int flags;
    int optval = 1;

    for(i=0;i<LISTENQ;i++)
    {
        client_connect[i].socket = -1;
        client_connect[i].time = 0;
    }


    save_index  = 0x00;
    read_index  = 0x00;
    buff_remain_count  = 0x00;
    for(i=0;i<BUF_MAX;i++)
    {
        for(j=0;j<MAX_IPC_NUM;j++)
        {
            ipc_status_msg[i].ipc_status[j] = 0;
        }
        ipc_status_msg[i].open_ret = 0;
        //ipc_status_msg[i].is_using = 0;
    }

    //对链接进来的客户端设置close socket延时
    struct linger m_sLinger;
    m_sLinger.l_onoff = 1; //在调用closesocket()时还有数据未发送完，允许等待
    // 若m_sLinger.l_onoff=0;则调用closesocket()后强制关闭
    m_sLinger.l_linger = 5; //设置等待时间为5秒

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

    FD_ZERO(&allset);         //清空集合
    FD_SET(listenfd,&allset); //将一个给定的文件描述符加入集合之中

    create_read_ipc_status_thread();
    create_deal_thread();    
    
    //监听客户端链接
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
            //flags=fcntl(connfd,F_GETFL,0);
            //fcntl(connfd,F_SETFL,flags|O_NONBLOCK);
         
            //setsockopt(connfd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(int));//使用KEEPALIVE
            setsockopt(connfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(int));//禁用NAGLE算法

            //设置延时关闭socket，socket不可以设置为非阻塞
            //setsockopt(connfd, SOL_SOCKET, SO_LINGER, (const char*)&m_sLinger, sizeof(m_sLinger));

            for(i=0;i<LISTENQ;i++)
            {
                if(client_connect[i].socket < 0)
                {
                    gettimeofday(&start,0);
                    client_connect[i].time = start.tv_sec;
                    client_connect[i].socket = connfd;
                    break;
                }
            }
 
            if(i == LISTENQ)
            {
                printf("****error get ipc status reach max clients connect !!!!\r\n");
                response_msg(REACH_MAX_CLIENT_CONNECT,NULL,connfd);
                close(connfd);
                continue;
            }

            FD_SET(connfd,&allset);//将一个给定的文件描述符加入集合之中

        }//if(FD_ISSET(listenfd,&rset))
 
    }//for(;;)
ERR:
    return 0;
}



