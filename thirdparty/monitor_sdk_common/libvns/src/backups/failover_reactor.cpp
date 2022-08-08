//
// Created by 吴婷 on 2020-06-17.
//
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/storage/file/file.h"
#include "thirdparty/monitor_sdk_common/system/threading/this_thread.h"

#include "core/host_reactor.h"
#include "backups/failover_reactor.h"
#include "cache/disk_cache.h"
#include "utils/log_utils.h"
#include "utils/utils.h"
#include "utils/utilandcoms.h"

using namespace common;

int FailoverReactor::DAY_PERIOD_MINUTES = 24 * 60;   // 单位：分钟

FailoverReactor::~FailoverReactor(){
    if(_scheduled_executor != nullptr){
        LogUtils::LogInfo("FailoverReactor终止调度类");
        if (_scheduled_executor->IsTerminated()) {
            _scheduled_executor->Shutdown();
        }
    }
}

void FailoverReactor::init(){
    // 单线程
    std::shared_ptr<ScheduledExecutor> _scheduled_executor = std::make_shared<ScheduledExecutor>(1);

    // 新建写入任务
    std::shared_ptr<DiskFileWriter> writer_task = std::make_shared<DiskFileWriter>(this);

    // 3.如果故障转移目录为空，在启动时进行数据落盘
    // 若容灾目录为空，备份文件立即运行
    // 判断容灾目录是否存在
    if(VnsFileUtils::AccessDirExist(_failover_dir)){    // 成功创建目录
        LogUtils::LogInfo(StringPrint("FailoverReactor::init Directory exist or make successfully! dir: [%s]", _failover_dir));
    } else {
        LogUtils::LogError(StringPrint("FailoverReactor::init Directory make errorly! dir: [%s]", _failover_dir));
        return;
    }

    // 获取缓存目录的文件数
    scoped_ptr<FileIterator> iter(File::Iterate(_failover_dir));
    FileEntry entry;
    int file_num = 0;   //目录下文件数
    while (iter->GetNext(&entry)) {
        file_num++;
    }
    // 若缓存目录下什么都没有，直接先读取缓存
    if(0 == file_num) {
        writer_task->run();
    }


    // 1.定时检查落盘文件刷新情况（5秒）
    // 新建刷新任务
    std::shared_ptr<SwitchRefresher> refresh_task = std::make_shared<SwitchRefresher>(this);
    refresh_task->run();    // 立马执行一次
    _scheduled_executor->ScheduleWithFixedDelay([this, refresh_task](bool *terminated) {
        if((*terminated)){
            return;
        }
        LogUtils::LogInfo("检查落盘文件刷新情况, 调度中...");
        refresh_task->run();
    }, 0, 5000);

    // 2.定时持久化服务信息（1天）
    _scheduled_executor->ScheduleWithFixedDelay([this, writer_task](bool *terminated) {
        if((*terminated)){
            return;
        }
        LogUtils::LogInfo("持久化服务, 调度中...");
        writer_task->run();
    }, 30 * 60 * 1000, DAY_PERIOD_MINUTES * 60 * 1000); // 30分钟后开始， 一天调度一次

}

std::string FailoverReactor::getFailoverDir(){
    return _failover_dir;
}

SwitchRefresher::SwitchRefresher(FailoverReactor* failover_reactor): _failover_reactor(failover_reactor){
    _failover_dir = _failover_reactor->getFailoverDir();
}

void SwitchRefresher::run(){
    // 1.控制开关的文件名
    std::string switch_filename = _failover_dir + UtilAndComs::FAILOVER_SWITCH;
    LogUtils::LogInfo(StringPrint("开关文件名: %s", switch_filename));
    // 判断文件是否存在
    if(access(switch_filename.c_str(), 0) == -1){
        LogUtils::LogError(StringPrint("failover switch is not found: [%s]", _failover_dir));
        _failover_reactor->switch_params["failover-mode"] = false;
        return;
    }
    // 2.获取文件最近修改时间
    _mutex.Lock();
    long modified = VnsFileUtils::GetFileLastModifyTime(switch_filename);
    _mutex.Unlock();
    // 3.文件已修改，进行刷新
    if (_last_modified_millis < modified) {
        // 按行读取文件内容
        scoped_ptr<File> fp(File::Open(switch_filename, "r"));
        std::vector<std::string> lines;
        int line_num;
        // 读取成功，且文件内容存在
        if(File::ReadLines(switch_filename, &lines)){
            line_num = lines.size();
            if(line_num > 0){       // 文件内容存在
                // 依次覆盖
                for (int i = 0; i<line_num; i++) {
                    int n;
                    StringToNumber(lines[i], &n);
                    if(1 == n){
                        LogUtils::LogInfo("failover-mode is on");
                        _failover_reactor->switch_params["failover-mode"] = true;
                        // 新建failover文件读取任务
                        std::shared_ptr<FailoverFileReader> failover_task = std::make_shared<FailoverFileReader>(_failover_reactor);
                        failover_task->run();   // 运行读取任务
                    } else if (0 == n) {
                        LogUtils::LogInfo("failover-mode is off");
                        _failover_reactor->switch_params["failover-mode"] = false;
                    }
                }
                return;
            }
        }
        // 读取失败或者文件内容为空
        LogUtils::LogInfo(StringPrint("failed to ReadLines failover dir: ", _failover_dir));
        _failover_reactor->switch_params["failover-mode"] = false;

    }
}

FailoverFileReader::FailoverFileReader(FailoverReactor* failover_reactor): _failover_reactor(failover_reactor) {
    _failover_dir = _failover_reactor->getFailoverDir();
}

void FailoverFileReader::run(){
    std::unordered_map<std::string, std::shared_ptr<ServiceInfo>> tmp_map;
    // 判断容灾目录是否存在
    if(access(_failover_dir.c_str(), 0) == -1){
        LogUtils::LogInfo(StringPrint("[%s] directory is not existing, now make it ...", _failover_dir));
        int flag = mkdir(_failover_dir.c_str(), 0777);   // linux
        if(flag == 0){
            LogUtils::LogInfo("FailoverFileReader::run Directory make successfully!");
        } else {
            LogUtils::LogError("FailoverFileReader::run Directory make errorly!");
            return;
        }
    }
    // 依次读取目录下的文件
    DiskCache::read(_failover_dir, tmp_map);
    // 缓存更新
    if(tmp_map.size() > 0){
        _failover_reactor->service_map = tmp_map;   // map拷贝
        //LogUtils::LogInfo(StringPrint("FailoverFileReader 缓存service_map的内容为: [%s]", _failover_reactor->_service_map["test@@c1"]->toString()));
        LogUtils::LogInfo(StringPrint("FailoverFileReader 缓存service_map的大小为: [%d]", _failover_reactor->service_map.size()));
    } else {
        LogUtils::LogInfo(StringPrint("容灾路径[%s]下没有缓存内容", _failover_dir));
    }
}

DiskFileWriter::DiskFileWriter(FailoverReactor* failover_reactor): _failover_reactor(failover_reactor) {
    _failover_dir = _failover_reactor->getFailoverDir();
}

void DiskFileWriter::run(){
    // 从缓存中依次取出服务信息
    std::unordered_map<std::string, std::shared_ptr<ServiceInfo>> service_map = _failover_reactor->host_reactor->serviceInfo_map;
    LogUtils::LogInfo(StringPrint("DiskFileWriter 缓存service_map的大小为: [%d]", service_map.size()));
    std::unordered_map<std::string, std::shared_ptr<ServiceInfo>>::iterator iter;
    for(iter = service_map.begin(); iter != service_map.end(); iter++){
        std::shared_ptr<ServiceInfo> service = iter->second;
        // 跳过
        if(UtilAndComs::ALL_IPS == service->getKey() || UtilAndComs::ENV_LIST_KEY == service->name
            || "00-00---000-ENV_CONFIGS-000---00-00" == service->name
            || "00-00---000-ALL_HOSTS-000---00-00" == service->name) {
            continue;
        }
        // 写入缓存
        LogUtils::LogInfo(StringPrint("DiskFileWriter写入缓存内容: [%s]", service->toString()));
        DiskCache::write(service, _failover_dir);
    }
}

bool FailoverReactor::isFailoverSwitch(){
    auto iter = switch_params.find("failover-mode");
    // 存在
    if (iter != switch_params.end()) {
        return iter->second;
    }
    return false;
}

std::shared_ptr<ServiceInfo> FailoverReactor::getService(const std::string &key){
    auto iter = service_map.find(key);
    // 缓存中存在
    if (iter != service_map.end()) {
        return iter->second;
    }
    return std::make_shared<ServiceInfo>();
}