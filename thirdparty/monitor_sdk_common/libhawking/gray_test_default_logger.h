//
// Created by 陈嘉豪 on 2019/12/3.
//

#ifndef COMMON_LIBHAWKING_GRAY_TEST_DEFAULT_LOGGER_H
#define COMMON_LIBHAWKING_GRAY_TEST_DEFAULT_LOGGER_H

#include "thirdparty/monitor_sdk_common/libhawking/gray_test_logger.h"

#include "thirdparty/glog/logging.h"

namespace common {

class GrayTestDefaultLogger : public GrayTestLogger {
public:
    GrayTestDefaultLogger() = default;
    
    virtual ~GrayTestDefaultLogger() {}
    
    void LogInfo(const std::string &msg) override {
        LOG(INFO) << msg;
    }
    
    void LogDebug(const std::string &msg) override {
        DLOG(INFO) << msg;
    }
    
    void LogWarn(const std::string &msg) override {
        LOG(WARNING) << msg;
    }
    
    void LogError(const std::string &msg) override {
        LOG(ERROR) << msg;
    }
};

} // namespace common

#endif //COMMON_LIBHAWKING_GRAY_TEST_DEFAULT_LOGGER_H
