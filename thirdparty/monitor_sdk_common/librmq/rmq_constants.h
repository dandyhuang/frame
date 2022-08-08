//
// Created by 黄伟锋 on 2019-04-24.
//

#ifndef COMMON_LIBRMQ_RMQ_CONSTANTS_H_
#define COMMON_LIBRMQ_RMQ_CONSTANTS_H_

#include <string>

namespace common {

class RmqConstants {
 public:

  // 使用的SDK版本号，会体现在连接上
  static std::string VERSION;

  // topic分隔符
  static std::string TOPIC_SPLIT;

  // 缓存channel数量
  static int CACHING_CHANNEL_SIZE;

  // 心跳时间
  static int HEART_BEAT_TIME;

  // 连接超时时间
  static int CONNECTION_TIMEOUT;

  // 默认使用消费线程数
  static int DEFAULT_THREAD_NUM;

  // 默认prefetch
  static int DEFAULT_PREFETCH;

  // 启动消费线程数，队列空闲将会减少消息，减少MQ服务端消息压力
  static int START_THREAD_NUM;

  // 默认重试次数
  static int DEFAULT_RETRY_TRIMES;

  // 无限重试表示
  static int UNLIMITS_RETRY_TIMES;

  // 重试时间间隔
  static long DEFAULT_INITIAL_INTERVAL;

  static std::string NAME_SERVER_PREFIX;

  // 启动获取mq接口api
  static std::string NAME_SERVER_AUTH_API;

  // 阻塞获取连接参数
  static std::string NAME_SERVER_BLOCKED_API;

  // 统计数据上报接口
  static std::string NAME_SERVER_MERSURE_API;

  static std::string APP_NAME;

  static std::string NONCE;

  //消息的默认发送次数
  static int DEFAULT_SEND_TIMES;

  // 模块名
  static std::string MODULE_NAME;

  // QOS 预取长度
  static int QOS_PREFETCH_COUNT;

  // 每个 vhost client 检查 tcp 连接联通性的间隔，单位毫秒
  static int CHECK_USABLE_INTERVAL;

  // 连接断开后重试的时间间隔
  static int CONNECTION_RETRY_INTERVAL;
};

/**
 * 公共头字段
 */
class RmqHeaders {
public:
    // 发送服务名
    static std::string appname;

    // 发送节点ip
    static std::string appnode;
};

} // namespace common

#endif //COMMON_LIBRMQ_RMQ_CONSTANTS_H_
