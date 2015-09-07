#ifndef _SQLITE_ACCESS_H
#define _SQLITE_ACCESS_H

#define MAX_IPC_NUM        10
struct ipc_msg
{
	int id;
    char ip_addr[20];
    char name[20];
    char password[20];
    char rtspport[20];
    char httpport[20];
	//char* aliases;//用指针出现错误
    char aliases[150];
	
	char indexid[11];
	char roomid[11];
	char ipc_status[11];

	char DomainName[100];
	char SerialNumber[100];
};
struct ipc_list
{
	int total_num;
	struct ipc_msg l_ipc_msg[MAX_IPC_NUM];
};

int init_database();
int uninit_database();
//int addipc(char *ipaddr, char *rtspport, char *httpport,char *name, char *password, char *aliases);
//int ipaddr_isuse(char *target_ipaddr);
int getipclist(struct ipc_list *ipclist_buf);
#endif
