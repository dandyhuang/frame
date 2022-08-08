//
// Created by 吴婷 on 2019-11-23.
//

#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/libvep/config/dynamic_client_config.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/service/vep_config_service.h"
#include "thirdparty/monitor_sdk_common/libvep/result/resultVo.h"
#include "thirdparty/monitor_sdk_common/libvep/util/deserialize_utils.h"
#include "thirdparty/monitor_sdk_common/libvep/util/utils.h"
#include "thirdparty/monitor_sdk_common/libvep/util/log_utils.h"
#include "layer_config_service.h"

#include "thirdparty/jsoncpp/json.h"

using namespace common;

LayerConfigService::LayerConfigService() {
    http_invoker_ptr_ = std::make_shared<VepHttpInvoker>();
    libcurl_wrapper_ = std::make_shared<LibcurlWrapper>();
}

LayerConfigService::~LayerConfigService() {
    http_invoker_ptr_ = nullptr;
    libcurl_wrapper_ = nullptr;
}

void LayerConfigService::initialize() {
    init_ibcurl_wrapper();
}

/**
 * 初始化libcurl_wrapper
 */
void LayerConfigService::init_ibcurl_wrapper() {
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
 * 根据实验层的请求参数，返回实验层的流量信息
 * @param layer_req_param 实验层请求参数
 * @return 实验层的流量信息
 */
std::shared_ptr<VepApiLayerTrafficVo> LayerConfigService::getLayerTrafficVo(const VepLayerReqParam &layer_req_param) {
    //1.拼接构造请求url地址
    std::string req_url = StringPrint("%s/vep/layer/layerConfig/%s/%s", VepConfigService::obtainApiServerUrl()
            , layer_req_param.biz_type, layer_req_param.layer_code);
    std::string req_param = layer_req_param.ToString();    //将req_param转换为string格式
    return obtainLayerTrafficVo(req_url, req_param);
}

/**
 * 根据实验层的请求信息，返回launch层的流量信息
 * @param layer_req_param
 * @return launch层的流量信息
 */
std::shared_ptr<VepApiLaunchLayerVo> LayerConfigService::getLaunchLayerList(const VepLayerReqParam &layer_req_param) {
    std::string req_url = StringPrint("%s/vep/layer/launchLayerConfig/%s/%s", VepConfigService::obtainApiServerUrl()
            , layer_req_param.biz_type, layer_req_param.layer_code);
    std::string req_param = layer_req_param.ToString();
    return obtainLaunchLayerList(req_url, req_param);
}

/**
 * 根据post请求返回更新结果
 * @param layer_req_param
 * @return
 */
std::shared_ptr<VepApiLayerFetchVo> LayerConfigService::hasChanged(const VepLayerReqParam &layer_req_param) {
    std::string req_url = StringPrint("%s/vep/v2/layer/watch", VepConfigService::obtainApiServerUrl());
    std::string req_param = layer_req_param.ToString();
    return obtainChangedVal(req_url, req_param);
}

/**
 * 根据实验层的请求参数，返回实验层的流量信息【场景】
 * @param layer_req_param 实验层请求参数
 * @return 实验层的流量信息
 */
std::shared_ptr<VepApiLayerTrafficVo> LayerConfigService::getLayerTrafficVo(const VepLayerSceneReqParam &scene_req_param) {
    //1.拼接构造请求url地址
    std::string req_url = StringPrint("%s/vep/layer/scene/layerConfig/%s/%s", VepConfigService::obtainApiServerUrl()
            , scene_req_param.scene_code, scene_req_param.layer_code);
    std::string req_param = scene_req_param.ToString();
    return obtainLayerTrafficVo(req_url, req_param);
}

/**
 * 根据实验层的请求信息，返回launch层的流量信息【场景】
 * @param layer_req_param
 * @return launch层的流量信息
 */
std::shared_ptr<VepApiLaunchLayerVo> LayerConfigService::getLaunchLayerList(const VepLayerSceneReqParam &scene_req_param) {
    std::string req_url = StringPrint("%s/vep/layer/scene/launchLayerConfig/%s/%s", VepConfigService::obtainApiServerUrl()
            , scene_req_param.scene_code, scene_req_param.layer_code);
    std::string req_param = scene_req_param.ToString();
    return obtainLaunchLayerList(req_url, req_param);
}

/**
 * 根据post请求返回更新结果【场景】
 * @param layer_req_param
 * @return
 */
std::shared_ptr<VepApiLayerFetchVo> LayerConfigService::hasChanged(const VepLayerSceneReqParam &scene_req_param) {
    std::string req_url = StringPrint("%s/vep/v2/scene/watch/%s", VepConfigService::obtainApiServerUrl(), scene_req_param.scene_code);
    std::string req_param = scene_req_param.ToString();
    return obtainChangedVal(req_url, req_param);
}

/**
 * 根据请求url和请求参数，返回launch层的数据类
 * @param req_url
 * @param layer_req_param
 * @return
 */
std::shared_ptr<VepApiLaunchLayerVo> LayerConfigService::obtainLaunchLayerList(const std::string &req_url, const std::string &layer_req_param) {
    std::shared_ptr<VepApiLaunchLayerVo> empty = std::make_shared<VepApiLaunchLayerVo>();  //空类返回
    std::map<std::string, std::string> param_map;  //请求参数map
    param_map.insert({"reportParam", StringTrim(layer_req_param)});

    //在最多3次失败的条件下，进行数据拉取
    for (int i = 0, max_retry_num = acquireMaxRetryNum(); i < max_retry_num; i++) {
        std::string result = http_invoker_ptr_->doHttpGet(req_url, param_map, VepConfigService::obtainRequestTimeoutMilliseconds());
        //LogUtils::LogInfo(StringPrint("obtainLaunchLayerList, reqUrl: %s, result: %s", req_url, result));

        if (!isHttpApiResultValid(result)) {
            LogUtils::LogWarn(StringPrint("LayerConfigService getLayerTrafficVo, reqUrl: %s, result: %s", req_url, result));
            continue;
        }
        //解析http返回结果
        std::string json_str = result;
        Json::CharReaderBuilder builder;
        scoped_ptr<Json::CharReader> reader(builder.newCharReader());
        Json::Value root;
        if (!reader->parse(json_str.c_str(), json_str.c_str() + json_str.size(), &root, NULL)) return empty;
        //结果填充类
        ResultVo<VepApiLaunchLayerVo> vo;
        vo.success = root["success"].asBool();
        vo.code = root["code"].asString();
        vo.msg = root["msg"].asString();

        Json::Value data_root = root["data"];
        std::shared_ptr<VepApiLaunchLayerVo> launch_layer_vo = std::make_shared<VepApiLaunchLayerVo>();  //返回结果类
        VepDeserializeUtils::DeserializeVepApiLaunchLayerVo(data_root, launch_layer_vo);  //直接调用相关类的反序列化

        if (vo.success == false) {
            LogUtils::LogWarn(StringPrint("LayerConfigService getLayerTrafficVo, reqUrl: %s, result: %s", req_url, vo.ToString()));
        }
        return launch_layer_vo;
    }

    return empty;
}

/**
 * 根据请求url和请求参数，返回实验层的数据类
 * 思路：将http获得的json字符串填充到类里面
 * @param reqUrl 请求url
 * @param layer_req_param 请求参数
 * @return 实验层的数据类
 */
std::shared_ptr<VepApiLayerTrafficVo> LayerConfigService::obtainLayerTrafficVo(const std::string &req_url, const std::string &layer_req_param) {
    std::shared_ptr<VepApiLayerTrafficVo> empty = std::make_shared<VepApiLayerTrafficVo>();  //空类返回
    std::map<std::string, std::string> param_map;  //请求参数map
    param_map.insert({"reportParam", StringTrim(layer_req_param)});

    //在最多3次失败的条件下，进行数据拉取
    for (int i = 0, max_retry_num = acquireMaxRetryNum(); i < max_retry_num; i++) {
        std::string result = http_invoker_ptr_->doHttpGet(req_url, param_map, VepConfigService::obtainRequestTimeoutMilliseconds());
        //LogUtils::LogInfo(StringPrint("obtainLayerTrafficVo, reqUrl: %s, result: %s", req_url, result));

        if(!isHttpApiResultValid(result)){
            LogUtils::LogWarn(StringPrint("LayerConfigService getLayerTrafficVo, reqUrl: %s, result: %s", req_url, result));
            continue;
        }
        //解析http返回结果
        std::string json_str = result;
        Json::CharReaderBuilder builder;
        scoped_ptr<Json::CharReader> reader(builder.newCharReader());
        Json::Value root;
        if (!reader->parse(json_str.c_str(), json_str.c_str() + json_str.size(), &root, NULL)) return empty;
        //结果填充类
        ResultVo<VepApiLayerTrafficVo> vo;
        vo.success = root["success"].asBool();
        vo.code = root["code"].asString();
        vo.msg = root["msg"].asString();

        Json::Value data_root = root["data"];
        std::shared_ptr<VepApiLayerTrafficVo> layer_traffic_vo = std::make_shared<VepApiLayerTrafficVo>();  //返回结果类
        VepDeserializeUtils::DeserializeVepApiLayerTrafficVo(data_root, layer_traffic_vo);  //直接调用相关类的反序列化

        if (vo.success == false) {
            LogUtils::LogWarn(StringPrint("LayerConfigService getLayerTrafficVo, reqUrl: %s, result: %s", req_url, vo.ToString()));
        }
        return layer_traffic_vo;
    }

    return empty;
}

/**
 * 根据请求url和请求参数，返回实验层拉取变更信息
 * @param reqUrl
 * @param req_param
 * @return
 */
std::shared_ptr<VepApiLayerFetchVo> LayerConfigService::obtainChangedVal(const std::string &req_url, const std::string &req_param) {
    std::shared_ptr<VepApiLayerFetchVo> empty = std::make_shared<VepApiLayerFetchVo>();  //空类返回

    int32_t http_status_code;
    std::string http_header_data;
    std::string result;
    libcurl_wrapper_->send_post_request(req_url, req_param,
            &http_status_code, &http_header_data, &result);
    //LogUtils::LogInfo(StringPrint("obtainChangedVal http_status_code: %d\n response_data: %s \n url: %s \n req_param: %s", http_status_code, result, req_url, req_param));

    if (isHttpApiResultValid(result)) {
        //解析http返回结果
        std::string json_str = result;
        Json::CharReaderBuilder builder;
        scoped_ptr<Json::CharReader> reader(builder.newCharReader());
        Json::Value root;
        if (!reader->parse(json_str.c_str(), json_str.c_str() + json_str.size(), &root, NULL)) return empty;
        //结果填充
        std::shared_ptr<VepApiLayerFetchVo> layer_traffic_vo = std::make_shared<VepApiLayerFetchVo>();  //返回结果类
        VepDeserializeUtils::DeserializeVepApiLayerFetchVo(root, layer_traffic_vo);
        LogUtils::LogInfo("[obtainChangedVal]，hasChange拉取成功，有更新数据!");
        return layer_traffic_vo;
    }
    LogUtils::LogInfo("[obtainChangedVal]，hasChange更新数据为空!");
    return empty;
}

/**
 * 判断http返回结果是否有效
 * @param result
 * @return
 */
bool LayerConfigService::isHttpApiResultValid(const std::string &result) {
    if (VepStringUtils::IsStringNotblank(result) && StringStartsWith(result, "{") && StringEndsWith(result, "}")) {
        return true;
    }
    return false;
}

/**
 * 获取拉取阈值
 * @return
 */
int LayerConfigService::acquireMaxRetryNum() {
    int max_retry_num = DynamicClientConfig::getFetchFailedCountThreshold();
    return max_retry_num;
}

