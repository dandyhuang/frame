//
// Created by 吴婷 on 2019-12-02.
//

#ifndef COMMON_LOG_UTILS_H
#define COMMON_LOG_UTILS_H

#include <memory>
#include <string>

#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/base/singleton.h"
#include "utils/vns_logger.h"

class LogUtils : public common::SingletonBase<LogUtils> {
public:
    static void LogInfo(const std::string &msg);

    static void LogError(const std::string &msg);

    static void LogWarn(const std::string &msg);

    static void LogDebug(const std::string &msg);

    void SetLogger(std::shared_ptr <VnsLogger> logger);

    std::shared_ptr <VnsLogger> GetLogger();

private:
    std::shared_ptr <VnsLogger> logger_ = nullptr;
    static bool is_validate;
};

#endif //COMMON_LOG_UTILS_H
