#ifndef _SQLITE_ACCESS_H
#define _SQLITE_ACCESS_H
int init_database();
int uninit_database();
int addipc(char *ipaddr, char *rtspport, char *httpport,char *name, char *password, char *aliases,char *indexid,char *roomid,char *ipc_status);
int ipaddr_isuse(char *target_ipaddr);

int findipfromid(char *ipc_id,char *ipaddr);
int findnamefromid(char *ipc_id,char *name);
int findpwfromid(char *ipc_id,char *name);
int findaliasfromid(char *ipc_id,char *alias);


#endif

