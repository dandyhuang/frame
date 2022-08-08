//
// Created by 吴婷 on 2019-11-23.
//

#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/libvep/util/utils.h"
#include "thirdparty/monitor_sdk_common/libvep/util/log_utils.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/service/layer/layer_config_service.h"
#include "vep_thirdparty_related_client.h"

using namespace common;

VepThirdPartyRelatedClient::VepThirdPartyRelatedClient(const std::string &position_type, const std::string &layer_codes) {
    //异常判断
    if(!VepStringUtils::IsStringNotblank(position_type)){
        LogUtils::LogInfo("position_type should not be null!");
    }
    if(!VepStringUtils::IsStringNotblank(layer_codes)){
        LogUtils::LogInfo("layer_codes should not be null!");
    }

    //逗号分隔
    std::vector<std::string> layer_set;
    SplitString(layer_codes, ",", &layer_set);    //按,分隔string，将分隔内容填充到向量中

    VepLayerClientDto layer_param(LayerUsingTypeEnum::THIRD_PARTY_WITH_LAYER, position_type, layer_set); //传入信息
    this->layer_param_ = layer_param;
    std::shared_ptr<LayerConfigService> layer_config_service_ptr = std::make_shared<LayerConfigService>(); //配置服务
    this->layer_config_provider_ptr_ = std::make_shared<VepLayerConfigProvider>(layer_param, layer_config_service_ptr);      //provider
}

VepThirdPartyRelatedClient::~VepThirdPartyRelatedClient() {
    close();
}

/**
 * 启动客户端
 * @param is_scheduled 是否定时拉取，默认不填，代表定时拉取
 */
void VepThirdPartyRelatedClient::initialize(bool is_scheduled){
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
 * 通过业务线+实验层Code查询实验信息
 * @param layer_code 实验层名
 * @param traffic_position_id 流量位id
 * @param shunt 号段
 * @return 结果类
 */
ResultVo<VepLabHitInfo> VepThirdPartyRelatedClient::abValues(const std::string &layer_code, const std::string &traffic_position_id, const std::string &shunt) {
    return layer_config_provider_ptr_->abValues(layer_code, traffic_position_id, shunt);
}

/**
 * 按号段分流
 * @param layer_code 实验层名
 * @param traffic_position_id 流量位id
 * @param shunt 号段
 * @return
 */
ResultVo<VepLabHitInfo> VepThirdPartyRelatedClient::abValuesWithSeg(const std::string &layer_code, const std::string &traffic_position_id, const std::string &shunt) {
    return layer_config_provider_ptr_->abValues(layer_code, traffic_position_id, shunt, "", TrafficAllocationTypeEnum::SEGMENT);
}

/**
 * 按圈定字段分流
 * @param layer_code 实验层名
 * @param traffic_position_id 流量位id
 * @param tag 圈定字段
 * @return
 */
ResultVo<VepLabHitInfo> VepThirdPartyRelatedClient::abValueWithTag(const std::string &layer_code, const std::string &traffic_position_id, const std::string &tag) {
    return layer_config_provider_ptr_->abValues(layer_code, traffic_position_id, "", tag, TrafficAllocationTypeEnum::TAG);
}

/**
 * 按号段+圈定字段分流（圈定字段为空）
 * @param layer_code 实验层名
 * @param traffic_position_id 流量位id
 * @param shunt 号段
 * @return
 */
ResultVo<VepLabHitInfo> VepThirdPartyRelatedClient::abValueWithSegAndDiscardTag(const std::string &layer_code, const std::string &traffic_position_id, const std::string &shunt) {
    return layer_config_provider_ptr_->abValues(layer_code, traffic_position_id, shunt, "", TrafficAllocationTypeEnum::TAG_WITH_SEGMENT);
}

/**
 * 按号段+圈定字段分流
 * @param layer_code 实验层名
 * @param traffic_position_id 流量位id
 * @param shunt 号段
 * @param tag 圈定字段
 * @return
 */
ResultVo<VepLabHitInfo> VepThirdPartyRelatedClient::abValueWithSegAndTag(const std::string &layer_code, const std::string & traffic_position_id, const std::string &shunt, const std::string &tag) {
    return layer_config_provider_ptr_->abValues(layer_code, traffic_position_id, shunt, tag, TrafficAllocationTypeEnum::TAG_WITH_SEGMENT);
}

/**
 * 通用接口
 * 1.如果圈定字段为空、IMEI为空，提示输入参数无效
 * 2.如果圈定字段非空，IMEI为空，则走单选圈定字段逻辑
 * 3.如果圈定字段为空、IMEI非空，则走单选IMEI逻辑
 * 4.如果圈定字段非空，IMEI非空，则走圈定字段+号段组合分流
 * @param layer_code  实验层名
 * @param traffic_position_id 流量位id
 * @param shunt 号段
 * @param tag 圈定字段
 * @return
 */
ResultVo<VepLabHitInfo> VepThirdPartyRelatedClient::abValues(const std::string &layer_code, const std::string &traffic_position_id, const std::string &shunt, const std::string &tag) {
    return layer_config_provider_ptr_->abValues(layer_code, traffic_position_id, shunt, tag, TrafficAllocationTypeEnum::REASONING);
}

/**
 * 关闭客户端
 */
void VepThirdPartyRelatedClient::close(){
    layer_config_provider_ptr_->close();
}
