//
// Created by 陈嘉豪 on 2019/12/3.
//

#include "thirdparty/monitor_sdk_common/libhawking/gray_test_log_utils.h"

using namespace common;

void GrayTestLogUtils::LogInfo(const std::string &msg) {
    Instance()->GetLogger()->LogInfo(msg);
}

void GrayTestLogUtils::LogDebug(const std::string &msg) {
    Instance()->GetLogger()->LogDebug(msg);
}

void GrayTestLogUtils::LogWarn(const std::string &msg) {
    Instance()->GetLogger()->LogWarn(msg);
}

void GrayTestLogUtils::LogError(const std::string &msg) {
    Instance()->GetLogger()->LogError(msg);
}

void GrayTestLogUtils::SetLogger(std::shared_ptr<GrayTestLogger> logger) {
    if (logger == nullptr) {
        logger_ = std::shared_ptr<GrayTestLogger>(new GrayTestDefaultLogger());
    } else {
        logger_ = logger;
    }
}

std::shared_ptr<GrayTestLogger> GrayTestLogUtils::GetLogger() {
    return logger_;
}