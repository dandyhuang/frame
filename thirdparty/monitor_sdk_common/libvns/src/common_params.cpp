//
// Created by 吴婷 on 2020-07-01.
//

#include "thirdparty/monitor_sdk_common_params.h"

const std::string CommonParams::APP = "app";

const std::string CommonParams::TAG = "tag";

const std::string CommonParams::PID = "pid";

const std::string CommonParams::CODE = "code";

const std::string CommonParams::SERVICE_NAME = "serviceName";

const std::string CommonParams::CLUSTER_NAME = "clusterName";

const std::string CommonParams::NAMESPACE_ID = "namespaceId";

const std::string CommonParams::GROUP_NAME = "groupName";

const std::string CommonParams::ACCESS_TOKEN = "accessToken";

const std::string CommonParams::APP_ENV = "appEnv";

///////////////////

int PropertyKeyConst::UDP_PORT = 1111;

const std::string PropertyKeyConst::CACHE_PATH = "cachePath";

const std::string PropertyKeyConst::NAMESPACE = "namespace";

const std::string PropertyKeyConst::APP_ENV = "appEnv";

const std::string PropertyKeyConst::APP_SECRET = "appSecret";

const std::string PropertyKeyConst::APP_NAME = "appName";

const std::string PropertyKeyConst::APP_LOC = "appLoc";

const std::string PropertyKeyConst::CLUSTER_NAME = "clusterName";

const std::string PropertyKeyConst::NAMING_LOAD_CACHE_AT_START = "namingLoadCacheAtStart";  // 是否开启缓存

const std::string PropertyKeyConst::NAMING_CLIENT_BEAT_THREAD_COUNT = "namingClientBeatThreadCount";    // 心跳线程数

const std::string PropertyKeyConst::NAMING_POLLING_THREAD_COUNT = "namingPollingThreadCount";   // 线程池的线程数

//////////////////
std::string LocalProperties::app_name = "";

std::string LocalProperties::tag = "";

std::string LocalProperties::ip = "";

std::string LocalProperties::pid = "";

std::string LocalProperties::env = "";

std::string LocalProperties::app_loc = "";