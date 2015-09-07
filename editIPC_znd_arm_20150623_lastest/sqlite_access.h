#ifndef _SQLITE_ACCESS_H
#define _SQLITE_ACCESS_H
int init_database();
int uninit_database();
int editipc(char *ipaddr, char *rtspport, char *httpport,char *name, char *password, char *aliases,char *indexid, char *roomid,char *ipc_status,char *ipc_id,char *DomainName,char *SerialNumber);

//int ipaddr_isuse(char *target_ipaddr);

#endif
