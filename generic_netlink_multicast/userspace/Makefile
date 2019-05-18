TARGET = prog_generic_netlink_multicast

prog_generic_netlink_multicast_SOURCES = main.cpp
prog_generic_netlink_multicast_OBJECTS = $(prog_generic_netlink_multicast_SOURCES:.cpp=.o)
prog_generic_netlink_multicast_LDFLAGS = -lnl-genl-3 -lnl-3
prog_generic_netlink_multicast_CXXFLAGS = -Wall -Werror -pedantic -g -O0 -std=c++17 -I/usr/include/libnl3 -I../kernelspace

all: $(TARGET)

$(TARGET): $(prog_generic_netlink_multicast_OBJECTS)
	$(CXX) $^ $(prog_generic_netlink_multicast_LDFLAGS) -o $@

$(prog_generic_netlink_multicast_OBJECTS): %.o: %.cpp
	$(CXX) $^ $(prog_generic_netlink_multicast_CXXFLAGS) -c -o $@

clean:
	rm -fr $(TARGET) $(prog_generic_netlink_multicast_OBJECTS)




