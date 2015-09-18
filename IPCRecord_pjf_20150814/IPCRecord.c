/**********************-*****************************************************
  Copyright (C), 2009-2015 GuangdongGuanglian Electronic Technology Co.,Ltd.
  FileName:      IPCRecord.c
  Author:        pjf
  Version :      1.0    
  Date:          2015-08-14
  Description:   IPC record a video     
  History:         
      <author>  <time>   	<version >   	<desc>
	
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
//#include "online_detect.h"
#include "online_detect.h"

#include <iconv.h> 


#define SERV_PORT	5017
#define VIDEO_MAX	40
#define DURATION_MAX    120      
#define STREAM_NUM      2      // num 1 means mainstream, num 2 means substream   


/***************************************************************************
  Function:       void get_servaddr(char *servaddr)
  Description:    从/gl/etc/gatewaysetting.json中读取服务器ip地址
  Input:          char* servaddr
                  
  Output:         
  Return:          
  Others:         
***************************************************************************/

void get_servaddr(char *servaddr)
{
	cJSON *root,*json_temp;
	FILE *fd;
	int len;
	char *file_str;

	fd=fopen("/gl/etc/gateway_setting.json","r");
	fseek(fd,0,SEEK_END);
	len=ftell(fd);
	fseek(fd,0,SEEK_SET);
	file_str=(char *)malloc(sizeof(char)*len);
	fread(file_str,sizeof(char),len,fd);
	root=cJSON_Parse(file_str);
	json_temp=cJSON_GetObjectItem(root,"xmpphost");
	strcpy(servaddr,json_temp->valuestring);

	cJSON_Delete(json_temp);
	free(file_str);
	
}

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
char *package_json(int type,char *msg,char *video_name)
{
    cJSON *root,*fmt;
    char *out;	
    //Our "Video" datatype: 
    root=cJSON_CreateObject();	
    cJSON_AddStringToObject(root, "action", "IPC_record");
    cJSON_AddItemToObject(root, "response_params", fmt=cJSON_CreateObject());
    cJSON_AddNumberToObject(fmt,"status",     type);
    cJSON_AddStringToObject(fmt,"status_msg", msg);
    if(type == 0)
    {
        cJSON_AddStringToObject(fmt,"video_name",video_name);
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
char *package_json_callback(int type,char *video_name,int duration)
{
    cJSON *root,*fmt;
    char *out;	
    //Our "Video" datatype: 
    root=cJSON_CreateObject();
	cJSON_AddNumberToObject(root,"msgtype",0);
	cJSON_AddNumberToObject(root,"mainid",2);
	cJSON_AddNumberToObject(root,"subid",7);
	cJSON_AddNumberToObject(root,"status",type );
	cJSON_AddNumberToObject(root,"video_duration",duration);
	if(type == 0)
	{
		
		cJSON_AddStringToObject(root,"video_name",video_name);

	}
    out=cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return out; 
}



int main()
{
	printf("Cotent-type:text/html;charset=utf-8\n\n");
	printf("<html>");
	printf("<head><title>IPCRecord</title></head><body>");

	

	char *input;
	char *req_method;
	FILE *fp;
	FILE *fp_count,*fp_old;
	FILE *fp_record,*fp_rm;
	FILE *fp_upload;
	FILE *fp_mac;

	char ipc_id[10];
	char ipaddr[20];
	//char name[100];
	//char password[100];
	char record_time[30];
	char record_cmd[500];
	char upload_cmd[500];
	char rm_cmd[500];
	char file_path[200];
	char video_name[100];
	char ieee[100];
	char ipc_status[10];
	char buffer_mac[1024];
	char buffer_count[1024],buffer_old[2014];
	char *send_buf;
	char serv_ip[50];

	int findip_ret;
	//int findname_ret;
	//int findpw_ret;
	int init_ret;
	int initdetect_ret;
	int err_num = 0;
	int close_record;
	int sockfd_send;
	int res;
	int send_ret;
	int close_count;
	int video_num;
	int stream_num;
	
	FILE *fp_chmod;
	int close_chmod;
	char chmod_cmd[200];

	time_t tt;
	int t_before;
	int t_after;
	int t_middle;
	

	char duration[10];
	int iduration;
	//pid_t status_record;
	pid_t status_upload;

	FILE *uploadfailure_fd;
	char str_upload[150];

	uploadfailure_fd=fopen("/var/www/uploadfailure_log","a+");

	get_servaddr(serv_ip);
	

	/*initialize the socket and build the connection*/

	struct sockaddr_in servaddr;
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	sockfd_send = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd_send < 0)
	{
		err_num=-1;
		goto over;
	}

	res=connect(sockfd_send,(struct sockaddr*)&servaddr,sizeof(struct sockaddr_in));
	 if (res !=0)
	 {		   
		 err_num=-2;
		 goto over; 	   
         }

	
	
	
	/*initialize database*/

	init_ret = init_database();
	if(init_ret == -1)
	{
		err_num = -3;
		goto over;
	}
	else if(init_ret == -2)
	{
		err_num = -4;
		goto over;
	}


	/*get ipc_id, record_time, ieee address,duration form input*/
	
	req_method = getenv("REQUEST_METHOD");
	input = getcgidata(stdin,req_method);
	parse_data(input,ipc_id,"ipc_id=");
		if(ipc_id==NULL)
		{
			err_num = -5;
			goto over;
		}
	if(check_ipc_id(ipc_id)<0)
	{
		err_num=-6;
		goto over;
	}
	
	parse_data(input,record_time,"time=");
	if(record_time== NULL)
	{
		err_num = -7;
		goto over;
	}
	
	parse_data(input,duration,"duration=");
	if(duration == NULL)
	{
		err_num = -8;
		goto over;
	}
	iduration = atoi(duration); 
	if(iduration<=0||iduration>DURATION_MAX)
	{
		err_num = -9;
		goto over;
	}
	parse_data(input,ieee,"ieee=");
	if(ieee == NULL)
	{
		err_num = -19;
		goto over;
	}

	
	//find ip form database by ipc_id
	findip_ret=findipfromid(ipc_id,ipaddr);
	if(findip_ret==-1)
	{
		err_num = -10;
		goto over;
	}
	
	//get ipc online status	
	initdetect_ret = init_online_detect();
	if(initdetect_ret!=0)
	{
		err_num=-11;
		goto over;
	}
	ipc_status[0]=get_ipc_status(ipaddr); // return a,b or c  
	ipc_status[1]='\0';


	/*start video recording*/

	if(ipc_status[0]=='a')
	{
		stream_num = STREAM_NUM;

		//sprintf(video_name,"%s_%s_%d.avi",record_time,ipc_id,iduration);
		sprintf(video_name,"%s_%s_%s_%d.mkv",record_time,ieee,ipc_id,iduration);
		//sprintf(video_name,"%s_%s_%s_%d.mp4",record_time,ieee,ipc_id,iduration);
		sprintf(file_path,"/var/www/IPCRecord/%s",video_name);	
	
		//sprintf(record_cmd,"/usr/bin/ffmpeg -i rtsp://%s:554/Streaming/Channels/1 -t %d -vcodec copy %s",ipaddr,iduration,file_path);
		sprintf(record_cmd,"/usr/bin/ffmpeg -i rtsp://%s:554/Streaming/Channels/%d -t %d -vcodec copy %s",ipaddr,stream_num,iduration,file_path);
		//printf("<p>%s\n</p>",video_name);
		//printf("<p>%s\n</p>",file_path);
		//printf("<p>%s\n</p>",record_cmd);

		
#if 0
		FILE *sh;
		sh=fopen("record.sh","w");
		fputs("#!/bin/sh\n\n",sh);
		fputs(record_cmd,sh);
		fclose(sh);

		fp_record=popen("/bin/sh record.sh","r");
#endif
		time(&tt);
		t_before=(int)tt;

		fp_record=popen(record_cmd,"r");			
		if(fp_record==NULL)
		{
			err_num=-12;
			goto over;
		}			

		close_record=pclose(fp_record);
		if(close_record==-1)
		{
			err_num=-13;
			goto over;
		}
		
		time(&tt);
		t_middle=(int)tt;
		printf("<p>recording time cost:%d s \n</p>",t_middle-t_before);
			

		/*change the permission of video file*/

		sprintf(chmod_cmd,"chmod 644 %s",file_path);
		fp_chmod=popen(chmod_cmd,"r");
		if(fp_chmod==NULL)
		{
			err_num=-14;
			goto over;
		}

		close_record=pclose(fp_record);
		if(close_record==-1)
		{
			err_num=-15;
			goto over;
		}


		/*upload video file to the server*/

		fp_mac=popen("cat /gl/etc/mac.conf","r");
		while(!feof(fp_mac))
		{
			fgets(buffer_mac,sizeof(buffer_mac),fp_mac);
		}
		pclose(fp_mac);

		sprintf(upload_cmd,"/usr/bin/curl -F filename=%s -F capture=@%s -F gateway=%s -F OK=ok http://%s:8888/SmartHome/uploadfile",video_name,file_path,buffer_mac,serv_ip);
		//sprintf(upload_cmd,"/usr/bin/curl -F filename=%s -F capture=@%s -F gateway=%s -F OK=ok http://121.199.21.14:88818/SmartHome/uploadfile",video_name,file_path,buffer_mac);  //a wrong url, just for uploadfailure test


#if 1		
		status_upload=system(upload_cmd);		
		if(-1==status_upload)
		{
			err_num=-16;
			
		} else
		{
			//printf("<p>exit status value =[0x%d]\n</p>",status);

			if(WIFEXITED(status_upload))  //  如果若为正常结束子进程返回的状态，则为真
			{
				if(0==WEXITSTATUS(status_upload))   // 取得子进程 exit()返回的结束代码
				{
					/*SEND THE CALLBACK DATA*/

					#if 0					
					send_buf=package_json_callback(err_num,video_name,iduration);
					send_ret=send(sockfd_send,send_buf,strlen(send_buf),0);
					close(sockfd_send);
					#endif

					time(&tt);
					t_after=(int)tt;
					printf("<p>total time cost:%d s \n</p>",t_after-t_before);
					printf("<p>uploading time cost:%d s \n</p>",t_after-t_before-iduration);
				}
				else
				{
					//printf("<p>run shell script fail,script exit code:%d\n</p>",WEXITSTATUS(status));
					err_num=-17;
										
				}
			}			
			else
			{
				err_num=-18;
				
				//printf("<p>exit status=[%d]\n</p>",WEXITSTATUS(status));
			}
		}
#endif
	}else
	{
		err_num=-20;
		goto over;
	}

	
	if(-16==err_num||-17==err_num||-18==err_num)
	{
		sprintf(str_upload,"%s\n",file_path);
		fputs(str_upload,uploadfailure_fd);
	}




	/*make sure that video files are less than VIDEO_MAX*/

	sleep(1);
	fp_count=popen("ls /var/www/IPCRecord | wc -l ","r"); //get the number of files
	while(!feof(fp_count))
	{
		fgets(buffer_count,sizeof(buffer_count),fp_count);
	}
	close_count=pclose(fp_count);
	if(close_count==-1)
	{
		err_num=-19;
		goto over;
	}
	video_num = atoi(buffer_count);
	while(video_num>VIDEO_MAX)
	{
		fp_old=popen("ls -t /var/www/IPCRecord | tail -n 1","r");//get the last file name ordered by creating time
		while(!feof(fp_old))
		{
			fgets(buffer_old,sizeof(buffer_old),fp_old);
		}
		pclose(fp_old);
		sprintf(rm_cmd,"rm -rf /var/www/IPCRecord/%s",buffer_old);
		fp_rm=popen(rm_cmd,"r");
		pclose(fp_rm);
		system("sync");
		video_num--;
	}




over:
	switch(err_num)
	{
		case 0:printf("<p>%s</p>",package_json(0,"IPC record success",video_name));break;
		case -1:printf("<p>%s</p>",package_json(-1,"socket() built failed error",NULL));break;
		case -2:printf("<p>%s</p>",package_json(-2,"connect() bulit failed error",NULL));break;
		case -3:printf("<p>%s</p>",package_json(-3,"initialize database failed error",NULL));break;
		case -4:printf("<p>%s</p>",package_json(-4,"initialize database failed error",NULL));break;
		case -5:printf("<p>%s</p>",package_json(-5,"input parameter ipc_id error",NULL));break;
		case -6:printf("<p>%s</p>",package_json(-6,"ipc_id is out of range,must be between 0 and 10",NULL));break;
		case -7:printf("<p>%s</p>",package_json(-7,"input parameter record_time error",NULL));break;
		case -8:printf("<p>%s</p>",package_json(-8,"input parameter duration error",NULL));break;
		case -9:printf("<p>%s</p>",package_json(-9,"parameter duration is out of range",NULL));break;
		case -10:printf("<p>%s</p>",package_json(-10,"can't find ip from ipc_id",NULL));break;
		case -11:printf("<p>%s</p>",package_json(-11,"initialize online_detect error",NULL));break;
		case -12:printf("<p>%s</p>",package_json(-12,"popen record_cmd failed",NULL));break;
		case -13:printf("<p>%s</p>",package_json(-13,"pclose record_cmd failed",NULL));break;
		case -14:printf("<p>%s</p>",package_json(-14,"popen chmod_cmd failed",NULL));break;
		case -15:printf("<p>%s</p>",package_json(-15,"pclose chmod_cmd failed",NULL));break;
		case -16:printf("<p>%s</p>",package_json(-16,"system() error return -1",NULL));break;
		case -17:printf("<p>%s</p>",package_json(-17,"upload picture to server failed",NULL));break;
		case -18:printf("<p>%s</p>",package_json(-18,"system() child process exit abnormally",NULL));break;
		case -19:printf("<p>%s</p>",package_json(-19,"input parameter ieee error",NULL));break;
		case -20:printf("<p>%s</p>",package_json(-20,"ipc is not online error",NULL));break;
		
		default:break;
	}



	if(0==err_num||-16==err_num||-17==err_num||-18==err_num)
	{
		//send_buf=package_json_callback(err_num,video_name,iduration);
		send_buf=package_json_callback(0,video_name,iduration);
		send_ret=send(sockfd_send,send_buf,strlen(send_buf),0);
		close(sockfd_send);
	}

	
	fclose(uploadfailure_fd);	
	uninit_database();
	//uninit_online_detect();	

	printf("</body></html>"); 
	
        return 0;
}





















