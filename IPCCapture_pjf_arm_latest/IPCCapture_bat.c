/***************************************************************************
  Copyright (C), GuangdongGuanglian Electronic Technology Co.,Ltd.
  FileName:      IPCCapture.c
  Author:        ZhangNanDe
  Version :      1.0    
  Date:          2015-05-12
  Description:   IPC capture and return callback      
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


#include"cJSON.h"
#include"sqlite_access.h"
#include"online_detect.h"



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
  Function:       parse_data(char* inputstr,char *outputstr,char* value)
  Description:    find out the "outputstr" form "value"
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
  Function:       check_ipc_id(char *ipc_id)
  Description:    check whether the ipc_id is in the range of 0-10
  Input:          char* ipc_id
                  
  Output:         
  Return:          
  Others:         
***************************************************************************/
int check_ipc_id(char *ipc_id)
{
	int ipc_id_int;
	int ret=-1;
	check_ipc_id(ipc_id);
	ipc_id_int=atoi(ipc_id);
	if(ipc_id_int>=0&&ipc_id_int<10)
	{
		ret=0;
	}
    return ret;
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
    cJSON_AddStringToObject(root, "action", "addipc");
    cJSON_AddItemToObject(root, "response_params", fmt=cJSON_CreateObject());
    cJSON_AddNumberToObject(fmt,"status",     type);
    cJSON_AddStringToObject(fmt,"status_msg", msg);
    if(type == 0)
    {
        cJSON_AddNumberToObject(fmt,"ipc_id",0);
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
  Description:    IPCCapture
  Input:          
                  
  Output:         
  Return:         
  Others:         
***************************************************************************/

int main()
{
    printf("Content-type:text/html;charset=utf-8\n\n");      
    printf("<html>"); 
    printf("<head><title>addIPC</title></head><body>"); 


	int err_num;
    char *req_method; 
    char *input;
	char ipc_id[20];
	char flag[20];
	char ruleid[20];


	req_method = getenv("REQUEST_METHOD"); // GET or POST ?
    input = getcgidata(stdin, req_method);

	parse_data(input,ipc_id,"ipc_id=");
	if(ipc_id==NULL)
    {
		err_num = -2;
		goto over;
    }
	if(check_ipc_id(ipc_id)<0)
	{
		err_num=-3;
		goto over;
	}
	
	parse_data(input,flag,"flag=");
	if(ipc_id== NULL)
	{
		err_num = -2;
		goto over;
	}

	parse_data(input,ruleid,"ruleid=");
	if(ipc_id== NULL)
	{
		err_num = -2;
		goto over;
	}


	err_num=0;
	over:
	switch(err_num)
    {
		case 0:printf("<p>%s</p>",package_json(0,"add ipc success"));break;
        
        case -1:printf("<p>%s</p>",package_json(-1,"gateway write database failed error"));break;

		case -2:printf("<p>%s</p>",package_json(-2,"input parameter error"));break;

		case -3:printf("<p>%s</p>",package_json(-3,"input ipc_id parameter error exceed 0-10"));break;

		default:break;

	}
	printf("</body></html>"); 

	return 0;
}
