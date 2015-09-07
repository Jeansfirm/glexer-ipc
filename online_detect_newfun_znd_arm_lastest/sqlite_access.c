/***************************************************************************
  Copyright (C), 2009-2014 GuangdongGuanglian Electronic Technology Co.,Ltd.
  FileName:      sqlite_access.c
  Author:        jiang
  Version :      1.0    
  Date:          2014-08-05
  Description:   å®ç°æ•°æ®åº“åˆå§‹åŒ–ï¼Œä»æ•°æ®åº“ä¸­å–å›IPåœ°å€      
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
  Description:    Ìí¼Ó±êµã·ûºÅ
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
  Description:    åˆå§‹åŒ–æ•°æ®åº“
  Input:          
                  
  Output:         
  Return:       0:æˆåŠŸï¼Œ-1æ‰“å¼€æ•°æ®åº“å¤±è´¥ ï¼Œ-2å»ºæ•°æ®åº“è¡¨å¤±è´¥     
  Others:         
***************************************************************************************/
int init_database()
{
	int result;
    //sqlite3 * db;
    int ret = 0;
    //åˆ›å»ºä¸€è¡¨ï¼Œè¡¨åå«video_conf
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
		//æ•°æ®åº“æ‰“å¼€å¤±è´¥
        printf("open sqlite db faile,result = %d\r\n",result);
        //printf("<p>addipc open sqlite db faile</p>");
		ret = -1;
	}
    
    //å»ºè¡¨  
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
  Description:   é‡Šæ”¾æ•°æ®åº“
  Input:          
                  
  Output:         
  Return:            
  Others:         
***************************************************************************************/
int uninit_database()
{
	//å…³é—­æ•°æ®åº“  
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
  Description:    æ ¹æ®IPåœ°å€æŸ¥æ‰¾æ•°æ®åº“ä¸­æ˜¯å¦å­˜åœ¨
  Input:          
                  
  Output:         
  Return:         0ï¼šæŸ¥è¯¢æ•°æ®åº“æ“ä½œæˆåŠŸï¼š-1ï¼šæ“ä½œæ•°æ®åº“å¤±è´¥   
  Others:         
***************************************************************************************/ 
int find_ipc(sqlite3 *db,struct etc_msg *ipc_etc) 
{  
    int result;
    char **dbResult; //æ˜¯ char ** ç±»å‹ï¼Œä¸¤ä¸ª*å·
    int nrow, nColumn,index;
    char* errmsg=NULL;
    int return_ret = -1;
    int i,j,id;
    

    //char target_ipc_id[20];

    //char check_ip_sql[200] = "select * from video_conf where id = '1'";
    //char check_ip_sql[200] = "select * from video_conf where id =";
    //char check_ip_sql[200] = "select * from video_conf order by id";
    char check_ip_sql[200] = "select id,ipaddr from video_conf order by id";//æŒ‰idæ’åºï¼Œå–å‡ºidã€ipaddr

    //sprintf(target_ipc_id, "%d", ipc_num);//æ•°å­—è½¬å­—ç¬¦
    //strcat(check_ip_sql,target_ipc_id);

	result = sqlite3_get_table(db,check_ip_sql, &dbResult, &nrow, &nColumn, &errmsg );
	if(SQLITE_OK == result)//æŸ¥è¯¢æˆåŠŸ
	{
		//ipä¸å­˜åœ¨
        if(nrow == 0)
        {
            return_ret = 0;
        }
        else//ipå­˜åœ¨
        {
            index = nColumn; //dbResult å‰é¢ç¬¬ä¸€è¡Œæ•°æ®æ˜¯å­—æ®µåç§°ï¼Œä» nColumn ç´¢å¼•å¼€å§‹æ‰æ˜¯çœŸæ­£çš„æ•°æ®ï¼Œå­—æ®µåç§°åªæœ‰idã€ipaddr

		    for(j=0;j<nrow;j++)
		    {
                id = atoi(dbResult[index]);
		        for(i=0;i<MAX_IPC_NUM;i++)//æŸ¥æ‰¾å‡ºæ˜¯å“ªä¸ªIPC
		        {
                    if(i == id)
                    {
                        strcpy(ipc_etc->ipc_list[i].ip_addr,dbResult[index+1]);
                        //printf("id:%s,ipaddr:%s\r\n",dbResult[index],dbResult[index+1]);
                        continue;
                    }
		        }
                index += nColumn;//ä¸€æ¡è®°å½•æ€»å…±æœ‰nColumné¡¹
		    }
            return_ret = 0;
        }
    }
    else//æŸ¥è¯¢ä¸æˆåŠŸ
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
  Return:         0ï¼šæŸ¥è¯¢æ•°æ®åº“æˆåŠŸï¼Œ-1ï¼šæ“ä½œæ•°æ®åº“å¤±è´¥   
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
  Description:   µÚÒ»´Î¶ÁÈëÊı¾İ¿âÖĞ10¸öIPCµÄ×´Ì¬£¬aÎªÔÚÏß£¬bÎª²»ÔÚÏß£¬cÎªÃ»Ìí¼ÓÊı¾İÖĞ
  Input:             
                  
  Output:    µÚÒ»´Î¶Áµ½µÄ×´Ì¬´æ´¢µ½init_status_array Êı×éÖĞ      
  Return:    0:¶ÁÈë³É¹¦;-1:¶ÁÈëÊ§°Ü       
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
	char **dbResult; // ÊÇchar **ÀàĞÍ£¬Á½¸ö*ºÅ
	int nrow, nColumn,index;
	char* errmsg=NULL;
	
	char read_status_sql[200] = "select id,ipc_status from video_conf order by id";//°´idÅÅĞò£¬È¡³öid£¬ipc_status
	result = sqlite3_get_table(db,read_status_sql, &dbResult, &nrow, &nColumn, &errmsg );
	if(SQLITE_OK == result)
	{
		index = nColumn;
		for(j=0;j<nrow;j++)
	    {
			id = atoi(dbResult[index]);
			for(i=0;i<MAX_IPC_NUM;i++)  // ÕÒ³öÊÇÄÄ¸öIPC
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
  Description:    ¼ì²éipc_index_listÀï²»Îª'*'µÄ¸öÊıÊÇ·ñÓëÊı¾İÀï´æ´¢µÄĞĞÊıÏàµÈ
  Input:          
                  
  Output:         
  Return:       0:²»Îª'*'µÄ¸öÊıÓëÊı¾İÀï´æ´¢µÄĞĞÊıÏàµÈ£¬-1:»ñÈ¡±íÀïÊı¾İĞĞÊıÊ§°Ü£¬
  				-2:²»Îª'*'µÄ¸öÊıÓëÊı¾İÀï´æ´¢µÄĞĞÊı²»ÏàµÈ
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
	char **dbResult; //ÊÇ char ** ÀàĞÍ£¬Á½¸ö*ºÅ
	
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
	if(SQLITE_OK == result)//²éÑ¯³É¹¦
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
  Description:    ¼ì²é´ı²åÈëÔªËØµÄÖµÊÇ·ñÎª0~9
  Input:          
                  
  Output:         
  Return:       0:³É¹¦£¬-1:´ı²åÈëÔªËØµÄÖµ²»Îª0~9
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
  Description:    ¼ì²é±íÀïÊı¾İÊÇ·ñ´æÔÚidÎªiµÄĞĞ
  Input:          
                  
  Output:         
  Return:       0:³É¹¦£¬-1:²»´æÔÚ -2:»ñÈ¡Êı¾İÊ§°Ü
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
	char **dbResult; //ÊÇ char ** ÀàĞÍ£¬Á½¸ö*ºÅ
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
  Description:    ½«ÅÅÁĞºÅ²åÈëµ½¶ÔÓ¦µÄĞĞºÅÀï
  Input:          
                  
  Output:         
  Return:       0:³É¹¦£¬-1:¸üĞÂindexÖµÊ§°Ü
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
  Function:       int update_status(sqlite3 *db£¬char *newstatus)
  Description:    ²åÈëÊı¾İ¿â¼ÇÂ¼
  Input:          
                  
  Output:         
  Return:       0:³É¹¦£¬-1ºÍ-3:»ñÈ¡Êı¾İ¿âĞÅÏ¢Ê§°Ü ,-2:ĞÂµÄÅÅÁĞË³ĞòºÅµÄ¸öÊıÓëÊı¾İ¿âµÄĞĞÊı²»Ïà·û 
  				-4:´æÔÚÆäËû×Ö·û -5:Êı¾İ¿âÃ»ÓĞ´æÔÚ¶ÔÓ¦µÄidºÅ,-6: ¸üĞÂindexidÊ§°Ü
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
  Description:    ¸ü¸ÄÊı¾İÀïipc_status×Ö¶Î
  Input:          
                  
  Output:         
  Return:      0:³É¹¦£¬-1ºÍ-3:»ñÈ¡Êı¾İ¿âĞÅÏ¢Ê§°Ü ,-2:ĞÂµÄÅÅÁĞË³ĞòºÅµÄ¸öÊıÓëÊı¾İ¿âµÄĞĞÊı²»Ïà·û 
  				-4:´æÔÚÆäËû×Ö·û -5:Êı¾İ¿âÃ»ÓĞ´æÔÚ¶ÔÓ¦µÄidºÅ,-6: ¸üĞÂindexidÊ§°Ü
  Others:         
***************************************************************************************/

int update_ipc_status(char *new_status)
{
	int update_index_ret=0;
	update_index_ret=update_status(db,new_status);	
	return update_index_ret;
}

