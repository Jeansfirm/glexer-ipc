/***************************************************************************
  Copyright (C), 2009-2014 GuangdongGuanglian Electronic Technology Co.,Ltd.
  FileName:      editIPC.c
  Author:        jiang
  Version :      1.0    
  Date:          2014-04-14
  Description:   修改IPC排列顺序号    
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
#include <sys/file.h>

#include "cJSON.h"
#include "sqlite_access.h"



#define MAX_IPC_NUM        10
int file_num = 0;  //IPC ID

#define SERV_PORT            5017


/***************************************************************************
  Function:       check_string_len(const char *str,int t_len)
  Description:   检查输入字符串长度是否在t_len范围内
  Input:          
                  
  Output:         
  Return:          
  Others:    0: 长度正确(大小为10)，-1:长度错误(大小不为10)    
***************************************************************************/
int check_string_len(const char *str,int t_len)
{
    int ret = -1;
	int len = -1;
	len = strlen(str);
    if((len == t_len))
    {
		ret = 0;
    }
    return ret;
}

/***************************************************************************
 Function: 	  check_indexid(const char *str)
 Description:	check the indexid 0-10
 Input:		  
					  
 Output:		  
 Return:		   
 Others:		  
***************************************************************************/
int check_indexid(const char *str)
{
	int ret=-1;
	int temp=-1;
	temp=atoi(str);
	if((temp>=0)&&(temp<MAX_IPC_NUM))
	{
		ret=1;
	}
	return ret;
}


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
		                 
		}
    }
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
    cJSON_AddStringToObject(root, "action", "editipc");
    cJSON_AddItemToObject(root, "response_params", fmt=cJSON_CreateObject());
    cJSON_AddNumberToObject(fmt,"status",     type);
    cJSON_AddStringToObject(fmt,"status_msg", msg);
	/*
    if(type == 0)
    {
        cJSON_AddNumberToObject(fmt,"ipc_id",file_num);
    }
	*/
	
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
char *package_json_callback(int type,char *ipc_index_list)
{
    cJSON *root,*fmt;
    char *out;	
    //Our "Video" datatype: 
    root=cJSON_CreateObject();
	cJSON_AddNumberToObject(root,"msgtype",0);
	cJSON_AddNumberToObject(root,"mainid",2);
	cJSON_AddNumberToObject(root,"subid",5);
	cJSON_AddNumberToObject(root,"status",type );
	if(type == 0)
	{
		cJSON_AddStringToObject(root,"ipc_index_list", ipc_index_list);	
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
    //printf("Content-type:text/html\n\n");                        //cgi 输出html标准格式
    //printf("Content-type:text/html;charset=gb2312\n\n");        //cgi 输出html标准格式 ,输出中文显示
    printf("Content-type:text/html;charset=utf-8\n\n");        //cgi 输出html标准格式 ,输出中文显示
    printf("<html>"); 
    printf("<head><title>modifyIPCindex</title></head><body>"); 
   // printf("<p>hello world</p>"); 

    char *input;
    char *req_method; 
    FILE *fp;  

 	int err_num=0;
	int init_ret;
	char ipc_index_list[30];
	int string_ret;
	int i_ret;
	int update_ret=0;

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

    init_ret = init_database();
    if(init_ret == -1)
    {
        err_num = -2;
        goto over;
    }
    else if(init_ret == -2)
    {
        err_num = -3;
        goto over;
    }

    parse_data(input,ipc_index_list,"ipc_index_list=");
	//printf("<p>ipc_index_list:%s\n</p>",ipc_index_list);
	if(*ipc_index_list=='\0')
	{
		err_num=-1;
		goto over;
	}
	
	string_ret=check_string_len(ipc_index_list,MAX_IPC_NUM);
	if(string_ret==-1)
	{
		err_num=-4;
		goto over;
	}

	update_ret=update_database_index(ipc_index_list);
	if(update_ret==-1)
	{
		err_num=-5;
		goto over;
	}
	else if(update_ret==-2)
	{
		err_num=-6;
		goto over;
	}
	else if(update_ret==-3)
	{
		err_num=-7;
		goto over;
	}
	else if(update_ret==-4)
	{
		err_num=-8;
		goto over;
	}
	else if(update_ret==-5)
	{
		err_num=-9;
		goto over;
	}
	else if(update_ret==-6)
	{
		err_num=-10;
		goto over;
	}

	uninit_database();
	//while(sockfd_send = socket(AF_INET, SOCK_STREAM, 0)<0);	

	sockfd_send = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd_send <0)
	{			 
		err_num=-11;
		goto over;   
	 }
		 
	// while(connect(sockfd_send, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in))!=0);
	res=connect(sockfd_send, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in));
	 if (res !=0)
	 {		   
		 err_num=-12;
		 goto over; 	   
	}
   
over:
	switch(err_num)
    {
		case 0:printf("<p>%s</p>",package_json(0,"modify ipc index success"));break;

        case -1:printf("<p>%s</p>",package_json(-1,"input parameter error"));break;

        case -2:printf("<p>%s</p>",package_json(-2,"open database failed"));break;
  
        case -3:printf("<p>%s</p>",package_json(-3,"access database failed"));break;

		case -4:printf("<p>%s</p>",package_json(-4,"strlen of ipc_index_list not equal 10"));break;

		case -5:printf("<p>%s</p>",package_json(-5,"sqlite3_get_table() failed"));break;

		case -6:printf("<p>%s</p>",package_json(-6,"number of new index not equal to row of database"));break;

		case -7:printf("<p>%s</p>",package_json(-7,"sqlite3_get_table() failed "));break;

		case -8:printf("<p>%s</p>",package_json(-8,"exist other char"));break;

		case -9:printf("<p>%s</p>",package_json(-9,"database can't find specific data "));break;

		case -10:printf("<p>%s</p>",package_json(-10,"update index failed"));break;
		
        case -11:printf("<p>%s</p>",package_json(-11,"socket() buit faild"));break;
		
        case -12:printf("<p>%s</p>",package_json(-12,"connect() buit faild"));break;
		
        case -13:printf("<p>%s</p>",package_json(-13,"send data to 5017 port faild"));break;
		
        default:break;
    }

	 send_buf=package_json_callback(err_num,ipc_index_list);
	// printf("<p>send_buf:%s\n</p>",send_buf);

 
	 send_ret=send(sockfd_send,send_buf,strlen(send_buf),0);
	// printf("<p>send_ret:%d\n</p>",send_ret);
	
	 close(sockfd_send);

    printf("</body></html>"); 

    return 0;
}



