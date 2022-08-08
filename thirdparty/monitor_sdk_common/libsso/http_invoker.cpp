//
// Created by 吴婷 on 2020-05-14.
//

#include "http_invoker.h"

using namespace common;

std::string SSOHttpInvoker::doHttpPost(const std::string &url, const std::string &payload){
    HttpClient::Options options = GetCommonHttpOptions();
    HttpClient client;
    HttpResponse response;
    HttpClient::ErrorCode error;
    if(!client.Post(url, payload, options, &response, &error)){
        return "";
    }

    return response.Body();
}

int SSOHttpInvoker::doHttpPostStatus(const std::string &url, const std::string &payload){
    HttpClient::Options options = GetCommonHttpOptions();
    HttpClient client;
    HttpResponse response;
    HttpClient::ErrorCode error;
    client.Post(url, payload, options, &response, &error);

    return (int)response.Status();  //返回状态
}

HttpClient::Options SSOHttpInvoker::GetCommonHttpOptions() {
    HttpClient::Options options;
    options.AddHeader("User-Agent", "sso-sdk");
    options.AddHeader("Content-Type", "application/json");
    options.AddHeader("Accept", "application/json");
    return options;
}
