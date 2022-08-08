#include "thirdparty/monitor_sdk_common/rpc_proxy/proxy_impl.h"

#include <string>
#include <vector>

#include "servant/Application.h"
#include "thirdparty/rapidjson/writer.h"

#include "thirdparty/monitor_sdk_common/pb_json_converter/pb_json_converter.h"
#include "thirdparty/monitor_sdk_common/rpc_proxy/rpc_lib.h"

using std::string;

void ProxyImp::initialize() {
  // initialize servant here:
}

void ProxyImp::destroy() {
  // destroy servant here:
}

vivo::commonproxy::RespData ProxyImp::getResp(const vivo::commonproxy::NormalReq& req, tars::TarsCurrentPtr current) {
  vivo::commonproxy::RespData resp;
  RpcLib rpc_lib;
  if (InitProxy(req.common_info(), &rpc_lib) < 0) {
    resp.set_answer_json(
        "{\"error\":\"rpc_lib init error, please check protofile: " +
        req.common_info().protofile_path().empty() ? "." : req.common_info().protofile_path() +
        "/" + req.common_info().protofile() +
        "\"}");
    return resp;
  }

  string answer_jsonstr;
  if (rpc_lib.Req(req.query_json().query_msg_name(), req.query_json().json_data(),
                  req.query_json().rpc_func_name(), req.query_json().answer_msg_name(), &answer_jsonstr) < 0) {
    resp.set_answer_json("{\"error\":\"" + answer_jsonstr + "\"}");
  } else {
    resp.set_answer_json(answer_jsonstr);
  }

  return resp;
}

vivo::commonproxy::MsgDefault ProxyImp::getDefalut(
  const vivo::commonproxy::MsgInfo &msg_info, tars::TarsCurrentPtr current) {
  vivo::commonproxy::MsgDefault msg_default;
  PbLoader pb_loader;
  std::vector<string> path_list;
  path_list.push_back(msg_info.protofile_path());
  pb_loader.Init(msg_info.protofile(), path_list);
  string *json = msg_default.mutable_msg_default();
  pb_loader.PbDefalut2FullJson(msg_info.msg_name(), *json);
  return msg_default;
}

int ProxyImp::InitProxy(const vivo::commonproxy::CommonInfo& com_info,
                      RpcLib* rpc_lib) {
  RpcLibConf conf;
  conf.locator_host = com_info.locator_ip();
  conf.locator_port = com_info.locator_port();
  conf.servant_obj = com_info.service_name();
  conf.ipport = com_info.ipport_assign();
  conf.base_path.push_back(com_info.protofile_path());
  conf.instance_proto = com_info.protofile();
  return rpc_lib->Init(conf);
}

