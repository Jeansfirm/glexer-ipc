/***************************************************************************
  Copyright (C), 2009-2014 GuangdongGuanglian Electronic Technology Co.,Ltd.
  FileName:      addIPC.c
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

char *ipc_config[10];
char *ipc0_config = "/gl/etc/video/ipc0.config";
char *ipc1_config = "/gl/etc/video/ipc1.config";
char *ipc2_config = "/gl/etc/video/ipc2.config";
char *ipc3_config = "/gl/etc/video/ipc3.config";
char *ipc4_config = "/gl/etc/video/ipc4.config";
char *ipc5_config = "/gl/etc/video/ipc5.config";
char *ipc6_config = "/gl/etc/video/ipc6.config";
char *ipc7_config = "/gl/etc/video/ipc7.config";
char *ipc8_config = "/gl/etc/video/ipc8.config";
char *ipc9_config = "/gl/etc/video/ipc9.config";
#define MAX_IPC_NUM        10

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
		{   //POST在cgi中传递的Username="admin"&Password="aaaaa"被写入stdin标准输入流中
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
char *package_json(char type,char *msg)
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
  Function:       lock_file()
  Description:    实现锁文件
  Input:          1:加锁，0：解锁
                  
  Output:           
  Return:         1成功，0：失败
  Others:         
***************************************************************************/
int lock_file(int fd,int lock_access)
{
    int ret = -1;
	if(lock_access == 1)
    {
		if(flock(fd,LOCK_EX|LOCK_NB)==0)
        {
            ret = 1;
            //printf("<p>the file was locked</p>");
        }
        else
        {
            //printf("the file locked faile\r\n");
            //printf("<p>the file locked faile</p>");
        }
    }
    else
    {
		if(flock(fd,LOCK_UN)==0)
        {
            //printf("the file was unlocked\r\n");
            ret = 1;
            //printf("<p>the file was unlocked</p>");
        }
        else
        {
            //printf("the file unlocked faile\r\n");
            //printf("<p>the file unlocked faile</p>");
        }
    }
    return ret;
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
    //printf("Content-type:text/html\n\n");                        //cgi 输出html标准格式
    //printf("Content-type:text/html;charset=gb2312\n\n");       //cgi 输出html标准格式 ,输出中文显示
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

    ipc_config[0] = ipc0_config;
	ipc_config[1] = ipc1_config;
	ipc_config[2] = ipc2_config;
	ipc_config[3] = ipc3_config;
	ipc_config[4] = ipc4_config;
	ipc_config[5] = ipc5_config;
	ipc_config[6] = ipc6_config;
	ipc_config[7] = ipc7_config;
	ipc_config[8] = ipc8_config;
	ipc_config[9] = ipc9_config;

    req_method = getenv("REQUEST_METHOD");//访问页面时的请求方法
    input = getcgidata(stdin, req_method);

    //获取删除IPC ID;
    parse_data(input,ipc_id,"ipc_id=");
    if(ipc_id == NULL)
    {
		err_num = 1;
		goto over;
    }

	//printf("<p>string ipc_id = %s</p>",ipc_id);

    //检查IPC ID是否在0-9范围内
    delete_ipc_id = atoi(ipc_id);
    //printf("<p>delete_ipc_id = %d</p>",delete_ipc_id);
    if(delete_ipc_id<0 || delete_ipc_id >= MAX_IPC_NUM)
    {
		err_num = 2;
		goto over;
    }

    //查找要删除的IPC配置文件是否存在
 	if(access(ipc_config[delete_ipc_id], 0) != 0)//文件不存在
    {
		err_num = 3;
		goto over;
    }

    int fd;
    //char path[]="tt.ini";
    //fd=open(path,O_WRONLY|O_CREAT);
    //fd=open(path,O_WRONLY|O_CREAT|O_APPEND);
    //fd=open(path,O_WRONLY|O_APPEND);
    fd=open(ipc_config[delete_ipc_id],O_WRONLY|O_APPEND);  
    if(lock_file(fd,1) == 1)//锁文件
    {
		//删除配置文件
		if(remove(ipc_config[delete_ipc_id])!= 0)//删除文件失败
		{
			err_num = 4;
			//goto over;
		}
        lock_file(fd,0);//解锁
    }
    else
    {
		err_num = 5;
    }
    close(fd);

over:
	switch(err_num)
    {
		case 0:printf("<p>%s</p>",package_json(0,"delete ipc success"));break;

		case 1:printf("<p>%s</p>",package_json(1,"input Parameter error"));break;

		case 2:printf("<p>%s</p>",package_json(2,"input ipc id Parameter errorr exceed 0-9"));break;

		case 3:printf("<p>%s</p>",package_json(3,"ipc not exist"));break;

		case 4:printf("<p>%s</p>",package_json(4,"delete ipc config file faile"));break;

		case 5:printf("<p>%s</p>",package_json(5,"system is busy try again later"));break;

        default:break;
    }
    printf("</body></html>"); 

    return 0;
}



