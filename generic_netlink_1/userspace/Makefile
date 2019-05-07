TARGET = prog_generic_netlink_1

prog_generic_netlink_1_SOURCES = main.cpp
prog_generic_netlink_1_OBJECTS = $(prog_generic_netlink_1_SOURCES:.cpp=.o)
prog_generic_netlink_1_LDFLAGS = -lnl-genl-3 -lnl-3
prog_generic_netlink_1_CXXFLAGS = -Wall -Werror -pedantic -g -O0 -std=c++17 -I/usr/include/libnl3 -I../kernelspace

all: $(TARGET)

$(TARGET): $(prog_generic_netlink_1_OBJECTS)
	$(CXX) $^ $(prog_generic_netlink_1_LDFLAGS) -o $@

$(prog_generic_netlink_1_OBJECTS): %.o: %.cpp
	$(CXX) $^ $(prog_generic_netlink_1_CXXFLAGS) -c -o $@

clean:
	rm -fr $(TARGET) $(prog_generic_netlink_1_OBJECTS)




