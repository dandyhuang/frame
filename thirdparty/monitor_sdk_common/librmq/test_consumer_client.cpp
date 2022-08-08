
#include <iostream>
#include <set>

#include "thirdparty/monitor_sdk_common/librmq/rmq_consumer_client.h"

using namespace common;
using namespace std;

class TestConsumerHandler : public RmqConsumerHandler {
 public:
  ~TestConsumerHandler() {}

  void OnReady() {
    cout << "消费者注册成功" << endl;
  }

  void OnError(const std::string &message) {
    cout << "消费者注册失败: " << message << endl;
  }

  bool OnMessage(const std::string &topic_key, const std::string &message) {
    cout << "消费消息： topic_key: " << topic_key << ", message: " << message << endl;

    // 返回 true 表示消息处理成功，可以 ack 消息；
    // 返回 false 表示消息处理失败，不会 ack 消息，接下来会重复消费
    return true;
  }

};


int main(int argc, char const *argv[]) {

  auto *loop = EV_DEFAULT;

  std::string topic_key = "46c8cfc69db646968daf2ec23d263a75";
  std::string secret_key = "9eb604ae566a47dcbb478b651a64d9da";

  RmqConsumerClient consumer_client(loop);
  consumer_client.SetAppname("rmq");
  consumer_client.SetIdc("sz-sk");
  consumer_client.SetServerHost("middleware-rmq-name-test.vmic.xyz");
  consumer_client.PutSecretKey(topic_key, secret_key);

  TestConsumerHandler *handler = new TestConsumerHandler();

  auto on_init_ready = [&consumer_client, handler]() {
    cout << "初始化成功，开始注册消费者" << endl;
    consumer_client.Consume(handler);
  };

  auto on_init_error = [](const std::string &message) {
    cout << "初始化失败" << message << endl;
  };

  // 初始化 client
  consumer_client.Initialize(on_init_ready, on_init_error);

  ev_run(loop, 0);

  delete handler;

  return 0;
}
