//
// Created by 吴婷 on 2019-12-04.
//
#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/libvep/util/log_utils.h"
#include "cache_utils.h"

using namespace common;

//初始化静态实验层映射map
std::unordered_map<std::string, std::shared_ptr<ConversionLayerDto> > LayerCacheUtils::conversion_layer_dto_map_ = std::unordered_map<std::string, std::shared_ptr<ConversionLayerDto> >();
//初始化静态launch层映射map
std::unordered_map<std::string, std::shared_ptr<ConversionLaunchLayerDto> > LayerCacheUtils::conversion_launch_layer_dto_map_ = std::unordered_map<std::string, std::shared_ptr<ConversionLaunchLayerDto> >();
//初始化静态场景映射map
std::unordered_map<std::string, std::shared_ptr<VepApiSceneVo>> LayerCacheUtils::scene_vo_map_ = std::unordered_map<std::string, std::shared_ptr<VepApiSceneVo>>();


bool LayerCacheUtils::putLayer(const std::string &cache_key, std::shared_ptr<ConversionLayerDto> layer_dto) {
    conversion_layer_dto_map_[cache_key] = layer_dto;
    return true;
}

bool LayerCacheUtils::getLayer(const std::string &cache_key, std::shared_ptr<ConversionLayerDto>& layer_dto) {
    //1.查找map中是否存在
    auto iter = conversion_layer_dto_map_.find(cache_key);
    if (iter == conversion_layer_dto_map_.end()) {  //不包含，就打印日志，返回nullptr
        LogUtils::LogInfo(StringPrint("缓存中没有key：[%s]", cache_key));
        return false;
    }
    layer_dto = iter->second;
    return true;
}

bool LayerCacheUtils::putLaunchLayer(const std::string &cache_key, std::shared_ptr<ConversionLaunchLayerDto> layer_dto) {
    conversion_launch_layer_dto_map_[cache_key] = layer_dto;
    return true;
}

bool LayerCacheUtils::getLaunchLayer(const std::string &cache_key, std::shared_ptr<ConversionLaunchLayerDto>& layer_dto) {
    //1.查找map中是否存在
    auto iter = conversion_launch_layer_dto_map_.find(cache_key);
    if (iter == conversion_launch_layer_dto_map_.end()) {  //不包含，就打印日志，返回nullptr
        LogUtils::LogInfo(StringPrint("缓存中没有key：[%s]", cache_key));
        return false;
    }
    layer_dto = iter->second;
    return true;
}

bool LayerCacheUtils::put(const std::string &scene_code, std::shared_ptr<VepApiSceneVo> scene_vo) {
    scene_vo_map_[scene_code] = scene_vo;
    return true;
}

bool LayerCacheUtils::get(const std::string &scene_code, std::shared_ptr<VepApiSceneVo> &scene_vo) {
    //1.查找map中是否存在
    auto iter = scene_vo_map_.find(scene_code);
    if (iter == scene_vo_map_.end()) {  //不包含，就打印日志，返回nullptr
        LogUtils::LogInfo(StringPrint("缓存中没有key：[%s]", scene_code));
        return false;
    }
    scene_vo = iter->second;
    return true;
}