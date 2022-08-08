//
// Created by 吴婷 on 2020-06-15.
//

#ifndef COMMON_LIBVNS_CONSTANTS_H
#define COMMON_LIBVNS_CONSTANTS_H

#include <string>

/**
 * http响应状态码
 */
class HttpStatusCode {
public:
    const static int OK;
    const static int Unauthorized;
    const static int InternalServerError;
};

class Constants {
public:
    const static std::string CLIENT_VERSION;

    const static int DATA_IN_BODY_VERSION;

    const static std::string DEFAULT_GROUP;

    const static std::string APPNAME;

    const static std::string UNKNOWN_APP;

    const static std::string DEFAULT_DOMAINNAME;

    const static std::string DAILY_DOMAINNAME;

    const static std::string NUL;

    const static std::string DATAID;

    const static std::string GROUP;

    const static std::string LAST_MODIFIED;

    const static std::string ACCEPT_ENCODING;

    const static std::string CONTENT_ENCODING;

    const static std::string PROBE_MODIFY_REQUEST;

    const static std::string PROBE_MODIFY_RESPONSE;

    const static std::string PROBE_MODIFY_RESPONSE_NEW;

    const static std::string USE_ZIP;

    const static std::string CONTENT_MD5;

    const static std::string CONFIG_VERSION;

    const static std::string IF_MODIFIED_SINCE;

    const static std::string SPACING_INTERVAL;

    const static std::string BASE_PATH;

    const static std::string CONFIG_CONTROLLER_PATH;

    /**
     * second
     */
    const static int ASYNC_UPDATE_ADDRESS_INTERVAL;

    /**
     * second
     */
    const static int POLLING_INTERVAL_TIME;

    /**
     * millisecond
     */
    const static int ONCE_TIMEOUT;

    /**
     * millisecond
     */
    const static int SO_TIMEOUT;

    /**
     * millisecond
     */
    const static int RECV_WAIT_TIMEOUT;

    const static std::string ENCODE;

    const static std::string MAP_FILE;

    const static int FLOW_CONTROL_THRESHOLD;

    const static int FLOW_CONTROL_SLOT;

    const static int FLOW_CONTROL_INTERVAL;

    const static std::string LINE_SEPARATOR;

    const static std::string WORD_SEPARATOR;

    const static std::string LONGPOLLING_LINE_SEPARATOR;

    const static std::string CLIENT_APPNAME_HEADER;
    const static std::string CLIENT_REQUEST_TS_HEADER;
    const static std::string CLIENT_REQUEST_TOKEN_HEADER;

    const static int ATOMIC_MAX_SIZE;

    const static std::string NAMING_INSTANCE_ID_SPLITTER;
    const static int NAMING_INSTANCE_ID_SEG_COUNT;
    const static std::string NAMING_HTTP_HEADER_SPILIER;

    const static std::string DEFAULT_CLUSTER_NAME;

    const static std::string DEFAULT_CACHE_PATH;

    const static std::string DEFAULT_NAMESPACE_ID;

    const static int WRITE_REDIRECT_CODE;

    const static std::string SERVICE_INFO_SPLITER;

    const static std::string NULL_STRING;

    const static std::string FILE_SEPARATOR;

    const static long DEFAULT_HEART_BEAT_TIMEOUT;
    const static long DEFAULT_IP_DELETE_TIMEOUT;
    const static long DEFAULT_HEART_BEAT_INTERVAL;
};


#endif //COMMON_LIBVNS_CONSTANTS_H
