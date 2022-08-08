// Authors: sg@dandy.com

#include "dag/util.h"
#include <time.h>
#include "common/loghelper.h"
#include "common/concurrent_map.h"
namespace Rec {
namespace Util {
using namespace vlog;
int64_t get_cur_time_us() {
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    return tp.tv_sec * 1000000 + tp.tv_nsec / 1000;
}
std::shared_ptr<google::protobuf::Message> createMessage(const std::string& typeName) {
    google::protobuf::Message* message = nullptr;
    const google::protobuf::Descriptor* descriptor =
        google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
    if (!descriptor) {
        VLOG_APP(ERROR) << "createMessage failed, message name: " << typeName;
    } else {
        const google::protobuf::Message* prototype = 
            google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
        if (prototype) {
            message = prototype->New();
        } else {
            VLOG_APP(ERROR) << "createMessage failed, message name: " << typeName;
        }
    }
    return std::shared_ptr<google::protobuf::Message>(message);
}
std::string message_to_json(const google::protobuf::Message& message) {
    std::string json_string;
    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    options.always_print_primitive_fields = true;
    options.preserve_proto_field_names = true;
    auto status = google::protobuf::util::MessageToJsonString(message, &json_string, options);
    return json_string;
}

} // Util
} // Rec
