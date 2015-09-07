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
  Function:     findipfromid(char *ipc_id,char *ipaddr);
  Description: find out the ip according to the id 
  Input:          
                  
  Output:   output *ipaddr    
  Return:   0:ipc_id exit; -2:ipc_id  didn'texit; -1: operat database failed    
  Others:         
**************************************************************************************/
int findipfromid(char *ipc_id,char *ipaddr)
{
	int result;
    char **dbResult;
    int nrow, nColumn;
	char* errmsg=NULL;
	int ret=0;
	char select_sql[500] = "select * from video_conf where id = ";
	strcat(select_sql,ipc_id);
	//printf("<p>select_sql:%s\n</p>",select_sql);
	result = sqlite3_get_table(db,select_sql, &dbResult, &nrow, &nColumn, &errmsg );
	//printf("<p>nrow:%d\n</p>",nrow);
	//printf("<p>ncolumn:%d\n</p>",nColumn);
	if(result == SQLITE_OK )  
	{  
    	 if(nrow==0)
    	 {
			ret=-2; // it means the ipc_id  didn't exit in the database
		 }
		 else 
		 {
		   /*
			// Print the two-dimensional array
			int i,j;
			int index = 0;
			 for(i=0; i<=nrow; i++)
			 {
				 for(j=0; j<nColumn; j++)
				 {
					printf("%s ", dbResult[index]);
					 index++;
				 }
				 printf("<p>\n</p>");
			 }
		   */
			ret=0; //it means the ipc_id exit in the database
			strcpy(ipaddr,dbResult[nColumn+1]); //take out the ip according to the ipc_id
			//printf("ipaddr:%s\n",ipaddr);
		 }
	}
	 else
	 {  
		printf("error:%d,reasion:%s\n",result,errmsg); 
        ret = -1;
	 }
	sqlite3_free(errmsg);
	return ret;
}


/**************************************************************************************
  Function:     findnamefromid(char *ipc_id,char *name);
  Description: find out the IPC account according to the id 
  Input:          
                  
  Output:   output *name    
  Return:   0:ipc_id exit; -2:ipc_id  didn'texit; -1: operat database failed    
  Others:         
**************************************************************************************/
int findnamefromid(char *ipc_id,char *name)
{
	int result;
    char **dbResult;
    int nrow, nColumn;
	char* errmsg=NULL;
	int ret=0;
	char select_sql[500] = "select * from video_conf where id = ";
	strcat(select_sql,ipc_id);
	//printf("<p>select_sql:%s\n</p>",select_sql);
	result = sqlite3_get_table(db,select_sql, &dbResult, &nrow, &nColumn, &errmsg );
	//printf("<p>nrow:%d\n</p>",nrow);
	//printf("<p>ncolumn:%d\n</p>",nColumn);
	if(result == SQLITE_OK )  
	{  
    	 if(nrow==0)
    	 {
			ret=-2; // it means the ipc_id  didn't exit in the database
		 }
		 else 
		 {
		   /*
			// Print the two-dimensional array
			int i,j;
			int index = 0;
			 for(i=0; i<=nrow; i++)
			 {
				 for(j=0; j<nColumn; j++)
				 {
					printf("%s ", dbResult[index]);
					 index++;
				 }
				 printf("<p>\n</p>");
			 }
		   */
			ret=0; //it means the ipc_id exit in the database
			strcpy(name,dbResult[nColumn+4]); //take out the IPC account according to the ipc_id
			//printf("ipaddr:%s\n",ipaddr);
		 }
	}
	 else
	 {  
		printf("error:%d,reasion:%s\n",result,errmsg); 
        ret = -1;
	 }
	sqlite3_free(errmsg);
	return ret;
}


/**************************************************************************************
  Function:     findaliasfromid(char *ipc_id,char *alias);
  Description: find out the alias according to the id 
  Input:          
                  

  Output:   output *alias   
  Return:   0:ipc_id exit; -2:ipc_id  didn'texit; -1: operat database failed    
  Others:         
**************************************************************************************/
int findaliasfromid(char *ipc_id,char *alias)
{
	int result;
    char **dbResult;
    int nrow, nColumn;
	char* errmsg=NULL;
	int ret=0;
	char select_sql[500] = "select * from video_conf where id = ";
	strcat(select_sql,ipc_id);
	//printf("<p>select_sql:%s\n</p>",select_sql);
	result = sqlite3_get_table(db,select_sql, &dbResult, &nrow, &nColumn, &errmsg );
	//printf("<p>nrow:%d\n</p>",nrow);
	//printf("<p>ncolumn:%d\n</p>",nColumn);
	if(result == SQLITE_OK )  
	{  
    	 if(nrow==0)
    	 {
			ret=-2; // it means the ipc_id  didn't exit in the database
		 }
		 else 
		 {
		   /*
			// Print the two-dimensional array
			int i,j;
			int index = 0;

			 for(i=0; i<=nrow; i++)
			 {
				 for(j=0; j<nColumn; j++)
				 {
					printf("%s ", dbResult[index]);

					 index++;
				 }
				 printf("<p>\n</p>");
			 }
		   */
			ret=0; //it means the ipc_id exit in the database
			strcpy(alias,dbResult[nColumn+6]); //take out the IPC account according to the ipc_id
			//printf("ipaddr:%s\n",ipaddr);
		 }
	}
	 else
	 {  
		printf("error:%d,reasion:%s\n",result,errmsg); 
        ret = -1;
	 }
	sqlite3_free(errmsg);
	return ret;
}


/**************************************************************************************
  Function:     findpwfromid(char *ipc_id,char *password);
  Description: find out the IPC password according to the id 
  Input:          
                  
  Output:   output *password    
  Return:   0:ipc_id exit; -2:ipc_id  didn'texit; -1: operat database failed    
  Others:         
**************************************************************************************/
int findpwfromid(char *ipc_id,char *password)
{
	int result;
    char **dbResult;
    int nrow, nColumn;
	char* errmsg=NULL;
	int ret=0;
	char select_sql[500] = "select * from video_conf where id = ";
	strcat(select_sql,ipc_id);
	//printf("<p>select_sql:%s\n</p>",select_sql);
	result = sqlite3_get_table(db,select_sql, &dbResult, &nrow, &nColumn, &errmsg );
	//printf("<p>nrow:%d\n</p>",nrow);
	//printf("<p>ncolumn:%d\n</p>",nColumn);
	if(result == SQLITE_OK )  
	{  
    	 if(nrow==0)
    	 {
			ret=-2; // it means the ipc_id  didn't exit in the database
		 }
		 else 
		 {
		   /*
			// Print the two-dimensional array
			int i,j;
			int index = 0;
			 for(i=0; i<=nrow; i++)
			 {
				 for(j=0; j<nColumn; j++)
				 {
					printf("%s ", dbResult[index]);
					 index++;
				 }
				 printf("<p>\n</p>");
			 }
		   */
			ret=0; //it means the ipc_id exit in the database
			strcpy(password,dbResult[nColumn+5]); //take out the IPC password according to the ipc_id
			//printf("ipaddr:%s\n",ipaddr);
		 }
	}
	 else
	 {  
		printf("error:%d,reasion:%s\n",result,errmsg); 
        ret = -1;
	 }
	sqlite3_free(errmsg);
	return ret;
}


#if 0
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
    int index_ret;
    char* errmsg=NULL;
    int return_ret = -1;
    char full_ipc = 0;
    char ipc_id[MAX_IPC_NUM] = {0};
    char target;

	//开始查询，传入的 dbResult 已经是 char **，这里又加了一个 & 取地址符，传递进去的就成了 char ***
	//result = sqlite3_get_table(db,"select * from video_conf", &dbResult, &nrow, &nColumn, &errmsg );
    result = sqlite3_get_table(db,"select id from video_conf", &dbResult, &nrow, &nColumn, &errmsg );//减少获取数据的大小
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
                index_ret = nColumn; //dbResult 前面第一行数据是字段名称，从 nColumn 索引开始才是真正的数据
		        for(i=0;i<nrow;i++)
		        {
                    //target = dbResult[index_ret][0];
		            //if(id[j] == target)//记录中的第一项是id
                    if(id[j] == dbResult[index_ret][0])//记录中的第一项是id
		            {
		                ipc_id[j] = 1;//标记ipc已添加
                        //printf("id[%d] = %c\r\n",j,id[j]);
                        //printf("****ret = %d,id = %s****\r\n",ret,findid);
                        //printf("id[%d] = %c****</p>",j,id[j]);
		                
		            }
		            //index_ret += 7;//一条记录总共有7项
                    index_ret += nColumn;
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
                              char *name, char *password, char *aliases, char *indexid, char *roomid,char *ipc_status)
{
    char* errmsg=NULL;
    int result;
    char insert_sql[500] ="insert into video_conf(id,ipaddr,rtspport,httpport,name,password,aliases,indexid,roomid,ipc_status) values(";
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

    //add_marks(indexid,1);
    strcat(insert_sql,",");
    strcat(insert_sql,indexid);

    //add_marks(roomid,1);
    strcat(insert_sql,",");
    strcat(insert_sql,roomid);

    add_marks(ipc_status,1);
    strcat(insert_sql,ipc_status);

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

#if 0
/**************************************************************************************
  Function:       addipc(char *ipaddr, char *rtspport, char *httpport,char *name, char *password, char *aliases)
  Description:    添加ipc
  Input:          
                  
  Output:         
  Return:       0-9:成功返回IPC的ID号，-1:IPC已达最大数, -2 查找ID操作数据库失败,-3插入数据库失败  
  Others:         
***************************************************************************************/
int addipc(char *ipaddr, char *rtspport, char *httpport,char *name, char *password, char *aliases,char *indexid, char *roomid,char *ipc_status)
{

    char findid[20];  
    int ret,findid_ret,insert_ret; 
    
    ret = 0;
    //printf("****find id****\r\n");
    //printf("<p>addipc find id</p>");
 
    memset(findid,0,sizeof(findid));
    findid_ret = find_id(db,findid);
    //printf("****ret = %d,id = %s****\r\n",ret,findid);
    //printf("<p>****ret = %d,id = %s****</p>",ret,findid);
    
    if(findid_ret == 0)
    {
		insert_ret = insert_database(db,findid,ipaddr,rtspport,httpport,name,password,aliases,indexid,roomid,ipc_status);
		//sqlite3_exec(db,"select * from video_conf", LoadMyInfo, NULL, NULL);
        if(insert_ret != 0)
        {
           ret = -3;
        }
        else
        {
           ret = atoi(findid);
        }
    }
    else if (findid_ret == 1)//IPC已达最大数
    {
        ret = -1;
    }
    else if (findid_ret == -1)//查找ID操作数据库失败
    {
        ret = -2;
    }

    return ret;
  
}


/**************************************************************************************
  Function:       find_ip(sqlite3 *db,char *target_ip)
  Description:    根据IP地址查找数据库中是否存在
  Input:          
                  
  Output:         
  Return:         0：不存在 id；1：存在：-1：操作数据库失败   
  Others:         
***************************************************************************************/ 
int find_ip(sqlite3 *db,char *target_ip) 
{  
    int result;
    char **dbResult; //是 char ** 类型，两个*号
    int nrow, nColumn;
    char* errmsg=NULL;
    int return_ret = -1;

    char check_ip_sql[200] = "select * from video_conf where ipaddr =";
    
    add_marks(target_ip,0);
    strcat(check_ip_sql,target_ip);

	//开始查询，传入的 dbResult 已经是 char **，这里又加了一个 & 取地址符，传递进去的就成了 char ***
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
            return_ret = 1;
        }
		    
    }//if(SQLITE_OK == result)//查询成功
    else//查询不成功
    {
        return_ret = -1;
        //printf("error:%d,reasion:%s\n",result,errmsg); 
    }
  
    sqlite3_free(errmsg);
    sqlite3_free_table(dbResult);
  
    return return_ret;
} 

int ipaddr_isuse(char *target_ipaddr)
{
    int ret;
    ret = find_ip(db,target_ipaddr);
    return ret;
}
#endif

