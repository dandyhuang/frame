//
// Created by 吴婷 on 2019-11-23.
//
#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/base/string/string_piece.h"
#include "thirdparty/monitor_sdk_common/net/uri/uri.h"
#include "thirdparty/monitor_sdk_common/libvep/provider/http/http_invoker.h"
#include "thirdparty/monitor_sdk_common/libvep/util/utils.h"

using namespace common;

/**
 * 生成最终的请求url
 * @param url
 * @param param_map
 * @return
 */
std::string VepHttpInvoker::buildUltimateUrl(const std::string &url, std::map<std::string, std::string> param_map){
    std::string ultimate_url;
    std::string pairs = VepKeyValueStrBuilder::mapToKeyValueStr(param_map);
    if(VepStringUtils::IsStringNotblank(pairs)){
        ultimate_url = url + "?" + pairs;
    } else{
        ultimate_url = url;
    }
    return ultimate_url;
}

/**
 * Http Get请求
 * @param url
 * @param param_map      请求参数map传入
 * @param http_timeout
 * @return
 */
std::string VepHttpInvoker::doHttpGet(const std::string &url, std::map<std::string, std::string> param_map, int http_timeout){
    std::string ultimate_url = buildUltimateUrl(url, param_map);    //构造get请求地址

    HttpClient::Options options = GetCommonHttpOptions();
    HttpClient client;
    HttpResponse response;
    HttpClient::ErrorCode error;
    if(!client.Get(ultimate_url, options, &response, &error)){
        //---日志打印错误
        return "get连接失败";
    }

    return response.Body();  //返回值
}

/**
 * Post请求
 * @param url
 * @param payload
 * @param http_timeout
 * @return
 */
std::string VepHttpInvoker::doHttpPost(const std::string &url, const std::string &payload, int http_timeout){
    HttpClient::Options options = GetCommonHttpOptions();
    HttpClient client;
    HttpResponse response;
    HttpClient::ErrorCode error;
    if(!client.Post(url, payload, options, &response, &error)){
        return "";
    }

    return response.Body();
}

/**
 * 返回请求的状态码
 * @param url
 * @param payload
 * @param http_timeout
 * @return
 */
int VepHttpInvoker::doHttpPostStatus(const std::string &url, const std::string &payload, int http_timeout){
    HttpClient::Options options = GetCommonHttpOptions();
    HttpClient client;
    HttpResponse response;
    HttpClient::ErrorCode error;
    client.Post(url, payload, options, &response, &error);

    return (int)response.Status();  //返回状态
}

/**
 * Http header配置
 * @return
 */
HttpClient::Options VepHttpInvoker::GetCommonHttpOptions() {
    HttpClient::Options options;
    options.AddHeader("User-Agent", VepHttpUtils::user_agent());
    options.AddHeader("Content-Type", "application/json");
    options.AddHeader("Accept", "application/json");
    return options;
}
