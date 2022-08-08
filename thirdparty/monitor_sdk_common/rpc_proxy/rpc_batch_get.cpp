// @copyright vivo
// author: yuchengzhen

#include "thirdparty/monitor_sdk_common/rpc_proxy/rpc_lib.h"
#include "thirdparty/monitor_sdk_common/storage/file/local_file.h"
#include "thirdparty/rapidjson/document.h"

#define fill_string(s, k, v) do {\
  auto it = v.FindMember(k);\
  if (it != v.MemberEnd()) {\
    if (it->value.IsString()) s = it->value.GetString();\
    if (it->value.IsInt64()) s = std::to_string(it->value.GetInt64());\
  }\
}while(0)


struct ConfContent {
  RpcLibConf conf;
  std::string req_msg;
  std::string resp_msg;
  std::string rpc_name;
  std::string req_data_file;
  std::string out_file;
};

static int32_t parse_conf(const std::string &conf_file, ConfContent *out) {
  std::string content;
  if (!common::File::ReadAll(conf_file, &content)) return -1;
  rapidjson::Document doc;
  if (doc.Parse(content.c_str(), content.size()).HasParseError()) {
    printf("conf format error: parse doc to json failed\n");
    return -1;
  }
  fill_string(out->req_msg, "req_msg", doc);
  fill_string(out->resp_msg, "resp_msg", doc);
  fill_string(out->rpc_name, "rpc_name", doc);
  fill_string(out->req_data_file, "req_data_file", doc);
  fill_string(out->out_file, "out_file", doc);
  fill_string(out->conf.locator_host, "locator_host", doc);
  fill_string(out->conf.locator_port, "locator_port", doc);
  fill_string(out->conf.servant_obj, "servant_obj", doc);
  fill_string(out->conf.ipport, "ipport", doc);
  fill_string(out->conf.instance_proto, "instance_proto", doc);
  // base_path
  auto it = doc.FindMember("base_path");
  if (it != doc.MemberEnd() && it->value.IsArray()) {
    for (int32_t i = 0; i < (int32_t)it->value.Size(); ++i) {
      if (it->value[i].IsString()) {
        out->conf.base_path.push_back(it->value[i].GetString());
      }
    }
  }
  return 0;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("usage: %s conf_file\n", argv[0]);
    return -1;
  }
  ConfContent conf;
  if (parse_conf(argv[1], &conf)) {
    printf("read conf failed\n");
    return -1;
  }
  RpcLib rpc_lib;
  if (rpc_lib.Init(conf.conf)) {
    printf("init rpc failed\n");
    return -1;
  }
  std::unique_ptr<common::File> fr(common::File::Open(conf.req_data_file, "r"));
  std::unique_ptr<common::File> fw(common::File::Open(conf.out_file, "w"));
  std::string line;
  string nl = "\n";
  while (fr->ReadLine(&line)) {
    std::string out;
    if (!rpc_lib.Req(conf.req_msg, line, conf.rpc_name, conf.resp_msg, &out)) {
      fw->Write(out.data(), out.size());
    }
    fw->Write(nl.data(), nl.size());
  }
  return 0;
}
