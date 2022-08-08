//
// Created by 吴婷 on 2020-05-14.
//

#include "config_params.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

using namespace common;

std::string SSOProperties::SSODOMAIN = "http://privilege.test.vivo.xyz:8080";

std::string TokenType::TOKEN = "token";
std::string TokenType::MTOKEN = "mtoken";


std::string TokenLocation::URL = "url";
std::string TokenLocation::COOKIE = "cookie";
std::string TokenLocation::UNKNOWN = "unknown";

std::string ResultMsg::SUCCESS = "校验成功通过";
std::string ResultMsg::PARAMFAIL = "参数校验失败";
std::string ResultMsg::REDIRECT = "需要重定向";

std::string LoginParam::ToString() {
    rapidjson::StringBuffer str_buf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(str_buf);
    writer.StartObject();

    writer.Key("location");
    writer.String(location.data());
    writer.Key("url");
    writer.String(url.data());
    writer.Key("type");
    writer.String(type.data());
    writer.Key("token");
    writer.String(token.data());
    writer.Key("ip");
    writer.String(ip.data());
    writer.Key("appCode");
    writer.String(app_code.data());
    writer.Key("dateTime");
    writer.String(date_time.data());

    writer.EndObject();
    return str_buf.GetString();
}

std::string SSORequest::ToString() {
    rapidjson::StringBuffer str_buf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(str_buf);
    writer.StartObject();

    writer.Key("Sign");
    writer.String(sign.data());
    writer.Key("AppCode");
    writer.String(app_code.data());
    writer.Key("DateTime");
    writer.String(date_time.data());

    writer.EndObject();
    return str_buf.GetString();
}