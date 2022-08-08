
#ifndef COMMON_LIBVEP_CONFIG_CONSTANTS_H_
#define COMMON_LIBVEP_CONFIG_CONSTANTS_H_

#include <utility>
#include <string>

namespace common {

/**
 * 请求的固定配置信息
 */
class ConfigConstants {
public:
    static std::string CLIENT_LOGGER_NAME;          //客户端logger名称

    static std::string CLIENT_CONFIG_APP_NAME;      //客户端在配置中心中的应用名称

    static std::string FETCH_VIVO_CFG_URL;          //拉取配置中心数据_URL地址

    static int INITIALIZE_CONFIG_FAILED_COUNT_THRESHOLD;    //拉取配置中心_失败次数阈值

    static int REQUEST_TIMEOUT_MILLISECONDS;        //请求超时时间

    static int DEFAULT_HASH_SEGMENT_LENGTH;         //哈希号段尾号匹配长度

    static std::string BURIED_LOG_VERSION;          //埋点版本

    static int NOT_MODIFIED;                        //未修改的状态码

    static int SCHEDULED_FETCH_CONFIG_INTERVAL_MILLISECONDS;    //定时拉取配置中心信息间隔

    static std::string QUERY_SCENE_CONFIG_URL;     //请求业务场景数据_请求路径

    static std::string QUERY_LAB_CONFIG_URL; //请求实验数据_请求路径

    static std::string WATCH_SCENE_CHANGED_URL;  //请求配置是否发生变更_请求路径

    static std::string UPLOAD_CLIENT_INFO_URL;    //上报客户端日志信息_请求路径

};

/**
 * 请求参数变量名_常量类
 */
class HttpConstants{
public:
    static std::string PARAM_LAB_NAME;          //实验名称变量
    static std::string PARAM_SCENE_CODE;        //业务场景Code变量
    static std::string PARAM_CLIENT_REPORT;     //业务信息上报变量
    static std::string VEP_SIGNATURE;           //请求签名
};

/**
 * 流量位类型：如广告，cpd，场景
 */
class TrafficPositionTypeEnum {
public:
    static std::string AD;    //TrafficPositionTypeEnum::AD
    static std::string CPD;
    static std::string SCENE;
};

/**
 * 实验层使用类型
 */
class LayerUsingTypeEnum{
public:
    static std::string THIRD_PARTY_WITH_LAYER;
    static std::string SCENE_WITH_LAYER;
};

/**
 * 拉取类型，ALL：全部，INCREMENT：增量
 */
class LayerFetchTypeEnum{
public:
    static std::string ALL;
    static std::string INCREMENT;
};

/**
 * 集成测试状态枚举类
 */
class IntegrationStateEnum{
public:
    static std::string NOT_STARTING;
    static std::string PROCESSING;
    static std::string TEST_SUCCEED;
    static std::string TEST_FAILED;
    static std::string APPLYING_SKIP_TEST;
    static std::string SKIP_TEST;
    static std::string FINISHED;
};

/**
 * 层关联信息变化类型
 */
class LayerRelevanceChangeTypeEnum{
public:
    static std::string CHANGE_OF_TRAFFIC_POSITION;  //场景关联的流量位变更
    static std::string CHANGE_OF_WHITELIST;         //白名单变更
    static std::string CHANGE_OF_TRAFFIC;           //流量变更
    static std::string CHANGE_OF_VAR_PARAM;         //参数变更
    static std::string CHANGE_OF_LAUNCH;            //Launch层信息变更
    static std::string CHANGE_OF_LAYER;             //实验层信息变更
};

/**
 * 实验状态枚举类
 */
class LabStateEnum{
public:
    static std::pair<std::string, int> NOT_STARTING;
    static std::pair<std::string, int> APPLYING;
    static std::pair<std::string, int> EFFECTIVE;
    static std::pair<std::string, int> FINISHED;
    static std::pair<std::string, int> DELETED;
};

/**
 * 获取远程配置枚举类
 */
class FetchRemoteConfigTypeEnum{
public:
    static std::string INITIALIZE;
    static std::string SCHEDULE;
    static std::string LONG_POLLING;
};

/**
 * 返回结果信息集
 */
class ResultConstants{
public:
    static std::pair<int, std::string> SUCCESS;    //返回信息的代码: 代码的含义
    static std::pair<int, std::string> ERROR;
    static std::pair<int, std::string> AUTH_HEADER_EMPTY;
    static std::pair<int, std::string> AUTH_TIME_ERROR;
    static std::pair<int, std::string> BIZ_SCENE_NOT_EXIST;
    static std::pair<int, std::string> AUTH_FAILED;
    static std::pair<int, std::string> EMPTY_LAB_NAME;
    static std::pair<int, std::string> EMPTY_SHUNT;
    static std::pair<int, std::string> EMPTY_PARAM;
    static std::pair<int, std::string> USING_API_NOT_CONSIST_WITH_LAB_CONFIG;
    static std::pair<int, std::string> EMPTY_TAG;
    static std::pair<int, std::string> INIT_ERROR;
    static std::pair<int, std::string> NOT_FOUND_SCENE;
    static std::pair<int, std::string> NOT_FOUND_LAB;
    static std::pair<int, std::string> LAB_HAS_FINISHED;
    static std::pair<int, std::string> NOT_FOUND_LAB_VERSION;
    static std::pair<int, std::string> LAYER_NOT_FOUND;
    static std::pair<int, std::string> LAYER_HAS_DISABLED;
    static std::pair<int, std::string> LAYER_MATCH_MISSING;
};

/**
 * 动态配置Key常量_枚举类
 */
class DynamicConfigKeyEnum{
public:
    static std::pair<std::string, std::string> DEFAULT_API_SERVER_URL;          //默认的接口服务地址
    static std::pair<std::string, std::string> CLIENT_REPORT_API_SERVER_URL;    //客户端上报日志_接口服务地址
    static std::pair<std::string, std::string> REQUEST_TIMEOUT_MILLISECONDS;    //请求超时时间
    static std::pair<std::string, std::string> LONG_POLLING_TIMEOUT_MILLISECONDS;//请求长轮询_超时时间
    static std::pair<std::string, std::string> SCHEDULED_WATCH_INTERVAL;            //定时执行长轮询的间隔
    static std::pair<std::string, std::string> SCHEDULED_FETCH_INTERVAL;            //定时获取配置的时间间隔
    static std::pair<std::string, std::string> SCHEDULED_REPORT_INTERVAL;           //定时上报客户端信息的时间间隔
    static std::pair<std::string, std::string> FETCH_FAILED_COUNT_THRESHOLD;        //获取配置信息错误_拉取次数阈值
    static std::pair<std::string, std::string> BURIED_LOG_FIELDS;                   //埋点字段集配置:样例：algCode,trafficRatio
};

/**
 * 流量分配类型枚举类, segment为号段分配, proportion为占比分配
 */
class TrafficAllocationTypeEnum{
public:
    static bool hasTag(const std::string &traffic_allocation_type_enum); //判断是否有圈定字段
    static bool hasSeg(const std::string &traffic_allocation_type_enum); //判断是否有分流字段

public:
    static std::string SEGMENT;
    static std::string PROPORTION;
    static std::string TAG;
    static std::string TAG_WITH_SEGMENT;
    static std::string REASONING;
};

/**
 * 本地配置【用于上报信息】
 */
class VepLocalProperties {
public:
    static std::string app_name;
    static std::string client_version;
    static std::string client_ip;
};

/**
 * 拉取配置中心
 */
class VepPropertySource{
public:
    static std::string app_env;
    static std::string app_loc;
    static std::string config_host;
};

} //namespace common

#endif //COMMON_LIBVEP_CONFIG_CONSTANTS_H_

