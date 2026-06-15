CC=gcc
CFLAGS=-Wall -Wextra -g -Iinclude

TARGET=krakentrap

SRC=src/main.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
	rm -f examples/loop
