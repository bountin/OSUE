#
#	Makefile for sort{client,serv}
#
#	Author: Martin Prebio <martin.prebio@students.tuwien.ac.at>
#

CC=gcc
DEFS=-D_XOPEN_SOURCE=500 -D_BSD_SOURCE
CFLAGS=-Wall -g -std=c99 -pedantic $(DEFS)

OBJECTFILES=sortserv.o sortclient.o shared.o

all: sortserv sortclient

sortserv: sortserv.o shared.o
	$(CC) $(LDFLAGS) -o sortserv sortserv.o shared.o -lsem182

sortclient: sortclient.o shared.o
	$(CC) $(LDFLAGS) -o sortclient sortclient.o shared.o -lsem182

%.o.: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTFILES)
	rm -f sortclient sortserv
