//
// Created by 吴婷 on 2019-12-02.
//
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "log_utils.h"

using namespace common;

void LogUtils::LogInfo(const std::string &msg) {
    if (Instance()->GetLogger() == nullptr) return;
    Instance()->GetLogger()->LogInfo(msg);
}

void LogUtils::LogError(const std::string &msg) {
    if (Instance()->GetLogger() == nullptr) return;
    Instance()->GetLogger()->LogError(msg);
}

void LogUtils::LogWarn(const std::string &msg) {
    if (Instance()->GetLogger() == nullptr) return;
    Instance()->GetLogger()->LogWarn(msg);
}

void LogUtils::LogDebug(const std::string &msg) {
    if (Instance()->GetLogger() == nullptr) return;
    Instance()->GetLogger()->LogDebug(msg);
}

void LogUtils::SetLogger(std::shared_ptr<VepLogger> logger) {
    logger_ = logger;
}

std::shared_ptr<VepLogger> LogUtils::GetLogger() {
    return logger_;
}