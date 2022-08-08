// @copyright vivo
// author: yuchengzhen
#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include "servant/Application.h"
#include "servant/Communicator.h"
#include "thirdparty/rapidjson/document.h"
#include "thirdparty/monitor_sdk_common/rpc_proxy/pb_loader.h"

struct RpcLibConf{
  std::string locator_host;
  std::string locator_port;
  std::string servant_obj;  // obj
  std::string ipport;  // 可选，可指定ipport
  std::vector<std::string> base_path;  // 查找proto的路径,为空时下游会加上当前路径
  std::string instance_proto;  // 相对路径
};

class RpcLib {
 public:
  RpcLib();
  ~RpcLib();

  int32_t Init(const RpcLibConf &conf);

  // 发送RPC请求，异常时
  // 返回：0 正常，-1 RPC异常，-2 PB异常
  int32_t Req(const std::string &req_msg, const std::string &req_data, const std::string &rpc_name,
              const std::string &resp_msg,  std::string *outjson, int32_t timeout_ms = 10000) const;

 private:
  PbLoader pb_loader_;
  tars::Communicator comm_;
  typedef tars::TC_AutoPtr<ServantProxy> ServantProxyPtr;
  ServantProxyPtr prx_;
};
