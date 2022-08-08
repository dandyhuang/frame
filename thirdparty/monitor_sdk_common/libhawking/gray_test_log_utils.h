//
// Created by 陈嘉豪 on 2019/12/3.
//

#ifndef COMMON_LIBHAWKING_GRAY_TEST_LOG_UTILS_H
#define COMMON_LIBHAWKING_GRAY_TEST_LOG_UTILS_H

#include <memory>
#include <string>

#include "thirdparty/monitor_sdk_common/base/singleton.h"
#include "thirdparty/monitor_sdk_common/libhawking/gray_test_default_logger.h"
#include "thirdparty/monitor_sdk_common/libhawking/gray_test_logger.h"

namespace common {

class GrayTestLogUtils : public SingletonBase<GrayTestLogUtils> {
public:
    static void LogInfo(const std::string &msg);
    static void LogDebug(const std::string &msg);
    static void LogWarn(const std::string &msg);
    static void LogError(const std::string &msg);
    
    void SetLogger(std::shared_ptr<GrayTestLogger> logger);
    std::shared_ptr<GrayTestLogger> GetLogger();
private:
    std::shared_ptr<GrayTestLogger> logger_;
};

} // namespace common

#endif //COMMON_LIBHAWKING_GRAY_TEST_LOG_UTILS_H
