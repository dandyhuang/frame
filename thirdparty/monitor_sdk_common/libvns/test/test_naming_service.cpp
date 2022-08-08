//
// Created by 吴婷 on 2020-07-02.
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

    // 参数
    std::map<std::string, std::string> props;
    props[PropertyKeyConst::APP_NAME] = "vns-naming";       // 应用名
    props[PropertyKeyConst::CLUSTER_NAME] = "serverlist";   // tag必填
    props[PropertyKeyConst::NAMESPACE] = "press";             // 空间
    props[PropertyKeyConst::APP_SECRET] = "testit";         // 密钥
    props[PropertyKeyConst::APP_ENV] = "dev";               // 环境
    props[PropertyKeyConst::CACHE_PATH] = "/data/vns";      // 设置缓存路径
    std::string ip = NetUtils::localIP();                       // 必须是app下面的一个ip


    // 名字服务
    std::shared_ptr<NamingService> naming_service = std::make_shared<NamingService>(props);
    // 选设，默认为1111
    naming_service->setUDPPort(1112);               // 设置接收推送的UDP端口
    // 启动名字服务连接
    naming_service->init();

    // 服务名
    std::string service_name = "HelloServer";
    std::string service_name2 = "HelloServer2";
    // 实例
    //std::string ip = "10.101.33.58";            // 与本地LocalIp需要是同一个
    int port = 3000;
    Instance instance;
    instance.ip = ip;
    instance.port = port;
    // 注册
    naming_service->registerInstance(service_name, "group1", instance);
    naming_service->registerInstance(service_name2, "group2", instance);
    std::cout << "等待平台注册..."<< std::endl;
    sleep(3);    // 等待平台注册

    /*// 订阅
    std::vector<Instance> instances;
    std::vector<std::string> clusters;

    instances = naming_service->getAllInstances(service_name, "group1", clusters, true);
    int count = instances.size();
    if(count > 0){
        std::cout << "返回实例数: " << count << std::endl;
        for (int i = 0; i < count; ++i) {
            std::cout << "---------------订阅: " << "[group1@@" << service_name << "]" << instances[i].toString() << std::endl;
        }
    } else {
        std::cout << "没有返回任何实例!" << std::endl;
    }

    // 更改服务信息，查看是否push更改
    Instance r_instance;
    r_instance.ip = ip;
    r_instance.port = 1188;
    naming_service->registerInstance(service_name, "group1", r_instance); // 注册
    sleep(15);      // 暂停15秒，默认pull定时10秒拉取一次

    // 仅订阅健康节点
    instances = naming_service->selectInstances(service_name, "group1", clusters, true, true);
    count = instances.size();
    if(count > 0){
        std::cout << "返回实例数: " << instances.size() << std::endl;
        for (int i = 0; i < count; ++i) {
            std::cout << "---------------健康订阅: " << "[group1@@" << service_name << "]" << instances[i].toString() << std::endl;
        }
    } else {
        std::cout << "没有返回任何实例!" << std::endl;
    }

    // 订阅2
    sleep(30);       // 暂停30秒，期间立刻开启容灾开关，查看效果
    std::vector<Instance> instances2;
    std::vector<std::string> clusters2;
    instances2 = naming_service->getAllInstances(service_name2, "group2", clusters2, true);
    count = instances2.size();
    if(count > 0){
        std::cout << "返回实例数: " << instances2.size() << std::endl;
        for (int i = 0; i < count; ++i) {
            std::cout << "---------------订阅: " << "[group2@@" << service_name2 << "]" << instances2[i].toString() << std::endl;
        }
    } else {
        std::cout << "没有返回任何实例!" << std::endl;
    }

    // 60分钟后注销
    sleep(60 * 60);
    // 注销
    //naming_service->deregisterInstance(service_name2, "group2", instance);
    //naming_service->deregisterInstance(service_name, "group1", instance);
    //naming_service->deregisterInstance(service_name, "group1", r_instance);*/


    std::cout << "hello" << std::endl;

    //保持程序开启
    while (!s_quit_flag) {
        sleep(1);
    }

    return 0;
}

