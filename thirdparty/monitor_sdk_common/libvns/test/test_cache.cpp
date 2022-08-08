//
// Created by 吴婷 on 2020-06-15.
//
#include <iostream>
#include <memory>
#include <vector>
#include <signal.h>

#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/storage/file/file.h"
#include "thirdparty/monitor_sdk_common/storage/path/path.h"
#include "thirdparty/glog/logging.h"

#include "utils/utils.h"
#include "utils/log_utils.h"
#include "cache/disk_cache.h"
#include "object/service_info.h"

#define CACHE_PATH "./cache"

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

    // 服务信息
    std::shared_ptr<ServiceInfo> service= std::make_shared<ServiceInfo>();
    service->name = "test";
    service->clusters = "c1";
    // 实例
    Instance instance;
    instance.ip = "10.101.97.1";
    instance.port = 34;
    service->hosts.push_back(instance);

    // 写入本地
    DiskCache::write(service, CACHE_PATH);

    // 读取
    std::unordered_map<std::string, std::shared_ptr<ServiceInfo>> serviceInfo_map;
    DiskCache::read(CACHE_PATH, serviceInfo_map);
    std::cout << "服务文件数量：" << serviceInfo_map.size() << std::endl;
    auto iter = std::unordered_map<std::string, std::shared_ptr<ServiceInfo>>::iterator();
    for(iter = serviceInfo_map.begin(); iter != serviceInfo_map.end(); iter++){
        std::cout << "文件名：" << iter->first << std::endl;
        std::cout << "文件内容：" << iter->second->toString() << std::endl;
    }

    std::cout << "文件修改日期:" <<
    VnsFileUtils::GetFileLastModifyTime("/data/wuting/tars/build64_release/common/libvns/cache/test@@c1")
    << "秒"
    << std::endl;

    return 0;
}