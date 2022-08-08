//
// Created by 黄伟锋 on 2019-04-13.
//

#ifndef COMMON_LIBCFG_CONFIG_MANAGER_INTERVAL_H_
#define COMMON_LIBCFG_CONFIG_MANAGER_INTERNAL_H_

#include <memory>
#include <string>

#include "thirdparty/monitor_sdk_common/base/singleton.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_listener.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_manager.h"


namespace common {

// 配置中心配置管理类
class ConfigManagerInternal : public SingletonBase<ConfigManagerInternal>  {
 public:
  ConfigManagerInternal();
  ~ConfigManagerInternal();

  // 添加监听器
  bool AddListener(std::shared_ptr<ConfigListener> listener);

  // 移除监听器
  bool RemoveListener(std::shared_ptr<ConfigListener> listener);

  void CleanAndPutAll(std::map<std::string, std::string> configs);

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


 private:
  void Clear();
  void Remove(std::string key, std::string value);
  void Put(std::string key, std::string value);


  // 配置项集合, 包括本地配置和远程配置中心配置
  std::map<std::string, std::string> props_;

  //监听器集合
  std::vector<std::shared_ptr<ConfigListener>> listeners_;
};

} // namespace common

#endif //COMMON_LIBCFG_CONFIG_MANAGER_INTERVAL_H_
