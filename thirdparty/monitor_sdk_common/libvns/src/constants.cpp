//
// Created by 吴婷 on 2020-06-15.
//

#include "constants.h"

const std::string Constants::CLIENT_VERSION = "3.0.0";

const int Constants::DATA_IN_BODY_VERSION = 204;

const std::string Constants::DEFAULT_GROUP = "DEFAULT_GROUP";   // 默认组别

const std::string Constants::APPNAME = "AppName";

const std::string Constants::UNKNOWN_APP = "UnknownApp";

const std::string Constants::DEFAULT_DOMAINNAME = "commonconfig.config-host.taobao.com";

const std::string Constants::DAILY_DOMAINNAME = "commonconfig.taobao.net";

const std::string Constants::NUL = "";

const std::string Constants::DATAID = "dataId";

const std::string Constants::GROUP = "group";

const std::string Constants::LAST_MODIFIED = "Last-Modified";

const std::string Constants::ACCEPT_ENCODING = "Accept-Encoding";

const std::string Constants::CONTENT_ENCODING = "Content-Encoding";

const std::string Constants::PROBE_MODIFY_REQUEST = "Listening-Configs";

const std::string Constants::PROBE_MODIFY_RESPONSE = "Probe-Modify-Response";

const std::string Constants::PROBE_MODIFY_RESPONSE_NEW = "Probe-Modify-Response-New";

const std::string Constants::USE_ZIP = "true";

const std::string Constants::CONTENT_MD5 = "Content-MD5";

const std::string Constants::CONFIG_VERSION = "Config-Version";

const std::string Constants::IF_MODIFIED_SINCE = "If-Modified-Since";

const std::string Constants::SPACING_INTERVAL = "client-spacing-interval";

const std::string Constants::BASE_PATH = "/v1/cs";

const std::string Constants::CONFIG_CONTROLLER_PATH = BASE_PATH + "/configs";

/**
* second
*/
const int Constants::ASYNC_UPDATE_ADDRESS_INTERVAL = 300;

/**
* second
*/
const int Constants::POLLING_INTERVAL_TIME = 15;

/**
* millisecond
*/
const int Constants::ONCE_TIMEOUT = 2000;

/**
* millisecond
*/
const int Constants::SO_TIMEOUT = 60000;

/**
* millisecond
*/
const int Constants::RECV_WAIT_TIMEOUT = ONCE_TIMEOUT * 5;

const std::string Constants::ENCODE = "UTF-8";

const std::string Constants::MAP_FILE = "map-file.js";

const int Constants::FLOW_CONTROL_THRESHOLD = 20;

const int Constants::FLOW_CONTROL_SLOT = 10;

const int Constants::FLOW_CONTROL_INTERVAL = 1000;

const std::string Constants::LINE_SEPARATOR = "\x1";

const std::string Constants::WORD_SEPARATOR = "\x2";

const std::string Constants::LONGPOLLING_LINE_SEPARATOR = "\r\n";

const std::string Constants::CLIENT_APPNAME_HEADER = "Client-AppName";
const std::string Constants::CLIENT_REQUEST_TS_HEADER = "Client-RequestTS";
const std::string Constants::CLIENT_REQUEST_TOKEN_HEADER = "Client-RequestToken";

const int Constants::ATOMIC_MAX_SIZE = 1000;

const std::string Constants::NAMING_INSTANCE_ID_SPLITTER = "#";
const int Constants::NAMING_INSTANCE_ID_SEG_COUNT = 4;
const std::string Constants::NAMING_HTTP_HEADER_SPILIER = "\\|";

const std::string Constants::DEFAULT_CLUSTER_NAME = "serverlist";   // 默认tag

const std::string Constants::DEFAULT_CACHE_PATH = "/tmp/vns/";      // 服务缓存路径

const std::string Constants::DEFAULT_NAMESPACE_ID = "public";

const int Constants::WRITE_REDIRECT_CODE = 307;

const std::string Constants::SERVICE_INFO_SPLITER = "@@";

const std::string Constants::NULL_STRING = "null";

const std::string Constants::FILE_SEPARATOR = "/";

const long Constants::DEFAULT_HEART_BEAT_TIMEOUT = 15 * 1000;  //单位：毫秒
const long Constants::DEFAULT_IP_DELETE_TIMEOUT = 30 * 1000;
const long Constants::DEFAULT_HEART_BEAT_INTERVAL = 5 * 1000;

///////////////////////////
const int HttpStatusCode::OK = 200;                 // http成功返回
const int HttpStatusCode::Unauthorized = 401;       // 未经授权
const int HttpStatusCode::InternalServerError = 500;// 失败