//
// Created by 吴婷 on 2019-12-04.
//

#ifndef BLADE_BASE_COMMON_LAYER_CACHE_UTILS_H
#define BLADE_BASE_COMMON_LAYER_CACHE_UTILS_H

#include <map>
#include <memory>
#include <string>

#include "thirdparty/monitor_sdk_common/base/unordered_map.h"
#include "thirdparty/monitor_sdk_common/base/singleton.h"
#include "thirdparty/monitor_sdk_common/libvep/entity/model_Dto.h"
#include "thirdparty/monitor_sdk_common/libvep/entity/scene_vo.h"

namespace common{
/**
 * 缓存提供器（实验层/launch层）
 */
class LayerCacheUtils : public SingletonBase<LayerCacheUtils>{
public:
    static bool putLayer(const std::string &cache_key, std::shared_ptr<ConversionLayerDto> layer_dto);
    static bool getLayer(const std::string &cache_key, std::shared_ptr<ConversionLayerDto>& layer_dto);

    static bool putLaunchLayer(const std::string &cache_key, std::shared_ptr<ConversionLaunchLayerDto> layer_dto);
    static bool getLaunchLayer(const std::string &cache_key, std::shared_ptr<ConversionLaunchLayerDto>& layer_dto);

    static bool put(const std::string &scene_code, std::shared_ptr<VepApiSceneVo> scene_vo);
    static bool get(const std::string &scene_code, std::shared_ptr<VepApiSceneVo>& scene_vo);

private:
    static std::unordered_map<std::string, std::shared_ptr<ConversionLayerDto> > conversion_layer_dto_map_;  //缓存ConversionLayerDto类
    static std::unordered_map<std::string, std::shared_ptr<ConversionLaunchLayerDto> > conversion_launch_layer_dto_map_;  //缓存ConversionLayerDto类
    static std::unordered_map<std::string, std::shared_ptr<VepApiSceneVo>> scene_vo_map_;             //缓存场景值对象类V1.0

};

} //namespace common

#endif //BLADE_BASE_COMMON_LAYER_CACHE_UTILS_H
