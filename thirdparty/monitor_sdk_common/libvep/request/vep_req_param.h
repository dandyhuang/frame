//
// Created by 吴婷 on 2019-12-09.
//

#ifndef COMMON_LIBVEP_VEP_REQ_PARAM_H
#define COMMON_LIBVEP_VEP_REQ_PARAM_H

#include <memory>

#include "thirdparty/monitor_sdk_common/libvep/request/layer_report_param.h"
#include "thirdparty/monitor_sdk_common/libvep/request/client_report_param.h"
#include "thirdparty/monitor_sdk_common/libvep/util/utils.h"

#include "thirdparty/jsoncpp/json.h"

namespace common {
/**
 * 场景请求参数【V1.0】
 */
class VepSceneReqParam{
public:
    VepSceneReqParam(){}

    VepSceneReqParam(const std::string &scene_code, long last_accumulate_updated_time) :
        scene_code(scene_code), last_accumulate_updated_time(last_accumulate_updated_time){}

    VepSceneReqParam(const std::string &scene_code, long last_accumulate_updated_time, ClientReportParam client_report_param) :
        scene_code(scene_code), last_accumulate_updated_time(last_accumulate_updated_time), client_report_param(client_report_param){
    }


    std::string ToString() const {
        Json::Value req_root;
        req_root["sceneCode"] = scene_code;
        req_root["lastAccumulateUpdatedTime"] = last_accumulate_updated_time;

        //client_report_param
        Json::Value root;
        root["appName"] = client_report_param.app_name;
        root["clientIp"] = client_report_param.client_ip;
        root["clientVersion"] = client_report_param.client_version;
        root["lastAccumulateUpdatedTime"] = client_report_param.last_accumulate_updated_time;
        root["lastFetchedChangingTime"] = client_report_param.last_fetched_changing_time;
        root["lastWatchedChangingTime"] = client_report_param.last_watched_changing_time;
        root["requestTimestamp"] = client_report_param.request_timestamp;
        root["sceneCode"] = client_report_param.scene_code;
        //嵌套的map
        Json::Value params_root;
        int i = 0;
        for (auto lab_report_param : client_report_param.lab_report_params){
            Json::Value lab_root;
            lab_root["labId"] = lab_report_param.lab_id;
            lab_root["labName"] = lab_report_param.lab_name;
            lab_root["updateTime"] = lab_report_param.update_time;
            //填充到根
            params_root[i] = lab_root;
            i++;
        }
        root["labReportParams"] = params_root;

        req_root["clientReportParam"] = root;

        return VepJsonUtils::ParseString(req_root);
    }


public:
    std::string scene_code = "";
    long last_accumulate_updated_time = -1l;   //客户端当前的累计更新时间
    ClientReportParam client_report_param;    //客户端上报对象
};

/**
 * 实验层请求参数【V2.0 第三方流量位】
 * 会用于url上报参数
 */
class VepLayerReqParam {
public:
    VepLayerReqParam(const std::string &biz_type, const std::string &layer_code, long last_layer_changing_time,
                     long last_watched_changing_time, long last_fetched_changing_time) {
        this->biz_type = biz_type;
        this->layer_code = layer_code;
        this->last_layer_changing_time = last_layer_changing_time;
        this->report_param_ptr = std::shared_ptr<LayerReportParam>(new LayerReportParam(last_layer_changing_time, last_watched_changing_time, last_fetched_changing_time));
    }

    /**
     * json字符型返回
     * @return
     */
    const std::string ToString() const {
        Json::Value root;
        root["bizType"] = biz_type;
        root["layerCode"] = layer_code;
        root["lastLayerChangingTime"] = last_layer_changing_time;

        //嵌套的map
        Json::Value empty_root;  //空值
        Json::Value params_root;
        params_root["appName"] = report_param_ptr->app_name;
        params_root["clientVersion"] = report_param_ptr->client_version;
        params_root["clientIp"] = report_param_ptr->client_ip;
        params_root["lastLayerChangingTime"] = report_param_ptr->last_layer_changing_time;
        params_root["requestTimestamp"] = report_param_ptr->request_timestamp;
        params_root["lastWatchedChangingTime"] = report_param_ptr->last_watched_changing_time;
        params_root["lastFetchedChangingTime"] = report_param_ptr->last_fetched_changing_time;

        root["reportParam"] = params_root.isNull() ? empty_root : params_root;

        return VepJsonUtils::ParseString(root);
    }

public:
    std::string biz_type = "";  // bizType 业务类型，比如广告、CPD
    std::string layer_code = ""; //实验层Code
    long last_layer_changing_time = -1L; //最新的实验层变更时间
    std::shared_ptr <LayerReportParam> report_param_ptr; //客户端上报对象

};

/**
 * 实验层请求参数【V2.0 场景】
 */
class VepLayerSceneReqParam : public VepLayerReqParam {
public:
    VepLayerSceneReqParam(const std::string &biz_type, const std::string &layer_code
            , long last_layer_changing_time, long last_watched_changing_time, long last_fetched_changing_time
            , const std::string &scene_code) :
            VepLayerReqParam(biz_type, layer_code, last_layer_changing_time, last_watched_changing_time, last_fetched_changing_time){
        this->scene_code = scene_code;
    }

    /**
     * json字符型返回
     * @return
     */
    const std::string ToString() const {
        Json::Value root;
        root["bizType"] = biz_type;
        root["layerCode"] = layer_code;
        root["lastLayerChangingTime"] = last_layer_changing_time;
        root["sceneCode"] = scene_code;

        //嵌套的map
        Json::Value empty_root;  //空值
        Json::Value params_root;
        params_root["appName"] = report_param_ptr->app_name;
        params_root["clientVersion"] = report_param_ptr->client_version;
        params_root["clientIp"] = report_param_ptr->client_ip;
        params_root["lastLayerChangingTime"] = report_param_ptr->last_layer_changing_time;
        params_root["requestTimestamp"] = report_param_ptr->request_timestamp;
        params_root["lastWatchedChangingTime"] = report_param_ptr->last_watched_changing_time;
        params_root["lastFetchedChangingTime"] = report_param_ptr->last_fetched_changing_time;

        root["reportParam"] = params_root.isNull() ? empty_root : params_root;

        return VepJsonUtils::ParseString(root);
    }

public:
    std::string scene_code = "";  //场景Code
};


}//namespace common

#endif //COMMON_LIBVEP_VEP_REQ_PARAM_H
