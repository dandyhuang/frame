//
// Created by 黄伟锋 on 2019-04-10.
//

#include <algorithm>
#include <cassert>
#include <memory>
#include <sstream>

#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/base/string/compare.h"
#include "thirdparty/monitor_sdk_common/base/string/concat.h"
#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/base/string/string_piece.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_constants.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_loader_factory.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_manager_internal.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_stats.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_utils.h"
#include "thirdparty/monitor_sdk_common/libcfg/global_client_settings.h"
#include "thirdparty/monitor_sdk_common/libcfg/http_config_loader.h"
#include "thirdparty/monitor_sdk_common/libcfg/local_properties.h"
#include "thirdparty/monitor_sdk_common/libcfg/utils.h"
#include "thirdparty/monitor_sdk_common/net/uri/uri.h"

#include "thirdparty/gflags/gflags.h"

using namespace common;

bool is_string_empty(const std::string& s);
bool is_string_number(const std::string& s);
bool to_bool(const std::string &str);


ConfigLoaderFactory::ConfigLoaderFactory() {
  Init();
}


ConfigLoaderFactory::~ConfigLoaderFactory() {}


void ConfigLoaderFactory::Init() {
  std::string str_config_commons_spi_enabled = GetLocalProperty(ConfigConstants::CONFIG_VIVO_COMMONS_SPI_ENABLED);
  if (!is_string_empty(str_config_commons_spi_enabled)) {
    GlobalClientSettings::config_commons_spi_enabled =
        !StringEqualsIgnoreCase(StringPiece(ConfigConstants::FALSE), StringPiece(str_config_commons_spi_enabled));
  }

  base_app_config_keys_.push_back(ConfigConstants::APP_NAME);
  base_app_config_keys_.push_back(ConfigConstants::APP_ENV);
  base_app_config_keys_.push_back(ConfigConstants::CONFIG_VERSION);
  base_app_config_keys_.push_back(ConfigConstants::APP_LOC);
  base_app_config_keys_.push_back(ConfigConstants::CONFIG_HOST);
  base_app_config_keys_.push_back(ConfigConstants::CONFIG_AGENT);
  base_app_config_keys_.push_back(ConfigConstants::CONFIG_MONITOR);
  base_app_config_keys_.push_back(ConfigConstants::CONFIG_HTTP_TIMEOUT);

  LoadLocalFileProperties(&local_properties_);
}


bool ConfigLoaderFactory::InitializeConfigLoader(const std::vector<PropertySource> &common_configs,
    std::map<std::string, std::string> *ret) {

  // 加载远程配置中心开关
  bool enable_remote_config = EnableRemoteConfig();

  // 加载客户端全局设置
  LoadGlobalClientSettings();

  // 加载配置中心基础配置
  LoadClientSettings(nullptr);

  // 拉取配置中心的配置
  if (enable_remote_config) {
    LoadConfig(common_configs);
  }
  // 刷新合并远程配置中心的配置
  RefreshRemoteProperties();

  *ret = remote_properties_;

  return true;
}


void ConfigLoaderFactory::RefreshRemoteProperties() {
  // 1.清除原有配置
  remote_properties_.clear();
  remote_property_source_tags_.clear();

  // 2.添加公共配置属性
  for (auto& kv : common_config_loaders_) {
    std::shared_ptr<ConfigLoader> loader = kv.second;
    std::map<std::string, std::string> common;
    loader->ExportProperties(&common);
    PutAll(common, loader);
  }

  // 3.添加业务配置属性
  for (auto& kv : app_config_loaders_) {
    std::shared_ptr<ConfigLoader> loader = kv.second;
    std::map<std::string, std::string> remote;
    loader->ExportProperties(&remote);
    PutAll(remote, loader);
  }

  // refresh
  RefreshProps();
}


void ConfigLoaderFactory::PutAll(std::map<std::string, std::string> map, std::shared_ptr<ConfigLoader> loader) {
  if (map.size() <= 0) return;
  for (auto& kv : map) {
    std::string key = kv.first;
    if(std::find(base_app_config_keys_.begin(), base_app_config_keys_.end(), key) == base_app_config_keys_.end()) {
      std::string value = kv.second;
      remote_properties_[key] = value;

      std::string tag = GenerateAppSourceTag(loader->app_name(), loader->app_env(), loader->config_version());
      remote_property_source_tags_[key] = tag;
    }
  }
}


void ConfigLoaderFactory::RefreshProps() {
  // 将生效的本地/远程配置项全量注入到静态工厂类中
  std::map<std::string, std::string> ctx_props;

  for (auto& kv : local_properties_) {
    std::string key = kv.first;
    std::string value = kv.second;
    // 排除基础变量
    if(std::find(base_app_config_keys_.begin(), base_app_config_keys_.end(), key) == base_app_config_keys_.end()) {
      ctx_props[key] = value;
    }
  }

  for (auto& kv : remote_properties_) {
    std::string key = kv.first;
    std::string value = kv.second;
    ctx_props[key] = value;
  }

  // 全量刷新
  ConfigManagerInternal::Instance()->CleanAndPutAll(ctx_props);
}


void ConfigLoaderFactory::LoadGlobalClientSettings() {
  std::string temp_str;
  GetProperty(ConfigConstants::CONFIG_HTTP_TIMEOUT, &temp_str);

  if (is_string_empty(temp_str)) {
    temp_str = std::to_string(ConfigConstants::DEFAULT_CONFIG_HTTP_TIMEOUT);
  } else if (!is_string_number((temp_str))) {
    CfgLogUtils::LogWarn(StringPrint("ConfigLoaderFactory load [config.http.timeout=%s] failed. It should be a number. use default:%s.",
        temp_str, ConfigConstants::DEFAULT_CONFIG_HTTP_TIMEOUT));
    temp_str = std::to_string(ConfigConstants::DEFAULT_CONFIG_HTTP_TIMEOUT);
  }

  int config_http_timeout;
  StringToNumber(temp_str, &config_http_timeout);

  if (config_http_timeout <= 0) {
    CfgLogUtils::LogWarn(StringPrint("ConfigLoaderFactory load [config.http.timeout=%s] failed. It should be positive. use default:%s.",
        temp_str, ConfigConstants::DEFAULT_CONFIG_HTTP_TIMEOUT));
    config_http_timeout = ConfigConstants::DEFAULT_CONFIG_HTTP_TIMEOUT;
  }
  GlobalClientSettings::config_http_timeout = config_http_timeout;

  // config.http.long.polling.enabled
  temp_str = "";
  GetProperty(ConfigConstants::CONFIG_HTTP_LONG_POLLING_ENABLED, &temp_str);
  if (!is_string_empty(temp_str)) {
    GlobalClientSettings::config_http_long_polling_enabled =
        !StringEqualsIgnoreCase(StringPiece(ConfigConstants::FALSE), StringPiece(temp_str));
  }
}

void ConfigLoaderFactory::LoadClientSettings(const PropertySource *app_config) {
  std::string app_name;
  GetProperty(ConfigConstants::APP_NAME, &app_name);
  std::string app_env;
  GetProperty(ConfigConstants::APP_ENV, &app_env);
  std::string config_version;
  GetProperty(ConfigConstants::CONFIG_VERSION, &config_version);
  std::string app_loc;
  GetProperty(ConfigConstants::APP_LOC, &app_loc);
  std::string config_host;
  GetProperty(ConfigConstants::CONFIG_HOST, &config_host);
  std::string config_agent;
  GetProperty(ConfigConstants::CONFIG_AGENT, &config_agent);
  std::string config_monitor;
  GetProperty(ConfigConstants::CONFIG_MONITOR, &config_monitor);

  // 校验参数
  Check(app_name, app_env, config_version, app_loc, config_host, config_agent, config_monitor);

  // 设置应用配置信息
  if (is_string_empty(app_name)) {
    app_name = ConfigConstants::DEFAULT_APP_NAME;
  }
  client_settings_[ConfigConstants::APP_NAME] = app_name;

  if (is_string_empty(app_env)) {
    app_env = ConfigConstants::DEFAULT_APP_ENV;
  }
  client_settings_[ConfigConstants::APP_ENV] = app_env;

  if (is_string_empty(config_version)) {
    CfgLogUtils::LogInfo(StringPrint("Set default config version %s for %s|%s.", ConfigConstants::DEFAULT_CONFIG_VERSION, app_name, app_env));
    config_version = ConfigConstants::DEFAULT_CONFIG_VERSION;
  }
  client_settings_[ConfigConstants::CONFIG_VERSION] = config_version;

  if (is_string_empty(app_loc)) {
    app_loc = ConfigConstants::DEFAULT_APP_LOC;
  }
  client_settings_[ConfigConstants::APP_LOC] = app_loc;

  if (is_string_empty(config_host)) {
    config_host = ConfigConstants::DEFAULT_CONFIG_HOST;
  }
  client_settings_[ConfigConstants::CONFIG_HOST] = config_host;

  if (is_string_empty(config_agent)) {
    config_agent = ConfigConstants::DEFAULT_CONFIG_AGENT;
  }
  client_settings_[ConfigConstants::CONFIG_AGENT] =  config_agent;

  if (is_string_empty(config_monitor)) {
    config_monitor = ConfigConstants::DEFAULT_CONFIG_MONITOR;
  }
  client_settings_[ConfigConstants::CONFIG_MONITOR] = config_monitor;
}


void ConfigLoaderFactory::Check(const std::string &app_name, const std::string &app_env, const std::string &config_version,
    const std::string &app_loc, const std::string &config_host, const std::string &config_agent, const std::string &config_monitor) {

  if (is_string_empty(app_name)) {
    CfgLogUtils::LogError("ConfigLoaderFactory Cannot Find app.name");
  }
  if (is_string_empty(app_env)) {
    CfgLogUtils::LogError("ConfigLoaderFactory Cannot Find app.env");
  }
  if (is_string_empty(config_version)) {
    CfgLogUtils::LogError("ConfigLoaderFactory Cannot Find config.version");
  }
  if (is_string_empty(app_loc)) {
    CfgLogUtils::LogError("ConfigLoaderFactory Cannot Find app.loc");
  }
  if (is_string_empty(config_host)) {
    CfgLogUtils::LogError("ConfigLoaderFactory Cannot Find config.host");
  }
  if (is_string_empty(config_agent)) {
    CfgLogUtils::LogError("ConfigLoaderFactory Cannot Find config.agent");
  }
  if (is_string_empty(config_monitor)) {
    CfgLogUtils::LogError("ConfigLoaderFactory Cannot Find config.monitor");
  }
}


bool ConfigLoaderFactory::LoadConfig(const std::vector<PropertySource> &common_configs) {
  // 设置配置中心基础配置信息
  PropertySource app_config;
  GetProperty(ConfigConstants::APP_NAME, &app_config.app_name);
  GetProperty(ConfigConstants::APP_ENV, &app_config.app_env);
  GetProperty(ConfigConstants::CONFIG_VERSION, &app_config.config_version);
  GetProperty(ConfigConstants::APP_LOC, &app_config.app_loc);
  GetProperty(ConfigConstants::CONFIG_HOST, &app_config.config_host);
  GetProperty(ConfigConstants::CONFIG_AGENT, &app_config.config_agent);
  GetProperty(ConfigConstants::CONFIG_MONITOR, &app_config.config_monitor);

  // 加载远程配置中心配置
  LoadConfig(app_config, false);

  if (common_configs.size() > 0) {
    std::string log = "";
    log = StringConcat(log, StringPrint("currentApp:%s,%s,%s", app_config.app_name, app_config.app_env, app_config.config_version));

    for (PropertySource common : common_configs) {
      // 覆盖公共配置部分属性
      if (is_string_empty(common.app_name)) {
        common.app_name = ConfigConstants::DEFAULT_APP_NAME;
      }
      common.app_env = app_config.app_env;

      if (is_string_empty(common.config_version)) {
        CfgLogUtils::LogInfo(StringPrint("Set default config version %s for %s/%s.", ConfigConstants::DEFAULT_CONFIG_VERSION, common.app_name, common.app_env));
        common.config_version = ConfigConstants::DEFAULT_CONFIG_VERSION;
      }
      //公共配置获取app.loc参数的优先级,优先取JVM的参数  JVM > xml
      std::string app_loc;
      GetProperty(ConfigConstants::APP_LOC, &app_loc);
      common.app_loc = app_loc;
      if (is_string_empty(common.app_loc)) {
        common.app_loc = ConfigConstants::DEFAULT_APP_LOC;
      }
      common.config_host = app_config.config_host;
      common.config_agent = app_config.config_agent;
      if (is_string_empty(common.config_monitor)) {
        common.config_monitor = ConfigConstants::DEFAULT_CONFIG_MONITOR;
      }
      LoadConfig(common, true);

      log = StringConcat(log, StringPrint(";common:%s,%s,%s", common.app_name, common.app_env, common.config_version));
    }

    ConfigStats::LogInfo(StringPrint("[commonCfg] %s", log));
  }

  return true;
}


bool ConfigLoaderFactory::LoadConfig(PropertySource source, bool is_common) {
  std::string config_host = source.config_host;
  URI uri;
  uri.Parse(config_host);
  std::string scheme = uri.Scheme();

  if (is_string_empty(scheme)) {
    scheme = ConfigConstants::UNKNOWN_CONFIG_HOST_SCHEME;
  }
  std::transform(scheme.begin(), scheme.end(), scheme.begin(), ::tolower);

  if (CfgStringUtils::AreStringsEqual(scheme, "https") || CfgStringUtils::AreStringsEqual(scheme, "http") ) {

    std::shared_ptr<ConfigLoader> hcl = std::make_shared<HttpConfigLoader>(source);
    AddConfigModules(hcl, is_common, source);

  } else if (CfgStringUtils::AreStringsEqual(scheme, "zookeeper")) {
    CfgLogUtils::LogError("'zookeeper://' in config.host is not supported!");
    return false;
  } else {
    std::string url = StringConcat("http://", config_host);
    source.config_host = url;

    std::shared_ptr<ConfigLoader> dcl = std::make_shared<HttpConfigLoader>(source);
    AddConfigModules(dcl, is_common, source);
  }

  return true;
}


void ConfigLoaderFactory::AddConfigModules(std::shared_ptr<ConfigLoader> loader, bool is_common, const PropertySource &source) {
  // 获取应用标识
  std::string key = GenerateAppSourceTag(loader->app_name(), loader->app_env(), loader->config_version());
  // 添加加载器信息
  if (is_common) {
    AddCommonConfigLoader(key, loader);
  } else {
    AddAppConfigLoader(key, loader);
  }
}


void ConfigLoaderFactory::AddAppConfigLoader(const std::string &key, std::shared_ptr<ConfigLoader> loader) {
  if (app_config_loaders_.find(key) == app_config_loaders_.end()) {

    app_config_loaders_[key] = loader;

    // 加载配置
    loader->Start();
  } else {
    CfgLogUtils::LogError(StringPrint("Duplicated add loader: %s", key));
  }
}


void ConfigLoaderFactory::AddCommonConfigLoader(const std::string &key, std::shared_ptr<ConfigLoader> loader) {
  if (common_config_loaders_.find(key) == common_config_loaders_.end()) {
    common_config_loaders_[key] = loader;
    // 加载配置
    loader->Start();
  } else {
    CfgLogUtils::LogError(StringPrint("Duplicated add loader: {}", key));
  }
}


bool ConfigLoaderFactory::GetProperty(const std::string &key, std::string *ret) {
  std::string value = GetLocalProperty(key);
  if (!is_string_empty(value)) {
    CfgLogUtils::LogInfo(StringPrint("ConfigLoaderFactory read local file, key : [%s], value : [%s]", key, value));
  } else {
    value = ConfigConstants::EMPTY_STRING_VALUE;
    CfgLogUtils::LogInfo(StringPrint("ConfigLoaderFactory cannot find key : [%s], assign the default value", key));
  }

  *ret = value;

  return true;
}

std::string ConfigLoaderFactory::GetMethodName(const std::string &key) {
  std::string::size_type index = key.find(".", 0);
  std::string sb = "get";

  std::string sk = key.substr(0, 1);
  std::transform(sk.begin(), sk.end(), sk.begin(), ::toupper);
  sb = StringConcat(sb, sk);

  sb = StringConcat(sb, key.substr(1, index));

  sk = key.substr(index + 1, index + 2);
  std::transform(sk.begin(), sk.end(), sk.begin(), ::toupper);
  sb = StringConcat(sb, sk);

  sb = StringConcat(sb, key.substr(index + 2));
  return sb;
}


std::string ConfigLoaderFactory::GetLocalProperty(const std::string &key) {
  return local_properties_[key];
}


void ConfigLoaderFactory::FillLocalProperties(const std::map<std::string, std::string> &props) {
  for (auto &kv : props) {
    std::string key = kv.first;
    std::string value = kv.second;
    local_properties_[key] = value;
    CfgLogUtils::LogInfo(StringPrint("ConfigLoaderFactory read local properties file, [%s] = [%s]", key, value));
  }
}


bool ConfigLoaderFactory::LoadLocalFileProperties(std::map<std::string, std::string> *ret) {
  std::map<std::string, std::string> props;
  props[ConfigConstants::APP_NAME] = LocalProperties::app_name;
  props[ConfigConstants::APP_ENV] = LocalProperties::app_env;
  props[ConfigConstants::CONFIG_VERSION] = LocalProperties::config_version;
  props[ConfigConstants::CONFIG_HOST] = LocalProperties::config_host;
  props[ConfigConstants::ENABLE_REMOTE_CONFIG] = LocalProperties::enable_remote_config;

  *ret = props;

  return true;
}


std::string ConfigLoaderFactory::GenerateAppSourceTag(const std::string &app_name, const std::string &app_env, const std::string &config_version) {
  assert(!is_string_empty(app_env));
  assert(!is_string_empty(app_env));
  assert(!is_string_empty(config_version));
  return StringPrint("%s_%s_%s", app_name, app_env, config_version);
}


bool ConfigLoaderFactory::EnableRemoteConfig() {
  std::string v1 = GetLocalProperty(ConfigConstants::ENABLE_REMOTE_CONFIG);
  if (!is_string_empty(v1)) {
    return to_bool(v1);
  }
  return ConfigConstants::DEFAULT_ENABLE_REMOTE_CONFIG;
}


std::string ConfigLoaderFactory::app_name() {
  return client_settings_[ConfigConstants::APP_NAME];
}


std::string ConfigLoaderFactory::app_env() {
  return client_settings_[ConfigConstants::APP_ENV];
}


std::string ConfigLoaderFactory::config_version() {
  return client_settings_[ConfigConstants::CONFIG_VERSION];
}


std::string ConfigLoaderFactory::app_loc() {
  return client_settings_[ConfigConstants::APP_LOC];
}


std::string ConfigLoaderFactory::config_host() {
  return client_settings_[ConfigConstants::CONFIG_HOST];

}
std::string ConfigLoaderFactory::config_agent() {
  return client_settings_[ConfigConstants::CONFIG_AGENT];
}


std::string ConfigLoaderFactory::config_monitor() {
  return client_settings_[ConfigConstants::CONFIG_MONITOR];
}


bool is_string_number(const std::string& s) {
  return !s.empty() && std::find_if(s.begin(),
                                    s.end(), [](char c)
                                    { return !std::isdigit(c); }) == s.end();
}


bool is_string_empty(const std::string& s) {
  return s.size() == 0;
}


bool to_bool(const std::string &str) {
  std::string t_str = str;
  std::transform(t_str.begin(), t_str.end(), t_str.begin(), ::tolower);
  std::stringstream is(t_str);
  bool b;
  is >> std::boolalpha >> b;
  return b;
}