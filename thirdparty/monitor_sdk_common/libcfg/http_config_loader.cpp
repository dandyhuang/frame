//
// Created by 黄伟锋 on 2019-04-10.
//
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_constants.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_effect_info.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_stats.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_utils.h"
#include "thirdparty/monitor_sdk_common/libcfg/get_config_request.h"
#include "thirdparty/monitor_sdk_common/libcfg/get_config_result.h"
#include "thirdparty/monitor_sdk_common/libcfg/global_client_settings.h"
#include "thirdparty/monitor_sdk_common/libcfg/http_config_loader.h"
#include "thirdparty/monitor_sdk_common/libcfg/remote_config_service.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_scheduled_executor.h"
#include "thirdparty/monitor_sdk_common/libcfg/utils.h"
#include "thirdparty/monitor_sdk_common/libcfg/local_properties.h"
#include "thirdparty/monitor_sdk_common/system/threading/this_thread.h"

#include "thirdparty/jsoncpp/json.h"

void double_log_info(const std::string &log_msg);
void get_local_ip(std::string *ret);
void get_host_name(std::string *ret);

using namespace common;


// 长轮询连续出错过多时长休眠阈值（单位：ms）
const int LONG_POLLING_SLEEP_THRESHOLD = 10000;

// 长轮询连续出错过多时长休眠时长（单位：ms）
const int LONG_POLLING_SLEEP_MILLISECOND = 30 * 1000;

// 长轮询连续失败次数阈值（超出阈值才将错误信息打印到业务端日志）
const int LONG_POLLING_FAILED_COUNT_THRESHOLD = 10;

// 定时轮询连续失败次数阈值（超出阈值才将错误信息打印到业务端日志）
const int FETCH_FAILED_COUNT_THRESHOLD = 6;

#include <iostream>

HttpConfigLoader::HttpConfigLoader() {
  long_polling_failed_count_ = 0;
  fetch_failed_count_ = 0;
  get_host_name(&host_name_);
  get_local_ip(&ip_);
}


HttpConfigLoader::HttpConfigLoader(const PropertySource &source) : ConfigLoader(source) {
  long_polling_failed_count_ = 0;
  fetch_failed_count_ = 0;
  get_host_name(&host_name_);
  get_local_ip(&ip_);
}


HttpConfigLoader::~HttpConfigLoader() {}


bool HttpConfigLoader::InitializeConfigLoader() {
  std::string display_name = ConfigCoreProcessor::display_name();
  display_name = display_name.size() == 0 ? "default" : display_name;

  scheduled_executor_ = std::make_shared<ConfigScheduledExecutor>(LocalProperties::thread_pool_num);
  config_service_ = std::make_shared<RemoteConfigService>();

  config_http_timeout_ = GlobalClientSettings::config_http_timeout;

  return true;
}


bool HttpConfigLoader::RemotePropertiesInitialized() {

  // 初始化生效时长搜集器
  config_effect_info_collector_ = std::make_shared<ConfigEffectInfoCollector>(scheduled_executor_, config_service_, ConfigCoreProcessor::config_host());
  config_effect_info_collector_->Start();

  // 开始定时拉取
  scheduled_executor_->ScheduleWithFixedDelay([this] (bool *terminated) {

    if ((*terminated)) return;

    CfgLogUtils::LogDebug(StringPrint("[%s] scheduled refresh", ConfigCoreProcessor::display_name()));

    if (RefreshRemoteProperties()) {

      long failed_count = fetch_failed_count_;
      if (failed_count != 0) {
        fetch_failed_count_ = 0;
        CfgLogUtils::LogInfo(StringPrint("Recovery after %ld failures", failed_count));
      }

    } else {

      long failed_count = ++fetch_failed_count_;
      ConfigStats::LogError(StringPrint("[%s] refreshRemoteProperties error", ConfigCoreProcessor::display_name()));

      // 失败超过6次输出错误日志到业务端
      if (failed_count > FETCH_FAILED_COUNT_THRESHOLD) {
        CfgLogUtils::LogError("reach the limit of fetch failed count threshold");
        fetch_failed_count_ = 0;
      }
    }

    CfgLogUtils::LogInfo("定时拉取 round finish");

  }, 5000, GlobalClientSettings::scheduled_fetch_interval());

  CfgLogUtils::LogInfo(StringPrint("[%s] Scheduled fetching config started", ConfigCoreProcessor::display_name()));

  // 开始 long polling
  if (GlobalClientSettings::config_http_long_polling_enabled) {
    last_long_polling_success_time_ = CfgTimeUtils::GetCurrentTimeMillis();

    scheduled_executor_->Execute([this] (bool *terminated) {

      CfgLogUtils::LogInfo(StringPrint("[%s] Http long polling task started", ConfigCoreProcessor::display_name()));

      // 初始睡眠 2000 ms
      int delay_count = 2 * 1000;
      while (delay_count > 0 && !(*terminated)) {
        ThisThread::Sleep(1);
        delay_count --;
      }

      if (*terminated) return;

      long start_time;
      while (GlobalClientSettings::config_http_long_polling_enabled && !(*terminated)) {
        start_time = CfgTimeUtils::GetCurrentTimeMillis();
        CfgLogUtils::LogDebug(StringPrint("[%s] Http long polling start!",
            ConfigCoreProcessor::display_name()));
        if (LongPolling(terminated)) {
          CfgLogUtils::LogDebug(StringPrint("[%s] Http long polling end! takes %ld ms.",
              ConfigCoreProcessor::display_name(), (CfgTimeUtils::GetCurrentTimeMillis() - start_time)));
        } else {
          CfgLogUtils::LogDebug(StringPrint("[%s] Http long polling end! failed !",
              ConfigCoreProcessor::display_name()));
        }

        if ((*terminated)) break;

        if (GlobalClientSettings::http_long_polling_paused) {
          // 暂停长轮询，进入长时休眠
          ConfigStats::LogInfo(StringPrint("[%s]Stop polling and sleep.", ConfigCoreProcessor::display_name()));
          ThisThread::Sleep(GlobalClientSettings::http_long_polling_pause_milliseconds);
        }
      }

      CfgLogUtils::LogInfo("long polling loop terminate.");

    });

  }

  return true;
}


bool HttpConfigLoader::RefreshRemoteProperties() {
  if (scheduled_executor_->IsTerminated()) return false;

  CfgLogUtils::LogInfo("refresh remote properties");

  std::unique_lock<std::recursive_mutex> lk(refresh_remote_properties_mutex_);

  bool is_first_load = false;
  if (is_first_load_properties_) {
    is_first_load_properties_ = false;
    is_first_load = true;
  }

  // 初次请求使用3倍http超时时间
  int http_timeout = is_first_load ? config_http_timeout_ * 3 : config_http_timeout_;
  GetConfigRequest request = BuildGetConfigRequest();

  CommonVO<GetConfigResult> result_vo;

    bool ret = false;
    try {
      ret = config_service_->GetConfig(request, http_timeout, &result_vo);
    } catch(...){
        CfgLogUtils::LogWarn("Get config failed.");
    }

    if (ret) {

    if (result_vo.retcode == 0) {

      GetConfigResult get_config_response = result_vo.data;

      // 检查服务端返回的时间戳是否有更新
      if (CheckLastModifiedChanged(get_config_response.last_modified)) {

        std::map<std::string, std::string> new_properties;
        for (PropertyItem item : get_config_response.configs) {
          new_properties[item.name] = item.value;
        }

        // 检查配置是否变化
        bool is_change = Diff(new_properties);

        // 如果发生变化，则更新远程配置
        if (is_change) {
          ConfigLoader::ClearAndPutAll(new_properties);

          // 更新客户端时间戳
          UpdateLastModified(get_config_response.last_modified, get_config_response.request_timestamp, get_config_response.response_timestamp);

          // 触发配置变更事件
          ConfigLoader::HandleChangeEvent();

        } else {
          CfgLogUtils::LogDebug(StringPrint("[%s] remote properties not changed", ConfigCoreProcessor::display_name()));
        }

        return is_change;

      } else {

        CfgLogUtils::LogDebug(StringPrint("[%s] lastModifed not changed.", ConfigCoreProcessor::display_name()));

        return false;
      }
      
    } else if (result_vo.retcode == ConfigConstants::CFG_AGENT_RETCODE_NOT_MODIFIED) {

      CfgLogUtils::LogDebug(StringPrint("[%s] vivocfg-agent returned %d, %s",
          ConfigCoreProcessor::display_name(), result_vo.retcode, result_vo.message));

      return true;

    } else if (result_vo.retcode == ConfigConstants::CFG_AGENT_RETCODE_ERR_CODE_CONFIG_VERSION_CHANGED) {

      CfgLogUtils::LogDebug(StringPrint("[%s] vivocfg-agent returned %d, %s",
          ConfigCoreProcessor::display_name(), result_vo.retcode, result_vo.message));

      GetConfigResult get_config_response = result_vo.data;
      std::string current_verison = ConfigCoreProcessor::config_version();

      if (CfgStringUtils::IsStringNotblank(get_config_response.real_config_version) && !CfgStringUtils::AreStringsEqual(current_verison, get_config_response.real_config_version)) {

        // 更新版本号
        double_log_info(StringPrint("Real config version changed. Original:{%s}. Version change: %s -> %s",
                                    ConfigCoreProcessor::display_name(), current_verison, get_config_response.real_config_version));

        ConfigCoreProcessor::set_config_version(get_config_response.real_config_version);

        // 重置时间戳
        ConfigLoader::SetLastModified(-1);

        double_log_info(StringPrint("After real config version changed. Current:%s. LastModified reset to -1.", ConfigCoreProcessor::display_name()));
        // 重新拉取配置
        double_log_info(StringPrint("After real config version changed. Current:%s. Begin refresh remote properties.", ConfigCoreProcessor::display_name()));

        return RefreshRemoteProperties();

      } else {
        ConfigStats::LogError(StringPrint("Invalid config version change! currentConfigVersion=%s, realConfigVersion=%s", current_verison, get_config_response.real_config_version));
      }

      return false;

    } else if (result_vo.retcode == ConfigConstants::CFG_AGENT_RETCODE_BAD_TIMESTAMP) {

      CfgLogUtils::LogWarn(StringPrint("[%s] vivocfg-agent returned %d, %s. Request:%s", ConfigCoreProcessor::display_name(), result_vo.retcode, result_vo.message, request.ToString()));

      ConfigStats::LogError(StringPrint("[%s] vivocfg-agent returned %d, %s. Request:%s", ConfigCoreProcessor::display_name(), result_vo.retcode, result_vo.message, request.ToString()));

      return false;
    }
  }

  return true;
}


void HttpConfigLoader::Run() {
  if (!RefreshRemoteProperties()) {
    CfgLogUtils::LogError("fail to refresh remote properties.");
  }
}


bool HttpConfigLoader::Close() {
  if (scheduled_executor_ != nullptr) {
    scheduled_executor_->Shutdown();
  }
  return true;
}


bool HttpConfigLoader::LongPolling(bool *terminated) {

  bool is_success = true;
  std::string error_message = "";

  long last_modified = ConfigLoader::GetLastModified();
  if (last_modified < 0) {
    is_success = false;
    error_message = "longPolling cancelled";

  } else {

    GetConfigRequest request = BuildGetConfigRequest();
    ConfigStats::LogInfo(StringPrint("[longPollingAppInfo] %s", ConfigCoreProcessor::display_name()));
    CommonVO<ConfigChangeMessage> result_vo;

    bool ret = false;
    try {
        ret = config_service_->LongPolling(request, &result_vo, terminated);
    } catch(...) {
        CfgLogUtils::LogWarn("LongPolling failed.");
    }

    if (!ret) return false;

    if (result_vo.retcode == ConfigConstants::CFG_AGENT_RETCODE_NOT_MODIFIED) {
      CfgLogUtils::LogDebug(StringPrint("[%s] vivocfg-agent returned %d, %s",
          ConfigCoreProcessor::display_name(), result_vo.retcode, result_vo.message));
    } else if (result_vo.retcode == ConfigConstants::CFG_AGENT_RETCODE_BAD_TIMESTAMP) {
      CfgLogUtils::LogWarn(StringPrint("[%s] vivocfg-agent returned %d, %s. Request:%s",
                                  ConfigCoreProcessor::display_name(), result_vo.retcode, result_vo.message, request.ToString()));
    } else if (result_vo.retcode == ConfigConstants::CFG_AGENT_RETCODE_TOO_MANY_LONG_POLLING_CONNECTION) {
      CfgLogUtils::LogWarn(StringPrint("[%s] [too many long polling connection] vivocfg-agent returned %d, %s. Request:%s",
                                  ConfigCoreProcessor::display_name(), result_vo.retcode, result_vo.message, request.ToString()));
      GlobalClientSettings::http_long_polling_paused = true;
    } else if (result_vo.retcode == 0) {
      ConfigChangeMessage config_change = result_vo.data;
      // 检查服务端返回的时间戳是否有更新
      if (CheckLastModifiedChanged(config_change.last_modified)) {
        // 刷新远程配置
        CfgLogUtils::LogInfo(StringPrint("[%s] longPolling find lastModified changed, and fire refreshRemoteProperties", ConfigCoreProcessor::display_name()));
        if (!RefreshRemoteProperties()) {
          CfgLogUtils::LogError(StringPrint("[%s] refreshRemoteProperties failed.", ConfigCoreProcessor::display_name()));
        }
      } else {
        CfgLogUtils::LogDebug("lastModifed not changed.");
      }
    } else {
      is_success = false;
    }

  }

  // finally

  // 如果小于LONG_POLLING_MIN_INTERVAL，则进行休眠，避免请求发起频率过高
  long polling_interval = CfgTimeUtils::GetCurrentTimeMillis() - last_long_polling_success_time_;
  ConfigStats::LogInfo(StringPrint("[%s] [pollingInterval] {%ld}", ConfigCoreProcessor::display_name(), polling_interval));

  if (long_polling_failed_count_ > 3 && polling_interval < LONG_POLLING_SLEEP_THRESHOLD) {
    if (long_polling_failed_count_ < 10) {

      CfgLogUtils::LogDebug(StringPrint("http long polling api fail too fast (%ld), sleep %ld ms", polling_interval, LONG_POLLING_SLEEP_MILLISECOND));

      ThisThread::Sleep(LONG_POLLING_SLEEP_MILLISECOND);

    } else {

      CfgLogUtils::LogDebug(StringPrint("http long polling api fail too fast (%ld), sleep %ld ms", polling_interval, LONG_POLLING_SLEEP_MILLISECOND * 10));

      ThisThread::Sleep(LONG_POLLING_SLEEP_MILLISECOND * 10);
    }
  }

  if (is_success) {
    if (long_polling_failed_count_ != 0) {
      long_polling_failed_count_ = 0;
    }
  } else {
    long failed_count = ++long_polling_failed_count_;

    std::string fail_msg = StringPrint("[%s] [HttpConfigLoader longPolling] failed (%ld), error: %s", ConfigCoreProcessor::display_name(), failed_count, error_message);
    ConfigStats::LogError(fail_msg);

    // 失败超过10次输出错误日志到业务端
    if (failed_count > LONG_POLLING_FAILED_COUNT_THRESHOLD) {
      CfgLogUtils::LogInfo(fail_msg);
    }
  }

  last_long_polling_success_time_ = CfgTimeUtils::GetCurrentTimeMillis();

  return is_success;
}


bool HttpConfigLoader::CheckLastModifiedChanged(long new_last_modified) {
  bool is_changed = false;

  long last_modified = ConfigLoader::GetLastModified();
  if (last_modified < new_last_modified) {
    is_changed = true;

    CfgLogUtils::LogDebug(StringPrint("lastModified changed. %s, %ld -> %ld",
        GetTimestampKey(ConfigCoreProcessor::app_name(), ConfigCoreProcessor::config_version()),
        last_modified, new_last_modified));
  }

  if (!is_changed) {
    CfgLogUtils::LogDebug("timestamp not changed");
  }

  return is_changed;
}


void HttpConfigLoader::UpdateLastModified(long current_last_modified, long request_timestamp, long response_timestamp) {
// 如果lastModifed为-1，则可能是第一次启动或者重置时间戳后的
  long last_modified = ConfigLoader::GetLastModified();
  bool is_not_first = last_modified > -1;

  if (last_modified != current_last_modified) {

    // 更新当前应用时间戳
    ConfigLoader::SetLastModified(current_last_modified);

    if (is_not_first) {
      ConfigEffectInfo config_effect_Info;
      config_effect_Info.app_env = ConfigCoreProcessor::app_env();
      config_effect_Info.app_loc = ConfigCoreProcessor::app_loc();
      config_effect_Info.effect_timestamp = CfgTimeUtils::GetCurrentTimeMillis();
      config_effect_Info.request_timestamp = request_timestamp;
      config_effect_Info.response_timestamp = response_timestamp;
      config_effect_Info.app_name = ConfigCoreProcessor::app_name();
      config_effect_Info.config_version = ConfigCoreProcessor::config_version();
      config_effect_Info.config_last_modified = current_last_modified;
      config_effect_Info.host_name = host_name_;
      // 上报生效时间信息
      PutConfigEffectInfo(config_effect_Info);
    }
  }
}


GetConfigRequest HttpConfigLoader::BuildGetConfigRequest() {
  GetConfigRequest request;
  request.app_name = ConfigCoreProcessor::app_name();
  request.app_env = ConfigCoreProcessor::app_env();
  request.config_version = ConfigCoreProcessor::config_version();
  request.client_default_config_version = ConfigCoreProcessor::client_default_config_version();
  request.config_host = ConfigCoreProcessor::config_host();
  request.last_modified = ConfigLoader::GetLastModified();
  request.host_name = host_name_;
  request.ip = ip_;
  request.app_loc = ConfigCoreProcessor::app_loc();
  return request;
}


bool HttpConfigLoader::Diff(std::map<std::string, std::string> new_configs) {
  std::map<std::string, std::string> configs;
  ConfigLoader::ExportProperties(&configs);
  return !CfgMapUtils::AreMapsEqual(configs, new_configs);
}


void HttpConfigLoader::PutConfigEffectInfo(const ConfigEffectInfo &config_effect_info) {
  if (config_effect_info_collector_) {
    config_effect_info_collector_->Put(config_effect_info);
  }
}


std::string HttpConfigLoader::GetTimestampKey(std::string app_name, std::string version) {
  return StringPrint("%s,%s", app_name, version);
}


void double_log_info(const std::string &log_msg) {
  CfgLogUtils::LogInfo(log_msg);
  ConfigStats::LogInfo(log_msg);
}


void get_local_ip(std::string *ret) {
  std::string local_ip;

  struct ifaddrs * ifAddrStruct=NULL;
  struct ifaddrs * ifa=NULL;
  void * tmpAddrPtr=NULL;

  getifaddrs(&ifAddrStruct);
  for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
    if (!ifa->ifa_addr) {
      continue;
    }
    if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
      // is a valid IP4 Address
      tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
      char addressBuffer[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

      if (CfgStringUtils::AreStringsEqual(std::string(ifa->ifa_name), "eth0")) {
        local_ip = std::string(addressBuffer);
        break;
      }

    } else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
      // is a valid IP6 Address
      tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
      char addressBuffer[INET6_ADDRSTRLEN];
      inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);

      if (CfgStringUtils::AreStringsEqual(std::string(ifa->ifa_name), "eth0")) {
        local_ip = std::string(addressBuffer);
        break;
      }
    }
  }
  if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);

  *ret = local_ip;
}


void get_host_name(std::string *ret) {
  char hostname[1024];
  gethostname(hostname, 1024);
  *ret = std::string(hostname);
}