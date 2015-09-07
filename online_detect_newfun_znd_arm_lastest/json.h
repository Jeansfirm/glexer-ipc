/***************************************************************************
  Copyright (C),  2009-2014 GuangdongGuanglian Electronic Technology Co.,Ltd.
  File name:      json.h
  Description:    定义封装和解析json API
  Author:         jiang   
  Version:        1.0       
  Date:           2014-03-19
  History:        
                  
    1. Date:
       Author:
       Modification:
    2. ...
***************************************************************************/
#ifndef _JSON_H
#define _JSON_H
int parse_json(char *text);
//void package_json(char type,char *out);
//void package_json(char type,char *msg,char *out);
//char *package_json(char type,char *msg);
//void response_msg(char type,int tmp_socket);
//char *package_json(int type,char *msg);
//char *package_json(int type);
//int *package_json(int type,int *msg);
char *package_json(int type,char *msg);
//void response_msg(int type,int tmp_socket);
//void response_msg(int type,int *tempbuf,int tmp_socket);
char *package_json_heartbeat();
void response_msg(int type,char *tempbuf,int tmp_socket);
#endif
