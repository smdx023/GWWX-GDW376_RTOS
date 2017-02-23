

#makefile example

#PWD_DIR=$(shell pwd)
#PORT_DIR=$(PWD_DIR)/port
#INC_DIR=$(PWD_DIR)/include
#DLT104_DIR=$(PWD_DIR)/DLT104
#OBJ_DIR=$(PWD_DIR)/obj
#TEST_DIR=$(PWD_DIR)/test


#CC=gcc
#CFLAGS=-Wall -g
#CFLAGS+=-I$(INC_DIR)
#LFLAGS=-lpthread -lm

#export PWD_DIR PORT_DIR INC_DI INC_DIR DLT104_DIR OBJ_DIR TEST_DI CC CFLAGS LFLAGS

#SRCS=test_104.c eth_client.c dlt_104.c dlt_104_init.c dlt_104_port_com.c dlt_104_lib.c

#OBJS=test_104.o eth_client.o dlt_104.o dlt_104_init.o dlt_104_port_com.o dlt_104_lib.o
#EXEC=test

#all:$(OBJS)
#	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) $(LFLAGS)

#all:
#	make -C $(PORT_DIR)
#	make -C $(DLT104_DIR)
#	make -C $(TEST_DIR)
#	make -C $(OBJ_DIR)
#clean:
#	rm -rf $(OBJ_DIR) /*.o
#	rm -rf $(OBJ_DIR) /test


#	gcc -o test DLT104/dlt_104.a test/test.a port/port.a
#DLT104/dlt_104.a test/test.a port/port.a
#.PHONY:clean


all:
	make -C ./DLT104	
	make -C ./port	
	make -C ./test
	make -C ./obj	

clean:
	rm -vf obj/*.o	
	rm -vf obj/*.a	
	rm -vf test_out	

























