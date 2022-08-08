//
// Created by 吴婷 on 2020-07-28.
//
#include <map>
#include <iostream>
#include <memory>
#include <vector>
#include <signal.h>

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
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

    // 1.传入参数
    std::map<std::string, std::string> props;
    props[PropertyKeyConst::APP_NAME] = "soa-provider-test";       // 应用名（必填）
    props[PropertyKeyConst::CLUSTER_NAME] = "serverlist2";   // tag（必填），与ip地址对应
    props[PropertyKeyConst::APP_SECRET] = "testit";         // 权限校验: 密钥（必填）
    props[PropertyKeyConst::APP_ENV] = "prd";               // 权限校验: 环境（必填）
    //props[PropertyKeyConst::NAMESPACE] = "test";             // 空间（选填）
    std::string ip = NetUtils::localIP();                       // 必须是app下面的一个ip：注册实例ip必须为本机ip


    // 2.名字服务
    std::shared_ptr<NamingService> naming_service = std::make_shared<NamingService>(props);
    // 启动名字服务连接
    naming_service->init();


    // 3.服务信息
    // 服务名
    std::string service_name = "HelloServer";
    std::string service_name2 = "HelloServer2";
    // 实例
    int port = 3001;
    Instance instance;
    instance.ip = ip;                                     // 与本地LocalIp需要是同一个
    instance.port = port;

    // 4.注册
    std::cout << "等待平台注册..."<< std::endl;
    long start = VnsTimeUtils::GetCurrentTimeMills();     // 开始时间
    naming_service->registerInstance(service_name, "group1", instance);     // 注册HelloServer
    long end = VnsTimeUtils::GetCurrentTimeMills();       // 结束时间

    naming_service->registerInstance(service_name2, "group2", instance);    // 注册HelloServer2
    std::cout << "平台注册完成, 服务:" << service_name << "注册耗时:" << (end-start) << "毫秒" << std::endl;

    // 5.注销
    // 60分钟后自动注销实例
    sleep(60 * 60);
    naming_service->deregisterInstance(service_name2, "group2", instance);
    naming_service->deregisterInstance(service_name, "group1", instance);


    //保持程序开启
    while (!s_quit_flag) {
        sleep(1);
    }

    return 0;
}


