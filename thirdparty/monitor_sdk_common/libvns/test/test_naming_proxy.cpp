//
// Created by 吴婷 on 2020-07-03.
//
#include <iostream>
#include <signal.h>

#include "naming_proxy.h"
#include "thirdparty/monitor_sdk_common_params.h"
#include "utils/utils.h"
#include "utils/log_utils.h"

#include "thirdparty/glog/logging.h"

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

    //日志
    std::shared_ptr<VnsLogger> logger(new TestVnsLogger());
    LogUtils::Instance()->SetLogger(logger);
    LogUtils::LogInfo("日志测试!");

    // 服务信息
    std::string service_name = "HelloServer"; // 服务名
    std::string clusters = "set1";

    // 参数
    std::string tag = "serverlist";
    std::map<std::string, std::string> props;
    props[PropertyKeyConst::APP_NAME] = "vns-naming";  // 应用名
    props[PropertyKeyConst::CLUSTER_NAME] = tag; // 集群名
    // 注释掉下面2行，查看权限错误时，逻辑运行情况
    props[PropertyKeyConst::APP_SECRET] = "testit";         // 密钥
    props[PropertyKeyConst::APP_ENV] = "dev";               // 环境
    // 实例
    std::string ip = "10.101.11.159";
    int port = 3000;
    Instance instance;
    instance.ip = ip;
    instance.port = port;


    // proxy
    std::shared_ptr<NamingProxy> server_proxy = std::make_shared<NamingProxy>("dev", tag);
    // 传入参数信息
    server_proxy->properties = props;

    // 接口1：启动接口测试
    server_proxy->init();           // code返回测试

    // 接口2：注册服务
    server_proxy->registerService(NamingUtils::getGroupedName(service_name, clusters), clusters, instance);  // 查看返回结果

    // 接口3：实例查询（host刷新缓存）
    std::string result = server_proxy->queryList(service_name, clusters, 1111, false);
    std::cout << "queryList 结果: " << result << std::endl;

    // 接口4：心跳返回值
    BeatInfo beatInfo;
    int num = server_proxy->sendBeat(beatInfo);
    std::cout << "sendBeat 返回下次心跳间隔: " << num << std::endl;

    // 接口5：注销
    server_proxy->deregisterService(service_name, instance);

    result = server_proxy->queryList(service_name, clusters, 1111, false);
    std::cout << "queryList 结果: " << result << std::endl;


    std::cout << "hello" << std::endl;

    //保持程序开启
    while (!s_quit_flag) {
        sleep(1);
    }

    return 0;
}


