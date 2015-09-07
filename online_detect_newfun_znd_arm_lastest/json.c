/***************************************************************************
  Copyright (C), 2009-2014 GuangdongGuanglian Electronic Technology Co.,Ltd.
  FileName:      json.cpp
  Author:        jiang
  Version :      1.0    
  Date:          2014-03-19
  Description:   实现封装和解析json相关接口      
  History:         
      <author>  <time>   <version >   <desc> 
***************************************************************************/
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include"json.h"
#include"cJSON.h"
#include"public.h"

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>

/***************************************************************************
  Function:       parse_json(char *text) 
  Description:    解析接收到客户端的请求视频JSON数据
  Input:          接收到的socket字符串
                  
  Output:         
  Return:         视频通道
  Others:         
***************************************************************************/
int parse_json(char *text)
{
    char *strName = NULL; 
    cJSON* name = NULL;
    cJSON* ch = NULL;
    int channel = -1;
    cJSON *json;
    int ret = 0;
    
    json=cJSON_Parse(text);
    if (!json)
    {
        printf("****Parse JSON Error before: [%s]\n****",cJSON_GetErrorPtr());
        ret = JSON_PARSE_FAILED;//json串解析失败
    }
    else
    {
        name = cJSON_GetObjectItem(json, "action");  
 
        if((name == NULL)||(name->type != cJSON_String))
        {
            ret = INVALID_ACTION;//action数据不可用
            return ret;
        } 
         
		strName = cJSON_GetObjectItem(json, "action")->valuestring;
		if(strcmp(strName,"getipcstatus")==0)
		{
		    //printf("****strName = %s\n",strName);
            ret = PARSE_SUCCESS;
		}
		else
		{
		    ret = UNKNOWN_ACTION;//action不可知
		    printf("****receive client start video unknown json****\r\n");
		} 
                 
        //out=cJSON_Print(json);
        //cJSON_Delete(json); //test
        free(strName);
        strName = NULL;
        //printf("%s\n",out);
        //free(out);
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
char *package_json(int type,char *msg)
{
    cJSON *root,*fmt;
    char *out;	
    char *key[MAX_IPC_NUM];
    int onlist = 0;
    int i;
/*
    key[0] = "id0";
    key[1] = "id1";
    key[2] = "id2";
    key[3] = "id3";
    key[4] = "id4";
    key[5] = "id5";
    key[6] = "id6";
    key[7] = "id7";
    key[8] = "id8";
    key[9] = "id9";
  */ 
    root=cJSON_CreateObject();	
    switch(type)
    {
        case GET_IPC_STATUS_SUCCESS:
        //case GET_IPC_STATUS_NOT_ALL_SUCCESS:
        {
/*
            for(i=0;i<MAX_IPC_NUM;i++)//统计添加到网关的IPC数目
            {
                if(msg[i]!=0xf)
                {
                    onlist = onlist | (1<<i);
                }
            } 
*/ 
            cJSON_AddNumberToObject(root,"status",type);//全部在列表的ipc配置文件都可以打开      

            //cJSON_AddNumberToObject(root,"onlist",onlist);
            cJSON_AddStringToObject(root,"onlist",msg);
/*
            for(i=0;i<MAX_IPC_NUM;i++)
            {
                if((msg[i] ==0) || (msg[i] ==1)|| (msg[i] ==2))
                {
                    cJSON_AddNumberToObject(root,key[i],msg[i]);
                }
            }
*/
           break;
       }//case GET_IPC_STATUS_SUCCESS:
       case JSON_PARSE_FAILED:
       case INVALID_ACTION:
       case UNKNOWN_ACTION:
       case ACCESS_DATABASE_FAILED:
       case REACH_MAX_CLIENT_CONNECT:
       {
           cJSON_AddNumberToObject(root,"status", type);
           break;
       }
       default:break;
    }//switch(type)

    //out=cJSON_Print(root);//ok 有带空格
    out=cJSON_PrintUnformatted(root);//不带空格
    
    cJSON_Delete(root);

    return out; 
}
/***************************************************************************
  Function:       package_json_heartbeat 
  Description:    封装JSON
  Input:          消息类型
                  
  Output:         
  Return:         封装json后转化为char类型的数据
  Others:         
****************************************************************************/
 char *package_json_heartbeat()
{
    cJSON *root,*fmt;
    char *out1;
    char *data="716871768372";

    root=cJSON_CreateObject();	
    //cJSON_AddNumberToObject(root,"heartbeat",data);
    cJSON_AddStringToObject(root,"heartbeat",data);     

     //out=cJSON_Print(root);//ok 有带空格
    out1=cJSON_PrintUnformatted(root);//不带空格
    cJSON_Delete(root);
     return out1; 
}

/***************************************************************************
  Function:       response_msg(char type)
  Description:    网关给链接的客户端发送回应消息
  Input:                            
  Output:         
  Return:         
  Others:         
*************************************************************************/
//void response_msg(char type,int tmp_socket)
//void response_msg(int type,int *tempbuf,int tmp_socket)
void response_msg(int type,char *tempbuf,int tmp_socket)
{
    char *response_buf;
    int ret = 0;
    response_buf = package_json(type,tempbuf);
    //printf("%s\n",response_buf);
    if(response_buf != NULL)
    {
		ret = send(tmp_socket,response_buf, strlen(response_buf), MSG_DONTWAIT); //非阻塞发送
		if(ret<0)
		{
			printf(":::: send ipc online msg faile::::\r\n");
		}
		else
		{
		    //printf(":::: send ipc online msg success::::\r\n");
		}
    }
    else
    {
           printf(":::: int ipc online package json error ::::\r\n");
    }
}
