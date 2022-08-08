//
// Created by 吴婷 on 2020-05-14.
//

#ifndef COMMON_LIBSSO_HTTPINVOKER_H
#define COMMON_LIBSSO_HTTPINVOKER_H

#include "thirdparty/monitor_sdk_common/net/uri/uri.h"
#include "thirdparty/monitor_sdk_common/net/http/client.h"

namespace common {
/**
 * HTTP请求工具类
 */
class SSOHttpInvoker {
public:
    /**
     * Http Post请求
     * @param url           请求地址
     * @param payload       请求主体
     * @return json_str
     */
    std::string doHttpPost(const std::string &url, const std::string &payload);

    /**
     * 返回请求的状态码
     * @param url
     * @param payload
     * @return 请求状态
     */
    int doHttpPostStatus(const std::string &url, const std::string &payload);

private:
    /**
     * Http header配置
     * @return 头信息
     */
    HttpClient::Options GetCommonHttpOptions();
};

} //namespace common

#endif //COMMON_LIBSSO_HTTPINVOKER_H
