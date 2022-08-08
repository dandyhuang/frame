//
// Created by 黄伟锋 on 2019-04-10.
//

#ifndef COMMON_LIBCFG_CONFIG_CORE_PROCESSOR_H_
#define COMMON_LIBCFG_CONFIG_CORE_PROCESSOR_H_

#include <atomic>
#include <mutex>
#include <string>

#include "thirdparty/monitor_sdk_common/libcfg/property_source.h"


namespace common {

class ConfigCoreProcessor {
 public:
  ConfigCoreProcessor();
  ConfigCoreProcessor(const PropertySource &source);
  virtual ~ConfigCoreProcessor();

  // start
  virtual bool Start();

  // close
  virtual bool Close();
  
  std::string app_name();
  void set_app_name(const std::string &app_name);
  std::string app_env();
  void set_app_env(const std::string &app_env);
  std::string config_version();
  void set_config_version(const std::string &config_version);
  std::string app_loc();
  void set_app_loc(const std::string &app_loc);
  std::string config_host();
  void set_config_host(const std::string &config_host);
  std::string config_agent();
  void set_config_agent(const std::string &config_agent);
  std::string config_monitor();
  void set_config_monitor(const std::string &config_monitor);
  std::string display_name();
  void set_display_name(const std::string &display_name);
  std::string cache_file_name();

 protected:

  std::string client_default_config_version();

 private:
  int InitAppConfig(const PropertySource &source);
  int InitCacheFilename();


  // 配置应用名
  std::string app_name_;

  // 配置应用环境
  std::string app_env_;

  // 当前实际配置版本
  std::string config_version_;

  // 客户单原始配置的配置版本
  std::string client_default_config_version_;

  // 应用部署机房
  std::string app_loc_;

  // 配置中心地址
  std::string config_host_;

  // 配置中心Agent地址
  std::string config_agent_;

  // 配置监控上报地址
  std::string config_monitor_;
  std::string display_name_;
  std::string cache_file_name_;

  // 读写锁
  std::mutex config_version_mutex_;
};

} // namespace common

#endif // LIBCFG_CONFIG_CORE_PROCESSOR_H_



