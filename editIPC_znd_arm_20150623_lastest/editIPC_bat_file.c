/***************************************************************************
  Copyright (C), 2009-2014 GuangdongGuanglian Electronic Technology Co.,Ltd.
  FileName:      editIPC.c
  Author:        jiang
  Version :      1.0    
  Date:          2014-04-14
  Description:   实现添加IPC      
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
#include "online_detect.h"


#define MAX_IPC_NUM        10
int file_num = 0;  //IPC ID

#define SERV_PORT            5017

/***************************************************************************
  Function:       check_ip(const char *str)
  Description:    检查ip地址是否合法 
  Input:          char* inputstr,char *outputstr,char* value
                  
  Output:         
  Return:          
  Others:         
***************************************************************************/
int check_ip(const char *str)
{
    struct in_addr addr;
    int ret;
    volatile int local_errno;
	bzero(&addr, sizeof(addr));
    ret = inet_pton(AF_INET, str, &addr);
    local_errno = errno;
    if (ret > 0)
    {
        ret = 1;
    }
    else 
    {
        ret = -1;
    }
    return ret;
}

/***************************************************************************
  Function:       check_string_len(const char *str,int t_len)
  Description:    检查输入字符串长度是否在t_len范围�?
  Input:          
                  
  Output:         
  Return:          
  Others:         
***************************************************************************/
int check_string_len(const char *str,int t_len)
{
    int ret = -1;
	int len = -1;
	len = strlen(str);
    if((len >= 0) && (len <= t_len))
    {
		ret = 1;
    }
    return ret;
}

/***************************************************************************
  Function:       check_port(const char *str)
  Description:    检查rtsp端口是否合法 0-65535
  Input:          char* inputstr,char *outputstr,char* value
                  
  Output:         
  Return:          
  Others:         
***************************************************************************/
int check_port(const char *str)
{
    int ret = -1;
	int temp = -1;
	temp = atoi(str);
    if((temp >= 0) && (temp < 65536))
    {
		ret = 1;
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
 Function: 	  check_roomid(const char *str)
 Description:	check the roomid more than -1
 Input:		  
					  
 Output:		  
 Return:		   
 Others:		  
***************************************************************************/
int check_roomid(const char *str)
{
	int ret=-1;
	int temp=-1;
	temp=atoi(str);
	if(temp>=-1)
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
                  
  Output:         返回web服务器输入的字符�?
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
		//GET在cgi中传递的Username="admin"&Password="aaaaa"被放置在环境变量QUERY_STRING�?
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
  Return:         封装json后转化为char类型的数�?
  Others:         
****************************************************************************/
char *package_json(char type,char *msg)
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
  Description:    ����5017�˿ڵ�callback���ݸ�ʽ
  Input:          
                  
  Output:      �����װ�õ�callback���ݵĵ�ַ   
  Return:       
  Others:         
****************************************************************************/
char *package_json_callback(int type,char file_num,char *ipaddr,char *rtspport,char *httpport,char *name,char *password,char *alias,char *indexid,char *roomid,char *ipc_status)
{
    cJSON *root,*fmt;
    char *out;	
    //Our "Video" datatype: 
    root=cJSON_CreateObject();
	cJSON_AddNumberToObject(root,"msgtype",0);
	cJSON_AddNumberToObject(root,"mainid",2);
	cJSON_AddNumberToObject(root,"subid",2);
	cJSON_AddNumberToObject(root,"status",type );
	if(type == 0)
	{
		cJSON_AddNumberToObject(root,"id",file_num);
		cJSON_AddStringToObject(root,"ipaddr", ipaddr);
		cJSON_AddStringToObject(root,"rtspport", rtspport);
		cJSON_AddStringToObject(root,"httpport", httpport);
		cJSON_AddStringToObject(root,"name", name);
		cJSON_AddStringToObject(root,"password", password);
		cJSON_AddStringToObject(root,"aliases", alias);
		cJSON_AddStringToObject(root,"index", indexid);
		cJSON_AddStringToObject(root,"roomid", roomid);
		cJSON_AddStringToObject(root,"ipc_status", ipc_status);
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
  Description:    主函�?
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
    printf("<head><title>editIPC</title></head><body>"); 
   // printf("<p>hello world</p>"); 

    char *input;
    char *req_method; 
    FILE *fp;  
	char ipaddr[20];
	char ipaddr_init[20];
    char ipaddr4check[25] = {0};
	char name[20]; 
	char name_init[20];
	char password[20];
	char password_init[20];
	char rtspport[20]; 
	char rtspport_init[20];
    char httpport[20]; 
	char httpport_init[20];
    char alias[150];
	char alias_init[150];
	char ipc_id[20];
	int edit_ipc_id;
    int err_num = 0;
    int isused_ip;
    int edit_ret;
	char indexid[11];
	//char indexid_init[11];
	char roomid[11];
	//char roomid_init[11];
	char ipc_status[10];
	char ipc_status_init[10];
    int init_ret;
	int initdetect_ret;

	int res; 
	int sockfd_send; 	
	char *send_buf;
	int send_ret;

	
	struct sockaddr_in servaddr;	  
	bzero(&servaddr, sizeof(servaddr));    
	servaddr.sin_family = AF_INET;						 
	servaddr.sin_port = htons(SERV_PORT);				 
	servaddr.sin_addr.s_addr = inet_addr("192.168.1.162");	//for test
	//servaddr.sin_addr.s_addr = htons(INADDR_ANY); //INADDR_ANY��ʾ�Զ���ȡ������ַ

    req_method = getenv("REQUEST_METHOD");//访问页面时的请求方法
    input = getcgidata(stdin, req_method);

    init_ret = init_database();
    if(init_ret == -1)
    {
        err_num = 2;
        goto over;
    }
    else if(init_ret == -2)
    {
        err_num = 3;
        goto over;
    }

    //获取检查ip地址;
    parse_data(input,ipaddr,"ipaddr=");
	strcpy(ipaddr_init,ipaddr);
    if(ipaddr == NULL)
    {
		err_num = 1;
		goto over;
    }
	if(check_ip(ipaddr) < 0)
    {
		err_num = 4;
		goto over;				
    }
/*
	strcpy(ipaddr4check,ipaddr);
    isused_ip = ipaddr_isuse(ipaddr4check);
    if(isused_ip ==1)
    {
        err_num = 10;
		goto over;	
    }
    else if(isused_ip ==-1)
    {
        err_num = 11;
		goto over;
    }
    */

    //获取检查用户名
    parse_data(input,name,"name=");
	strcpy(name_init,name);
    if(name == NULL)
    {
		err_num = 1;
		goto over;
    }
    if(check_string_len(name,16)<0)
    {
		err_num = 5;
		goto over;				
    }

    //获取检查密�?
    parse_data(input,password,"password=");
	strcpy(password_init,password);
    if(password == NULL)
    {
		err_num = 1;
		goto over;
    }
    if(check_string_len(password,16)<0)
    {
		err_num = 6;
		goto over;				
    }

    //获取检查rtsp端口�?
    parse_data(input,rtspport,"rtspport=");
	strcpy(rtspport_init,rtspport);
    if(rtspport == NULL)
    {
		err_num = 1;
		goto over;	
    }
	if(check_port(rtspport) < 0)
    {
		err_num = 7;
		goto over;					
    }

    //获取检查http端口�?
    parse_data(input,httpport,"httpport=");
	strcpy(httpport_init,httpport);
    if(httpport == NULL)
    {
		err_num = 1;
		goto over;	
    }
	if(check_port(httpport) < 0)
    {
		err_num = 8;
		goto over;					
    }

    //获取检查别名长�?
    parse_data(input,alias,"alias=");
	strcpy(alias_init,alias);
    if(alias == NULL)
    {
		err_num = 1;
		goto over;	
    }
	if(check_string_len(alias,150)<0)
    {
		err_num = 9;
		goto over;					
    }

   parse_data(input,indexid,"indexid=");
   if(indexid==NULL)
   {
		err_num=1;
		goto over;
   }
   if(check_indexid(indexid)<0)
   {
		err_num=15;
		goto over;
   }
   parse_data(input,roomid,"roomid=");
	 if(roomid==NULL)
	 {
		  err_num=1;
		  goto over;
	 }
	 if(check_roomid(roomid)<0)
	 {
		  err_num=16;
		  goto over;
	 }
	parse_data(input,ipc_id,"ipc_id=");
    if(ipc_id == NULL)
    {
		err_num = 1;
		goto over;
    }
    file_num=atoi(ipc_id);
	
	 edit_ipc_id = atoi(ipc_id);
    //printf("<p>delete_ipc_id = %d</p>",delete_ipc_id);
    if(edit_ipc_id<0 || edit_ipc_id >= MAX_IPC_NUM)
    {
		err_num = 10;
		goto over;
    }
		
	initdetect_ret = init_online_detect();
	if(initdetect_ret!=0)
	{
		err_num=17;
		goto over;
	}

	ipc_status[0]=get_ipc_status(ipaddr); // return a,b or c  
	ipc_status[1]='\0';
	strcpy(ipc_status_init,ipc_status);
	
    
	edit_ret = editipc(ipaddr, rtspport, httpport,name, password, alias,indexid,roomid,ipc_status,ipc_id);
	if(edit_ret == -1)
	{
		    err_num = 11;
		    goto over;
	}
   	if(edit_ret == -2)
	{
		    err_num =3;
		    goto over;
	}
    if(edit_ret == -3)
	{
		    err_num = 12;
		    goto over;
	}
    if(edit_ret == -5)
	{
		    err_num = 14;
		    goto over;
	}
	if(edit_ret == -4)
	{
		    err_num = 13;
		    goto over;
	}
   
    uninit_database();


	//while(sockfd_send = socket(AF_INET, SOCK_STREAM, 0)<0);	
	   
	sockfd_send = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd_send <0)
	{			 
		err_num=18;
		goto over;   
	 }
		 
	// while(connect(sockfd_send, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in))!=0);

	send_buf=package_json_callback(err_num,file_num,ipaddr_init,rtspport_init,httpport_init, name_init,password_init,alias_init, indexid,roomid,ipc_status_init);
	printf("<p>%s</p>",send_buf);
		
	res=connect(sockfd_send, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in));
	 if (res !=0)
	 {		   
		 err_num=19;
		 goto over; 	   
	}
		 
	 send_ret=send(sockfd_send,send_buf,strlen(send_buf),0);
	// printf("<p>send_ret:%d\n</p>",send_ret);
	if(send_ret<0)
	 {
		 err_num=20;
		 goto over;
	 }
	 close(sockfd_send);
   
   
over:
	switch(err_num)
    {
		case 0:printf("<p>%s</p>",package_json(0,"edit ipc success"));break;

        case 1:printf("<p>%s</p>",package_json(-1,"input parameter error"));break;

        case 2:printf("<p>%s</p>",package_json(-2,"open database failed"));break;
  
        case 3:printf("<p>%s</p>",package_json(-3,"access database failed"));break;

		case 4:printf("<p>%s</p>",package_json(-4,"input ip address parameter error"));break;

		case 5:printf("<p>%s</p>",package_json(-5,"input user name parameter error exceed max len"));break;

		case 6:printf("<p>%s</p>",package_json(-6,"input password parameter error exceed max len"));break;

		case 7:printf("<p>%s</p>",package_json(-7,"input rtspport parameter error exceed 0-65535"));break;

		case 8:printf("<p>%s</p>",package_json(-8,"input httppport parameter error exceed 0-65535"));break;

		case 9:printf("<p>%s</p>",package_json(-9,"input alias parameter error exceed max len"));break;

		case 10:printf("<p>%s</p>",package_json(-10,"input ipc id parameter errorr exceed 0-9"));break;

        case 11:printf("<p>%s</p>",package_json(-11,"ipc not exist"));break;

        case 12:printf("<p>%s</p>",package_json(-12,"ipaddr is already used error"));break;

        case 13:printf("<p>%s</p>",package_json(-13,"editipc udate database access failed"));break;

        case 14:printf("<p>%s</p>",package_json(-14,"find ipcaddr access failed"));break;

	    case 15:printf("<p>%s</p>",package_json(-15,"input indexid parameter error exceed 0-10"));break;

		case 16:printf("<p>%s</p>",package_json(-16,"input roomid parameter error less than -1"));break;

		case 17:printf("<p>%s</p>",package_json(-17,"ininit_online_detect error"));break;

		case 18:printf("<p>%s</p>",package_json(-18,"socket() buit faild"));break;

		case 19:printf("<p>%s</p>",package_json(-19,"connect() buit faild"));break;

		case 20:printf("<p>%s</p>",package_json(-20,"send data to 5017 port faild"));break;

        default:break;
    }

    printf("</body></html>"); 

    return 0;
}



