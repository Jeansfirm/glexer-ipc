SQLITE3_DIR = /gl/lib/arm
#SQLITE3_DIR = /gl/lib/x86
INCLUDE_SQLITE3 = $(SQLITE3_DIR)/include  
LIB_SQLITE3 = $(SQLITE3_DIR)/lib -l sqlite3 
#LIB_SQLITE3 = $(SQLITE3_DIR)/lib -static -lsqlite3 

CROSS_COMPILE =	arm-arago-linux-gnueabi-gcc
#CROSS_COMPILE =	gcc
addIPC_OBJS   = modifyIPCindex.o cJSON.o sqlite_access.o
#addIPC_OBJS   = addIPC.o cJSON.o 
target        = modifyIPCindex
all:target
target: $(addIPC_OBJS)
	$(CROSS_COMPILE) -o $(target).cgi $(addIPC_OBJS) -lm -L$(LIB_SQLITE3)   
#	$(CROSS_COMPILE) -o $(target).cgi $(addIPC_OBJS) -lm 
#	$(CROSS_COMPILE) -o $(target).cgi $(addIPC_OBJS) -lm cJSON中用到数学函数库
modifyIPCindex.o: modifyIPCindex.c 
	$(CROSS_COMPILE) -c modifyIPCindex.c
sqlite_access.o: sqlite_access.c 
	$(CROSS_COMPILE) -c sqlite_access.c -I$(INCLUDE_SQLITE3) 
#	$(CROSS_COMPILE) -c sqlite_access.c 
cJSON.o: cJSON.c 
	$(CROSS_COMPILE) -c cJSON.c 
clean:
	-rm -rf *.o $(target).cgi
