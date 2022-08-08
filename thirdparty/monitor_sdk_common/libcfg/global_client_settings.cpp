

#include "thirdparty/monitor_sdk_common/libcfg/global_client_settings.h"

using namespace common;


// 长轮询http超时时间（单位：ms）
int GlobalClientSettings::long_polling_http_timeout = 70 * 1000;

// 配置拉取http请求的超时时长（毫秒）
int GlobalClientSettings::config_http_timeout = 3 * 1000;

// 开启配置长轮询
bool GlobalClientSettings::config_http_long_polling_enabled = true;

// 配置定时拉取间隔时长（毫秒）
int GlobalClientSettings::default_scheduled_fetch_interval = 15 * 1000;

// 不进行长轮询时的配置定时拉取间隔时长（毫秒）
int GlobalClientSettings::default_scheduled_fetch_interval_without_long_polling = 15 * 1000;

// 是否禁用vivo-commons SPI加载时加载配置中心
bool GlobalClientSettings::config_commons_spi_enabled = true;

// 是否暂停长轮询
bool GlobalClientSettings::http_long_polling_paused = false;

// 暂停长轮询休眠时长（毫秒），休眠1小时
int GlobalClientSettings::http_long_polling_pause_milliseconds = 60 * 60 * 1000;