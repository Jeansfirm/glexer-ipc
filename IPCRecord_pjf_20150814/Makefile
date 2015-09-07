FFMPEG_DIR=/jeansfirm/ffmpeg_arm
#FFMPEG_DIR=/jeansfirm/smb-share/ffmpeg-lib/x86
INCLUDE_FFMPEG=$(FFMPEG_DIR)/include
LIB_FFMPEG=$(FFMPEG_DIR)/lib

SQLITE3_DIR = /gl/lib/arm
#SQLITE3_DIR= /gl/lib/x86
INCLUDE_SQLITE3 = $(SQLITE3_DIR)/include  
LIB_SQLITE3 = $(SQLITE3_DIR)/lib -l sqlite3 
#LIB_SQLITE3 = $(SQLITE3_DIR)/lib -static -lsqlite3 

CROSS_COMPILE = arm-linux-gcc
#CROSS_COMPILE = gcc

IPCRecord_OBJS = IPCRecord.o cJSON.o sqlite_access.o online_detect.o

target = IPCRecord
all:target
target:$(IPCRecord_OBJS)
	$(CROSS_COMPILE) -o $(target).cgi $(IPCRecord_OBJS) -lm -L$(LIB_SQLITE3) -L$(LIB_FFMPEG)
IPCRecord.o:IPCRecord.c
	$(CROSS_COMPILE) -c IPCRecord.c -I$(INCLUDE_FFMPEG)
sqlite_access.o: sqlite_access.c 
	$(CROSS_COMPILE) -c sqlite_access.c -I$(INCLUDE_SQLITE3) 
#	$(CROSS_COMPILE) -c sqlite_access.c 
cJSON.o: cJSON.c 
	$(CROSS_COMPILE) -c cJSON.c 
online_detect.o:online_detect.c
	$(CROSS_COMPILE) -c -o online_detect.o online_detect.c
clean:
	-rm -rf *.o $(target).cgi
