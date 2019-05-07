TARGET=helloworld-make
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
CC=gcc
LDFLAGS=-lstdc++
CFLAGS=-Werror -pedantic -g

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

$(OBJECTS): %.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm -f $(OBJECTS) $(TARGET)

