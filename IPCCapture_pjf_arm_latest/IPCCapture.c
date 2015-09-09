/**********************-*****************************************************
  Copyright (C), 2009-2014 GuangdongGuanglian Electronic Technology Co.,Ltd.
  FileName:      IPCCapture.c
  Author:        znd
  Version :      1.0    
  Date:          2015-06-03
  Description:   IPC capture a picture     
  History:         
      <author>  <time>   	<version >   	<desc>
	pjf	 2015-7-23            		add a URLDecode function
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
#include <time.h>
#include <sys/stat.h>
#include<sys/wait.h>

#include "cJSON.h"
#include "sqlite_access.h"
#include "online_detect.h"

#include <iconv.h> 


#define MAX_IPC_NUM      50
#define SERV_PORT        5017
#define PIC_MAX		 10  //maximum allowable number in the folder
#define CAP_MAX          10
#define CAP_LOG		 0

char file_num = 0;  //IPC ID


/***************************************************************************
  Function:       parse_data(char* inputstr,char *outputstr,char* value)
  Description:    在字符串inputstr中查找value，输出查找到的字符串outputstr
  Input:          char* inputstr,char *outputstr,char* value
                  
  Output:         
  Return:          
  Others:         
***************************************************************************/
void parse_data(char* inputstr,char *outputstr,char* value)
{
	int i,j;
	char *tmpstr;

	tmpstr=strstr(inputstr,value);
	j = 0;

	for(i = strlen(value); i < (int)strlen(tmpstr); i++)
	{
		if(tmpstr[i] == '&')
		{
			break;
		}                  
		outputstr[j++]=tmpstr[i];
	}
	outputstr[j] = '\0';
}


/***************************************************************************
  Function:       char* getcgidata(FILE* fp, char* requestmethod)
  Description:    获取web服务器的输入
  Input:          
                  
  Output:         返回web服务器输入的字符串
  Return:         
  Others:         
***************************************************************************/
char* getcgidata(FILE* fp, char* requestmethod)
{
    char* input;
    int len;
    int size = 1024;
    int i = 0;
       
    if (!strcmp(requestmethod, "GET"))
    { 
		//GET在cgi中传递的Username="admin"&Password="aaaaa"被放置在环境变量QUERY_STRING中
		input = getenv("QUERY_STRING");
		return input;
    }
    else if (!strcmp(requestmethod, "POST"))
    {
		len = atoi(getenv("CONTENT_LENGTH"));
		input = (char*)malloc(sizeof(char)*(size + 1));
		          
		if (len == 0)
		{
			input[0] = '\0';
			return input;
		}
		          
		while(1)
		{  //POST在cgi中传递的Username="admin"&Password="aaaaa"被写入stdin标准输入流中
	 	    input[i] = (char)fgetc(fp);
		        if (i == size)
		        {
		             input[i+1] = '\0';
		             return input;
		        }
		                 
		        --len;
		        if (feof(fp) || (!(len)))
		        {
		              i++;
		              input[i] = '\0';
		              return input;
		        }
		        i++;
		                 
		}
    }
       return NULL;
}


/***************************************************************************
  Function:       check_ipc_id(char *ipc_id)
  Description:    check whether the ipc_id is in the range of 0-10
  Input:          char* ipc_id
                  
  Output:         
  Return:          
  Others:         
***************************************************************************/
int check_ipc_id(char *ipc_id)
{
	int ipc_id_int;
	int ret=-1;
	ipc_id_int=atoi(ipc_id);
	if(ipc_id_int>=0&&ipc_id_int<10)
	{
		ret=0;
	}
    return ret;
}


/***************************************************************************
  Function:       package_json 
  Description:    封装JSON
  Input:         消息类型
                  
  Output:         
  Return:         封装json后转化为char类型的数据
  Others:         
****************************************************************************/
char *package_json(int type,char *msg,char *pic_name)
{
    cJSON *root,*fmt;
    char *out;	
    //Our "Video" datatype: 
    root=cJSON_CreateObject();	
    cJSON_AddStringToObject(root, "action", "IPC_capture");
    cJSON_AddItemToObject(root, "response_params", fmt=cJSON_CreateObject());
    cJSON_AddNumberToObject(fmt,"status",     type);
    cJSON_AddStringToObject(fmt,"status_msg", msg);
    if(type == 0)
    {
        cJSON_AddStringToObject(fmt,"pic_name",pic_name);
    }

    //out=cJSON_Print(root);
    out=cJSON_PrintUnformatted(root);
    

    cJSON_Delete(root);
    //cJSON_Delete(fmt);
    //printf("%s\n",out);	
    //free(out);
    // Print to text, Delete the cJSON, print it, release the string. 

    return out; 
}


/***************************************************************************
  Function:       package_json_callback 
  Description:    发到5017端口的callback数据格式
  Input:          
                  
  Output:      输出封装好的callback数据的地址   
  Return:       
  Others:         
****************************************************************************/
char *package_json_callback(int type,char *pic_name,int num)
{
    cJSON *root,*fmt;
    char *out;	
    //Our "Video" datatype: 
    root=cJSON_CreateObject();
	cJSON_AddNumberToObject(root,"msgtype",0);
	cJSON_AddNumberToObject(root,"mainid",2);
	cJSON_AddNumberToObject(root,"subid",6);
	cJSON_AddNumberToObject(root,"status",type );
	cJSON_AddNumberToObject(root,"pic_count",num);
	if(type == 0)
	{
		
		cJSON_AddStringToObject(root,"pic_name",pic_name);

	}
    out=cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return out; 
}


#if 0
/***************************************************************************
  Function:       strtrans 

  Description:    replace "%20" with '_' in a string
  Input:          
                  
  Output:        

  Return:       
  Others:         
****************************************************************************/
void strtrans(char *instr,char *outstr)
{
   	char *temp;
	int len=strlen(instr);
	int i=0,j=0;
	while(i<len)
	{	
		if(instr[i]!='%')
		{
			outstr[j]=instr[i];
		}else{
			//outstr[j]=' ';
			outstr[j]='_';
			i=i+2;		
		}
		i++;
		j++;
	}	
	outstr[j]='\0';
	
}


#define NON_NUM '0'

int hex2num(char c)
{
    if (c>='0' && c<='9') return c - '0';
    if (c>='a' && c<='z') return c - 'a' + 10;//这里+10的原因是:比如16进制的a值为10
    if (c>='A' && c<='Z') return c - 'A' + 10;
    
    //printf("unexpected char: %c", c);
    return NON_NUM;
}

/**
 * @brief URLDecode 对字符串URL解码,编码的逆过程
 *
 * @param str 原字符串
 * @param strSize 原字符串大小（不包括最后的\0）
 * @param result 结果字符串缓存区
 * @param resultSize 结果地址的缓冲区大小(包括最后的\0)
 *
 * @return: >0 result 里实际有效的字符串长度
 *            0 解码失败
 */
int URLDecode(const char* str, const int strSize, char* result, const int resultSize)
{
    char ch,ch1,ch2;
    int i;
    int j = 0;//record result index

    if ((str==NULL) || (result==NULL) || (strSize<=0) || (resultSize<=0)) {
        return 0;
    }

    for ( i=0; (i<strSize) && (j<resultSize); ++i) {
        ch = str[i];
        switch (ch) {
            case '+':
                result[j++] = ' ';
                break;
            case '%':
                if (i+2<strSize) {
                    ch1 = hex2num(str[i+1]);//高4位
                    ch2 = hex2num(str[i+2]);//低4位
                    if ((ch1!=NON_NUM) && (ch2!=NON_NUM))
                        result[j++] = (char)((ch1<<4) | ch2);
                    i += 2;




                    break;
                } else {
                    break;
                }
            default:
                result[j++] = ch;
                break;
        }
    }
    
    result[j] = 0;
    return j;
}




/***************************************************************************
  Function:       code_convert
  Description:   Chinese Transcoding
  Input:          
                  
  Output:         
  Return:       
  Others:         
****************************************************************************/

int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen)  
{  
        iconv_t cd;  
        int rc;  
        char **pin = &inbuf;  
        char **pout = &outbuf;  
  
        cd = iconv_open(to_charset,from_charset);  
        if (cd==0)  
                return -1;  
        memset(outbuf,0,outlen);  
        if (iconv(cd,pin,&inlen,pout,&outlen) == -1)  
                return -1;  
        iconv_close(cd);  
        return 0;  
} 

int u2g(char *inbuf,int inlen,char *outbuf,int outlen)	
{  
		return code_convert("utf-8","gb2312",inbuf,inlen,outbuf,outlen);  
}  

#endif



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
    //printf("Content-type:text/html;charset=gb2312\n\n");       ///cgi 输出html标准格式 ,输出中文显示
    printf("Content-type:text/html;charset=utf-8\n\n");       //cgi 输出html标准格式 ,输出中文显示
    printf("<html>"); 
    printf("<head><title>IPCCapture</title></head><body>"); 
    //printf("<p>hello world</p>"); 


   	char *input;
   	char *req_method; 
	char *send_buf;
   	FILE *fp; 
	FILE *fp_count,*fp_old;
	FILE *fp_capture,*fp_rm;
	FILE *fp_upload;
	FILE *fp_mac;


	char ipc_id[20];
	//char flag[20];
	//char ruleid[20];
	char ieee[50];
	//int  findalias_ret;
	char alias[100];
	//char alias_out[100];	
	char capture_cmd[500];
	char upload_cmd[500];
	char file_path[200];
	char pic_name[100];
	char captime[30];
	//char captime_out[30];	
	char buffer_count[1024],buffer_old[2014],buffer_mac[1024];
	char rm_cmd[200];
	char ipaddr[20];	
	char ipc_status[10];
	char name[100];
	char password[100];
	char cap_num[10];


	//time_t timep; 
	//struct tm *p; 
	//time(&timep);
	int err_num = 0;
	int system_ret;
	int pic_num;	
        int init_ret;
	int initdetect_ret;
	int  findip_ret;
	//int  findalias_ret;
	int res; 
	int sockfd_send;
	int send_ret;
	int close_capture;
	int close_count;
	int close_old;
	pid_t status;	
	int findname_ret;
	int findpw_ret;	
	int icap_num;



#if CAP_LOG
	//IPCCapture log file related
	FILE *cap_log_fd;
	cap_log_fd=fopen("/var/www/capturelog","a");
	char str_log[100];
	time_t t_log;
	struct tm *tm_log;
	time(&t_log);
	tm_log=localtime(&t_log);
	sprintf(str_log,"\n\nipccapture.cgi executed time:%d%02d%02d%02d%02d%02d\n",(1900+tm_log->tm_year),( 1+tm_log->tm_mon),tm_log->tm_mday,tm_log->tm_hour,tm_log->tm_min,tm_log->tm_sec);
	fputs(str_log,cap_log_fd);
#endif	

	

	/*socket initialization and connection build*/

	struct sockaddr_in servaddr;	  
	bzero(&servaddr, sizeof(servaddr));    
	servaddr.sin_family = AF_INET;						 
	servaddr.sin_port = htons(SERV_PORT);				 
	//servaddr.sin_addr.s_addr = inet_addr("192.168.1.163");	//for test
	servaddr.sin_addr.s_addr = htons(INADDR_ANY); //INADDR_ANY表示自动获取本机地址
	//while(sockfd_send = socket(AF_INET, SOCK_STREAM, 0)<0);  
	sockfd_send = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd_send <0)
	{			 
		 err_num=-11;
		 goto over;   
	 }	 
	// while(connect(sockfd_send, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in))!=0);	
	res=connect(sockfd_send, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in));
	 if (res !=0)
	 {		   
		 err_num=-12;
		 goto over; 	   
    	 }



	/*initialize database*/

	init_ret = init_database();
	if(init_ret == -1)
        {
        	err_num = -12;
        	//goto over;
        }
    	else if(init_ret == -2)
    	{
        	err_num = -11;
        	//goto over;
    	}



	/*get parameters from cgi access input*/

    	req_method = getenv("REQUEST_METHOD");//访问页面时的请求方法
    	input = getcgidata(stdin, req_method);

	parse_data(input,ipc_id,"ipc_id=");
	if(ipc_id==NULL)
	{
		err_num = -2;
		goto over;
	}
	if(check_ipc_id(ipc_id)<0)
	{
		err_num=-3;
		goto over;
	}
		
	/*
		parse_data(input,flag,"flag=");
		if(ipc_id== NULL)
		{
			err_num = -2;
			goto over;
		}
	
		parse_data(input,ruleid,"ruleid=");
		if(ipc_id== NULL)
		{
			err_num = -2;
			goto over;
		}

	*/

	parse_data(input,ieee,"ieee=");
	if(ieee== NULL)
	{
		err_num = -2;
		goto over;
	}
		
	parse_data(input,captime,"time=");
	if(captime== NULL)
	{
		err_num = -21;
		goto over;
	}
	//strtrans(captime,captime_out); //transform the string containing "%20" to string containing '_'

	parse_data(input,cap_num,"num=");
	if(cap_num == NULL)
	{
		err_num = -22;
		goto over;
	}
	icap_num = atoi(cap_num);
	if(icap_num<=0||icap_num>CAP_MAX)
	{
		err_num = -23;
		goto over;
	}
		
	findip_ret=findipfromid(ipc_id,ipaddr);
	if(findip_ret==-1)
	{
		err_num = -4;
		goto over;
	}
	if(findip_ret==-2)
	{
		err_num = -5;
		goto over;
	}
 
	/*
	findalias_ret=findaliasfromid(ipc_id,alias);
	if(findalias_ret==-1)
	{
		err_num = -4;
		goto over;
	}
	if(findalias_ret==-2)
	{
		err_num = -5;
		goto over;
	}
	*/  

   //	URLDecode(alias,strlen(alias),alias_out,100);


/*
    u2g(alias,strlen(alias),alias_out,1000);
	printf("<p>alias:%s\n</p>",alias_out);
*/



	/*ipc online detect*/
	
	initdetect_ret = init_online_detect();
	if(initdetect_ret!=0)
	{
		err_num=-6;
		goto over;
	}
	ipc_status[0]=get_ipc_status(ipaddr); // return a,b or c  
	ipc_status[1]='\0';	
	//printf("<p>ipc_status:%s\n</p>",ipc_status);



	/*start IPC capturing*/
	
    	if(ipc_status[0]=='a')
    	{
    		//p=localtime(&timep); // obtain local time
/*		sprintf (pic_name,"%d%02d%02d%02d%02d%02d_%s_%s_%s.jpg",(1900+p->tm_year),( 1+p->tm_mon),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec,ieee,ipc_id,alias_out); */ 
/*		sprintf (pic_name,"%d%02d%02d%02d%02d%02d_%s_%s.jpg",(1900+p->tm_year),( 1+p->tm_mon),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec,ieee,ipc_id);*/
    		int i;
	for(i=1;i<=icap_num;i++)	
	{
		sprintf (pic_name,"%s_%s_%s_%d.jpg",captime,ieee,ipc_id,i);
		//printf("<p>ipc_name:%s\n</p>",pic_name);
		sprintf(file_path,"/var/www/IPCCapture/%s",pic_name);
		findname_ret=findnamefromid(ipc_id,name);		
		findpw_ret=findpwfromid(ipc_id,password);
				
		//sprintf(capture_cmd,"/usr/bin/curl -u admin:12345 -o %s http://%s:80/ISAPI/streaming/channels/1/picture",file_path,ipaddr);
		sprintf(capture_cmd,"/usr/bin/curl -u %s:%s -o %s http://%s:80/ISAPI/streaming/channels/1/picture",name,password,file_path,ipaddr);
		//printf("<p>capture_cmd:%s\n</p>",capture_cmd);
		
		fp_capture=popen(capture_cmd,"r");
		if(fp_capture==NULL)
       		{
            		err_num=-13;
        	}

		close_capture=pclose(fp_capture);
		if(close_capture==-1)
		{
			err_num=-15;
			goto over;
		}


#if CAP_LOG		
		// ipccapture log file related
		time(&t_log);
		tm_log=localtime(&t_log);
		sprintf(str_log,"capture successfully, time:%d%02d%02d%02d%02d%02d\n",(1900+tm_log->tm_year),( 1+tm_log->tm_mon),tm_log->tm_mday,tm_log->tm_hour,tm_log->tm_min,tm_log->tm_sec);
		fputs(str_log,cap_log_fd);
#endif
	
	
		if(i==1)
		{
		fp_mac=popen("cat /gl/etc/mac.conf","r"); //获取mac地址
		while(!feof(fp_mac))
		{
			fgets(buffer_mac,sizeof(buffer_mac),fp_mac);
		}
		//printf("<p>mac_addr:%s\n</p>",buffer_mac);
		pclose(fp_mac);
		}

		
		/*start uploading picture to server*/

		sprintf(upload_cmd,"/usr/bin/curl -F filename=%s -F capture=@%s -F gateway=%s -F OK=ok http://121.199.21.14:8888/SmartHome/uploadfile",pic_name,file_path,buffer_mac);
		//printf("<p>upload_cmd:%s\n</p>",upload_cmd);
    		status=system(upload_cmd);
		if(-1==status)
		{
			err_num=-18;
			
			//printf("<p>system error!\n</p>");

#if CAP_LOG
			// ipccapture log file related
			time(&t_log);
			tm_log=localtime(&t_log);
			sprintf(str_log,"pic upload failure, status=-18, time:%d%02d%02d%02d%02d%02d\n",(1900+tm_log->tm_year),( 1+tm_log->tm_mon),tm_log->tm_mday,tm_log->tm_hour,tm_log->tm_min,tm_log->tm_sec);
			fputs(str_log,cap_log_fd);
#endif

			goto over;

		}
		else
		{
			//printf("<p>exit status value =[0x%d]\n</p>",status);

			if(WIFEXITED(status))  //  如果若为正常结束子进程返回的状态，则为真
			{
				if(0==WEXITSTATUS(status))   // 取得子进程 exit()返回的结束代码
				{

					if(i==icap_num)
					{
						
						#if 0
						//printf("<p>run shell script successfully\n</p>");
						send_buf=package_json_callback(err_num,pic_name,icap_num);
						send_ret=send(sockfd_send,send_buf,strlen(send_buf),0);
					
						close(sockfd_send);
						#endif

					}

#if CAP_LOG
					// ipccapture log file related
					time(&t_log);
					tm_log=localtime(&t_log);
					sprintf(str_log,"pic upload successfully, time:%d%02d%02d%02d%02d%02d\n",(1900+tm_log->tm_year),( 1+tm_log->tm_mon),tm_log->tm_mday,tm_log->tm_hour,tm_log->tm_min,tm_log->tm_sec);
					fputs(str_log,cap_log_fd);
#endif


				}
				else{
					//printf("<p>run shell script fail,script exit code:%d\n</p>",WEXITSTATUS(status));
					err_num=-19;

#if CAP_LOG
					// ipccapture log file related
					time(&t_log);
					tm_log=localtime(&t_log);
					sprintf(str_log,"pic upload failure, status=-19, time:%d%02d%02d%02d%02d%02d\n",(1900+tm_log->tm_year),( 1+tm_log->tm_mon),tm_log->tm_mday,tm_log->tm_hour,tm_log->tm_min,tm_log->tm_sec);
					fputs(str_log,cap_log_fd);
#endif

					goto over;
					
				}
			}
			else
			{
				err_num=-20;

#if CAP_LOG
				// ipccapture log file related
				time(&t_log);
				tm_log=localtime(&t_log);
				sprintf(str_log,"pic upload failure, status=-20, time:%d%02d%02d%02d%02d%02d\n",(1900+tm_log->tm_year),( 1+tm_log->tm_mon),tm_log->tm_mday,tm_log->tm_hour,tm_log->tm_min,tm_log->tm_sec);
				fputs(str_log,cap_log_fd);
#endif

				goto over;
				//printf("<p>exit status=[%d]\n</p>",WEXITSTATUS(status));
			}
		}
		sleep(1);
	}
	}
   else
   {
		err_num=-8;  //the IPC with ipc_id is not online

#if CAP_LOG
		// ipccapture log file related
		time(&t_log);
		tm_log=localtime(&t_log);
		sprintf(str_log,"pic upload failure, status=-8, ipc is not online, time:%d%02d%02d%02d%02d%02d\n",(1900+tm_log->tm_year),( 1+tm_log->tm_mon),tm_log->tm_mday,tm_log->tm_hour,tm_log->tm_min,tm_log->tm_sec);
		fputs(str_log,cap_log_fd);
#endif

		goto over;
   }



	/*make sure that the number of picture is less than MAX_PIC*/

	sleep(1);
	fp_count=popen("ls /var/www/IPCCapture | wc -l","r");
	while(!feof(fp_count))
	{
		fgets(buffer_count,sizeof(buffer_count),fp_count);
	}
	//printf("<p>the number of picture:%s\n</p>",buffer_count);
	close_count=pclose(fp_count);	
	if(close_count==-1)
	{
		err_num=-16;
		goto over;
	}
	pic_num=atoi(buffer_count);

	while(pic_num>PIC_MAX) //Make sure that the number of pictures in the folder is no more than PIC_MAX
	{
		fp_old=popen("ls -t /var/www/IPCCapture | tail -n 1","r");
		while(!feof(fp_old))
		{
			fgets(buffer_old,sizeof(buffer_old),fp_old);
		}
		//printf("<p>the oldest picture name:%s\n</p>",buffer_old);
		close_old=pclose(fp_old);
		if(close_old==-1)
		{
			err_num=-17;
			goto over;
		}
		sprintf(rm_cmd,"rm /var/www/IPCCapture/%s -rf",buffer_old);
		//printf("<p>rm_cmd:%s\n</p>",rm_cmd);
		//system_ret=system(rm_cmd);
		fp_rm=popen(rm_cmd,"r");
		if(fp_rm==NULL)
		{
			err_num=-9; // rm the oldest picture failed
			goto over;
		}
		pclose(fp_rm);
		//sleep(3);
		system("sync");
		pic_num--;
		//printf("<p>ipc_num:%d\n</p>",pic_num);
	}
   
   
   
over:
	switch(err_num)
    {
		case 0:printf("<p>%s</p>",package_json(0,"IPC capture success",pic_name));break;
    
        case -1:printf("<p>%s</p>",package_json(-1,"gateway write database failed error",NULL));break;

		case -2:printf("<p>%s</p>",package_json(-2,"input parameter error",NULL));break;

		case -3:printf("<p>%s</p>",package_json(-2,"input parameter error",NULL));break;

		case -4:printf("<p>%s</p>",package_json(-4,"opera database failed",NULL));break;

		case -5:printf("<p>%s</p>",package_json(-5,"ipc_id  didn'texit in the database",NULL));break;

		case -6:printf("<p>%s</p>",package_json(-6,"init icmp protocol failed",NULL));break;

		case -7:printf("<p>%s</p>",package_json(-7,"system() run failed",NULL));break;
		
		case -8:printf("<p>%s</p>",package_json(-7,"the IPC with ipc_id is not online",NULL));break;

		case -9:printf("<p>%s</p>",package_json(-9,"rm the oldest picture failed",NULL));break;

		case -10:printf("<p>%s</p>",package_json(-10,"upload to server failed",NULL));break;

		case -11:printf("<p>%s</p>",package_json(-11,"socket() buit faild",NULL));break;

		case -12:printf("<p>%s</p>",package_json(-12,"connect() buit faild",NULL));break;

		case -13:printf("<p>%s</p>",package_json(-13,"popen capture failed",NULL));break;

		case -14:printf("<p>%s</p>",package_json(-14,"pclose fp_upload failed",NULL));break;

		case -15:printf("<p>%s</p>",package_json(-15,"pclose fp_capture failed",NULL));break;

		case -16:printf("<p>%s</p>",package_json(-16,"pclose fp_count failed",NULL));break;

		case -17:printf("<p>%s</p>",package_json(-17,"pclose fp_old failed",NULL));break;

		case -18:printf("<p>%s</p>",package_json(-18,"system() error return -1",NULL));break;

		case -19:printf("<p>%s</p>",package_json(-19,"upload picture to server fail",NULL));break;

		case -20:printf("<p>%s</p>",package_json(-20,"system() child process exit status",NULL));break;

		case -21:printf("<p>%s</p>",package_json(-21,"input captime parameter error",NULL));break;

		case -22:printf("<p>%s</p>",package_json(-22,"input cap_num parameter error",NULL));break;

		case -23:printf("<p>%s</p>",package_json(-22,"input cap_num is out of range",NULL));break;
		
        default:break;
    }
	

#if CAP_LOG	
	//IPCCapture log file related
	fclose(cap_log_fd); 
#endif

	//for test
	send_buf=package_json_callback(err_num,pic_name,icap_num);
	send_ret=send(sockfd_send,send_buf,strlen(send_buf),0);
	close(sockfd_send);

	uninit_database();

    	printf("</body></html>"); 
	
    	return 0;
}


