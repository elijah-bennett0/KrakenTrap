CC=gcc
CFLAGS=-Wall -Wextra -g

all:
	$(CC) $(CFLAGS) src/main.c -o krakentrap

example:
	$(CC) -g -no-pie -fno-stack-protector examples/loop.c -o examples/loop

run: all example
	./krakentrap ./examples/loop

clean:
	rm -f krakentrap examples/loop
