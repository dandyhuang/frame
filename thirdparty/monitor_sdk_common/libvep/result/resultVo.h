//
// Created by 吴婷 on 2019-11-22.
//

#ifndef COMMON_LIBVEP_MODEL_RESULTVO_H
#define COMMON_LIBVEP_MODEL_RESULTVO_H

#include <string>

#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/libvep/config/constants.h"
#include "thirdparty/monitor_sdk_common/libvep/util/utils.h"

#include "thirdparty/jsoncpp/json.h"

namespace common {

template<class T>
class ResultVo {
public:
    ResultVo(){}
    ~ResultVo(){}

    ResultVo(const std::string &code, const std::string &msg, bool success){
        this->code = code;
        this->msg = msg;
        this->success = success;
    }

    explicit ResultVo(T data){
        this->code = IntegerToString(ResultConstants::SUCCESS.first);
        this->msg = ResultConstants::SUCCESS.second;
        this->success = true;
        this->data = data;
    }

    const std::string ToString() const{
        Json::Value root;
        root["success"] = success;
        root["code"] = code;
        root["msg"] = msg;

        return VepJsonUtils::ParseStringUtf8(root);
    }

public:
    bool success;      //读取是否成功
    std::string code; //返回码
    std::string msg;  //返回文本消息
    T data;           //业务数据
};

}//namespace common

#endif //COMMON_LIBVEP_MODEL_RESULTVO_H
