# Makefile for string protocol, server, and client
CC=/usr/bin/clang
CFLAGS=-Wall -g3 -Werror -Wall -Wpedantic -std=c17
LDFLAGS=-lpthread
CFDEBUG=-DDEBUG

default: build

rebuild: clean default

build: build_rel build_dbg

build_rel: server-rel

build_dbg: server-dbg

server-dbg: hashtable
	$(CC) $(CFLAGS) $(CFDEBUG) -o ./dbg/server redis.c hastable.o $(LDFLAGS)
server-rel: hashtable
	$(CC) $(CFLAGS) -o ./rel/server redis.c hastable.o $(LDFLAGS)
hashtable: hastable.c
	$(CC) $(CFLAGS) -c hastable.c
clean:
	@/bin/rm -rf server *.o *core *~ -f ./rel -f ./dbg