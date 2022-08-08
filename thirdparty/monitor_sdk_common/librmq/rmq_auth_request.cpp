//
// Created by 黄伟锋 on 2019-04-23.
//

#include "thirdparty/monitor_sdk_common/librmq/rmq_auth_request.h"

#include "thirdparty/jsoncpp/json.h"

using namespace common;


std::string RmqAuthRequest::ToJson() const {
  Json::Value root;
  root["appName"] = appname;
  root["idc"] = idc;
  root["nonce"] = nonce;
  root["producer"] = producer;
  root["timestamp"] = timestamp;
  root["token"] = token;
  root["topicKey"] = topic_key;
  root["version"] = version;

  return root.toStyledString();
}
