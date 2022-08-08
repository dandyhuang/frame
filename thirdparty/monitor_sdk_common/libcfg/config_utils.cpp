//
// Created by 黄伟锋 on 2019-04-16.
//

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_constants.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_utils.h"
#include "thirdparty/monitor_sdk_common/libcfg/utils.h"


using namespace common;


void CfgLogUtils::LogInfo(const std::string &msg) {
  if (Instance()->GetLogger() == nullptr) return;
  Instance()->GetLogger()->LogInfo(msg);
}


void CfgLogUtils::LogError(const std::string &msg) {
  if (Instance()->GetLogger() == nullptr) return;
  Instance()->GetLogger()->LogError(msg);
}


void CfgLogUtils::LogWarn(const std::string &msg) {
  if (Instance()->GetLogger() == nullptr) return;
  Instance()->GetLogger()->LogWarn(msg);
}


void CfgLogUtils::LogDebug(const std::string &msg) {
  if (Instance()->GetLogger() == nullptr) return;
  Instance()->GetLogger()->LogDebug(msg);
}


void CfgLogUtils::SetLogger(std::shared_ptr<ConfigLogger> logger) {
  logger_ = logger;
}


std::shared_ptr<ConfigLogger> CfgLogUtils::GetLogger() {
  return logger_;
}
