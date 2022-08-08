//
// Created by 吴婷 on 2020-06-17.
//

#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"

#include "utils/json_utils.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

std::string JsonUtils::ToJSONString(std::map<std::string, std::string> params){
    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    writer.StartObject();

    std::map<std::string, std::string>::iterator iter;
    for (iter = params.begin(); iter != params.end(); iter++) {
        writer.Key(iter->first.data());
        writer.String(iter->second.data());
    }
    writer.EndObject();
    return strbuf.GetString();
}

std::string JsonUtils::ToJSONString(BeatInfo &beat_info){
    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    writer.StartObject();

    // 字符型
    writer.Key("ip");
    writer.String(beat_info.ip.data());
    writer.Key("serviceName");
    writer.String(beat_info.service_name.data());
    writer.Key("cluster");
    writer.String(beat_info.cluster_name.data());
    // 整型
    writer.Key("port");
    writer.Int(beat_info.port);
    writer.Key("period");
    writer.Int(beat_info.period);
    // 浮点型
    writer.Key("weight");
    writer.Double(beat_info.weight);
    // 布尔型
    writer.Key("scheduled");
    writer.Bool(beat_info.scheduled);
    writer.Key("stopped");
    writer.Bool(beat_info.stopped);
    // 结构体
    writer.Key("metadata");
    writer.StartObject();
    std::map<std::string, std::string>::iterator iter;
    for (iter = beat_info.metadata.begin(); iter != beat_info.metadata.end(); iter++) {
        writer.Key(iter->first.data());
        writer.String(iter->second.data());
    }
    writer.EndObject();

    writer.EndObject();
    return strbuf.GetString();
}

std::string JsonUtils::ToJSONString(Instance &instance){
    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    writer.StartObject();
    // 字符型
    writer.Key("instanceId");
    writer.String(instance.instance_id.data());
    writer.Key("ip");
    writer.String(instance.ip.data());
    writer.Key("clusterName");
    writer.String(instance.cluster_name.data());
    writer.Key("serviceName");
    writer.String(instance.service_name.data());
    // 整型
    writer.Key("port");
    writer.Int(instance.port);
    // 浮点型
    writer.Key("weight");
    writer.Double(instance.weight);
    // 布尔型
    writer.Key("healthy");
    writer.Bool(instance.healthy);
    writer.Key("enabled");
    writer.Bool(instance.enabled);
    writer.Key("ephemeral");
    writer.Bool(instance.ephemeral);
    // 结构体
    writer.Key("metadata");
    writer.StartObject();
    std::map<std::string, std::string>::iterator iter;
    for (iter = instance.metadata.begin(); iter != instance.metadata.end(); iter++) {
        writer.Key(iter->first.data());
        writer.String(iter->second.data());
    }
    writer.EndObject();

    writer.EndObject();
    return strbuf.GetString();
}

std::string JsonUtils::ToJSONString(ServiceInfo &service){
    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    writer.StartObject();
    // 字符型
    writer.Key("name");
    writer.String(service.name.data());
    writer.Key("groupName");
    writer.String(service.group_name.data());
    writer.Key("clusters");
    writer.String(service.clusters.data());
    writer.Key("checksum");
    writer.String(service.checksum.data());
    // 整型
    writer.Key("cacheMills");
    writer.Int(service.cache_millis);
    writer.Key("lastRefTime");
    writer.Int(service.last_ref_time);
    // 布尔型
    writer.Key("allIps");
    writer.Bool(service.all_ips);
    // 数组
    writer.Key("hosts");
    writer.StartArray();
    for (int i=0; i<(int)service.hosts.size(); i++) {
        writer.String(service.hosts[i].toString().data());
    }
    writer.EndArray();

    writer.EndObject();
    return strbuf.GetString();
}

std::string JsonUtils::ToJSONString(common::PushPacket &push_packet){
    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    writer.StartObject();
    // 字符型
    writer.Key("type");
    writer.String(push_packet.type.data());
    writer.Key("data");
    writer.String(push_packet.data.data());
    // 整型
    writer.Key("lastRefTime");
    writer.Int(push_packet.last_ref_time);

    writer.EndObject();
    return strbuf.GetString();
}

std::string JsonUtils::ToJSONString(TokenInfo &token_info){
    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    writer.StartObject();
    // 整型
    writer.Key("retcode");
    writer.Int(token_info.retcode);
    // 字符型
    writer.Key("message");
    writer.String(token_info.message.data());
    // map
    writer.Key("data");
    writer.StartObject();
    std::map<std::string, std::string>::iterator iter;
    for (iter = token_info.data.begin(); iter != token_info.data.end(); iter++) {
        writer.Key(iter->first.data());
        writer.String(iter->second.data());
    }
    writer.EndObject();

    writer.EndObject();
    return strbuf.GetString();
}

std::string JsonUtils::ToJSONString(ResponseError &response){
    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    writer.StartObject();
    // 整型
    writer.Key("status");
    writer.Int(response.status);
    // 字符型
    writer.Key("timestamp");
    writer.String(response.timestamp.data());
    writer.Key("error");
    writer.String(response.error.data());
    writer.Key("message");
    writer.String(response.message.data());
    writer.Key("path");
    writer.String(response.path.data());

    writer.EndObject();
    return strbuf.GetString();
}

///////////////////////////////////
bool JsonUtils::JsonStr2TokenInfo(const std::string &json_str, std::shared_ptr<TokenInfo> ret){
    // 创建解析对象
    rapidjson::Document doc;
    // 解析失败直接退出
    if (doc.Parse(json_str.data()).HasParseError()) {
        return false;
    }
    // 解析整数
    if(doc.HasMember("retcode") && doc["retcode"].IsInt()){
        ret->retcode = doc["retcode"].GetInt();
    }
    // 解析字符串
    if(doc.HasMember("message") && doc["message"].IsString()){
        ret->message = doc["message"].GetString();
    }
    // 解析结构体
    if(doc.HasMember("data") && doc["data"].IsObject()){
        const rapidjson::Value& data = doc["data"];
        if(data.HasMember("accessToken") && data["accessToken"].IsString()){
            ret->data["accessToken"] = data["accessToken"].GetString();
        }
        if(data.HasMember("refreshToken") && data["refreshToken"].IsString()){
            ret->data["refreshToken"] = data["refreshToken"].GetString();
        }
    }
    return true;
}

bool JsonUtils::JsonStr2ResponseError(const std::string &json_str, std::shared_ptr<ResponseError> ret){
    // 创建解析对象
    rapidjson::Document doc;
    // 解析失败直接退出
    if (doc.Parse(json_str.data()).HasParseError()) {
        return false;
    }
    // 解析整数
    if(doc.HasMember("status") && doc["status"].IsInt()){
        ret->status = doc["status"].GetInt();
    }
    // 解析字符串
    if(doc.HasMember("timestamp") && doc["timestamp"].IsString()){
        ret->timestamp = doc["timestamp"].GetString();
    }
    if(doc.HasMember("error") && doc["error"].IsString()){
        ret->error = doc["error"].GetString();
    }
    if(doc.HasMember("message") && doc["message"].IsString()){
        ret->message = doc["message"].GetString();
    }
    if(doc.HasMember("path") && doc["path"].IsString()){
        ret->path = doc["path"].GetString();
    }

    return true;
}

bool JsonUtils::JsonStr2Instance(const std::string &json_str, std::shared_ptr<Instance> ret) {
    // 创建解析对象
    rapidjson::Document doc;
    // 解析失败直接退出
    if (doc.Parse(json_str.data()).HasParseError()) {
        return false;
    }

    // 解析整数
    if(doc.HasMember("port") && doc["port"].IsInt()){
        ret->port = doc["port"].GetInt();
    }
    // 解析浮点型
    if(doc.HasMember("weight") && doc["weight"].IsDouble()){
        ret->weight = doc["weight"].GetDouble();
    }
    // 解析布尔型
    if(doc.HasMember("healthy") && doc["healthy"].IsBool()){
        ret->healthy = doc["healthy"].GetBool();
    }
    if(doc.HasMember("enabled") && doc["enabled"].IsBool()){
        ret->enabled = doc["enabled"].GetBool();
    }
    if(doc.HasMember("ephemeral") && doc["ephemeral"].IsBool()){
        ret->ephemeral = doc["ephemeral"].GetBool();
    }
    // 解析字符串
    if(doc.HasMember("instanceId") && doc["instanceId"].IsString()){
        ret->instance_id = doc["instanceId"].GetString();
    }
    if(doc.HasMember("ip") && doc["ip"].IsString()){
        ret->ip = doc["ip"].GetString();
    }
    if(doc.HasMember("clusterName") && doc["clusterName"].IsString()){
        ret->cluster_name = doc["clusterName"].GetString();
    }
    if(doc.HasMember("serviceName") && doc["serviceName"].IsString()){
        ret->service_name = doc["serviceName"].GetString();
    }
    // 解析结构体
    if(doc.HasMember("metadata") && doc["metadata"].IsObject()){
        const rapidjson::Value& data = doc["metadata"];
        rapidjson::Value::ConstMemberIterator iter;
        for (iter = data.MemberBegin(); iter != data.MemberEnd(); ++iter) {
            std::string name = (iter->name).GetString();
            std::string value = (iter->value).GetString();
            ret->metadata[name] = value;
        }
    }

    return true;
}

bool JsonUtils::JsonStr2ServiceInfo(const std::string &json_str, std::shared_ptr<ServiceInfo> ret){
    // 创建解析对象
    rapidjson::Document doc;
    // 解析失败直接退出
    if (doc.Parse(json_str.data()).HasParseError()) {
        return false;
    }

    // 必须以{开头，避免只有一个字符的文件
    if(!common::StringStartsWith(json_str, "{")){
        return false;
    }

    // 解析整数
    if(doc.HasMember("cacheMills") && doc["cacheMills"].IsInt()){
        ret->cache_millis = doc["cacheMills"].GetInt();
    }
    if(doc.HasMember("lastRefTime") && doc["lastRefTime"].IsInt()){
        ret->last_ref_time = doc["lastRefTime"].GetInt();
    }
    // 解析字符串
    if(doc.HasMember("name") && doc["name"].IsString()){
        ret->name = doc["name"].GetString();
    }
    if(doc.HasMember("groupName") && doc["groupName"].IsString()){
        ret->group_name = doc["groupName"].GetString();
    }
    if(doc.HasMember("clusters") && doc["clusters"].IsString()){
        ret->clusters = doc["clusters"].GetString();
    }
    if(doc.HasMember("checksum") && doc["checksum"].IsString()){
        ret->checksum = doc["checksum"].GetString();
    }
    // 解析布尔型
    if(doc.HasMember("allIps") && doc["allIps"].IsBool()){
        ret->all_ips = doc["allIps"].GetBool();
    }
    // 解析数组
    if(doc.HasMember("hosts") && doc["hosts"].IsArray()){
        const rapidjson::Value& data = doc["hosts"];
        size_t len = data.Size();
        for (size_t i=0; i<len; i++) {
            //依次解析Instance
            // TODO 解析Instance代码复用
            const rapidjson::Value& doc_ins = data[i];
            std::shared_ptr<Instance> instance = std::make_shared<Instance>();
            // 解析整数
            if(doc_ins.HasMember("port") && doc_ins["port"].IsInt()){
                instance->port = doc_ins["port"].GetInt();
            }
            // 解析浮点型
            if(doc_ins.HasMember("weight") && doc_ins["weight"].IsDouble()){
                instance->weight = doc_ins["weight"].GetDouble();
            }
            // 解析布尔型
            if(doc_ins.HasMember("healthy") && doc_ins["healthy"].IsBool()){
                instance->healthy = doc_ins["healthy"].GetBool();
            }
            if(doc_ins.HasMember("enabled") && doc_ins["enabled"].IsBool()){
                instance->enabled = doc_ins["enabled"].GetBool();
            }
            if(doc_ins.HasMember("ephemeral") && doc_ins["ephemeral"].IsBool()){
                instance->ephemeral = doc_ins["ephemeral"].GetBool();
            }
            // 解析字符串
            if(doc_ins.HasMember("instanceId") && doc_ins["instanceId"].IsString()){
                instance->instance_id = doc_ins["instanceId"].GetString();
            }
            if(doc_ins.HasMember("ip") && doc_ins["ip"].IsString()){
                instance->ip = doc_ins["ip"].GetString();
            }
            if(doc_ins.HasMember("clusterName") && doc_ins["clusterName"].IsString()){
                instance->cluster_name = doc_ins["clusterName"].GetString();
            }
            if(doc_ins.HasMember("serviceName") && doc_ins["serviceName"].IsString()){
                instance->service_name = doc_ins["serviceName"].GetString();
            }
            // 解析结构体
            if(doc_ins.HasMember("metadata") && doc_ins["metadata"].IsObject()){
                const rapidjson::Value& data = doc_ins["metadata"];
                rapidjson::Value::ConstMemberIterator iter;
                for (iter = data.MemberBegin(); iter != data.MemberEnd(); ++iter) {
                    std::string name = (iter->name).GetString();
                    std::string value = (iter->value).GetString();
                    instance->metadata[name] = value;
                }
            }

            ret->hosts.push_back(*instance);

        }
    }

    return true;
}

bool JsonUtils::JsonStr2PushPacket(const std::string &json_str, std::shared_ptr<common::PushPacket> ret){
    // 创建解析对象
    rapidjson::Document doc;
    // 解析失败直接退出
    if (doc.Parse(json_str.data()).HasParseError()) {
        return false;
    }

    // 解析整数
    if(doc.HasMember("lastRefTime") && doc["lastRefTime"].IsInt()){
        ret->last_ref_time = doc["lastRefTime"].GetInt();
    }
    // 解析字符串
    if(doc.HasMember("type") && doc["type"].IsString()){
        ret->type = doc["type"].GetString();
    }
    if(doc.HasMember("data") && doc["data"].IsString()){
        ret->data = doc["data"].GetString();
    }

    return true;
}

bool JsonUtils::JsonStr2ResultVo(const std::string &json_str, std::shared_ptr<ResultVo> ret){
    // 创建解析对象
    rapidjson::Document doc;
    // 解析失败直接退出
    if (doc.Parse(json_str.data()).HasParseError()) {
        return false;
    }
    // 解析整数
    if(doc.HasMember("Code") && doc["Code"].IsInt()){
        ret->code = doc["Code"].GetInt();
    }
    // 解析字符串
    if(doc.HasMember("Message") && doc["Message"].IsString()){
        ret->message = doc["Message"].GetString();
    }

    return true;
}

bool JsonUtils::JsonStr2BeatResult(const std::string &json_str, std::shared_ptr<BeatResult> ret){
    // 创建解析对象
    rapidjson::Document doc;
    // 解析失败直接退出
    if (doc.Parse(json_str.data()).HasParseError()) {
        return false;
    }
    // 解析整数
    if(doc.HasMember("clientBeatInterval") && doc["clientBeatInterval"].IsInt()){
        ret->client_beat_interval = doc["clientBeatInterval"].GetInt();
    }
    return true;
}