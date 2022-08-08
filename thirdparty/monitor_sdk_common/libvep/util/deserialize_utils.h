//
// Created by 吴婷 on 2019-11-30.
//

#ifndef COMMON_LIBVEP_DESERIALIZEUTILS_H
#define COMMON_LIBVEP_DESERIALIZEUTILS_H

#include <string>
#include <vector>
#include <tuple>

#include "thirdparty/monitor_sdk_common/base/unordered_map.h"
#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/libvep/entity/meta_Dto.h"
#include "thirdparty/monitor_sdk_common/libvep/entity/layer_vo.h"
#include "thirdparty/monitor_sdk_common/libvep/entity/scene_vo.h"
#include "thirdparty/monitor_sdk_common/libvep/entity/vivo_cfg_vo.h"

#include "thirdparty/jsoncpp/json.h"

namespace common {
/**
 * 对api返回的内容反序列化填充到各个类中
 */
class VepDeserializeUtils {
public:
    static bool StringToJson(const std::string &json_str, Json::Value *json);  //将string类型转换为json格式填充

    //1.LabVersionDto
    static bool DeserializeLabVersionDto(const Json::Value &root, std::shared_ptr<LabVersionDto> ret);             //将json格式数据填充到类
    static bool DeserializeLabVersionDto(const std::string &json_str, std::shared_ptr<LabVersionDto> ret);  //将json_str填充到类
    //std::vector<LabVersionDto>
    static bool DeserializeLabVersionDto(const Json::Value &root, std::vector<std::shared_ptr<LabVersionDto> > *ret);
    static bool DeserializeLabVersionDto(const std::string &json_str, std::vector<std::shared_ptr<LabVersionDto> > *ret);
    //std::map<int, LabVersionDto>
    static bool DeserializeLabVersionDto(const Json::Value &root, std::map<int, std::shared_ptr<LabVersionDto> > *ret);
    static bool DeserializeLabVersionDto(const std::string &json_str, std::map<int, std::shared_ptr<LabVersionDto> > *ret);

    //2.VepApiLabWithPositionVo
    static bool DeserializeVepApiLabWithPositionVo(const Json::Value &root, std::shared_ptr<VepApiLabWithPositionVo> ret);             //将json格式数据填充到类
    static bool DeserializeVepApiLabWithPositionVo(const std::string &json_str, std::shared_ptr<VepApiLabWithPositionVo> ret);  //将json_str填充到类
    //std::vector<VepApiLabWithPositionVo>
    static bool DeserializeVepApiLabWithPositionVo(const Json::Value &root, std::vector<std::shared_ptr<VepApiLabWithPositionVo> > *ret);
    static bool DeserializeVepApiLabWithPositionVo(const std::string &json_str, std::vector<std::shared_ptr<VepApiLabWithPositionVo> > *ret);

    //3.VepApiLayerTrafficVo
    static bool DeserializeVepApiLayerTrafficVo(const Json::Value &json, std::shared_ptr<VepApiLayerTrafficVo> ret);             //将json格式数据填充到类
    static bool DeserializeVepApiLayerTrafficVo(const std::string &json_str, std::shared_ptr<VepApiLayerTrafficVo> ret);  //将json_str填充到类

    //RelatedLayerRelevanceChangeDto
    static bool DeserializeRelatedLayerRelevanceChangeDto(const Json::Value &root, std::shared_ptr<RelatedLayerRelevanceChangeDto> ret);
    static bool DeserializeRelatedLayerRelevanceChangeDto(const std::string &json_str, std::shared_ptr<RelatedLayerRelevanceChangeDto> ret);
    //std::vector<RelatedLayerRelevanceChangeDto>
    static bool DeserializeRelatedLayerRelevanceChangeDto(const Json::Value &root, std::vector<std::shared_ptr<RelatedLayerRelevanceChangeDto>> *ret);
    static bool DeserializeRelatedLayerRelevanceChangeDto(const std::string &json_str, std::vector<std::shared_ptr<RelatedLayerRelevanceChangeDto>> *ret);

    //VepApiLayerFetchVo
    static bool DeserializeVepApiLayerFetchVo(const Json::Value &root, std::shared_ptr<VepApiLayerFetchVo> ret);
    static bool DeserializeVepApiLayerFetchVo(const std::string &json_str, std::shared_ptr<VepApiLayerFetchVo> ret);


    //LaunchLayerDto
    static bool DeserializeLaunchLayerDto(const Json::Value &root, std::shared_ptr<LaunchLayerDto> ret);
    static bool DeserializeLaunchLayerDto(const std::string &json_str, std::shared_ptr<LaunchLayerDto> ret);
    //std::vector<LaunchLayerDto>
    static bool DeserializeLaunchLayerDto(const Json::Value &root, std::vector<std::shared_ptr<LaunchLayerDto>> *ret);
    static bool DeserializeLaunchLayerDto(const std::string &json_str, std::vector<std::shared_ptr<LaunchLayerDto>> *ret);

    //VepApiLaunchLayerVo
    static bool DeserializeVepApiLaunchLayerVo(const Json::Value &root, std::shared_ptr<VepApiLaunchLayerVo> ret);
    static bool DeserializeVepApiLaunchLayerVo(const std::string &json_str, std::shared_ptr<VepApiLaunchLayerVo> ret);


    //ThirdRelatedPositionParam
    static bool DeserializeThirdRelatedPositionParam(const Json::Value &root, std::shared_ptr<ThirdRelatedPositionParam> ret);
    static bool DeserializeThirdRelatedPositionParam(const std::string &json_str, std::shared_ptr<ThirdRelatedPositionParam> ret);
    //std::vector<ThirdRelatedPositionParam>
    static bool DeserializeThirdRelatedPositionParam(const Json::Value &root, std::vector<std::shared_ptr<ThirdRelatedPositionParam>> *ret);
    static bool DeserializeThirdRelatedPositionParam(const std::string &json_str, std::vector<std::shared_ptr<ThirdRelatedPositionParam>> *ret);

    //map<std::string, string>  参数map返回
    static std::map<std::string, std::string> DeserializeMapStr(const Json::Value &root);
    static std::map<std::string, std::string> DeserializeMapStr(const std::string &json_str);

    //unordered_map<std::string, int>  参数map返回
    static std::unordered_map<std::string, int> DeserializeMap(const Json::Value &root);
    static std::unordered_map<std::string, int> DeserializeMap(const std::string &json_str);

    //(参数名key, 参数值vaule, 参数类型type)
    static std::vector<std::tuple<std::string, std::string, std::string>> DeserializeParamTuple(const Json::Value &root);
    static std::vector<std::tuple<std::string, std::string, std::string>> DeserializeParamTuple(const std::string &json_str);

    //V1.0反序列化
    //LabDto
    static bool DeserializeLabDto(const Json::Value &root, LabDto *ret);             //将json格式数据填充到类
    static bool DeserializeLabDto(const std::string &json_str, LabDto *ret);  //将json_str填充到类

    //VepApiLabVo
    static bool DeserializeVepApiLabVo(const Json::Value &root, std::shared_ptr<VepApiLabVo> ret);
    static bool DeserializeVepApiLabVo(const std::string &json_str, std::shared_ptr<VepApiLabVo> ret);
    //std::map<std::string, VepApiLabVo>
    static bool DeserializeVepApiLabVo(const Json::Value &root, std::unordered_map<std::string, std::shared_ptr<VepApiLabVo>> *ret);
    static bool DeserializeVepApiLabVo(const std::string &json_str, std::unordered_map<std::string, std::shared_ptr<VepApiLabVo>> *ret);

    //SceneDto
    static bool DeserializeSceneDto(const Json::Value &root, SceneDto *ret);
    static bool DeserializeSceneDto(const std::string &json_str, SceneDto *ret);

    //VepApiSceneVo
    static bool DeserializeVepApiSceneVo(const Json::Value &root, std::shared_ptr<VepApiSceneVo> ret);
    static bool DeserializeVepApiSceneVo(const std::string &json_str, std::shared_ptr<VepApiSceneVo> ret);

    //拉取配置反序列化
    //PropertyItem
    static bool DeserializePropertyItem(const Json::Value &root, PropertyItem *ret);
    static bool DeserializePropertyItem(const std::string &json_str, PropertyItem *ret);
    //std::vector<PropertyItem>
    static bool DeserializePropertyItem(const Json::Value &root, std::vector<PropertyItem> *ret);
    static bool DeserializePropertyItem(const std::string &json_str, std::vector<PropertyItem> *ret);

    //CfgResultData
    static bool DeserializeCfgResultData(const Json::Value &root, CfgResultData *ret);
    static bool DeserializeCfgResultData(const std::string &json_str, CfgResultData *ret);

    //VivoCfgResultVo
    static bool DeserializeVivoCfgResultVo(const Json::Value &root, VivoCfgResultVo *ret);
    static bool DeserializeVivoCfgResultVo(const std::string &json_str, VivoCfgResultVo *ret);

};

}   //namespace common
#endif //COMMON_LIBVEP_DESERIALIZEUTILS_H
