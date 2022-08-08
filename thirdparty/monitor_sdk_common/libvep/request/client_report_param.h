//
// Created by 吴婷 on 2019-12-10.
//

#ifndef COMMON_LIBVEP_CLIENT_REPORT_PARAM_H
#define COMMON_LIBVEP_CLIENT_REPORT_PARAM_H

#include <vector>

#include "thirdparty/monitor_sdk_common/libvep/util/utils.h"
#include "thirdparty/jsoncpp/json.h"

namespace common {
/**
 * 实验上报信息 V1.0
 */
class LabReportParam {
public:
    LabReportParam() {
    }

    LabReportParam(int lab_id, const std::string &lab_name, long update_time) :
        lab_id(lab_id), lab_name(lab_name), update_time(update_time){}

public:
    int lab_id;              //实验id
    std::string lab_name;    //实验名
    long update_time;        //更新时间
};

/**
 * 用于客户信息上传,服务端进行统计跟踪 V1.0
 */
class ClientReportParam {
public:
    ClientReportParam(){}

    ClientReportParam(const std::string &app_name, const std::string &scene_code, const std::string &client_version, const std::string &client_ip,
                      long last_accumulate_updated_time, long request_timestamp, long last_watched_changing_time,
                      long last_fetched_changing_time, std::vector <LabReportParam> lab_report_params):
        app_name(app_name), scene_code(scene_code), client_version(client_version),
        client_ip(client_ip), last_accumulate_updated_time(last_accumulate_updated_time), request_timestamp(request_timestamp),
        last_watched_changing_time(last_watched_changing_time), last_fetched_changing_time(last_fetched_changing_time), lab_report_params(lab_report_params){
    }

    const std::string ToString() const {
        Json::Value root;
        root["appName"] = app_name;
        root["clientIp"] = client_ip;
        root["clientVersion"] = client_version;
        root["lastAccumulateUpdatedTime"] = last_accumulate_updated_time;
        root["lastFetchedChangingTime"] = last_fetched_changing_time;
        root["lastWatchedChangingTime"] = last_watched_changing_time;
        root["requestTimestamp"] = request_timestamp;
        root["sceneCode"] = scene_code;

        //嵌套的map
        Json::Value params_root;
        int i = 0;
        for (auto lab_report_param : lab_report_params){
            Json::Value lab_root;
            lab_root["labId"] = lab_report_param.lab_id;
            lab_root["labName"] = lab_report_param.lab_name;
            lab_root["updateTime"] = lab_report_param.update_time;
            //填充到根
            params_root[i] = lab_root;
            i++;
        }
        root["labReportParams"] = params_root;

        return VepJsonUtils::ParseString(root);
    }

public:
    std::string app_name;       //嵌入的应用名称
    std::string scene_code;     //场景code
    std::string client_version; //客户端版本
    std::string client_ip;      //客户端机器IP
    long last_accumulate_updated_time = -1l;  //客户端当前的累计更新时间
    long request_timestamp;     //当前服务器时间
    long last_watched_changing_time;    //长轮询监听感知到的变更时间
    long last_fetched_changing_time;    //最后更新时间
    std::vector <LabReportParam> lab_report_params;  //实验上报参数信息
};

}//namespace common

#endif //COMMON_LIBVEP_CLIENT_REPORT_PARAM_H
