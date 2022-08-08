//
// Created by 吴婷 on 2020-06-19.
//
#include <iostream>
#include <memory>

#include "thirdparty/glog/logging.h"
#include "utils/log_utils.h"
#include "utils/json_utils.h"

using namespace common;

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
    //日志
    std::shared_ptr<VnsLogger> logger(new TestVnsLogger());
    LogUtils::Instance()->SetLogger(logger);
    LogUtils::LogInfo("日志测试!");

    // TODO 确定请求传入是否符合

    // response返回值
    std::string response = "{\"timestamp\":\"2020-07-24T09:44:17.124+0000\",\"status\":401,\"error\":\"Unauthorized\",\"message\":\"access token is invalid\",\"path\":\"/naming/v1/ns/instance\"}";
    std::shared_ptr<ResponseError> response_error = std::make_shared<ResponseError>();
    if(JsonUtils::JsonStr2ResponseError(response, response_error)){
        std::cout << "JsonStr2ResponseError成功解析..." << std::endl;
        std::cout << "response_error解析:" << response_error->toString() << std::endl;
    }

    // token信息
    std::string token = "{\"retcode\":200,\"message\":\"操作成功\",\"data\":{\"accessToken\":\"7MonxCGTg8GN3PMmEj4fXhfGut87fqdZvVcGVmkSFho=\",\"refreshToken\":\"DsquvAs9iSzyQSVw4l9QHv-n37DBgNzNM5Q3PcJfeR8=\",\"expiresIn\":1800}}";
    std::shared_ptr<TokenInfo> token_info = std::make_shared<TokenInfo>();
    if(JsonUtils::JsonStr2TokenInfo(token, token_info)){
        std::cout << "JsonStr2TokenInfo成功解析..." << std::endl;
        std::cout << "token_info解析:" << token_info->toString() << std::endl;
        std::cout << "retcode: " << token_info->retcode << std::endl;
        std::cout << "accessToken: " << token_info->data["accessToken"] << std::endl;
        std::cout << "refreshToken: " << token_info->data["refreshToken"] << std::endl;
    }

    // 服务实例
    Instance instance;
    instance.instance_id = "100";
    std::cout << "Instance解析: " << instance.toString() << std::endl;
    std::shared_ptr<Instance> instance2 = std::make_shared<Instance>();
    /*
    if(JsonUtils::JsonStr2Instance(instance.toString(), instance2)){
        std::cout << "JsonStr2Instance成功, 再次解析..." << std::endl;
        std::cout << "Instance2解析: " << instance2->toString() << std::endl;
    }
    */
    std::string str_instance = "{\"ip\":\"10.101.33.57\",\"port\":3000,\"valid\":true,\"healthy\":true,\"marked\":false,\"instanceId\":\"10.101.33.57#3000#DEFAULT#group1@@HelloServer\",\"metadata\":{\"kk\":\"ss\"},\"enabled\":true,\"weight\":1,\"clusterName\":\"DEFAULT\",\"appLoc\":\"prd\",\"pid\":1442230,\"app\":\"vivo-initializer\",\"serviceName\":\"HelloServer\",\"ephemeral\":true}";
    if(JsonUtils::JsonStr2Instance(str_instance, instance2)){
        std::cout << "\n\nJsonStr2Instance解析文本：" << str_instance << std::endl;
        std::cout << "JsonStr2Instance成功, 再次解析..." << std::endl;
        std::cout << "Instance2解析: " << instance2->toString() << std::endl;
    }

    // 服务
    ServiceInfo serviceInfo;
    serviceInfo.name = "服务1";
    serviceInfo.hosts.push_back(instance);
    std::cout << "ServiceInfo解析: " << serviceInfo.toString() << std::endl;
    std::shared_ptr<ServiceInfo> serviceInfo2 = std::make_shared<ServiceInfo>();
    /*
    if(JsonUtils::JsonStr2ServiceInfo(serviceInfo.toString(), serviceInfo2)){
        std::cout << "JsonStr2ServiceInfo成功, 再次解析..." << std::endl;
        std::cout << "ServiceInfo2解析: " << serviceInfo2->toString() << std::endl;
    }
    */

    std::string str_service = "{\"hosts\":[{\"ip\":\"10.101.33.57\",\"port\":3000,\"valid\":true,\"healthy\":true,\"marked\":false,\"instanceId\":\"10.101.33.57#3000#DEFAULT#group1@@HelloServer\",\"metadata\":{},\"enabled\":true,\"weight\":1,\"clusterName\":\"DEFAULT\",\"appLoc\":\"prd\",\"pid\":1442230,\"app\":\"vivo-initializer\",\"serviceName\":\"HelloServer\",\"ephemeral\":true}],\"dom\":\"HelloServer\",\"name\":\"group1@@HelloServer\",\"cacheMillis\":3000,\"lastRefTime\":1595243815043,\"checksum\":\"b021c55945a30f6eedbcccda063380df\",\"useSpecifiedURL\":false,\"namespaceId\":\"prd\",\"clusters\":\"\",\"env\":\"\",\"metadata\":{}}";
    if(JsonUtils::JsonStr2ServiceInfo(str_service, serviceInfo2)){
        std::cout << "\n\nJsonStr2ServiceInfo解析文本：" << str_service << std::endl;
        std::cout << "JsonStr2ServiceInfo成功, 再次解析..." << std::endl;
        std::cout << "ServiceInfo2解析: " << serviceInfo2->toString() << std::endl;
    }

    // 心跳信息
    BeatInfo beatInfo;
    beatInfo.metadata["name"] = "test";
    std::cout << "\nBeatInfo解析: " << beatInfo.toString() << std::endl;

    std::cout << "hello" << 90 << std::endl;

    return 0;
}