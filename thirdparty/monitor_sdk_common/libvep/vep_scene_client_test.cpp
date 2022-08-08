//
// Created by 吴婷 on 2019-12-09.
//
#include <iostream>
#include <memory>
#include <signal.h>

#include "thirdparty/monitor_sdk_common/system/threading/this_thread.h"
#include "thirdparty/monitor_sdk_common/system/atomic/atomic.h"
#include "thirdparty/monitor_sdk_common/libvep/vep_scene_related_client.h"
#include "thirdparty/monitor_sdk_common/libvep/util/log_utils.h"

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
 * glog打印日志
 */
class TestVepLogger : public VepLogger{
public:
    TestVepLogger(){}
    ~TestVepLogger(){}

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

std::shared_ptr<VepSceneRelatedClient> vep_related_client;   //定义客户端

int main(int argc, char** argv){
    // 设置扑捉到信号后的动作
    signal(SIGINT, &sig_graceful_quit);
    signal(SIGTERM, &sig_graceful_quit);
    signal(SIGQUIT, &sig_graceful_quit);
    signal(SIGPIPE, SIG_IGN);

    //日志
    std::shared_ptr<VepLogger> logger(new TestVepLogger());
    LogUtils::Instance()->SetLogger(logger);
    LogUtils::LogInfo("日志测试!");

    //1.测试用例设置开发环境的启动参数
    VepPropertySource::app_env = "dev";         //"test"（测试环境）或者"dev"（开发环境）
    VepPropertySource::app_loc = "sz-zk";
    VepPropertySource::config_host = "vivocfg-agent.test.vivo.xyz/vivocfg"; //拉取配置中心

    //传入参数
    std::string scene_code = "zzz";
    std::string layer_code = "zLayer1220Seg_Tag";
    std::string shunt = "90";
    std::string tag = "a";

    //2.初始化
    vep_related_client = std::make_shared<VepSceneRelatedClient>(scene_code, layer_code);
    LogUtils::LogInfo("VepSceneRelatedClient类初始化成功!");

    //3.启动客户端
    vep_related_client->initialize();

    //查询
    ResultVo<VepLabHitInfo> result_vo = vep_related_client->abValueWithSegAndTag(layer_code, shunt, tag);
    //ResultVo<VepLabHitInfo> result_vo = vep_related_client->abValuesWithSeg(layer_code, shunt);
    //ResultVo<VepLabHitInfo> result_vo = vep_related_client->abValueWithTag(layer_code, tag);
    //ResultVo<VepLabHitInfo> result_vo = vep_related_client->abValueWithSegAndDiscardTag(layer_code, shunt);
    //ResultVo<VepLabHitInfo> result_vo = vep_related_client->abValues(layer_code, shunt);		    //【即将弃用，新接口为abValuesWithSeg】
    //ResultVo<VepLabHitInfo> result_vo = vep_related_client->abValues(layer_code, shunt, tag);		// 通用接口api

    //结果类读取
    std::cout << result_vo.ToString() << std::endl;
    std::cout << "data: " << result_vo.data.ToString() << std::endl;
    //返回参数集合
    std::string ss = VepParamsUtils::ToPrint(result_vo.data.params);
    std::cout << "\n参数集合:\n"<< ss << std::endl;

    while (!s_quit_flag) {
        sleep(1);
    }

    vep_related_client->close();
}
