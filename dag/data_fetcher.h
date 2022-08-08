#pragma once
#include "client/redis_client_manager.h"
#include "google/protobuf/message.h"
#include "compute_server.pb.h"
#include <memory>
#include "dag/dict.h"
#include <map>

namespace data_fetcher {
// 单例
class DataFetcher {
private:
    DataFetcher() {
    }
public:
    typedef std::function<void(void)> ParseFunc;
    static DataFetcher* instance() {
        static DataFetcher inst;
        return &inst;
    }
    void get_item_aync(const std::vector<std::string>& keys, int start, int end,
        std::shared_ptr<Rec::Dict::DictResult> result, std::function<void(std::shared_ptr<fs::interface::FsResponse>)> computeCb,
        const std::string& channel_name);
private:
    void repeated_field_parse(std::shared_ptr<brpc::RedisResponse> response,
        google::protobuf::Message* target_message,
        const google::protobuf::Reflection* target_reflection,
        const google::protobuf::FieldDescriptor* target_field_dec,
        std::shared_ptr<const FetchInfo> fetch_info);
    void message_parse(std::shared_ptr<brpc::RedisResponse> response, google::protobuf::Message* message,
        std::shared_ptr<const FetchInfo> fetch_info);

    // repeated_field parser
    // std::map<std::string, std::function<void(std::shared_ptr<RedisResponse>, std::shared_ptr<const FetchInfo>, vmic::compute::server::ComputeServerRequest* request)>>
    // repeated_field_parser_map;
};

} // end of namespace
