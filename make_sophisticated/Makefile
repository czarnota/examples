
prog_SOURCES=main.cpp
prog_CXXFLAGS=-std=c++17 -Wall -Werror -pedantic -g -O0
prog_LDFLAGS=-L. -lfoo
prog_DEPENDS=libfoo.so
TARGETS+=prog

libfoo.so_SOURCES=foo.cpp
libfoo.so_CXXFLAGS=-std=c++17 -Wall -Werror -pedantic -g -O0 -fPIC
libfoo.so_LDFLAGS=-shared
TARGETS+=libfoo.so

define compile
$(1)_OBJECTS_C = $$(patsubst %.c,%.o,$$(filter %.c,$$($(1)_SOURCES)))
$(1)_OBJECTS_CC = $$(patsubst %.cc,%.o,$$(filter %.cc,$$($(1)_SOURCES)))
$(1)_OBJECTS_CXX = $$(patsubst %.cpp,%.o,$$(filter %.cpp,$$($(1)_SOURCES)))

$(1): $$($(1)_OBJECTS_CXX) $$($(1)_OBJECTS_CC) $$($(1)_OBJECTS_C) $$($(1)_DEPENDS)
	$$(CXX) $$($(1)_LDFLAGS) $$^ -o $$@

$$($(1)_OBJECTS_CXX): %.o: %.cpp
	$$(CXX) $$($(1)_CXXFLAGS) -c $$^ -o $$@

$$($(1)_OBJECTS_CC): %.o: %.cc
	$$(CXX) $$($(1)_CXXFLAGS) -c $$^ -o $$@

$$($(1)_OBJECTS_C): %.o: %.c
	$$(CC) $$($(1)_CFLAGS) -c $$^ -o $$@

$(1)_clean:
	rm -fr $(1) $$($(1)_OBJECTS_CXX) $$($(1)_OBJECTS_CC) $$($(1)_OBJECTS_C)

clean: $(1)_clean
endef
$(foreach target,$(TARGETS),$(eval $(call compile,$(target))))

