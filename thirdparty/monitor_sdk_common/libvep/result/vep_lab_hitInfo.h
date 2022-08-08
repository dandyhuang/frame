//
// Created by 吴婷 on 2019-11-22.
//

#ifndef COMMON_LIBVEP_MODEL_VEP_LAB_HITINFO_H
#define COMMON_LIBVEP_MODEL_VEP_LAB_HITINFO_H

#include <string>
#include <map>
#include <utility>
#include <tuple>

#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/libvep/util/utils.h"

#include "thirdparty/jsoncpp/json.h"

namespace common {
/**
 * 第三方数据 + 实验层 返回的结果信息
 */
class VepLabHitInfo {

public:
    const std::string ToString() const {
        Json::Value empty_root;  //空值
        if(lab_id == -1 && launch_layer_id == -1){         //空类，直接返回null
            return empty_root.toStyledString();
        }

        Json::Value root;
        root["shunt"] = shunt;
        root["tag"] = tag;
        root["hashShunt"] = hash_shunt;
        root["sceneId"] = scene_id;
        root["sceneCode"] = scene_code;
        root["labId"] = lab_id;
        root["labName"] = lab_name;
        root["factor"] = factor;
        root["labGroupId"] = lab_group_id;
        root["labGroupName"] = lab_group_name;
        root["parameterTemplateName"] = parameter_template_name;
        root["parameterTemplateCode"] = parameter_template_code;
        root["launchLayerId"] = launch_layer_id;

        //嵌套的map
        Json::Value params_root;
        for (auto param : params) {
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
    std::string tag = "";       //用户分流值(圈定字段),比如imei
    std::string hash_shunt;      //加盐之后的hash值
    int scene_id = 0;            //业务场景Id
    std::string scene_code = ""; //业务场景Code
    int lab_id = -1;             //实验Id
    std::string lab_name = "";   //实验Name
    std::string factor = "";    //实验分流因子
    int lab_group_id = -1;        //实验组Id：labVersionId
    std::string lab_group_name = ""; //实验组Name：labVersionName
    std::string parameter_template_name = ""; //参数模板名称: algName
    std::string parameter_template_code = ""; //参数模板Code: algCode
    std::vector<std::tuple<std::string, std::string, std::string>> params;  //(key, value, type)
    int launch_layer_id = -1;     //launchLayerId Launch层信息配置Id

};

}//namespace common

#endif //COMMON_LIBVEP_MODEL_VEP_LAB_HITINFO_H
