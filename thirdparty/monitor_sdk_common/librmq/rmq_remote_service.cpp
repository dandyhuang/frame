//
// Created by 黄伟锋 on 2019-04-23.
//

#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/base/string/format.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_connection_config.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_constants.h"
#include "thirdparty/monitor_sdk_common/librmq/rmq_remote_service.h"
#include "thirdparty/monitor_sdk_common/librmq/utils.h"

#include "thirdparty/jsoncpp/json.h"

using namespace common;

RmqRemoteService::RmqRemoteService(const std::map<std::string, std::string> &topic_key_secret_key_map) {
  topic_key_secret_key_map_ = topic_key_secret_key_map;
}


RmqRemoteService::~RmqRemoteService() {}


bool RmqRemoteService::PostAuth(RmqAuthRequest auth_request, const std::string &server_host, RmqConnectionConfig *ret) {
  std::string timestamp = StringPrint("%d", RmqTimeUtils::GetCurrentTimeMillis());
  std::string secret_key = topic_key_secret_key_map_[auth_request.topic_key];

  if (!RmqSignatureUtils::GenerateSignature(auth_request.appname,
      auth_request.topic_key, auth_request.idc, timestamp, RmqConstants::NONCE, secret_key, &auth_request.token))
    return false;

  std::string params = auth_request.ToJson();
  std::string url = StringPrint("http://%s%s", server_host, RmqConstants::NAME_SERVER_AUTH_API);

  HttpClient::Options options = GetCommonHttpOptions();
  HttpClient client;
  HttpResponse response;
  HttpClient::ErrorCode error;

  RmqLogUtils::LogInfo(StringPrint("post auth url: %s", url));
  RmqLogUtils::LogInfo(StringPrint("post auth params: %s", params));

  if (!client.Post(url, params, options, &response, &error)) {
    return LoadLocalConnectionConfig(auth_request.topic_key, ret);
  }

  std::string json_str = response.Body();

  RmqLogUtils::LogInfo(StringPrint("post auth response: %s", json_str));

  Json::CharReaderBuilder builder;
  scoped_ptr<Json::CharReader> reader(builder.newCharReader());
  Json::Value root;
  if (!reader->parse(json_str.c_str(), json_str.c_str() + json_str.size(), &root, NULL)) {
    return LoadLocalConnectionConfig(auth_request.topic_key, ret);
  }

  if (root["code"].isNull()) {
    return LoadLocalConnectionConfig(auth_request.topic_key, ret);
  }

  std::string code = root["code"].asString();
  if (code != "0") {
    if (code.find("10") == 0) {
      // 致命错误
      return false;
    } else {
      // 非致命错误
      return LoadLocalConnectionConfig(auth_request.topic_key, ret);
    }
  }

  return RmqConnectionConfig::DeserializeConnectionConfig(root["data"].toStyledString(), ret);
}


bool RmqRemoteService::PostHeartbeat(RmqMetric metric, const std::string &server_host, std::vector<RmqConnectionConfig> *ret) {
  std::string timestamp = StringPrint("%d", metric.end);
  std::string secret_key = topic_key_secret_key_map_[metric.topic_key];
  if (!RmqSignatureUtils::GenerateSignature(metric.appname,
      metric.topic_key, metric.idc, timestamp, RmqConstants::NONCE, secret_key, &metric.token))
    return false;

  std::string params = metric.ToJson();
  std::string url = StringPrint("http://%s%s", server_host, RmqConstants::NAME_SERVER_MERSURE_API);

  HttpClient::Options options = GetCommonHttpOptions();
  HttpClient client;
  HttpResponse response;
  HttpClient::ErrorCode error;

  RmqLogUtils::LogInfo(StringPrint("post heartbeat params: %s", params));
  RmqLogUtils::LogInfo(StringPrint("post heartbeat url: %s", url));

  if (!client.Post(url, params, options, &response, &error)) return false;

  std::string json_str = response.Body();

  RmqLogUtils::LogInfo(StringPrint("post heartbeat response: %s", json_str));

  Json::CharReaderBuilder builder;
  scoped_ptr<Json::CharReader> reader(builder.newCharReader());
  Json::Value root;
  if (!reader->parse(json_str.c_str(), json_str.c_str() + json_str.size(), &root, NULL)) return false;

  if (root["code"].asString().compare("0") != 0) return false;

  // 心跳成功，缓存 heartbeat 结果，防止下次 auth 接口请求失败
  for (Json::Value item : root["data"]) {
    RmqConnectionConfigUtils::SaveLocalCache(item["topicKey"].asString(), item.toStyledString());
  }

  std::vector<RmqConnectionConfig> configs;
  if (!RmqConnectionConfig::DeserializeConnectionConfigs(root["data"].toStyledString(), &configs)) return false;
  *ret = configs;

  return true;
}


bool RmqRemoteService::LoadLocalConnectionConfig(const std::string &topic_key, RmqConnectionConfig *ret) {
  std::string connection_config_str;
  if (!RmqConnectionConfigUtils::LoadLocalCache(topic_key, &connection_config_str)) return false;
  if (!RmqConnectionConfig::DeserializeConnectionConfig(connection_config_str, ret)) return false;

  return true;
}


HttpClient::Options RmqRemoteService::GetCommonHttpOptions() {
  HttpClient::Options options;
  options.AddHeader("User-Agent", RmqHttpUtils::user_agent());
  options.AddHeader("Content-Type", "application/json");
  options.AddHeader("Accept", "application/json");
  return options;
}

