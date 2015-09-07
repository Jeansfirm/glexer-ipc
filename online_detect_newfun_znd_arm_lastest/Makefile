SQLITE3_DIR = /gl/lib/arm
#SQLITE3_DIR = /gl/lib/x86
INCLUDE_SQLITE3 = $(SQLITE3_DIR)/include  
LIB_SQLITE3 = $(SQLITE3_DIR)/lib -l sqlite3 

COMPILE_OPTS           = -I. -O2 -DSOCKLEN_T=socklen_t -DNO_SSTREAM=1 -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -I$(INCLUDE_SQLITE3)
C                      = c
OBJ                    = o	
C_COMPILER             = arm-arago-linux-gnueabi-gcc
#C_COMPILER             = gcc

C_FLAGS                = $(COMPILE_OPTS)

CPLUSPLUS_COMPILER     = arm-arago-linux-gnueabi-g++
#CPLUSPLUS_COMPILER     = gcc


CPLUSPLUS_FLAGS        = $(COMPILE_OPTS) -Wall -DBSD=1

LINK                   = arm-arago-linux-gnueabi-g++ -o
#LINK                   = gcc -o

CONSOLE_LINK_OPTS      =	

objects                = main.o online_detect.o cJSON.o json.o sqlite_access.o
#objects                = main.o online_detect.o cJSON.o 
target                 = ipc_online

#all:target

.$(C).$(OBJ):
	$(C_COMPILER) -c $(C_FLAGS) $<
#.$(CPP).$(OBJ):
#	$(CPLUSPLUS_COMPILER) -c $(CPLUSPLUS_FLAGS) $<

ipc_online:	$(objects)
	$(LINK) $@ $(CONSOLE_LINK_OPTS) $(objects) -lpthread -lm -L$(LIB_SQLITE3)

clean:
	-rm -rf *.o ipc_online
