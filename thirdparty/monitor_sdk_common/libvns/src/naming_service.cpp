//
// Created by 吴婷 on 2020-07-01.
//

#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/storage/path/path.h"

#include "naming_service.h"
#include "constants.h"
#include "thirdparty/monitor_sdk_common_params.h"
#include "utils/utilandcoms.h"
#include "utils/utils.h"
#include "utils/log_utils.h"

using namespace common;

bool NamingService::_init_connect = false;

NamingService::NamingService(){
    NamingService(_properties);
}

NamingService::NamingService(std::map<std::string, std::string> properties){
    _properties = properties;
    initAppName(properties);
    initNamespace(properties);
    initClusterName(properties);
    initCacheDir(properties);
    initAppLoc(properties);
    LogUtils::LogInfo(StringPrint("NamingService 缓存目录: [%s]", _cache_dir));
}

NamingService::~NamingService(){
    if(_beat_reactor != NULL) {
        delete _beat_reactor;
        _beat_reactor = NULL;
    }
}

bool NamingService::init(){
    _server_proxy = std::make_shared<NamingProxy>(_namespace, _cluster_name);
    _server_proxy->properties = _properties;
    _beat_reactor = new BeatReactor(_server_proxy, initClientBeatThreadCount(_properties));
    // TODO 可设置HostReactor的线程数
    _host_reactor = std::make_shared<HostReactor>(_server_proxy, _cache_dir, isLoadCacheAtStart(_properties), initPollingThreadCount(_properties));

    if(!_server_proxy->init()){
        LogUtils::LogError("vns sdk初始化失败");
        return false;
    }
    _beat_reactor->initialize();
    _host_reactor->initialize();

    _init_connect = true;
    return true;
}

void NamingService::initAppName(std::map<std::string, std::string> properties){
    if(!properties.empty()){
        auto iter = properties.find(PropertyKeyConst::APP_NAME);
        if(iter != properties.end()){
            LocalProperties::app_name = iter->second;
        }
    }
}

void NamingService::initNamespace(std::map<std::string, std::string> properties){
    // 判断传入参数中，是否有namespace。若无，填默认值
    if(!properties.empty()){
        auto iter = properties.find(PropertyKeyConst::NAMESPACE);
        if(iter != properties.end()){
            _namespace = iter->second;
        }
    }
    if(_namespace.empty()){
        _namespace = UtilAndComs::DEFAULT_NAMESPACE_ID;
    }

    LocalProperties::env = _namespace;
}

void NamingService::initClusterName(std::map<std::string, std::string> properties){
    if(!properties.empty()){
        auto iter = properties.find(PropertyKeyConst::CLUSTER_NAME);
        if(iter != properties.end()){
            _cluster_name = iter->second;
        }
    }
    if(_cluster_name.empty()){
        _cluster_name = Constants::DEFAULT_CLUSTER_NAME;
    }

    LocalProperties::tag = _cluster_name;
}

void NamingService::initAppLoc(std::map<std::string, std::string> properties){
    if(!properties.empty()){
        auto iter = properties.find(PropertyKeyConst::APP_LOC);
        if(iter != properties.end()){
            LocalProperties::app_loc = iter->second;
        }
    }
}

void NamingService::initCacheDir(std::map<std::string, std::string> properties){
    // 参数中传入，否则使用默认值
    if(!properties.empty()){
        auto iter = properties.find(PropertyKeyConst::CACHE_PATH);
        if(iter != properties.end()){
            _cache_dir = iter->second;
        }
    }
    // 路径为空或者没有/，返回使用默认路径
    if(_cache_dir.empty() || _cache_dir.find('/') == StringPiece::npos){
        _cache_dir = Constants::DEFAULT_CACHE_PATH;
    } else {
        // 判断路径最后是否加/,没有添加
        int index = _cache_dir.rfind('/');
        int str_len = _cache_dir.length();
        if(index != str_len-1){
            _cache_dir += "/";
        }
    }

    // 拼接namespace
    _cache_dir += _namespace;
    // 创建目录
    if(VnsFileUtils::AccessDirExist(_cache_dir)){
        LogUtils::LogInfo(StringPrint("NamingService::initCacheDir, 创建目录成功: [%s]", Path::ToAbsolute(_cache_dir)));
    } else {
        LogUtils::LogError(StringPrint("NamingService::initCacheDir, 创建目录失败: [%s]", Path::ToAbsolute(_cache_dir)));
    }
}

int NamingService::initClientBeatThreadCount(std::map<std::string, std::string> properties){
    // 判断传入参数中，是否有心跳上报线程数。若无，填默认值
    if(!properties.empty()){
        auto iter = properties.find(PropertyKeyConst::NAMING_CLIENT_BEAT_THREAD_COUNT);
        if(iter != properties.end()){
            int ret;
            StringToNumber(iter->second, &ret);
            return ret;
        }
    }
    // 默认值
    return UtilAndComs::DEFAULT_CLIENT_BEAT_THREAD_COUNT;
}

int NamingService::initPollingThreadCount(std::map<std::string, std::string> properties){
    // 判断传入参数中，是否有心跳上报线程数。若无，填默认值
    if(!properties.empty()){
        auto iter = properties.find(PropertyKeyConst::NAMING_POLLING_THREAD_COUNT);
        if(iter != properties.end()){
            int ret;
            StringToNumber(iter->second, &ret);
            return ret;
        }
    }
    // 默认值
    return UtilAndComs::DEFAULT_POLLING_THREAD_COUNT;
}

bool NamingService::isLoadCacheAtStart(std::map<std::string, std::string> properties){
    // 判断传入参数中，是否有。若无，填默认值
    if(!properties.empty()){
        auto iter = properties.find(PropertyKeyConst::NAMING_LOAD_CACHE_AT_START);
        if(iter != properties.end()){
            return ((iter->second) == "true") ? true : false;
        }
    }
    return false;
}

bool NamingService::registerInstance(std::string service_name, std::string group_name, Instance instance){
    // 判断是否初始化成功
    if(!_init_connect){
        LogUtils::LogError("NamingService::registerInstance, 注册失败! 原因：vns 初始化失败!");
        return false;
    }

    std::string name = NamingUtils::getGroupedName(service_name, group_name);
    if(instance.ephemeral){
        // 增加心跳上报
        BeatInfo beatInfo;
        beatInfo.service_name = name;
        beatInfo.ip = instance.ip;
        beatInfo.port = instance.port;
        beatInfo.cluster_name = instance.cluster_name;
        beatInfo.weight = instance.weight;
        beatInfo.metadata = instance.metadata;
        beatInfo.scheduled = false;
        beatInfo.period = instance.getInstanceHeartBeatInterval();
        _beat_reactor->addBeatInfo(name, beatInfo);
    }
    // proxy注册
    return _server_proxy->registerService(name, group_name, instance);
}

bool NamingService::registerInstance(std::string service_name, std::string ip, int port){
    return registerInstance(service_name, Constants::DEFAULT_GROUP, ip, port, _cluster_name);
}

bool NamingService::registerInstance(std::string service_name, std::string group_name, std::string ip, int port, std::string cluster_name){
    Instance instance;
    instance.ip = ip;
    instance.port = port;
    instance.weight = 1;
    instance.cluster_name = cluster_name;

    return registerInstance(service_name, group_name, instance);
}

void NamingService::deregisterInstance(std::string service_name, std::string group_name, Instance instance){
    // 判断是否初始化成功
    if(!_init_connect){
        LogUtils::LogError("NamingService::deregisterInstance, 注销失败! 原因：vns 初始化失败!");
        return;
    }

    std::string name = NamingUtils::getGroupedName(service_name, group_name);
    if(instance.ephemeral){
        _beat_reactor->removeBeatInfo(name, instance.ip, instance.port);
    }
    _server_proxy->deregisterService(name, instance);
}

std::vector<Instance> NamingService::getAllInstances(std::string service_name, std::string group_name, std::vector<std::string> clusters, bool subscribe){
    std::vector<Instance> instances;    // 返回实例
    // 判断是否初始化成功
    if(!_init_connect){
        LogUtils::LogError("NamingService::getAllInstances, 查询/订阅失败! 原因：vns 初始化失败!");
        return instances;
    }

    std::string name = NamingUtils::getGroupedName(service_name, group_name);  // 组名@@服务名
    std::string sclusters = JoinStrings(clusters, ",");
    std::shared_ptr<ServiceInfo> service = std::make_shared<ServiceInfo>(name, sclusters);
    // 是否订阅
    if(subscribe){
        service = _host_reactor->getServiceInfo(name, sclusters);
    } else {
        service = _host_reactor->getServiceInfoDirectlyFromServer(name, sclusters);
    }
    // 判断返回值
    if(service->hosts.size() == 0){
        LogUtils::LogInfo(StringPrint("NamingService::getAllInstances, 查询/订阅失败! service_name: [%s]",name));
        return instances;
    }

    LogUtils::LogInfo(StringPrint("NamingService::getAllInstances, 查询/订阅成功! service_name: [%s], 实例数: [%d]",name, instances.size()));
    return service->hosts;
}

std::vector<Instance> NamingService::selectInstances(std::string service_name, std::string group_name,
        std::vector<std::string> clusters, bool healthy, bool subscribe){
    std::vector<Instance> instances;    // 返回实例
    // 判断是否初始化成功
    if(!_init_connect){
        LogUtils::LogError("NamingService::getAllInstances, 查询/订阅失败! 原因：vns 初始化失败!");
        return instances;
    }

    std::string name = NamingUtils::getGroupedName(service_name, group_name);
    std::string sclusters = JoinStrings(clusters, ",");
    std::shared_ptr<ServiceInfo> service = std::make_shared<ServiceInfo>(name, sclusters);
    // 是否订阅
    if(subscribe){
        service = _host_reactor->getServiceInfo(name, sclusters);
    } else {
        service = _host_reactor->getServiceInfoDirectlyFromServer(name, sclusters);
    }
    // 判断返回值
    if(service->hosts.size() == 0){
        LogUtils::LogInfo(StringPrint("NamingService::selectInstances, 查询/订阅失败! service_name: [%s]", name));
        return instances;
    }
    // 按条件选取
    return selectInstances(service, healthy);
}

std::vector<Instance> NamingService::selectInstances(std::string service_name, bool healthy, bool subscribe){
    std::vector<std::string> clusters;
    return selectInstances(service_name, Constants::DEFAULT_GROUP, clusters, healthy, subscribe);
}

std::vector<Instance> NamingService::selectInstances(std::shared_ptr<ServiceInfo> service, bool healthy){
    std::vector<Instance> healthy_instances;                    // 待返回的实例
    std::vector<Instance> service_instances = service->hosts;   // 服务所有的实例
    // 判断server是否存在内容
    if(service_instances.size() == 0){
        return healthy_instances;
    }

    for (auto iter : service_instances) {
        if(healthy != iter.healthy || !iter.enabled || iter.weight <= 0){   // 取出条件判断：健康状态，是否可用，权重
            continue;
        }
        healthy_instances.push_back(iter);  // 没有问题的才添加进来
    }
    LogUtils::LogInfo(StringPrint("NamingService::selectInstances, 查询/订阅成功! service_name: [%s], 实例数: [%d]",service->getKey(), healthy_instances.size()));
    return healthy_instances;
}

void NamingService::setUDPPort(int port){
    PropertyKeyConst::UDP_PORT = port;
    LogUtils::LogInfo(StringPrint("NamingService::setUDPPort, UDP端口: [%d]", PropertyKeyConst::UDP_PORT));
}