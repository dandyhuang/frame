//
// Created by 吴婷 on 2019-12-10.
//
#include "thirdparty/monitor_sdk_common/libvep/provider/http/http_invoker.h"
#include "thirdparty/monitor_sdk_common/libvep/util/utils.h"
#include "vep_client.h"

using namespace common;

/**
 * 根据场景code和密钥初始化client
 * @param scene_code
 * @param scene_secret
 */
VepClient::VepClient(const std::string &scene_code, const std::string &scene_secret) {
    VepSceneParamVo scene_Param_vo(scene_code, scene_secret);
    this->scene_Param_vo_ = scene_Param_vo;   //场景参数
    std::shared_ptr<VepConfigService> vep_config_service_ptr = std::make_shared<VepConfigService>(scene_Param_vo); //配置服务
    std::shared_ptr<ClientReportService> client_report_service_ptr = std::make_shared<ClientReportService>(scene_Param_vo);  //客户端信息生成器
    this->vep_config_provider_ptr_ = std::make_shared<VepConfigProvider>(vep_config_service_ptr, client_report_service_ptr, scene_Param_vo);      //provider
}

VepClient::VepClient(const std::string &scene_code) {
    VepClient(scene_code, "");
}

VepClient::~VepClient(){
    close();
}

/**
 * 启动客户端
 * @param is_scheduled 是否定时拉取，默认不填，代表定时拉取
 */
void VepClient::initialize(bool is_scheduled) {
    //定时拉取，默认
    if(is_scheduled){
        DynamicClientConfig::Instance()->initialize();      //配置中心拉取配置
        vep_config_provider_ptr_->initialize();
    } else {
        //只拉取一次，不定时更新，在gtest时使用
        vep_config_provider_ptr_->initializeForGtest();
    }
}

/**
 * 获取场景信息
 * @return 场景值对象
 */
std::shared_ptr<VepApiSceneVo> VepClient::getSceneVo() {
    return vep_config_provider_ptr_->getSceneVo();
}

/**
 * 通过实验名称+分流字段，返回匹配到的实验版本信息
 * @param lab_name 实验名称
 * @param shunt 分流字段
 * @return 实验版本信息
 */
ResultVo<VepLabParamVo> VepClient::abValues(const std::string &lab_name, const std::string &shunt) {
    return vep_config_provider_ptr_->abValues(lab_name, shunt, "", TrafficAllocationTypeEnum::SEGMENT);
}

/**
 * 通过分流字段依次匹配场景下的所有实验
 * @param shunt 分流字段
 * @return 所有实验
 */
std::vector<ResultVo<VepLabParamVo>> VepClient::abValues(const std::string &shunt) {
    std::shared_ptr<VepApiSceneVo> scene_vo = getSceneVo();
    std::vector<ResultVo<VepLabParamVo>> result_vo_list;
    if(scene_vo->last_accumulate_updated_time == -1l){
        return result_vo_list;
    }
    //实验列表名
    std::vector<std::string> lab_name_list = scene_vo->lab_name_list;
    if (lab_name_list.size() == 0){
        return result_vo_list;
    }

    for (auto lab_name : lab_name_list){
        ResultVo<VepLabParamVo> result_vo = abValues(lab_name, shunt);
        result_vo_list.push_back(result_vo);
    }
    return result_vo_list;
}

/**
 * 通过实验名称+分流字段seg，返回匹配到的实验版本信息
 * @param lab_name 实验名称
 * @param shunt 分流字段
 * @return 实验版本信息
 */
ResultVo<VepLabParamVo> VepClient::abValuesWithSeg(const std::string &lab_name, const std::string &shunt) {
    return vep_config_provider_ptr_->abValues(lab_name, shunt, "", TrafficAllocationTypeEnum::SEGMENT);
}

/**
 * 通过实验名称+圈定字段tag，返回匹配到的实验版本信息
 * @param lab_name 实验名称
 * @param tag 圈定字段
 * @return 实验版本信息
 */
ResultVo<VepLabParamVo> VepClient::abValueWithTag(const std::string &lab_name, const std::string &tag) {
    return vep_config_provider_ptr_->abValues(lab_name, "", tag, TrafficAllocationTypeEnum::TAG);
}

/**
 * 通过实验名称+（分流字段&圈定字段，忽略圈定字段），返回匹配到的实验版本信息
 * @param lab_name 实验名称
 * @param shunt 分流字段
 * @return 实验版本信息
 */
ResultVo<VepLabParamVo> VepClient::abValueWithSegAndDiscardTag(const std::string &lab_name, const std::string &shunt) {
    return vep_config_provider_ptr_->abValues(lab_name, shunt, "", TrafficAllocationTypeEnum::TAG_WITH_SEGMENT);
}

/**
 * 通过实验名称+（分流字段&圈定字段），返回匹配到的实验版本信息
 * @param lab_name 实验名称
 * @param shunt 分流字段
 * @param tag 圈定字段
 * @return 实验版本信息
 */
ResultVo<VepLabParamVo> VepClient::abValueWithSegAndTag(const std::string &lab_name, const std::string &shunt, const std::string &tag) {
    return vep_config_provider_ptr_->abValues(lab_name, shunt, tag, TrafficAllocationTypeEnum::TAG_WITH_SEGMENT);
}

/**
 * 通用接口
 * 1.如果圈定字段为空、IMEI为空，提示输入参数无效
 * 2.如果圈定字段非空，IMEI为空，则走单选圈定字段逻辑
 * 3.如果圈定字段为空、IMEI非空，则走单选IMEI逻辑
 * 4.如果圈定字段非空，IMEI非空，则走圈定字段+号段组合分流
 * @param lab_name
 * @param shunt
 * @param tag
 * @return
 */
ResultVo<VepLabParamVo> VepClient::abValues(const std::string &lab_name, const std::string &shunt, const std::string &tag) {
    return vep_config_provider_ptr_->abValues(lab_name, shunt, tag, TrafficAllocationTypeEnum::REASONING);
}

/**
 * 关闭客户端
 */
void VepClient::close() {
    vep_config_provider_ptr_->close();
}