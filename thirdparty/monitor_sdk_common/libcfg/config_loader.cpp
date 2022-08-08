//
// Created by 黄伟锋 on 2019-04-10.
//

#include <chrono>
#include <ctime>

#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/base/string/format.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_loader.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_logger.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_loader_factory.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_utils.h"
#include "thirdparty/monitor_sdk_common/libcfg/utils.h"
#include "thirdparty/monitor_sdk_common/storage/file/file.h"
#include "thirdparty/monitor_sdk_common/storage/path/path.h"

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/jsoncpp/json.h"

using namespace common;

DEFINE_string(cfg_tmp_dir, "/tmp",
              "dir for the temp file that is used for saving local config");


ConfigLoader::ConfigLoader() {}


ConfigLoader::ConfigLoader(const PropertySource &source): ConfigCoreProcessor(source) {}


ConfigLoader::~ConfigLoader() {}


bool ConfigLoader::Start() {
  if (!InitializeConfigLoader() || !RefreshRemoteProperties() || !RemotePropertiesInitialized()) {
    if (!LoadLocalCache()) return false;
    if (!RemotePropertiesInitialized()) return false;
  }
  return true;
}


bool ConfigLoader::InitializeConfigLoader() {
  return true;
}


bool ConfigLoader::RemotePropertiesInitialized() {
  return true;
}


bool ConfigLoader::RefreshRemoteProperties() {
  return true;
}


bool ConfigLoader::ExportProperties(std::map<std::string, std::string> *ret) {
  *ret = remote_properties_;
  return true;
}


std::string ConfigLoader::Get(const std::string &key) {
  return remote_properties_[key];
}


int ConfigLoader::Put(const std::string &key, const std::string &value) {
  last_load_time_ = CfgTimeUtils::GetCurrentTimeMillis();
  std::string pre_value = this->remote_properties_[key];

  if (!CfgStringUtils::AreStringsEqual(pre_value, value)) {
    CfgLogUtils::LogInfo(StringPrint("ConfigLoader : Key [%s] change from [%s] to [%s]", key, pre_value, value));
    this->remote_properties_[key] = value;
  }

  return 0;
}


int ConfigLoader::ClearAndPutAll(const std::map<std::string, std::string> &new_configs) {
  last_load_time_ = CfgTimeUtils::GetCurrentTimeMillis();
  remote_properties_.clear();
  remote_properties_ = std::map<std::string, std::string>(new_configs);
  return 0;
}


bool ConfigLoader::HandleChangeEvent() {
  if (!SaveLocalCache()) {
    CfgLogUtils::LogError("ConfigLoader handleChangeEvent error");
  }

  ConfigLoaderFactory::Instance()->RefreshRemoteProperties();

  return true;
}


bool ConfigLoader::SaveLocalCache() {
  // 导出配置
  std::map<std::string, std::string> data;
  ExportProperties(&data);

  if (data.size() == 0) return 0;

  // json 序列化
  Json::Value root;
  for (auto& kv : data) {
    root[kv.first] = kv.second;
  }
  std::string data_str = CfgJsonUtils::ParseString(root);

  // 写缓存文件
  std::string file_path = Path::Join(FLAGS_cfg_tmp_dir, cache_file_name());
  scoped_ptr<File> fp(File::Open(file_path, "w"));
  if(!fp){
      CfgLogUtils::LogError(StringPrint("ConfigLoader::SaveLocalCache fail! Maybe inconsistent read and write permissions. file_path:[%s]", file_path));
      return false;
  }

  if (!fp->Write(data_str.c_str(), data_str.size())) return false;
  if (!fp->Flush()) return false;

  return true;
}


bool ConfigLoader::LoadLocalCache() {
  // 读取缓存文件
  std::string file_path = Path::Join(FLAGS_cfg_tmp_dir, cache_file_name());
  std::string data_str;
  File::ReadAll(file_path, &data_str);

  // json 反序列化
  std::map<std::string, std::string> local_map;

  Json::CharReaderBuilder builder;
  scoped_ptr<Json::CharReader> reader(builder.newCharReader());
  Json::Value root;
  if (!reader->parse(data_str.c_str(), data_str.c_str() + data_str.size(), &root, NULL)) return false;


  Json::Value::Members members = root.getMemberNames();
  for (Json::Value::Members::iterator iter_member = members.begin(); iter_member != members.end(); iter_member++) {
    std::string str_key = *iter_member;
    std::string str_val = root[str_key.c_str()].asString();
    local_map[str_key] = str_val;
  }

  // 更新到内存
  ClearAndPutAll(local_map);

  return true;
}


long ConfigLoader::GetLastLoadTime() {
  return last_load_time_;
}


long ConfigLoader::GetLastModified() {
  return last_modified_;
}


void ConfigLoader::SetLastModified(long last_modified) {
  std::unique_lock<std::mutex> lk(last_modified_mutex_);
  last_modified_ = last_modified;
};
