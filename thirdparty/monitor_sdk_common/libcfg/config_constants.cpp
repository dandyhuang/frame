//
// Created by 黄伟锋 on 2019-04-10.
//

#include "thirdparty/monitor_sdk_common/libcfg/config_constants.h"

using namespace common;

// 编码格式
std::string ConfigConstants::DEFAULT_CHARSET_NAME = "UTF-8";

// 内容类型
std::string ConfigConstants::DEFAULT_CONTENT_TYPE = "application/json";

// 本地缓存文件名前缀
std::string ConfigConstants::LOCAL_CACHE_FILE_NAME_PREFIX = "vivocfg-v2-";

// 本地缓存文件名后缀
std::string ConfigConstants::LOCAL_CACHE_FILE_NAME_SUFFIX = ".cache";

// 基础包名
std::string ConfigConstants::BASE_PACKAGE_NAME = "com.vivo.internet.vivocfg.client";

// 版本信息配置文件
std::string ConfigConstants::VERSION_PROPERTIES_FILE = "version.properties";

// 模块名
std::string ConfigConstants::MODULE_NAME = "module.name";

// 默认模块名
std::string ConfigConstants::DEFAULT_MODULE_NAME = "vivocfg-client-cpp";

// 模块版本
std::string ConfigConstants::MODULE_VERSION = "module.version";

// 未知模块版本
std::string ConfigConstants::UNKNOWN_MODULE_VERSION = "unknown";

// 未知主机HostName
std::string ConfigConstants::UNKNOWN_HOST_NAME = "UnknownHost";

// 未知主机IP地址
std::string ConfigConstants::UNKNOWN_IP_ADDRESS = "UnknownIp";

// 配置中心Zookeeper根目录
std::string ConfigConstants::CFG_ZOOKEEPER_ROOT = "/vivocfg";

// 应用名
std::string ConfigConstants::APP_NAME = "app.name";

// 应用环境
std::string ConfigConstants::APP_ENV = "app.env";

// 配置版本
std::string ConfigConstants::CONFIG_VERSION = "config.version";

// 应用部署机房
std::string ConfigConstants::APP_LOC = "app.loc";

// 配置中心地址
std::string ConfigConstants::CONFIG_HOST = "config.host";

// 配置中心agent地址
std::string ConfigConstants::CONFIG_AGENT = "config.agent";

// 配置监控上报地址
std::string ConfigConstants::CONFIG_MONITOR = "config.monitor";

// Http读取配置超时时间（单位：毫秒）
std::string ConfigConstants::CONFIG_HTTP_TIMEOUT = "config.http.timeout";

// http长轮询是否开启
std::string ConfigConstants::CONFIG_HTTP_LONG_POLLING_ENABLED = "config.http.long.polling.enabled";

// vivo-commons spi是否开启加载
std::string ConfigConstants::CONFIG_VIVO_COMMONS_SPI_ENABLED = "config.dandy.commons.spi.enabled";

// 默认应用名
std::string ConfigConstants::DEFAULT_APP_NAME = "";

//默认应用环境
std::string ConfigConstants::DEFAULT_APP_ENV = "";

// 默认配置版本
std::string ConfigConstants::DEFAULT_CONFIG_VERSION = "1";

// 默认应用部署机房
std::string ConfigConstants::DEFAULT_APP_LOC = "";

// 默认配置中心地址
std::string ConfigConstants::DEFAULT_CONFIG_HOST = "";

// 默认配置中心agent地址
std::string ConfigConstants::DEFAULT_CONFIG_AGENT = "";

// 默认配置监控上报地址
std::string ConfigConstants::DEFAULT_CONFIG_MONITOR = "";

// 未知配置中心地址scheme
std::string ConfigConstants::UNKNOWN_CONFIG_HOST_SCHEME = "unknown";

// 空字符串
std::string ConfigConstants::EMPTY_STRING_VALUE = "";

// 是否开启远程配置中心
std::string ConfigConstants::ENABLE_REMOTE_CONFIG = "enable.remote.config";

// 配置业务日志目录
std::string ConfigConstants::CONFIG_BIZ_LOG_DIR = "config.client.biz.log.dir";

// 是否开启远程配置中心, 默认开启
bool ConfigConstants::DEFAULT_ENABLE_REMOTE_CONFIG = true;

// Http 读取配置超时时间默认值，3000 ms
int ConfigConstants::DEFAULT_CONFIG_HTTP_TIMEOUT = 3000;

// 客户端 lastModified 等于配置当前更新时间戳
int ConfigConstants::CFG_AGENT_RETCODE_NOT_MODIFIED = 21304;

// 客户端 lastModified 大于配置当前更新时间戳（客户端打warn日志）
int ConfigConstants::CFG_AGENT_RETCODE_BAD_TIMESTAMP = 21400;

// 服务端长连接超出负载
int ConfigConstants::CFG_AGENT_RETCODE_TOO_MANY_LONG_POLLING_CONNECTION = 21001;

// 配置版本变化
int ConfigConstants::CFG_AGENT_RETCODE_ERR_CODE_CONFIG_VERSION_CHANGED = 21302;

// false
std::string ConfigConstants::FALSE = "false";

// true
std::string ConfigConstants::TRUE = "true";

