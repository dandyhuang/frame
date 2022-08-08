//
// Created by 黄伟锋 on 2019-04-24.
//

#include "thirdparty/monitor_sdk_common/librmq/rmq_constants.h"
#include "thirdparty/monitor_sdk_common/librmq/utils.h"

using namespace common;

std::string RmqConstants::VERSION = "9.0.0-vivo-cpp";

std::string RmqConstants::TOPIC_SPLIT = "/";

int RmqConstants::CACHING_CHANNEL_SIZE = 100;

int RmqConstants::HEART_BEAT_TIME = 45;

int RmqConstants::CONNECTION_TIMEOUT = 30000;

int RmqConstants::DEFAULT_THREAD_NUM = 8;

int RmqConstants::DEFAULT_PREFETCH = 32;

int RmqConstants::START_THREAD_NUM = 1;

int RmqConstants::DEFAULT_RETRY_TRIMES = 3;

int RmqConstants::UNLIMITS_RETRY_TIMES = 0;

long RmqConstants::DEFAULT_INITIAL_INTERVAL = 10000L;

std::string RmqConstants::NAME_SERVER_PREFIX = "rmq.name.url";

std::string RmqConstants::NAME_SERVER_AUTH_API = "/v1/name/sdk/auth";

std::string RmqConstants::NAME_SERVER_BLOCKED_API = "/v1/name/sdk/block";

std::string RmqConstants::NAME_SERVER_MERSURE_API = "/v1/name/sdk/heartbeat";

std::string RmqConstants::APP_NAME = "app.name";

std::string RmqConstants::NONCE = "100";

int RmqConstants::DEFAULT_SEND_TIMES = 1;

std::string RmqConstants::MODULE_NAME = "vivormq-client-cpp";

int RmqConstants::QOS_PREFETCH_COUNT = 32;

int RmqConstants::CHECK_USABLE_INTERVAL = 2 * 1000;

int RmqConstants::CONNECTION_RETRY_INTERVAL = 5 * 1000;

/////////////////////////
std::string RmqHeaders::appname = "";

std::string RmqHeaders::appnode = RmqIpUtils::get_host_name();