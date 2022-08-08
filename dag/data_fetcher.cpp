// #include "dag/data_fetcher.h"
// #include "butil/third_party/snappy/snappy.h"
// #include "dag/util.h"
// #include "dag/gtransport_manager.h"
// #include <memory>
// #include "dag/util.h"
// #include "common/concurrent_map.h"
// #include "common/loghelper.h"
// #include "dag/global.h"

// namespace data_fetcher {

// void DataFetcher::get_item_aync(const std::vector<std::string>& keys, int start, int end, std::shared_ptr<Rec::Dict::DictResult> result, std::function<void(std::shared_ptr<fs::interface::FsResponse>)> computeCb, const std::string& channel_name) {
//     auto channel = dag::GtransportManager::instance().get_transport(channel_name);
//     if (channel == nullptr) {
//         VLOG_APP(ERROR) << "channel_name: " << channel_name << " not found";
//         computeCb(std::shared_ptr<fs::interface::FsResponse>());
//         return;
//     }
//     auto cntl = std::make_shared<brpc::Controller>();
//     fs::interface::FsRequest request;
//     // request.set_user_id(imei);
//     for (size_t i = start; i < end; i++) {
//         request.add_ad_id(keys[i]);
//     }
//     // VLOG_APP(ERROR) << "send to fs size: " << request.ad_id_size();
//     auto response = std::make_shared<fs::interface::FsResponse>();
//     fs::interface::FsService_Stub stub(channel.get());
//     stub.getFeature(cntl.get(), &request, response.get(), NULL);
//     global::get_async_item_pool()->enqueue([cntl, response, result, computeCb, channel_name](){
//         if(cntl->Failed()) {
//             VLOG_APP(ERROR) << "param_name: "<< channel_name << ", cntl failed, remote_ip:" << cntl->remote_side()
//                          << " cost_us:" << cntl->latency_us() 
//                          << " error:"<< cntl->ErrorText();
//             response.reset();
//         } else {
//             // TERRORLOG << " item data back" << std::endl;
//         }
//         computeCb(response);
//     });
// }
// void DataFetcher::repeated_field_parse (std::shared_ptr<brpc::RedisResponse> response,
//     google::protobuf::Message* target_message,
//     const google::protobuf::Reflection* target_reflection,
//     const google::protobuf::FieldDescriptor* target_field_dec,
//     std::shared_ptr<const FetchInfo> fetch_info) {
//     if (response != nullptr) {
//         if (fetch_info->source_field_tags.size() == 0) {
//             // 这种情况是target_field_name 配置的是repeated_field类型，但source_field_tags没有解析到
//             // 配置文件配错
//             VLOG_APP(ERROR) << "error; source_field_tags is empty in repeated_field_parse, check fetcher.conf"
//            ;
//             return;
//         }

//         // 1. prepare message
//         auto source_message = Rec::Util::createMessage(fetch_info->source_message_name);
//         if (!source_message) {
//             VLOG_APP(ERROR) << "error; source_message_name: " << fetch_info->source_message_name << " is wrong"
//            ;
//             return;
//         }
//         // VLOG_APP(ERROR) << "get form redis " << fetch_info->param_name << " size:" << response->reply(0).size();

//         // 2. uncompress redis data
//         if (!fetch_info->compress.empty() && fetch_info->compress == "snappy") {
//             std::string data_str = "";
//             data_str.reserve(response->reply(0).data().size() * 6); // snappy compress ratio is 22%, so mutilpy by 6
//             auto ok = butil::snappy::Uncompress(response->reply(0).c_str(), response->reply(0).data().size(),
//                             &data_str);
//             if (!ok || !source_message->ParseFromArray(data_str.c_str(), data_str.size())) {
//                 VLOG_APP(ERROR) << "parse source message failed!, source field name: " << fetch_info->source_field_name <<
//                              " compress ok:" << ok;
//                 return;
//             }
//         } else {
//             if (!source_message->ParseFromArray(response->reply(0).c_str(), response->reply(0).data().size())) {
//                 VLOG_APP(ERROR) << "parse source message failed!, source field name: " << fetch_info->source_field_name ;
//                 return;
//             }
//         }
        
//         // 3.find repeated field  and fill data in message
//         google::protobuf::RepeatedPtrField< google::protobuf::Message > * repeated_field = nullptr;
//         google::protobuf::FieldDescriptor* field_dec = nullptr;
//         google::protobuf::Message* cur_message = source_message.get();
//         const google::protobuf::Reflection* reflection = nullptr;
//         for (auto tag : fetch_info->source_field_tags) {
//             const google::protobuf::Descriptor* descriptor = cur_message->GetDescriptor();
//             reflection = cur_message->GetReflection();
//             google::protobuf::FieldDescriptor* field = descriptor->FindFieldByNumber(tag);
//             if (field->is_repeated()) {
//                 // if repeated , then it should be the final tag
//                 repeated_field = reflection->MutableRepeatedPtrField<google::protobuf::Message>(cur_message, field);
//                 field_dec = field;
//                 break;
//             }
//             cur_message = reflection->MutableMessage(cur_message, field);
//         }
//         if(reflection == nullptr || field_dec == nullptr || !field_dec->is_repeated() || repeated_field == nullptr) {
//             VLOG_APP(ERROR) << "source_field_tags reference to a non-repeated field or field not exists, check fetcher.conf "
//             << "source_field_name: " << fetch_info->source_field_name;
//             return;
//         }
//         auto target_repeated_msg = target_reflection->MutableRepeatedPtrField<google::protobuf::Message>(target_message, target_field_dec);
//         target_repeated_msg->Swap(repeated_field);
//     }
// }
// void DataFetcher::message_parse(std::shared_ptr<brpc::RedisResponse> response, google::protobuf::Message* message,
//     std::shared_ptr<const FetchInfo> fetch_info) {
//     if (response != nullptr) {
//         // VLOG_APP(ERROR) << "get form redis " << fetch_info->param_name << " size:" << response->reply(0).size();
//         if (!fetch_info->compress.empty() && fetch_info->compress == "snappy") {
//             std::string data_str = "";
//             data_str.reserve(response->reply(0).data().size() * 6); // snappy compress ratio is 22%, so mutilpy by 6
//             auto ok = butil::snappy::Uncompress(response->reply(0).c_str(), response->reply(0).data().size(),
//                             &data_str);
//             if (ok && !message->ParseFromArray(data_str.c_str(), data_str.size())) {
//                 VLOG_APP(ERROR) << "parse message failed! message: [" <<  fetch_info->target_message_name << "] prefix: "
//                 << fetch_info->param_name << " compress ok: " << ok;
//             }
//         } else {
//             if (!message->ParseFromArray(response->reply(0).c_str(), response->reply(0).data().size())) {
//                 VLOG_APP(ERROR) << "parse message failed! message: [" <<  fetch_info->target_field_name << "] prefix: "
//                 << fetch_info->param_name << " size:" << response->reply(0).data().size();
//             }
//         }
//         /*
//         if (fetch_info->param_name == "rtrs_user_uninstall" ||
//             fetch_info->param_name == "rtrs_user_download_new" ||
//             fetch_info->param_name  == "rtrs_user_install_ex") {
//             VLOG_APP(ERROR) << "param_name: " << fetch_info->param_name;
//             std::string&& json = Rec::Util::message_to_json(*message);
//             VLOG_APP(ERROR) << "json: " << json.c_str();
//         }
//         */
//     }
// }

// } // end of namespace
