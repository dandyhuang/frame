//
// Created by 黄伟锋 on 2019-04-10.
//

#ifndef COMMON_LIBCFG_GLOBAL_CLIENT_SETTINGS_H_
#define COMMON_LIBCFG_GLOBAL_CLIENT_SETTINGS_H_

#include <string>

namespace common {

class GlobalClientSettings {
 public:
  static int scheduled_fetch_interval() {
    return config_http_long_polling_enabled ? default_scheduled_fetch_interval : default_scheduled_fetch_interval_without_long_polling;
  }

  // 长轮询http超时时间（单位：ms）
  static int long_polling_http_timeout;

  // 配置拉取http请求的超时时长（毫秒）
  static int config_http_timeout;

  // 开启配置长轮询
  static bool config_http_long_polling_enabled;

  // 配置定时拉取间隔时长（毫秒）
  static int default_scheduled_fetch_interval;

  // 不进行长轮询时的配置定时拉取间隔时长（毫秒）
  static int default_scheduled_fetch_interval_without_long_polling;

  // 是否禁用vivo-commons SPI加载时加载配置中心
  static bool config_commons_spi_enabled;

  // 是否暂停长轮询
  static bool http_long_polling_paused;

  // 暂停长轮询休眠时长（毫秒），休眠1小时
  static int http_long_polling_pause_milliseconds;
};

} // namespace common

#endif // LIBCFG_GLOBAL_CLIENT_SETTINGS_H_
