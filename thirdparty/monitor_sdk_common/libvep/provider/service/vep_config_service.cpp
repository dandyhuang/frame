//
// Created by 吴婷 on 2019-11-28.
//
#include <map>

#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/libvep/config/constants.h"
#include "thirdparty/monitor_sdk_common/libvep/config/dynamic_client_config.h"
#include "thirdparty/monitor_sdk_common/libvep/result/resultVo.h"
#include "thirdparty/monitor_sdk_common/libvep/util/deserialize_utils.h"
#include "thirdparty/monitor_sdk_common/libvep/util/utils.h"
#include "thirdparty/monitor_sdk_common/libvep/util/log_utils.h"
#include "vep_config_service.h"

#include "thirdparty/jsoncpp/json.h"

using namespace common;

VepConfigService::VepConfigService(const VepSceneParamVo &scene_param_vo) {
    //异常判断
    if(scene_param_vo.scene_code == ""){
    LogUtils::LogInfo("VepConfigService初始化失败，没有传入VepSceneParamVo");
    }

    http_invoker_ptr_ = std::make_shared<VepHttpInvoker>();
    libcurl_wrapper_ = std::make_shared<LibcurlWrapper>();
    scene_param_vo_ = scene_param_vo;
}

VepConfigService::~VepConfigService() {
    http_invoker_ptr_ = nullptr;
    libcurl_wrapper_ = nullptr;
}

void VepConfigService::initialize() {
    init_ibcurl_wrapper();
}

/**
 * 初始化libcurl_wrapper
 */
void VepConfigService::init_ibcurl_wrapper() {
    libcurl_wrapper_->init();
    libcurl_wrapper_->reset();
    // 设置http请求超时时间，单位毫秒
    libcurl_wrapper_->set_timeout(VepConfigService::obtainRequestTimeoutMilliseconds());
    // 设置报文头
    std::set<std::string> http_headers;
    std::string user_agent = StringPrint("User-Agent:%s", VepHttpUtils::user_agent());
    http_headers.insert(user_agent);
    http_headers.insert(std::string("Content-Type:application/json"));
    http_headers.insert(std::string("Accept:application/json"));
    libcurl_wrapper_->set_headers(http_headers);
}

/**
 * 获取场景值对象
 * @param client_report_param 客户端上报参数
 * @return
 */
std::shared_ptr<VepApiSceneVo> VepConfigService::getSceneVO(const ClientReportParam &client_report_param) {
    std::shared_ptr<VepApiSceneVo> scene_vo = std::make_shared<VepApiSceneVo>(); //返回结果类

    std::string req_url = StringPrint("%s%s", VepConfigService::obtainApiServerUrl(), ConfigConstants::QUERY_SCENE_CONFIG_URL);

    std::map<std::string, std::string> param_map;  //请求参数map
    param_map.insert({HttpConstants::PARAM_SCENE_CODE, scene_param_vo_.scene_code});
    param_map.insert({HttpConstants::PARAM_CLIENT_REPORT, StringTrim(client_report_param.ToString())}); //将参数类转为string

    std::string result = http_invoker_ptr_->doHttpGet(req_url, param_map, VepConfigService::obtainRequestTimeoutMilliseconds());
    if (!VepStringUtils::IsStringNotblank(result)) { //返回结果为空
        LogUtils::LogWarn(StringPrint("VepConfigService getSceneVO error, reqUrl: %s, sceneCode: %s", req_url, scene_param_vo_.scene_code));
        return scene_vo;
    }

    //解析http返回结果
    std::string json_str = result;
    Json::CharReaderBuilder builder;
    scoped_ptr<Json::CharReader> reader(builder.newCharReader());
    Json::Value root;
    if (!reader->parse(json_str.c_str(), json_str.c_str() + json_str.size(), &root, NULL)) {
        return scene_vo;
    }
    //结果填充类
    ResultVo<VepApiSceneVo> vo;
    vo.success = root["success"].asBool();
    vo.code = root["code"].asString();
    vo.msg = root["msg"].asString();

    Json::Value data_root = root["data"];
    VepDeserializeUtils::DeserializeVepApiSceneVo(data_root, scene_vo);  //直接调用相关类的反序列化

    if (vo.success == false) {
        LogUtils::LogWarn(StringPrint("VepConfigService getSceneVO error, reqUrl: %s, result: %s", req_url, vo.ToString()));
    }
    return scene_vo;
}

/**
 * 判断接口是否改变
 * @param req_param
 * @param client_report_param
 * @return
 */
bool VepConfigService::hasChanged(VepSceneReqParam req_param, const ClientReportParam &client_report_param) {
    std::string req_url = StringPrint("%s%s", VepConfigService::obtainApiServerUrl(), ConfigConstants::WATCH_SCENE_CHANGED_URL);
    req_param.client_report_param = client_report_param;
    std::string payload = req_param.ToString();

    libcurl_wrapper_->set_timeout(VepConfigService::obtainLongPollingTimeoutMilliseconds());
    int32_t http_status_code;
    std::string http_header_data;
    std::string result;
    libcurl_wrapper_->send_post_request(req_url, payload,
                                        &http_status_code, &http_header_data, &result);
    //int code = http_invoker_ptr_->doHttpPostStatus(req_url, payload, VepConfigService::obtainLongPollingTimeoutMilliseconds());
    if (http_status_code == ConfigConstants::NOT_MODIFIED) {
        return false;
    } else if(http_status_code == 200) {
        return true;
    }
    return false;
}

/**
 * 获取上报信息
 * @param client_report_param
 * @return
 */
bool VepConfigService::reportClientInfo(const ClientReportParam &client_report_param) {
    std::string req_url = StringPrint("%s%s/%s", VepConfigService::obtainClientReportApiServerUrl(),
            ConfigConstants::UPLOAD_CLIENT_INFO_URL, scene_param_vo_.scene_code);
    std::string payload = client_report_param.ToString();

    int32_t http_status_code;
    std::string http_header_data;
    std::string result;
    libcurl_wrapper_->send_post_request(req_url, payload,
                                        &http_status_code, &http_header_data, &result);
    //std::string result = http_invoker_ptr_->doHttpPost(req_url, payload, VepConfigService::obtainRequestTimeoutMilliseconds());

    //解析http返回结果
    std::string json_str = result;
    Json::CharReaderBuilder builder;
    scoped_ptr<Json::CharReader> reader(builder.newCharReader());
    Json::Value root;
    if (!reader->parse(json_str.c_str(), json_str.c_str() + json_str.size(), &root, NULL)) {
        return false;
    }
    ResultVo<bool> vo;
    vo.success = root["success"].asBool();
    vo.code = root["code"].asString();
    vo.msg = root["msg"].asString();
    vo.data = root["data"].asBool();

    return vo.data;
}

std::string VepConfigService::obtainApiServerUrl() {
    return DynamicClientConfig::Instance()->getVal(DynamicConfigKeyEnum::DEFAULT_API_SERVER_URL.first, DynamicConfigKeyEnum::DEFAULT_API_SERVER_URL.second);
}

std::string VepConfigService::obtainClientReportApiServerUrl() {
    return DynamicClientConfig::Instance()->getVal(DynamicConfigKeyEnum::CLIENT_REPORT_API_SERVER_URL.first, DynamicConfigKeyEnum::CLIENT_REPORT_API_SERVER_URL.second);
}

int VepConfigService::obtainRequestTimeoutMilliseconds() {
    std::string val = DynamicClientConfig::Instance()->getVal(DynamicConfigKeyEnum::REQUEST_TIMEOUT_MILLISECONDS.first, DynamicConfigKeyEnum::REQUEST_TIMEOUT_MILLISECONDS.second);
    int ret;
    StringToNumber(val, &ret); //将字符串类型转换为int
    return ret;
}

int VepConfigService::obtainLongPollingTimeoutMilliseconds() {
    std::string val = DynamicClientConfig::Instance()->getVal(DynamicConfigKeyEnum::LONG_POLLING_TIMEOUT_MILLISECONDS.first, DynamicConfigKeyEnum::LONG_POLLING_TIMEOUT_MILLISECONDS.second);
    int ret;
    StringToNumber(val, &ret);
    return ret;
}