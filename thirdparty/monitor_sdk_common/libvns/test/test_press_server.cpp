//
// Created by 吴婷 on 2020-09-21.
// TODO: 服务端压测
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
 * 服务端
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


    bool isSuccess = true;  // 单次注册成功与否
    int register_count = 0; // 注册总数
    int fail_count = 0;     // 注册失败数
    std::vector<long> latency_times; // 注册耗时：毫秒


    std::vector<std::shared_ptr<NamingService>> naming_services;
    std::vector<std::string> apps = {"A", "B", "C", "D", "E", "F", "G", "I", "X"};
    std::vector<std::string> sers = {"H", "K", "M", "N", "L", "O", "P", "Q", "J"};
    int kk = 0;                      // 机器数----0,1,2,3,4,5,6,7

    if(argc == 2){                  //运行： ./PressServiceTest 1 &
        kk = atoi(argv[1]); //自动字符串转换为整数
    } else if (argc == 3) {         //运行：GLOG_minloglevel=2 ./PressServiceTest 1 &
        // 运行时指定：0,1,2,3,4,5
        kk = atoi(argv[2]);
    }


    int app_num = 0;    // 应用数
    // 每台机器设定: 8台机器
    int given_app_num = 200;        // app数: 300-8台
    int given_server_num = 3;       // 每个app，4个服务
    int given_instance_num = 5;     // 每个服务，5个实例
    //注意： given_server_num * given_instance_num + 1 需要对应更改心跳线程数

    // 240个应用
    // 10.101.32.245: A0, A1
    // 10.101.11.220: A2, A3
    for (int k = 0; k < given_app_num; ++k) {    //------240

        std::string app = apps[kk];      // 不同机器需要改变
        std::string ser = sers[kk];

        // 应用
        std::map<std::string, std::string> props;
        props[PropertyKeyConst::APP_NAME] = app + IntegerToString(k);     // A B C D E
        props[PropertyKeyConst::NAMESPACE] = "test";
        std::string ip = NetUtils::localIP();
        props[PropertyKeyConst::APP_LOC] = "sz-sk";     // 压测指定机房环境

        // todo : 初始化多个NamingService
        // 2.名字服务
        std::shared_ptr<NamingService> naming_service = std::make_shared<NamingService>(props);
        naming_services.push_back(naming_service);
        // 启动名字服务连接
        naming_service->init();
        std::cout << "NamingService appName:" << props[PropertyKeyConst::APP_NAME] << "初始化完成" << std::endl;

        // 3.服务信息
        int port;
        // 服务名: 5个服务
        std::vector<std::string> service_names;
        for (int i = 0; i < given_server_num; ++i) {
            //service_names.push_back(ser + IntegerToString(app_num * 5 + i));  // H K M N L
            service_names.push_back(ser + IntegerToString(app_num * given_server_num + i));
            //std::cout <<  service_names[i]<< std::endl;
            // 实例：8个实例，端口区分
            // 1个实例
            for (int j = 0; j < given_instance_num; ++j) {
                // 端口
                //port = app_num * 40 + i * 8 + j + 1;
                port = app_num * (given_server_num * given_instance_num) + i * given_instance_num + j + 1;
                Instance instance;
                instance.ip = ip;
                instance.port = port;
                // 注册
                register_count++;   // 总数

                long start = VnsTimeUtils::GetCurrentTimeMills();       // 开始时间
                isSuccess = naming_service->registerInstance(service_names[i], "group1", instance);
                long end = VnsTimeUtils::GetCurrentTimeMills();         // 结束时间

                if(isSuccess){
                    latency_times.push_back((end - start));                 // 本次注册耗时
                    LogUtils::LogInfo("平台注册完成, 服务:" + service_names[i] + "实例端口:" + IntegerToString(port));
                    std::cout << "平台注册完成, 服务:" << service_names[i] << "实例端口:" << port << std::endl;
                } else {
                    fail_count++;       // 失败数
                    LogUtils::LogError("平台注册失败, 服务:" + service_names[i] + "实例端口:" + IntegerToString(port) + "失败数:" + IntegerToString(fail_count));
                    std::cout << "------平台注册失败, 服务:" << service_names[i] << "实例端口:" << port << std::endl;
                }
            }
        }

        app_num++;
    }


    std::cout << "注册实例统计: "<< std::endl;
    int succ_count = register_count - fail_count;   // 成功数
    std::cout << "总数:" << register_count << ", 成功数:" << succ_count << ", 失败数:" << fail_count << ", 失败率:" << (double)fail_count/register_count * 100 << "%" << std::endl;


    long total_time = 0;  // 总耗时
    for (int l = 0; l < succ_count; ++l) {
        total_time += latency_times[l];
    }
    if(latency_times.size() > 0) {
        long min_time = *min_element(latency_times.begin(), latency_times.end());     // 最小耗时
        long max_time = *max_element(latency_times.begin(), latency_times.end());     // 最大耗时
        long avg_time = total_time / succ_count; //平均耗时
        std::cout << "总耗时(毫秒):" << total_time << ", 平均耗时(毫秒):"<< avg_time << ", 最小耗时(毫秒):" << min_time << ", 最大耗时(毫秒):" << max_time << std::endl;
    }

    //保持程序开启
    while (!s_quit_flag) {
        sleep(1);
    }

    return 0;
}