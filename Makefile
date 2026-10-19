CC=gcc
CFLAGS=-Wall -Wextra -g -Iinclude
LDLIBS=-lbfd

SRC=src/main.c src/process.c src/repl.c src/breakpoint.c src/registers.c src/memory.c src/symbols.c

all:
	$(CC) $(CFLAGS) $(SRC) $(LDLIBS) -o krakentrap

example:
	$(CC) -g -no-pie -fno-stack-protector examples/loop.c -o examples/loop

run: all example
	./krakentrap ./examples/loop

clean:
	rm -f krakentrap examples/loop
