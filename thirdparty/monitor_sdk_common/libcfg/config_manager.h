//
// Created by 黄伟锋 on 2019-04-10.
//

#ifndef COMMON_LIBCFG_CONFIG_MANAGER_H_
#define COMMON_LIBCFG_CONFIG_MANAGER_H_

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "thirdparty/monitor_sdk_common/base/singleton.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_listener.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_logger.h"
#include "thirdparty/monitor_sdk_common/libcfg/property_source.h"

namespace common {

// 配置中心配置管理类，提供 sdk 的对外接口。
class ConfigManager : public SingletonBase<ConfigManager> {
 public:
  ConfigManager();
  ~ConfigManager();

  // 初始化配置
  bool Initialize();
  bool Initialize(const std::vector<PropertySource> &common_configs);
  void SetLogger(std::shared_ptr<ConfigLogger> logger);

  // 添加监听器
  bool AddListener(std::shared_ptr<ConfigListener> listener);

  // 移除监听器
  bool RemoveListener(std::shared_ptr<ConfigListener> listener);

  // 设置 app 本地配置
  void SetAppName(const std::string &app_name);
  void SetAppEnv(const std::string &app_env);
  void SetConfigVersion(const std::string &config_version);
  void SetConfigHost(const std::string &config_host);
  bool LoadLocalConfigFile(const std::string &path);
  // 设置开启线程数（建议>=4）[选设：默认为4]
  void SetThreadNum(int thread_pool_num);

  std::map<std::string, std::string> GetProps();
  bool Get(const std::string &key, std::string *ret);
  bool GetString(const std::string &key, std::string *ret);
  bool GetInteger(const std::string &key, int *ret);
  bool GetLong(const std::string &key, long *ret);
  bool GetShort(const std::string &key, short *ret);
  bool GetBoolean(const std::string &key, bool *ret);
  bool GetDouble(const std::string &key, double *ret);
  bool GetFloat(const std::string &key, float *ret);

  std::set<std::string> Keys();
  std::string app_name();
  std::string app_env();
  std::string config_version();
  std::string app_loc();
  std::string config_host();
  std::string config_agent();
  std::string config_monitor();
};

} // namespace common

#endif // LIBCFG_CONFIG_MANAGER_H_
