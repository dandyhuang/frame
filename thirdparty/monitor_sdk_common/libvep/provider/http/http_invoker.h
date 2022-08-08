//
// Created by 吴婷 on 2019-11-23.
//

#ifndef COMMON_LIBVEP_HTTPINVOKER_H
#define COMMON_LIBVEP_HTTPINVOKER_H

#include <map>

#include "thirdparty/monitor_sdk_common/net/uri/uri.h"
#include "thirdparty/monitor_sdk_common/net/http/client.h"

#include "thirdparty/monitor_sdk_common/libvep/request/vep_req_param.h"


namespace common {
/**
 * HTTP请求工具类
 */
class VepHttpInvoker {

public:
    /**
     * Http Get请求【map参数】
     * @param url           请求地址
     * @param param_map      请求参数map传入
     * @param http_timeout   超时时间限制
     * @return
     */
    std::string doHttpGet(const std::string &url, std::map<std::string, std::string> param_map, int http_timeout);

    /**
     * Http Post请求
     * @param url           请求地址
     * @param payload       请求主体
     * @param http_timeout   超时时间限制
     * @return json_str
     */
    std::string doHttpPost(const std::string &url, const std::string &payload, int http_timeout);

    /**
     * 返回请求的状态码
     * @param url
     * @param payload
     * @param http_timeout
     * @return 请求状态
     */
    int doHttpPostStatus(const std::string &url, const std::string &payload, int http_timeout);

    /**
     * 生成最终的请求url
     * @param url
     * @param param_map
     * @return
     */
    std::string buildUltimateUrl(const std::string &url, std::map<std::string, std::string> param_map);

private:
    HttpClient::Options GetCommonHttpOptions();     //Http header配置

};

} // namespace common

#endif //COMMON_LIBVEP_HTTPINVOKER_H
