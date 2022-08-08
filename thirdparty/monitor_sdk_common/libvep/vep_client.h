//
// Created by 吴婷 on 2019-12-10.
//

#ifndef COMMON_LIBVEP_VEP_CLIENT_H
#define COMMON_LIBVEP_VEP_CLIENT_H

#include <map>
#include <vector>
#include <memory>

#include "thirdparty/monitor_sdk_common/libvep/entity/meta_Dto.h"
#include "thirdparty/monitor_sdk_common/libvep/entity/scene_vo.h"
#include "thirdparty/monitor_sdk_common/libvep/result/resultVo.h"
#include "thirdparty/monitor_sdk_common/libvep/result/vep_lab_paramVo.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/vep_config_provider.h"
#include "thirdparty/monitor_sdk_common/libvep/config/dynamic_client_config.h"

namespace common {
/**
 * V1.0 客户端
 */
class VepClient {
public:
    explicit VepClient(const std::string &scene_code);
    VepClient(const std::string &scene_code, const std::string &scene_secret); //根据场景code和密钥初始化client
    ~VepClient();

    void initialize(bool is_scheduled = true);                      //启动客户端
    void close();                           //关闭客户端

    std::shared_ptr<VepApiSceneVo> getSceneVo();             //获取场景信息
    ResultVo<VepLabParamVo> abValues(const std::string &lab_name, const std::string &shunt); //通过实验名称+分流字段，返回匹配到的实验版本信息
    std::vector<ResultVo<VepLabParamVo>> abValues(const std::string &shunt);   //通过分流字段依次匹配场景下的所有实验(只适用于单选号段情况)

    ResultVo<VepLabParamVo> abValuesWithSeg(const std::string &lab_name, const std::string &shunt);
    ResultVo<VepLabParamVo> abValueWithTag(const std::string &lab_name, const std::string &tag);
    ResultVo<VepLabParamVo> abValueWithSegAndDiscardTag(const std::string &lab_name, const std::string &shunt);
    ResultVo<VepLabParamVo> abValueWithSegAndTag(const std::string &lab_name, const std::string &shunt, const std::string &tag);

    //通用接口
    ResultVo<VepLabParamVo> abValues(const std::string &lab_name, const std::string &shunt, const std::string &tag);

private:
    VepSceneParamVo scene_Param_vo_;           //用户传入场景参数
    std::shared_ptr <VepConfigProvider> vep_config_provider_ptr_; //场景数据提供器

};

} //namespace common
#endif //COMMON_LIBVEP_VEP_CLIENT_H
