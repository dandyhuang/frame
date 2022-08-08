//
// Created by 吴婷 on 2020-06-17.
//

#ifndef COMMON_LIBVNS_JSON_UTILS_H
#define COMMON_LIBVNS_JSON_UTILS_H

#include <map>
#include <string>
#include <memory>
#include <iostream>

#include "utils/utilandcoms.h"
#include "object/instance.h"
#include "object/service_info.h"
#include "object/token_info.h"
#include "beat/beat_info.h"
#include "core/push_receiver.h"

/**
 * json解析
 */
class JsonUtils {
public:
    static std::string ToJSONString(std::map<std::string, std::string> params);
    static std::string ToJSONString(BeatInfo &beat_info);
    static std::string ToJSONString(Instance &instance);
    static std::string ToJSONString(ServiceInfo &service);
    static std::string ToJSONString(common::PushPacket &push_packet);
    static std::string ToJSONString(TokenInfo &token_info);
    static std::string ToJSONString(ResponseError &response);

    static bool JsonStr2TokenInfo(const std::string &json_str, std::shared_ptr<TokenInfo> ret);
    static bool JsonStr2ResponseError(const std::string &json_str, std::shared_ptr<ResponseError> ret);
    static bool JsonStr2Instance(const std::string &json_str, std::shared_ptr<Instance> ret);
    static bool JsonStr2ServiceInfo(const std::string &json_str, std::shared_ptr<ServiceInfo> ret);
    static bool JsonStr2PushPacket(const std::string &json_str, std::shared_ptr<common::PushPacket> ret);
    static bool JsonStr2ResultVo(const std::string &json_str, std::shared_ptr<ResultVo> ret);
    static bool JsonStr2BeatResult(const std::string &json_str, std::shared_ptr<BeatResult> ret);

};


#endif //COMMON_LIBVNS_JSON_UTILS_H
