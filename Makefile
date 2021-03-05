# Makefile for string protocol, server, and client
CC=/usr/bin/clang
CFLAGS=-Wall  -Werror -Wall -Wpedantic -std=c17
LDFLAGS=-lpthread
CFLAGS_DEBUG= -DDEBUG -g3
CFLAGS_REL= -O2

default: build

rebuild: clean default

build: build_rel build_dbg

build_rel: server-rel

build_dbg: 	server-dbg

server-dbg: mkdir_dbg hashtable
	$(CC) $(CFLAGS) $(CFLAGS_DEBUG) -o ./dbg/server redis.c hastable.o $(LDFLAGS)

server-rel: mkdir_rel hashtable
	$(CC) $(CFLAGS) $(CFLAGS_REL) -o ./rel/server redis.c hastable.o $(LDFLAGS)

hashtable: hastable.c
	$(CC) $(CFLAGS) -c hastable.c
clean:
	@/bin/rm -rf server *.o *core *~ ./rel ./dbg

mkdir_dbg:
	@/bin/mkdir ./dbg
mkdir_rel:
	@/bin/mkdir ./rel