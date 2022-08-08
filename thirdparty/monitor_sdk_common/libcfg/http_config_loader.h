//
// Created by 黄伟锋 on 2019-04-10.
//

#ifndef COMMON_LIBCFG_HTTP_CONFIG_LOADER_H_
#define COMMON_LIBCFG_HTTP_CONFIG_LOADER_H_

#include <atomic>
#include <memory>
#include <mutex>
#include <string>

#include "thirdparty/monitor_sdk_common/libcfg/config_loader.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_effect_info.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_effect_info_collector.h"
#include "thirdparty/monitor_sdk_common/libcfg/get_config_request.h"
#include "thirdparty/monitor_sdk_common/libcfg/property_source.h"
#include "thirdparty/monitor_sdk_common/libcfg/remote_config_service.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_scheduled_executor.h"


namespace common {

// HTTP配置加载器。
class HttpConfigLoader : public ConfigLoader {
 public:
  HttpConfigLoader();
  HttpConfigLoader(const PropertySource &source);
  ~HttpConfigLoader();

  bool RefreshRemoteProperties();
  void Run();
  bool Close();

 protected:
  bool InitializeConfigLoader();
  bool RemotePropertiesInitialized();

 private:
  // 长轮询
  bool LongPolling(bool *terminated);

  // 检查时间戳是否有更新
  bool CheckLastModifiedChanged(long new_last_modified);

  // 更新配置时间戳
  void UpdateLastModified(long current_last_modified, long request_timestamp, long response_timestamp);

  // 创建请求实体
  GetConfigRequest BuildGetConfigRequest();

  // 判断配置是否相同
  bool Diff(std::map<std::string, std::string> new_configs);

  void PutConfigEffectInfo(const ConfigEffectInfo &config_effect_info);
  std::string GetTimestampKey(std::string app_name, std::string version);

  // Http读取配置超时时间
  int config_http_timeout_;

  // 是否第一次加载配置
  bool is_first_load_properties_ = true;

  // 上次long polling的时间
  long last_long_polling_success_time_ = -1;

  // long polling失败次数
  std::atomic<long> long_polling_failed_count_;

  // 拉取配置失败次数
  std::atomic<long> fetch_failed_count_;

  std::string host_name_;
  std::string ip_;

  // 定时任务线程池(一个线程用于定时拉取，另一个线程用于long polling)
  std::shared_ptr<ConfigScheduledExecutor> scheduled_executor_;
  std::shared_ptr<ConfigEffectInfoCollector> config_effect_info_collector_;
  std::shared_ptr<RemoteConfigService> config_service_;

  // 读写锁
  std::recursive_mutex refresh_remote_properties_mutex_;
};

} // namespace common

#endif // LIBCFG_HTTP_CONFIG_LOADER_H_
