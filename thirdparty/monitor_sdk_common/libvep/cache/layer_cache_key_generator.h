//
// Created by 吴婷 on 2019-11-27.
//

#ifndef COMMON_LAYER_CACHE_KEY_GENERATOR_H
#define COMMON_LAYER_CACHE_KEY_GENERATOR_H

#include <string>

#include "thirdparty/monitor_sdk_common/libvep/entity/model_Dto.h"

namespace common {
/**
 * 缓存Key生成器
 */
class LayerCacheKeyGenerator {
public:
    //如：layer-ad-SS
    static void generateLayerCacheKey(const VepLayerClientDto& layer_param, const std::string &layer_code, std::string *key);         //根据实验请求对象生成缓存key：实验层

    //如：launch-ad-SS
    static void generateLaunchLayerCacheKey(const VepLayerClientDto& layer_param, const std::string &layer_code, std::string *key);   //根据实验请求对象生成缓存key：launch层

private:
    static void generateLayerCacheKey(const std::string &biz_type, const std::string &layer_code, std::string *key);                  //生成实验层存储key

    static void generateLaunchLayerCacheKey(const std::string &biz_type, const std::string &layer_code, std::string *key);            //生成launch层存储key

    static void generateLayerCacheKeyForScene(const std::string &scene_code, const std::string &layer_code, std::string *key);        //生成场景+实验层存储key

    static void generateLaunchLayerCacheKeyForScene(const std::string &scene_code, const std::string &layer_code, std::string *key);  //生成场景+launch层存储key

private:
    static std::string LAYER_KEY_PREFIX;

    static std::string LAUNCH_LAYER_KEY_PREFIX;

    static std::string SCENE_LAYER_KEY_PREFIX;

    static std::string SCENE_LAUNCH_LAYER_KEY_PREFIX;
};
} //namespace common

#endif //COMMON_LAYER_CACHE_KEY_GENERATOR_H
