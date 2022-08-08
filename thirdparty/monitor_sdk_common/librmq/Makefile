###########################
# general
###########################
RED = \\e[1m\\e[31m
DARKRED = \\e[31m
GREEN = \\e[1m\\e[32m
DARKGREEN = \\e[32m
BLUE = \\e[1m\\e[34m
DARKBLUE = \\e[34m
YELLOW = \\e[1m\\e[33m
DARKYELLOW = \\e[33m
MAGENTA = \\e[1m\\e[35m
DARKMAGENTA = \\e[35m
CYAN = \\e[1m\\e[36m
DARKCYAN = \\e[36m
RESET = \\e[m

CPP = cpp
CC  = gcc
CXX = g++
LD = g++
AR  = ar

BIN_DIR = $(HOME_DIR)/bin
OBJ_DIR = $(HOME_DIR)/obj
OUTPUT_DIR = $(HOME_DIR)/output
LIB_DIR = $(OUTPUT_DIR)/lib
INC_DIR = .
SRC_DIR = .

OBJ_EXT= .o
CXXSRC_EXT= .cpp
CSRC_EXT= .c

AUTO_OBJS = \
$(patsubst $(SRC_DIR)/%$(CXXSRC_EXT), $(OBJ_DIR)/%$(OBJ_EXT), $(wildcard $(SRC_DIR)/*$(CXXSRC_EXT))) \
$(patsubst $(SRC_DIR)/%$(CSRC_EXT), $(OBJ_DIR)/%$(OBJ_EXT), $(wildcard $(SRC_DIR)/*$(CSRC_EXT)))

###########################
# violate
###########################

HOME_DIR = .
ROOT_DIR = $(HOME_DIR)/../..

CFLAGS  = -g -fpermissive -fPIC -O2 -Wall -pipe -fno-ident -D_GNU_SOURCE -D_REENTRNT -Wno-strict-aliasing -D__STDC_LIMIT_MACROS -std=c++11 -m64 -static-libgcc -static-libstdc++
CXXFLAGS = $(CFLAGS)
LDFLAGS = -L$(LIB_DIR) -lrmq -lfile -l_thread_group -l_thread_pool -l_sync_object -ltime -lrt -l_thread -l_this_thread \
 -lcheck_error -lclosure -linfo -lclient -ltypes -lurl -lencoding -lstringencoders -ldomain_resolver -lsocket -lsocket_address -lip_address -l_class_registry \
 -lpath -lwildcard -luuid -l_algorithm -l_concat -l_string_piece -l_compare -l_format -lglog -lgflags -l_number -lev -lamqp -lssl -lcrypto -lpthread -ldl -ljsoncpp

INC = -I$(INC_DIR) -I$(ROOT_DIR) -I$(ROOT_DIR)/thirdparty/gflags-2.2.1/include -I$(ROOT_DIR)/thirdparty/gtest-1.6.0/include

RMQ_LIB = $(LIB_DIR)/librmq.a
RMQ_LIB_OBJ = $(OBJ_DIR)/rmq_auth_request.o \
$(OBJ_DIR)/rmq_connection_config.o \
$(OBJ_DIR)/rmq_client.o \
$(OBJ_DIR)/rmq_metric.o \
$(OBJ_DIR)/rmq_remote_service.o \
$(OBJ_DIR)/rmq_scheduled_executor.o \
$(OBJ_DIR)/utils.o \
$(OBJ_DIR)/rmq_constants.o \
$(OBJ_DIR)/rmq_vhost_client.o \
$(OBJ_DIR)/rmq_consumer_client.o \
$(OBJ_DIR)/rmq_producer_client.o

CONSUMER_CLIENT_TEST = $(BIN_DIR)/consumer-client-test
CONSUMER_CLIENT_TEST_OBJ = $(OBJ_DIR)/test_consumer_client.o \

PRODUCER_CLIENT_TEST = $(BIN_DIR)/producer-client-test
PRODUCER_CLIENT_TEST_OBJ = $(OBJ_DIR)/test_producer_client.o

.PHONY: all clean

all: $(RMQ_LIB) $(PRODUCER_CLIENT_TEST) $(CONSUMER_CLIENT_TEST)

$(RMQ_LIB): $(RMQ_LIB_OBJ)
	@echo
	@echo -e $(CYAN)Building$(RESET) $(GREEN)$^$(RESET) to $(RED)$@$(RESET)
	@-mkdir -p $(LIB_DIR)
	$(BUILD_A)

$(CONSUMER_CLIENT_TEST): $(CONSUMER_CLIENT_TEST_OBJ)
	@echo
	@echo -e $(CYAN)Building$(RESET) $(GREEN)$^$(RESET) to $(RED)$@$(RESET)
	@-mkdir -p $(BIN_DIR)
	$(BUILD_BIN)

$(PRODUCER_CLIENT_TEST): $(PRODUCER_CLIENT_TEST_OBJ)
	@echo
	@echo -e $(CYAN)Building$(RESET) $(GREEN)$^$(RESET) to $(RED)$@$(RESET)
	@-mkdir -p $(BIN_DIR)
	$(BUILD_BIN)

clean:
	@echo
	@echo -e  $(CYAN)Cleaning...$(RESET)
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)
	rm -rf $(OUTPUT_DIR)
	rm -rf librmq.tgz

###########################
# constant
###########################

BUILD_BIN = $(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
BUILD_SO = $(CXX) -o $@ $^ -shared $(LDFLAGS)
BUILD_A = $(AR) crv $@ $^

$(OBJ_DIR)/%$(OBJ_EXT): $(SRC_DIR)/%$(CXXSRC_EXT)
	@echo
	@echo -e $(CYAN)Compiling$(RESET) $(GREEN)$<$(RESET) to $(MAGENTA)$@$(RESET)
	@-mkdir -p $(OBJ_DIR)
	$(CXX) $(INC) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%$(OBJ_EXT): $(SRC_DIR)/%$(CSRC_EXT)
	@echo
	@echo -e $(CYAN)Compiling$(RESET) $(GREEN)$<$(RESET) to $(MAGENTA)$@$(RESET)
	@-mkdir -p $(OBJ_DIR)
	$(CC) $(INC) $(CFLAGS) -c $< -o $@
