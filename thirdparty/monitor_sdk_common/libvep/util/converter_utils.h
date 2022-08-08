//
// Created by 吴婷 on 2019-12-03.
//

#ifndef COMMON_LIBVEP_CONVERTER_UTILS_H
#define COMMON_LIBVEP_CONVERTER_UTILS_H

#include "thirdparty/monitor_sdk_common/libvep/entity/layer_vo.h"
#include "thirdparty/monitor_sdk_common/libvep/entity/model_Dto.h"
#include "thirdparty/monitor_sdk_common/libvep/entity/scene_vo.h"

class LayerConverter {
public:
    static std::shared_ptr<ConversionLayerDto> convert(const VepApiLayerTrafficVo& layer_traffic_vo);
};

class LaunchLayerConverter{
public:
    static std::shared_ptr<ConversionLaunchLayerDto> convert(const VepApiLaunchLayerVo& launch_layer_vo);
};

class SceneConverter{
public:
    static void generateMapTagToVersionWithinLab(std::shared_ptr<VepApiLabVo> vep_lab_vo);
    static void generateMapTagToVersionWithinScene(std::shared_ptr<VepApiSceneVo> vep_scene_vo);
};

#endif //COMMON_LIBVEP_CONVERTER_UTILS_H
