main_CC=gcc
main_TARGET=main
main_SOURCES=$(wildcard *.S)
main_DEFAULT_CFLAGS=
main_DEFAULT_LDFLAGS=-nostdlib
main_CFLAGS=
main_LDFLAGS=
main_OBJECTS=$(main_SOURCES:.S=.o)
main_CLEAN=$(main_OBJECTS) $(main_TARGET)


all: $(main_TARGET)


$(main_TARGET): $(main_OBJECTS)
	$(main_CC) $(main_DEFAULT_LDFLAGS) $(main_LDFLAGS) $(main_OBJECTS) -o $(main_TARGET)


$(main_OBJECTS): %.o: %.S
	$(main_CC) $(main_DEFAULT_CFLAGS) $(main_CFLAGS) -c $^ -o $@


clean:
	rm -fr $(main_CLEAN)
