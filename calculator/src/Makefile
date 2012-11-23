CC=gcc
DEFS=-D_XOPEN_SOURCE=500 -D_BSD_SOURCE -DENDEBUG
CFLAGS=-Wall -g -std=c99 -pedantic $(DEFS)

OBJECTFILES=calculator.o child.o parent.o

all: calculator

calculator: $(OBJECTFILES)
	$(CC) $(LDFLAGS) -o $@ $^
%.o.: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTFILES)
	rm -f calculator
