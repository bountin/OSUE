#
#	Makefile for svctl
#
#	Author: Martin Prebio <martin.prebio@students.tuwien.ac.at>
#

CC=gcc
DEFS=-D_XOPEN_SOURCE=500 -D_BSD_SOURCE
CFLAGS=-Wall -g -std=c99 -pedantic $(DEFS)

OBJECTFILES=svctl.o

all: svctl

svctl: svctl.o
	$(CC) $(LDFLAGS) -o svctl svctl.o

%.o.: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTFILES)
	rm -f svctl
