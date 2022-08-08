//
// Created by 黄伟锋 on 2019-04-16.
//

#include <algorithm>
#include <chrono>
#include <utility>

#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/libcfg/utils.h"

#include "thirdparty/glog/logging.h"

using namespace common;


bool CfgStringUtils::CfgStringUtils::IsStringNotblank(const std::string &token) {
  return token.size() > 0;
}


bool CfgStringUtils::CfgStringUtils::AreStringsEqual(const std::string &a, const std::string &b) {
  return a.compare(b) == 0;
}



long CfgTimeUtils::GetCurrentTimeMillis() {
  using namespace std::chrono;
  milliseconds ms = duration_cast<milliseconds>(
      system_clock::now().time_since_epoch()
  );
  return long(ms.count());
}



bool CfgMapUtils::AreMapsEqual(const std::map<std::string, std::string> &lhs, const std::map<std::string, std::string> &rhs) {
  return lhs.size() == rhs.size()
      && std::equal(lhs.begin(), lhs.end(),
                    rhs.begin());
}


std::string CfgJsonUtils::ParseString(const Json::Value &root) {
//  return root.toStyledString();

  Json::FastWriter fast_writer;
  return fast_writer.write(root);
}