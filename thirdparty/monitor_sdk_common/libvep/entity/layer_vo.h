//
// Created by 吴婷 on 2019-12-02.
//

#ifndef COMMON_LIBVEP_LAYER_VO_H
#define COMMON_LIBVEP_LAYER_VO_H

#include <vector>

#include "thirdparty/monitor_sdk_common/libvep/entity/meta_Dto.h"

/**
 * 实验层对象
 */
class VepApiLayerVo{
public:
    int id;                     //主键Id
    std::string code;           //实验层code
    std::string name;           //实验层名称
    std::string biz_line_name;    //业务线名称 bizLineName
    std::string shunt_field;     //分流字段 shuntField，比如imei，请求Id
    std::string shunt_tag_type;     //圈定字段
    int salt_switch;             //是否加盐开关 saltSwitch，0：关闭，1：打开
    int salt_location;           //加盐的位置 saltLocation，1:前面，2：后面
    std::string salt;           //分层因子，盐值
    int encrypt_switch;          //分流字段是否MD5加密 encryptSwitch,1:是,0:否
    int hash_switch;             //哈希开关 hashSwitch，1：需要哈希，0：不需要
    std::string creator;        //创建人
    std::string modifier;       //更新人
    long create_time;            //创建时间 createTime
    long update_time;            //更新时间 updateTime
    int enable;                 //是否启用，1：是，0：否
};

/**
 * 实验+流量信息_值对象
 */
class VepApiLabWithPositionVo : public RelatedLabDto{
public:
    std::vector<std::shared_ptr<LabVersionDto> > version_list;     //实验版本列表 versionList
    std::vector<std::string> traffic_positions;  //流量位Id集合 trafficPositions
};

/**
 * 实验层流量信息
 */
class VepApiLayerTrafficVo{
public:
    VepApiLayerVo layer;                            //实验层信息
    std::vector<std::shared_ptr<VepApiLabWithPositionVo>> lab_list;   //实验列表数据 labList
    long last_layer_changing_time = -1L;               //最新的实验层变更时间 lastLayerChangingTime
};

/**
 * 实验层拉取信息
 */
class VepApiLayerFetchVo{
public:
    std::string fetch_type;                                  //拉取类型 fetchType
    std::vector<std::shared_ptr<RelatedLayerRelevanceChangeDto>> change_list; //实验层关联信息变更信息 changeList
};

/**
 * launch层流量信息
 */
class VepApiLaunchLayerVo{
public:
    std::string layer_code;                          //实验层code layerCode
    std::vector<std::shared_ptr<LaunchLayerDto>> launch_layer_list;  //Launch层_传输对象 launchLayerList
    long last_layer_changing_time = -1L;   //最新的实验层变更时间
};

/**
 * 第三方关联的流量位参数
 */
class ThirdRelatedPositionParam{
public:
    std::string related_key;     //relatedKey 关联Key，比如广告位Id、资源位Id
    std::string related_name;    //relatedName 关联的名称, 比如广告位名称
};

#endif //COMMON_LIBVEP_LAYER_VO_H
