TARGET=tuntap-example
CC=gcc
DEFAULT_CFLAGS=-g -Wall -Werror
DEFAULT_LDFLAGS=
CFLAGS=
LDFLAGS=
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(DEFAULT_LDFLAGS) $(LDFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(DEFAULT_CFLAGS) $(CFLAGS) -c $^ -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

test: $(TARGET)
	sudo ./test.sh
