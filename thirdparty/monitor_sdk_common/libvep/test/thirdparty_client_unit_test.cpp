//
// Created by 吴婷 on 2019-12-02.
//
#include <iostream>
#include <memory>
#include <tuple>
#include <map>

#include "thirdparty/monitor_sdk_common/libvep/config/dynamic_client_config.h"
#include "thirdparty/monitor_sdk_common/libvep/vep_thirdparty_related_client.h"
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
 * 第三方流量位测试
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
    /**
     * dev开发环境例子
     */
    std::string position_type = TrafficPositionTypeEnum::AD;
    std::string layer_code = "zLayer1220Seg_Tag";

    //2.初始化
    VepThirdPartyRelatedClient vep_related_client(position_type, layer_code);
    LogUtils::LogInfo("VepThirdPartyRelatedClient类初始化成功!");

    //3.启动客户端
    vep_related_client.initialize();

    /**
     * 查询4: abValueWithTag
     */
    //1.不同的tags
    std::string traffic_position_id = "1";
    std::vector<std::string> tags = {"搜索2", "搜索3", "ai", "kk"}; //59-62: layer_code = "wt_tag";
    for (auto tag : tags){
        ResultVo<VepLabHitInfo> result_vo = vep_related_client.abValueWithTag(layer_code, traffic_position_id, tag);

        std::cout << "---------------------------------------" << std::endl;
        std::cout<< StringPrint("position_type: %s, layer_code: %s, traffic_position_id: %s, tag: %s,",
                position_type, layer_code, traffic_position_id, tag) << std::endl;  //Print打印
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
    std::string traffic_position_id = "1";
    std::string shunt = "a";
    std::vector<ResultVo<VepLabHitInfo>> result_vo_vec;
    result_vo_vec.push_back(vep_related_client.abValues(layer_code, traffic_position_id, shunt));
    result_vo_vec.push_back(vep_related_client.abValuesWithSeg(layer_code, traffic_position_id, shunt));
    result_vo_vec.push_back(vep_related_client.abValueWithTag(layer_code, traffic_position_id, ""));

    for (auto result_vo : result_vo_vec){
        std::cout << "---------------------------------------" << std::endl;
        std::cout<< StringPrint("position_type: %s, layer_code: %s, traffic_position_id: %s, shunt: %s,",
                                position_type, layer_code, traffic_position_id, shunt) << std::endl;
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
    std::string traffic_position_id = "1";
    //std::string traffic_position_id = "2"; //43-50
    std::vector<std::tuple<std::string, std::string>> shunt_tag_tuple = {
            //10-15
            {"3b!", "a"}, {"^#", "a"}, {")(", "a"}, {"~", "a"}, {"8574", "a"}, {"sdas", "a"},
            //25-33
            {"613", "3-23a"}, {"11", "kkkk"}, {"0", ""}, {"0", "KKKK"},
            {"656", "a"}, {"598", "a"}, {"868", "a"}, {"56", "a"}, {"75", "a"}
    };
    std::string shunt;
    std::string tag;
    for (auto entry : shunt_tag_tuple){
        std::tie(shunt, tag) = entry;
        ResultVo<VepLabHitInfo> result_vo = vep_related_client.abValueWithSegAndTag(layer_code, traffic_position_id, shunt, tag);

        std::cout << "---------------------------------------" << std::endl;
        std::cout<< StringPrint("position_type: %s, layer_code: %s, traffic_position_id: %s, shunt: %s, tag: %s",
                                position_type, layer_code, traffic_position_id, shunt, tag) << std::endl;  //Print打印
        //结果类读取
        std::cout << result_vo.ToString() << std::endl;
        std::cout << "data: " << result_vo.data.ToString() << std::endl;
        //返回参数集合
        std::string ss = VepParamsUtils::ToPrint(result_vo.data.params);
        std::cout << "\n参数集合:\n"<< ss << std::endl;
    }
     */

    //2.空
    /*
    std::vector<std::tuple<std::string, std::string, std::string>> position_shunt_tag_tuple = {
            //56-58
            {"1", "", ""}, {"$%#^^*", "", "a"}, {"$%#^^*", "1", "a"}
    };
    std::string traffic_position_id;
    std::string shunt;
    std::string tag;
    for (auto entry : position_shunt_tag_tuple){
        std::tie(traffic_position_id, shunt, tag) = entry;
        ResultVo<VepLabHitInfo> result_vo = vep_related_client.abValueWithSegAndTag(layer_code, traffic_position_id, shunt, tag);

        std::cout << "---------------------------------------" << std::endl;
        std::cout<< StringPrint("position_type: %s, layer_code: %s, traffic_position_id: %s, shunt: %s, tag: %s",
                                position_type, layer_code, traffic_position_id, shunt, tag) << std::endl;  //Print打印
        //结果类读取
        std::cout << result_vo.ToString() << std::endl;
        std::cout << "data: " << result_vo.data.ToString() << std::endl;
        //返回参数集合
        std::string ss = VepParamsUtils::ToPrint(result_vo.data.params);
        std::cout << "\n参数集合:\n"<< ss << std::endl;
    }
     */

    /**
     * 查询1: abValueWithSegAndDiscardTag
     */
    //1.不同的shunts
    /*
    std::string traffic_position_id = "1";
    std::vector<std::string> shunts = {"3b!", "^#", "￥#", "1b!", "@", ")(", "~", "9527", "sdas",  //1-9
                                       "613", "11", "0", "35", "656", "598", "868", "56", "75", }; //16-24
    for (auto shunt : shunts){
        ResultVo<VepLabHitInfo> result_vo = vep_related_client.abValueWithSegAndDiscardTag(layer_code, traffic_position_id, shunt);

        std::cout << "---------------------------------------" << std::endl;
        std::cout<< StringPrint("position_type: %s, layer_code: %s, traffic_position_id: %s, shunt: %s,",
                position_type, layer_code, traffic_position_id, shunt) << std::endl;  //Print打印
        //结果类读取
        std::cout << result_vo.ToString() << std::endl;
        std::cout << "data: " << result_vo.data.ToString() << std::endl;
        //返回参数集合
        std::string ss = VepParamsUtils::ToPrint(result_vo.data.params);
        std::cout << "\n参数集合:\n"<< ss << std::endl;
    }
    */

    //2.不同的traffic_position_id
    /*
    std::vector<std::tuple<std::string, std::string>> position_id_shunt_tuple = {
            {"4", "613"}, {"5", "11"}, {"5", "0"}, {"2", "35"},
            {"2", "656"}, {"2", "598"}, {"2", "868"}, {"8", "56"}, {"4", "75"}
    };//34-42
    std::string traffic_position_id;
    std::string shunt;
    for (auto entry : position_id_shunt_tuple){
        std::tie(traffic_position_id, shunt) = entry;
        ResultVo<VepLabHitInfo> result_vo = vep_related_client.abValueWithSegAndDiscardTag(layer_code, traffic_position_id, shunt);

        std::cout << "---------------------------------------" << std::endl;
        std::cout<< StringPrint("position_type: %s, layer_code: %s, traffic_position_id: %s, shunt: %s,",
                                position_type, layer_code, traffic_position_id, shunt) << std::endl;  //Print打印
        //结果类读取
        std::cout << result_vo.ToString() << std::endl;
        std::cout << "data: " << result_vo.data.ToString() << std::endl;
        //返回参数集合
        std::string ss = VepParamsUtils::ToPrint(result_vo.data.params);
        std::cout << "\n参数集合:\n"<< ss << std::endl;
    }
     */
    //3.空shunt
    /*
    std::string traffic_position_id = "1";
    std::string shunt = "";
    ResultVo<VepLabHitInfo> result_vo = vep_related_client.abValueWithSegAndDiscardTag(layer_code, traffic_position_id, shunt);
    std::cout << "---------------------------------------" << std::endl;
    std::cout<< StringPrint("position_type: %s, layer_code: %s, traffic_position_id: %s, shunt: %s,",
                            position_type, layer_code, traffic_position_id, shunt) << std::endl;  //Print打印
    std::cout << result_vo.ToString() << std::endl;
    std::cout << "data: " << result_vo.data.ToString() << std::endl;
    std::string ss = VepParamsUtils::ToPrint(result_vo.data.params);
    std::cout << "\n参数集合:\n"<< ss << std::endl;
    */
}
