#ifndef _SQLITE_ACCESS_H
#define _SQLITE_ACCESS_H
int init_database();
int uninit_database();
//int editipc(char *ipaddr, char *rtspport, char *httpport,char *name, char *password, char *aliases,char *indexid, char *roomid,char *ipc_status,char *ipc_id);
int update_database_index(char *ipc_index_list);

//int ipaddr_isuse(char *target_ipaddr);

#endif
