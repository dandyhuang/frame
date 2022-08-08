//
// Created by 吴婷 on 2020-06-24.
//
#include <iostream>
#include <memory>
#include <vector>
#include <signal.h>

#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

#include "utils/log_utils.h"
#include "utils/utils.h"
#include "core/host_reactor.h"
#include "core/push_receiver.h"
#include "cache/disk_cache.h"

//#define CACHE_PATH "./cache"
#define CACHE_PATH "/data/vns/public"

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

    //gflags::ParseCommandLineFlags(&argc, &argv, true);
    //google::InitGoogleLogging(argv[0]);

    //日志
    std::shared_ptr<VnsLogger> logger(new TestVnsLogger());
    LogUtils::Instance()->SetLogger(logger);
    LogUtils::LogInfo("日志测试!");

    // 参数
    std::map<std::string, std::string> props;
    props[PropertyKeyConst::APP_NAME] = "vns-naming";  // 应用名
    props[PropertyKeyConst::CLUSTER_NAME] = "dev";          // tag必填，与ip地址对应
    props[PropertyKeyConst::NAMESPACE] = "dev";             // 空间
    props[PropertyKeyConst::APP_SECRET] = "testit";         // 密钥
    props[PropertyKeyConst::APP_ENV] = "dev";               // 环境

    std::string ip = "10.101.11.159";
    int port = 3000;
    Instance instance;
    instance.ip = ip;
    instance.port = port;
    std::string service_name = "HelloServer";
    std::string group_name = "set1";

    // HostReactor
    std::shared_ptr<NamingProxy> server_proxy = std::make_shared<NamingProxy>("dev", "dev");
    server_proxy->properties =  props;
    std::shared_ptr<BeatReactor> beat_reactor = std::make_shared<BeatReactor>(server_proxy);
    std::string cache_dir = CACHE_PATH;
    bool load_cache_at_start = true;
    HostReactor host_reactor(server_proxy, cache_dir, load_cache_at_start);


    // HostReactor测试
    host_reactor.initialize();  // 初始化
    // 注册
    server_proxy->registerService(service_name, group_name, instance);

    // 获取服务内容
    // 订阅模式/容灾模式
    std::string name = NamingUtils::getGroupedName(service_name, group_name);
    std::vector<std::string> clusters;
    std::string sclusters = JoinStrings(clusters, ",");
    std::shared_ptr<ServiceInfo> service = host_reactor.getServiceInfo(service_name, sclusters);
    std::cout << "订阅获取服务内容：" << service->toString() <<  std::endl;
    // 单次查询(api)
    // 由于注册没有同步增加心跳，所以很快就会失败
    std::shared_ptr<ServiceInfo> service2 = host_reactor.getServiceInfoDirectlyFromServer(service_name, sclusters);
    std::cout << "单次查询获取服务内容：" << service2->toString() <<  std::endl;
    // 新注册一个服务，然后主动拉取
    host_reactor.updateServiceNow("test2", "");


    /*
    // PushReceiver测试
    std::shared_ptr<PushReceiver> push_task = std::make_shared<PushReceiver>(&host_reactor);
    //启动任务队列
    TaskQueue::Instance()->start();
    //将任务放入任务队列
    TaskQueue::Instance()->push(push_task);
    */


    /*
    // FailoverReactor测试
    //host_reactor.initialize();  // 初始化
    DiskCache::read(cache_dir, host_reactor.serviceInfo_map);   // 往host的map中增加服务
    std::cout << "hostReactor获取服务内容：" << host_reactor.serviceInfo_map["test@@c1"]->toString() <<  std::endl;
    // 定义
    FailoverReactor failover_reactor = FailoverReactor(&host_reactor, cache_dir);
    // 初始化
    failover_reactor.init();
    // 新建刷新任务
    //std::shared_ptr<SwitchRefresher> refresh_task = std::make_shared<SwitchRefresher>(&failover_reactor);
    //refresh_task->run();
    if(failover_reactor.isFailoverSwitch()){
        std::cout << "failover模式开启" << std::endl;
    } else {
        std::cout << "failover模式没有开启" << std::endl;
    }
    std::unordered_map<std::string, std::shared_ptr<ServiceInfo>> service_map = failover_reactor.service_map;
    std::cout << "failover中map服务个数:" << service_map.size() << std::endl;
    // 获取服务
    std::shared_ptr<ServiceInfo> service_f = failover_reactor.getService("test@@c1");
    std::cout << "failover获取服务内容：" << service_f->toString() <<  std::endl;
    */




    std::cout << "hello" << std::endl;


    //保持程序开启
    while (!s_quit_flag) {
        sleep(1);
    }

    std::cout << "关闭程序" << std::endl;

    return 0;
}
