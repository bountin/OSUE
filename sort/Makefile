#
#	Makefile for sort{client,serv}
#
#	Author: Martin Prebio <martin.prebio@students.tuwien.ac.at>
#

CC=gcc -I/usr/local/include/ -L/usr/local/lib/ -DENDEBUG
DEFS=-D_XOPEN_SOURCE=500 -D_BSD_SOURCE
CFLAGS=-Wall -g -std=c99 -pedantic $(DEFS)

OBJECTFILES=sortserv.o sortclient.o shared.o

all: sortserv sortclient

sortserv: sortserv.o shared.o
	$(CC) $(LDFLAGS) -o sortserv -lsem182 sortserv.o shared.o

sortclient: sortclient.o shared.o
	$(CC) $(LDFLAGS) -o sortclient -lsem182 sortclient.o shared.o

%.o.: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTFILES)
	rm -f sortclient sortserv
