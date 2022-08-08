//
// Created by 黄伟锋 on 2019-04-16.
//

#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_effect_info_collector.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_stats.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_utils.h"
#include "thirdparty/monitor_sdk_common/libcfg/utils.h"

using namespace common;


ConfigEffectInfoCollector::ConfigEffectInfoCollector(std::shared_ptr<ConfigScheduledExecutor> scheduled_executor,
    std::shared_ptr<RemoteConfigService> config_service,
    const std::string &config_host) {

  scheduled_executor_ = scheduled_executor;
  config_service_ = config_service;
  config_host_ = config_host;
  config_effect_info_queue_ = std::unique_ptr<BlockQueue<ConfigEffectInfo> >(new BlockQueue<ConfigEffectInfo>(100));
}


ConfigEffectInfoCollector::~ConfigEffectInfoCollector() {}


void ConfigEffectInfoCollector::Start() {
  scheduled_executor_->ScheduleWithFixedDelay([this] (bool *terminated) {

    if ((*terminated)) return;

    while (true) {
      std::vector<ConfigEffectInfo> infos = config_effect_info_queue_->Drain(20);

      if (infos.size() <= 0) {
        return;
      }

      std::string log_msg = StringPrint("upload effect: %s", ConfigEffectInfo::SerializeList(infos));
      ConfigStats::LogInfo(log_msg);
      CfgLogUtils::LogDebug(log_msg);

      try {
          config_service_->UploadConfigEffectInfo(config_host_, infos);
      } catch (...) {
          CfgLogUtils::LogWarn("Post config failed.");
      }
    }

  }, 60 * 1000, 60 * 1000);

  started_ = true;
}


void ConfigEffectInfoCollector::Put(const ConfigEffectInfo &config_effect_info) {
  if (config_effect_info.effect_timestamp > 0 && config_effect_info.config_last_modified > 0) {
    ConfigStats::LogInfo(StringPrint("%ld", config_effect_info.effect_timestamp - config_effect_info.config_last_modified));
  }

  if (started_ && config_effect_info_queue_->RemainingCapacity() > 0) {
    config_effect_info_queue_->Push(config_effect_info);
  } else {
    CfgLogUtils::LogDebug("Cannot put config_effect_info, no remainingCapacity.");
  }
}
