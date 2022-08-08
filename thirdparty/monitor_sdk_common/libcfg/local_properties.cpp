//
// Created by 黄伟锋 on 2019-05-27.
//

#include "thirdparty/monitor_sdk_common/libcfg/local_properties.h"

using namespace common;


std::string LocalProperties::app_name = "";

std::string LocalProperties::app_env = "";

std::string LocalProperties::config_version = "";

std::string LocalProperties::config_host = "";

std::string LocalProperties::enable_remote_config = "true";

int LocalProperties::thread_pool_num = 4;