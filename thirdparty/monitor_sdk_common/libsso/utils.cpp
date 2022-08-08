//
// Created by 吴婷 on 2020-05-15.
//
#include <chrono>
#include <utility>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "utils.h"

#include "thirdparty/monitor_sdk_common/base/string/string_piece.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

using namespace common;
using namespace std::chrono;

long SSOTimeUtils::GetCurrentTimeMills(){
    milliseconds ms = duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
    );
    return long(ms.count());
}

long SSOTimeUtils::GetCurrentTimeMicros() {
    using namespace std::chrono;
    microseconds ms = duration_cast<microseconds>(
            system_clock::now().time_since_epoch()
    );
    return long(ms.count());
}

bool SSOParseUtils::ParseSSOResponse(const std::string& json_str, std::shared_ptr<SSOResponse> ret){
    // 创建解析对象
    rapidjson::Document doc;
    // 解析失败直接退出
    if (doc.Parse(json_str.data()).HasParseError()) {
        return false;
    }
    // 解析整数
    if(doc.HasMember("code") && doc["code"].IsInt()){
        ret->code = doc["code"].GetInt();
        //std::cout << ret->code << std::endl;
    }
    // 解析字符串
    if(doc.HasMember("msg") && doc["msg"].IsString()){
        ret->msg = doc["msg"].GetString();
    }
    if(doc.HasMember("redirectUrl") && doc["redirectUrl"].IsString()){
        ret->redirect_url = doc["redirectUrl"].GetString();
    }
    // 解析Object
    if(doc.HasMember("data") && doc["data"].IsObject()){
        const rapidjson::Value& data = doc["data"];
        if(data.HasMember("userId") && data["userId"].IsString()){
            ret->data = data["userId"].GetString();    //只取其中的一个字段作为是否为空判断
        }
    }

    return true;
}

std::map<std::string, std::string> SSOParseUtils::ParseMap(const std::string& json_str){
    std::map<std::string, std::string> params;

    rapidjson::Document doc;
    if (doc.Parse(json_str.data()).HasParseError()) {
        return params;
    }

    for (rapidjson::Value::ConstMemberIterator iter = doc.MemberBegin(); iter != doc.MemberEnd(); ++iter) {
        std::string name = (iter->name).GetString();
        const rapidjson::Value& value = iter->value;
        if(value.IsString()){
            params[name] = value.GetString();
        } else if (value.IsInt()){
            params[name] = value.GetInt();
        } else if (value.IsBool()){
            params[name] = value.GetBool();
        }
    }

    return params;
}

std::string HttpUtils::GetIpAddress(std::map<std::string, std::string> request_headers){
    //1. X-Forwarded-For
    std::string xFor;
    auto iter = request_headers.find("X-Forwarded-For");
    if(iter != request_headers.end()){  //找到
        xFor = iter->second;
        if("unKnown" != xFor){
            // 多次反向代理，取第一个ip
            if(xFor.npos == xFor.find(',')){ //没有找到
                return xFor;
            } else {    //取第一个
                int index = xFor.find(',');
                return xFor.substr(0, index);
            }
        }
    }
    //2. X-Real-IP
    std::string xIp;
    iter = request_headers.find("X-Real-IP");
    if(iter != request_headers.end()){  //找到
        xIp = iter->second;
    }
    if(!xIp.empty() && "unKnown" != xIp){
        return xIp;
    }
    // Proxy-Client-IP
    if(xIp.empty() || "unKnown" == xIp){
        iter = request_headers.find("Proxy-Client-IP");
        if(iter != request_headers.end()) {  //找到
            xIp = iter->second;
        }
    }
    // WL-Proxy-Client-IP
    if(xIp.empty() || "unKnown" == xIp){
        iter = request_headers.find("WL-Proxy-Client-IP");
        if(iter != request_headers.end()) {  //找到
            xIp = iter->second;
        }
    }
    // HTTP_CLIENT_IP
    if(xIp.empty() || "unKnown" == xIp){
        iter = request_headers.find("HTTP_CLIENT_IP");
        if(iter != request_headers.end()) {  //找到
            xIp = iter->second;
        }
    }
    // HTTP_X_FORWARDED_FOR
    if(xIp.empty() || "unKnown" == xIp){
        iter = request_headers.find("HTTP_X_FORWARDED_FOR");
        if(iter != request_headers.end()) {  //找到
            xIp = iter->second;
        }
    }
    // 直接读取本机ip
    if(xIp.empty() || "unKnown" == xIp){
        int inet_sock;
        struct ifreq ifr;
        char ip[32];

        inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
        strcpy(ifr.ifr_name, "eth0");
        ioctl(inet_sock, SIOCGIFADDR, &ifr);
        strcpy(ip, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
        xIp = std::string(ip);
    }

    return xIp;
}
