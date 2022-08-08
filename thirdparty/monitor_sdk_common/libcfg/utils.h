//
// Created by 黄伟锋 on 2019-04-16.
//

#ifndef COMMON_LIBCFG_UTILS_H_
#define COMMON_LIBCFG_UTILS_H_

#include <string>
#include <map>

#include "thirdparty/jsoncpp/json.h"


namespace common {


class CfgStringUtils {
 public:
  static bool IsStringNotblank(const std::string &token);
  static bool AreStringsEqual(const std::string &a, const std::string &b);
};


class CfgTimeUtils {
 public:
  static long GetCurrentTimeMillis();
};


class CfgMapUtils {
 public:
  static bool AreMapsEqual(const std::map<std::string, std::string> &lhs, const std::map<std::string, std::string> &rhs);
};


class CfgJsonUtils {
 public:
  static std::string ParseString(const Json::Value &root);
};


} // namespace common

#endif //COMMON_LIBCFG_UTILS_H_
