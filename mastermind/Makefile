CC=gcc
DEFS=-D_XOPEN_SOURCE=500 -D_BSD_SOURCE -DENDEBUG
CFLAGS=-Wall -g -std=c99 -pedantic $(DEFS)

SERVER_OBJECTFILES=server.o
CLIENT_OBJECTFILES=client.o gamelogic.o

all: server client

server: $(SERVER_OBJECTFILES)
	$(CC) $(LDFLAGS) -o $@ $^

client: $(CLIENT_OBJECTFILES)
	$(CC) $(LDFLAGS) -o $@ $^

%.o.: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(SERVER_OBJECTFILES)
	rm -f $(CLIENT_OBJECTFILES)
	rm -f server
	rm -f client
