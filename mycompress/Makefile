CC=gcc
DEFS=-D_XOPEN_SOURCE=500 -D_BSD_SOURCE
CFLAGS=-Wall -g -std=c99 -pedantic $(DEFS)

OBJECTFILES=compress.o mycompress.o

mycompress: $(OBJECTFILES)
	$(CC) $(LDFLAGS) -o $@ $^
%.o.: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

mycompress.c: compress.h

clean:
	rm -f $(OBJECTFILES)
	rm -f mycompress