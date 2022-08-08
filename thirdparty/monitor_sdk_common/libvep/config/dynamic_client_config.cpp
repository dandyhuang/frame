//
// Created by 吴婷 on 2019-11-26.
//

#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/base/string/string_piece.h"
#include "thirdparty/monitor_sdk_common/libvep/config/constants.h"
#include "thirdparty/monitor_sdk_common/libvep/util/utils.h"
#include "thirdparty/monitor_sdk_common/libvep/util/deserialize_utils.h"
#include "dynamic_client_config.h"
#include "thirdparty/monitor_sdk_common/libvep/util/log_utils.h"

using namespace common;


void DynamicClientConfig::initialize() {
    http_invoker_ptr_ = std::make_shared<VepHttpInvoker>();         //Http请求器

    std::string ultimate_url = DynamicClientConfig::buildUltimateUrl();          //获取拉取配置中心请求地址
    initialize_ = DynamicClientConfig::initializeVivoCfgData(ultimate_url);  //首次拉取配置
}

/**
 * 定时读取配置中心的动态配置信息
 */
void DynamicClientConfig::DynamicClientConfigScheduled() {
    if(initialize_){
        LogUtils::LogInfo("开始配置中心定时拉取配置>>>");
        std::string ultimate_url = DynamicClientConfig::buildUltimateUrl();
        std::map<std::string, std::string> http_ret_map = DynamicClientConfig::handleVivoCfgData(ultimate_url);
        if (!http_ret_map.empty()) {
            for (auto kv : http_ret_map) {
                DynamicClientConfig::Instance()->config_map_[kv.first] = kv.second;
            }
        }
    }
}

/**
 * 构造url地址：拉取配置中心
 * @return
 */
std::string DynamicClientConfig::buildUltimateUrl() {
    //获取启动参数：app.env,app.loc,config.host
    //如果app.env、config.host参数都不为空，则可以去配置中心拉取数据
    std::string app_env = VepPropertySource::app_env;
    std::string app_loc = VepPropertySource::app_loc;
    std::string config_host = VepPropertySource::config_host;

    if(VepStringUtils::IsStringNotblank(app_env) && VepStringUtils::IsStringNotblank(config_host)){
        StringPiece config_hostP(config_host);
        std::string req_url = StringPrint("http://%s%s", config_hostP.substr(0, config_hostP.rfind("/")).as_string(), ConfigConstants::FETCH_VIVO_CFG_URL);
        std::string ultimate_url = StringPrint("%s?appName=%s&appEnv=%s&appLoc=%s&lastModified=%s", req_url
                , ConfigConstants::CLIENT_CONFIG_APP_NAME, app_env, app_loc, IntegerToString(DynamicClientConfig::last_modified_));
        return ultimate_url;
    }
    return "";
}

/**
 * 根据url进行拉取，填充map
 * @param ultimate_url
 * @return
 */
std::map<std::string, std::string> DynamicClientConfig::handleVivoCfgData(const std::string &ultimate_url) {
    std::map<std::string, std::string> http_ret_map;
    std::map<std::string, std::string> param_map;  //请求参数为空
    std::string result = http_invoker_ptr_->doHttpGet(ultimate_url, param_map, ConfigConstants::REQUEST_TIMEOUT_MILLISECONDS); //返回请求结果
    if(!VepStringUtils::IsStringNotblank(result)){
        return http_ret_map;
    }
    //解析http填充到类
    VivoCfgResultVo result_vo;
    VepDeserializeUtils::DeserializeVivoCfgResultVo(result, &result_vo);
    if(result_vo.message != "" && result_vo.retcode == 0){
        CfgResultData data = result_vo.data;
        if(data.last_modified > 0 && data.configs.size() > 0){
            std::vector<PropertyItem> property_item_list = data.configs;
            for(auto propertyItem : property_item_list){
                http_ret_map.insert({propertyItem.name, propertyItem.value});
            }
            last_modified_ = data.last_modified;
        }
    }
    return http_ret_map;
}

/**
 * 首次拉取配置
 * @return
 */
bool DynamicClientConfig::initializeVivoCfgData(const std::string &ultimate_url) {
    if(!VepStringUtils::IsStringNotblank(ultimate_url)){
        return false;
    }
    bool success_flag = false;
    int max_failed_count = ConfigConstants::INITIALIZE_CONFIG_FAILED_COUNT_THRESHOLD;
    for (int i = 0; i < max_failed_count; ++i) {
        std::map<std::string, std::string> http_ret_map = DynamicClientConfig::handleVivoCfgData(ultimate_url);
        if(!http_ret_map.empty()){
            success_flag = true;
            for (auto kv : http_ret_map) {
                config_map_[kv.first] = kv.second;
            }
            break;
        }
    }
    return success_flag;
}

/**
 * 从map中获取配置
 * @param key
 * @param def_val 默认值
 * @return
 */
std::string DynamicClientConfig::getVal(const std::string &key, const std::string &def_val) {
    std::string val;
    auto iter = config_map_.find(key);
    if (iter != config_map_.end()) {   //找到
        val = iter->second;
        if (val != "") {
            return val;
        }
    }
    return def_val;
}

int DynamicClientConfig::getScheduledFetchInterval() {
    std::string val = DynamicClientConfig::Instance()->getVal(DynamicConfigKeyEnum::SCHEDULED_FETCH_INTERVAL.first, DynamicConfigKeyEnum::SCHEDULED_FETCH_INTERVAL.second);
    int n;
    StringToNumber(val, &n);
    return n;
}

int DynamicClientConfig::getScheduledWatchInterval() {
    std::string val = DynamicClientConfig::Instance()->getVal(DynamicConfigKeyEnum::SCHEDULED_WATCH_INTERVAL.first, DynamicConfigKeyEnum::SCHEDULED_WATCH_INTERVAL.second);
    int n;
    StringToNumber(val, &n);
    return n;
}

int DynamicClientConfig::getScheduledReportInterval() {
    std::string val = DynamicClientConfig::Instance()->getVal(DynamicConfigKeyEnum::SCHEDULED_REPORT_INTERVAL.first, DynamicConfigKeyEnum::SCHEDULED_REPORT_INTERVAL.second);
    int n;
    StringToNumber(val, &n);
    return n;
}

int DynamicClientConfig::getFetchFailedCountThreshold() {
    std::string val = DynamicClientConfig::Instance()->getVal(DynamicConfigKeyEnum::FETCH_FAILED_COUNT_THRESHOLD.first, DynamicConfigKeyEnum::FETCH_FAILED_COUNT_THRESHOLD.second);
    int n;
    StringToNumber(val, &n);
    return n;
}
