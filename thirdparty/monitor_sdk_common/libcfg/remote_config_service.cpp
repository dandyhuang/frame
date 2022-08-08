//
// Created by 黄伟锋 on 2019-04-15.
// Modified by 王锦涛 on 2019-11-18.
//

#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/base/string/concat.h"
#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/base/string/bitwise.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_constants.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_utils.h"
#include "thirdparty/monitor_sdk_common/libcfg/global_client_settings.h"
#include "thirdparty/monitor_sdk_common/libcfg/remote_config_service.h"
#include "thirdparty/monitor_sdk_common/libcfg/utils.h"
#include "thirdparty/monitor_sdk_common/system/threading/this_thread.h"
#include "thirdparty/monitor_sdk_common/encoding/base64.h"
#include "thirdparty/monitor_sdk_common/encoding/vivobase64.h"

#include "thirdparty/glog/logging.h"

// 用于CryptoPP解密
#include "thirdparty/cryptopp/cryptlib.h"
using CryptoPP::Exception;

#include "thirdparty/cryptopp/hex.h"
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

#include "thirdparty/cryptopp/filters.h"
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;

#include "thirdparty/cryptopp/aes.h"
using CryptoPP::AES;

#include "thirdparty/cryptopp/modes.h"
using CryptoPP::CFB_Mode;

#include "thirdparty/cryptopp/config.h"
using CryptoPP::byte;

using namespace common;

HttpClient::Options GetCommonHttpOptions();


const std::string GET_CONFIG_ACTION_NAME = "getConfig";
const std::string GET_CONFIG_CHANGE_MESSAGE_ACTION_NAME = "getConfigChangeMessage";


RemoteConfigService::RemoteConfigService() {}


RemoteConfigService::~RemoteConfigService() {}


std::string RemoteConfigService::GetDeocdedValue(std::string &kmsvalue, std::string &kmskey1, std::string &kmskey2) {
  std::string base64_key1, base64_key2;
  Base64::Decode(kmskey1, &base64_key1);
  Base64::Decode(kmskey2, &base64_key2);
  size_t size_of_key = (base64_key1.size() < base64_key2.size()) ? base64_key1.size() : base64_key2.size();

  // 这里key的长度必须是32
  if(size_of_key == 32){
    // 设置解密所需要的key
    byte key[32];
    for (int i = 0; i < 32; ++i)
    {
      key[i] = base64_key1[i] ^ base64_key2[i];
    }
    // 设置解密所需要的iv
    byte iv[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

    VivoBase64 vb64;
    std::vector<char> vivobase64_decoded_value = vb64.Decode(kmsvalue);
    std::string cipher_text, plain_text;
    cipher_text = "";
    // 从第45位开始取密文
    for (std::vector<char>::size_type i = 45; i < vivobase64_decoded_value.size(); ++i)
    {
      cipher_text += vivobase64_decoded_value[i];
    }
    //LogUtils::LogDebug(StringPrint("kms value is [%s] and cipher text is [%s]", kmsvalue, cipher_text));

    try
    {
      CFB_Mode< AES >::Decryption d;
      // 给Decryption实例绑定key和iv
      d.SetKeyWithIV(key, sizeof(key), iv);

      // 开始解密
      // The StreamTransformationFilter removes padding as required.
      StringSource s(cipher_text, true, new StreamTransformationFilter(
          d, new StringSink(plain_text), StreamTransformationFilter::DEFAULT_PADDING
        ) // StreamTransformationFilter
      ); // StringSource
      //LogUtils::LogDebug(StringPrint("plain text is [%s]", plain_text));
    }
    catch(const CryptoPP::Exception& e)
    {
      exit(1);
    }

    return plain_text;
  }
  // 如果key的长度不是32，则返回为解密前的密文
  else {
    //LogUtils::LogError(StringPrint("length of key is [%d], which is incorrect, should be 32!", size_of_key));
    return kmsvalue;
  }
}


bool RemoteConfigService::GetConfig(const GetConfigRequest &get_config_request, int http_timeout, CommonVO<GetConfigResult> *ret) {

  std::string url = BuildGetConfigUrl(get_config_request, GET_CONFIG_ACTION_NAME);

  HttpClient::Options options = GetCommonHttpOptions();
  HttpClient client;
  HttpResponse response;
  HttpClient::ErrorCode error;
  bool get_ret = false;
  try {
      get_ret = client.Get(url, options, &response, &error);
  } catch(...){
        CfgLogUtils::LogWarn("http client get method failed.");
  }
  if (!get_ret) return false;

  std::string json_str = response.Body();
  Json::CharReaderBuilder builder;
  scoped_ptr<Json::CharReader> reader(builder.newCharReader());
  Json::Value root;
  if (!reader->parse(json_str.c_str(), json_str.c_str() + json_str.size(), &root, NULL)) return false;

  CommonVO<GetConfigResult> vo;
  vo.retcode = root["retcode"].asInt();
  vo.message = root["message"].asString();

  Json::Value data = root["data"];
  GetConfigResult get_config_result;
  get_config_result.last_modified = long(data["lastModified"].asUInt64());
  get_config_result.request_timestamp = long(data["requestTimestamp"].asUInt64());
  get_config_result.response_timestamp = long(data["responseTimestamp"].asUInt64());
  get_config_result.real_config_version = std::string(data["realConfigVersion"].asString());

  std::vector<PropertyItem> configs;
  for (Json::Value &item : data["configs"]) {
    int isEncrypted = item["isEncrypted"].asInt();
    std::string value = "";
    if(isEncrypted != 0){
      std::string kmsvalue = item["value"].asString();
      std::string kmskey1 = item["kmsKey1"].asString();
      std::string kmskey2 = item["kmsKey2"].asString();
      value = GetDeocdedValue(kmsvalue, kmskey1, kmskey2);
    } else {
      value = item["value"].asString();
    }
    std::string name = item["name"].asString();
    configs.push_back(PropertyItem(name, value));
  }
  get_config_result.configs = configs;

  vo.data = get_config_result;

  *ret = vo;

  return true;
}


bool RemoteConfigService::LongPolling(
    const GetConfigRequest &get_config_request,
    CommonVO<ConfigChangeMessage> *ret,
    bool *terminated) {

  std::string url = BuildGetConfigUrl(get_config_request, GET_CONFIG_CHANGE_MESSAGE_ACTION_NAME);

  HttpClient::Options options = GetCommonHttpOptions();
  options.AddHeader("Connection", "Keep-Alive");
  options.AddHeader("Keep-Alive", StringPrint("timeout=%d, max=1000", GlobalClientSettings::long_polling_http_timeout / 1000));

  HttpClient client;
  HttpResponse response;
  HttpClient::ErrorCode error;

  bool http_get_success = false;
  bool http_get_fail = false;
  auto task_func = [this, &http_get_success, &http_get_fail, &client, &url, &options, &response, &error] {
      try {
            bool get_ret = client.Get(url, options, &response, &error);
            if (!get_ret) {
                http_get_fail = true;
            }
            http_get_success = true;
      } catch(...){
        CfgLogUtils::LogWarn("http client get method failed.");
      }
  };
  Thread thread;
  thread.Start(task_func);

  while (true) {
    ThisThread::Sleep(1);
    if (http_get_fail) return false;
    if (http_get_success) break;
    if (*terminated) {
      thread.Detach();
      return false;
    }
  }

  std::string json_str = response.Body();
  Json::CharReaderBuilder builder;
  scoped_ptr<Json::CharReader> reader(builder.newCharReader());
  Json::Value root;
  if (!reader->parse(json_str.c_str(), json_str.c_str() + json_str.size(), &root, NULL)) return false;

  CommonVO<ConfigChangeMessage> vo;
  vo.retcode = root["retcode"].asInt();
  vo.message = root["message"].asString();

  bool has_data = false;
  Json::Value::Members members = root.getMemberNames();
  for (Json::Value::Members::iterator iter_member = members.begin(); iter_member != members.end(); iter_member++) {
    std::string str_key = *iter_member;
    if (CfgStringUtils::AreStringsEqual(str_key, "data")) {
      has_data = true;
      break;
    }
  }

  if (has_data) {
    Json::Value data = root["data"];
    ConfigChangeMessage config_change_message;
    config_change_message.last_modified = long(data["lastModified"].asUInt64());
    vo.data = config_change_message;
  }

  *ret = vo;

  return true;
}


bool RemoteConfigService::UploadConfigEffectInfo(const std::string &config_host,
    const std::vector<ConfigEffectInfo> &config_effect_infos) {
  std::string url = StringPrint("%sV2/uploadConfigEffectInfo", config_host);

  HttpClient::Options options = GetCommonHttpOptions();
  HttpClient client;
  HttpResponse response;
  HttpClient::ErrorCode error;
  std::string data = ConfigEffectInfo::SerializeList(config_effect_infos);

  return client.Post(url, data, options, &response, &error);
}


std::string RemoteConfigService::BuildGetConfigUrl(const GetConfigRequest &get_config_request, const std::string &method_name) {

  std::string app_name = get_config_request.app_name;
  std::string app_env = get_config_request.app_env;
  std::string config_version = get_config_request.config_version;
  std::string client_default_config_version = get_config_request.client_default_config_version;
  std::string config_host = get_config_request.config_host;
  long last_modified = get_config_request.last_modified;
  std::string host_name = get_config_request.host_name;
  std::string ip = get_config_request.ip;
  std::string app_loc = get_config_request.app_loc;

  std::string sb = "";
  sb = StringConcat(sb, "lastModified");
  sb = StringConcat(sb, "=");
  sb = StringConcat(sb, URI::Encode(StringPrint("%ld", last_modified)));

  if (CfgStringUtils::IsStringNotblank(app_env)) {
    sb = StringConcat(sb, "&appEnv=");
    sb = StringConcat(sb, URI::Encode(app_env));
  }

  if (CfgStringUtils::IsStringNotblank(app_name)) {
    sb = StringConcat(sb, "&appName=");
    sb = StringConcat(sb, URI::Encode(app_name));
  }

  if (CfgStringUtils::IsStringNotblank(config_version)) {
    sb = StringConcat(sb, "&configVersion=");
    sb = StringConcat(sb, URI::Encode(config_version));
  }

  if (CfgStringUtils::IsStringNotblank(client_default_config_version)) {
    sb = StringConcat(sb, "&clientDefaultConfigVersion=");
    sb = StringConcat(sb, URI::Encode(client_default_config_version));
  }

  // 客户端信息
  if (CfgStringUtils::IsStringNotblank(config_host)) {
    sb = StringConcat(sb, "&configHost=");
    sb = StringConcat(sb, URI::Encode(config_host));
  }

  if (CfgStringUtils::IsStringNotblank(host_name)) {
    sb = StringConcat(sb, "&hostName=");
    sb = StringConcat(sb, URI::Encode(host_name));
  }

  if (CfgStringUtils::IsStringNotblank(ip)) {
    sb = StringConcat(sb, "&ip=");
    sb = StringConcat(sb, URI::Encode(ip));
  }

  if (CfgStringUtils::IsStringNotblank(app_loc)) {
    sb = StringConcat(sb, "&appLoc=");
    sb = StringConcat(sb, URI::Encode(app_loc));
  }

  return StringPrint("%sV2/%s?%s", config_host, method_name, sb);
}


HttpClient::Options RemoteConfigService::GetCommonHttpOptions() {
  HttpClient::Options options;
  std::string user_agent = StringPrint("%s/%s", ConfigConstants::DEFAULT_MODULE_NAME, ConfigConstants::UNKNOWN_MODULE_VERSION);
  options.AddHeader("User-Agent", user_agent);
  options.AddHeader("Content-Type", ConfigConstants::DEFAULT_CONTENT_TYPE);
  options.AddHeader("charset", ConfigConstants::DEFAULT_CHARSET_NAME);
  return options;
}

