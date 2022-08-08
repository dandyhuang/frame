//
// Created by 黄伟锋 on 2019-04-13.
//

#include <set>
#include <sstream>

#include "thirdparty/monitor_sdk_common/base/string/format.h"
#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/libcfg/change_event_type.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_loader_factory.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_utils.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_manager_internal.h"
#include "thirdparty/monitor_sdk_common/libcfg/property_item.h"
#include "thirdparty/monitor_sdk_common/libcfg/utils.h"

using namespace common;


ConfigManagerInternal::ConfigManagerInternal() {}


ConfigManagerInternal::~ConfigManagerInternal() {}


bool ConfigManagerInternal::AddListener(std::shared_ptr<ConfigListener> listener) {
  listeners_.push_back(listener);
  return true;
}


bool ConfigManagerInternal::RemoveListener(std::shared_ptr<ConfigListener> listener) {
  auto it = std::find(listeners_.begin(), listeners_.end(), listener);
  listeners_.erase(it);
  return true;
}


void ConfigManagerInternal::CleanAndPutAll(std::map<std::string, std::string> configs) {
  if (configs.size() > 0) {

    if (props_.size() > 0) {
      std::vector<std::string> new_keys;
      for (auto &kv : configs) new_keys.push_back(kv.first);

      std::vector<std::string> redundances;
      for (auto &kv : props_) {
        std::string input = kv.first;
        if (std::find(new_keys.begin(), new_keys.end(), input) == new_keys.end()) {
          redundances.push_back(input);
        }
      }

      for (std::string redundance : redundances) {
        std::string value = props_[redundance];
        props_.erase(redundance);

        Remove(redundance, value);
      }
    }

    for (auto &kv : configs) {
      std::string key = kv.first;
      std::string value = kv.second;
      Put(key, value);
    }

  } else {

    CfgLogUtils::LogInfo("ConfigManager : cleanAndPutAll, the new config has none keys, clear.");
    Clear();
  }
}


void ConfigManagerInternal::Clear() {
  for (auto &kv : props_) {
    std::string key = kv.first;
    std::string value = kv.second;
    Remove(key, value);
  }

  props_.clear();
}


void ConfigManagerInternal::Remove(std::string key, std::string value) {
  CfgLogUtils::LogInfo(StringPrint("ConfigManager : remove %s", PropertyItem(key, value).ToString()));
  // 通知监听器移除配置
  for (std::shared_ptr<ConfigListener> listener : listeners_) {
    PropertyItem item(key, value);
    listener->EventReceived(item, ChangeEventType::ITEM_REMOVED);
  }
}


void ConfigManagerInternal::Put(std::string key, std::string value) {
  if (props_.find(key) == props_.end()) {
    // add
    props_[key] = value;

    CfgLogUtils::LogInfo(StringPrint("ConfigManager : add %s", PropertyItem(key, value).ToString()));
    // 通知监听器添加配置
    for (std::shared_ptr<ConfigListener> listener : listeners_) {
      PropertyItem item(key, value);
      listener->EventReceived(item, ChangeEventType::ITEM_ADDED);
    }
  } else {
    std::string pre_value = props_[key];
    if (!CfgStringUtils::AreStringsEqual(pre_value, value)) {
      // update
      props_[key] = value;

      CfgLogUtils::LogInfo(StringPrint("ConfigManager : Key %s change from %s to %s", key, pre_value, value));
      for (std::shared_ptr<ConfigListener> listener : listeners_) {
        PropertyItem item(key, value);
        listener->EventReceived(item, ChangeEventType::ITEM_UPDATED);
      }
    } else {
      // no change
    }
  }
}

std::map<std::string, std::string> ConfigManagerInternal::GetProps(){
    return props_;
}

bool ConfigManagerInternal::Get(const std::string &key, std::string *ret) {
  if (props_.find(key) == props_.end()) return false;
  *ret = props_[key];
  return true;
}


bool ConfigManagerInternal::GetString(const std::string &key, std::string *ret) {
  if (props_.find(key) == props_.end()) return false;
  *ret = props_[key];
  return true;
}


bool ConfigManagerInternal::GetInteger(const std::string &key, int *ret) {
  if (props_.find(key) == props_.end()) return false;

  int num;
  bool t = StringToNumber(props_[key], &num);

  if (!t) return false;

  *ret = num;
  return true;
}


bool ConfigManagerInternal::GetLong(const std::string &key, long *ret) {
  if (props_.find(key) == props_.end()) return false;
  long num;
  bool t = StringToNumber(props_[key], &num);

  if (!t) return false;

  *ret = num;
  return true;
}


bool ConfigManagerInternal::GetShort(const std::string &key, short *ret) {
  if (props_.find(key) == props_.end()) return false;

  short num;
  bool t = StringToNumber(props_[key], &num);

  if (!t) return false;

  *ret = num;
  return true;
}


bool ConfigManagerInternal::GetBoolean(const std::string &key, bool *ret) {
  if (props_.find(key) == props_.end()) return false;

  std::string value = props_[key];
  std::transform(value.begin(), value.end(), value.begin(), ::tolower);
  std::stringstream is(value);
  bool b;
  is >> std::boolalpha >> b;

  *ret = b;

  return true;
}


bool ConfigManagerInternal::GetDouble(const std::string &key, double *ret) {
  if (props_.find(key) == props_.end()) return false;

  double num;
  bool t = StringToNumber(props_[key], &num);

  if (!t) return false;

  *ret = num;
  return true;
}


bool ConfigManagerInternal::GetFloat(const std::string &key, float *ret) {
  if (props_.find(key) == props_.end()) return false;

  float num;
  bool t = StringToNumber(props_[key], &num);

  if (!t) return false;

  *ret = num;
  return true;
}


std::set<std::string> ConfigManagerInternal::Keys() {
  std::set<std::string> keys;
  for (auto &kv : props_) {
    std::string key = kv.first;
    keys.insert(key);
  }
  return keys;
}

std::string ConfigManagerInternal::app_name() {
  return ConfigLoaderFactory::Instance()->app_name();
}

std::string ConfigManagerInternal::app_env() {
  return ConfigLoaderFactory::Instance()->app_env();
}

std::string ConfigManagerInternal::config_version() {
  return ConfigLoaderFactory::Instance()->config_version();
}

std::string ConfigManagerInternal::app_loc() {
  return ConfigLoaderFactory::Instance()->app_loc();
}

std::string ConfigManagerInternal::config_host() {
  return ConfigLoaderFactory::Instance()->config_host();
}

std::string ConfigManagerInternal::config_agent() {
  return ConfigLoaderFactory::Instance()->config_agent();
}

std::string ConfigManagerInternal::config_monitor() {
  return ConfigLoaderFactory::Instance()->config_monitor();
}
