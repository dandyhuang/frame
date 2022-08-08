//
// Created by 黄伟锋 on 2019-04-10.
//

#ifndef COMMON_LIBCFG_CONFIG_CONSTANTS_H_
#define COMMON_LIBCFG_CONFIG_CONSTANTS_H_

#include <string>

namespace common {

// 配置项。
class ConfigConstants {
 public:
  // 编码格式
  static std::string DEFAULT_CHARSET_NAME;

  // 内容类型
  static std::string DEFAULT_CONTENT_TYPE;

  // 本地缓存文件名前缀
  static std::string LOCAL_CACHE_FILE_NAME_PREFIX;

  // 本地缓存文件名后缀
  static std::string LOCAL_CACHE_FILE_NAME_SUFFIX;

  // 基础包名
  static std::string BASE_PACKAGE_NAME;

  // 版本信息配置文件
  static std::string VERSION_PROPERTIES_FILE;

  // 模块名
  static std::string MODULE_NAME;

  // 默认模块名
  static std::string DEFAULT_MODULE_NAME;

  // 模块版本
  static std::string MODULE_VERSION;

  // 未知模块版本
  static std::string UNKNOWN_MODULE_VERSION;

  // 未知主机HostName
  static std::string UNKNOWN_HOST_NAME;

  // 未知主机IP地址
  static std::string UNKNOWN_IP_ADDRESS;

  // 配置中心Zookeeper根目录
  static std::string CFG_ZOOKEEPER_ROOT;

  // 应用名
  static std::string APP_NAME;

  // 应用环境
  static std::string APP_ENV;

  // 配置版本
  static std::string CONFIG_VERSION;

  // 应用部署机房
  static std::string APP_LOC;

  // 配置中心地址
  static std::string CONFIG_HOST;

  // 配置中心agent地址
  static std::string CONFIG_AGENT;

  // 配置监控上报地址
  static std::string CONFIG_MONITOR;

  // Http读取配置超时时间（单位：毫秒）
  static std::string CONFIG_HTTP_TIMEOUT;

  // http长轮询是否开启
  static std::string CONFIG_HTTP_LONG_POLLING_ENABLED;

  // vivo-commons spi是否开启加载
  static std::string CONFIG_VIVO_COMMONS_SPI_ENABLED;

  // 默认应用名
  static std::string DEFAULT_APP_NAME;

  //默认应用环境
  static std::string DEFAULT_APP_ENV;

  // 默认配置版本
  static std::string DEFAULT_CONFIG_VERSION;

  // 默认应用部署机房
  static std::string DEFAULT_APP_LOC;

  // 默认配置中心地址
  static std::string DEFAULT_CONFIG_HOST;

  // 默认配置中心agent地址
  static std::string DEFAULT_CONFIG_AGENT;

  // 默认配置监控上报地址
  static std::string DEFAULT_CONFIG_MONITOR;

  // 未知配置中心地址scheme
  static std::string UNKNOWN_CONFIG_HOST_SCHEME;

  // 空字符串
  static std::string EMPTY_STRING_VALUE;

  // 是否开启远程配置中心
  static std::string ENABLE_REMOTE_CONFIG;

  // 配置业务日志目录
  static std::string CONFIG_BIZ_LOG_DIR;

  // 是否开启远程配置中心, 默认开启
  static bool DEFAULT_ENABLE_REMOTE_CONFIG;

  // Http 读取配置超时时间默认值，3000 ms
  static int DEFAULT_CONFIG_HTTP_TIMEOUT;

  // 客户端 lastModified 等于配置当前更新时间戳
  static int CFG_AGENT_RETCODE_NOT_MODIFIED;

  // 客户端 lastModified 大于配置当前更新时间戳（客户端打warn日志）
  static int CFG_AGENT_RETCODE_BAD_TIMESTAMP;

  // 服务端长连接超出负载
  static int CFG_AGENT_RETCODE_TOO_MANY_LONG_POLLING_CONNECTION;

  // 配置版本变化
  static int CFG_AGENT_RETCODE_ERR_CODE_CONFIG_VERSION_CHANGED;

  // false
  static std::string FALSE;

  // true
  static std::string TRUE;
};

} // namespace common

#endif // LIBCFG_CONFIG_CONSTANTS_H_
