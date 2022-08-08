//
// Created by 黄伟锋 on 2019-04-22.
//

#ifndef COMMON_LIBRMQ_RMQ_VHOST_CONFIG_H_
#define COMMON_LIBRMQ_RMQ_VHOST_CONFIG_H_

#include <string>
#include <vector>

namespace common {

class RmqVhostConfig {
 public:
  // 使用的vhost */
  std::string vhost;

  // 为容错，会有多个ip下来，其中第一个是master节点IP，然后随机选几个IP，最终会提供业务设置的域名，或是第二机房的一个节点ip */
  std::vector<std::string> nodes;
};

} // namespace common

#endif //COMMON_LIBRMQ_RMQ_VHOST_CONFIG_H_
