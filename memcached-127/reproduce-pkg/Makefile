# To make the trigger

LIBMEMCACHED_HOME=/opt/bug/memcached-bug1/libmemcached-0.49

CC = gcc
CFLAGS = -g -Wall -Werror
INCS = -I$(LIBMEMCACHED_HOME)/include
LDFLAGS = -L$(LIBMEMCACHED_HOME)/lib -Wl,-rpath,$(LIBMEMCACHED_HOME)/lib

all: trigger

trigger: trigger.c
	$(CC) $(CFLAGS) $(INCS) $(LDFLAGS) -o $@ $< -lpthread -lmemcached

clean:
	rm -f trigger
