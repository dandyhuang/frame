//
// Created by 黄伟锋 on 2019-04-23.
//

#ifndef COMMON_LIBRMQ_RMQ_REMOTE_SERVICE_H_
#define COMMON_LIBRMQ_RMQ_REMOTE_SERVICE_H_

#include <string>

#include "thirdparty/monitor_sdk_common/base/singleton.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_auth_request.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_connection_config.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_metric.h"
#include "thirdparty/monitor_sdk_common/net/http/client.h"
#include "thirdparty/monitor_sdk_common/net/uri/uri.h"

namespace common {

class RmqRemoteService {

 public:
  RmqRemoteService(const std::map<std::string, std::string> &topic_key_secret_key_map);
  ~RmqRemoteService();

  bool PostAuth(RmqAuthRequest auth_request, const std::string &server_host, RmqConnectionConfig *ret);

  bool PostHeartbeat(RmqMetric metric, const std::string &server_host, std::vector<RmqConnectionConfig> *ret);

 protected:
  RmqRemoteService() {}

 private:
  HttpClient::Options GetCommonHttpOptions();

  bool LoadLocalConnectionConfig(const std::string &topic_key, RmqConnectionConfig *ret);

  std::map<std::string, std::string> topic_key_secret_key_map_;

};

} // namespace common

#endif //COMMON_LIBRMQ_RMQ_REMOTE_SERVICE_H_
