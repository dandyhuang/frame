.PHONY:clean

CXX= g++
CPPFLAGS= -g --std=c++11 -Wall -O2 -fPIC
CFLAGS := $(CPPFLAGS)
BUILD = build
OBJ = local_cache_test.o
LIBS = -lpthread
FACTORY=test_factory
SCOPE=test_scope_guard
FIND = $(patsubst %.cc, $(BUILD)/%.$(2), $(shell find $(1) -name "*.cc" -type f))

INCLUDE= -I./ -Ifactory/ \
	-Ifactory/query_manager \
	-Iscope_guard

$(FACTORY): $(call FIND, factory factory/query_manager,o)
	$(CXX) $(filter %.o, $^) $(CFLAGS) $(INCLUDE) -o $@

$(SCOPE): $(call FIND, scope_guard,o)
	$(CXX) $(filter %.o, $^) $(CFLAGS) $(INCLUDE) -o $@


%.o: %.cc
	${CXX} -c ${CFLAGS} $(INCLUDE) $(CPPINCS) $< -o $@

# build object
$(BUILD)/%.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) $(INCLUDE) $(CFLAGS) -pthread \
    -c $< \
    -o $@

# calc dependency
$(BUILD)/%.d: %.cc 
	@mkdir -p $(dir $@)
	@$(CXX) -MM $(INCLUDE) $(CFLAGS) $< | \
    sed 's#\($(notdir $*)\)\.o[ :]*#$(BUILD)/$*.o $@: #g' > $@
all: $(FACTORY)

clean:
	rm -f *.o  $(FACTORY) $(SCOPE)