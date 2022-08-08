// @copyright vivo
// author: yuchengzhen
#pragma once

#include "thirdparty/monitor_sdk_common/rpc_proxy/proxy.tars.h"
#include "servant/Application.h"

class RpcLib;

class ProxyImp : public vivo::commonproxy::ProxyService {
 public:
  virtual ~ProxyImp() {}

  virtual void initialize();

  virtual void destroy();

  vivo::commonproxy::RespData getResp(const vivo::commonproxy::NormalReq &,
                                     tars::TarsCurrentPtr current) override;

  vivo::commonproxy::MsgDefault getDefalut(const vivo::commonproxy::MsgInfo &msg_info,
                                     tars::TarsCurrentPtr current) override;

 private:
  int InitProxy(const vivo::commonproxy::CommonInfo &, RpcLib *);
};

