//
// Created by 黄伟锋 on 2019-04-23.
//

#ifndef COMMON_LIBRMQ_RMQ_AUTH_REQUEST_H_
#define COMMON_LIBRMQ_RMQ_AUTH_REQUEST_H_

#include <string>


namespace common {

class RmqAuthRequest {
 public:

  std::string ToJson() const;

  // 应用名
  std::string appname;

  //topic key
  std::string topic_key;

  //idc
  std::string idc;

  //时间戳
  std::string timestamp;

  //随机数
  std::string nonce;

  //客户端版本
  std::string version;

  //是否生产者
  bool producer;

  //token
  std::string token;

  //异常原因
  std::string reason;

};

} // namespace common

#endif //COMMON_LIBRMQ_RMQ_AUTH_REQUEST_H_
