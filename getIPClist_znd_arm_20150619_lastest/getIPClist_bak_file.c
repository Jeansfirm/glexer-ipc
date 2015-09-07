/***************************************************************************
  Copyright (C), 2009-2014 GuangdongGuanglian Electronic Technology Co.,Ltd.
  FileName:      gedIPClist.c
  Author:        jiang
  Version :      1.0    
  Date:          2014-04-16
  Description:   实现获取IPC列表      
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

/***************************************************************************
  Function:       package_ipclist_json(struct ipc_list package_ipc) 
  Description:    封装ipc列表成JSON格式数据
  Input:          消息类型
                  
  Output:         
  Return:         封装json后转化为char类型的数据
  Others:         
****************************************************************************/
char *package_ipclist_json(struct ipc_list package_ipc ,char type)
{
    cJSON *root,*fmt,*list,*fld;
    char *out;	
    int j = 0;

    root=cJSON_CreateObject();	
    cJSON_AddStringToObject(root,"action","getipclist");
    cJSON_AddItemToObject(root,"response_params",fmt=cJSON_CreateObject());
   
    //cJSON_AddNumberToObject(fmt,"status",0);
    /*
    if(type == 0)
    {
    	cJSON_AddStringToObject(fmt,"status_msg","get ipc list success");
    }
    else
    {
    	cJSON_AddStringToObject(fmt,"status_msg","get ipc list incomplete");
    }
    */

    switch(type)
    {
        case 0:
		{
            cJSON_AddNumberToObject(fmt,"status",0);
			cJSON_AddStringToObject(fmt,"status_msg","get ipc list success");
			break;
		}
        case 1:
		{
			cJSON_AddNumberToObject(fmt,"status",1);
			cJSON_AddStringToObject(fmt,"status_msg","get ipc list open database failed");
			break;
		}
        case 2:
		{
			cJSON_AddNumberToObject(fmt,"status",2);
			cJSON_AddStringToObject(fmt,"status_msg","get ipc list access database failed");
			break;
		}
        case 3:
		{
			cJSON_AddNumberToObject(fmt,"status",3);
			cJSON_AddStringToObject(fmt,"status_msg","get ipc list failed");
			break;
		}
	    case 4:
		{
			cJSON_AddNumberToObject(fmt,"status",4);
			cJSON_AddStringToObject(fmt,"status_msg","no ipc list");
			break;
		}
        default:
		{
			cJSON_AddNumberToObject(fmt,"status",5);
			cJSON_AddStringToObject(fmt,"status_msg","error occur int getting ipc list");
			break;
		}
    }
        
    if(package_ipc.total_num > 0)
    {
		cJSON_AddItemToObject(root,"list",list=cJSON_CreateArray());
		for(j=0;j<package_ipc.total_num;j++)
		{
			cJSON_AddItemToArray(list,fld=cJSON_CreateObject());
			cJSON_AddNumberToObject(fld,"id",package_ipc.l_ipc_msg[j].id);
			cJSON_AddStringToObject(fld,"ipc_ipaddr",package_ipc.l_ipc_msg[j].ip_addr);
		    cJSON_AddStringToObject(fld,"name",package_ipc.l_ipc_msg[j].name);
			cJSON_AddStringToObject(fld,"password",package_ipc.l_ipc_msg[j].password);
			cJSON_AddStringToObject(fld,"rtspport",package_ipc.l_ipc_msg[j].rtspport);
		    cJSON_AddStringToObject(fld,"httpport",package_ipc.l_ipc_msg[j].httpport);
			cJSON_AddStringToObject(fld,"alias",package_ipc.l_ipc_msg[j].aliases);
			int indexid_int;
			indexid_int=atoi(package_ipc.l_ipc_msg[j].indexid);
			cJSON_AddNumberToObject(fld,"index",indexid_int);
			int roomid_int;
			roomid_int=atoi(package_ipc.l_ipc_msg[j].roomid);
			cJSON_AddNumberToObject(fld,"roomid",roomid_int);
			cJSON_AddStringToObject(fld,"ipc_status",package_ipc.l_ipc_msg[j].ipc_status);

			cJSON_AddStringToObject(fld,"DomainName",package_ipc.l_ipc_msg[j].DomainName);
			cJSON_AddStringToObject(fld,"SerialNumber",package_ipc.l_ipc_msg[j].SerialNumber);
		}
    }
    
    //out=cJSON_Print(root);
    out=cJSON_PrintUnformatted(root);
    
    cJSON_Delete(root);

    return out; 
}

/***************************************************************************
  Function:       package_json 
  Description:    封装JSON
  Input:          消息类型
                  
  Output:         
  Return:         封装json后转化为char类型的数据
  Others:         
***************************************************************************/
char *package_json(int type,char *msg)
{
    cJSON *root,*fmt;
    char *out;	
    //Our "Video" datatype: 
    root=cJSON_CreateObject();	
    cJSON_AddStringToObject(root, "action", "getipclist");
    cJSON_AddItemToObject(root, "response_params", fmt=cJSON_CreateObject());
    cJSON_AddNumberToObject(fmt,"status",     type);
    cJSON_AddStringToObject(fmt,"status_msg", msg);

    //out=cJSON_Print(root);
    out=cJSON_PrintUnformatted(root);
    
    cJSON_Delete(root);

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
    //printf("Content-type:text/html;charset=gb2312\n\n");         //cgi 输出html标准格式 ,输出中文显示
    printf("Content-type:text/html;charset=utf-8\n\n");            //cgi 输出html标准格式 ,输出中文显示
    printf("<html>"); 
    printf("<head><title>getIPClist</title></head><body>"); 
    //printf("<p>hello world</p>"); 

    //FILE *fp;   
    int err_num = 0;
    //char file_num = 0;
    char buf[512];
    char ipaddr[20];
    char name[20];
    char password[20];
    char rtspport[20];
    char httpport[20];
    char aliases[150]; 
    int i = 0;
    //int can_not_open_file = 0;
    int exist_file_num = 0;
    int lock_file_faile_num = 0;
    struct ipc_list l_ipc_list;
    
    //can_not_open_file = 0;
    l_ipc_list.total_num = 0;

    int init_ret;
    int get_ret;
    
    init_ret = init_database();
    if(init_ret == -1)
    {
        err_num = 1;
    }
    else if(init_ret == -2)
    {
        err_num = 2;
    }
    else
    {
        get_ret = getipclist(&l_ipc_list);
        if(get_ret == 1)
        {
            err_num = 4;
        }
        else if(get_ret == -1)
        {
            err_num = 3;
        }
        else
        {
            err_num = 0;
        }
    }
    uninit_database();
over:
	switch(err_num)
    {
        case 0:printf("<p>%s</p>",package_ipclist_json(l_ipc_list,0));break;

		case 1:printf("<p>%s</p>",package_ipclist_json(l_ipc_list,1));break;

		case 2:printf("<p>%s</p>",package_ipclist_json(l_ipc_list,2));break;

		case 3:printf("<p>%s</p>",package_ipclist_json(l_ipc_list,3));break;

		//case 4:printf("<p>%s</p>",package_ipclist_json(l_ipc_list,4));break;

		case 4:printf("<p>%s</p>",package_json(-4,"no ipc list"));break;

		//case 5:printf("<p>%s</p>",package_json(5,"input rtspport Parameter error exceed 0-65535"));break;

		//case 6:printf("<p>%s</p>",package_json(6,"ipc total num reach max ipc num 10"));break;

        //case 7:printf("<p>%s</p>",package_json(7,"gate write file error"));break;
        default:break;
    }

    printf("</body></html>"); 

    return 0;
}



