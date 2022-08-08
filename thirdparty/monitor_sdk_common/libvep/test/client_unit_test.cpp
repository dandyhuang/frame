//
// Created by 吴婷 on 2019-12-23.
//
#include <iostream>
#include <memory>
#include <tuple>

#include "thirdparty/monitor_sdk_common/libvep/config/dynamic_client_config.h"
#include "thirdparty/monitor_sdk_common/libvep/vep_client.h"
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

/**
 * V1.0测试
 * @param argc
 * @param argv
 * @return
 */
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

    //2.初始化
    VepClient vep_client(scene_code, "");
    LogUtils::LogInfo("VepClient类初始化成功!");

    //3.启动客户端
    vep_client.initialize();

    /**
     * 查询3：abValues与abValuesWithSeg与abValueWithTag
     */
    std::vector<ResultVo<VepLabParamVo>> result_vo_vec;
    result_vo_vec.push_back(vep_client.abValues("zLab1220seg_tagA_S", "ccc"));
    result_vo_vec.push_back(vep_client.abValuesWithSeg("zLab1220seg_tagA_S", "ccc"));
    result_vo_vec.push_back(vep_client.abValueWithTag("zLab1220seg_tagA_S", "a"));

    for (auto result_vo : result_vo_vec){
        std::cout << "---------------------------------------" << std::endl;
        std::cout << result_vo.ToString() << std::endl;
        std::cout << "data: " << result_vo.data.ToString() << std::endl;
        std::string ss = VepParamsUtils::ToPrint(result_vo.data.params);
        std::cout << "\n参数集合:\n"<< ss << std::endl;
    }

    /**
     * 查询2：abValueWithSegAndTag
     */
    /*
    std::vector<std::tuple<std::string, std::string, std::string>> lab_shunt_tag_tuple = {
            //8-14
            {"zLab1220seg_tagA_S", "ccc", "a"}, {"zLab1220seg_tagA_S", "aaa", "a"},
            {"zLab1220seg_tagB_S", "q", "a"},{"zLab1220seg_tagB_S", "w", "a"},
            {"zLab1220seg_tagC_S", "qqq", "a"},
            {"zLab1220seg_tagD_S", "r", "a"},{"zLab1220seg_tagD_S", "s", "a"},
            //21-27
            {"zLab1220seg_tagA_S", "22", "adas"}, {"zLab1220seg_tagA_S", "78", "2-bd!"}, {"zLab1220seg_tagA_S", "22", "2-23a"},
            {"zLab1220seg_tagB_S", "16", "b"}, {"zLab1220seg_tagB_S", "4", "b-43%"},
            {"zLab1220seg_tagC_S", "13", "b-bd!"},{"zLab1220seg_tagD_S", "", "b-bd!"}
    };
    std::string lab_name;
    std::string shunt;
    std::string tag;
    for (auto entry : lab_shunt_tag_tuple){
        std::tie(lab_name, shunt, tag) = entry;
        ResultVo<VepLabParamVo> result_vo = vep_client.abValueWithSegAndTag(lab_name, shunt, tag);

        std::cout << "---------------------------------------" << std::endl;
        std::cout<< StringPrint("scene_code: %s, lab_name: %s, shunt: %s, tag: %s",
                                scene_code, lab_name, shunt, tag) << std::endl;  //Print打印
        //结果类读取
        std::cout << result_vo.ToString() << std::endl;
        std::cout << "data: " << result_vo.data.ToString() << std::endl;
        //返回参数集合
        std::string ss = VepParamsUtils::ToPrint(result_vo.data.params);
        std::cout << "\n参数集合:\n"<< ss << std::endl;
    }
    */


    /**
     * 查询1：abValueWithSegAndDiscardTag
     */
     /*
    std::vector<std::tuple<std::string, std::string>> lab_shunt_tuple = {
            //1-7
            {"zLab1220seg_tagA_S", "ccc"}, {"zLab1220seg_tagA_S", "aaa"},
            {"zLab1220seg_tagB_S", "q"}, {"zLab1220seg_tagB_S", "w"},
            {"zLab1220seg_tagC_S", "qqq"}, {"zLab1220seg_tagD_S", "r"}, {"zLab1220seg_tagD_S", "s"},
            //15-20
            {"zLab1220seg_tagA_S", "78"}, {"zLab1220seg_tagA_S", "22"},
            {"zLab1220seg_tagB_S", "16"}, {"zLab1220seg_tagB_S", "4"},
            {"zLab1220seg_tagC_S", "13"}, {"zLab1220seg_tagD_S", ""}
    };
    std::string lab_name;
    std::string shunt;
    for (auto entry : lab_shunt_tuple){
        std::tie(lab_name, shunt) = entry;
        ResultVo<VepLabParamVo> result_vo = vep_client.abValueWithSegAndDiscardTag(lab_name, shunt);

        std::cout << "---------------------------------------" << std::endl;
        std::cout<< StringPrint("scene_code: %s, lab_name: %s, shunt: %s,",
                                scene_code, lab_name, shunt) << std::endl;  //Print打印
        //结果类读取
        std::cout << result_vo.ToString() << std::endl;
        std::cout << "data: " << result_vo.data.ToString() << std::endl;
        //返回参数集合
        std::string ss = VepParamsUtils::ToPrint(result_vo.data.params);
        std::cout << "\n参数集合:\n"<< ss << std::endl;
    }
    */
}
