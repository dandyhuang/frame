//
// Created by 黄伟锋 on 2019-04-16.
//

#ifndef COMMON_LIBCFG_CONFIG_UTILS_H_
#define COMMON_LIBCFG_CONFIG_UTILS_H_

#include <memory>
#include <string>

#include "thirdparty/monitor_sdk_common/base/singleton.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_logger.h"


namespace common {

class CfgLogUtils : public SingletonBase<CfgLogUtils>  {
 public:
  static void LogInfo(const std::string &msg);
  static void LogError(const std::string &msg);
  static void LogWarn(const std::string &msg);
  static void LogDebug(const std::string &msg);

  void SetLogger(std::shared_ptr<ConfigLogger> logger);
  std::shared_ptr<ConfigLogger> GetLogger();

 private:
  std::shared_ptr<ConfigLogger> logger_ = nullptr;
};


} // namespace common

#endif //COMMON_LIBCFG_CONFIG_UTILS_H_
