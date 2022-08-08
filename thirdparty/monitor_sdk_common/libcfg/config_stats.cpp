//
// Created by 黄伟锋 on 2019-04-13.
//

#include "thirdparty/monitor_sdk_common/libcfg/config_stats.h"
#include "thirdparty/monitor_sdk_common/libcfg/utils.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_utils.h"

using namespace common;


void ConfigStats::LogInfo(const std::string &msg) {
  CfgLogUtils::LogInfo(msg);
}


void ConfigStats::LogError(const std::string &msg) {
  CfgLogUtils::LogError(msg);
}


void ConfigStats::LogWarn(const std::string &msg) {
  CfgLogUtils::LogWarn(msg);
}