//
// Created by 吴婷 on 2019-11-30.
//
#include <utility>
#include <tuple>

#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "deserialize_utils.h"
#include "utils.h"

using namespace common;

/**
 * 将string类型转换为json格式填充
 * @param json_str
 * @param json
 * @return json转换是否成功
 */
bool VepDeserializeUtils::StringToJson(const std::string &json_str, Json::Value *json) {
    Json::CharReaderBuilder builder;
    scoped_ptr<Json::CharReader> reader(builder.newCharReader());

    if (!reader->parse(json_str.c_str(), json_str.c_str() + json_str.size(), json, NULL)) {
        return false;
    }
    return true;
}

/**
 * 1.将json_str填充到类
 */

//LabDto
bool VepDeserializeUtils::DeserializeLabDto(const std::string &json_str, LabDto *ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    if(VepDeserializeUtils::DeserializeLabDto(root, ret)) {
        return true;
    }
    return false;
}

//LabVersionDto
bool VepDeserializeUtils::DeserializeLabVersionDto(const std::string &json_str, std::shared_ptr<LabVersionDto> ret) {
    //1.json解析
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    //2.调用json格式填充到类
    if(VepDeserializeUtils::DeserializeLabVersionDto(root, ret)) {
        return true;
    }
    return false;
}
//std::vector<LabVersionDto>
bool VepDeserializeUtils::DeserializeLabVersionDto(const std::string &json_str, std::vector<std::shared_ptr<LabVersionDto> > *ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    //调用json格式填充到类
    if(VepDeserializeUtils::DeserializeLabVersionDto(root, ret)) {
        return true;
    }
    return false;
}
//std::map<int, LabVersionDto>
bool VepDeserializeUtils::DeserializeLabVersionDto(const std::string &json_str, std::map<int, std::shared_ptr<LabVersionDto> > *ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    //调用json格式填充到类
    if(VepDeserializeUtils::DeserializeLabVersionDto(root, ret)) {
        return true;
    }
    return false;
}

//VepApiLabWithPositionVo
bool VepDeserializeUtils::DeserializeVepApiLabWithPositionVo(const std::string &json_str, std::shared_ptr<VepApiLabWithPositionVo> ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    //调用json格式填充到类
    if(VepDeserializeUtils::DeserializeVepApiLabWithPositionVo(root, ret)) {
        return true;
    }
    return false;
}
//std::vector<VepApiLabWithPositionVo>
bool VepDeserializeUtils::DeserializeVepApiLabWithPositionVo(const std::string &json_str, std::vector<std::shared_ptr<VepApiLabWithPositionVo> > *ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    //调用json格式填充到类
    if(VepDeserializeUtils::DeserializeVepApiLabWithPositionVo(root, ret)) {
        return true;
    }
    return false;
}

//VepApiLayerTrafficVo
bool VepDeserializeUtils::DeserializeVepApiLayerTrafficVo(const std::string &json_str, std::shared_ptr<VepApiLayerTrafficVo> ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    if(VepDeserializeUtils::DeserializeVepApiLayerTrafficVo(root, ret)) {
        return true;
    }
    return false;
}

//RelatedLayerRelevanceChangeDto
bool VepDeserializeUtils::DeserializeRelatedLayerRelevanceChangeDto(const std::string &json_str, std::shared_ptr<RelatedLayerRelevanceChangeDto> ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    if(VepDeserializeUtils::DeserializeRelatedLayerRelevanceChangeDto(root, ret)) {
        return true;
    }
    return false;
}

//std::vector<RelatedLayerRelevanceChangeDto>
bool VepDeserializeUtils::DeserializeRelatedLayerRelevanceChangeDto(const std::string &json_str, std::vector<std::shared_ptr<RelatedLayerRelevanceChangeDto>> *ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    if(VepDeserializeUtils::DeserializeRelatedLayerRelevanceChangeDto(root, ret)) {
        return true;
    }
    return false;
}

//VepApiLayerFetchVo
bool VepDeserializeUtils::DeserializeVepApiLayerFetchVo(const std::string &json_str, std::shared_ptr<VepApiLayerFetchVo> ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    if(VepDeserializeUtils::DeserializeVepApiLayerFetchVo(root, ret)) {
        return true;
    }
    return false;
}

//LaunchLayerDto
bool VepDeserializeUtils::DeserializeLaunchLayerDto(const std::string &json_str, std::shared_ptr<LaunchLayerDto> ret){
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    if(VepDeserializeUtils::DeserializeLaunchLayerDto(root, ret)) {
        return true;
    }
    return false;
}

//std::vector<LaunchLayerDto>
bool VepDeserializeUtils::DeserializeLaunchLayerDto(const std::string &json_str, std::vector<std::shared_ptr<LaunchLayerDto>> *ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    if(VepDeserializeUtils::DeserializeLaunchLayerDto(root, ret)) {
        return true;
    }
    return false;
}

//VepApiLaunchLayerVo
bool VepDeserializeUtils::DeserializeVepApiLaunchLayerVo(const std::string &json_str, std::shared_ptr<VepApiLaunchLayerVo> ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    if(VepDeserializeUtils::DeserializeVepApiLaunchLayerVo(root, ret)) {
        return true;
    }
    return false;
}

//ThirdRelatedPositionParam
bool VepDeserializeUtils::DeserializeThirdRelatedPositionParam(const std::string &json_str, std::shared_ptr<ThirdRelatedPositionParam> ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    if(VepDeserializeUtils::DeserializeThirdRelatedPositionParam(root, ret)) {
        return true;
    }
    return false;
}

//std::vector<ThirdRelatedPositionParam>
bool VepDeserializeUtils::DeserializeThirdRelatedPositionParam(const std::string &json_str, std::vector<std::shared_ptr<ThirdRelatedPositionParam>> *ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    if(VepDeserializeUtils::DeserializeThirdRelatedPositionParam(root, ret)) {
        return true;
    }
    return false;
}

//VepApiLabVo
bool VepDeserializeUtils::DeserializeVepApiLabVo(const std::string &json_str, std::shared_ptr<VepApiLabVo> ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    if(VepDeserializeUtils::DeserializeVepApiLabVo(root, ret)) {
        return true;
    }
    return false;
}
//std::unordered_map<std::string, VepApiLabVo>
bool VepDeserializeUtils::DeserializeVepApiLabVo(const std::string &json_str, std::unordered_map<std::string, std::shared_ptr<VepApiLabVo>> *ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    if(VepDeserializeUtils::DeserializeVepApiLabVo(root, ret)) {
        return true;
    }
    return false;
}

//SceneDto
bool VepDeserializeUtils::DeserializeSceneDto(const std::string &json_str, SceneDto *ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    if(VepDeserializeUtils::DeserializeSceneDto(root, ret)) {
        return true;
    }
    return false;
}
//VepApiSceneVo
bool VepDeserializeUtils::DeserializeVepApiSceneVo(const std::string &json_str, std::shared_ptr<VepApiSceneVo> ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    if(VepDeserializeUtils::DeserializeVepApiSceneVo(root, ret)) {
        return true;
    }
    return false;
}

//PropertyItem
bool VepDeserializeUtils::DeserializePropertyItem(const std::string &json_str, PropertyItem *ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    if(VepDeserializeUtils::DeserializePropertyItem(root, ret)) {
        return true;
    }
    return false;
}
//std::vector<PropertyItem>
bool VepDeserializeUtils::DeserializePropertyItem(const std::string &json_str, std::vector<PropertyItem> *ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    if(VepDeserializeUtils::DeserializePropertyItem(root, ret)) {
        return true;
    }
    return false;
}

//CfgResultData
bool VepDeserializeUtils::DeserializeCfgResultData(const std::string &json_str, CfgResultData *ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    if(VepDeserializeUtils::DeserializeCfgResultData(root, ret)) {
        return true;
    }
    return false;
}

//VivoCfgResultVo
bool VepDeserializeUtils::DeserializeVivoCfgResultVo(const std::string &json_str, VivoCfgResultVo *ret) {
    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return false;  //json解析不成功直接退出
    }
    if(VepDeserializeUtils::DeserializeVivoCfgResultVo(root, ret)) {
        return true;
    }
    return false;
}

//mapStr
std::map<std::string, std::string> VepDeserializeUtils::DeserializeMapStr(const std::string &json_str) {
    std::map<std::string, std::string> params;

    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return params;  //json解析不成功
    }
    params = VepDeserializeUtils::DeserializeMapStr(root);
    return params;
}

//mapStr
std::map<std::string, std::string> VepDeserializeUtils::DeserializeMapStr(const Json::Value &root) {
    std::map<std::string, std::string> params;
    //遍历key字段填充参数map
    Json::Value::Members params_members = root.getMemberNames();
    std::string params_key;
    for (Json::Value::Members::iterator iter = params_members.begin(); iter != params_members.end(); iter++) {
        params_key = *iter;
        params[params_key] = root[params_key].asString(); //强制转换为字符串
    }
    return params;
}

//map
std::unordered_map<std::string, int> VepDeserializeUtils::DeserializeMap(const std::string &json_str) {
    std::unordered_map<std::string, int> params;

    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return params;  //json解析不成功
    }
    params = VepDeserializeUtils::DeserializeMap(root);
    return params;
}

//map
std::unordered_map<std::string, int> VepDeserializeUtils::DeserializeMap(const Json::Value &root) {
    std::unordered_map<std::string, int> params;
    //遍历key字段填充参数map
    Json::Value::Members params_members = root.getMemberNames();
    std::string params_key;
    for (Json::Value::Members::iterator iter = params_members.begin(); iter != params_members.end(); iter++) {
        params_key = *iter;
        params[params_key] = root[params_key].asInt(); //强制转换为字符串
    }
    return params;
}

//(参数名key, 参数值vaule, 参数类型type)
std::vector<std::tuple<std::string, std::string, std::string>> VepDeserializeUtils::DeserializeParamTuple(const std::string &json_str) {
    std::vector<std::tuple<std::string, std::string, std::string>> params;

    Json::Value root;
    if(!VepDeserializeUtils::StringToJson(json_str, &root)) {
        return params;  //json解析不成功
    }
    params = VepDeserializeUtils::DeserializeParamTuple(root);
    return params;
}

//(参数名key, 参数值vaule, 参数类型type)
std::vector<std::tuple<std::string, std::string, std::string>> VepDeserializeUtils::DeserializeParamTuple(const Json::Value &root) {
    std::vector<std::tuple<std::string, std::string, std::string>> params;

    Json::Value::Members params_members = root.getMemberNames();  //获取所有key的值
    std::string params_key;
    for (Json::Value::Members::iterator iter = params_members.begin(); iter != params_members.end(); iter++){
        params_key = *iter; //取出key
        //判断value返回值类型
        if(root[params_key].isString()){
            params.push_back(std::make_tuple(params_key, root[params_key].asString(), "string"));
        } else if(root[params_key].isInt()){
            params.push_back(std::make_tuple(params_key, root[params_key].asString(), "int"));
        } else {
            params.push_back(std::make_tuple(params_key, root[params_key].asString(), "double"));
        }
    }
    return params;
}

/**
 * 2.将json格式数据填充到类【http结果解析后用到】
 */
//LabVersionDto
bool VepDeserializeUtils::DeserializeLabVersionDto(const Json::Value &root, std::shared_ptr<LabVersionDto> ret) {
    ret->id = root["id"].asInt();
    ret->lab_id = root["labId"].asInt();
    ret->version_name = root["versionName"].asString();
    ret->description = root["description"].asString();
    ret->whitelist = root["whitelist"].asString();
    ret->baseline_state = root["baselineState"].asInt();
    ret->integration_state = root["integrationState"].asString();
    ret->lab_variable_name = root["labVariableName"].asString();
    ret->lab_variable_code = root["labVariableCode"].asString();
    ret->diversion_type = root["diversionType"].asString();
    ret->diversion_strategy = root["diversionStrategy"].asString();
    ret->shunt_tag_strategy = root["shuntTagStrategy"].asString();
    ret->traffic_ratio = root["trafficRatio"].asInt();
    ret->intervention_strategy = root["interventionStrategy"].asString();
    StringToNumber(root["createTime"].asString(), &ret->create_time);
    StringToNumber(root["updateTime"].asString(), &ret->update_time);

    ret->params = VepDeserializeUtils::DeserializeMapStr(root["params"]);
    ret->param_type = root["paramtype"].asString();
    //将param转换为带参数类型的tuple，再填充
    ret->params_with_type = VepParamsUtils::ParamMapToTuple(ret->params, VepDeserializeUtils::DeserializeMapStr(ret->param_type));

    return true;
}
//std::vector<LabVersionDto>
bool VepDeserializeUtils::DeserializeLabVersionDto(const Json::Value &root, std::vector<std::shared_ptr<LabVersionDto> > *ret) {
    for (unsigned int i = 0; i < root.size(); i++) {
        std::shared_ptr<LabVersionDto> lab_version_dto = std::make_shared<LabVersionDto>();
        VepDeserializeUtils::DeserializeLabVersionDto(root[i], lab_version_dto);
        //将每个填充类放入vector
        ret->push_back(lab_version_dto);
    }
    return true;
}
//std::map<int, LabVersionDto>
bool VepDeserializeUtils::DeserializeLabVersionDto(const Json::Value &root, std::map<int, std::shared_ptr<LabVersionDto> > *ret) {
    Json::Value::Members members = root.getMemberNames();  //获取所有key的值
    for (Json::Value::Members::iterator iter = members.begin(); iter != members.end(); iter++) {
        std::string key = *iter; //取出key
        std::shared_ptr<LabVersionDto> lab_version_dto = std::make_shared<LabVersionDto>();
        VepDeserializeUtils::DeserializeLabVersionDto(root[key], lab_version_dto);
        //将key转换为int类型
        int n;
        StringToNumber(key, &n);
        ret->insert({n, lab_version_dto});
    }
    return true;
}

//VepApiLabWithPositionVo
bool VepDeserializeUtils::DeserializeVepApiLabWithPositionVo(const Json::Value &root, std::shared_ptr<VepApiLabWithPositionVo> ret) {
    ret->id = root["id"].asInt();
    ret->lab_name = root["labName"].asString();
    ret->description = root["description"].asString();
    ret->factor = root["factor"].asString();
    ret->lab_state = root["labState"].asString();
    ret->lab_type = root["labType"].asString();
    ret->module_id = root["moduleId"].asInt();
    ret->scene_id = root["sceneId"].asInt();
    ret->scene_code = root["sceneCode"].asString();
    ret->traffic_allocation_type = root["trafficAllocationType"].asString();
    ret->biz_line_name = root["bizLineName"].asString();
    StringToNumber(root["createTime"].asString(), &ret->create_time);  //将string转换为long类型
    StringToNumber(root["updateTime"].asString(), &ret->update_time);
    ret->hash_encrypt = root["hashEncrypt"].asInt();
    ret->hash_switch = root["hashSwitch"].asInt();
    //RelatedLabDto
    ret->module_code = root["moduleCode"].asString();
    ret->module_name = root["moduleName"].asString();
    ret->scene_name = root["sceneName"].asString();
    ret->version_count = root["versionCount"].asInt();
    ret->with_third_party = root["withThirdParty"].asInt();
    ret->third_biz_type = root["thirdBizType"].asString();
    ret->layer_code = root["layerCode"].asString();
    ret->layer_name = root["layerName"].asString();
    //version_list   实验版本列表
    VepDeserializeUtils::DeserializeLabVersionDto(root["versionList"], &(ret->version_list));
    //traffic_positions
    for (const Json::Value &item : root["trafficPositions"]) {
        ret->traffic_positions.push_back(item.asString());
    }

    return true;
}

//std::vector<VepApiLabWithPositionVo>
bool VepDeserializeUtils::DeserializeVepApiLabWithPositionVo(const Json::Value &root, std::vector<std::shared_ptr<VepApiLabWithPositionVo> > *ret) {
    for (unsigned int i = 0; i < root.size(); i++) {
        std::shared_ptr<VepApiLabWithPositionVo> vepApiLabWithPositionVo = std::make_shared<VepApiLabWithPositionVo>();
        VepDeserializeUtils::DeserializeVepApiLabWithPositionVo(root[i], vepApiLabWithPositionVo);
        //将每个填充类放入vector
        ret->push_back(vepApiLabWithPositionVo);
    }
    return true;
}

//VepApiLayerTrafficVo
bool VepDeserializeUtils::DeserializeVepApiLayerTrafficVo(const Json::Value &root, std::shared_ptr<VepApiLayerTrafficVo> ret) {
    //添加属性：VepApiLayerVo 实验层信息
    Json::Value layer_root = root["layer"];

    ret->layer.id = layer_root["id"].asInt();
    ret->layer.code = layer_root["code"].asString();
    ret->layer.name = layer_root["name"].asString();
    ret->layer.biz_line_name = layer_root["bizLineName"].asString();
    ret->layer.shunt_field = layer_root["shuntField"].asString();
    ret->layer.shunt_tag_type = layer_root["shuntTagType"].asString();
    ret->layer.salt_switch = layer_root["saltSwitch"].asInt();
    ret->layer.salt_location = layer_root["saltLocation"].asInt();
    ret->layer.salt = layer_root["salt"].asString();
    ret->layer.encrypt_switch = layer_root["encryptSwitch"].asInt();
    ret->layer.hash_switch = layer_root["hashSwitch"].asInt();
    ret->layer.creator = layer_root["creator"].asString();
    ret->layer.modifier = layer_root["modifier"].asString();
    StringToNumber(layer_root["createTime"].asString(), &ret->layer.create_time);
    StringToNumber(layer_root["updateTime"].asString(), &ret->layer.update_time);
    ret->layer.enable = layer_root["enable"].asInt();
    //实验列表数据
    VepDeserializeUtils::DeserializeVepApiLabWithPositionVo(root["labList"], &(ret->lab_list));  //添加一系列实验流量层
    //最新的实验层变更时间
    StringToNumber(root["lastLayerChangingTime"].asString(), &ret->last_layer_changing_time);

    return true;
}

//RelatedLayerRelevanceChangeDto
bool VepDeserializeUtils::DeserializeRelatedLayerRelevanceChangeDto(const Json::Value &root, std::shared_ptr<RelatedLayerRelevanceChangeDto> ret) {
    ret->id = root["id"].asInt();
    ret->layer_id = root["layerId"].asInt();
    ret->biz_line_name = root["bizLineName"].asString();
    ret->change_type = root["changeType"].asString();
    ret->related_key = root["relatedKey"].asString();
    ret->related_val = root["relatedVal"].asString();
    ret->operator_ = root["operator"].asString();
    ret->remark = root["remark"].asString();
    ret->layer_code = root["layerCode"].asString();
    ret->layer_name = root["layerName"].asString();
    StringToNumber(root["createTime"].asString(), &ret->create_time);

    return true;
}
//std::vector<RelatedLayerRelevanceChangeDto>
bool VepDeserializeUtils::DeserializeRelatedLayerRelevanceChangeDto(const Json::Value &root, std::vector<std::shared_ptr<RelatedLayerRelevanceChangeDto>> *ret) {
    for (unsigned int i = 0; i < root.size(); i++) {
        std::shared_ptr<RelatedLayerRelevanceChangeDto> relatedLayerRelevanceChangeDto = std::make_shared<RelatedLayerRelevanceChangeDto>();
        VepDeserializeUtils::DeserializeRelatedLayerRelevanceChangeDto(root[i], relatedLayerRelevanceChangeDto);
        //将每个填充类放入vector
        ret->push_back(relatedLayerRelevanceChangeDto);
    }
    return true;
}

//VepApiLayerFetchVo
bool VepDeserializeUtils::DeserializeVepApiLayerFetchVo(const Json::Value &root, std::shared_ptr<VepApiLayerFetchVo> ret) {
    //实验层关联信息变更信息向量
    VepDeserializeUtils::DeserializeRelatedLayerRelevanceChangeDto(root["changeList"], &ret->change_list);
    //拉取类型
    ret->fetch_type = root["fetchType"].asString();
    return true;
}

//LaunchLayerDto
bool VepDeserializeUtils::DeserializeLaunchLayerDto(const Json::Value &root, std::shared_ptr<LaunchLayerDto> ret) {
    ret->id = root["id"].asInt();
    ret->layer_id = root["layerId"].asInt();
    ret->traffic_position_type = root["trafficPositionType"].asString();
    ret->traffic_position_ids = root["trafficPositionIds"].asString();
    ret->lab_variable_name = root["labVariableName"].asString();
    ret->lab_variable_code = root["labVariableCode"].asString();  //
    ret->var_param = root["varParam"].asString();
    ret->launch_state = root["launchState"].asString();
    ret->enable = root["enable"].asInt();
    ret->creator = root["creator"].asString();
    ret->modifier = root["modifier"].asString();
    StringToNumber(root["createTime"].asString(), &ret->create_time);
    StringToNumber(root["updateTime"].asString(), &ret->update_time);
    ret->approval_id = root["approvalId"].asInt();
    ret->param_type = root["paramtype"].asString();
    //将param转换为带参数类型的tuple，再填充
    ret->params_with_type = VepParamsUtils::ParamMapToTuple(VepDeserializeUtils::DeserializeMapStr(ret->var_param),
                                                            VepDeserializeUtils::DeserializeMapStr(ret->param_type));
    return true;
}

//std::vector<LaunchLayerDto>
bool VepDeserializeUtils::DeserializeLaunchLayerDto(const Json::Value &root, std::vector<std::shared_ptr<LaunchLayerDto>> *ret) {
    for (unsigned int i = 0; i < root.size(); i++) {
        std::shared_ptr<LaunchLayerDto> launch_layer_dto = std::make_shared<LaunchLayerDto>();
        VepDeserializeUtils::DeserializeLaunchLayerDto(root[i], launch_layer_dto);
        //将每个填充类放入vector
        ret->push_back(launch_layer_dto);
    }
    return true;
}

//VepApiLaunchLayerVo
bool VepDeserializeUtils::DeserializeVepApiLaunchLayerVo(const Json::Value &root, std::shared_ptr<VepApiLaunchLayerVo> ret) {
    //实验层code
    ret->layer_code = root["layerCode"].asString();
    //launch层拉取信息
    VepDeserializeUtils::DeserializeLaunchLayerDto(root["launchLayerList"], &(ret->launch_layer_list));
    //拉取类型
    StringToNumber(root["lastLayerChangingTime"].asString(), &ret->last_layer_changing_time);

    return true;
}

//ThirdRelatedPositionParam
bool VepDeserializeUtils::DeserializeThirdRelatedPositionParam(const Json::Value &root, std::shared_ptr<ThirdRelatedPositionParam> ret) {
    ret->related_key = root["relatedKey"].asString();
    ret->related_name = root["relatedName"].asString();
    return true;
}

//std::vector<ThirdRelatedPositionParam>
bool VepDeserializeUtils::DeserializeThirdRelatedPositionParam(const Json::Value &root, std::vector<std::shared_ptr<ThirdRelatedPositionParam>> *ret) {
    for (unsigned int i = 0; i < root.size(); i++) {
        std::shared_ptr<ThirdRelatedPositionParam> thirdRelatedPositionParam = std::make_shared<ThirdRelatedPositionParam>();
        VepDeserializeUtils::DeserializeThirdRelatedPositionParam(root[i], thirdRelatedPositionParam);
        //将每个填充类放入vector
        ret->push_back(thirdRelatedPositionParam);
    }
    return true;
}

//LabDto
bool VepDeserializeUtils::DeserializeLabDto(const Json::Value &root, LabDto *ret) {
    ret->id = root["id"].asInt();
    ret->lab_name = root["labName"].asString();
    ret->description = root["description"].asString();
    ret->factor = root["factor"].asString();
    ret->lab_state = root["labState"].asString();
    ret->lab_type = root["labType"].asString();
    ret->module_id = root["moduleId"].asInt();
    ret->scene_id = root["sceneId"].asInt();
    ret->scene_code = root["sceneCode"].asString();
    ret->traffic_allocation_type = root["trafficAllocationType"].asString();
    ret->biz_line_name = root["bizLineName"].asString();
    StringToNumber(root["createTime"].asString(), &ret->create_time);  //将string转换为long类型
    StringToNumber(root["updateTime"].asString(), &ret->update_time);
    ret->hash_encrypt = root["hashEncrypt"].asInt();
    ret->hash_switch = root["hashSwitch"].asInt();
    ret->shunt_field = root["shuntField"].asString();
    ret->shunt_tag_type = root["shuntTagType"].asString();
    ret->salt_switch = root["saltSwitch"].asInt();
    ret->salt_location = root["saltLocation"].asInt();

    return true;
}

//VepApiLabVo
bool VepDeserializeUtils::DeserializeVepApiLabVo(const Json::Value &root, std::shared_ptr<VepApiLabVo> ret) {
    //填充实验Vo
    VepDeserializeUtils::DeserializeLabDto(root["lab"], &ret->lab);
    //哈希匹配长度
    ret->hash_match_length = root["hashMatchLength"].asInt();
    //实验版本列表id
    for (const Json::Value &item : root["versionIdList"]) {
        ret->version_id_list.push_back(item.asInt());
    }
    //填充白名单
    Json::Value white_root = root["whitelistMap"];
    ret->whitelist_map = VepDeserializeUtils::DeserializeMap(white_root);
    //填充号段名单
    Json::Value segment_root = root["segmentMap"];
    ret->segment_map = VepDeserializeUtils::DeserializeMap(segment_root);
    //实验版本map
    std::map<int, std::shared_ptr<LabVersionDto> > version_map;
    VepDeserializeUtils::DeserializeLabVersionDto(root["versionMap"], &version_map);
    for(auto it = version_map.begin(); it != version_map.end(); it++) {
        ret->version_map.insert({it->first, it->second});
    }
    return true;
}
//std::map<std::string, VepApiLabVo>
bool VepDeserializeUtils::DeserializeVepApiLabVo(const Json::Value &root, std::unordered_map<std::string, std::shared_ptr<VepApiLabVo>> *ret) {

    Json::Value::Members members = root.getMemberNames();  //获取所有key的值
    for (Json::Value::Members::iterator iter = members.begin(); iter != members.end(); iter++) {
        std::string key = *iter; //取出key

        std::shared_ptr<VepApiLabVo> lab_vo = std::make_shared<VepApiLabVo>();
        VepDeserializeUtils::DeserializeVepApiLabVo(root[key], lab_vo);

        ret->insert({key, lab_vo});
    }
    return true;
}

//SceneDto
bool VepDeserializeUtils::DeserializeSceneDto(const Json::Value &root, SceneDto *ret) {
    ret->id = root["id"].asInt();
    ret->module_id = root["moduleId"].asInt();
    ret->module_code = root["moduleCode"].asString();
    ret->name = root["name"].asString();
    ret->code = root["code"].asString();
    ret->state = root["state"].asInt();
    StringToNumber(root["createTime"].asString(), &ret->create_time);
    StringToNumber(root["updateTime"].asString(), &ret->update_time);

    return true;
}

//VepApiSceneVo
bool VepDeserializeUtils::DeserializeVepApiSceneVo(const Json::Value &root, std::shared_ptr<VepApiSceneVo> ret) {
    //填充场景对象
    VepDeserializeUtils::DeserializeSceneDto(root["scene"], &ret->scene);
    //labNameList
    for (const Json::Value &item : root["labNameList"]) {
        ret->lab_name_list.push_back(item.asString());
    }
    //lab_map {实验名：实验值对象}
    VepDeserializeUtils::DeserializeVepApiLabVo(root["labMap"], &ret->lab_map);
    //最近更新时间
    StringToNumber(root["lastAccumulateUpdatedTime"].asString(), &ret->last_accumulate_updated_time);
    return true;
}

//PropertyItem
bool VepDeserializeUtils::DeserializePropertyItem(const Json::Value &root, PropertyItem *ret) {
    ret->name = root["name"].asString();
    ret->value = root["value"].asString();
    return true;
}

//std::vector<PropertyItem>
bool VepDeserializeUtils::DeserializePropertyItem(const Json::Value &root, std::vector<PropertyItem> *ret) {
    for (unsigned int i = 0; i < root.size(); i++) {
        PropertyItem item;
        VepDeserializeUtils::DeserializePropertyItem(root[i], &item);
        //将每个填充类放入vector
        ret->push_back(item);
    }
    return true;
}

//CfgResultData
bool VepDeserializeUtils::DeserializeCfgResultData(const Json::Value &root, CfgResultData *ret) {
    StringToNumber(root["lastModified"].asString(), &ret->last_modified);
    StringToNumber(root["requestTimestamp"].asString(), &ret->request_timestamp);
    StringToNumber(root["responseTimestamp"].asString(), &ret->response_timestamp);
    ret->real_config_version = root["realConfigVersion"].asString();
    //std::vector<PropertyItem> configs;
    VepDeserializeUtils::DeserializePropertyItem(root["configs"], &ret->configs);

    return true;
}

//VivoCfgResultVo
bool VepDeserializeUtils::DeserializeVivoCfgResultVo(const Json::Value &root, VivoCfgResultVo *ret) {
    ret->retcode = root["retcode"].asInt();
    ret->message = root["message"].asString();
    //CfgResultData
    VepDeserializeUtils::DeserializeCfgResultData(root["data"], &ret->data);
    
    return true;
}
