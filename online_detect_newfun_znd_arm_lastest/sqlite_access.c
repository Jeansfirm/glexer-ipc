﻿/***************************************************************************
  Copyright (C), 2009-2014 GuangdongGuanglian Electronic Technology Co.,Ltd.
  FileName:      sqlite_access.c
  Author:        jiang
  Version :      1.0    
  Date:          2014-08-05
  Description:   实现数据库初始化，从数据库中取回IP地址      
  History:         
      <author>  <time>   <version >   <desc> 
***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <dlfcn.h> 
#include <stdlib.h>

#include "sqlite3.h"
#include "sqlite_access.h"

sqlite3 * db;
//char* errmsg=NULL;
char *ipc_conf_database = "/gl/etc/video/video_conf.db";

//test
unsigned int open_time = 0;
//test

/**************************************************************************************
  Function:       add_marks(char *str,int comma)
  Description:    ��ӱ�����
  Input:          
                  
  Output:         
  Return:           
  Others:         
**************************************************************************************/
void add_marks(char *str,int comma)
{
    char temp[200]="";

    if(comma == 1)
    {
        strcat(temp,",'");
    }
    else
    {
       strcat(temp,"'");
    }

    strcat(temp,str);
	strcat(temp,"'");
	memset(str,0,strlen(str));
	strcpy(str,temp);
} 

/**************************************************************************************
  Function:       init_database()
  Description:    初始化数据库
  Input:          
                  
  Output:         
  Return:       0:成功，-1打开数据库失败 ，-2建数据库表失败     
  Others:         
***************************************************************************************/
int init_database()
{
	int result;
    //sqlite3 * db;
    int ret = 0;
    //创建一表，表名叫video_conf
    const char *create_table_sql="create table if not exists video_conf(id int primary key,ipaddr varchar(60),rtspport varchar(20),\
                                                httpport varchar(20),name varchar(50),password varchar(50),aliases nvarchar(100),indexid varchar(10),roomid varchar(10),ipc_status varchar(10),\
                                                DomainName nvarchar(100),SerialNumber nvarchar(100))";
    char* errmsg=NULL;
   
    //open_time++;
    //printf("****database open_time = %d\r\n",open_time);  
   
    //result=sqlite3_open("video_conf.db",&db);
    result=sqlite3_open(ipc_conf_database,&db);
	if(result !=SQLITE_OK)
	{
		//数据库打开失败
        printf("open sqlite db faile,result = %d\r\n",result);
        //printf("<p>addipc open sqlite db faile</p>");
		ret = -1;
	}
    
    //建表  
    result = sqlite3_exec(db,create_table_sql, NULL, NULL, &errmsg);
	if(result != SQLITE_OK )  
	{  
		 printf("error:%d,reasion:%s\n",result,errmsg); 
         //printf("<p>error%d,reasion:%s</p>",result,errmsg);
         ret = -2;
	} 
    sqlite3_free(errmsg);
    return ret;
}

/**************************************************************************************
  Function:      uninit_database() 
  Description:   释放数据库
  Input:          
                  
  Output:         
  Return:            
  Others:         
***************************************************************************************/
int uninit_database()
{
	//关闭数据库  
    //sqlite3_free(errmsg);

	//sqlite3_close(db);
    int close_ret;

    if(db != NULL)
    {
        close_ret = sqlite3_close(db);
        if(close_ret != SQLITE_OK)
        {
            printf("close_ret = %d\r\n",close_ret);
        }


        if(close_ret)
        db = NULL;
    }
    return 0;
}

/**************************************************************************************
  Function:       find_ipc(sqlite3 *db,cstruct etc_msg *ipc_etc)
  Description:    根据IP地址查找数据库中是否存在
  Input:          
                  
  Output:         
  Return:         0：查询数据库操作成功：-1：操作数据库失败   
  Others:         
***************************************************************************************/ 
int find_ipc(sqlite3 *db,struct etc_msg *ipc_etc) 
{  
    int result;
    char **dbResult; //是 char ** 类型，两个*号
    int nrow, nColumn,index;
    char* errmsg=NULL;
    int return_ret = -1;
    int i,j,id;
    

    //char target_ipc_id[20];

    //char check_ip_sql[200] = "select * from video_conf where id = '1'";
    //char check_ip_sql[200] = "select * from video_conf where id =";
    //char check_ip_sql[200] = "select * from video_conf order by id";
    char check_ip_sql[200] = "select id,ipaddr from video_conf order by id";//按id排序，取出id、ipaddr

    //sprintf(target_ipc_id, "%d", ipc_num);//数字转字符
    //strcat(check_ip_sql,target_ipc_id);

	result = sqlite3_get_table(db,check_ip_sql, &dbResult, &nrow, &nColumn, &errmsg );
	if(SQLITE_OK == result)//查询成功
	{
		//ip不存在
        if(nrow == 0)
        {
            return_ret = 0;
        }
        else//ip存在
        {
            index = nColumn; //dbResult 前面第一行数据是字段名称，从 nColumn 索引开始才是真正的数据，字段名称只有id、ipaddr

		    for(j=0;j<nrow;j++)
		    {
                id = atoi(dbResult[index]);
		        for(i=0;i<MAX_IPC_NUM;i++)//查找出是哪个IPC
		        {
                    if(i == id)
                    {
                        strcpy(ipc_etc->ipc_list[i].ip_addr,dbResult[index+1]);
                        //printf("id:%s,ipaddr:%s\r\n",dbResult[index],dbResult[index+1]);
                        continue;
                    }
		        }
                index += nColumn;//一条记录总共有nColumn项
		    }
            return_ret = 0;
        }
    }
    else//查询不成功
    {
        return_ret = -1;
        printf("sqlite3_get_table error:%d,reasion:%s\n",result,errmsg); 
    }
  
    sqlite3_free(errmsg);
    sqlite3_free_table(dbResult);
 
    return return_ret;
} 

/**************************************************************************************
  Function:       find_ipc_etc(int ipc_num ,struct config_msg *ipc_etc)
  Description:    
  Input:          
                  
  Output:         
  Return:         0：查询数据库成功，-1：操作数据库失败   
  Others:         
***************************************************************************************/
int find_ipc_etc(struct etc_msg *ipc_etc)
{
    int ret;
    ret = find_ipc(db,ipc_etc);
    //printf("44444***ret = %d\r\n",ret);
    return ret;
}




/**************************************************************************************
  Function:       read_init_status(char *init_status_array);
  Description:   ��һ�ζ������ݿ���10��IPC��״̬��aΪ���ߣ�bΪ�����ߣ�cΪû���������
  Input:             
                  
  Output:    ��һ�ζ�����״̬�洢��init_status_array ������      
  Return:    0:����ɹ�;-1:����ʧ��       
  Others:         
***************************************************************************************/

int read_init_status(char *init_status_array)
{
	int ret=0;
	char array[2];
	int i,j,id;
	int status_sign[11];
	for(i=0;i<10;i++)
	{
		status_sign[i]=0;
	}
	int result;
	char **dbResult; // ��char **���ͣ�����*��
	int nrow, nColumn,index;
	char* errmsg=NULL;
	
	char read_status_sql[200] = "select id,ipc_status from video_conf order by id";//��id����ȡ��id��ipc_status
	result = sqlite3_get_table(db,read_status_sql, &dbResult, &nrow, &nColumn, &errmsg );
	if(SQLITE_OK == result)
	{
		index = nColumn;
		for(j=0;j<nrow;j++)
	    {
			id = atoi(dbResult[index]);
			for(i=0;i<MAX_IPC_NUM;i++)  // �ҳ����ĸ�IPC
			{
				if(i == id)
				{
					strcpy(array,dbResult[index+1]);
					array[1]='\0';
					status_sign[i]=1;
					init_status_array[i]=array[0];
					continue;
				 }

			}
			index += nColumn;
		}
		for(i=0;i<MAX_IPC_NUM;i++)
		{
			if(status_sign[i]==0)
			{
				init_status_array[i]='c';
			}
		}
		init_status_array[MAX_IPC_NUM]='\0';
	}
	else
	{
		ret=-1;
	}
	sqlite3_free(errmsg);
	sqlite3_free_table(dbResult);
	return ret;
}


/**************************************************************************************
  Function:       check_list_len(char *index_list)
  Description:    ���ipc_index_list�ﲻΪ'*'�ĸ����Ƿ���������洢���������
  Input:          
                  
  Output:         
  Return:       0:��Ϊ'*'�ĸ�����������洢��������ȣ�-1:��ȡ������������ʧ�ܣ�
  				-2:��Ϊ'*'�ĸ�����������洢�����������
  Others:         
***************************************************************************************/
int check_list_len(char *newstatus)
{
	int i;
	int index_count=0;
	int ret=0;
	int result;
	int nrow, nColumn;
	char* errmsg=NULL;
	char **dbResult; //�� char ** ���ͣ�����*��
	
	for(i=0;i<MAX_IPC_NUM;i++)
	{  
		if(newstatus[i]!='c')
		{
			index_count++;
		}
	}
	//printf("index_count=%d\n",index_count);
	result = sqlite3_get_table(db,"select * from video_conf order by id", &dbResult, &nrow, &nColumn, &errmsg );
	//printf("<p>nrow=%d\n</p>",nrow);
	if(SQLITE_OK == result)//��ѯ�ɹ�
	{
		if(index_count!=nrow)
		{
			ret=-2;
		}
	}
	else
	{
		ret=-1;
		printf("get table failed reason:%s\n",errmsg);
	}
	//printf("<p>ret=%d\n</p>",ret);
	return ret;
}
/**************************************************************************************
  Function:       check_size(char index)
  Description:    ��������Ԫ�ص�ֵ�Ƿ�Ϊ0~9
  Input:          
                  
  Output:         
  Return:       0:�ɹ���-1:������Ԫ�ص�ֵ��Ϊ0~9
  Others:         
***************************************************************************************/

int check_size(char index)
{
	int ret;
	//int index_value_int;
   // char index_value_char[10];
	//index_value_char[0]=index;
	//index_value_char[1]='\0';
	//index_value_int=atoi(index_value_char);
	//if((index_value_int>=0)&&(index_value_int<MAX_IPC_NUM))
	if((index>='a')&&(index<='b'))
	{
		ret=0;
	}
	else
	{
		ret=-1;
	}
	//printf("<p>ret=%d\n</p>",ret);
	return ret;
}
/**************************************************************************************
  Function:       check_row(int i)
  Description:    �����������Ƿ����idΪi����
  Input:          
                  
  Output:         
  Return:       0:�ɹ���-1:������ -2:��ȡ����ʧ��
  Others:         
***************************************************************************************/

int check_row(int i)
{
	char select_index_sql[500] ="select * from video_conf where id =";
	char array[11]="0123456789";
	char a[10];
	int result;
	int nrow, nColumn;
	char* errmsg=NULL;
	char **dbResult; //�� char ** ���ͣ�����*��
	int ret;
		
	a[0]=array[i];
	a[1]='\0';
	strcat(select_index_sql,a);
	result = sqlite3_get_table(db,select_index_sql, &dbResult, &nrow, &nColumn, &errmsg );
	if(SQLITE_OK == result)
	{
		if(nrow==1)
		{
			ret=0;
		}
		else 
		ret=-1;
	}
	else
	{
		ret=-2;
	}
	return ret;
}


/**************************************************************************************
  Function:       update_index_value(int i,char index_value)
  Description:    �����кŲ��뵽��Ӧ���к���
  Input:          
                  
  Output:         
  Return:       0:�ɹ���-1:����indexֵʧ��
  Others:         
***************************************************************************************/

int update_status_value(int i,char status_value)
{
	char update_sql[500] ="update video_conf set ipc_status = ";
	char array[11]="0123456789";
	char a[10];
	char b[10];
	int result;
	int ret;
	char* errmsg=NULL;
	
	a[0]=array[i];
	a[1]='\0';
	b[0]=status_value;
	b[1]='\0';
	add_marks(b,0);
	strcat(update_sql,b);
	strcat(update_sql," where id = ");
	strcat(update_sql,a);	
	//printf("update_sql:%s\n",update_sql);
	result = sqlite3_exec(db,update_sql, NULL, NULL, &errmsg);
	if(SQLITE_OK == result)
	{
		ret=0;
	}
	else 
		ret=-1;
}


/**************************************************************************************
  Function:       int update_status(sqlite3 *db��char *newstatus)
  Description:    �������ݿ��¼
  Input:          
                  
  Output:         
  Return:       0:�ɹ���-1��-3:��ȡ���ݿ���Ϣʧ�� ,-2:�µ�����˳��ŵĸ��������ݿ����������� 
  				-4:���������ַ� -5:���ݿ�û�д��ڶ�Ӧ��id��,-6: ����indexidʧ��
  Others:         
***************************************************************************************/

int update_status(sqlite3 *db, char *newstatus)
{
	int i;
	int return_ret=0;
 	int len_ret;
	int size_ret;
	int update_ret;
	int row_ret;
	
    len_ret=check_list_len(newstatus);
	if(len_ret==-1)
	{
		return_ret=-1;
		goto over;
	}
	if(len_ret==-2)
	{
		return_ret=-2;
		goto over;
	}

	for(i=0;i<MAX_IPC_NUM;i++)
	{
		if(newstatus[i]!='c')
		{
			size_ret=check_size(newstatus[i]);
		    if(size_ret==-1)
		    {
				return_ret=-4;
				goto over;
		    }
			row_ret=check_row(i);
			if(row_ret==-1)
			{
				return_ret=-5;
				goto over;
			}
			if(row_ret==-2)
			{
				return_ret=-3;
				goto over;
			}
			//printf("newstatus[%d]:%c\n",i,newstatus[i]);
			update_ret=update_status_value(i,newstatus[i]);
			if(update_ret==-1)
			{
				return_ret=-6;
				goto over;
			}
		}
	}
	over:
	return return_ret;
	
}


/**************************************************************************************
  Function:       update_ipc_status(char *new_status);
  Description:    ����������ipc_status�ֶ�
  Input:          
                  
  Output:         
  Return:      0:�ɹ���-1��-3:��ȡ���ݿ���Ϣʧ�� ,-2:�µ�����˳��ŵĸ��������ݿ����������� 
  				-4:���������ַ� -5:���ݿ�û�д��ڶ�Ӧ��id��,-6: ����indexidʧ��
  Others:         
***************************************************************************************/

int update_ipc_status(char *new_status)
{
	int update_index_ret=0;
	update_index_ret=update_status(db,new_status);	
	return update_index_ret;
}

