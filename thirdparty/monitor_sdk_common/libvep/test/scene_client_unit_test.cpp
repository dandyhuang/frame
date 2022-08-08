//
// Created by 吴婷 on 2019-12-25.
//
#include <iostream>
#include <memory>
#include <tuple>

#include "thirdparty/monitor_sdk_common/libvep/config/dynamic_client_config.h"
#include "thirdparty/monitor_sdk_common/libvep/vep_scene_related_client.h"
#include "thirdparty/monitor_sdk_common/libvep/util/log_utils.h"

#include "thirdparty/glog/logging.h"

using namespace common;

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

int main(int argc, char** argv){
    //日志
    std::shared_ptr<VepLogger> logger(new TestVepLogger());
    LogUtils::Instance()->SetLogger(logger);
    LogUtils::LogInfo("日志测试!");

    //1.测试用例设置开发环境的启动参数
    VepPropertySource::app_env = "dev";   //"test"（测试环境）或者"dev"（开发环境）
    VepPropertySource::app_loc = "sz-zk";
    VepPropertySource::config_host = "vivocfg-agent.test.vivo.xyz/vivocfg"; //拉取配置中心
    DynamicClientConfig::Instance()->initialize();    //配置中心拉取配置

    //传入参数
    std::string scene_code = "zzz";
    std::string layer_code = "zLayer1220Seg_Tag";

    //2.初始化
    VepSceneRelatedClient vep_related_client(scene_code, layer_code);
    LogUtils::LogInfo("VepSceneRelatedClient类初始化成功!");

    //3.启动客户端
    vep_related_client.initialize();

    /**
     * 查询4: abValueWithTag
     */
    //1.不同的tags
    //std::string scene_code = "wt_scene01";
    //std::string layer_code = "wt_tag";
    std::vector<std::string> tags = {"搜索2", "搜索3", "搜索6", "搜索10"};  //32-35
    for (auto tag : tags){
        ResultVo<VepLabHitInfo> result_vo = vep_related_client.abValueWithTag(layer_code, tag);

        std::cout << "---------------------------------------" << std::endl;
        std::cout<< StringPrint("scene_code: %s, layer_code: %s, tag: %s,",
                                scene_code, layer_code, tag) << std::endl;  //Print打印
        //结果类读取
        std::cout << result_vo.ToString() << std::endl;
        std::cout << "data: " << result_vo.data.ToString() << std::endl;
        //返回参数集合
        std::string ss = VepParamsUtils::ToPrint(result_vo.data.params);
        std::cout << "\n参数集合:\n"<< ss << std::endl;
    }

    /**
     * 查询3：abValues与abValuesWithSeg与abValueWithTag
     */
     /*
    std::vector<ResultVo<VepLabHitInfo>> result_vo_vec;
    result_vo_vec.push_back(vep_related_client.abValues(layer_code, "1"));
    result_vo_vec.push_back(vep_related_client.abValuesWithSeg(layer_code, "a"));
    result_vo_vec.push_back(vep_related_client.abValueWithTag(layer_code, "a"));

    for (auto result_vo : result_vo_vec){
        std::cout << "---------------------------------------" << std::endl;
        std::cout<< StringPrint("scene_code: %s, layer_code: %s", scene_code, layer_code) << std::endl;  //Print打印
        std::cout << result_vo.ToString() << std::endl;
        std::cout << "data: " << result_vo.data.ToString() << std::endl;
        std::string ss = VepParamsUtils::ToPrint(result_vo.data.params);
        std::cout << "\n参数集合:\n"<< ss << std::endl;
    }
      */

    /**
     * 查询2: abValueWithSegAndTag
     */
    //1.不同的shunt与tag
    /*
    std::vector<std::tuple<std::string, std::string>> shunt_tag_tuple = {
            {"a", "a"}, {"b", "a"}, {"260", "a"}, {"259", "a"}, {"bbbb", "a"}, {"tttt", "a"}, {"ccc", "a"},//8-14
            {"613", "a"}, {"11", "a"}, {"424", "a-//1"}, {"675", "KKKK"}, {"90", "a"},    //20-24
            {"", ""}, {"", "a"}, {"1", "a"}   //29-31
    };
    std::string shunt;
    std::string tag;
    for (auto entry : shunt_tag_tuple){
        std::tie(shunt, tag) = entry;
        ResultVo<VepLabHitInfo> result_vo = vep_related_client.abValueWithSegAndTag(layer_code, shunt, tag);

        std::cout << "---------------------------------------" << std::endl;
        std::cout<< StringPrint("scene_code: %s, layer_code: %s, shunt: %s, tag: %s",
                                scene_code, layer_code, shunt, tag) << std::endl;  //Print打印
        //结果类读取
        std::cout << result_vo.ToString() << std::endl;
        std::cout << "data: " << result_vo.data.ToString() << std::endl;
        std::string ss = VepParamsUtils::ToPrint(result_vo.data.params);
        std::cout << "\n参数集合:\n"<< ss << std::endl;
    }
     */

    /**
     * 查询1: abValueWithSegAndDiscardTag
     */
    //1.不同的shunts
    /*
    std::vector<std::string> shunts = {
            "a", "b", "260", "259", "bbbb", "tttt", "ccc",  //1-7
            "613", "11", "424", "675", "90",                 //15-19
            "" //28
    };
    for (auto shunt : shunts){
        ResultVo<VepLabHitInfo> result_vo = vep_related_client.abValueWithSegAndDiscardTag(layer_code, shunt);

        std::cout << "---------------------------------------" << std::endl;
        std::cout<< StringPrint("scene_code: %s, layer_code: %s, shunt: %s,",
                                scene_code, layer_code, shunt) << std::endl;  //Print打印
        //结果类读取
        std::cout << result_vo.ToString() << std::endl;
        std::cout << "data: " << result_vo.data.ToString() << std::endl;
        //返回参数集合
        std::string ss = VepParamsUtils::ToPrint(result_vo.data.params);
        std::cout << "\n参数集合:\n"<< ss << std::endl;
    }
    */

}

