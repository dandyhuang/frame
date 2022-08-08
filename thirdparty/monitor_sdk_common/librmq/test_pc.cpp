// Copyright (c) 2020, Tencent Inc. All rights reserved.
// Author: Fly Qiu <jamesqiu@tencent.com>
// Created: 2020-05-12
#include<iostream>
#include<map>
#include<thread>
#include "thirdparty/monitor_sdk_common/librmq/rmq_consumer_client.h"
#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/librmq/utils.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_producer_client.h"
#include "thirdparty/monitor_sdk_common/system/threading/this_thread.h"

using namespace common;
using namespace std;

struct ev_loop* loop2;
struct ev_loop* loop3;

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

class TestProducerHandler : public RmqProducerHandler {
 public:
  void OnError(const std::string &topic_key, const std::string &message) {
    cout << "Producer OnError topic: " << topic_key << " ,  message: " << message << endl;
  }

  void OnSuccess(const std::string &topic_key, const std::string &message) {
    cout << "Producer OnSuccess topic: " << topic_key << " ,  message: " << message << endl;
  }
};

void TestConsumer()
{
    std::string topic_key = "46c8cfc69db646968daf2ec23d263a75";
    std::string secret_key = "9eb604ae566a47dcbb478b651a64d9da";

    RmqConsumerClient consumer_client(loop3);
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

    ev_loop(loop3, 0);

    delete handler;
}

void TestProducer()
{
    std::string topic_key = "0e99c3e7789344cd948cb1d9b3820b58";
    std::string secret_key = "cf9d84fde6a144bca5a69826ac687b3f";

    RmqProducerClient producer_client(loop2);
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

    ev_loop(loop2, 0);

    delete handler;

    cout << "finish !!!" << endl;
}

int main()
{
    struct ev_loop *loop = EV_DEFAULT;
    loop2 = ev_loop_new(0);
    loop3 = ev_loop_new(0);
    std::thread testConsumer = std::thread(&TestConsumer);
    std::thread testProducer = std::thread(&TestProducer);
    ev_loop(loop, 0);
    testConsumer.join();
    testProducer.join();
    return 0;
}




