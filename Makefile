.PHONY:clean

CXX= g++
CPPFLAGS= -g --std=c++11 -Wall -O2 -fPIC
CFLAGS := $(CPPFLAGS)
BUILD = build
OBJ = local_cache_test.o
LIBS = -lpthread
BIN=test_factory
FIND = $(patsubst %.cc, $(BUILD)/%.$(2), $(shell find $(1) -name "*.cc" -type f))

$(BIN): $(call FIND, factory factory/query_manager,o)
	$(CXX) $(filter %.o, $^) $(CFLAGS) -o $@

%.o: %.cc
	${CXX} -c ${CFLAGS} $(INCLUDE) $(CPPINCS) $< -o $@

all: $(BIN)

clean:
	rm -f *.o  $(BIN) 