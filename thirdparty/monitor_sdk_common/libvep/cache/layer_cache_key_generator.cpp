//
// Created by 吴婷 on 2019-11-27.
//

#include "layer_cache_key_generator.h"
#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/libvep/config/constants.h"

using namespace common;

std::string LayerCacheKeyGenerator::LAYER_KEY_PREFIX = "layer-";

std::string LayerCacheKeyGenerator::LAUNCH_LAYER_KEY_PREFIX = "launch-";

std::string LayerCacheKeyGenerator::SCENE_LAYER_KEY_PREFIX = "scene-layer-";

std::string LayerCacheKeyGenerator::SCENE_LAUNCH_LAYER_KEY_PREFIX = "scene-launch-";

/**
 * 根据实验请求对象生成缓存key：实验层
 * @param layer_param
 * @param layer_code
 * @return
 */
void LayerCacheKeyGenerator::generateLayerCacheKey(const VepLayerClientDto& layer_param, const std::string & layer_code, std::string *key) {
    if (0 == layer_param.layer_using_type.compare(LayerUsingTypeEnum::THIRD_PARTY_WITH_LAYER)) {
        return LayerCacheKeyGenerator::generateLayerCacheKey(layer_param.position_type, layer_code, key);
    }
    return LayerCacheKeyGenerator::generateLayerCacheKeyForScene(layer_param.scene_code, layer_code, key);
}

/**
 * 根据实验请求对象生成缓存key：launch层
 * @param layer_param
 * @param layer_code
 * @return
 */
void LayerCacheKeyGenerator::generateLaunchLayerCacheKey(const VepLayerClientDto& layer_param, const std::string &layer_code, std::string *key) {
    if (0 == layer_param.layer_using_type.compare(LayerUsingTypeEnum::THIRD_PARTY_WITH_LAYER)) {
        return LayerCacheKeyGenerator::generateLaunchLayerCacheKey(layer_param.position_type, layer_code, key);
    }
    return LayerCacheKeyGenerator::generateLaunchLayerCacheKeyForScene(layer_param.scene_code, layer_code, key);
}

/**
 * 生成实验层存储key
 * @param biz_type
 * @param layer_code
 * @return
 */
void LayerCacheKeyGenerator::generateLayerCacheKey(const std::string &biz_type, const std::string &layer_code, std::string *key) {
    *key += LAYER_KEY_PREFIX;
    *key += biz_type;
    *key += "-";
    *key += layer_code;
    //return StringPrint("%s%s-%s", LAYER_KEY_PREFIX, biz_type, layer_code);
}

/**
 * 生成launch层存储key
 * @param biz_type
 * @param layer_code
 * @return
 */
void LayerCacheKeyGenerator::generateLaunchLayerCacheKey(const std::string &biz_type, const std::string &layer_code, std::string *key) {
    *key += LAUNCH_LAYER_KEY_PREFIX;
    *key += biz_type;
    *key += "-";
    *key += layer_code;
    //return StringPrint("%s%s-%s", LAUNCH_LAYER_KEY_PREFIX, biz_type, layer_code);
}

/**
 * 生成场景+实验层存储key
 * @param scene_code
 * @param layer_code
 * @return
 */
void LayerCacheKeyGenerator::generateLayerCacheKeyForScene(const std::string &scene_code, const std::string &layer_code, std::string *key) {
    *key += SCENE_LAYER_KEY_PREFIX;
    *key += scene_code;
    *key += "-";
    *key += layer_code;
    //return StringPrint("%s%s-%s", SCENE_LAYER_KEY_PREFIX, scene_code, layer_code);
}

/**
 * 生成场景+launch层存储key
 * @param scene_code
 * @param layer_code
 * @return
 */
void LayerCacheKeyGenerator::generateLaunchLayerCacheKeyForScene(const std::string &scene_code, const std::string &layer_code, std::string *key) {
    *key += SCENE_LAUNCH_LAYER_KEY_PREFIX;
    *key += scene_code;
    *key += "-";
    *key += layer_code;
    //return StringPrint("%s%s-%s", SCENE_LAUNCH_LAYER_KEY_PREFIX, scene_code, layer_code);
}
