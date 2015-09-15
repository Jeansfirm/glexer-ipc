CROSS_COMPILE = arm-arago-linux-gnueabi-gcc
#CROSS_COMPILE = gcc
target = ipc_reupload
all:target
target:
	$(CROSS_COMPILE) -o $(target) detect_server.c ipc_reupload.c

clean: 
	-rm -rf *.o ipc_reupload
