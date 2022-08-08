//
// Created by 吴婷 on 2020-06-16.
//
#include <iostream>
#include <unordered_map>
#include <vector>

#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/system/threading/this_thread.h"
#include "core/host_reactor.h"
#include "cache/disk_cache.h"
#include "utils/log_utils.h"
#include "utils/json_utils.h"

using namespace common;

int HostReactor::DEFAULT_DELAY = 5000;
int HostReactor::UPDATE_HOLD_INTERVAL = 5000;

void HostReactor::initialize(){
    // 清除map缓存
    if(serviceInfo_map.size() > 0){
        LogUtils::LogInfo("HostReactor 清除map缓存");
        serviceInfo_map.clear();
    }

    LogUtils::LogInfo(StringPrint("HostReactor 缓存目录: [%s]", _cache_dir));
    if(_load_cache_at_start){
        LogUtils::LogInfo("HostReactor::initialize 一开始就加入缓存服务");
        DiskCache::read(_cache_dir, serviceInfo_map);
    }

    _scheduled_executor = std::make_shared<ScheduledExecutor>(_polling_thread_count);  // 定时任务
    _failover_reactor = std::make_shared<FailoverReactor>(this, _cache_dir);
    _push_receiver = std::make_shared<PushReceiver>(this);

    //启动push任务队列
    TaskQueue::Instance()->start();
    TaskQueue::Instance()->push(_push_receiver);

    // 各个类初始化
    _failover_reactor->init();


}

std::shared_ptr<ServiceInfo> HostReactor::getServiceInfo(const std::string &service_name, const std::string &clusters){
    // 服务信息的key
    std::string key = ServiceInfo::getKey(service_name, clusters);
    // failove状态，缓存查找判断
    bool is_failover_switch = _failover_reactor->isFailoverSwitch();
    LogUtils::LogInfo(StringPrint("failover-mode: %s", is_failover_switch ? "true" : "false"));
    if(is_failover_switch){
        return _failover_reactor->getService(key);  // 容灾缓存中获取服务信息
    }

    // 2.从缓存中取出
    bool is_exist = false;      // 缓存中是否存在
    // 从缓存中取出服务信息
    auto iter = HostReactor::serviceInfo_map.find(key);
    if(iter == HostReactor::serviceInfo_map.end()){ // 未找到
        LogUtils::LogInfo(StringPrint("HostReactor::getServiceInfo 新服务缓存中添加: [%s]", key));
        // 新建服务信息
        std::shared_ptr<ServiceInfo> tmp_service = std::make_shared<ServiceInfo>(service_name, clusters);
        // 增加到缓存中
        serviceInfo_map[key] = tmp_service;
        // 更新中
        updating_map[service_name] = tmp_service;
        updateServiceNow(service_name, clusters);
        // 从接口返回，到vns平台注册耗时在1分钟左右：立马注册，立马查询，vns平台返回速度较慢。原因：接口本身返回值为空
        //ThisThread::Sleep(1000);    // TODO 待优化：暂停1秒进行数据更新，否则可能函数结束时，map还没有被及时更新
        updating_map.erase(service_name);
        // 将更新后的缓存值返回
        //return serviceInfo_map[key];                //【返回值】
    } else {
        LogUtils::LogInfo(StringPrint("HostReactor::getServiceInfo 缓存中查找到服务: [%s]", key));
        // 缓存中存在
        is_exist = true;
        // 更新中，暂停一段时间，保持更新
        auto iter_update = updating_map.find(service_name);
        if (iter_update != updating_map.end()) {
            if (UPDATE_HOLD_INTERVAL > 0){
                LogUtils::LogInfo(StringPrint("HostReactor::getServiceInfo 暂停[%d]秒，更新服务:[%s]中...", UPDATE_HOLD_INTERVAL, key));
                ThisThread::Sleep(UPDATE_HOLD_INTERVAL);
            }
        }
        //return iter->second;         //【返回值】
    }

    // 3.增加更新任务，定时更新服务信息（缓存中不存在的时候，才需要添加一次。已经存在的，不需要重复添加）
    if(!is_exist){
        // 新建更新任务
        std::shared_ptr<UpdateTask> update_task = std::make_shared<UpdateTask>(service_name, clusters, this);
        _scheduled_executor->ScheduleWithFixedDelay([this, update_task, service_name, clusters](bool *terminated) {
            LogUtils::LogInfo(StringPrint("HostReactor更新任务[%s:%s]调度中, 时间间隔为:[%d]秒...", service_name, clusters, DEFAULT_DELAY/1000));
            update_task->run();
        }, DEFAULT_DELAY, DEFAULT_DELAY);
    }

    return serviceInfo_map[key];
}

std::shared_ptr<ServiceInfo> HostReactor::getServiceInfoDirectlyFromServer(const std::string &service_name, const std::string &clusters){
    std::shared_ptr<ServiceInfo> service = std::make_shared<ServiceInfo>(service_name, clusters);
    std::string result = _server_proxy->queryList(service_name, clusters, 0, false);
    if(!result.empty()){
        JsonUtils::JsonStr2ServiceInfo(result, service);
    }
    return service;
}

std::shared_ptr<ServiceInfo> HostReactor::processServiceJSON(const std::string &json) {
    // api返回服务类
    std::shared_ptr<ServiceInfo> service = std::make_shared<ServiceInfo>();
    if(!JsonUtils::JsonStr2ServiceInfo(json, service)){
        LogUtils::LogError(StringPrint("HostReactor::processServiceJSON中json解析失败, /instance/list返回值: [%s]", json));
        return service;
    }
    LogUtils::LogInfo(StringPrint("HostReactor::processServiceJSON中json解析成功, 服务信息: [%s]", service->toString()));

    if(service->hosts.empty() || !service->validate()){ // api返回内容为空，直接返回
        LogUtils::LogError("HostReactor::processServiceJSON api返回结果为空/服务设置为不可用");
        return service;
    }
    std::string key = service->getKey();
    bool changed = false;

    // 判断此服务是否在缓存中
    auto iter = serviceInfo_map.find(key);
    if(iter != serviceInfo_map.end()){
        LogUtils::LogInfo(StringPrint("HostReactor::processServiceJSON 缓存serviceInfo_map 服务存在: [%s]", key));
        // 1.缓存中已有此服务
        std::shared_ptr<ServiceInfo> old_service = iter->second;    // 缓存
        if(old_service->last_ref_time > service->last_ref_time){
            LogUtils::LogInfo(StringPrint("HostReactor out of date data received, old-t:%l, new-t:%l", old_service->last_ref_time, service->last_ref_time));
        }
        // 缓存中更新服务
        LogUtils::LogInfo(StringPrint("HostReactor serviceInfo_map size: [%d], 更新缓存服务: [%s]", serviceInfo_map.size(), key));
        serviceInfo_map[key] = service;
        // 获取old服务的实例列表
        std::unordered_map<std::string, Instance> old_hostmap;   //{ip:port, instance}
        for (auto instance : old_service->hosts) {
            old_hostmap[instance.toInetAddr()] = instance;
        }
        // 获取new服务的实例列表
        std::unordered_map<std::string, Instance> new_hostmap;   //{ip:port, instance}
        for (auto instance : service->hosts) {
            new_hostmap[instance.toInetAddr()] = instance;
        }
        // 实例分组：修改、新增、删除
        std::vector<Instance> mod_hosts;
        std::vector<Instance> new_hosts;
        std::vector<Instance> remv_hosts;

        // new服务中每个实例与old进行比较
        std::unordered_map<std::string, Instance>::iterator iter;
        for(iter = new_hostmap.begin(); iter != new_hostmap.end(); iter++){
            std::string instance_key = iter->first;
            Instance instance = iter->second;
            // 比较
            auto iter_old = old_hostmap.find(instance_key);
            if(iter_old != old_hostmap.end()){
                // old中存在但实例不等，加入mod
                if(instance.toString() != iter_old->second.toString()){
                    mod_hosts.push_back(instance);
                    continue;
                }
            } else {
                // old中不含，加入new
                new_hosts.push_back(instance);
            }
        }

        // old中的实例在new中查找，查不到，就加入remove
        for(iter = old_hostmap.begin(); iter != old_hostmap.end(); iter++){
            std::string instance_key = iter->first;
            Instance instance = iter->second;
            // 比较
            auto iter_new = new_hostmap.find(instance_key);
            if(iter_new != new_hostmap.end()){
                continue;
            } else {
                remv_hosts.push_back(instance);
            }
        }

        // 结果提示
        if (new_hosts.size() > 0) {
            changed = true;
            LogUtils::LogInfo(StringPrint("HostReactor new ips size: [%d], service: [%s]", new_hosts.size(), key));
        }
        if (remv_hosts.size() > 0) {
            changed = true;
            LogUtils::LogInfo(StringPrint("HostReactor removed ips size: [%d], service: [%s]", remv_hosts.size(), key));
        }
        if (mod_hosts.size() > 0) {
            changed = true;
            LogUtils::LogInfo(StringPrint("HostReactor modified ips size: [%d], service: [%s]", mod_hosts.size(), key));
        }

        service->json_from_server = json;

        // 只要有变化，就重新写入硬盘
        if(new_hosts.size() > 0 || remv_hosts.size() > 0 || mod_hosts.size() > 0){
            // 写入本地文件
            LogUtils::LogInfo(StringPrint("HostReactor 本地目录: [%s], 写入服务信息: [%s]", _cache_dir, service->getKey()));
            DiskCache::write(service, _cache_dir);
        }

    } else {
        LogUtils::LogInfo(StringPrint("HostReactor::processServiceJSON 缓存serviceInfo_map 服务从未存在: [%s]", key));
        // 2.之前没有这个服务
        changed = true;
        LogUtils::LogInfo(StringPrint("HostReactor init new ips size: [%d], key: [%s], service: [%s]", service->ipCount(), key, service->toString()));
        // 更新缓存
        service->json_from_server = json;
        LogUtils::LogInfo(StringPrint("HostReactor serviceInfo_map size: [%d], 更新缓存服务: [%s]", serviceInfo_map.size(), key));
        serviceInfo_map[key] = service;
        // 写入本地文件
        LogUtils::LogInfo(StringPrint("HostReactor 本地目录: [%s], 写入服务信息: [%s]", _cache_dir, service->getKey()));
        DiskCache::write(service, _cache_dir);
    }

    if (changed) {
        LogUtils::LogInfo(StringPrint("HostReactor current ips size: [%d], key: [%s], service: [%s]", service->ipCount(), key, service->toString()));
        LogUtils::LogInfo(StringPrint("HostReactor serviceInfo_map size: [%d]", serviceInfo_map.size()));
    }

    return service;
}

void HostReactor::updateServiceNow(const std::string &service_name, const std::string &clusters){
    // 连接API接口
    std::string result = _server_proxy->queryList(service_name, clusters, _push_receiver->getUDPPort(), false);
    LogUtils::LogInfo(StringPrint("HostReactor::updateServiceNow 更新服务信息: %s", result));
    if(!result.empty()){
        // 更新缓存和本地文件
        processServiceJSON(result);
    }
}

void HostReactor::refreshOnly(const std::string &service_name, const std::string &clusters){
    std::string result = _server_proxy->queryList(service_name, clusters, _push_receiver->getUDPPort(), false);
    if(result.empty()){
        LogUtils::LogError("HostReactor::refreshOnly接口获取结果为空，刷新失败！");
    }
}

/////////////////////////////////////////////////////
void UpdateTask::run(){
    // TODO 增加try-catch

    std::string key = ServiceInfo::getKey(_service_name, _clusters);
    // 从缓存中取出服务信息
    auto iter = _host_reactor->serviceInfo_map.find(key);
    if(iter == _host_reactor->serviceInfo_map.end()){ // 未找到
        // 主动拉取
        _host_reactor->updateServiceNow(_service_name, _clusters);
        return;
    }

    // 缓存中对应的服务
    std::shared_ptr<ServiceInfo> service = iter->second;
    // 服务待更新
    if(service->last_ref_time <= _last_ref_time){
        _host_reactor->updateServiceNow(_service_name, _clusters);
        service = _host_reactor->serviceInfo_map[key];   // 找到更新后的服务, update后一定可以找到
    } else {
        // 服务数据已经由push更新了，不再主动pull
        _host_reactor->refreshOnly(_service_name, _clusters);
    }
    // 更新任务时间
    _last_ref_time = service->last_ref_time;
}
