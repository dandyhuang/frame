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

    // 1.传入参数
    std::map<std::string, std::string> props;
    props[PropertyKeyConst::APP_NAME] = "soa-provider-test";       // 应用名（必填）
    //props[PropertyKeyConst::CLUSTER_NAME] = "serverlist";   // tag（选填），默认：serverlist, 与ip地址对应
    props[PropertyKeyConst::APP_SECRET] = "testit";         // 权限校验: 密钥（必填）
    props[PropertyKeyConst::NAMESPACE] = "test";             // 空间（选填）,同时也是权限校验: 环境（必填）
    //props[PropertyKeyConst::CACHE_PATH] = "/tmp/vns";      // 设置缓存路径（选填），注意：运行程序的用户需要拥有对该目录操作的权限
    std::string ip = NetUtils::localIP();                       // 必须是app下面的一个ip：注册实例ip必须为本机ip


    // 2.名字服务
    std::shared_ptr<NamingService> naming_service = std::make_shared<NamingService>(props);
    // 选设，默认为1111
    naming_service->setUDPPort(1112);               // 设置接收推送的UDP端口
    // 启动名字服务连接
    long start, end;
    std::cout << "开始初始化vns..." << std::endl;
    start = VnsTimeUtils::GetCurrentTimeMills();     // 开始时间
    if(!naming_service->init()){
        std::cout << "sdk初始化失败" << std::endl;
        exit(0);
    }
    end = VnsTimeUtils::GetCurrentTimeMills();     // 开始时间
    std::cout << "平台初始化完成, 初始化耗时:" << (end-start) << "毫秒" << std::endl;

    // 3.订阅
    // 服务名
    std::string service_name = "HelloServer";
    std::string service_name2 = "HelloServer2";
    // 集群名：默认为空
    std::vector<std::string> clusters;
    // 返回实例
    std::vector<Instance> instances;
    int count;

    /*
    // 订阅1
    instances = naming_service->getAllInstances(service_name, "group1", clusters, true);  // subscribe为true表示订阅，false仅一次查询
    count = instances.size();       // 返回实例数
    if(count > 0){
        std::cout << "返回实例数: " << count << std::endl;
        for (int i = 0; i < count; ++i) {
            std::cout << "---------------订阅: " << "[group1@@" << service_name << "]" << instances[i].toString() << std::endl;
        }
    } else {
        std::cout << "没有返回任何实例!" << std::endl;
    }
    // 仅订阅健康节点
    instances = naming_service->selectInstances(service_name, "group1", clusters, true, true);
    count = instances.size();
    if(count > 0){
        std::cout << "返回健康实例数: " << instances.size() << std::endl;
        for (int i = 0; i < count; ++i) {
            std::cout << "---------------订阅: " << "[group1@@" << service_name << "]" << instances[i].toString() << std::endl;
        }
    } else {
        std::cout << "没有返回任何实例!" << std::endl;
    }
     */


    // 订阅2
    //sleep(30);       // 暂停30秒，期间立刻开启容灾开关，查看效果
    std::vector<Instance> instances2;
    std::vector<std::string> clusters2;
    std::cout << "开始订阅..." << std::endl;
    start = VnsTimeUtils::GetCurrentTimeMills();     // 开始时间
    instances2 = naming_service->selectInstances(service_name2, true, true);
    //instances2 = naming_service->selectInstances(service_name2, "DEFAULT_GROUP",clusters2,true, true);
    end = VnsTimeUtils::GetCurrentTimeMills();       // 结束时间
    count = instances2.size();
    if(count > 0){
        std::cout << "返回实例数: " << instances2.size() << std::endl;
        std::cout << "订阅完成, 服务:" << service_name << "订阅耗时:" << (end-start) << "毫秒" << std::endl;
        for (int i = 0; i < count; ++i) {
            std::cout << "---------------订阅: " << "[group2@@" << service_name2 << "]" << instances2[i].toString() << std::endl;
        }
    } else {
        std::cout << "没有返回任何实例!" << std::endl;
    }


    std::cout << "hello" << std::endl;

    //保持程序开启
    while (!s_quit_flag) {
        sleep(1);
    }

    return 0;
}


