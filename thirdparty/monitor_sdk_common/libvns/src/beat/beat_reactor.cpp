//
// Created by 吴婷 on 2020-06-15.
//
#include "thirdparty/monitor_sdk_common/base/closure.h"
#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "constants.h"
#include "beat/beat_reactor.h"
#include "utils/utilandcoms.h"
#include "utils/log_utils.h"

using namespace common;

BeatReactor::~BeatReactor(){
    _stop = true;
    _beatMaster->Join();
    delete _beatMaster;
    _beatMaster = NULL;
    // TODO removeAllBeatInfo();
}

void BeatReactor::initialize(){
    _stop = false;
    _beatMaster->Start(std::bind(beatMater, this));   // 开启心跳管理
}

void *BeatReactor::beatMater(void *param){
    BeatReactor *thisobj = (BeatReactor *)param;
    while (!thisobj->_stop) {
        // 加锁
        thisobj->_mutex.Lock();
        std::unordered_map<std::string, std::shared_ptr<BeatTask>>::iterator it = thisobj->beatinfo_list.begin();
        for (; it != thisobj->beatinfo_list.end(); it++) {
            // 当前任务：普通指针类型
            BeatTask *curtask = it->second.get();
            thisobj->_thread_pool->AddTask(NewClosure(curtask, &BeatTask::run));
        }
        thisobj->_mutex.Unlock();
        // 暂停间隔
        sleep(thisobj->_client_beatinterval / 1000);
    }

    return NULL;
}

void BeatReactor::addBeatInfo(const std::string &service_name, BeatInfo &beatInfo){
    LogUtils::LogInfo(StringPrint("[BEAT] adding beat: [%s] to beat map.", service_name));
    //TODO NAMING_LOGGER.info("[BEAT] adding beat: {} to beat map.", beatInfo);

    // 新建心跳任务
    std::shared_ptr<BeatTask> beat_task = std::make_shared<BeatTask>(beatInfo, _server_proxy, this);
    // 生成key
    std::string key = buildKey(service_name, beatInfo.ip, beatInfo.port);
    // 加锁，加入任务列表
    _mutex.Lock();
    auto iter = beatinfo_list.find(key);
    // 存在
    if (iter != beatinfo_list.end()) {
        LogUtils::LogInfo(StringPrint("BeatReactor::addBeatInfo adding already-exist key: %s", key));
        _mutex.Unlock();
        return;
    }
    beatinfo_list[key] = beat_task;
    LogUtils::LogInfo(StringPrint("BeatReactor::addBeatInfo 任务列中的任务数: %d", beatinfo_list.size()));
    _mutex.Unlock();
}

void BeatReactor::removeBeatInfo(const std::string &service_name, const std::string &ip, int port){
    LogUtils::LogInfo(StringPrint("[BEAT] removing beat: [%s:%s:%d] from beat map.", service_name, ip, port));

    // 生成key，从任务列表中找到该任务
    std::string key = buildKey(service_name, ip, port);
    auto iter = beatinfo_list.find(key);
    if (iter == beatinfo_list.end()) {  //不包含
        LogUtils::LogInfo(StringPrint("Removing non-existent key: %s", key));
        return;
    }
    // 找到任务，停止任务上报
    std::shared_ptr<BeatTask> beat_task = iter->second;
    beat_task->beatInfo.stopped = true;
    // 加锁，移除任务
    _mutex.Lock();
    beatinfo_list.erase(key);
    LogUtils::LogInfo(StringPrint("任务列中的任务数: %d", beatinfo_list.size()));
    _mutex.Unlock();

}

void BeatReactor::setClientBeatInterval(int interval){
    _client_beatinterval = interval;
}

std::string BeatReactor::buildKey(const std::string &service_name, const std::string &ip, int port){
    return service_name + Constants::NAMING_INSTANCE_ID_SPLITTER
           + ip + Constants::NAMING_INSTANCE_ID_SPLITTER + common::IntegerToString(port);
}

/////////////////////
BeatTask::BeatTask(BeatInfo &beatInfo, std::shared_ptr<NamingProxy> server_proxy, BeatReactor* beat_reactor)
: beatInfo(beatInfo)
, _server_proxy(server_proxy)
, _beat_reactor(beat_reactor) {
}

void BeatTask::run() {
    // 停止上报，直接返回
    if(beatInfo.stopped){
        return;
    }
    // 上报心跳
    int new_interval = _server_proxy->sendBeat(beatInfo);
    LogUtils::LogInfo(StringPrint("BeatTask::run 本次查询返回心跳间隔为：[%d]", new_interval));
    if(new_interval>0) {
        _beat_reactor->setClientBeatInterval(new_interval); //重置上报时间间隔
        LogUtils::LogInfo(StringPrint("BeatTask::run 更改下次心跳间隔为：[%d]", new_interval));
    }
}