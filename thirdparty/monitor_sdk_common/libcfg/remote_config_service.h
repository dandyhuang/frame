//
// Created by 黄伟锋 on 2019-04-15.
//

#ifndef COMMON_LIBCFG_SERVICE_REMOTE_CONFIG_SERVICE_H_
#define COMMON_LIBCFG_SERVICE_REMOTE_CONFIG_SERVICE_H_

#include <string>
#include <vector>

#include "thirdparty/monitor_sdk_common/libcfg/common_vo.h"
#include "thirdparty/monitor_sdk_common/libcfg/get_config_request.h"
#include "thirdparty/monitor_sdk_common/libcfg/get_config_result.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_change_message.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_effect_info.h"
#include "thirdparty/monitor_sdk_common/net/http/client.h"
#include "thirdparty/monitor_sdk_common/net/uri/uri.h"
#include "thirdparty/monitor_sdk_common/system/threading/thread_pool.h"

namespace common {

class RemoteConfigService {
 public:
  RemoteConfigService();
  ~RemoteConfigService();

  bool GetConfig(const GetConfigRequest &get_config_request, int http_timeout, CommonVO<GetConfigResult> *ret);

  bool LongPolling(
      const GetConfigRequest &get_config_request,
      CommonVO<ConfigChangeMessage> *ret,
      bool *terminated);

  bool UploadConfigEffectInfo(const std::string &config_host,
      const std::vector<ConfigEffectInfo> &config_effect_infos);

 private:
  HttpClient::Options GetCommonHttpOptions();

  std::string BuildGetConfigUrl(const GetConfigRequest &get_config_request, const std::string &method_name);

  std::string GetDeocdedValue(std::string &kms_value, std::string &key1, std::string &key2);
};

} // namespace common

#endif //COMMON_LIBCFG_SERVICE_REMOTE_CONFIG_SERVICE_H_
