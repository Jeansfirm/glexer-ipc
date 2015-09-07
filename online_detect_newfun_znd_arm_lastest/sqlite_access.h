/***************************************************************************
  Copyright (C),  2009-2014 GuangdongGuanglian Electronic Technology Co.,Ltd.
  File name:      sqlite_access.h
  Description:    定义数据库操作的相关接口
  Author:         jiang   
  Version:        1.0       
  Date:           2014-08-05
  History:        
                  
    1. Date:
       Author:
       Modification:
    2. ...
**************************************************************************/

#ifndef _SQLITE_ACCESS_H
#define _SQLITE_ACCESS_H
#include "public.h"
//IPC信息结构体
struct config_msg
{
    char ip_addr[20];
    //char name[20];
    //char password[20];
    //char rtspport[20];
};
//IPC地址数据结构体
struct etc_msg 
{
  struct config_msg ipc_list[MAX_IPC_NUM];
};

int init_database();
int uninit_database();

int find_ipc_etc(struct etc_msg *ipc_etc);


int read_init_status(char *init_status_array);
int update_ipc_status(char *new_status);


#endif
