//
// Created by 杨民善 on 2020/6/20.
//

#ifndef BLADE_BASE_COMMON_GENERAL_MONITOR_REPORT_H
#define BLADE_BASE_COMMON_GENERAL_MONITOR_REPORT_H

#include "context/monitor_context.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <atomic>

namespace vivo_general_monitor_cpp_report
{
    class GeneralMonitorReport
    {
    public:
        GeneralMonitorReport() {}
        virtual ~GeneralMonitorReport(){}

        /**
         * 初始化函数，在上报开始前初始化一次即可
         *
         * @param token      通用监控分配的鉴权标识，可以联系齐军、贺炎分配
         * @param vtRuleId   需要上报的 rule id 集合，包含调用 report 上报的 rule id 即可
         * @param spaceId    通用监控分配的空间 id，可以联系齐军、贺炎分配
         * @param configUrl  通用监控相关配置获取地址，对应不同环境，可以联系齐军、贺炎要不同环境的地址
         * @param appCode    配置、自监控识别码，建议使用应用_服务名称的组合（不重复即可，如：rec_rankserver）。如果appCode不对，则线程数和缓存数会使用默认配置；不影响上报结果，但是可能影响自监控数据查看
         * @param glogLevel  log 级别，默认是 error，有默认值可不传。0-INFO 级别日志；1-WARNING 级别日志；2-ERROR 级别日志；3-FATAL 级别日志；
         * @param localIp    本机ip，不传自动获取 eth0 ip 地址。如果 eth0 地址有错，建议手动传入
         * @param isAsync    是否异步执行，true-异步；false-同步。当前未实现，默认值即可
         * @return 注册是否成功。0-成功；非0-失败；可参考 util.h 中的 MONITOR_CPP_REPORT_RESULT
         */
        static int init(const std::string &token, const std::vector<int> &vtRuleId, const std::string &spaceId
                        , const std::string &configUrl, const std::string &appCode, const int &glogLevel = 2
                        , const std::string &localIp = "", bool isAsync = true);

        /**
         * 设置 glog 日志等级，可在运行时切换，如对性能要求高建议 error 级别
         * 目前 sdk 只使用了 info、error 两种级别的日志
         * @param glogLevel  log 级别，默认是 error，有默认值可不传。0-INFO 级别日志；1-WARNING 级别日志；2-ERROR 级别日志；3-FATAL 级别日志；
         * @return void
         */
        static void setLogLevel(const int &glogLevel);

        /**
         * 获取当前版本号
         * @param void
         * @return 当前版本号
         */
        static std::string getVersion();

        /**
         * 上报函数，在 init 初始化成功后才可正常调用
         *
         * @param ruleId    上报数据对应的采集规则 id，有疑问可联系齐军、贺炎
         * @param timeStamp 上报数据时间戳，单位：秒
         * @param mapStat   上报数据集合，将维度和指标按业务需求以 key-value 的方式传入即可。注意上报函数使用了移动语义，调用后map数据会被转移，不可再使用mapStat
         * @return 上报结果，0-成功；非0-失败；可参考 util.h 中的 MONITOR_CPP_REPORT_RESULT
         */
        static int report(const int &ruleId, const long &timeStamp, ReportMap &mapStat);

        /**
         * 资源回收函数，在进程退出时执行
         *
         * @return void
         */
        static void destroy();

    private:
        static int startTask(MonitorContext &ctx);
        static void stopTask();
        static void releaseQueueData();
        static void releaseBufferData();

    private:
        static MonitorContext m_monitorContext;
        static std::atomic<bool> m_isInit;
    };
}

#endif //BLADE_BASE_COMMON_GENERAL_MONITOR_REPORT_H
