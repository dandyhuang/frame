//
// Created by 吴婷 on 2019-12-09.
//

#ifndef COMMON_LIBVEP_VEP_SCENE_RELATED_CLIENT_H
#define COMMON_LIBVEP_VEP_SCENE_RELATED_CLIENT_H

#include <memory>
#include <string>

#include "thirdparty/monitor_sdk_common/libvep/entity/model_Dto.h"
#include "thirdparty/monitor_sdk_common/libvep/config/constants.h"
#include "thirdparty/monitor_sdk_common/libvep/config/dynamic_client_config.h"
#include "thirdparty/monitor_sdk_common/libvep/result/resultVo.h"
#include "thirdparty/monitor_sdk_common/libvep/result/vep_lab_hitInfo.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/vep_layer_config_provider.h"

namespace common {
/**
 * 场景不关联第三方数据源+实验层_SDK
 */
class VepSceneRelatedClient {
public:
    VepSceneRelatedClient(const std::string &scene_code, const std::string &layer_codes);
    ~VepSceneRelatedClient();

    void initialize(bool is_scheduled = true);                          //启动客户端
    void close();       //关闭客户端

    //结果返回
    ResultVo<VepLabHitInfo> abValues(const std::string &layer_code, const std::string &shunt);

    ResultVo<VepLabHitInfo> abValuesWithSeg(const std::string &layer_code, const std::string &shunt);
    ResultVo<VepLabHitInfo> abValueWithTag(const std::string &layer_code, const std::string &tag);
    ResultVo<VepLabHitInfo> abValueWithSegAndDiscardTag(const std::string &layer_code, const std::string &shunt);
    ResultVo<VepLabHitInfo> abValueWithSegAndTag(const std::string &layer_code, const std::string &shunt, const std::string &tag);

    //通用接口
    ResultVo<VepLabHitInfo> abValues(const std::string &layer_code, const std::string &shunt, const std::string &tag);

private:

    VepLayerClientDto layer_param_;              //实验层参数

    std::shared_ptr<VepLayerConfigProvider> layer_config_provider_ptr_; //实验层_数据提供器
};

}  //namespace common

#endif //COMMON_LIBVEP_VEP_SCENE_RELATED_CLIENT_H
