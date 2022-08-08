//
// Created by 黄伟锋 on 2019-04-22.
//

#ifndef COMMON_LIBCFG_CONFIG_LOGGER_H_
#define COMMON_LIBCFG_CONFIG_LOGGER_H_

class ConfigLogger {
 public:
  ConfigLogger() {}
  virtual ~ConfigLogger() {}

  virtual void LogInfo(const std::string &msg) {}
  virtual void LogError(const std::string &msg) {}
  virtual void LogWarn(const std::string &msg) {}
  virtual void LogDebug(const std::string &msg) {}
};

#endif //COMMON_LIBCFG_CONFIG_LOGGER_H_
