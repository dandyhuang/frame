//
// Created by 黄伟锋 on 2019-04-22.
//

#ifndef COMMON_LIBRMQ_UTILS_H_
#define COMMON_LIBRMQ_UTILS_H_

#include <string>
#include <map>
#include <vector>
#include <sys/syscall.h>

#include "thirdparty/monitor_sdk_common/librmq/rmq_connection_config.h"


namespace common {

class RmqConnectionConfigUtils {
 public:
  static bool SaveLocalCache(const std::string &topic_key, const std::string &connection_config_str);
  static bool LoadLocalCache(const std::string &topic_key, std::string *ret);

  static std::string get_cache_filename(const std::string &topic_key);
};


class RmqSignatureUtils {
 public:
  static bool GenerateSignature(
      const std::string &app_name,
      const std::string &topic_key,
      const std::string &idc,
      const std::string &timestamp,
      const std::string &nonce,
      const std::string &secret_key,
      std::string *ret);

  static bool Hamcsha1(const std::string &data_str, const std::string &secret_key,
      std::string *ret);

};


class RmqLogUtils {
 public:
  static void LogInfo(const std::string &msg);
};


class RmqTimeUtils {
 public:
  static long GetCurrentTimeMillis();
};


class RmqHttpUtils {
 public:
  static std::string user_agent();
};


template <typename T>
class RmqMapUtils {
 public:
  static std::vector<std::string> GetStringKeys(std::map<std::string, T> &m) {
    std::vector<std::string> keys;
    keys.reserve(m.size());
    for (auto &kv : m) {
      keys.push_back(kv.first);
    }
    return keys;
  }
};

class RmqIpUtils {
public:
    static std::string get_host_name();
};

} // namespace common

#endif //COMMON_LIBRMQ_UTILS_H_
