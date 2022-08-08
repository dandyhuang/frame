CC = g++

INCLUDES = -I. 
FILTERS = test_vivolog.cpp test_vivotrace.cpp
FILES := $(wildcard *.cpp) 
	 
SOURCES = $(filter-out $(FILTERS), $(FILES))
OBJS = $(patsubst %.cpp, %.o, $(SOURCES))
OBJS_DEMO  = $(OBJS) test_vivolog.o
OBJS_DEMO1  = $(OBJS) test_vivotrace.o

#CXXFLAGS = -Wall -O3 -g
CXXFLAGS = -Wall -g
LDFLAGS = -pthread 

%.o:%.cpp
	$(CC) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) -fPIC -c $< -o $@

LIBINDEX_SO=libvivolog.so
LIBINDEX_A=libvivolog.a

TARGET=$(LIBINDEX_SO) $(LIBINDEX_A)
TEST_MAIN=test_vivolog
TEST_MAIN1=test_vivotrace

all:$(TARGET) $(TEST_MAIN) $(TEST_MAIN1)

$(LIBINDEX_SO): $(OBJS)
	$(CC) -shared $(LDFLAGS) $(OBJS) -o $@

$(LIBINDEX_A): $(OBJS)
	ar cru $@ $^

$(TEST_MAIN): $(OBJS_DEMO) 
	$(CC) $(LDFLAGS) $(OBJS_DEMO) -o $@

$(TEST_MAIN1): $(OBJS_DEMO1) 
	$(CC) $(LDFLAGS) $(OBJS_DEMO1) -o $@

test:
	./$(TEST_MAIN)

clean:
	rm -rf $(TARGET) $(TEST_MAIN) $(TEST_MAIN1) *.o logs

install:
	mkdir -p /usr/local/include/libindex/
	mkdir -p /usr/local/lib/
	
	rm -f /usr/local/lib/libindex.*
	
	cp -f *.h /usr/local/include/libindex/
	cp -f $(TARGET) /usr/local/lib/
	cp -f $(TARGET) ../indexserver/lib/
	/sbin/ldconfig

# valgrind --tool=memcheck --leak-check=full --show-reachable=yes ./test_vivotrace 10000	
	