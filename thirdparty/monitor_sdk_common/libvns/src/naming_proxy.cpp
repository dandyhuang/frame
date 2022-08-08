//
// Created by 吴婷 on 2020-06-15.
//
#include <iostream>
#include <sys/syscall.h>
#include <unistd.h>

#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/base/string/number.h"

#include "naming_proxy.h"
#include "constants.h"
#include "thirdparty/monitor_sdk_common_params.h"
#include "utils/log_utils.h"
#include "utils/json_utils.h"
#include "utils/utils.h"
#include "utils/utilandcoms.h"

using namespace common;

int NamingProxy::DEFAULT_DELAY = 20 * 60 * 1000;    // 20分钟刷新一次

NamingProxy::NamingProxy(const std::string &namespace_id, const std::string &cluster_name)
: _namespace_id(namespace_id), _cluster_name(cluster_name)
{
    _vns_domain = UtilAndComs::Agent_URL; // 与agent的通信地址

    // http客户端
    options = GetCommonHttpOptions();
    _client = std::make_shared<HttpClient>();
    _client->SetUserAgent(UtilAndComs::VERSION);  // 设置UserAgent
}

NamingProxy::NamingProxy(){
    _namespace_id = UtilAndComs::DEFAULT_NAMESPACE_ID;
    _cluster_name = "";
    _vns_domain = UtilAndComs::Agent_URL; // 与agent的通信地址

    // http客户端
    options = GetCommonHttpOptions();
    _client = std::make_shared<HttpClient>();
    _client->SetUserAgent(UtilAndComs::VERSION);  // 设置UserAgent
}

bool NamingProxy::init(){
    // 初始化应用名appName
    initAppName(properties);
    // 1.调用与agent初次通信的接口
    std::map<std::string, std::string> params;
    // 获取tag信息
    if(!_cluster_name.empty()){
        params[CommonParams::TAG] = _cluster_name;
    }
    // 获取进程id
    int thread_id = (uint32_t)syscall(SYS_gettid);
    params[CommonParams::PID] = IntegerToString(thread_id);
    // 共有参数集传入同一个pid
    properties[CommonParams::PID] = IntegerToString(thread_id);
    LogUtils::LogInfo("NamingProxy::init pid: " + properties[CommonParams::PID]);
    // appName
    params[CommonParams::APP] = _app_name;
    // 与agent通信上报
    std::string ret = reqAPI(UtilAndComs::Agent_INIT_URL, params, HttpMethod::GET);
    // 接口调试后，再继续下面的判断逻辑
    // 返回值为空，表明寻址失败，再试一次。如果还是失败，等待重试【每次重试的次数,增加】
    int count = 0;
    while (ret.empty() && count < 3){
        ret = reqAPI(UtilAndComs::Agent_INIT_URL, params, HttpMethod::GET);
        LogUtils::LogError(StringPrint("vns sdk初始化失败, 第[%d]次重试, 等待[%d]秒后重试...", count+1, _sleep_sec * count));
        sleep(_sleep_sec * count);
        count++;
    }

    // json解析返回值
    std::shared_ptr<ResultVo> result = std::make_shared<ResultVo>();
    JsonUtils::JsonStr2ResultVo(ret, result);
    if(result->code != 0){
        LogUtils::LogError("vns sdk初始化失败: agent寻址失败");
        //exit(1);
        return false;
    }

    // 2.获取token
    //_scheduled_executor = std::make_shared<ScheduledExecutor>(1);  // 定时任务
    if(!NamingProxy::initToken()){
        LogUtils::LogError("vns sdk初始化失败：获取token失败");
        //exit(1);
        return false;
    }

    return true;
}

bool NamingProxy::initToken(){
    LogUtils::LogInfo(StringPrint("NamingProxy::initToken appName:[%s], appSecret:[%s], appEnv:[%s]",
            properties[PropertyKeyConst::APP_NAME], properties[PropertyKeyConst::APP_SECRET], properties[PropertyKeyConst::NAMESPACE]));
    // 1.初始化连接获取token
    int ret_code = getToken(LocalProperties::app_name, properties[PropertyKeyConst::APP_SECRET], properties[PropertyKeyConst::NAMESPACE]);
    LogUtils::LogInfo(StringPrint("NamingProxy::initToken 返回状态码:[%d]", ret_code));
    // 2.判断返回结果
    if(ret_code != 200){
        return false;
    }
    /*// 3.成功则开启定时任务 20201124 继坤延长了服务端的过期时间，此处定时刷新先屏蔽
    _scheduled_executor->ScheduleWithFixedDelay([this](bool *terminated) {
        if((*terminated)){
            return;
        }
        LogUtils::LogInfo(StringPrint("NamingProxy::initToken 定时更新token, accessToken[%s], refreshToken[%s]", _access_token, _refresh_token));
        refreshToken(_refresh_token, LocalProperties::env);
    }, 1000, DEFAULT_DELAY);*/

    LogUtils::LogInfo(StringPrint("NamingProxy::initToken 获取token成功，accessToken[%s]与refreshToken[%s]", _access_token, _refresh_token));

    return true;
}

int NamingProxy::getToken(const std::string &app_name, const std::string &app_secret, const std::string &app_env){
    // 1.连接api，获取token
    std::map<std::string, std::string> params;
    params[PropertyKeyConst::APP_NAME] = app_name;
    params[PropertyKeyConst::APP_SECRET] = app_secret;
    params[PropertyKeyConst::APP_ENV] = app_env;
    params["grantType"] = "authorization_code";

    int ret_code = reqToken(params);
    LogUtils::LogInfo(StringPrint("NamingProxy::getToken 返回状态码:[%d]", ret_code));

    // 2.返回码判断
    if(ret_code != 200){
        LogUtils::LogError(StringPrint("NamingProxy::reqToken 获取token失败，检查appName[%s]与appSecret[%s]是否合法",
                                       properties[PropertyKeyConst::APP_NAME], properties[PropertyKeyConst::APP_SECRET]));
    }
    return ret_code;
}

int NamingProxy::refreshToken(const std::string &refresh_token, const std::string &app_env){
    // 2.连接api，更新token
    std::map<std::string, std::string> params;
    params["refreshToken"] = refresh_token;
    params[PropertyKeyConst::APP_ENV] = app_env;
    params["grantType"] = "refresh_token";

    int ret_code = reqToken(params);
    LogUtils::LogInfo(StringPrint("NamingProxy::refreshToken 返回状态码:[%d]", ret_code));
    return ret_code;
}

int NamingProxy::reqToken(std::map<std::string, std::string> &params){
    // 1.请求token的url
    std::string request_url = _vns_domain + UtilAndComs::NACOS_URL_TOKEN;
    // http返回值
    HttpResponse response;
    HttpClient::ErrorCode error;

    // 将传入的map参数，转换为json字符串
    std::string payload = JsonUtils::ToJSONString(params);
    if(!_client->Post(request_url, payload, options, &response, &error)){
        LogUtils::LogError(StringPrint("Http Fail! method:[Post], url: [%s] \n params: %s \n", request_url, JsonUtils::ToJSONString(params)));
        return response.Status();
    }

    // 2.解析返回结果
    std::string result = response.Body();
    LogUtils::LogInfo(StringPrint("Http method:[reqToken], result: [%s], url: [%s] \n params: %s \n", result, request_url, JsonUtils::ToJSONString(params)));
    // 判断是否为空
    if(result.empty()){
        LogUtils::LogError(StringPrint("NamingProxy::reqToken Http Response Empty! 请求成功，但返回内容为空。后台接口波动或者寻址失败，需要确认app[%s]与tag[%s]是否对应! method:[POST], url: [%s] \n params: %s \n",
                                       properties[PropertyKeyConst::APP_NAME], _cluster_name, request_url, JsonUtils::ToJSONString(params)));
        return -1;      // 返回内容为空，直接返回-1
    }
    // json解析返回值
    std::shared_ptr<TokenInfo> token_info = std::make_shared<TokenInfo>();
    JsonUtils::JsonStr2TokenInfo(result, token_info);
    if(token_info->retcode == 200){
        // 3.token值更新
        _access_token = token_info->data["accessToken"];
        _refresh_token = token_info->data["refreshToken"];
        LogUtils::LogInfo(StringPrint("NamingProxy::reqToken 获取token成功，accessToken[%s]与refreshToken[%s]", _access_token, _refresh_token));
    }

    return token_info->retcode;
}

NamingProxy::CheckCode NamingProxy::checkResponse(const std::string &response, const std::string &method,
        const std::string &url, std::map<std::string, std::string>params, int req_count){
    // 成功请求返回有3种情况：成功，权限不过的转发,返回内容为空
    // 1.返回内容为空：接响应时间较长，一般都是寻址失败，需要确认app和tag
    if(response.empty()){
        LogUtils::LogError(StringPrint("NamingProxy::checkResponse Http Response Empty! 请求成功，但返回内容为空。后台接口波动!!! method:[%s], url: [%s] \n params: %s \n",
                method, url, JsonUtils::ToJSONString(params)));
        return NamingProxy::QUIT;
    }
    // 通过返回字符判断：如果含status，则为权限不过的转发
    // 2.接口正常返回
    if(response.find("status") == StringPiece::npos){
        LogUtils::LogInfo(StringPrint("NamingProxy::checkResponse Http Succeed! 请求次数:[%d], method:[%s], url: [%s], params: [%s]\n",
                req_count+1, method, url, JsonUtils::ToJSONString(params)));
        return NamingProxy::SUCCESS;
    }
    // 3. 权限不过的转发
    // json解析
    std::shared_ptr<ResponseError> response_error = std::make_shared<ResponseError>();
    if(!JsonUtils::JsonStr2ResponseError(response, response_error)){
        LogUtils::LogError(StringPrint("NamingProxy::checkResponse JsonStr2ResponseError解析失败: %s", response_error->toString()));
        return NamingProxy::QUIT;
    }
    LogUtils::LogInfo(StringPrint("NamingProxy::checkResponse JsonStr2ResponseError成功解析: %s", response_error->toString()));
    // 状态码
    int response_status_code = response_error->status;
    if(HttpStatusCode::OK == response_status_code){                   // 请求成功:200
        LogUtils::LogInfo(StringPrint("NamingProxy::checkResponse Http Succeed! 请求次数:[%d], method:[%s], url: [%s] \n params: %s \n", req_count+1, method, url, JsonUtils::ToJSONString(params)));
        return NamingProxy::SUCCESS;
    }
    if(HttpStatusCode::InternalServerError == response_status_code){   // 请求失败:500
        LogUtils::LogError(StringPrint("NamingProxy::checkResponse Http Response 状态码为500! 请求失败! method:[%s], url: [%s] \n params: %s \n", method, url, JsonUtils::ToJSONString(params)));
        return NamingProxy::QUIT;
    }
    if(HttpStatusCode::Unauthorized == response_status_code){          // 未经授权:401
        LogUtils::LogInfo(StringPrint("NamingProxy::checkResponse Http Response 状态码为401! 未经授权! NamingProxy::checkResponse 主动刷新token, 刷新凭证:[%s]", _refresh_token));
        for (int i = 0; i < 2; ++i) {   // 重试次数：2次，2次不成功直接退出
            // 刷新
            int ret_code = refreshToken(_refresh_token, LocalProperties::env);
            if(HttpStatusCode::OK == ret_code){                    // 请求成功:200
                LogUtils::LogInfo(StringPrint("NamingProxy::checkResponse 刷新token成功, Http Try Again! token:[%s], method:[%s], url: [%s]", _access_token, method, url));
                return NamingProxy::RETRY;  // 重试
            }
            if(HttpStatusCode::Unauthorized == ret_code){          // 未经授权:401，重新获取
                int ret_code = getToken(LocalProperties::app_name, properties[PropertyKeyConst::APP_SECRET], LocalProperties::env);
                LogUtils::LogInfo(StringPrint("NamingProxy::checkResponse 返回状态码:[%d]", ret_code));
                // 仍然失败，直接返回
                if(ret_code != 200){
                    LogUtils::LogError(StringPrint("NamingProxy::checkResponse 重新请求token失败, 确认token获取参数, appName[%s], appSecret[%s], appEnv[%s]! 当前token:[%s], method:[%s], url: [%s]",
                                                   LocalProperties::app_name, properties[PropertyKeyConst::APP_SECRET], LocalProperties::env, _access_token, method, url));
                    return NamingProxy::QUIT;
                } else {   // 成功，重新请求一次
                    LogUtils::LogInfo(StringPrint("NamingProxy::checkResponse 获取token成功, Http Try Again! token:[%s], method:[%s], url: [%s]", _access_token, method, url));
                    return NamingProxy::RETRY;
                }
            }
            // 其他情况，重试
            LogUtils::LogInfo(StringPrint("NamingProxy::checkResponse 刷新token失败, Http Try Again! 请求返回码: [%d]", ret_code));
        }
    }

    // 其他码直接退出
    LogUtils::LogError(StringPrint("NamingProxy::checkResponse Http Fail! 请求次数:[%d], method:[%s], url: [%s] \n params: %s \n", req_count+1, method, url, JsonUtils::ToJSONString(params)));
    return NamingProxy::QUIT;
}

std::string NamingProxy::reqAPI(const std::string &api, std::map<std::string, std::string> &params, const std::string &method){
    // TODO 2.0进行参数校验
    // injectSecurityInfo(params);

    // 请求转发接口与初始化接口区分开
    std::string request_url = api;
    if (!StringStartsWith(api, "http://")){
        // 拼接url
        request_url = _vns_domain + api;
        params[CommonParams::NAMESPACE_ID] = _namespace_id;
        // 增加权限校验参数
        if(!_access_token.empty()){  // 如果token非首次获取，在请求参数中添加
            params[CommonParams::ACCESS_TOKEN] = _access_token;                     // token
            params[CommonParams::APP_ENV] = LocalProperties::env;  // 环境
        }
    }

    // http返回值
    HttpResponse response;
    HttpClient::ErrorCode error;

    // api接口连接失败，最多重试3次
    for (int i = 0; i < UtilAndComs::REQUEST_DOMAIN_RETRY_COUNT; ++i) {
        // 根据请求方法不同，进行连接
        if(HttpMethod::GET == method){
            std::string url = buildUltimateUrl(request_url, params); // 拼接url地址
            if(!_client->Get(url, options, &response, &error)){
                LogUtils::LogError(StringPrint("Http Fail! 请求次数:[%d], method:[%s], url: [%s] \n params: %s \n ErrorCode: %d \n", i+1, method, url, JsonUtils::ToJSONString(params), (int)error));

                // 响应状态判断
                std::string result = response.Body();
                CheckCode code = checkResponse(result, method, url, params, i);
                if(NamingProxy::SUCCESS == code){       // 成功，直接返回
                    return result;
                } else if(NamingProxy::QUIT == code) {  // 直接退出
                    break;
                } else if(NamingProxy::RETRY == code) { // 重试
                    params[CommonParams::ACCESS_TOKEN] = _access_token;     // 更新token值
                    i--;
                    continue;
                }
            }
            else
            {
                return response.Body();
            }
        } else if (HttpMethod::DELETE == method){
            std::string url = buildUltimateUrl(request_url, params); // 拼接url地址
            if(!_client->Delete(url, options, &response, &error)){
                LogUtils::LogError(StringPrint("Http Fail! 请求次数:[%d], method:[%s], url: [%s] \n params: %s \n ErrorCode: %d \n", i+1, method, url, JsonUtils::ToJSONString(params), (int)error));

                // 响应状态判断
                std::string result = response.Body();
                CheckCode code = checkResponse(result, method, url, params, i);
                if(NamingProxy::SUCCESS == code){       // 成功，直接返回
                    return result;
                } else if(NamingProxy::QUIT == code) {  // 直接退出
                    break;
                } else if(NamingProxy::RETRY == code) { // 重试
                    params[CommonParams::ACCESS_TOKEN] = _access_token;     // 更新token值
                    i--;
                    continue;
                }
            }
            else
            {
                return response.Body();
            }
        } else if (HttpMethod::POST == method){
            std::string url = buildUltimateUrl(request_url, params); // 测试拼接
            // 将传入的map参数，转换为json字符串
            std::string payload = JsonUtils::ToJSONString(params);
            if(!_client->Post(url, payload, options, &response, &error)){
                LogUtils::LogError(StringPrint("Http Fail! 请求次数:[%d], method:[%s], url: [%s] \n params: %s \n ErrorCode: %d \n", i+1, method, url, JsonUtils::ToJSONString(params), (int)error));

                // 响应状态判断
                std::string result = response.Body();
                CheckCode code = checkResponse(result, method, url, params, i);
                if(NamingProxy::SUCCESS == code){       // 成功，直接返回
                    return result;
                } else if(NamingProxy::QUIT == code) {  // 直接退出
                    break;
                } else if(NamingProxy::RETRY == code) { // 重试
                    params[CommonParams::ACCESS_TOKEN] = _access_token;     // 更新token值
                    i--;
                    continue;
                }
            }
            else
            {
                return response.Body();
            }
        } else if (HttpMethod::PUT == method){
            std::string url = buildUltimateUrl(request_url, params); // 测试拼接
            // 将传入的map参数，转换为json字符串
            std::string payload = JsonUtils::ToJSONString(params);
            if(!_client->Put(url, payload, options, &response, &error)){
                std::string trace_id;
                response.Headers().Get("trace_id", &trace_id);
                LogUtils::LogError(StringPrint("Http Fail! 请求次数:[%d], method:[%s], url: [%s] \n params: %s \n ErrorCode: %d \n StatusCode: %d \n response body: %s \n trace_id: %s \n",
                        i+1, method, url, JsonUtils::ToJSONString(params), (int)error, (int)response.Status(), (std::string)response.Body(), trace_id));

                // 响应状态判断
                std::string result = response.Body();
                CheckCode code = checkResponse(result, method, url, params, i);
                if(NamingProxy::SUCCESS == code){       // 成功，直接返回
                    return result;
                } else if(NamingProxy::QUIT == code) {  // 直接退出
                    break;
                } else if(NamingProxy::RETRY == code) { // 重试
                    params[CommonParams::ACCESS_TOKEN] = _access_token;     // 更新token值
                    i--;
                    continue;
                }
            }
            else
            {
                return response.Body();
            }
        }
    }

    return "";
}

bool NamingProxy::registerService(std::string service_name, std::string group_name, Instance instance){
    LogUtils::LogInfo(StringPrint("[REGISTER-SERVICE] namespaceId:[%s], service:[%s], instance:[%s]", _namespace_id, service_name, instance.toString()));
    std::map<std::string, std::string> params;
    // 必填
    params["app"] = _app_name;
    params["ip"] = instance.ip;
    params["port"] = IntegerToString(instance.port);
    params[CommonParams::SERVICE_NAME] = service_name;
    // 获取进程id
    params[CommonParams::PID] = properties[CommonParams::PID];
    LogUtils::LogInfo("NamingProxy::registerService pid: " + params[CommonParams::PID]);

    // 选填
    if(!LocalProperties::app_loc.empty()){
        params["appLoc"] = LocalProperties::app_loc;
    }
    params[CommonParams::NAMESPACE_ID] = _namespace_id;
    params[CommonParams::CLUSTER_NAME] = instance.cluster_name;
    params["weight"] = DoubleToString(instance.weight);
    params["enable"] = VnsStrUtils::BoolToString(instance.enabled);
    params["healthy"] = VnsStrUtils::BoolToString(instance.healthy);
    params["ephemeral"] = VnsStrUtils::BoolToString(instance.ephemeral);
    params["metadata"] = JsonUtils::ToJSONString(instance.metadata);

    std::string result = reqAPI(UtilAndComs::NACOS_URL_INSTANCE, params, HttpMethod::POST);
    if(result != "ok"){
        LogUtils::LogError(StringPrint("[REGISTER-SERVICE] Fail! result:[%s]", result));
        return false;
    }
    LogUtils::LogInfo(StringPrint("[REGISTER-SERVICE] result:[%s]", result));
    return true;
}

void NamingProxy::deregisterService(std::string service_name, Instance instance){
    LogUtils::LogInfo(StringPrint("[DEREGISTER-SERVICE] namespaceId:[%s], service:[%s], instance:[%s]", _namespace_id, service_name, instance.toString()));
    std::map<std::string, std::string> params;
    // 必填
    params["app"] = _app_name;
    params["ip"] = instance.ip;
    params["port"] = IntegerToString(instance.port);
    params[CommonParams::SERVICE_NAME] = service_name;

    // 选填
    params[CommonParams::NAMESPACE_ID] = _namespace_id;
    params[CommonParams::CLUSTER_NAME] = instance.cluster_name;
    params["ephemeral"] = VnsStrUtils::BoolToString(instance.ephemeral);
    params["weight"] = DoubleToString(instance.weight);
    params["enable"] = VnsStrUtils::BoolToString(instance.enabled);
    params["healthy"] = VnsStrUtils::BoolToString(instance.healthy);

    std::string result = reqAPI(UtilAndComs::NACOS_URL_INSTANCE, params, HttpMethod::DELETE);
    LogUtils::LogInfo(StringPrint("[DEREGISTER-SERVICE] result:[%s]", result));
}

/**
 * 发送心跳，返回下次心跳间隔(毫秒)
 * @param beatInfo
 * @return
 */
int NamingProxy::sendBeat(BeatInfo &beatInfo){
    try {
        LogUtils::LogInfo(StringPrint("[BEAT] {%s} sending beat to server: {%s}", _namespace_id, beatInfo.toString()));
        std::map<std::string, std::string> params;
        // 必填
        params["app"] = _app_name;
        params[CommonParams::SERVICE_NAME] = beatInfo.service_name;
        // 获取进程id
        params[CommonParams::PID] = properties[CommonParams::PID];
        LogUtils::LogInfo("NamingProxy::sendBeat pid: " + params[CommonParams::PID]);

        // 选填
        params["beat"] = beatInfo.toString();
        params[CommonParams::NAMESPACE_ID] = _namespace_id;
        params[CommonParams::CLUSTER_NAME] = beatInfo.cluster_name;
        params["ip"] = beatInfo.ip;
        if(!LocalProperties::app_loc.empty()){
            params["appLoc"] = LocalProperties::app_loc;
        }
        params["port"] = IntegerToString(beatInfo.port);

        std::string result = reqAPI(UtilAndComs::NACOS_URL_BASE + "/instance/beat", params, HttpMethod::PUT);
        // 解析返回类
        std::shared_ptr<BeatResult> beat_result = std::make_shared<BeatResult>();
        JsonUtils::JsonStr2BeatResult(result, beat_result);

        if(beat_result->client_beat_interval != -1){
            return beat_result->client_beat_interval;
        }

    } catch(...) {
        LogUtils::LogError(StringPrint("[CLIRNT-BEAT] failed to send beat: {%s}", beatInfo.toString()));
    }

    return 0;
}

std::string NamingProxy::queryList(const std::string &service_name, const std::string &clusters, int udp_port, bool healthy_only){
    std::map<std::string, std::string> params;
    // 必填
    params["app"] = _app_name;
    params[CommonParams::SERVICE_NAME] = service_name;
    // 获取进程id
    params[CommonParams::PID] = properties[CommonParams::PID];
    LogUtils::LogInfo("NamingProxy::queryList pid: " + params[CommonParams::PID]);

    // 选填
    //params["env"] = "";         // 环境参数
    //params["isCheck"] = VnsStrUtils::BoolToString(false);   // 是否开启健康保护
    params["User-Agent"] = UtilAndComs::VERSION;
    params["clientIP"] = NetUtils::localIP();
    if(!LocalProperties::app_loc.empty()){
        params["appLoc"] = LocalProperties::app_loc;
    }
    params[CommonParams::NAMESPACE_ID] = _namespace_id;
    params["clusters"] = clusters;
    params["udpPort"] = IntegerToString(udp_port);
    params["healthyOnly"] = VnsStrUtils::BoolToString(healthy_only);

    // 如果接口获取失败，就返回空字符串
    return reqAPI(UtilAndComs::NACOS_URL_BASE + "/instance/list", params, HttpMethod::GET);
}

/**
 * 生成最终的请求url
 * @param url
 * @param param_map
 * @return
 */
std::string NamingProxy::buildUltimateUrl(const std::string &url, std::map<std::string, std::string> param_map){
    std::string ultimate_url;
    std::string pairs = VnsStrUtils::mapToKeyValueStr(param_map);
    if(!pairs.empty()){
        ultimate_url = url + "?" + pairs;
    } else{
        ultimate_url = url;
    }
    return ultimate_url;
}

/**
 * Http header配置
 * @return
 */
HttpClient::Options NamingProxy::GetCommonHttpOptions() {
    HttpClient::Options options;
    options.AddHeader("User-Agent", UtilAndComs::VERSION);
    options.AddHeader("Content-Type", "application/json");
    options.AddHeader("Accept", "application/json");
    options.AddHeader("Vns-naming-tag", _cluster_name);
    return options;
}

void NamingProxy::initAppName(std::map<std::string, std::string> properties){
    if(!properties.empty()){
        auto iter = properties.find(PropertyKeyConst::APP_NAME);
        if(iter != properties.end()){
            _app_name = iter->second;
            return;
        }
    }
    _app_name = LocalProperties::app_name;
}