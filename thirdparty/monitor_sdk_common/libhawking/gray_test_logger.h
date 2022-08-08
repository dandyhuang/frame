//
// Created by 陈嘉豪 on 2019/12/3.
//

#ifndef COMMON_LIBHAWKING_GRAY_TEST_LOGGER_H
#define COMMON_LIBHAWKING_GRAY_TEST_LOGGER_H

#include <string>

namespace common {

class GrayTestLogger {
public:
    GrayTestLogger() = default;
    virtual ~GrayTestLogger() {}
    
    virtual void LogInfo(const std::string &msg) {}
    virtual void LogError(const std::string &msg) {}
    virtual void LogWarn(const std::string &msg) {}
    virtual void LogDebug(const std::string &msg) {}
};

} // namespace common

#endif //COMMON_LIBHAWKING_GRAY_TEST_LOGGER_H
