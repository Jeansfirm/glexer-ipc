/***************************************************************************
  Copyright (C), 2009-2014 GuangdongGuanglian Electronic Technology Co.,Ltd.
  FileName:      main.c
  Author:        jiang
  Version :      1.0    
  Date:          2014-06-17
  Description:   检测ipc状态程序主函数的实现文件      
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

//#include"online_detect.h"
#include"online_detect.h"
#include"json.h"
#include"public.h"
#include"cJSON.h"

#define SERV_PORT            5017
int err_num;

/***************************************************************************
  Function:       *read_ipc_status(void* p) 
  Description:    读取ipc状态线程
  Input:          
                  
  Output:     0:�ɹ�;-1:���߼���ʼ��ʧ�� ;-2:��ȡ״̬ʧ��   
  Return:         
  Others:         
***************************************************************************/
int read_ipc_status(char *temp_status) 
{
    int index = 0;
	int get_ret;
    int i;
    int init_ret = 0;
	int return_ret=0;

	init_ret = init_online_detect();
	//sleep(1);
	if(init_ret !=0)
    {
        printf("****init_online_detect error****\r\n");
        return_ret=-1;
    }

    get_ret=get_ipc_status(temp_status);
	if(get_ret==-1)
	{
		return_ret=-2;
	}

	//printf("temp_status=%s\n",temp_status);
	return return_ret;
      
}



/***************************************************************************
  Function:       package_json_callback 
  Description:    ����5017�˿ڵ�callback���ݸ�ʽ
  Input:          
                  
  Output:      �����װ�õ�callback���ݵĵ�ַ   
  Return:       
  Others:         
****************************************************************************/
char *package_json_callback(char *ipc_status_list)
{
    cJSON *root,*fmt;
    char *out;	
    //Our "Video" datatype: 
    root=cJSON_CreateObject();
	cJSON_AddNumberToObject(root,"msgtype",0);
	cJSON_AddNumberToObject(root,"mainid",2);
	cJSON_AddNumberToObject(root,"subid",4);
	//cJSON_AddNumberToObject(root,"status",type );
	//if(type == 0)
	//{
		cJSON_AddStringToObject(root,"ipc_status_list", ipc_status_list);	
	//}
    //out=cJSON_Print(root);
    out=cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    //cJSON_Delete(fmt);
    //printf("%s\n",out);	
    //free(out);
    // Print to text, Delete the cJSON, print it, release the string. 

    return out; 
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
	int update_ret;
	char temp_status[11];
	char init_status_array[11];
	int read_ret;
	char *send_buf;
	int sockfd_send;
	int res;
    int send_ret;
	
	int init_status_ret;
	int init_database_ret;

	struct sockaddr_in servaddr;	  
	bzero(&servaddr, sizeof(servaddr));    
	servaddr.sin_family = AF_INET;						 
	servaddr.sin_port = htons(SERV_PORT);				 
	//servaddr.sin_addr.s_addr = inet_addr("192.168.1.162");	//for test
	servaddr.sin_addr.s_addr = htons(INADDR_ANY); //INADDR_ANY��ʾ�Զ���ȡ������ַ

while(1)
{
	err_num=0;
	init_database_ret=init_database();
	if(init_database_ret==-1)
	{
		err_num=-1;  // open database failed;
	}
	if(init_database_ret==-2)
	{
		err_num=-2;	// open table of database failed;
	}

	usleep(100000);
	init_status_ret=read_init_status(init_status_array);

	//printf("init_status_array:%s\n",init_status_array);

	//create_read_ipc_status_thread();
	//create_deal_thread();

	read_ret=read_ipc_status(temp_status);
	if(read_ret==-1)
	{
		err_num=-3; // init_online_detect failed
	}
	if(read_ret==-2)
	{
		err_num=-4; // get ipc status failed
	}

	//printf("temp_status=%s\n",temp_status);

	if(strcmp(init_status_array,temp_status)!=0)  // �и��¸������ݿⲢ����callback����
	{

		//printf("not equal!!!\n");
		//init_database();
		update_ret=update_ipc_status(temp_status);
		//uninit_database();
		if(update_ret==-1)
		{
			err_num=-5; // access database failed
		}
		if(update_ret==-2)
		{
			err_num=-6; // �µ�����˳��ŵĸ��������ݿ����������� 
		}
		if(update_ret==-3)
		{
			err_num=-6; // access database failed
		}
		if(update_ret==-4)
		{
			err_num=-7; // ���������ַ� 
		}
		if(update_ret==-5)
		{
			err_num=-8; // ���ݿ�û�д��ڶ�Ӧ��id��
		}
		if(update_ret==-6)
		{
			err_num=-9; // ����ipc_statusʧ��
		}

		//printf("init_status_array=%s\n",init_status_array);		

		if(err_num==0)
		{
			sockfd_send = socket(AF_INET, SOCK_STREAM, 0);
			if (sockfd_send <0)
			{			 
				err_num=-11; // socket() failled
			 }

			 // while(connect(sockfd_send, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in))!=0);
			res=connect(sockfd_send, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in));
			if (res !=0)
			{		   
				err_num=-12; // connect() failed   
			 }

			//printf("err_num:%d\n",err_num);
			if(err_num==0)
			{
	 			send_buf=package_json_callback(temp_status);
				//printf("send_buf:%s\n",send_buf);
		
	 			send_ret=send(sockfd_send,send_buf,strlen(send_buf),0);	
			//	sleep(3); // �ȴ�3�룬����鿴���������Ƿ�ɹ���������ɾ���˺���
				printf("update new_ipc_status success\n");
			}
			close(sockfd_send);
		}	
	}

	//printf("init_status_array=%s\n",init_status_array);
	//printf("err_num:%d\n",err_num);

	uninit_database();
}
}



