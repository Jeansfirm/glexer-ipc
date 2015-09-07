/***************************************************************************
  Copyright (C),  2009-2014 GuangdongGuanglian Electronic Technology Co.,Ltd.
  File name:      public.h
  Description:    定义公用的相关变量和宏
  Author:         jiang   
  Version:        1.0       
  Date:           2014-06-23
  History:        
                  
    1. Date:
       Author:
       Modification:
    2. ...
***************************************************************************/
#ifndef _PUBLIC_H
#define _PUBLIC_H
#define MAX_IPC_NUM                   10 //最大IPC数目

//回应客户端json类型
#define PARSE_SUCCESS	                   0     //解析JSON成功
#define GET_IPC_STATUS_SUCCESS	           1     //获取IPC状态成功
#define JSON_PARSE_FAILED	              -1     //json解析错误
#define INVALID_ACTION	                  -2     //action键不存在，或action键值不是string类型
#define UNKNOWN_ACTION	                  -3     //不能识别和处理的action值
#define ACCESS_DATABASE_FAILED            -4     //操作数据库失败
//#define GET_IPC_STATUS_NOT_ALL_SUCCESS    -5     //获取IPC状态部分成功，部分失败（打开IPC配置文件失败）
#define REACH_MAX_CLIENT_CONNECT          -5     //客户端链接达到最大数

#endif
