//
// Created by 吴婷 on 2019-12-02.
//

#ifndef COMMON_LOG_UTILS_H
#define COMMON_LOG_UTILS_H

#include <memory>
#include <string>

#include "thirdparty/monitor_sdk_common/base/singleton.h"
#include "vep_logger.h"

namespace common{

class LogUtils : public SingletonBase<LogUtils> {
public:
    static void LogInfo(const std::string &msg);

    static void LogError(const std::string &msg);

    static void LogWarn(const std::string &msg);

    static void LogDebug(const std::string &msg);

    void SetLogger(std::shared_ptr <VepLogger> logger);

    std::shared_ptr <VepLogger> GetLogger();

private:
    std::shared_ptr <VepLogger> logger_ = nullptr;
};

} //namespace common

#endif //COMMON_LOG_UTILS_H
