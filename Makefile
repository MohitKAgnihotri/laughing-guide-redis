# Makefile for string protocol, server, and client
CC=/usr/bin/clang
CFLAGS=-Wall  -Werror -Wall -Wpedantic -std=c17
LDFLAGS=-lpthread
CFLAGS_DEBUG= -g3
CFLAGS_LOG_FILE = -DLOG_FILE_ENABLED
CFLAGS_REL= -O2

default: build

rebuild: clean default

build: build_rel build_dbg

build_rel: server-rel server-lite

build_dbg: server-dbg server-full


server-dbg: mkdir_dbg hashtable
	$(CC) $(CFLAGS) $(CFLAGS_DEBUG) -o ./dbg/server_dbg redis.c hastable.o $(LDFLAGS)

server-rel: mkdir_rel hashtable
	$(CC) $(CFLAGS) $(CFLAGS_REL) -o ./rel/server_rel redis.c hastable.o $(LDFLAGS)

server-lite: mkdir_dbg hashtable
	$(CC) $(CFLAGS) $(CFLAGS_DEBUG) -o ./rel/server_lite redis.c hastable.o $(LDFLAGS)

server-full: mkdir_rel hashtable
	$(CC) $(CFLAGS) $(CFLAGS_DEBUG) $(CFLAGS_LOG_FILE) -o ./dbg/server_full redis.c hastable.o $(LDFLAGS)

hashtable: hastable.c
	$(CC) $(CFLAGS) -c hastable.c
clean:
	@/bin/rm -rf server *.o *core *~ ./rel ./dbg

mkdir_dbg:
	@/bin/mkdir ./dbg
mkdir_rel:
	@/bin/mkdir ./rel