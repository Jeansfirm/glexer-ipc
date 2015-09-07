#include <stdio.h>
#include <string.h>
#include <dlfcn.h> 
//extern c{ #include "sqlite3.h"};
#include "sqlite3.h"
#include "sqlite_access.h"

#define MAX_IPC_NUM  10
char id[11] = "0123456789";
sqlite3 * db;
char* errmsg=NULL;

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
#endif
#if 0
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
	sqlite3_close(db);
    return 0;
}
/**************************************************************************************
  Function:       check_list_len(char *index_list)
  Description:    检查ipc_index_list里不为'*'的个数是否与数据里存储的行数相等
  Input:          
                  
  Output:         
  Return:       0:不为'*'的个数与数据里存储的行数相等，-1:获取表里数据行数失败，
  				-2:不为'*'的个数与数据里存储的行数不相等
  Others:         
***************************************************************************************/
int check_list_len(char *index_list)
{
	int i;
	int index_count=0;
	int ret=0;
	int result;
	int nrow, nColumn;
	char* errmsg=NULL;
	char **dbResult; //是 char ** 类型，两个*号
	
	for(i=0;i<MAX_IPC_NUM;i++)
	{  
		if(index_list[i]!='*')
		{
			index_count++;
		}
	}
	//printf("<p>index_count=%d\n</p>",index_count);
	result = sqlite3_get_table(db,"select * from video_conf order by id", &dbResult, &nrow, &nColumn, &errmsg );
	//printf("<p>nrow=%d\n</p>",nrow);
	if(SQLITE_OK == result)//查询成功
	{
		if(index_count!=nrow)
		{
			ret=-2;
		}
	}
	else
	{
		ret=-1;
	}
	//printf("<p>ret=%d\n</p>",ret);
	return ret;
}
/**************************************************************************************
  Function:       check_size(char index)
  Description:    检查待插入元素的值是否为0~9
  Input:          
                  
  Output:         
  Return:       0:成功，-1:待插入元素的值不为0~9
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
	if((index>='0')&&(index<='9'))
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
  Description:    检查表里数据是否存在id为i的行
  Input:          
                  
  Output:         
  Return:       0:成功，-1:不存在 -2:获取数据失败
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
	char **dbResult; //是 char ** 类型，两个*号
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
  Description:    将排列号插入到对应的行号里
  Input:          
                  
  Output:         
  Return:       0:成功，-1:更新index值失败
  Others:         
***************************************************************************************/

int update_index_value(int i,char index_value)
{
	char update_sql[500] ="update video_conf set indexid = ";
	char array[11]="0123456789";
	char a[10];
	char b[10];
	int result;
	int ret;
	char* errmsg=NULL;
	
	a[0]=array[i];
	a[1]='\0';
	b[0]=index_value;
	b[1]='\0';
	strcat(update_sql,b);
	strcat(update_sql," where id = ");
	strcat(update_sql,a);	
	result = sqlite3_exec(db,update_sql, NULL, NULL, &errmsg);
	if(SQLITE_OK == result)
	{
		ret=0;
	}
	else 
		ret=-1;
}


/**************************************************************************************
  Function:       int update_database(sqlite3 *db，char *index_list)
  Description:    插入数据库记录
  Input:          
                  
  Output:         
  Return:       0:成功，-1和-3:获取数据库信息失败 ,-2:新的排列顺序号的个数与数据库的行数不相符 
  				-4:存在其他字符 -5:数据库没有存在对应的id号,-6: 更新indexid失败
  Others:         
***************************************************************************************/

int update_index(sqlite3 *db, char *index_list)
{
	int i;
	int return_ret=0;
 	int len_ret;
	int size_ret;
	int update_ret;
	int row_ret;
	
    len_ret=check_list_len(index_list);
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
		if(index_list[i]!='*')
		{
			size_ret=check_size(index_list[i]);
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
			update_ret=update_index_value(i,index_list[i]);
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
  Function:       update_database_index()
  Description:    更改数据里index字段
  Input:          
                  
  Output:         
  Return:      0:成功，-1和-3:获取数据库信息失败 ,-2:新的排列顺序号的个数与数据库的行数不相符 
  				-4:存在其他字符 -5:数据库没有存在对应的id号,-6: 更新indexid失败
  Others:         
***************************************************************************************/

int update_database_index(char *ipc_index_list)
{
	int update_index_ret=0;
	update_index_ret=update_index(db,ipc_index_list);	
	return update_index_ret;
}

