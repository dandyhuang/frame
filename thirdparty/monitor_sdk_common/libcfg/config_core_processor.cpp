//
// Created by 黄伟锋 on 2019-04-10.
//

#include "thirdparty/monitor_sdk_common/base/string/concat.h"
#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_constants.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_core_processor.h"

using namespace common;


ConfigCoreProcessor::ConfigCoreProcessor() {}


ConfigCoreProcessor::ConfigCoreProcessor(const PropertySource &source) {
  InitAppConfig(source);
}


ConfigCoreProcessor::~ConfigCoreProcessor() {}


bool ConfigCoreProcessor::Start() { return true; }


bool ConfigCoreProcessor::Close() { return true; }


int ConfigCoreProcessor::InitAppConfig(const PropertySource &source) {
  app_name_ = source.app_name;
  app_env_ = source.app_env;
  config_version_ = source.config_version;
  client_default_config_version_ = source.config_version;
  app_loc_ = source.app_loc;
  config_host_ = source.config_host;
  config_agent_ = source.config_agent;
  config_monitor_ = source.config_monitor;

  display_name_ = StringPrint("%s;%s;%s", app_name_, app_env_, config_version_);

  // 初始化时固定缓存名称，实际版本号可能会因设置了配置灰度发生变化
  InitCacheFilename();

  return 0;
}


int ConfigCoreProcessor::InitCacheFilename() {
  std::string sb = "";
  sb = StringConcat(sb, ConfigConstants::LOCAL_CACHE_FILE_NAME_PREFIX);
  sb = StringConcat(sb, app_name_);
  sb = StringConcat(sb, "-");
  sb = StringConcat(sb, app_env_);
  sb = StringConcat(sb, "-");
  sb = StringConcat(sb, config_version_);
  sb = StringConcat(sb, ConfigConstants::LOCAL_CACHE_FILE_NAME_SUFFIX);

  cache_file_name_ = sb;

  return 0;
}


std::string ConfigCoreProcessor::app_name() { return app_name_; }
void ConfigCoreProcessor::set_app_name(const std::string &app_name) {
  app_name_ = app_name;
}


std::string ConfigCoreProcessor::app_env() { return app_env_; }
void ConfigCoreProcessor::set_app_env(const std::string &app_env) {
  app_env_ = app_env;
}


std::string ConfigCoreProcessor::config_version() { return config_version_; }
void ConfigCoreProcessor::set_config_version(const std::string &config_version) {
  std::unique_lock<std::mutex> lk(config_version_mutex_);
  config_version_ = config_version;
}


std::string ConfigCoreProcessor::app_loc() { return app_loc_; }
void ConfigCoreProcessor::set_app_loc(const std::string &app_loc) {
  app_loc_ = app_loc;
}


std::string ConfigCoreProcessor::config_host() { return config_host_; }
void ConfigCoreProcessor::set_config_host(const std::string &config_host) {
  config_host_ = config_host;
}


std::string ConfigCoreProcessor::config_agent() { return config_agent_; }
void ConfigCoreProcessor::set_config_agent(const std::string &config_agent) {
  config_agent_ = config_agent;
}


std::string ConfigCoreProcessor::config_monitor() { return config_monitor_; }
void ConfigCoreProcessor::set_config_monitor(const std::string &config_monitor) {
  config_monitor_ = config_monitor;
}


std::string ConfigCoreProcessor::display_name() { return display_name_; }
void ConfigCoreProcessor::set_display_name(const std::string &display_name) { display_name_ = display_name; }


std::string ConfigCoreProcessor::cache_file_name() { return cache_file_name_; }


std::string ConfigCoreProcessor::client_default_config_version() { return client_default_config_version_; }
