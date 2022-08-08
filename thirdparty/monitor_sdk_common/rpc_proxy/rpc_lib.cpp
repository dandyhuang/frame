// @copyright vivo
// author: yuchengzhen

#include "thirdparty/monitor_sdk_common/rpc_proxy/rpc_lib.h"

#include <map>

#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"

using std::string;
using std::vector;
using std::map;

RpcLib::RpcLib() {}

RpcLib::~RpcLib() {}

int32_t RpcLib::Init(const RpcLibConf &conf) {
  std::vector<string> default_path = {"."};
  const vector<string> &path = (conf.base_path.size() == 0 ? default_path : conf.base_path);
  if (pb_loader_.Init(conf.instance_proto, path) < 0) {
    printf("Init failed: pb_loader initialization error\n");
    return -1;
  }
  vector<string> ipport;
  common::SplitString(conf.ipport, ":", &ipport);
  if (ipport.size() == 2) {
    prx_ = comm_.stringToProxy<ServantProxyPtr>(conf.servant_obj + "@tcp -h " + ipport[0] + " -p " + ipport[1]);
  } else {
    printf("ipport not specified\n");
    comm_.setProperty("locator", "tars.tarsregistry.QueryObj@tcp -h " +
                                            conf.locator_host +
                                            " -p " +
                                            conf.locator_port);
    prx_ = comm_.stringToProxy<ServantProxyPtr>(conf.servant_obj);
  }
  if (prx_.get() == nullptr) {
    printf("init failed: stringtoproxy error\n");
    return -1;
  }
  return 0;
}

int32_t RpcLib::Req(const std::string &req_msg, const std::string &req_data, const std::string &rpc_name,
                    const std::string &resp_msg,  std::string *outjson, int32_t timeout_ms) const {
  string req_pb;
  int ret = pb_loader_.Json2PbBinary(req_msg, req_data, req_pb);
  if (ret < 0) {
    printf("Json2PbBinary error\n");
    *outjson = "Json2PbBinary error";
    return -2;
  }

  vector<char> req_pb_vc(req_pb.begin(), req_pb.end());
  tars::ResponsePacket resp;
  map<string, string> context, _mStatus;
  try {
    prx_->tars_set_timeout(timeout_ms)->tars_invoke(tars::TARSNORMAL, rpc_name, req_pb_vc,
                     context, _mStatus, resp);
  } catch (std::exception &e) {
    *outjson = e.what();
    return -1;
  } catch (...) {
    *outjson = "rpc error";
    return -1;
  }

  string resp_pb(resp.sBuffer.data(), resp.sBuffer.size());
  ret = pb_loader_.PbBinary2Json(resp_msg, resp_pb, *outjson);
  if (ret < 0) {
    printf("PbBinary2Json error\n");
    *outjson = "PbBinary2Json error";
    return -2;
  }
  return 0;
}
