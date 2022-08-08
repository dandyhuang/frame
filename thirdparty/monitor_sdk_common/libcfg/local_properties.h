//
// Created by 黄伟锋 on 2019-05-27.
//

#ifndef COMMON_LIBCFG_LOCAL_PROPERTIES_H_
#define COMMON_LIBCFG_LOCAL_PROPERTIES_H_

#include <string>

namespace common {

class LocalProperties {
 public:
  static std::string app_name;
  static std::string app_env;
  static std::string config_version;
  static std::string config_host;
  static std::string enable_remote_config;

  static int thread_pool_num;   //每个线程池开启的线程数
};

} // namespace common

#endif //COMMON_LIBCFG_LOCAL_PROPERTIES_H_
