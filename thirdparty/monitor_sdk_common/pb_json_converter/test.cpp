// @Copyright vivo
// autho: yuchengzhen
#include "thirdparty/monitor_sdk_common/pb_json_converter/pb_json_converter.h"
#include "thirdparty/monitor_sdk_common/pb_json_converter/test.pb.h"
#include <string>

using std::string;
using google::protobuf::Message;

int test1() {
  string s = "{\"version\":0,\"word\":\"testword\",\"count\":[1,2,3,4],\"type\":1,\"subtests\":[{\"version\":1,\"word\":\"subtest1\"},{\"version\":1,\"word\":\"subtest2\"},{\"version\":1,\"word\":\"subtest3\"},{\"version\":1,\"word\":\"subtest4\"}],\"message\":{\"version\":1,\"word\":\"testmessageword\"},\"data_list\":[\"l1\",\"l2\",\"l3\",\"l4\"]}";
  test::Test output;
  Jsonstr2Pb(s, &output);
  printf("%s\n", output.DebugString().c_str());
  string final;
  Pb2JsonstrPretty(output, &final);
  printf ("%s\n", final.c_str());
  string full;
  Pb2FullJsonstrPretty(output, &final);
  printf ("%s\n", final.c_str());
  return 0;
}

int test2() {
  test::Node node;
  std::string s;
  Pb2FullJsonstrPretty(node, &s);
  printf("%s\n", s.c_str());
  return 0;
}

int main() {
  test1();
  return test2();
}
