//
// Created by 黄伟锋 on 2019-04-10.
//

#ifndef COMMON_LIBCFG_CONFIG_LOADER_FACTORY_H_
#define COMMON_LIBCFG_CONFIG_LOADER_FACTORY_H_

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "thirdparty/monitor_sdk_common/base/singleton.h"
#include "thirdparty/monitor_sdk_common/libcfg/property_source.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_loader.h"
#include "thirdparty/monitor_sdk_common/storage/file/file.h"

namespace common {

// 加载配置工厂类。
class ConfigLoaderFactory : public SingletonBase<ConfigLoaderFactory> {
 public:
  ConfigLoaderFactory();
  ~ConfigLoaderFactory();

  bool InitializeConfigLoader(const std::vector<PropertySource> &common_configs, std::map<std::string, std::string> *ret);
  void RefreshRemoteProperties();

  std::string app_name();
  std::string app_env();
  std::string config_version();
  std::string app_loc();
  std::string config_host();
  std::string config_agent();
  std::string config_monitor();


 private:

  void Init();

  // 合并配置
  void PutAll(std::map<std::string, std::string> map, std::shared_ptr<ConfigLoader> loader);

  // 刷新全量配置
  void RefreshProps();

  // 加载客户端全局设置
  void LoadGlobalClientSettings();

  // 加载配置中心基础配置
  void LoadClientSettings(const PropertySource *appConfig);

  // 检查配置中心应用配置属性
  void Check(const std::string &app_name, const std::string &app_env, const std::string &config_version,
      const std::string &app_loc, const std::string &config_host, const std::string &config_agent,
      const std::string &config_monitor);

  // 加载远程配置
  bool LoadConfig(const std::vector<PropertySource> &common_configs);
  bool LoadConfig(PropertySource source, bool is_common);
  void AddConfigModules(std::shared_ptr<ConfigLoader> loader, bool is_common, const PropertySource &source);
  void AddAppConfigLoader(const std::string &key, std::shared_ptr<ConfigLoader> loader);
  void AddCommonConfigLoader(const std::string &key, std::shared_ptr<ConfigLoader> loader);

  // 获取配置中心应用配置属性
  bool GetProperty(const std::string &key, std::string *ret);

  // 获取配置中心资源配置属性
  std::string GetMethodName(const std::string &key);

  // 获取本地配置属性
  std::string GetLocalProperty(const std::string &key);

  // 填充本地properties文件的配置
  void FillLocalProperties(const std::map<std::string, std::string> &props);

  // 加载本地配置文件
  bool LoadLocalFileProperties(std::map<std::string, std::string> *ret);

  // 加载本地文件资源
  File* DoLoadLocalFileResource();

  // 生成应用来源标识
  std::string GenerateAppSourceTag(const std::string &app_name, const std::string &app_env, const std::string &config_version);

  // 是否开启远程配置中心
  bool EnableRemoteConfig();


  // 配置中心基础配置key集合
  std::vector<std::string> base_app_config_keys_;

  // 配置中心基础配置集合
  std::map<std::string, std::string> client_settings_;

  // 远程配置中心应用配置加载器集合
  // key: {appName}_{appEnv}_{configVersion}
  // value: 应用配置加载器
  std::map<std::string, std::shared_ptr<ConfigLoader>> app_config_loaders_;

  // 远程配置中心公共配置加载器集合
  // key: {appName}_{appEnv}_{configVersion}
  // value: 公共配置加载器
  std::map<std::string, std::shared_ptr<ConfigLoader>> common_config_loaders_;

  // 远程配置中心配置
  std::map<std::string, std::string> remote_properties_;

  // 远程配置中心配置来源标记
  std::map<std::string, std::string> remote_property_source_tags_;

  // 本地配置
  std::map<std::string, std::string> local_properties_;
};

} // namespace common

#endif // LIBCFG_CONFIG_LOADER_FACTORY_H_
