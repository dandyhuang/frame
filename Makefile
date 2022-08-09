.PHONY:clean


CXX= g++
CPPFLAGS= -g --std=c++11 -Wall -O2 -fPIC
CFLAGS := $(CPPFLAGS)
BUILD = build
BIN = bin
FACTORY=$(BIN)/test_factory
SCOPE=$(BIN)/test_scope_guard
DAG=$(BIN)/test_dag
FIND = $(patsubst %.cc, $(BUILD)/%.$(2), $(shell find $(1) -name "*.cc" -type f))
FIND_CPP = $(patsubst %.cpp, $(BUILD)/%.$(2), $(shell find $(1) -name "*.cpp" -type f))


all: $(FACTORY) $(SCOPE) $(DAG)
.PHONY : all
mkdir -p $(BIN)
THIRD_PARTY=./thirdparty/
BOOST_INC = -I$(THIRD_PARTY)/boost/include
BOOST_LIB = -L$(THIRD_PARTY)/boost/lib64/ -lboost_filesystem -L$(THIRD_PARTY)/boost/lib64/ -lboost_system -L$(THIRD_PARTY)/boost/lib64/ -lboost_locale \
		-L$(THIRD_PARTY)/boost/lib64/ -lboost_thread -L$(THIRD_PARTY)/boost/lib64/ -lboost_regex
BRPC_INC=-I$(THIRD_PARTY)/brpc/include -I$(THIRD_PARTY)/glog/include -I$(THIRD_PARTY)/gtest/include \
	 -I$(THIRD_PARTY)/gflags/include -I$(THIRD_PARTY)/protobuf/include \
	-I$(THIRD_PARTY)/xml/include $(BOOST_INC) 
BRPC_LIB= -L$(THIRD_PARTY)/glog/lib64/ -lglog -L$(THIRD_PARTY)/brpc/lib64/ -lbrpc \
	 -lbthread -lbvar -lbutil -ljson2pb -lmcpack2pb -lcc_brpc_internal_proto -lcc_brpc_idl_options_proto \
	 -L$(THIRD_PARTY)/leveldb/lib64/ -lleveldb \
	 -L$(THIRD_PARTY)/gflags/lib64/ -lgflags  \
	 -L$(THIRD_PARTY)/gtest/lib64/ -lgtest -L$(THIRD_PARTY)/protobuf/lib64/ -lprotobuf -lprotoc \
	 -L$(THIRD_PARTY)/xml/lib64/ -l:libtinyxml.a $(BOOST_LIB) -lssl -lcrypto -pthread -ldl -lpthread  -lcrypt -lm 




INCLUDE= -I./ -Ifactory/ \
	-Ifactory/query_manager \
	-Iscope_guard \
	-Idag/ -Icontext \
	-Icommon -Icommon/conf $(BRPC_INC)

LIBS = $(BRPC_LIB)

f_source = $(call FIND, factory factory/query_manager,o) build/$(FACTORY).o
$(warning  "f_socure:$(f_source)"")
$(FACTORY): $(call FIND, factory factory/query_manager,o) build/$(FACTORY).o
	$(CXX) $(filter %.o, $^)  $(CFLAGS) $(INCLUDE) -o $@ $(LIBS)

$(SCOPE): $(call FIND, scope_guard,o)  build/$(SCOPE).o
	$(CXX) $(filter %.o, $^)  $(CFLAGS) $(INCLUDE) -o $@ $(LIBS)

$(DAG): $(call FIND_CPP, dag common/conf common dag_test,o)  build/$(DAG).o
	$(CXX) $(filter %.o, $^) $(CFLAGS) $(INCLUDE) -o $@ $(LIBS)



# $(BUILD)/%.o: %.cc
# 	${CXX} -c ${CFLAGS} $(INCLUDE) $(CPPINCS) $< -o $@

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



clean:
	rm -rf $(BUILD) *.o  $(FACTORY) $(SCOPE) $(DAG)