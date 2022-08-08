//
// Created by 吴婷 on 2020-06-15.
//

#include "utils/utilandcoms.h"

std::string HttpMethod::GET = "GET";
std::string HttpMethod::POST = "POST";
std::string HttpMethod::PUT = "PUT";
std::string HttpMethod::DELETE = "DELETE";

//////////////////////////////
std::string UtilAndComs::VERSION = "VNS-CPP-Client:v1.0.1";

// 与agent通信的初始化url
std::string UtilAndComs::Agent_INIT_URL = "http://127.0.0.1:16320/sdk/v1/push";

// 与agent通信的查询url
std::string UtilAndComs::Agent_URL = "http://127.0.0.1:16320/ns/v1/proxy";
//std::string UtilAndComs::Agent_URL = "http://172.25.196.89:8848";   // 石正兴
//std::string UtilAndComs::Agent_URL = "http://172.25.197.89:8848";   // 彭志

std::string UtilAndComs::WEB_CONTEXT = "/naming";    // 更改

std::string UtilAndComs::NACOS_URL_BASE = WEB_CONTEXT + "/v1/ns";

std::string UtilAndComs::NACOS_URL_TOKEN = WEB_CONTEXT + "/v1/extend/token";

std::string UtilAndComs::NACOS_URL_INSTANCE = NACOS_URL_BASE + "/instance";

std::string UtilAndComs::NACOS_URL_SERVICE = NACOS_URL_BASE + "/service";

std::string UtilAndComs::ENCODING = "UTF-8";

std::string UtilAndComs::ENV_LIST_KEY = "envList";

std::string UtilAndComs::ALL_IPS = "000--00-ALL_IPS--00--000";

std::string UtilAndComs::FAILOVER_SWITCH = "00-00---000-VIPSRV_FAILOVER_SWITCH-000---00-00";

std::string UtilAndComs::DEFAULT_NAMESPACE_ID = "public";

int UtilAndComs::REQUEST_DOMAIN_RETRY_COUNT = 3;

std::string UtilAndComs::NACOS_NAMING_LOG_NAME = "com.alibaba.nacos.naming.log.filename";

std::string UtilAndComs::NACOS_NAMING_LOG_LEVEL = "com.alibaba.nacos.naming.log.level";

std::string UtilAndComs::SERVER_ADDR_IP_SPLITER = ":";

// 注册的时候，心跳线程池维护注册实例存活情况
int UtilAndComs::DEFAULT_CLIENT_BEAT_THREAD_COUNT = 4; //TODO:Calc this according to nr_processors of the host

// 订阅的时候，每订阅一个服务，就需要有一个定时监听线程
int UtilAndComs::DEFAULT_POLLING_THREAD_COUNT = 4;     //TODO:Calc this according to nr_processors of the host
