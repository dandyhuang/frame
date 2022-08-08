//
// Created by 黄伟锋 on 2019-04-10.
//

#ifndef COMMON_LIBCFG_CONFIG_LOADER_H_
#define COMMON_LIBCFG_CONFIG_LOADER_H_

#include <string>
#include <map>
#include <mutex>

#include "thirdparty/monitor_sdk_common/libcfg/config_core_processor.h"
#include "thirdparty/monitor_sdk_common/libcfg/property_source.h"

namespace common {

// 配置加载器抽象类。
class ConfigLoader : public ConfigCoreProcessor {
 public:
  ConfigLoader();
  ConfigLoader(const PropertySource &source);
  virtual ~ConfigLoader();

  virtual bool Start();

  // 导出远程配置
  bool ExportProperties(std::map<std::string, std::string> *ret);

  std::string Get(const std::string &key);
  int Put(const std::string &key, const std::string &value);
  int ClearAndPutAll(const std::map<std::string, std::string> &new_configs);

  // 处理配置变更事件
  bool HandleChangeEvent();

 protected:
  // 初始化
  virtual bool InitializeConfigLoader();

  // 初次加载远程配置之后的操作
  virtual bool RemotePropertiesInitialized();

  // 调用远程接口刷新远程配置
  // 如果刷新成功则返回true，否则返回false。
  virtual bool RefreshRemoteProperties(); // 注意原子操作

  long GetLastLoadTime();
  long GetLastModified();
  void SetLastModified(long last_modified);

 private:
  // 缓存配置到本地文件
  bool SaveLocalCache();
  // 加载本地的缓存配置文件
  bool LoadLocalCache();


  // 远程配置中心配置集合
  std::map<std::string, std::string> remote_properties_;

  // 配置的最后加载时间
  long last_load_time_;

  // 配置最后修改时间戳
  long last_modified_ = -1;

  // 读写锁
  std::mutex last_modified_mutex_;
};

} // namespace common

#endif // LIBCFG_CONFIG_LOADER_H_
