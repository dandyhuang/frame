.PHONY:clean

CXX= g++
CPPFLAGS= -g --std=c++11 -Wall -O2 -fPIC
CFLAGS := $(CPPFLAGS)
BUILD = build
FACTORY=test_factory
SCOPE=test_scope_guard
DAG=test_dag
FIND = $(patsubst %.cc, $(BUILD)/%.$(2), $(shell find $(1) -name "*.cc" -type f))
FIND_CPP = $(patsubst %.cpp, $(BUILD)/%.$(2), $(shell find $(1) -name "*.cpp" -type f))


THIRD_PARTY=./thirdparty/
BRPC_INC=-I$(THIRD_PARTY)/brpc/include
BRPC_LIB=-L$(THIRD_PARTY)/brpc/lib64/ -lbrpc

INCLUDE= -I./ -Ifactory/ \
	-Ifactory/query_manager \
	-Iscope_guard \
	-Idag/ -Icontext \
	-Icommon -Icommon/conf $(BRPC_INC)
LIBS = -lpthread $(BRPC_LIB)

$(FACTORY): $(call FIND, factory factory/query_manager,o) $(FACTORY).o
	$(CXX) $(filter %.o, $^)  $(CFLAGS) $(INCLUDE) -o $@ $(LIBS)

$(SCOPE): $(call FIND, scope_guard,o)  $(SCOPE).o
	$(CXX) $(filter %.o, $^)  $(CFLAGS) $(INCLUDE) -o $@ $(LIBS)

$(DAG): $(call FIND_CPP, dag common/conf common,o)  $(dag).o
	$(CXX) $(filter %.o, $^) $(CFLAGS) $(INCLUDE) -o $@ $(LIBS)



$(BUILD)/%.o: %.cc
	${CXX} -c ${CFLAGS} $(INCLUDE) $(CPPINCS) $< -o $@

# %.o: %.cpp
# 	${CXX} -c ${CFLAGS} $(INCLUDE) $(CPPINCS) $< -o $@


# build object
$(BUILD)/%.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) $(INCLUDE) $(CFLAGS) -pthread \
    -c $< \
    -o $@

$(BUILD)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(INCLUDE) $(CFLAGS) -pthread \
    -c $< \
    -o $@

# calc dependency
$(BUILD)/%.d: %.cc 
	@mkdir -p $(dir $@)
	@$(CXX) -MM $(INCLUDE) $(CFLAGS) $< | \
    sed 's#\($(notdir $*)\)\.o[ :]*#$(BUILD)/$*.o $@: #g' > $@

all: $(FACTORY) $(SCOPE) $(DAG)

clean:
	rm -rf $(build) *.o  $(FACTORY) $(SCOPE)