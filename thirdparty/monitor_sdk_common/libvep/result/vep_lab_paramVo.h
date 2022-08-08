//
// Created by 吴婷 on 2019-12-10.
//

#ifndef COMMON_LIBVEP_VEP_LAB_PARAMVO_H
#define COMMON_LIBVEP_VEP_LAB_PARAMVO_H

#include <tuple>

#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/libvep/util/utils.h"

namespace common {
/**
 * V1.0场景请求返回实验信息类
 */
class VepLabParamVo {
public:
    const std::string ToString() const {
        Json::Value empty_root;  //空值
        if(lab_id == -1){         //空类，直接返回null
            return empty_root.toStyledString();
        }

        Json::Value root;
        root["shunt"] = shunt;
        root["shunt_tag"] = shunt_tag;
        root["hashShunt"] = hash_shunt;
        root["moduleId"] = module_id;
        root["moduleCode"] = module_code;
        root["sceneId"] = scene_id;
        root["sceneCode"] = scene_code;
        root["labId"] = lab_id;
        root["labName"] = lab_name;
        root["factor"] = factor;
        root["labVersionId"] = lab_version_id;
        root["labVersionName"] = lab_version_name;
        root["algName"] = alg_name;
        root["algCode"] = alg_code;
        root["baselineState"] = baseline_state;
        root["trafficRatio"] = traffic_ratio;
        root["statisticLogMsg"] = statistic_log_msg;

        //嵌套的map
        Json::Value params_root;
        for (auto &param : params) {
            //定义接收变量
            std::string key;
            std::string value;
            std::string type;
            std::tie(key, value, type) = param;

            //解析数据类型
            if(type == "double" || type == "float"){
                double n;
                common::StringToNumber(value, &n);
                params_root[key] = n;
            }
            if(type == "int"){
                int n;
                common::StringToNumber(value, &n);
                params_root[key] = n;
            }
            if(type == "string"){
                params_root[key] = value;
            }
        }
        root["params"] = params_root.isNull() ? empty_root : params_root;

        return VepJsonUtils::ParseStringUtf8(root);
    }

public:
    std::string shunt = "";     //用户分流值,比如imei
    std::string shunt_tag = "";     //用户分流值(圈定字段)
    std::string hash_shunt;      //加盐之后的hash值: hashShunt
    int module_id = 0;           //业务场景Id
    std::string module_code = "";//业务场景Code
    int scene_id = 0;            //业务场景Id
    std::string scene_code = ""; //业务场景Code
    int lab_id = -1;                  //实验Id: labId
    std::string lab_name = "";   //实验Name
    std::string factor = "";    //实验分流因子
    int lab_version_id;           //实验组Id: labVersionId
    std::string lab_version_name = ""; //实验组Name: labVersionName
    std::string alg_name = "";   //参数模板名称: algName
    std::string alg_code = "";   //参数模板Code: algCode
    int baseline_state = 1;      // baselineState 是否基线版本(0：基线；1：非基线)
    int traffic_ratio;           //流量占比: trafficRatio
    std::vector<std::tuple<std::string, std::string, std::string>> params;  //(key, value, type)
    std::string statistic_log_msg = "";   //日志Id信息: statisticLogMsg

};

}//namespace common
#endif //COMMON_LIBVEP_VEP_LAB_PARAMVO_H
