//
// Created by 吴婷 on 2019-12-02.
//

#ifndef COMMON_LIBVEP_LAB_DTO_H
#define COMMON_LIBVEP_LAB_DTO_H

#include <map>
#include <string>
#include <utility>
#include <tuple>

#include "thirdparty/monitor_sdk_common/base/string/number.h"

#include "thirdparty/jsoncpp/json.h"

/**
 * 业务场景VO
 */
class SceneDto{
public:
    int id;                 //主键ID
    int module_id;           //业务模块id moduleId
    std::string module_code; //业务模块Code  moduleCode
    std::string name;       //业务场景名称
    std::string code;       //业务场景code
    int state;              //状态：0 正常  -1 删除
    long create_time;        //创建时间
    long update_time;        //更新时间
};

/**
 * 实验VO
 */
class LabDto{
public:
    int id;                 //主键ID
    std::string lab_name;    //实验名称 labName
    std::string description;    //实验描述
    std::string factor;    //实验分流因子
    std::string lab_state;       //实验状态 labState
    std::string lab_type;        //实验类型 labType
    int module_id;               //业务模块code
    int scene_id;                //业务场景Id sceneId
    std::string scene_code;      //业务场景code: sceneCode
    std::string traffic_allocation_type;  // trafficAllocationType 流量分配类型，比如号段、流量占比
    std::string biz_line_name;    //业务线名称
    long create_time;            //创建时间
    long update_time;            //更新时间
    int hash_encrypt = 1;         // hashEncrypt 是否MD5加密:1：是，0：否
    int hash_switch = 1;         //哈希开关，1：需要哈希，0：不需要
    std::string shunt_field;      //分流字段
    std::string shunt_tag_type;    //圈定字段类型
    int salt_switch;     //是否加盐开关，0：关闭，1：打开
    int salt_location;   //加盐的位置，1:前面，2：后面
};

/**
 * 实验版本VO
 */
class LabVersionDto{
public:
    int id = -1;     //主键ID
    int lab_id = -1;  //实验Id
    std::string version_name;    //实验版本名称 versionName
    std::string description;    //实验版本描述
    std::string whitelist;      //白名单 whitelist
    int baseline_state;          //基线状态，0：对照版本，1：灰度实验版本
    std::string integration_state;   // integrationState 集成测试状态
    std::string lab_variable_name;    //实验变量name
    std::string lab_variable_code;    //实验变量code
    std::string diversion_type;      // diversionType 分流类型，hash_imei
    std::string diversion_strategy;  // diversionStrategy 分流策略，例如尾号0,1,2
    std::string shunt_tag_strategy;  //圈定字段
    int traffic_ratio;               //流量占比
    std::string intervention_strategy;   // interventionStrategy 运营干预策略
    long create_time;                //创建时间
    long update_time;                //更新时间
    std::map<std::string, std::string> params; //参数
    std::string param_type;          //参数类型

    std::vector<std::tuple<std::string, std::string, std::string>> params_with_type;  //合成(key, value, type)
};

/**
 * 实验_传输对象
 * 继承LabDto（实验V0）
 */
class RelatedLabDto : public LabDto {
public:
    std::string module_code;     //业务模块Code
    std::string module_name;     //moduleName 业务模块名称
    std::string scene_name;      // sceneName 业务场景名称
    int version_count;           //实验版本是数量
    int with_third_party;         // withThirdParty 是否关联第三方数据源
    std::string third_biz_type;   // thirdBizType 关联的第三方数据源类型
    std::string layer_code;      //关联的实验层Code
    std::string layer_name;      //关联的实验层Name
};

/**
 * 实验层关联信息变更信息
 */
class RelatedLayerRelevanceChangeDto{
public:
    int id;     //主键Id
    int layer_id;    // layerId 实验层Id
    std::string biz_line_name; //业务线名称
    std::string change_type;     // changeType 变更类型，比如白名单变更、流量变更、参数变更、流量位变更、Launch配置变更
    std::string related_key;     //关联的标识Key
    std::string related_val;     // relatedVal 关联的标识值
    std::string operator_;       //操作人员
    std::string remark;         //备注
    long create_time;            //创建时间
    std::string layer_code;      //实验层Code
    std::string layer_name;      //layerName 实验层Name
};

/**
 * Launch层_传输对象
 */
class LaunchLayerDto{
public:
    int id;         //主键Id
    int layer_id;    //实验层Id
    std::string traffic_position_type; // trafficPositionType 流量位类型，比如广告、CPD、场景
    std::string traffic_position_ids;  // traffic_position_ids 流量位Id
    std::string lab_variable_name;     // labVariableName 实验参数模板name
    std::string lab_variable_code;     // labVariableCode 实验参数模板code
    std::string var_param;            // varParam 自定义参数集
    std::string launch_state;         // launchState 状态，比如待生效，生效中，申请中
    int enable;             //是否有效，1:是，0：否
    std::string creator;     //创建人
    std::string modifier;        //更新人
    long create_time;        //创建时间
    long update_time;        //更新时间
    int approval_id;         // approvalId 审批单Id
    std::string param_type;          //参数类型

    std::vector<std::tuple<std::string, std::string, std::string>> params_with_type;  //合成(key, value, type)
};

#endif //COMMON_LIBVEP_LAB_DTO_H
