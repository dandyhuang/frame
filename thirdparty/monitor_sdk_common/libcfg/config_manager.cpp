//
// Created by 黄伟锋 on 2019-04-10.
//

#include <map>
#include <vector>
#include <sstream>

#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_constants.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_loader_factory.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_manager.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_manager_internal.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_utils.h"
#include "thirdparty/monitor_sdk_common/libcfg/local_properties.h"
#include "thirdparty/monitor_sdk_common/libcfg/utils.h"

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/jsoncpp/json.h"

using namespace common;

DEFINE_string(cfg_local_path, "config/vivo.properties","配置中心本地配置文件路径");


ConfigManager::ConfigManager() {}


ConfigManager::~ConfigManager() {}


bool ConfigManager::Initialize() {
  return Initialize(std::vector<PropertySource>());
}


bool ConfigManager::Initialize(const std::vector<PropertySource> &common_configs) {
  std::map<std::string, std::string> ret;
  if (!ConfigLoaderFactory::Instance()->InitializeConfigLoader(common_configs, &ret)) {
    CfgLogUtils::LogInfo(StringPrint("[ConfigManager] Initialize remote properties failed"));
    return false;
  }

  return true;
}


void ConfigManager::SetLogger(std::shared_ptr<ConfigLogger> logger) {
  CfgLogUtils::Instance()->SetLogger(logger);
}


bool ConfigManager::AddListener(std::shared_ptr<ConfigListener> listener) {
  return ConfigManagerInternal::Instance()->AddListener(listener);
}


bool ConfigManager::RemoveListener(std::shared_ptr<ConfigListener> listener) {
  return ConfigManagerInternal::Instance()->RemoveListener(listener);
}


void ConfigManager::SetAppName(const std::string &app_name) {
  LocalProperties::app_name = app_name;
}


void ConfigManager::SetAppEnv(const std::string &app_env) {
  LocalProperties::app_env = app_env;
}


void ConfigManager::SetConfigVersion(const std::string &config_version) {
  LocalProperties::config_version = config_version;
}


void ConfigManager::SetConfigHost(const std::string &config_host) {
  LocalProperties::config_host = config_host;
}

void ConfigManager::SetThreadNum(int thread_pool_num) {
    LocalProperties::thread_pool_num = thread_pool_num;
}

std::map<std::string, std::string> ConfigManager::GetProps(){
    return ConfigManagerInternal::Instance()->GetProps();
}

bool ConfigManager::Get(const std::string &key, std::string *ret) {
  return ConfigManagerInternal::Instance()->Get(key, ret);
}


bool ConfigManager::GetString(const std::string &key, std::string *ret) {
  return ConfigManagerInternal::Instance()->GetString(key, ret);
}


bool ConfigManager::GetInteger(const std::string &key, int *ret) {
  return ConfigManagerInternal::Instance()->GetInteger(key, ret);
}


bool ConfigManager::GetLong(const std::string &key, long *ret) {
  return ConfigManagerInternal::Instance()->GetLong(key, ret);
}


bool ConfigManager::GetShort(const std::string &key, short *ret) {
  return ConfigManagerInternal::Instance()->GetShort(key, ret);
}


bool ConfigManager::GetBoolean(const std::string &key, bool *ret) {
  return ConfigManagerInternal::Instance()->GetBoolean(key, ret);
}


bool ConfigManager::GetDouble(const std::string &key, double *ret) {
  return ConfigManagerInternal::Instance()->GetDouble(key, ret);
}


bool ConfigManager::GetFloat(const std::string &key, float *ret) {
  return ConfigManagerInternal::Instance()->GetFloat(key, ret);
}


std::set<std::string> ConfigManager::Keys() {
  return ConfigManagerInternal::Instance()->Keys();
}


std::string ConfigManager::app_name() {
  return ConfigManagerInternal::Instance()->app_name();
}


std::string ConfigManager::app_env() {
  return ConfigManagerInternal::Instance()->app_env();
}


std::string ConfigManager::config_version() {
  return ConfigManagerInternal::Instance()->config_version();
}


std::string ConfigManager::app_loc() {
  return ConfigManagerInternal::Instance()->app_loc();
}


std::string ConfigManager::config_host() {
  return ConfigManagerInternal::Instance()->config_host();
}


std::string ConfigManager::config_agent() {
  return ConfigManagerInternal::Instance()->config_agent();
}


std::string ConfigManager::config_monitor() {
  return ConfigManagerInternal::Instance()->config_monitor();
}

bool ConfigManager::LoadLocalConfigFile(const std::string &path) {
    std::string file_path = path.empty() ? FLAGS_cfg_local_path : path;
    std::string data_str;
    File::ReadAll(file_path, &data_str);

    Json::CharReaderBuilder builder;
    scoped_ptr<Json::CharReader> reader(builder.newCharReader());
    Json::Value root;
    if (!reader->parse(data_str.c_str(), data_str.c_str() + data_str.size(), &root, NULL)) {
        return false;
    }

    //std::map<std::string, std::string> props;
    Json::Value::Members members = root.getMemberNames();
    for (Json::Value::Members::iterator iter_member = members.begin(); iter_member != members.end(); iter_member++) {
        std::string str_key = *iter_member;
        std::string str_val = root[str_key.c_str()].asString();
        //props[str_key] = str_val;
        if (CfgStringUtils::AreStringsEqual(ConfigConstants::APP_NAME, str_key)) {
            SetAppName(str_val);
        } else if (CfgStringUtils::AreStringsEqual(ConfigConstants::APP_ENV, str_key)) {
            SetAppEnv(str_val);
        } else if (CfgStringUtils::AreStringsEqual(ConfigConstants::CONFIG_VERSION, str_key)) {
            SetConfigVersion(str_val);
        } else if (CfgStringUtils::AreStringsEqual(ConfigConstants::CONFIG_HOST, str_key)) {
            SetConfigHost(str_val);
        }
    }
    return true;
}

