// @Copyright vivo
// author: yuchengzhen

#ifndef COMMON_PB_JSON_CONVERTER_H_
#define COMMON_PB_JSON_CONVERTER_H_

#include <string>

#include "thirdparty/google/protobuf/message.h"
#include "thirdparty/rapidjson/document.h"

int Pb2Jsonstr(const google::protobuf::Message& input, std::string *output);

int Pb2FullJsonstr(const google::protobuf::Message& input, std::string *output);

int Jsonstr2Pb(const std::string &input, google::protobuf::Message* output);

int Pb2Json(const google::protobuf::Message& input, rapidjson::Document *output);

int Pb2FullJson(const google::protobuf::Message& input, rapidjson::Document *output);

int Json2Pb(const rapidjson::Document &input, google::protobuf::Message* output);

int Pb2JsonstrPretty(const google::protobuf::Message& input, std::string *output);

int Pb2FullJsonstrPretty(const google::protobuf::Message& input, std::string *output);

#endif  // COMMON_PB_JSON_CONVERTER_H_

