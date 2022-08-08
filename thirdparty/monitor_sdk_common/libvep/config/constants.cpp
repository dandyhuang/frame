
#include "thirdparty/monitor_sdk_common/libvep/config/constants.h"

using namespace common;

/**
 * 请求的固定配置信息
 */
std::string ConfigConstants::CLIENT_LOGGER_NAME = "VEP-CLIENT"; //客户端logger名称

std::string ConfigConstants::CLIENT_CONFIG_APP_NAME = "vep-sdk"; //客户端在配置中心中的应用名称

std::string ConfigConstants::FETCH_VIVO_CFG_URL = "/vivocfgV2/getConfig"; //拉取配置中心数据_URL地址

int ConfigConstants::INITIALIZE_CONFIG_FAILED_COUNT_THRESHOLD = 3; //拉取配置中心_失败次数阈值

int ConfigConstants::REQUEST_TIMEOUT_MILLISECONDS = 10 * 1000;  //请求超时时间

int ConfigConstants::DEFAULT_HASH_SEGMENT_LENGTH = 3;           //哈希号段尾号匹配长度

std::string ConfigConstants::BURIED_LOG_VERSION = "1.0.0";      //埋点版本

int ConfigConstants::NOT_MODIFIED = 304;                        //未修改的状态码

int ConfigConstants::SCHEDULED_FETCH_CONFIG_INTERVAL_MILLISECONDS = 600 * 1000;//定时拉取配置中心信息间隔

std::string ConfigConstants::QUERY_SCENE_CONFIG_URL = "/vep/sceneConfig"; //请求业务场景数据_请求路径

std::string ConfigConstants::QUERY_LAB_CONFIG_URL = "/vep/labConfig";   //请求实验数据_请求路径

std::string ConfigConstants::WATCH_SCENE_CHANGED_URL = "/vep/watch";    //请求配置是否发生变更_请求路径

std::string ConfigConstants::UPLOAD_CLIENT_INFO_URL = "/vep/clientReport";  //上报客户端日志信息_请求路径

/**
 * 请求参数变量名_常量类
 */
std::string HttpConstants::PARAM_LAB_NAME = "labName";
std::string HttpConstants::PARAM_SCENE_CODE = "sceneCode";
std::string HttpConstants::PARAM_CLIENT_REPORT = "clientReport";
std::string HttpConstants::VEP_SIGNATURE = "vepSignature";

/**
 * 流量位类型：如广告，cpd，场景
 */
std::string TrafficPositionTypeEnum::AD = "ad";         //广告-第三方数据源
std::string TrafficPositionTypeEnum::CPD = "cpd";       //CPD-第三方数据源
std::string TrafficPositionTypeEnum::SCENE = "scene";   //应用-场景

/**
 * 实验层使用类型
 */
std::string LayerUsingTypeEnum::THIRD_PARTY_WITH_LAYER = "thirdpartyRelated"; //第三方业务类型+关联实验层
std::string LayerUsingTypeEnum::SCENE_WITH_LAYER = "sceneRelated"; //场景+关联实验层

/**
 * 拉取类型
 */
std::string LayerFetchTypeEnum::ALL = "ALL"; //第三方业务类型+关联实验层
std::string LayerFetchTypeEnum::INCREMENT = "INCREMENT"; //场景+关联实验层

/**
 * 集成测试状态枚举类
 */
std::string IntegrationStateEnum::NOT_STARTING = "not_starting";
std::string IntegrationStateEnum::PROCESSING = "processing";
std::string IntegrationStateEnum::TEST_SUCCEED = "test_succeed";
std::string IntegrationStateEnum::TEST_FAILED = "test_failed";
std::string IntegrationStateEnum::APPLYING_SKIP_TEST = "applying_skip_test";
std::string IntegrationStateEnum::SKIP_TEST = "skip_test";
std::string IntegrationStateEnum::FINISHED = "finished";

/**
 * 层关联信息变化类型
 */
std::string LayerRelevanceChangeTypeEnum::CHANGE_OF_TRAFFIC_POSITION = "CHANGE_OF_TRAFFIC_POSITION";//场景关联的流量位变更
std::string LayerRelevanceChangeTypeEnum::CHANGE_OF_WHITELIST = "CHANGE_OF_WHITELIST";  //白名单变更
std::string LayerRelevanceChangeTypeEnum::CHANGE_OF_TRAFFIC = "CHANGE_OF_TRAFFIC";      //流量变更
std::string LayerRelevanceChangeTypeEnum::CHANGE_OF_VAR_PARAM = "CHANGE_OF_VAR_PARAM";  //参数变更
std::string LayerRelevanceChangeTypeEnum::CHANGE_OF_LAUNCH = "CHANGE_OF_LAUNCH";        //Launch层信息变更
std::string LayerRelevanceChangeTypeEnum::CHANGE_OF_LAYER = "CHANGE_OF_LAYER";          //实验层信息变更

/**
 * 实验状态枚举类
 */
std::pair<std::string, int> LabStateEnum::NOT_STARTING = std::make_pair("not_starting", 0);
std::pair<std::string, int> LabStateEnum::APPLYING = std::make_pair("applying", 1);
std::pair<std::string, int> LabStateEnum::EFFECTIVE = std::make_pair("effective", 2);
std::pair<std::string, int> LabStateEnum::FINISHED = std::make_pair("finished", 3);
std::pair<std::string, int> LabStateEnum::DELETED = std::make_pair("deleted", -1);

/**
 * 获取远程配置枚举类
 */
std::string FetchRemoteConfigTypeEnum::INITIALIZE = "INITIALIZE"; //初始化
std::string FetchRemoteConfigTypeEnum::SCHEDULE = "SCHEDULE";     //定时
std::string FetchRemoteConfigTypeEnum::LONG_POLLING = "LONG_POLLING";      //长轮询

/**
 * 返回结果信息集
 */
std::pair<int, std::string> ResultConstants::SUCCESS = std::make_pair(0, "");

std::pair<int, std::string> ResultConstants::ERROR = std::make_pair(10000, "service error");

std::pair<int, std::string> ResultConstants::AUTH_HEADER_EMPTY = std::make_pair(10001, "vep auth header is empty");

std::pair<int, std::string> ResultConstants::AUTH_TIME_ERROR = std::make_pair(10002, "vep auth time is error");

std::pair<int, std::string> ResultConstants::BIZ_SCENE_NOT_EXIST = std::make_pair(10003, "bizScene is not existed");

std::pair<int, std::string> ResultConstants::AUTH_FAILED = std::make_pair(10004, "vep auth failed");

std::pair<int, std::string> ResultConstants::EMPTY_LAB_NAME = std::make_pair(10005, "labName is empty");

std::pair<int, std::string> ResultConstants::EMPTY_SHUNT = std::make_pair(10006, "shunt is empty");

std::pair<int, std::string> ResultConstants::EMPTY_PARAM = std::make_pair(10007, "shunt AND tag are empty");

std::pair<int, std::string> ResultConstants::USING_API_NOT_CONSIST_WITH_LAB_CONFIG = std::make_pair(10008, "使用api与实验配置方式不匹配");

std::pair<int, std::string> ResultConstants::EMPTY_TAG = std::make_pair(10007, "tag is empty");

std::pair<int, std::string> ResultConstants::INIT_ERROR = std::make_pair(20001, "initialized error");

std::pair<int, std::string> ResultConstants::NOT_FOUND_SCENE = std::make_pair(20002, "not found scene");

std::pair<int, std::string> ResultConstants::NOT_FOUND_LAB = std::make_pair(20003, "not found lab");

std::pair<int, std::string> ResultConstants::LAB_HAS_FINISHED = std::make_pair(20004, "lab has finished");

std::pair<int, std::string> ResultConstants::NOT_FOUND_LAB_VERSION = std::make_pair(20005, "not found labVersion");

std::pair<int, std::string> ResultConstants::LAYER_NOT_FOUND = std::make_pair(11000, "layer is not found");

std::pair<int, std::string> ResultConstants::LAYER_HAS_DISABLED = std::make_pair(11001, "layer has disabled");

std::pair<int, std::string> ResultConstants::LAYER_MATCH_MISSING = std::make_pair(11002, "has not match any experiment");

/**
 * 动态配置Key常量_枚举类
 */
//默认的接口服务地址
std::pair<std::string, std::string> DynamicConfigKeyEnum::DEFAULT_API_SERVER_URL = std::make_pair("default.api.server.url", "http://10.101.19.232:8080");  //http://vep-api-prd.vivo.lan:8080
//客户端上报日志_接口服务地址
std::pair<std::string, std::string> DynamicConfigKeyEnum::CLIENT_REPORT_API_SERVER_URL = std::make_pair("client.report.api.server.url", "http://10.101.19.232:8080");
//请求超时时间
std::pair<std::string, std::string> DynamicConfigKeyEnum::REQUEST_TIMEOUT_MILLISECONDS = std::make_pair("request.timeout.milliseconds", "5000");
//请求长轮询_超时时间
std::pair<std::string, std::string> DynamicConfigKeyEnum::LONG_POLLING_TIMEOUT_MILLISECONDS = std::make_pair("long.polling.timeout.milliseconds", "120000");
//定时执行长轮询的间隔
std::pair<std::string, std::string> DynamicConfigKeyEnum::SCHEDULED_WATCH_INTERVAL = std::make_pair("scheduled.watch.interval", "200");
//定时获取配置的时间间隔
std::pair<std::string, std::string> DynamicConfigKeyEnum::SCHEDULED_FETCH_INTERVAL = std::make_pair("scheduled.fetch.interval", "60000");
//定时上报客户端信息的时间间隔
std::pair<std::string, std::string> DynamicConfigKeyEnum::SCHEDULED_REPORT_INTERVAL = std::make_pair("scheduled.report.interval", "90000");
//获取配置信息错误_拉取次数阈值
std::pair<std::string, std::string> DynamicConfigKeyEnum::FETCH_FAILED_COUNT_THRESHOLD = std::make_pair("fetch.failed.count.threshold", "3");
//埋点字段集配置:样例：algCode,trafficRatio
std::pair<std::string, std::string> DynamicConfigKeyEnum::BURIED_LOG_FIELDS = std::make_pair("buried.log.fields", "");

/**
 * 流量分配类型枚举类, segment为号段分配, proportion为占比分配
 */
std::string TrafficAllocationTypeEnum::SEGMENT = "segment";
std::string TrafficAllocationTypeEnum::PROPORTION = "proportion";
std::string TrafficAllocationTypeEnum::TAG = "tag";
std::string TrafficAllocationTypeEnum::TAG_WITH_SEGMENT = "tag_with_segment";
std::string TrafficAllocationTypeEnum::REASONING = "reasoning";

bool TrafficAllocationTypeEnum::hasTag(const std::string &traffic_allocation_type_enum){
    return (TrafficAllocationTypeEnum::TAG == traffic_allocation_type_enum)
            || (TrafficAllocationTypeEnum::TAG_WITH_SEGMENT == traffic_allocation_type_enum);
}

bool TrafficAllocationTypeEnum::hasSeg(const std::string &traffic_allocation_type_enum){
    return (TrafficAllocationTypeEnum::SEGMENT == traffic_allocation_type_enum)
            || (TrafficAllocationTypeEnum::TAG_WITH_SEGMENT == traffic_allocation_type_enum)
            || (TrafficAllocationTypeEnum::PROPORTION == traffic_allocation_type_enum);
}

/**
 * 本地配置
 */
std::string VepLocalProperties::app_name = "vep-sdk";
std::string VepLocalProperties::client_version = "2.2.0.20191220-SNAPSHOT";
std::string VepLocalProperties::client_ip = "";

/**
 * 拉取配置中心
 */
std::string VepPropertySource::app_env = "dev";
std::string VepPropertySource::app_loc = "sz-zk";
std::string VepPropertySource::config_host = "vivocfg-agent.test.vivo.xyz/vivocfg";
