//
// Created by 黄伟锋 on 2019-04-16.
//

#ifndef COMMON_LIBCFG_CONFIG_EFFECT_INFO_COLLECTOR_H_
#define COMMON_LIBCFG_CONFIG_EFFECT_INFO_COLLECTOR_H_

#include <string>

#include "thirdparty/monitor_sdk_common/libcfg/block_queue.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_effect_info.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_scheduled_executor.h"
#include "thirdparty/monitor_sdk_common/libcfg/remote_config_service.h"


namespace common {

class ConfigEffectInfoCollector {
 public:
  ConfigEffectInfoCollector(std::shared_ptr<ConfigScheduledExecutor> scheduled_executor,
      std::shared_ptr<RemoteConfigService> config_service,
      const std::string &config_host);
  ~ConfigEffectInfoCollector();

  void Start();
  void Put(const ConfigEffectInfo &config_effect_info);

 private:
  std::shared_ptr<ConfigScheduledExecutor> scheduled_executor_;
  std::shared_ptr<RemoteConfigService> config_service_;
  std::string config_host_;
  bool started_ = false;

  std::unique_ptr<BlockQueue<ConfigEffectInfo> > config_effect_info_queue_;
};

} // namespace common

#endif //COMMON_LIBCFG_CONFIG_EFFECT_INFO_COLLECTOR_H_
