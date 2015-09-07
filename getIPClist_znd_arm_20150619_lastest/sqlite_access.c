/***************************************************************************
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
//extern c{ #include "sqlite3.h"};
#include "sqlite3.h"
#include "sqlite_access.h"

//#define MAX_IPC_NUM  10
char id[10] = "0123456789";
sqlite3 * db;
//char* errmsg=NULL;

char *ipc_conf_database = "/gl/etc/video/video_conf.db";

#if 0
/**************************************************************************************
  Function:       LoadMyInfo(void * para,int n_column,char ** column_value,char ** column_name)
  Description:    查询数据库的回调函数(测试使用)
  Input:          
                  
  Output:         
  Return:           
  Others:         
*************************************************************************************/
int LoadMyInfo(void * para,int n_column,char ** column_value,char ** column_name)  
{  
	//para是你在 sqlite3_exec 里传入的void * 参数  
	//通过para参数，你可以传入一些特殊的指针（比如类指针、结构指针），然后在这里面强制转换成对应的类型（这里面是void*类型，必须强制转换成你的类型才可用）。然后操作这些数据  
	//n_column是这一条记录有多少个字段 (即这条记录有多少列)  
	// char ** column_value 是个关键值，查出来的数据都保存在这里，它实际上是个1维数组（不要以为是2维数组），每一个元素都是一个 char * 值，是一个字段内容（用字符串来表示，以\0结尾）  
	//char ** column_name 跟column_value是对应的，表示这个字段的字段名称  
	//这里，我不使用 para 参数。忽略它的存在.  
	int i;  

	//printf("include %d column\n", n_column );
    printf("<p>include %d column</p>",n_column);
	for( i = 0 ; i < n_column; i ++ )  
	{  
		//printf("column name:%s> column data:%s\n",  column_name[i], column_value[i] );
        printf("<p>column name:%s> column data:%s</p>",column_name[i], column_value[i]);
	}  
	//printf("------------------\n");  
    printf("<p>------------------</p>");   
	return 0;
}  

/**************************************************************************************
  Function:       add_marks(char *str,int comma)
  Description:    添加标点符号
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
  Function:       find_id(sqlite3 *db,char *target_id)
  Description:    超找出数据库中可用的ipc id
  Input:          sqlite3 *db,char *target_id
                  
  Output:         
  Return:         0：找到数据库中最排前的空缺ipc id,1：数据库中ipc列表已满,-1：数据库查询失败   
  Others:         
***************************************************************************************/ 
int find_id(sqlite3 *db,char *target_id) 
{  
    int result;
    char **dbResult; //是 char ** 类型，两个*号
    int nrow, nColumn;
    int i,j;
    int index;
    char* errmsg=NULL;
    int return_ret = -1;
    char full_ipc = 0;
    char ipc_id[MAX_IPC_NUM] = {0};
    char target;

	//开始查询，传入的 dbResult 已经是 char **，这里又加了一个 & 取地址符，传递进去的就成了 char ***
	result = sqlite3_get_table(db,"select * from video_conf", &dbResult, &nrow, &nColumn, &errmsg );
	if(SQLITE_OK == result)//查询成功
	{
		//数据库为空，没有添加IPC
        if(nrow == 0)
        {
            target_id[0] = id[0];
            target_id[1] = '\0';
            return_ret = 0;
        }
        else//数据库不为空，有添加IPC
        {
		    //取一个IPC ID查询数据库中是否有
		    for(j=0;j<MAX_IPC_NUM;j++)
		    {
                index = nColumn; //dbResult 前面第一行数据是字段名称，从 nColumn 索引开始才是真正的数据
		        for(i=0;i<nrow;i++)
		        {
                    //target = dbResult[index][0];
		            //if(id[j] == target)//记录中的第一项是id
                    if(id[j] == dbResult[index][0])//记录中的第一项是id
		            {
		                ipc_id[j] = 1;//标记ipc已添加
                        //printf("id[%d] = %c\r\n",j,id[j]);
                        //printf("****ret = %d,id = %s****\r\n",ret,findid);
                        //printf("id[%d] = %c****</p>",j,id[j]);
		                
		            }
		            index += 7;//一条记录总共有7项
		        }
		    }

		    //查询完所IPC ID
		    for(i=0;i<MAX_IPC_NUM;i++)
		    {
		        if(ipc_id[i]==0)//找到最前面的空缺的ipc id
		        {
		            target_id[0] = id[i];
                    target_id[1] = '\0';
                    return_ret = 0;
		            break;
		        }
                else
                {
                    full_ipc++;
                }
		    }
             
            if(full_ipc == MAX_IPC_NUM)//列表已满
            {
                return_ret = 1;
            }

        }//else//数据库不为空，有添加IPC
 
    }//if(SQLITE_OK == result)//查询成功
    else//查询不成功
    {
        return_ret = -1;
        //printf("error:%d,reasion:%s\n",result,errmsg); 
        //printf("<p>error%d,reasion:%s</p>",result,errmsg);
    }
  
    sqlite3_free(errmsg);
    sqlite3_free_table(dbResult);
  
    return return_ret;
} 

/**************************************************************************************
  Function:       udate_datebase(sqlite3 *db,char *id, char *ipaddr, char *rtspport, char *httpport,
                              char *name, char *password, char *aliases)
  Description:    更新数据库列表
  Input:          
                  
  Output:         
  Return:      0:成功，-1失败       
  Others:         
***************************************************************************************/
int udate_datebase(sqlite3 *db,char *id, char *ipaddr, char *rtspport, char *httpport,
                              char *name, char *password, char *aliases)
{
    char* errmsg=NULL;

    int result;
    int ret=0;

    //char ipaddr2[20]="192.168.1.233";
	char rtsp_str[30]=",rtspport=";
	char http_str[30]=",httpport=";
	char name_str[30]=",name="; 
	char password_str[30]=",password ="; 
    char aliases_str[150]=",aliases="; 

   // char update_sql0[500] ="update video_conf set ipaddr = '192.168.1.199',rtspprot =  WHERE id = 0";
    char update_sql[500] ="update video_conf set ipaddr = ";
   
    add_marks(ipaddr,0);
    strcat(update_sql,ipaddr);

    add_marks(rtspport,0);
    strcat(rtsp_str,rtspport);
    strcat(update_sql,rtsp_str);

    add_marks(httpport,0);
    strcat(http_str,httpport);
    strcat(update_sql,http_str);

    add_marks(name,0);
    strcat(name_str,name);
    strcat(update_sql,name_str);

    add_marks(password,0);
    strcat(password_str,password);
    strcat(update_sql,password_str);

    add_marks(aliases,0);
    strcat(aliases_str,aliases);
    strcat(update_sql,aliases_str);

    strcat(update_sql,"WHERE id = ");
    
    strcat(update_sql,id);

    result = sqlite3_exec(db,update_sql, NULL, NULL, &errmsg);
    //result = sqlite3_exec(db,update_sql0, NULL, NULL, &errmsg);
	if(result != SQLITE_OK )  
	{  
        ret = -1;
		printf("error:%d,reasion:%s\n",result,errmsg); 
	}
    sqlite3_free(errmsg);
    return ret;
}

/**************************************************************************************
  Function:       int delete_database(sqlite3 *db ,char *ipc_id)
  Description:    删除数据库记录
  Input:          
                  
  Output:         
  Return:   0:成功，-1失败         
  Others:         
***************************************************************************************/
int delete_database(sqlite3 *db ,char *ipc_id)
{
    char* errmsg=NULL;
    int result;
    char delete_sql[200] ="delete from video_conf where id = ";
    int ret = 0;

    strcat(delete_sql,ipc_id);
    result = sqlite3_exec(db,delete_sql,NULL,NULL,&errmsg);
    if(result != SQLITE_OK )  
	{  
		printf("error:%d,reasion:%s\n",result,errmsg); 
        ret = -1;
	}
    sqlite3_free(errmsg);
    return ret;
}

/**************************************************************************************
  Function:       int insert_database(sqlite3 *db,char *id, char *ipaddr, 
                                      char *rtspport, char *httpport,
                                      char *name, char *password, char *aliases)
  Description:    插入数据库记录
  Input:          
                  
  Output:         
  Return:       0:成功，-1失败      
  Others:         
***************************************************************************************/

int insert_database(sqlite3 *db,char *id, char *ipaddr, char *rtspport, char *httpport,
                              char *name, char *password, char *aliases)
{
    char* errmsg=NULL;
    int result;
    char insert_sql[500] ="insert into video_conf(id,ipaddr,rtspport,httpport,name,password,aliases) values(";
    int ret = 0;

    strcat(insert_sql,id);

    add_marks(ipaddr,1);
    strcat(insert_sql,ipaddr);

    add_marks(rtspport,1);
    strcat(insert_sql,rtspport);

    add_marks(httpport,1);
    strcat(insert_sql,httpport);

    add_marks(name,1);
    strcat(insert_sql,name);

    add_marks(password,1);
    strcat(insert_sql,password);

    add_marks(aliases,1);
    strcat(insert_sql,aliases);

    strcat(insert_sql,")");

    result = sqlite3_exec(db,insert_sql, NULL, NULL, &errmsg);
	if(result != SQLITE_OK )  
	{  
		//printf("error:%d,reasion:%s\n",result,errmsg); 
        ret = -1;
	}
    sqlite3_free(errmsg);

    return ret;
}
#endif

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

    //result=sqlite3_open("video_conf.db",&db);
    result=sqlite3_open(ipc_conf_database,&db);
	if(result !=SQLITE_OK)
	{
		//数据库打开失败
        //printf("open sqlite db faile\r\n");
        printf("<p>addipc open sqlite db faile</p>");
		ret = -1;
	}
    
    //建表  
    result = sqlite3_exec(db,create_table_sql, NULL, NULL, &errmsg);
	if(result != SQLITE_OK )  
	{  
		 //printf("error:%d,reasion:%s\n",result,errmsg); 
         printf("<p>error%d,reasion:%s</p>",result,errmsg);
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
	sqlite3_close(db);
    return 0;
}

/**************************************************************************************
  Function:       find_id(sqlite3 *db,char *target_id)
  Description:    超找出数据库中可用的ipc id
  Input:          sqlite3 *db,char *target_id
                  
  Output:         
  Return:         0：获取数据库列表成功并有数据 ,1：获取数据库列表成功但没有数据,-1：数据库查询失败   
  Others:         
***************************************************************************************/ 
int getlist(sqlite3 *db,struct ipc_list *ipclist_buf) 
{  
    int result;
    char **dbResult; //是 char ** 类型，两个*号
    int nrow, nColumn;
    int i,j;
    int index;
    char* errmsg=NULL;
    int return_ret = -1;
    char full_ipc = 0;
    char ipc_id[MAX_IPC_NUM] = {0};
    char target;

	//开始查询，传入的 dbResult 已经是 char **，这里又加了一个 & 取地址符，传递进去的就成了 char ***
	result = sqlite3_get_table(db,"select * from video_conf order by id", &dbResult, &nrow, &nColumn, &errmsg );
	if(SQLITE_OK == result)//查询成功
	{
		//数据库为空，没有IPC
        if(nrow == 0)
        {
            ipclist_buf->total_num = nrow;
            return_ret = 1;
        }
        else//数据库不为空，有IPC
        {
            ipclist_buf->total_num = nrow;
            
            index = nColumn; //dbResult 前面第一行数据是字段名称，从 nColumn 索引开始才是真正的数据

		    for(j=0;j<nrow;j++)
		    {
		        //for(i=0;i<nColumn;i++)
		        {
                    ipclist_buf->l_ipc_msg[j].id = atoi(dbResult[index]);
                    strcpy(ipclist_buf->l_ipc_msg[j].ip_addr,dbResult[index+1]);
                    strcpy(ipclist_buf->l_ipc_msg[j].rtspport,dbResult[index+2]);
                    strcpy(ipclist_buf->l_ipc_msg[j].httpport,dbResult[index+3]);
                    strcpy(ipclist_buf->l_ipc_msg[j].name,dbResult[index+4]);
                    strcpy(ipclist_buf->l_ipc_msg[j].password,dbResult[index+5]);
                    strcpy(ipclist_buf->l_ipc_msg[j].aliases,dbResult[index+6]);
					
					strcpy(ipclist_buf->l_ipc_msg[j].indexid,dbResult[index+7]);
					strcpy(ipclist_buf->l_ipc_msg[j].roomid,dbResult[index+8]);
					strcpy(ipclist_buf->l_ipc_msg[j].ipc_status,dbResult[index+9]);

					strcpy(ipclist_buf->l_ipc_msg[j].DomainName,dbResult[index+10]);
					strcpy(ipclist_buf->l_ipc_msg[j].SerialNumber,dbResult[index+11]);
		        }
                index += nColumn;//一条记录总共有10项
		    }
            return_ret = 0;
        }//else//数据库不为空，有IPC
        
    }//if(SQLITE_OK == result)//查询成功
    else//查询不成功
    {
        return_ret = -1;
        //printf("error:%d,reasion:%s\n",result,errmsg); 
        //printf("<p>error%d,reasion:%s</p>",result,errmsg);
    }
  
    sqlite3_free(errmsg);
    sqlite3_free_table(dbResult);
  
    return return_ret;
} 

/**************************************************************************************
  Function:       getipclist(struct ipc_list *ipclist_buf)
  Description:    
  Input:          
                  
  Output:         
  Return:         0：获取数据库列表成功并有数据 ,1：获取数据库列表成功但没有数据,-1：数据库查询失败   
  Others:         
***************************************************************************************/
int getipclist(struct ipc_list *ipclist_buf)
{
    int getlist_ret;
    getlist_ret = getlist(db,ipclist_buf);
    return getlist_ret;
}


