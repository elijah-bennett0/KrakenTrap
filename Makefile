CC=gcc
CFLAGS=-Wall -Wextra -g -Iinclude

SRC=src/main.c src/process.c src/repl.c src/breakpoint.c src/registers.c

all:
	$(CC) $(CFLAGS) $(SRC) -o krakentrap

example:
	$(CC) -g -no-pie -fno-stack-protector examples/loop.c -o examples/loop

run: all example
	./krakentrap ./examples/loop

clean:
	rm -f krakentrap examples/loop
