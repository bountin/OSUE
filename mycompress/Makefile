$CC = gcc -std=c99 -pedantic -Wall -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -gdb -c

mycompress: *.c
	CC -o mycompress *.c
	
clean:
	rm -f mycompress
