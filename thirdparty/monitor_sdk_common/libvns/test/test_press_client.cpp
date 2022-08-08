//
// Created by 吴婷 on 2020-09-21.
// TODO: 客户端压测
//
#include <map>
#include <iostream>
#include <algorithm>
#include <memory>
#include <vector>
#include <signal.h>

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "utils/log_utils.h"
#include "utils/utils.h"
#include "naming_service.h"
#include "thirdparty/monitor_sdk_common_params.h"

using namespace common;

static bool s_quit_flag = false;

static void sig_graceful_quit(int sig) {
    LOG(INFO) << __FUNCTION__ << " quit, sig=" << sig;
    if (sig == SIGINT) {
        s_quit_flag = true;
    }
    if (sig == SIGTERM) {
        s_quit_flag = true;
    }
    if (sig == SIGQUIT) {
        s_quit_flag = true;
    }
}

/**
 * 打印日志
 */
class TestVnsLogger : public VnsLogger{
public:
    TestVnsLogger(){}
    ~TestVnsLogger(){}

    void LogInfo(const std::string &msg){
        LOG(INFO) << msg;
    }
    void LogError(const std::string &msg){
        LOG(ERROR) << msg;
    }
    void LogWarn(const std::string &msg){
        LOG(WARNING) << msg;
    }
    void LogDebug(const std::string &msg){
        DLOG(INFO) << msg;
    }
};

/**
 * 客户端
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[]){
    signal(SIGINT, &sig_graceful_quit);
    signal(SIGTERM, &sig_graceful_quit);
    signal(SIGQUIT, &sig_graceful_quit);
    signal(SIGPIPE, SIG_IGN);

    gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);

    //日志
    std::shared_ptr<VnsLogger> logger(new TestVnsLogger());
    LogUtils::Instance()->SetLogger(logger);
    LogUtils::LogInfo("日志测试!");

    // 3.订阅
    int all_count = 0;      // 总订阅数
    int fail_count = 0;     // 订阅失败数
    std::vector<long> latency_times; // 订阅耗时：毫秒
    // 集群名：默认为空
    std::vector<std::string> clusters;
    // 返回实例
    std::vector<Instance> instances;

    int app_num = 0;
    std::map<std::string, std::string> props;
    std::vector<std::shared_ptr<NamingService>> naming_services;
    std::vector<std::string> apps = {"A", "B", "C", "D", "E", "F", "G", "I"};
    std::vector<std::string> pres = {"H", "K", "M", "N", "L", "O", "P", "Q"};
    // ABCDE：应用名
    for (int l = 0; l < 1; ++l) {
        for (int k = 0; k < 240; ++k) {     //------240
            props[PropertyKeyConst::APP_NAME] = apps[l] + IntegerToString(k);
            props[PropertyKeyConst::NAMESPACE] = "test";
            std::string ip = NetUtils::localIP();
            props["ip"] = ip;
            props[PropertyKeyConst::APP_LOC] = "sz-sk";     // 压测指定机房环境

            /*
            // 1.传入参数
            std::map<std::string, std::string> props;
            props[PropertyKeyConst::APP_NAME] = "soa-consumer-test";       // 应用名（必填）
            props[PropertyKeyConst::APP_SECRET] = "testit";         // 权限校验: 密钥（必填）
            props[PropertyKeyConst::NAMESPACE] = "test";             // 空间（选填）
            //props[PropertyKeyConst::CACHE_PATH] = "/tmp/vns";      // 设置缓存路径（选填），注意：运行程序的用户需要拥有对该目录操作的权限
            props["ip"] = "10.101.39.30";
             */

            // 2.名字服务
            std::shared_ptr<NamingService> naming_service = std::make_shared<NamingService>(props);
            naming_services.push_back(naming_service);
            // 启动名字服务连接
            naming_service->init();

            // 对应app的前缀
            std::string pre = pres[l];

            // 服务名: 4个服务
            std::vector<std::string> service_names; // 服务名
            for (int i = 0; i < 4; ++i) {
                service_names.push_back(pre + IntegerToString(app_num * 5 + i));
                std::cout <<  service_names[i]<< std::endl;
                all_count++;    // 总订阅数

                // 订阅
                long start = VnsTimeUtils::GetCurrentTimeMills();       // 开始时间
                instances = naming_service->getAllInstances(service_names[i], "group1", clusters, true);  // subscribe为true表示订阅，false仅一次查询
                long end = VnsTimeUtils::GetCurrentTimeMills();         // 结束时间

                int count = instances.size();       // 返回实例数
                if(count > 0){
                    latency_times.push_back((end - start));                 // 本次查询耗时
                    /*
                    std::cout <<"服务名:" << service_names[i] <<  "返回实例数: " << count << std::endl;
                    for (int j = 0; j < count; ++j) {
                        std::cout <<  instances[j].instance_id + ":" + IntegerToString(instances[j].port) << std::endl;
                    }
                     */
                    std::cout << "---------------订阅: " << "[group1@@" << service_names[i] << "]" << instances[i].toString() << std::endl;
                } else {
                    fail_count++;
                    std::cout << "没有返回任何实例!" << std::endl;
                }

            }
        }
        app_num++;
    }


    std::cout << "查询次数统计: "<< std::endl;
    int succ_count = all_count - fail_count;   // 成功数
    std::cout << "总数:" << all_count << ", 成功数:" << succ_count << ", 失败数:" << fail_count << ", 失败率:" << (double)fail_count/all_count * 100 << "%" << std::endl;



    int count = latency_times.size();
    long total_time = 0;  // 总耗时
    for (int l = 0; l < count; ++l) {
        total_time += latency_times[l];
    }
    long min_time = *min_element(latency_times.begin(), latency_times.end());     // 最小耗时
    long max_time = *max_element(latency_times.begin(), latency_times.end());     // 最大耗时
    long avg_time = total_time / count; //平均耗时

    std::cout << "总耗时(毫秒):" << total_time << ", 平均耗时(毫秒):"<< avg_time << ", 最小耗时(毫秒):" << min_time << ", 最大耗时(毫秒):" << max_time << std::endl;


    std::cout << "hello" << std::endl;

    //保持程序开启
    while (!s_quit_flag) {
        sleep(1);
    }

    return 0;
}
