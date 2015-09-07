SQLITE3_DIR = /gl/lib/arm
#SQLITE3_DIR = /gl/lib/x86
INCLUDE_SQLITE3 = $(SQLITE3_DIR)/include  
LIB_SQLITE3 = $(SQLITE3_DIR)/lib -l sqlite3 

CROSS_COMPILE =	arm-arago-linux-gnueabi-gcc
#CROSS_COMPILE = gcc

getIPClist_OBJS   = getIPClist.o cJSON.o sqlite_access.o

target        = getIPClist
all:target
target: $(getIPClist_OBJS)
	$(CROSS_COMPILE) -o $(target).cgi $(getIPClist_OBJS) -lm -L$(LIB_SQLITE3)  
#	$(CROSS_COMPILE) -o $(target).cgi $(getIPClist_OBJS) -lm cJSON中用到数学函数库
getIPClist.o: getIPClist.c 
	$(CROSS_COMPILE) -c getIPClist.c
sqlite_access.o: sqlite_access.c 
	$(CROSS_COMPILE) -c sqlite_access.c -I$(INCLUDE_SQLITE3)
cJSON.o: cJSON.c 
	$(CROSS_COMPILE) -c cJSON.c 
clean:
	-rm -rf *.o $(target).cgi
