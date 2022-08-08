//
// Created by 吴婷 on 2019-12-12.
//
#include <iostream>
#include <memory>
#include <vector>
#include <signal.h>

#include "thirdparty/monitor_sdk_common/system/threading/this_thread.h"
#include "thirdparty/monitor_sdk_common/system/atomic/atomic.h"
#include "thirdparty/monitor_sdk_common/libvep/vep_client.h"
#include "thirdparty/monitor_sdk_common/libvep/util/log_utils.h"
#include "thirdparty/monitor_sdk_common/libvep/util/utils.h"

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

std::shared_ptr<VepClient> vep_client;          //定义客户端

int main(int argc, char** argv){
    signal(SIGINT, &sig_graceful_quit);
    signal(SIGTERM, &sig_graceful_quit);
    signal(SIGQUIT, &sig_graceful_quit);
    signal(SIGPIPE, SIG_IGN);

    //日志
    std::shared_ptr<VepLogger> logger(new TestVepLogger());
    LogUtils::Instance()->SetLogger(logger);
    LogUtils::LogInfo("日志测试!");

    //1.测试用例设置开发环境的启动参数
    VepPropertySource::app_env = "dev";         //"test"（测试环境）或者"dev"（开发环境）  【注：正确选择】
    VepPropertySource::app_loc = "sz-zk";
    VepPropertySource::config_host = "vivocfg-agent.test.vivo.xyz/vivocfg"; //拉取配置中心

    //传入参数
    std::string scene_code = "zzz";
    std::string lab_name = "zLab1220seg_tagA_S";
    std::string shunt = "78";
    std::string tag = "2-bd!";

    //2.初始化
    vep_client = std::make_shared<VepClient>(scene_code, "");
    LogUtils::LogInfo("VepClient类初始化成功!");

    //3.启动客户端
    vep_client->initialize();
    LogUtils::LogInfo("VepClient initialize成功!");

    //(1)获取场景
    //std::shared_ptr<VepApiSceneVo> scene_vo = vep_client->getSceneVo();

    //(2)返回实验信息
    ResultVo<VepLabParamVo> result_vo = vep_client->abValueWithSegAndTag(lab_name, shunt, tag);
    //ResultVo<VepLabParamVo> result_vo = vep_client->abValueWithSegAndDiscardTag(lab_name, shunt);
    //ResultVo<VepLabParamVo> result_vo = vep_client->abValuesWithSeg(lab_name, shunt);
    //ResultVo<VepLabParamVo> result_vo = vep_client->abValueWithTag(lab_name, tag);
    //ResultVo<VepLabParamVo> result_vo = vep_client->abValues(lab_name, shunt);  				//【即将弃用，新接口为abValuesWithSeg】
    //ResultVo<VepLabParamVo> result_vo = vep_client->abValues(lab_name, shunt, tag);			// 通用接口api
    //结果类读取
    std::cout << result_vo.ToString() << std::endl;
    std::cout << "data: " << result_vo.data.ToString() << std::endl;
    //返回参数集合
    std::string ss = VepParamsUtils::ToPrint(result_vo.data.params);
    std::cout << "\n参数集合:\n"<< ss << std::endl;

    //(3)返回该号段下的所有实验
    //std::vector<ResultVo<VepLabParamVo>> result_vo_list = vep_client->abValues(shunt);

    //保持程序开启
    while (!s_quit_flag) {
        sleep(1);
    }
    //程序停止关闭
    std::cout << "close vep_related_client" << std::endl;
    vep_client->close();
}
