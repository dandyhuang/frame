//
// Created by 吴婷 on 2019-12-03.
//
#include <chrono>
#include <utility>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "utils.h"
#include "thirdparty/monitor_sdk_common/crypto/hash/md5.h"
#include "thirdparty/monitor_sdk_common/base/string/concat.h"
#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/base/string/format/print.h"
#include "thirdparty/monitor_sdk_common/base/string/string_piece.h"
#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/net/uri/uri.h"
#include "thirdparty/monitor_sdk_common/libvep/config/constants.h"

using namespace common;

/**
 * 将参数值转换为k-v字符串，比如key1=val1&key2=val2&key3=val3
 * @param param_map
 * @return
 */
std::string VepKeyValueStrBuilder::mapToKeyValueStr(const std::map<std::string, std::string> &param_map) {
    if(param_map.empty() || param_map.size() == 0){
        return "";
    }
    std::string content;
    for (auto iter = param_map.begin(); iter != param_map.end(); ++iter) {
        std::string keyValue = iter->first +  "=" + URI::EncodeComponent(StringTrim(iter->second)); //等号连接
        //第一个前面不连&
        if(iter == param_map.begin()){
            content = content + keyValue;
        } else {
            content = content + "&" + keyValue;
        }
    }
    return content;
}

/**
 * 打印参数集合
 * @param params
 * @return
 */
std::string VepParamsUtils::ToPrint(const std::vector<std::tuple<std::string, std::string, std::string>> &params) {
    Json::Value root;
    for (auto &param : params) {
        //定义接收变量
        std::string key;
        std::string value;
        std::string type;
        std::tie(key, value, type) = param;
        std::string pair = StringConcat("value: ", value, ", type: ", type);
        root[key] = pair;
    }
    return root.toStyledString();
}

/**
 * 将map参数加上类型标识，以tuple三元输出(key, value, type)
 * @param params
 * @param param_type
 * @return
 */
std::vector<std::tuple<std::string, std::string, std::string>> VepParamsUtils::ParamMapToTuple(const std::map<std::string, std::string> &params, std::map<std::string, std::string> param_type) {
    std::vector<std::tuple<std::string, std::string, std::string>> param_tuple;
    //判断参数个数与类型个数是否匹配
    if(params.size() != param_type.size()){
        return param_tuple;
    }
    for (auto param : params){
        param_tuple.push_back(std::make_tuple(param.first, param.second, param_type[param.first]));
    }
    return param_tuple;
}


bool VepStringUtils::IsStringNotblank(const std::string &token) {
    return !token.empty();
}

bool VepStringUtils::IsBlank(const std::string &token) {
    return token.empty();
}

bool VepStringUtils::AreStringsEqual(const std::string &a, const std::string &b) {
    return a.compare(b) == 0;
}

/**
 * 取最右边的3位，不足3位就补充位数
 * @param str
 * @param length 指定截取的长度
 * @param padding 补足的单个字符串
 * @return
 */
std::string VepStringUtils::leftPad(const std::string &str, int length, const std::string &padding) {
    int str_length = str.size();
    if(str_length == length){
        return str;
    }else if(str_length > length){
        StringPiece strP(str);                          //用StringPiece的方法进行截取
        return strP.substr(str_length-length).as_string();   //取最右边的3位
    }
    //不足3位进行补全
    std::string ret = str;
    for (int i = 0; i < length-str_length; ++i) {
        ret = StringConcat(padding, ret);
    }
    return ret;
}

std::string VepHttpUtils::user_agent() {
    return StringPrint("%s/%s", ConfigConstants::CLIENT_CONFIG_APP_NAME, ConfigConstants::BURIED_LOG_VERSION);   //app_name/version
}

long VepTimeUtils::GetCurrentTimeMills() {
    using namespace std::chrono;
    milliseconds ms = duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
    );
    return long(ms.count());
}

long VepTimeUtils::GetCurrentTimeMicros() {
    using namespace std::chrono;
    microseconds ms = duration_cast<microseconds>(
            system_clock::now().time_since_epoch()
    );
    return long(ms.count());
}

/**
 * md5+哈希处理
 * @param str
 * @return
 */
std::string VepHashValGenerator::getHashCodeVal(const std::string &str) {
    //1.md5
    MD5 md5;
    md5.Update(str);
    std::string md5Val = md5.HexFinal();
    //2.hashCode
    int hashCode = VepHashValGenerator::hashCode(md5Val);
    return IntegerToString(hashCode);
}

/**
 * 哈希处理
 * @param str
 * @return
 */
int VepHashValGenerator::hashCode(const std::string &str) {
    int h = 0;
    for (size_t i = 0; i < str.size(); ++i){
        h = h * 31 + static_cast<int>(str[i]);
    }
    return h;
}

/**
 * 客户端上报信息：获取本地ip
 */
std::string VepClientInfoUtils::getLocalIP() {
    int inet_sock;
    struct ifreq ifr;
    char ip[32];

    inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
    strcpy(ifr.ifr_name, "eth0");
    ioctl(inet_sock, SIOCGIFADDR, &ifr);
    strcpy(ip, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    return std::string(ip);
}

std::string VepJsonUtils::ParseString(const Json::Value &root) {
    //纯字符输出
    Json::StreamWriterBuilder builder;
    return Json::writeString(builder, root);
}

/**
 * 将json格式转换为utf8字符串格式
 * @param root
 * @return
 */
std::string VepJsonUtils::ParseStringUtf8(const Json::Value &root) {
    Json::StreamWriterBuilder builder;
    builder.settings_["emitUTF8"] = true;
    return Json::writeString(builder, root);
}

/**
 * 实验分流规则
 * @param lab
 * @return
 */
std::string VepParseTrafficRuleUtils::grabTrafficRule(const LabDto &lab) {
    std::string traffic_allocation_type = lab.traffic_allocation_type;
    if(TrafficAllocationTypeEnum::PROPORTION == traffic_allocation_type){
        traffic_allocation_type = TrafficAllocationTypeEnum::SEGMENT;
    }
    return traffic_allocation_type;
}

/**
 * 层分流规则
 * @param layer
 * @return
 */
std::string VepParseTrafficRuleUtils::grabTrafficRule(const VepApiLayerVo &layer) {
    std::string shunt_field = layer.shunt_field;
    std::string shunt_tag_type = layer.shunt_tag_type;
    return VepParseTrafficRuleUtils::parseTrafficType(shunt_field, shunt_tag_type);
}

/**
 * 根据传入参数，判断分流方式
 * @param shunt_field
 * @param shunt_tag_type
 * @return
 */
std::string VepParseTrafficRuleUtils::parseTrafficType(const std::string &shunt_field, const std::string &shunt_tag_type) {
    if(!VepStringUtils::IsStringNotblank(shunt_field)){
        return TrafficAllocationTypeEnum::TAG;          //号段为空，就圈定字段分流
    }
    if(!VepStringUtils::IsStringNotblank(shunt_tag_type)){
        return TrafficAllocationTypeEnum::SEGMENT;      //圈定字段为空，就号段分流
    }
    //同时存在
    return TrafficAllocationTypeEnum::TAG_WITH_SEGMENT;
}