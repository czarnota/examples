TARGET=example-packet-mmap
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
CC=gcc
LDFLAGS=
CFLAGS=-Werror -g -std=c99 -D_GNU_SOURCE

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

$(OBJECTS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm -f $(OBJECTS) $(TARGET)

