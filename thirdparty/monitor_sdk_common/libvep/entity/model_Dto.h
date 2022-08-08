//
// Created by 吴婷 on 2019-12-03.
//

#ifndef COMMON_LIBVEP_MODEL_DTO_H
#define COMMON_LIBVEP_MODEL_DTO_H

#include <map>
#include <vector>
#include <string>
#include <unordered_map>

#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/libvep/util/utils.h"
#include "thirdparty/monitor_sdk_common/libvep/entity/layer_vo.h"
#include "thirdparty/monitor_sdk_common/libvep/entity/meta_Dto.h"

#include "thirdparty/jsoncpp/json.h"

/**
 * 实验层请求对象
 * 含用户传入信息
 */
class VepLayerClientDto{
public:
    VepLayerClientDto(){}
    ~VepLayerClientDto(){}

    VepLayerClientDto(const std::string &layer_using_type, const std::string &position_type, std::vector<std::string> layer_code_set) :
            layer_using_type(layer_using_type), position_type(position_type), layer_code_set(layer_code_set) {
    }

    VepLayerClientDto(const std::string &layer_using_type, const std::string &position_type
            , std::vector<std::string> layer_code_set, const std::string &scene_code) :
            layer_using_type(layer_using_type), position_type(position_type),
            layer_code_set(layer_code_set), scene_code(scene_code) {
    }

    const std::string ToString() const {
        Json::Value root;
        root["layerUsingType"] = layer_using_type;
        root["positionType"] = position_type;
        root["sceneCode"] = scene_code;

        Json::Value layer_root;
        int count = layer_code_set.size();
        for (int i = 0; i < count; ++i) {
            layer_root[i] = layer_code_set[i];
        }
        root["layerCodeSet"] = layer_root;

        return common::VepJsonUtils::ParseString(root);
    }

public:
    std::string layer_using_type;     //实验层使用类型: layerUsingType
    std::string position_type;       // positionType 流量位类型，比如广告、CPD、场景     TrafficPositionType类型  结构体
    std::vector<std::string> layer_code_set;                          //实验层code集合
    std::string scene_code;                                     //场景code
};

/**
 * 解析之后的实验层对象，建立映射关系
 */
class ConversionLayerDto{
public:
    ConversionLayerDto() {}
    ~ConversionLayerDto() {}

    ConversionLayerDto(VepApiLayerTrafficVo layer_traffic_vo
            , std::unordered_map<int, std::shared_ptr<VepApiLabWithPositionVo> > lab_map
            , std::unordered_map<int, std::shared_ptr<LabVersionDto> > version_map
            , std::unordered_map<std::string, std::unordered_map<std::string, int>> traffic_whitelist_mappings
            , std::unordered_map<std::string, std::unordered_map<std::string, int>> traffic_segment_mappings
            , std::unordered_map<std::string, std::unordered_map<std::string, std::vector<int>>> traffic_tag_mappings) :
            layer_traffic_vo(layer_traffic_vo), lab_map(lab_map), version_map(version_map),
            traffic_whitelist_mappings(traffic_whitelist_mappings), traffic_segment_mappings(traffic_segment_mappings),
            traffic_tag_mappings(traffic_tag_mappings) {
    }

    const std::string ToString() const {
        Json::Value root;

        root["shuntField"] = layer_traffic_vo.layer.shunt_field;
        root["shuntTagType"] = layer_traffic_vo.layer.shunt_tag_type;

        //打印白名单
        Json::Value white_root;
        for (auto kv : traffic_whitelist_mappings) {
            Json::Value root_map;
            for (auto seg_kv : kv.second){
                root_map[seg_kv.first] = seg_kv.second;
            }
            white_root[kv.first] = root_map;
        }
        root["trafficWhitelistMappings"] = white_root;
        //打印号段名单
        Json::Value seg_root;
        for (auto kv : traffic_segment_mappings) {
            Json::Value root_map;
            for (auto seg_kv : kv.second){
                root_map[seg_kv.first] = seg_kv.second;
            }
            seg_root[kv.first] = root_map;
        }
        root["trafficSegmentMappings"] = seg_root;
        //打印圈定字段
        Json::Value tag_root;
        for (auto kv : traffic_tag_mappings) {
            Json::Value root_map;
            for (auto seg_kv : kv.second){
                //圈定字段对应的一组id
                Json::Value ids_root;
                for (unsigned int i = 0; i < seg_kv.second.size(); i++) {
                    ids_root[i] = seg_kv.second[i];
                }
                root_map[seg_kv.first] = ids_root;
            }
            tag_root[kv.first] = root_map;
        }
        root["trafficTagMappings"] = tag_root;

        Json::StreamWriterBuilder builder;
        return Json::writeString(builder, root);
    }

public:
    VepApiLayerTrafficVo layer_traffic_vo;  //实验层流量数据: layerTrafficVo
    std::unordered_map<int, std::shared_ptr<VepApiLabWithPositionVo> > lab_map;   // 反解析出映射关系：labId -> VepApiLabWithPositionVo;
    std::unordered_map<int, std::shared_ptr<LabVersionDto> > version_map;         // 反解析出映射关系：labVersionId -> labVersion对象
    std::unordered_map<std::string, std::unordered_map<std::string, int>> traffic_whitelist_mappings; // 映射关系：流量位Id -> (白名单 -> 实验组Id)
    std::unordered_map<std::string, std::unordered_map<std::string, int>> traffic_segment_mappings;   // 映射关系：流量位Id -> (号段 -> 实验组Id)
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<int>>> traffic_tag_mappings;     // 映射关系：流量位Id -> (圈定字段 -> 实验组Id)
};

/**
 * 解析之后的Launch层信息，建立映射关系
 */
class ConversionLaunchLayerDto{
public:
    ConversionLaunchLayerDto() {}
    ~ConversionLaunchLayerDto() {}

    ConversionLaunchLayerDto(const std::string &layer_code
            , std::vector<std::shared_ptr<LaunchLayerDto>> launch_layer_list
            , std::unordered_map<std::string, std::shared_ptr<LaunchLayerDto>> launch_layer_map) :
            layer_code(layer_code), launch_layer_list(launch_layer_list), launch_layer_map(launch_layer_map) {
    }

public:
    std::string layer_code;                      //实验层code
    std::vector<std::shared_ptr<LaunchLayerDto>> launch_layer_list;//launch层列表
    std::unordered_map<std::string, std::shared_ptr<LaunchLayerDto>> launch_layer_map; // launchLayerMap 建立映射关系，traffic_position_id -> LaunchLayer
};

#endif //COMMON_LIBVEP_MODEL_DTO_H
