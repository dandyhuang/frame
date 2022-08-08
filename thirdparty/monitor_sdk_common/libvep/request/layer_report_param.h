
#ifndef COMMON_LIBVEP_REQUEST_LAYER_REPORT_PARAM_H_
#define COMMON_LIBVEP_REQUEST_LAYER_REPORT_PARAM_H_

#include "thirdparty/monitor_sdk_common/libvep/util/utils.h"
#include "thirdparty/monitor_sdk_common/libvep/config/constants.h"

#include "thirdparty/jsoncpp/json.h"

namespace common {
/**
 * 实验层请求信息
 */
class LayerReportParam {
public:
    LayerReportParam() {
    }

    LayerReportParam(const std::string &app_name, const std::string &client_version, const std::string &client_ip,
                     long last_layer_changing_time, long request_timestamp, long last_watched_changing_time,
                     long last_fetched_changing_time):
        app_name(app_name), client_version(client_version), client_ip(client_ip), last_layer_changing_time(last_layer_changing_time),
        request_timestamp(request_timestamp), last_watched_changing_time(last_watched_changing_time), last_fetched_changing_time(last_fetched_changing_time){
    }

    /**
     * 构建实验层请求参数【req_param构造使用】//将build直接变为构造函数
     * @param last_layer_changing_time
     * @param last_watched_changing_time
     * @param last_fetched_changing_time
     * @return
     */
    LayerReportParam(long last_layer_changing_time, long last_watched_changing_time,
                     long last_fetched_changing_time) {   //将build直接变为构造函数
        this->app_name = VepLocalProperties::app_name;
        this->client_version = VepLocalProperties::client_version;
        this->client_ip = VepClientInfoUtils::getLocalIP();             //获取本地ip地址
        this->request_timestamp = VepTimeUtils::GetCurrentTimeMills();  //获取当前时间

        this->last_layer_changing_time = last_layer_changing_time;
        this->last_watched_changing_time = last_watched_changing_time;
        this->last_fetched_changing_time = last_fetched_changing_time;
    }

    const std::string ToString() const {
        Json::Value root;
        root["appName"] = app_name;
        root["clientVersion"] = client_version;
        root["clientIp"] = client_ip;
        root["lastLayerChangingTime"] = last_layer_changing_time;
        root["requestTimestamp"] = request_timestamp;
        root["lastWatchedChangingTime"] = last_watched_changing_time;
        root["lastFetchedChangingTime"] = last_fetched_changing_time;
        return root.toStyledString();
    }


public:
    std::string app_name;       //嵌入的应用名称
    std::string client_version; //客户端版本
    std::string client_ip;      //客户端机器IP
    long last_layer_changing_time = -1l;  //实验层的最新变更时间
    long request_timestamp;     //当前服务器时间
    long last_watched_changing_time = -1l;    //长轮询监听感知到的变更时间
    long last_fetched_changing_time = -1l;    //最后更新时间

};

} //namespace common

#endif //COMMON_LIBVEP_REQUEST_LAYER_REPORT_PARAM_H_
