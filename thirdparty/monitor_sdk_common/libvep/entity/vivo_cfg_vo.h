//
// Created by 吴婷 on 2019-12-13.
//

#ifndef COMMON_LIBVEP_VIVO_CFG_VO_H
#define COMMON_LIBVEP_VIVO_CFG_VO_H

#include <vector>

class PropertyItem{
public:
    std::string name;   //key
    std::string value;  //value
};

class CfgResultData{
public:
    long last_modified;         //lastModified
    long request_timestamp;     //requestTimestamp
    long response_timestamp;     //responseTimestamp
    std::vector<PropertyItem> configs;
    std::string real_config_version; //realConfigVersion
};

/**
 * 拉取配置中心接口_返回值对象
 */
class VivoCfgResultVo{
public:
    int retcode = 0;        //返回码
    std::string message;    //返回文本消息
    CfgResultData data;     //业务数据
};

#endif //COMMON_LIBVEP_VIVO_CFG_VO_H
