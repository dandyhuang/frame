#include "thirdparty/monitor_sdk_common/rpc_proxy/pb_loader.h"

using std::string;

int main(int argc, const char **argv) {
  if (argc < 4) {
    printf("usage: %s protofile, jsonstr, message_name\n", argv[0]);
    return -1;
  }
  string protoflie = argv[1];
  string jsonstr = argv[2];
  string message_name = argv[3];
  PbLoader pb_loader;
  pb_loader.Init(protoflie, {{"."}});
  string pb;
  pb_loader.Json2PbBinary(message_name, jsonstr, pb);
  string json;
  pb_loader.PbBinary2Json(message_name, pb, json);
  printf("json:\n%s\n", json.c_str());
  pb_loader.PbBinary2FullJson(message_name, pb, json);
  printf("fulljson:\n%s\n", json.c_str());
  pb_loader.PbDefalut2FullJson(message_name, json);
  printf("defaultjson: \n%s\n", json.c_str());
  return 0;
}
