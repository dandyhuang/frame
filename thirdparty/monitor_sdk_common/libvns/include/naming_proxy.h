//
// Created by 吴婷 on 2020-06-15.
//

#ifndef LIBVNS_NAMING_PROXY_H
#define LIBVNS_NAMING_PROXY_H

#include <map>
#include <vector>
#include <memory>

#include "thirdparty/monitor_sdk_common/net/uri/uri.h"
#include "thirdparty/monitor_sdk_common/net/http/client.h"

#include "object/instance.h"
#include "beat/beat_info.h"
#include "scheduled/scheduled_executor.h"

namespace common {
/**
 * 用于与agent服务端通信
 * 注册服务、注销服务、发送心跳等
 */
class NamingProxy {
public:
    // 权限校验后的返回码
    enum CheckCode {
        SUCCESS = 0,    // 成功
        RETRY,          // 重试
        QUIT,           // 退出
    };

public:
    NamingProxy();
    NamingProxy(const std::string &namespace_id, const std::string &cluster_name);

    /**
     * 初始化向agent发送寻址信息
     */
    bool init();

    /**
     * 注册服务
     * @param service_name 服务名
     * @param group_name 集群名
     * @param instance 实例
     */
    bool registerService(std::string service_name, std::string group_name, Instance instance);

    /**
     * 注销服务
     * @param service_name 服务名
     * @param instance 实例
     */
    void deregisterService(std::string service_name, Instance instance);

    /**
     * 上报心跳信息
     * @param beatInfo 心跳信息
     * @return 下次上报时间间隔
     */
    int sendBeat(BeatInfo &beatInfo);

    /**
     * 查询实例
     * @param service_name 服务名
     * @param clusters 集群名
     * @param udp_port UDP端口
     * @param healthy_only 是否只取存活
     * @return
     */
    std::string queryList(const std::string &service_name, const std::string &clusters, int udp_port, bool healthy_only);

    /**
     * api请求
     * @param api
     * @param params
     * @param method
     * @return
     */
    std::string reqAPI(const std::string &api, std::map<std::string, std::string> &params, const std::string &method);

private:
    /**
     * sdk启动获取token并定时更新token
     * @return 是否获取token成功
     */
    bool initToken();

    /**
     * 连接token获取接口，只对成功请求更新token
     * @param params
     * @return 响应状态码
     */
    int reqToken(std::map<std::string, std::string> &params);

    /**
     * 通过密钥获取token
     * @param app_name 应用名
     * @param app_secret 密钥
     * @param app_env
     * @return 响应状态码
     */
    int getToken(const std::string &app_name, const std::string &app_secret, const std::string &app_env);

    /**
     * 刷新token
     * @param refresh_token 刷新凭证
     * @param app_env
     * @return 响应状态码
     */
    int refreshToken(const std::string &refresh_token, const std::string &app_env);

    /**
     * 响应状态判断，确定随后操作：成功/退出/重试
     * @param response 响应内容
     * @param method 请求方法
     * @param url 请求url
     * @param params 请求参数
     * @param req_count 请求次数
     * @return 权限校验返回码
     */
    NamingProxy::CheckCode checkResponse(const std::string &response, const std::string &method, const std::string &url, std::map<std::string, std::string> params, int req_count);

    std::string buildUltimateUrl(const std::string &url, std::map<std::string, std::string> param_map);
    HttpClient::Options GetCommonHttpOptions();

    /**
     * 从配置中获取应用名
     * @param properties
     */
    void initAppName(std::map<std::string, std::string> properties);

public:
    std::map<std::string, std::string> properties;  // 传入配置
    HttpClient::Options options;                    // http Header信息

private:
    static int DEFAULT_DELAY;           // 默认定时刷新token轮询间隔（单位：毫秒）
    std::string _app_name;      // 应用名
    std::string _namespace_id;  // 空间id
    std::string _cluster_name;  // 集群名
    std::string _vns_domain;    // 与agent通信的地址
    int _sleep_sec = 1;         // 休眠时间（秒）
    std::string _access_token;  // 获取的token
    std::string _refresh_token; // 刷新凭据
    std::shared_ptr<ScheduledExecutor> _scheduled_executor = nullptr; // 调度类：定时更新token
    // http客户端
    std::shared_ptr<HttpClient> _client = nullptr;

};

} //namespace common

#endif //LIBVNS_NAMING_PROXY_H
