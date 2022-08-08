//
// Created by 吴婷 on 2019-12-09.
//
#include <vector>

#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/libvep/util/utils.h"
#include "thirdparty/monitor_sdk_common/libvep/util/log_utils.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/service/layer/layer_config_service.h"
#include "vep_scene_related_client.h"

using namespace common;

VepSceneRelatedClient::VepSceneRelatedClient(const std::string &scene_code, const std::string &layer_codes) {
    //异常判断
    if(!VepStringUtils::IsStringNotblank(scene_code)){
        LogUtils::LogInfo("scene_code should not be null!");
    }
    if(!VepStringUtils::IsStringNotblank(layer_codes)){
        LogUtils::LogInfo("layer_codes should not be null!");
    }

    //逗号分隔
    std::vector<std::string> layer_set;
    SplitString(layer_codes, ",", &layer_set);    //按,分隔string，将分隔内容填充到向量中

    VepLayerClientDto layer_param(LayerUsingTypeEnum::SCENE_WITH_LAYER, TrafficPositionTypeEnum::SCENE, layer_set, scene_code); //传入信息
    this->layer_param_ = layer_param;
    std::shared_ptr<LayerConfigService> layer_config_service_ptr = std::make_shared<LayerConfigService>(); //配置服务
    this->layer_config_provider_ptr_ = std::make_shared<VepLayerConfigProvider>(layer_param, layer_config_service_ptr);      //provider
}

VepSceneRelatedClient::~VepSceneRelatedClient(){
    close();
}

/**
 * 启动客户端
 * @param is_scheduled 是否定时拉取，默认不填，代表定时拉取
 */
void VepSceneRelatedClient::initialize(bool is_scheduled) {
    //定时拉取，默认
    if(is_scheduled){
        DynamicClientConfig::Instance()->initialize();      //配置中心拉取配置
        layer_config_provider_ptr_->initialize();
    } else {
        //只拉取一次，不定时更新，在gtest时使用
        layer_config_provider_ptr_->initializeForGtest();
    }
}

/**
 * 通过场景+实验层Code查询实验信息
 * @param layer_code 实验层code
 * @param shunt 号段
 * @return
 */
ResultVo<VepLabHitInfo> VepSceneRelatedClient::abValues(const std::string &layer_code, const std::string &shunt) {
    return layer_config_provider_ptr_->abValues(layer_code, layer_param_.scene_code, shunt);
}

/**
 * 按号段分流
 * @param layer_code 实验层code
 * @param shunt 号段
 * @return
 */
ResultVo<VepLabHitInfo> VepSceneRelatedClient::abValuesWithSeg(const std::string &layer_code, const std::string &shunt) {
    return layer_config_provider_ptr_->abValues(layer_code, layer_param_.scene_code, shunt, "", TrafficAllocationTypeEnum::SEGMENT);
}

/**
 * 按圈定字段分流
 * @param layer_code 实验层code
 * @param tag 圈定字段
 * @return
 */
ResultVo<VepLabHitInfo> VepSceneRelatedClient::abValueWithTag(const std::string &layer_code, const std::string &tag) {
    return layer_config_provider_ptr_->abValues(layer_code, layer_param_.scene_code, "", tag, TrafficAllocationTypeEnum::TAG);
}

/**
 *  按号段+圈定字段分流（圈定字段为空）
 * @param layer_code 实验层code
 * @param shunt 号段
 * @return
 */
ResultVo<VepLabHitInfo> VepSceneRelatedClient::abValueWithSegAndDiscardTag(const std::string &layer_code, const std::string &shunt) {
    return layer_config_provider_ptr_->abValues(layer_code, layer_param_.scene_code, shunt, "", TrafficAllocationTypeEnum::TAG_WITH_SEGMENT);
}

/**
 * 按号段+圈定字段分流
 * @param layer_code 实验层code
 * @param shunt 号段
 * @param tag 圈定字段
 * @return
 */
ResultVo<VepLabHitInfo> VepSceneRelatedClient::abValueWithSegAndTag(const std::string &layer_code, const std::string &shunt, const std::string &tag) {
    return layer_config_provider_ptr_->abValues(layer_code, layer_param_.scene_code, shunt, tag, TrafficAllocationTypeEnum::TAG_WITH_SEGMENT);
}

/**
 * 通用接口
 * 1.如果圈定字段为空、IMEI为空，提示输入参数无效
 * 2.如果圈定字段非空，IMEI为空，则走单选圈定字段逻辑
 * 3.如果圈定字段为空、IMEI非空，则走单选IMEI逻辑
 * 4.如果圈定字段非空，IMEI非空，则走圈定字段+号段组合分流
 * @param layer_code
 * @param shunt
 * @param tag
 * @return
 */
ResultVo<VepLabHitInfo> VepSceneRelatedClient::abValues(const std::string &layer_code, const std::string &shunt, const std::string &tag) {
    return layer_config_provider_ptr_->abValues(layer_code, layer_param_.scene_code, shunt, tag, TrafficAllocationTypeEnum::REASONING);
}

/**
 * 关闭客户端
 */
void VepSceneRelatedClient::close() {
    layer_config_provider_ptr_->close();
}
