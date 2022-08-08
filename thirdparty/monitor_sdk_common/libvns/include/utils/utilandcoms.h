//
// Created by 吴婷 on 2020-06-15.
//

#ifndef LIBVNS_UTILANDCOMS_H
#define LIBVNS_UTILANDCOMS_H

#include <string>

/**
 * http请求方法
 */
class HttpMethod{
public:
    static std::string GET;
    static std::string POST;
    static std::string PUT;
    static std::string DELETE;
};

class UtilAndComs {
public:
    static std::string VERSION;

    static std::string Agent_INIT_URL;

    static std::string Agent_URL;

    static std::string WEB_CONTEXT;

    static std::string NACOS_URL_BASE;

    static std::string NACOS_URL_TOKEN;

    static std::string NACOS_URL_INSTANCE;

    static std::string NACOS_URL_SERVICE;

    static std::string ENCODING;

    static std::string ENV_LIST_KEY;

    static std::string ALL_IPS;

    static std::string FAILOVER_SWITCH;

    static std::string DEFAULT_NAMESPACE_ID;

    static int REQUEST_DOMAIN_RETRY_COUNT;

    static std::string NACOS_NAMING_LOG_NAME;

    static std::string NACOS_NAMING_LOG_LEVEL;

    static std::string SERVER_ADDR_IP_SPLITER;

    static int DEFAULT_CLIENT_BEAT_THREAD_COUNT; //TODO:Calc this according to nr_processors of the host

    static int DEFAULT_POLLING_THREAD_COUNT;    //TODO:Calc this according to nr_processors of the host

};

class ResultVo{
public:
    std::string toString();

public:
    int code = -1;          // 0表示成功，非0失败
    std::string message;
    std::string data;
};

#endif //LIBVNS_UTILANDCOMS_H
