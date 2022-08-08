//
// Created by 吴婷 on 2020-06-15.
//
#include <iostream>
#include <memory>
#include <vector>
#include <signal.h>

#include "thirdparty/glog/logging.h"
#include "utils/log_utils.h"
#include "beat/beat_reactor.h"

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

    std::shared_ptr<NamingProxy> server_proxy = std::make_shared<NamingProxy>();
    // 初始化心跳上报
    BeatReactor beat_reactor(server_proxy, 10);     // 线程数必须开大，每次add都加入一个定时调度任务
    beat_reactor.initialize();

    //
    std::string ip = "10.101.97.1";
    int port = 3000;
    BeatInfo beatInfo;
    beatInfo.ip = ip;
    beatInfo.port = port;
    //加入上报任务
    beat_reactor.addBeatInfo("test1", beatInfo);
    beat_reactor.addBeatInfo("test2", beatInfo);
    beat_reactor.addBeatInfo("test3", beatInfo);

    // 删除上报任务
    beat_reactor.removeBeatInfo("test1", ip, port);

    std::cout << "hello" << std::endl;

    //保持程序开启
    while (!s_quit_flag) {
        sleep(1);
    }

    return 0;
}