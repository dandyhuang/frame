//
// Created by 吴婷 on 2019-12-03.
//
#ifndef COMMON_LIBVEP_UTILS_H_
#define COMMON_LIBVEP_UTILS_H_

#include <string>
#include <map>
#include <tuple>

#include "thirdparty/monitor_sdk_common/libvep/entity/layer_vo.h"
#include "thirdparty/monitor_sdk_common/libvep/entity/meta_Dto.h"
#include "thirdparty/jsoncpp/json.h"

namespace common{
/**
 * 解析参数集合
 * std::vector<std::tuple<std::string, std::string, std::string>> params;  //(key, value, type)
 */
class VepParamsUtils {
public:
    static std::string ToPrint(const std::vector<std::tuple<std::string, std::string, std::string>> &params);   //打印参数集合
    //将map参数加上类型标识，以tuple三元输出(key, value, type)
    static std::vector<std::tuple<std::string, std::string, std::string>> ParamMapToTuple(
            const std::map<std::string, std::string> &params, std::map<std::string, std::string> param_type);
};

class VepStringUtils {
public:
    static bool IsStringNotblank(const std::string &token);
    static bool IsBlank(const std::string &token);
    static bool AreStringsEqual(const std::string &a, const std::string &b);
    static std::string leftPad(const std::string &str, int length, const std::string &padding);  //补充位数
};


class VepJsonUtils {
public:
    static std::string ParseString(const Json::Value &root); //将json格式转换为纯字符串格式
    static std::string ParseStringUtf8(const Json::Value &root); //将json格式转换为utf8字符串格式
};


class VepHttpUtils{
public:
    static std::string user_agent();
};


class VepTimeUtils{
public:
    static long GetCurrentTimeMills();      //毫秒级
    static long GetCurrentTimeMicros();     //微秒级
};

class VepHashValGenerator{
public:
    static std::string getHashCodeVal(const std::string &str);     //md5解析+哈希
    static int hashCode(const std::string &str);            //哈希处理
};

/**
 * 客户端上报信息
 */
class VepClientInfoUtils{
public:
    static std::string getLocalIP();    //获取本地ip
};

class VepKeyValueStrBuilder{
public:
    static std::string mapToKeyValueStr(const std::map<std::string, std::string> &param_map);    //将参数值转换为k-v字符串，比如key1=val1&key2=val2&key3=val3
};

/**
 * 匹配分流规则
 */
class VepParseTrafficRuleUtils{
public:
    static std::string grabTrafficRule(const LabDto &lab); //实验分流规则
    static std::string grabTrafficRule(const VepApiLayerVo &layer);  //层分流规则
    static std::string parseTrafficType(const std::string &shunt_field, const std::string &shunt_tag_type);
};

} //namespace common

#endif //COMMON_LIBVEP_UTILS_H_