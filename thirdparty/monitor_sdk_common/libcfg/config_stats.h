//
// Created by 黄伟锋 on 2019-04-13.
//

#ifndef COMMON_LIBCFG_CONFIG_STATS_H_
#define COMMON_LIBCFG_CONFIG_STATS_H_

#include <string>


namespace common {

class ConfigStats {
 public:
  static void LogInfo(const std::string &msg);
  static void LogError(const std::string &msg);
  static void LogWarn(const std::string &msg);
};

} // namespace common

#endif //COMMON_LIBCFG_CONFIG_STATS_H_
