SQLITE3_DIR = /gl/lib/arm
#SQLITE3_DIR= /gl/lib/x86
INCLUDE_SQLITE3 = $(SQLITE3_DIR)/include  
LIB_SQLITE3 = $(SQLITE3_DIR)/lib -l sqlite3 
#LIB_SQLITE3 = $(SQLITE3_DIR)/lib -static -lsqlite3 

CROSS_COMPILE =	arm-arago-linux-gnueabi-gcc
#CROSS_COMPILE =	gcc
IPCCapture_OBJS   = IPCCapture.o cJSON.o sqlite_access.o online_detect.o
#addIPC_OBJS   = addIPC.o cJSON.o 
target        = IPCCapture
all:target
target: $(IPCCapture_OBJS)
	$(CROSS_COMPILE) -o $(target).cgi $(IPCCapture_OBJS) -lm -L$(LIB_SQLITE3)   
#	$(CROSS_COMPILE) -o $(target).cgi $(addIPC_OBJS) -lm 
#	$(CROSS_COMPILE) -o $(target).cgi $(addIPC_OBJS) -lm cJSON中用到数学函数库
IPCCapture.o: IPCCapture.c 
	$(CROSS_COMPILE) -c IPCCapture.c
sqlite_access.o: sqlite_access.c 
	$(CROSS_COMPILE) -c sqlite_access.c -I$(INCLUDE_SQLITE3) 
#	$(CROSS_COMPILE) -c sqlite_access.c 
cJSON.o: cJSON.c 
	$(CROSS_COMPILE) -c cJSON.c 
#new_online_detect.o:online_detect.c
online_detect.o:online_detect.c
	$(CROSS_COMPILE) -c -o online_detect.o online_detect.c
clean:
	-rm -rf *.o $(target).cgi
