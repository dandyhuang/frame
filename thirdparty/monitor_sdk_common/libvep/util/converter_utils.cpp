//
// Created by 吴婷 on 2019-12-03.
//

#include <vector>
#include <map>
#include <set>

#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/libvep/config/constants.h"
#include "thirdparty/monitor_sdk_common/libvep/util/utils.h"
#include "thirdparty/monitor_sdk_common/libvep/util/deserialize_utils.h"
#include "converter_utils.h"

using namespace common;

std::shared_ptr<ConversionLayerDto> LayerConverter::convert(const VepApiLayerTrafficVo& layer_traffic_vo) {
    std::shared_ptr<ConversionLayerDto> empty = std::make_shared<ConversionLayerDto>();     //返回空对象
    if(layer_traffic_vo.last_layer_changing_time == -1L){  //实验层流量对象为默认 ----------【待优化】
        return empty;
    }

    std::vector<std::shared_ptr<VepApiLabWithPositionVo> > lab_list = layer_traffic_vo.lab_list; //获取实验列表数据
    //定义map
    std::unordered_map<int, std::shared_ptr<VepApiLabWithPositionVo> > lab_map;   // 反解析出映射关系：labId -> VepApiLabWithPositionVo;
    std::unordered_map<int, std::shared_ptr<LabVersionDto> > version_map;         // 反解析出映射关系：labVersionId -> labVersion对象
    std::unordered_map<std::string, std::unordered_map<std::string, int> > traffic_whitelist_mappings; // trafficWhitelistMappings 映射关系：流量位Id -> (白名单 -> 实验组Id)
    std::unordered_map<std::string, std::unordered_map<std::string, int> > traffic_segment_mappings; // trafficSegmentMappings 映射关系：流量位Id -> (号段 -> 实验组Id)
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<int> > > traffic_tag_mappings;     // 映射关系：流量位Id -> (圈定字段 -> 实验组Id)

    //layerTrafficVo

    for (auto lab_with_position_vo : lab_list){
        lab_map.insert({lab_with_position_vo->id, lab_with_position_vo});  //---labMap

        std::vector<std::shared_ptr<LabVersionDto> > version_list = lab_with_position_vo->version_list;     //获取实验版本列表

        std::unordered_map<std::string, int> whitelist_sub_map;     //白名单
        std::unordered_map<std::string, int> segment_sub_map;       //号段名单
        std::unordered_map<std::string, std::vector<int>> tag_sub_map;    //圈定字段名单

        //针对实验版本
        for(auto version_dto : version_list){
            int version_id = version_dto->id;
            version_map.insert({version_id, version_dto});             //---versionMap

            //1.提取白名单
            std::string integration_state = version_dto->integration_state;
            if(IntegrationStateEnum::PROCESSING == integration_state || IntegrationStateEnum::TEST_SUCCEED == integration_state ){
                std::string whitelist = version_dto->whitelist;
                if(VepStringUtils::IsStringNotblank(whitelist)){
                    std::vector<std::string> whitelist_arr;
                    SplitString(whitelist, ",", &whitelist_arr);    //按,分隔string
                    for(auto item : whitelist_arr){
                        std::string segment_item = StringTrim(item);
                        whitelist_sub_map.insert({segment_item, version_id});
                    }
                }
            }

            //2.计算号段,圈定字段
            std::string lab_state_enum = lab_with_position_vo->lab_state;
            if(LabStateEnum::APPLYING.first == lab_state_enum || LabStateEnum::EFFECTIVE.first == lab_state_enum){
                //号段
                std::string diversion_strategy = version_dto->diversion_strategy;
                std::vector<std::string> diversion_arr;
                if(VepStringUtils::IsStringNotblank(diversion_strategy)){
                    SplitString(diversion_strategy, ",", &diversion_arr);    //按,分隔string
                }
                for (auto diversion_segment : diversion_arr) {
                    //去除空格
                    std::string segment_item = StringTrim(diversion_segment);
                    //不足3位的进行补足
                    if(segment_item.size() == 3){
                        segment_sub_map.insert({segment_item, version_id});
                    } else if (segment_item.size() == 2){
                        //填充为3位，由两位号段，变更为10个三位数字号段
                        for (int i = 0; i < 10; ++i) {
                            std::string real_segment = StringPrint("%d%s", i, segment_item);
                            segment_sub_map.insert({real_segment, version_id});
                        }
                    } else if (segment_item.size() == 1){
                        //填充为3位，由1位号段，变更为100个三位数字号段
                        for (int i = 0; i < 100; ++i) {
                            std::string real_segment = StringPrint("%02d%s", i, segment_item);
                            segment_sub_map.insert({real_segment, version_id});
                        }
                    }
                }
                //圈定字段
                std::string tag_diversion_strategy = version_dto->shunt_tag_strategy;
                std::vector<std::string> tags;
                if(VepStringUtils::IsStringNotblank(tag_diversion_strategy)){
                    SplitString(tag_diversion_strategy, ",", &tags);    //按,分隔string
                }
                for (auto tag : tags){
                    std::vector<int> contained_version_with_tag;
                    if(tag_sub_map.find(tag) != tag_sub_map.end()){   //找到，取出追加
                        contained_version_with_tag = tag_sub_map[tag];
                    }
                    //没有找到，直接添加
                    contained_version_with_tag.push_back(version_id);
                    //填充map
                    tag_sub_map[tag] = contained_version_with_tag;
                }

            }
        }

        //针对实验层流量：计算流量位信息
        std::vector<std::string> traffic_positions = lab_with_position_vo->traffic_positions;
        if(traffic_positions.size() != 0){
            for (auto traffic_position_id : traffic_positions) {
                std::unordered_map<std::string, std::unordered_map<std::string, int>>::iterator it;
                //处理白名单
                it = traffic_whitelist_mappings.find(traffic_position_id);
                if(it == traffic_whitelist_mappings.end()){   //不包含，就直接加入
                    traffic_whitelist_mappings.insert({traffic_position_id, whitelist_sub_map});
                } else {                                    //包含，就追加
                    std::unordered_map<std::string, int> whitelist_part = traffic_whitelist_mappings[traffic_position_id];
                    if(!whitelist_sub_map.empty()){
                        for (auto kv : whitelist_sub_map) {
                            whitelist_part.insert({kv.first, kv.second});
                        }
                    }
                    traffic_whitelist_mappings[traffic_position_id] = whitelist_part;
                }

                //处理号段信息
                if(traffic_segment_mappings.find(traffic_position_id) == traffic_segment_mappings.end()){     //不包含，就直接加入
                    traffic_segment_mappings.insert({traffic_position_id, segment_sub_map});
                } else {                                    //包含，就追加
                    std::unordered_map<std::string, int> segment_part = traffic_segment_mappings[traffic_position_id];
                    if(!segment_sub_map.empty()){
                        for (auto kv : segment_sub_map) {
                            segment_part.insert({kv.first, kv.second});
                        }
                    }
                    traffic_segment_mappings[traffic_position_id] = segment_part;
                }
                //处理圈定字段
                // 映射关系：流量位Id -> (圈定字段 -> 实验组Id)
                std::unordered_map<std::string, std::vector<int>> all_tag_in_cur_position;
                if(traffic_tag_mappings.find(traffic_position_id) != traffic_tag_mappings.end()){   //找到，取出map
                    all_tag_in_cur_position = traffic_tag_mappings[traffic_position_id];
                }
                for (auto kv : tag_sub_map){
                    std::string tmp_tag = kv.first; //key
                    std::vector<int> tmp_contained_version_id = kv.second; //value
                    std::vector<int> all_version_id_set;
                    if(all_tag_in_cur_position.find(tmp_tag) != all_tag_in_cur_position.end()){   //找到，取出追加
                        all_version_id_set = all_tag_in_cur_position[tmp_tag];
                    }
                    for (auto id : tmp_contained_version_id){
                        all_version_id_set.push_back(id);
                    }
                    all_tag_in_cur_position[tmp_tag] = all_version_id_set;
                    traffic_tag_mappings[traffic_position_id] = all_tag_in_cur_position;
                }


            }
        }
    }

    std::shared_ptr<ConversionLayerDto> conversion_layer_dto = std::make_shared<ConversionLayerDto>(layer_traffic_vo, lab_map, version_map, traffic_whitelist_mappings, traffic_segment_mappings, traffic_tag_mappings);
    return conversion_layer_dto;
}

std::shared_ptr<ConversionLaunchLayerDto> LaunchLayerConverter::convert(const VepApiLaunchLayerVo& launch_layer_vo){
    std::shared_ptr<ConversionLaunchLayerDto> empty = std::make_shared<ConversionLaunchLayerDto>();     //返回空对象
    if(launch_layer_vo.last_layer_changing_time == -1L){  //launch层流量对象为默认 ----------【待优化】
        return empty;
    }

    std::string layer_code = launch_layer_vo.layer_code;
    const std::vector<std::shared_ptr<LaunchLayerDto>> &launch_layer_list = launch_layer_vo.launch_layer_list;
    std::unordered_map<std::string, std::shared_ptr<LaunchLayerDto>> launch_layer_map; // 建立映射关系，traffic_position_id -> LaunchLayer
    //针对launch列表
    for (auto launch_layer_dto : launch_layer_list) {
        std::string traffic_position_ids = launch_layer_dto->traffic_position_ids;  //流量位Id
        std::vector<std::shared_ptr<ThirdRelatedPositionParam>> related_list;  //第三方关联的流量位参数
        VepDeserializeUtils::DeserializeThirdRelatedPositionParam(traffic_position_ids, &related_list);  //反序列化填充

        if(related_list.size() != 0){
            for (auto positionParam : related_list){
                launch_layer_map.insert({positionParam->related_key, launch_layer_dto});
            }
        }
    }

    std::shared_ptr<ConversionLaunchLayerDto> launch_layer_dto = std::make_shared<ConversionLaunchLayerDto>(layer_code, launch_layer_list, launch_layer_map);
    return launch_layer_dto;
}

void SceneConverter::generateMapTagToVersionWithinLab(std::shared_ptr<VepApiLabVo> vep_lab_vo){
    const std::unordered_map<int, std::shared_ptr<LabVersionDto>> &version_dto_map = vep_lab_vo->version_map;
    std::unordered_map<std::string, std::set<int>> tag_map_to_group;
    if(!version_dto_map.empty()){
        for(auto kv : version_dto_map){
            std::string shunt_tag_strategy = kv.second->shunt_tag_strategy;
            if(VepStringUtils::IsStringNotblank(shunt_tag_strategy)){
                //逗号分隔
                std::vector<std::string> tags;
                SplitString(shunt_tag_strategy, ",", &tags);    //按,分隔string，将分隔内容填充到向量中
                for (auto tag : tags){
                    std::set<int> version_set;
                    auto iter = tag_map_to_group.find(tag);
                    if(iter != tag_map_to_group.end()){  //包含，取出追加
                        version_set = iter->second;
                    }
                    version_set.insert(kv.first);
                    tag_map_to_group[tag] = version_set;
                }
            }
        }
    }
    vep_lab_vo->tag_map_to_group = tag_map_to_group;
}

void SceneConverter::generateMapTagToVersionWithinScene(std::shared_ptr<VepApiSceneVo> vep_scene_vo){
    const std::unordered_map<std::string, std::shared_ptr<VepApiLabVo>> &lab_map = vep_scene_vo->lab_map;
    if(lab_map.empty()){
        return;
    }
    for(auto kv : lab_map){
        SceneConverter::generateMapTagToVersionWithinLab(kv.second);
    }
}
