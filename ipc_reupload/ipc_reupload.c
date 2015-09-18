#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<math.h>

#include"cJSON.h"

FILE *log_fd_in,*log_fd_out,*fd_mac;
char file_name[150];
char file_path[150];
char cmd_upload[200];
char buff_mac[50];
int status_upload;

char a;


char ser_ip[50];


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


/**
 * @description:
 * @param ipaddr
 * @return
 */
int check_server_network(char *ipaddr)
{
	init_detect();
	if(detect_server(ipaddr)==3)
	{
		uninit_detect();
		return 1;
	}
	else
	{
		uninit_detect();
		return 0;
	}
}

/**
 * remove '\n' from a string
 * @param str
 */

void remove_return(char *str)
{

	int len,i,j;
	j=0;
	len=strlen(str);
	//str[len-1]='\0';

	char temp[150];
	for(i=0;i<len;i++)
	{
		if(str[i]=='\n')
		{
			continue;
		}
		temp[j]=str[i];
		j++;
	}
	temp[j]='\0';
	strcpy(str,temp);

}


/**
 *
 * @return
 */
void getnamefrompath(char *name,char *path)
{

	int i,j,k;
	int len;
	len=strlen(path);
	k=0;
	for(i=0;i<len;i++)
	{
		if(path[i]=='/')k++;
	}
	i=0;
	j=0;
	while(path[j]!='\0')
	{
		if(path[j]=='/')
		{
			k--;
			if(k==0)j++;
		}
		if(k==0)
		{
			name[i]=path[j];
			i++;
		}

		j++;
	}

	name[i]=='\0';

}


/**
 * @description: delete first line of the file stream, and its current file position must be at the beginning of second line
 * @param fd
 */
void del_firstline()
{

	while(1)
	{
		a=fgetc(log_fd_in);
		//printf("%c\t",c);
		if(EOF==a)break;
		fputc(a,log_fd_out);
	}
	fclose(log_fd_in);
	fclose(log_fd_out);
	remove("/var/www/uploadfailure_log");
	rename("/var/www/uploadfailure_log_temp","/var/www/uploadfailure_log");
}


/**
 * check if the file exists or not
 * @return
 */
int file_exist(char *file_path)
{
	FILE *fd;
	char temp_name[150];

	fd=popen("ls /var/www/IPCCapture","r");
	while(!feof(fd))
	{
		fgets(temp_name,sizeof(temp_name),fd);
		remove_return(temp_name);
		if(!strcmp(file_name,temp_name))
		{
			return 1;
		}
	}
	fclose(fd);

	fd=popen("ls /var/www/IPCRecord","r");
	while(!feof(fd))
	{
			fgets(temp_name,sizeof(temp_name),fd);
			remove_return(temp_name);
			if(!strcmp(file_name,temp_name))
			{
				return 1;
			}
	}
	fclose(fd);

	return 0;
}




int main()
{
	
	get_servaddr(ser_ip);
	fd_mac=popen("cat /gl/etc/mac.conf","r");
	while(!feof(fd_mac))
	{
		fgets(buff_mac,sizeof(buff_mac),fd_mac);
	}
	pclose(fd_mac);
	//printf("mac address:%s\n",buff_mac);
	
	//setuid(getuid());

	while(1)
	{

		log_fd_in=fopen("/var/www/uploadfailure_log","a+");
		log_fd_out=fopen("/var/www/uploadfailure_log_temp","w");
		file_path[0]='\0';
		file_name[0]='\0';
		fgets(file_path,sizeof(file_path),log_fd_in);
		remove_return(file_path);
		getnamefrompath(file_name,file_path);
		//printf("file address:%s\n",file_path);
		//printf("file name:%s\n",file_name);

		if(strlen(file_path)==0)
		{
			fclose(log_fd_in);
			fclose(log_fd_out);
			remove("/var/www/uploadfailure_log_temp");
			break;
		}

		if(check_server_network(ser_ip))
		{


			if(!file_exist(file_path))
			{

				printf("%s doesn't exist!\n",file_path);

				//del_firstline();
				//remove("/var/www/uploadfailure_log");
				//rename("/var/www/uploadfailure_log_temp","/var/www/uploadfailure_log");

				while(1)
				{
					a=fgetc(log_fd_in);
																
					if(EOF==a||a<0||a>127)
					{
						break;
					}	
					fputc(a,log_fd_out);
				}
				fclose(log_fd_in);
				fclose(log_fd_out);
				remove("/var/www/uploadfailure_log");
				rename("/var/www/uploadfailure_log_temp","/var/www/uploadfailure_log");

				continue;
			}


			printf("%s does exist!\n",file_path);

			sprintf(cmd_upload,"/usr/bin/curl -F filename=%s -F capture=@%s -F gateway=%s -F OK=ok http://%s:8888/SmartHome/uploadfile",file_name,file_path,buff_mac,ser_ip);
			//sprintf(cmd_upload,"/usr/bin/curl -F filename=1439456470_10197A000001222D_1_1.jpg -F capture=@/var/www/IPCCapture/1439456470_10197A000001222D_1_1.jpg -F gateway=%s -F OK=ok http://121.199.21.14:8888/SmartHome/uploadfile",buff_mac);
			status_upload=system(cmd_upload);
			if(-1==status_upload)
			{
				goto over;
			}else
			{
				if(WIFEXITED(status_upload))
				{
					if(0==WEXITSTATUS(status_upload))
					{

						printf("%s upload successfully!!\n",file_name);

						//del_firstline();
						//remove("/var/www/uploadfailure_log");
						//rename("/var/www/uploadfailure_log_temp","/var/www/uploadfailure_log");
						
						while(1)
							{
								a=fgetc(log_fd_in);
																
								if(EOF==a||a<0||a>127)
								{
									break;
								}	
								fputc(a,log_fd_out);
							}					

						
						fclose(log_fd_in);
						fclose(log_fd_out);
						remove("/var/www/uploadfailure_log");
						rename("/var/www/uploadfailure_log_temp","/var/www/uploadfailure_log");
						
						continue;

					}else
					{
						goto over;
					}
				}else
				{
					goto over;
				}
			}
			

	
			over:
				//printf("upload unsuccessfully!!\n");
				fclose(log_fd_in);
				fclose(log_fd_out);
				remove("/var/www/uploadfailure_log_temp");
				break;


		}else
		{
			fclose(log_fd_in);
			fclose(log_fd_out);
			remove("/var/www/uploadfailure_log_temp");
			break;
		}


	}


	return 0;

}
