/***************************************************************************
  Copyright (C), 2009-2015 GuangdongGuanglian Electronic Technology Co.,Ltd.
  FileName:      deleteIPC.c
  Author:        jiang
  Version :      1.0    
  Date:          2014-04-15
  Description:   实现删除IPC       
  History:         
      <author>  <time>   <version >   <desc> 
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<sys/file.h>

#include"cJSON.h"
#include "sqlite_access.h"

#define MAX_IPC_NUM        10
#define SERV_PORT            5017


/***************************************************************************
  Function:       parse_data(char* inputstr,char *outputstr,char* value)
  Description:    在字符串inputstr中查找value，输出查找到的字符串outputstr
  Input:          char* inputstr,char *outputstr,char* value
                  
  Output:         
  Return:          
  Others:         
***************************************************************************/
void parse_data(char* inputstr,char *outputstr,char* value)
{
	int i,j;
	char *tmpstr;

	tmpstr=strstr(inputstr,value);
	j = 0;

	for(i = strlen(value); i < (int)strlen(tmpstr); i++)
	{
		if(tmpstr[i] == '&')
		{
			break;
		}                  
		outputstr[j++]=tmpstr[i];
	}
	outputstr[j] = '\0';
}

/***************************************************************************
  Function:       char* getcgidata(FILE* fp, char* requestmethod)
  Description:    获取web服务器的输入
  Input:          
                  
  Output:         返回web服务器输入的字符串
  Return:         
  Others:         
***************************************************************************/
char* getcgidata(FILE* fp, char* requestmethod)
{
    char* input;
    int len;
    int size = 1024;
    int i = 0;
       
    if (!strcmp(requestmethod, "GET"))
    { 
		//GET在cgi中传递的Username="admin"&Password="aaaaa"被放置在环境变量QUERY_STRING中
		input = getenv("QUERY_STRING");
		return input;
    }
    else if (!strcmp(requestmethod, "POST"))
    {
		len = atoi(getenv("CONTENT_LENGTH"));
		input = (char*)malloc(sizeof(char)*(size + 1));
		          
		if (len == 0)
		{
			input[0] = '\0';
			return input;
		}
		          
		while(1)
		{  //POST在cgi中传递的Username="admin"&Password="aaaaa"被写入stdin标准输入流中
	 	    input[i] = (char)fgetc(fp);
		    if (i == size)
		    {
		         input[i+1] = '\0';
		         return input;
		    }
		                 
		    --len;
		    if (feof(fp) || (!(len)))
		    {
		         i++;
		         input[i] = '\0';
		         return input;
		    }
		    i++;
		                 
		}//while(1)
    }//else if (!strcmp(requestmethod, "POST"))
    return NULL;
}

/***************************************************************************
  Function:       package_json 
  Description:    封装JSON
  Input:          消息类型
                  
  Output:         
  Return:         封装json后转化为char类型的数据
  Others:         
****************************************************************************/
char *package_json(int type,char *msg)
{
    cJSON *root,*fmt;
    char *out;	
    //Our "Video" datatype: 
    root=cJSON_CreateObject();	
    cJSON_AddStringToObject(root, "action", "deleteipc");
    cJSON_AddItemToObject(root, "response_params", fmt=cJSON_CreateObject());
    cJSON_AddNumberToObject(fmt,"status",     type);
    cJSON_AddStringToObject(fmt,"status_msg", msg);

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
  Function:       package_json_callback 
  Description:    发到5017端口的callback数据格式
  Input:          
                  
  Output:      输出封装好的callback数据的地址   
  Return:       
  Others:         
****************************************************************************/
char *package_json_callback(int type,int file_num)
{
    cJSON *root,*fmt;
    char *out;	
    //Our "Video" datatype: 
    root=cJSON_CreateObject();
	cJSON_AddNumberToObject(root,"msgtype",0);
	cJSON_AddNumberToObject(root,"mainid",2);
	cJSON_AddNumberToObject(root,"subid",3);
	cJSON_AddNumberToObject(root,"status",type );
	if(type == 0)
	{
		cJSON_AddNumberToObject(root,"id",file_num);
	}
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
  Function:       main()
  Description:    主函数
  Input:          
                  
  Output:         
  Return:         
  Others:         
***************************************************************************/
int main()
{ 
    //printf("Content-type:text/html\n\n");                         //cgi 输出html标准格式
    //printf("Content-type:text/html;charset=gb2312\n\n");      //cgi 输出html标准格式 ,输出中文显示
    printf("Content-type:text/html;charset=utf-8\n\n");       //cgi 输出html标准格式 ,输出中文显示
    printf("<html>"); 
    printf("<head><title>deleteIPC</title></head><body>"); 
    //printf("<p>hello world</p>"); 

    char *input;
    char *req_method; 
    FILE *fp;  
	char ipc_id[20];
    char ipc_num = 0;
    int delete_ipc_id = -1;
    int err_num = 0;

	int file_num;
	int res; 
	int sockfd_send; 	
	char *send_buf;
	int send_ret;

	
	struct sockaddr_in servaddr;	  
	bzero(&servaddr, sizeof(servaddr));    
	servaddr.sin_family = AF_INET;						 
	servaddr.sin_port = htons(SERV_PORT);				 
	//servaddr.sin_addr.s_addr = inet_addr("192.168.1.162");	//for test
	servaddr.sin_addr.s_addr = htons(INADDR_ANY); //INADDR_ANY表示自动获取本机地址

    req_method = getenv("REQUEST_METHOD");//访问页面时的请求方法
    input = getcgidata(stdin, req_method);

    //获取删除IPC ID;
    parse_data(input,ipc_id,"ipc_id=");
    if(ipc_id == NULL)
    {
		err_num = -1;
		goto over;
    }

	//printf("<p>string ipc_id = %s</p>",ipc_id);

   //检查IPC ID是否在0-9范围内
    
    delete_ipc_id = atoi(ipc_id);
	file_num=atoi(ipc_id);
    //printf("<p>delete_ipc_id = %d</p>",delete_ipc_id);
    if(delete_ipc_id<0 || delete_ipc_id >= MAX_IPC_NUM)
    {
		err_num = -2;
		goto over;
    }

    int init_ret;
    int get_ret;
    
    init_ret = init_database();
    if(init_ret == -1)
    {
        err_num = -3;
		goto over;
    }
    else if(init_ret == -2)
    {
        err_num = -4;
		goto over;
    }
    else
    {
        get_ret = deleteipc(ipc_id);
        if(get_ret == 1)
        {
            err_num = -6;
			goto over;
        }
        else if(get_ret == -1)
        {
            err_num = -4;
			goto over;
        }
        else if(get_ret == -2)
        {
            err_num = -5;
			goto over;
        }
        else
        {
            err_num = 0;
        }
    }
    uninit_database();

	 //while(sockfd_send = socket(AF_INET, SOCK_STREAM, 0)<0);	
	 sockfd_send = socket(AF_INET, SOCK_STREAM, 0);
	 if (sockfd_send <0)
	 {			  
		err_num=-7;
		goto over;   
	 }
		  
	 // while(connect(sockfd_send, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in))!=0);	 
	 res=connect(sockfd_send, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in));
	 if (res !=0)
	 { 		
		err_num=-8;
		goto over;		
	 }

over:
	//printf("<p>err_num:%d\n</p>",err_num);
	switch(err_num)
    {
		case 0:printf("<p>%s</p>",package_json(0,"delete ipc success"));break;

		case -1:printf("<p>%s</p>",package_json(-1,"input Parameter error"));break;

		case -2:printf("<p>%s</p>",package_json(-2,"input ipc id Parameter errorr exceed 0-9"));break;

		case -3:printf("<p>%s</p>",package_json(-3,"creat database failed"));break;

		case -4:printf("<p>%s</p>",package_json(-4,"access database failed"));break;

		case -5:printf("<p>%s</p>",package_json(-5,"delete ipc failed"));break;

		case -6:printf("<p>%s</p>",package_json(-6,"IPC not exist"));break;

		case -7:printf("<p>%s</p>",package_json(-7,"socket() buit faild"));break;

		case -8:printf("<p>%s</p>",package_json(-8,"connect() buit faild"));break;

		case -9:printf("<p>%s</p>",package_json(-9,"send data to 5017 port faild"));break;

        default:break;
    }

		
		  send_buf=package_json_callback(err_num,file_num);
		  //printf("<p>send_buf=%s</p>",send_buf);
		  
		  send_ret=send(sockfd_send,send_buf,strlen(send_buf),0);
		 // printf("<p>send_ret:%d\n</p>",send_ret);

		 close(sockfd_send);

	
    printf("</body></html>"); 

    return 0;
}



