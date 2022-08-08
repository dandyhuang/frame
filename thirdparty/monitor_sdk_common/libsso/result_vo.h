//
// Created by 吴婷 on 2020-05-14.
//

#ifndef COMMON_LIBSSO_RESULT_VO_H
#define COMMON_LIBSSO_RESULT_VO_H

#include <iostream>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace common {
/**
 * 结果类
 */
class SSOResult {
public:
    const std::string ToString() const {
        rapidjson::StringBuffer str_buf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(str_buf);
        writer.StartObject();

        writer.Key("success");
        writer.Bool(success);
        writer.Key("code");
        writer.Int(code);
        writer.Key("msg");
        writer.String(msg.data());
        writer.Key("redirectUrl");
        writer.String(redirect_url.data());

        writer.EndObject();
        return str_buf.GetString();
    }

public:
    bool success;       // 验证是否成功
    int code;           // 返回码
    std::string msg;
    std::string redirect_url;   //重定向页面
};

}

#endif //COMMON_LIBSSO_RESULT_VO_H
