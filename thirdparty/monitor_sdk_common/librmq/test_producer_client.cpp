
#include <iostream>

#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/librmq/utils.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_producer_client.h"
#include "thirdparty/monitor_sdk_common/system/threading/this_thread.h"

using namespace common;
using namespace std;

class TestProducerHandler : public RmqProducerHandler {
 public:
  void OnError(const std::string &topic_key, const std::string &message) {
    cout << "Producer OnError topic: " << topic_key << " ,  message: " << message << endl;
  }

  void OnSuccess(const std::string &topic_key, const std::string &message) {
    cout << "Producer OnSuccess topic: " << topic_key << " ,  message: " << message << endl;
  }
};


int main(int argc, char const *argv[]) {
  auto *loop = EV_DEFAULT;

  std::string topic_key = "0e99c3e7789344cd948cb1d9b3820b58";
  std::string secret_key = "cf9d84fde6a144bca5a69826ac687b3f";

  RmqProducerClient producer_client(loop);
  producer_client.SetAppname("rmq");
  producer_client.SetIdc("sz-sk");
  producer_client.SetServerHost("middleware-rmq-name-test.vmic.xyz");
  producer_client.PutSecretKey(topic_key, secret_key);

  TestProducerHandler *handler = new TestProducerHandler();

  auto on_init_ready = [&producer_client, &topic_key, handler]() {
    cout << "初始化成功，开始生产消息!" << endl;

    producer_client.RegisterHandler(handler);

    for (int i = 0; i < 100; i ++) {
      producer_client.Publish(topic_key, StringPrint("测试消息: %d", i + 1).c_str());
    }
  };

  auto on_init_error = [](const std::string &message) {
    cout << "初始化失败: " << message << endl;
  };

  // 初始化 client
  producer_client.Initialize(on_init_ready, on_init_error);

  ev_run(loop, 0);

  delete handler;

  cout << "finish !!!" << endl;

  return 0;
}
