//
// Created by 吴婷 on 2019-12-10.
//

#ifndef COMMON_LIBVEP_SCENE_VO_H
#define COMMON_LIBVEP_SCENE_VO_H

#include <set>
#include <map>
#include <vector>
#include <memory>

#include "thirdparty/monitor_sdk_common/base/unordered_map.h"
#include "meta_Dto.h"

/**
 * 用户传入场景参数
 */
class VepSceneParamVo{
public:
    VepSceneParamVo(){}
    ~VepSceneParamVo(){}

    VepSceneParamVo(const std::string &scene_code, const std::string &scene_secret) :
        scene_code(scene_code), scene_secret(scene_secret){}

public:
    std::string scene_code;     //场景code
    std::string scene_secret;   //场景密钥
};

/**
 * 实验值对象
 */
class VepApiLabVo{
public:
    LabDto lab;                 //实验VO
    int hash_match_length;      // hashMatchLength 哈希匹配长度
    std::unordered_map<std::string, int> whitelist_map;    //白名单 {号段：实验版本号}
    std::unordered_map<std::string, int> segment_map;      // segmentMap 号段匹配 {号段：实验版本号}
    std::vector<int> version_id_list;             // versionIdList 实验版本列表id
    std::unordered_map<int, std::shared_ptr<LabVersionDto>> version_map;    //实验版本列表
    std::unordered_map<std::string, std::set<int>> tag_map_to_group;    //{tag: {一组version_id}}
};

/**
 * 场景值对象
 */
class VepApiSceneVo{
public:
    SceneDto scene;   //场景
    std::vector<std::string> lab_name_list;     //实验名列表 labNameList
    std::unordered_map<std::string, std::shared_ptr<VepApiLabVo>> lab_map; //labMap{实验名：实验值对象}
    long last_accumulate_updated_time = -1L;    //lastAccumulateUpdatedTime
};

#endif //COMMON_LIBVEP_SCENE_VO_H
