//
// Created by 吴婷 on 2020-06-17.
//
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/storage/file/file.h"
#include "thirdparty/monitor_sdk_common/storage/path/path.h"
#include "cache/disk_cache.h"
#include "utils/json_utils.h"
#include "utils/log_utils.h"

using namespace common;

std::string DiskCache::_ip_cache_path = "/tmp/vns";

bool DiskCache::write(std::shared_ptr<ServiceInfo> dom, const std::string &dir) {
    // 同步备份一份ip和port文件
    DiskCache::writeIpPort(dom);

    // 判断目录是否存在
    if(access(dir.c_str(), 0) == -1){
        LogUtils::LogInfo(StringPrint("[%s] directory is not existing, now make it ...", dir));
        int flag = mkdir(dir.c_str(), 0777);   // linux
        if(flag == 0){
            LogUtils::LogInfo("DiskCache: Directory make successfully!");
        } else {
            LogUtils::LogError("DiskCache: Directory make errorly!");
            return false;
        }
    }

    // 文件名
    std::string filename = dom->getKey();
    // 文件名全路径
    std::string file_path = Path::Join(dir, filename);
    LogUtils::LogInfo("文件绝对路径：" + Path::ToAbsolute(file_path));
    // 打开文件
    scoped_ptr<File> fp(File::Open(file_path, "w"));
    if(!fp){
        LogUtils::LogError(StringPrint("DiskCache::write [%s] file open fail! current user or directory may not have write permission", file_path));
        return false;
    }
    // 文件写入
    std::string service_json = dom->toString();

    if(!fp->Write(service_json.data(), service_json.size())){
        return false;
    }

    LogUtils::LogInfo("DiskCache写入服务信息：" + service_json);
    // 固化到磁盘
    return fp->Flush();
}

bool DiskCache::read(const std::string &cache_dir, std::unordered_map<std::string, std::shared_ptr<ServiceInfo>> &service_map){
    // 判断目录是否存在
    if(access(cache_dir.c_str(), 0) == -1){
        LogUtils::LogError(StringPrint("[%s] directory is not existing, read fail!", cache_dir));
        return false;
    }
    // 判断目录是否有读权限
    if(access(cache_dir.c_str(), 4) == -1){
        LogUtils::LogError(StringPrint("DiskCache::read [%s] directory does not have read permission!", cache_dir));
        return false;
    }
    // 依次取出各个服务文件
    scoped_ptr<FileIterator> iter(File::Iterate(cache_dir));
    FileEntry entry;
    while (iter->GetNext(&entry)) {
        // 文件名
        std::string filename = entry.name;
        // 服务信息对象
        std::shared_ptr<ServiceInfo> service = std::make_shared<ServiceInfo>(filename);
        // 取出文件内容
        std::string data;
        std::string file_path = Path::Join(cache_dir, filename);
        if(!File::ReadAll(file_path, &data)){  // 取文件失败，目录会读取失败
            LogUtils::LogInfo(StringPrint("[Fail] read file: [%s]. tips: It maybe directory.", filename));
            // 跳过json解析
            continue;
        }

        LogUtils::LogInfo(StringPrint("cache dir: [%s], filename: [%s]", file_path, filename));

        // json解析
        JsonUtils::JsonStr2ServiceInfo(data, service);
        // 至少有一个实例才添加到缓存
        if(service->hosts.size() > 0){
            // map增加
            service_map[filename] = service;
            LogUtils::LogInfo(StringPrint("DiskCache service_map大小: [%d], filename: [%s]", service_map.size(), filename));
        }
    }

    return true;
}

bool DiskCache::writeIpPort(std::shared_ptr<ServiceInfo> dom) {
    // 判断目录是否存在
    if(access(_ip_cache_path.c_str(), 0) == -1){
        LogUtils::LogInfo(StringPrint("[%s] directory is not existing, now make it ...", _ip_cache_path));
        int flag = mkdir(_ip_cache_path.c_str(), 0777);   // linux
        if(flag == 0){
            LogUtils::LogInfo("DiskCache: Directory make successfully!");
        } else {
            LogUtils::LogError("DiskCache: Directory make errorly!");
            return false;
        }
    }

    // 文件名: app应用名@服务名
    std::string filename = dom->getServiceName();
    // 文件名全路径
    std::string file_path = Path::Join(_ip_cache_path, filename);
    LogUtils::LogInfo("文件绝对路径：" + Path::ToAbsolute(file_path));

    // 待写入内容
    std::string result;
    int size = dom->hosts.size();   // ip和port数
    for (int i = 0; i < size; ++i) {
        std::string line = dom->hosts[i].ip + ":" + IntegerToString(dom->hosts[i].port) + "\n";
        result += line;
    }

    // 打开文件
    scoped_ptr<File> fp(File::Open(file_path, "w"));
    if(!fp){
        LogUtils::LogError(StringPrint("DiskCache::write [%s] file open fail! current user or directory may not have write permission", file_path));
        return false;
    }

    // 文件写入
    if(!fp->Write(result.data(), result.size())){
        return false;
    }

    LogUtils::LogInfo("DiskCache写入服务信息：" + result);
    // 固化到磁盘
    return fp->Flush();
}
