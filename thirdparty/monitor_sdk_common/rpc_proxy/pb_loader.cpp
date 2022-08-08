#include "thirdparty/monitor_sdk_common/rpc_proxy/pb_loader.h"

#include <stdio.h>
#include <memory>
#include "google/protobuf/descriptor.h"
#include "google/protobuf/descriptor.pb.h"
#include "google/protobuf/dynamic_message.h"
#include "google/protobuf/compiler/importer.h"
#include "thirdparty/monitor_sdk_common/pb_json_converter/pb_json_converter.h"

using std::string;
using std::vector;
using google::protobuf::compiler::Importer;
using google::protobuf::compiler::DiskSourceTree;
using google::protobuf::Message;
using google::protobuf::Descriptor;
using google::protobuf::DynamicMessageFactory;
using google::protobuf::FileDescriptor;

class TestErrorCollector : public google::protobuf::compiler::MultiFileErrorCollector {
 public:
  TestErrorCollector() {}
  ~TestErrorCollector() {}

  const string &error_text() {return error_text_;}
  const string &warn_text() {return warn_text_;}

  void AddError(const string &filename, int line, int column, const string &message) {
    error_text_.append("[error:]filename:").append(filename).append(",")
               .append("line:").append(std::to_string(line)).append(",")
               .append("column:").append(std::to_string(column)).append(",")
               .append("message:").append(message).append("\n");
  }

  void AddWarning(const string &filename, int line, int column, const string &message) {
    warn_text_.append("[warn:]filename:").append(filename).append(",")
              .append("line:").append(std::to_string(line)).append(",")
              .append("column:").append(std::to_string(column)).append(",")
              .append("message:").append(message).append("\n");
  }

 private:
  string error_text_;
  string warn_text_;
};

PbLoader::~PbLoader() {
  if (importer_) {
    delete ((Importer *)importer_);  // NOLINT
    importer_ = nullptr;
  }
}

int PbLoader::Init(const string &proto_file, const vector<string> &path_list) {
  DiskSourceTree sourceTree;
  TestErrorCollector error_collector;
  // look up .proto file in current directory
  sourceTree.MapPath("", "./");
  for (size_t i = 0; i < path_list.size(); ++i) {
    sourceTree.MapPath("", path_list[i]);
  }
  Importer *importer = new Importer(&sourceTree, &error_collector);

  // runtime compile support.proto
  const FileDescriptor *fd = importer->Import(proto_file);
  if (fd == NULL) {
    printf("import file %s failed\n%s%s", proto_file.c_str(), error_collector.error_text().c_str(), error_collector.warn_text().c_str());
    return -1;
  }

  importer_ = importer;
  return 0;
}
int PbLoader::Json2PbBinary(const string &msg_name, const string &json_str, string &out) const {  // NOLINT
  if (!importer_) return -1;
  Importer *importer = (Importer *)importer_;
  const Descriptor *descriptor = importer->pool()->FindMessageTypeByName(msg_name);
  if (!descriptor) {
    printf("error get message: %s\n", msg_name.c_str());
    return -1;
  }
  DynamicMessageFactory factory;
  const Message *message = factory.GetPrototype(descriptor);
  if (!message) {
    printf("error get new message: %s\n", msg_name.c_str());
    return -1;
  }
  std::unique_ptr<Message> query_info(message->New());
  int ret = Jsonstr2Pb(json_str, query_info.get());
  if (ret) {
    printf("Jsonstr2Pb error, ret:%d\n", ret);
    return -1;
  }
  query_info->SerializeToString(&out);
  return 0;
}
int PbLoader::PbBinary2Json(const string &msg_name, const string &pb_binary, string &out) const {  // NOLINT
  if (!importer_) return -1;
  Importer *importer = (Importer *)importer_;
  const Descriptor *descriptor = importer->pool()->FindMessageTypeByName(msg_name);
  if (!descriptor) {
    printf("error get message: %s\n", msg_name.c_str());
    return -1;
  }
  DynamicMessageFactory factory;
  const Message *message = factory.GetPrototype(descriptor);
  if (!message) {
    printf("error get new message: %s\n", msg_name.c_str());
    return -1;
  }
  std::unique_ptr<Message> query_info(message->New());
  query_info->ParseFromArray(pb_binary.data(), pb_binary.size());
  int ret = Pb2Jsonstr(*query_info.get(), &out);
  if (ret) {
    printf("Pb2Jsonstr error, ret:%d\n", ret);
    return -1;
  }
  return 0;
}

int PbLoader::PbBinary2FullJson(const string &msg_name, const string &pb_binary, string &out) const {
  if (!importer_) return -1;
  Importer *importer = (Importer *)importer_;  // NOLINT
  const Descriptor *descriptor = importer->pool()->FindMessageTypeByName(msg_name);
  if (!descriptor) {
    printf("error get message: %s\n", msg_name.c_str());
    return -1;
  }
  DynamicMessageFactory factory;
  const Message *message = factory.GetPrototype(descriptor);
  if (!message) {
    printf("error get new message: %s\n", msg_name.c_str());
    return -1;
  }
  std::unique_ptr<Message> query_info(message->New());
  query_info->ParseFromArray(pb_binary.data(), pb_binary.size());
  int ret = Pb2FullJsonstr(*query_info.get(), &out);
  if (ret) {
    printf("Pb2Jsonstr error, ret:%d\n", ret);
    return -1;
  }
  return 0;
}

int PbLoader::PbDefalut2FullJson(const string &msg_name, string &out) const {  // NOLINT
  if (!importer_) return -1;
  Importer *importer = (Importer *)importer_;  // NOLINT
  const Descriptor *descriptor = importer->pool()->FindMessageTypeByName(msg_name);
  if (!descriptor) {
    printf("error get message: %s\n", msg_name.c_str());
    return -1;
  }
  DynamicMessageFactory factory;
  const Message *message = factory.GetPrototype(descriptor);
  if (!message) {
    printf("error get new message: %s\n", msg_name.c_str());
    return -1;
  }
  std::unique_ptr<Message> query_info(message->New());
  int ret = Pb2FullJsonstr(*query_info.get(), &out);
  if (ret) {
    printf("Pb2Jsonstr error, ret:%d\n", ret);
    return -1;
  }
  return 0;
}
