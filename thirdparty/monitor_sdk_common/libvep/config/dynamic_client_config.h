//
// Created by 吴婷 on 2019-11-26.
//

#ifndef COMMON_LIBVEP_DYNAMIC_CLIENT_CONFIG_H
#define COMMON_LIBVEP_DYNAMIC_CLIENT_CONFIG_H

#include <string>
#include <map>
#include <memory>

#include "thirdparty/monitor_sdk_common/base/singleton.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/http/http_invoker.h"
#include "thirdparty/monitor_sdk_common/libvep/vep_scheduled_executor.h"

namespace common {
/**
 * 从配置中心拉取客户端动态配置，如api地址等
 */
class DynamicClientConfig : public SingletonBase<DynamicClientConfig>{
public:
    void initialize();               //初始化：首次读取配置中心的动态配置信息
    void DynamicClientConfigScheduled();      //定时读取配置中心的动态配置信息
    std::string getVal(const std::string &key, const std::string &def_val);  //从map中获取配置
    static int getScheduledFetchInterval();    //定时获取配置的时间间隔
    static int getScheduledWatchInterval();    //定时执行长轮询的间隔
    static int getScheduledReportInterval();   //定时上报客户端信息的时间间隔
    static int getFetchFailedCountThreshold(); //获取配置信息错误_拉取次数阈值

private:
    std::string buildUltimateUrl(); //构造url地址
    bool initializeVivoCfgData(const std::string &ultimate_url);    //首次拉取配置
    std::map<std::string, std::string> handleVivoCfgData(const std::string &ultimate_url);   //根据url进行拉取，填充map

private:
    std::map <std::string, std::string> config_map_;     //配置信息Map
    std::shared_ptr<VepHttpInvoker> http_invoker_ptr_;   //Http请求器
    long last_modified_ = 0l;;                      //最新拉取配置的时间，默认为0l
    bool initialize_ = false;            //是否成功初始化

};
}

#endif //COMMON_LIBVEP_DYNAMIC_CLIENT_CONFIG_H
