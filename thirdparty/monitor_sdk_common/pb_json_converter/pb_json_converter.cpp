// @Copyright vivo
// author: yuchengzhen

#include "thirdparty/monitor_sdk_common/pb_json_converter/pb_json_converter.h"

#include <stdio.h>

#include <limits.h>
#include <string>
#include <vector>

#include "thirdparty/google/protobuf/descriptor.h"
#include "thirdparty/rapidjson/prettywriter.h"
#include "thirdparty/rapidjson/stringbuffer.h"
#include "thirdparty/rapidjson/writer.h"

using rapidjson::Document;
using rapidjson::PrettyWriter;
using rapidjson::StringBuffer;
using rapidjson::StringRef;
using rapidjson::Value;
using rapidjson::Writer;
using google::protobuf::Descriptor;
using google::protobuf::EnumValueDescriptor;
using google::protobuf::EnumDescriptor;
using google::protobuf::FieldDescriptor;
using google::protobuf::Message;
using google::protobuf::Reflection;
using std::string;
using std::vector;

static void SingleField2Value(const FieldDescriptor *field,
                        const Reflection *reflection, const Message &message,
                        Value &v, Document::AllocatorType &allocator, bool full = false, vector<string> *parent_keys = NULL);

static void RepeatedField2Value(const FieldDescriptor *field,
                        const Reflection *reflection, const Message &message,
                        Value &v, Document::AllocatorType &allocator, bool full = false, vector<string> *parent_keys = NULL);

static void ParseMessage(const Message &input, Value &v,
                         Document::AllocatorType &allocator);

static void ParseFullMessage(const Message &input, Value &v,
                         Document::AllocatorType &allocator, vector<string> *parent_keys);

static inline int64_t GetInt64(const Value &v);

static inline int32_t GetInt32(const Value &v);

static void Value2Field(const Value &v, const FieldDescriptor *field,
                        const Reflection *reflection, Message *message);

static void ValueAppend2Field(const Value &v, const FieldDescriptor *field,
                              const Reflection *reflection, Message *message);

static void ParseJson(const Value &v, Message *message);

static string Value2Jsonstr(Value &v, bool pretty);

static bool StrvectorHasItem(const vector<string> &v, const string &s) {
  for (int i = 0; i < (int)v.size(); ++i) {  // NOLINT
    if (v[i].compare(s) == 0) return true;
  }
  return false;
}

static void SingleField2Value(const FieldDescriptor *field,
                        const Reflection *reflection, const Message &message,
                        Value &v, Document::AllocatorType &allocator, bool full, vector<string> *parent_keys) {
  switch (field->cpp_type()) {
    case FieldDescriptor::CPPTYPE_INT32:
      v.SetInt(reflection->GetInt32(message, field));
      break;
    case FieldDescriptor::CPPTYPE_INT64:
      v.SetInt64(reflection->GetInt64(message, field));
      break;
    case FieldDescriptor::CPPTYPE_UINT32:
      v.SetUint(reflection->GetUInt32(message, field));
      break;
    case FieldDescriptor::CPPTYPE_UINT64:
      v.SetUint64(reflection->GetUInt64(message, field));
      break;
    case FieldDescriptor::CPPTYPE_DOUBLE:
      v.SetDouble(reflection->GetDouble(message, field));
      break;
    case FieldDescriptor::CPPTYPE_FLOAT:
      v.SetFloat(reflection->GetFloat(message, field));
      break;
    case FieldDescriptor::CPPTYPE_BOOL:
      v.SetBool(reflection->GetBool(message, field));
      break;
    case FieldDescriptor::CPPTYPE_ENUM:
      v.SetInt(reflection->GetEnum(message, field)->number());
      break;
    case FieldDescriptor::CPPTYPE_STRING:
    {
      string s = reflection->GetString(message, field);
      v.SetString(s.c_str(), s.size(), allocator);
    }
      break;
    case FieldDescriptor::CPPTYPE_MESSAGE:
    {
      const Message &sub_message = reflection->GetMessage(message, field);
      if (full && !StrvectorHasItem(*parent_keys, field->full_name())) {
        if (parent_keys != NULL) parent_keys->push_back(field->full_name());
        ParseFullMessage(sub_message, v, allocator, parent_keys);
        if (parent_keys != NULL && parent_keys->size() > 0) parent_keys->resize(parent_keys->size() - 1);
      } else {
        ParseMessage(sub_message, v, allocator);
      }
    }
      break;
    default:
      break;
  }
}

static void RepeatedField2Value(const FieldDescriptor *field,
                        const Reflection *reflection, const Message &message,
                        Value &v, Document::AllocatorType &allocator, bool full, vector<string> *parent_keys) {
  v.SetArray();
  switch (field->cpp_type()) {
    case FieldDescriptor::CPPTYPE_INT32:
      for (int i = 0; i < reflection->FieldSize(message, field); i++) {
        v.PushBack(Value().SetInt(reflection->GetRepeatedInt32(message, field, i)), allocator);
      }
      break;
    case FieldDescriptor::CPPTYPE_INT64:
      for (int i = 0; i < reflection->FieldSize(message, field); i++) {
        v.PushBack(Value().SetInt64(reflection->GetRepeatedInt64(message, field, i)), allocator);
      }
      break;
    case FieldDescriptor::CPPTYPE_UINT32:
      for (int i = 0; i < reflection->FieldSize(message, field); i++) {
        v.PushBack(Value().SetUint(reflection->GetRepeatedUInt32(message, field, i)), allocator);
      }
      break;
    case FieldDescriptor::CPPTYPE_UINT64:
      for (int i = 0; i < reflection->FieldSize(message, field); i++) {
        v.PushBack(Value().SetUint64(reflection->GetRepeatedUInt64(message, field, i)), allocator);
      }
      break;
    case FieldDescriptor::CPPTYPE_DOUBLE:
      for (int i = 0; i < reflection->FieldSize(message, field); i++) {
        v.PushBack(Value().SetDouble(reflection->GetRepeatedDouble(message, field, i)), allocator);
      }
      break;
    case FieldDescriptor::CPPTYPE_FLOAT:
      for (int i = 0; i < reflection->FieldSize(message, field); i++) {
        v.PushBack(Value().SetFloat(reflection->GetRepeatedFloat(message, field, i)), allocator);
      }
      break;
    case FieldDescriptor::CPPTYPE_BOOL:
      for (int i = 0; i < reflection->FieldSize(message, field); i++) {
        v.PushBack(Value().SetBool(reflection->GetRepeatedBool(message, field, i)), allocator);
      }
      break;
    case FieldDescriptor::CPPTYPE_ENUM:
      for (int i = 0; i < reflection->FieldSize(message, field); i++) {
        v.PushBack(Value().SetInt(reflection->GetRepeatedEnum(message, field, i)->number()), allocator);
      }
      break;
    case FieldDescriptor::CPPTYPE_STRING:
      for (int i = 0; i < reflection->FieldSize(message, field); i++) {
        string s = reflection->GetRepeatedString(message, field, i);
        v.PushBack(Value().SetString(s.c_str(), s.size(), allocator), allocator);
      }
      break;
    case FieldDescriptor::CPPTYPE_MESSAGE:
      for (int i = 0; i < reflection->FieldSize(message, field); i++) {
        const Message &sub_message = reflection->GetRepeatedMessage(message, field, i);
        Value tmp;
        if (full && !StrvectorHasItem(*parent_keys, field->full_name())) {
          if (parent_keys != NULL) parent_keys->push_back(field->full_name());
          ParseFullMessage(sub_message, tmp, allocator, parent_keys);
          if (parent_keys != NULL && parent_keys->size() > 0) parent_keys->resize(parent_keys->size() - 1);
        } else {
          ParseMessage(sub_message, tmp, allocator);
        }
        v.PushBack(tmp, allocator);
      }
      break;
    default:
      break;
  }
}

static void ParseMessage(const Message &input, Value &v,
                         Document::AllocatorType &allocator) {
  const Reflection *reflection = input.GetReflection();
  std::vector<const FieldDescriptor *> fields;
  reflection->ListFields(input, &fields);
  size_t fields_size = fields.size();
  v.SetObject();
  for (size_t i = 0; i < fields_size; i++) {
    const FieldDescriptor *field = fields[i];
    Value tmpk;
    Value tmpv;
    if (field->is_repeated()) {
      RepeatedField2Value(field, reflection, input, tmpv, allocator);
    } else {
      SingleField2Value(field, reflection, input, tmpv, allocator);
    }
    string s = field->name();
    tmpk.SetString(s.c_str(), s.size(), allocator);
    v.AddMember(tmpk, tmpv, allocator);
  }
}

static void ParseFullMessage(const Message &input, Value &v,
                         Document::AllocatorType &allocator, vector<string> *parent_keys) {
  const Reflection *reflection = input.GetReflection();
  const Descriptor *descriptor = input.GetDescriptor();
  int32_t field_cnt = descriptor->field_count();
  v.SetObject();
  for (int32_t i = 0; i < field_cnt; ++i) {
    const FieldDescriptor *field = descriptor->field(i);
    Value tmpk;
    Value tmpv;
    if (field->is_repeated()) {
      RepeatedField2Value(field, reflection, input, tmpv, allocator, true, parent_keys);
    } else {
      SingleField2Value(field, reflection, input, tmpv, allocator, true, parent_keys);
    }
    string s = field->name();
    tmpk.SetString(s.c_str(), s.size(), allocator);
    v.AddMember(tmpk, tmpv, allocator);
  }
}

static inline int64_t GetInt64(const Value &v) {
  int64_t ret = 0;
  if (v.IsInt()) {
    ret = v.GetInt();
  } else if (v.IsUint()) {
    ret = v.GetUint();
  } else if (v.IsInt64()) {
    ret = v.GetInt64();
  } else if (v.IsUint64()) {
    ret = (int64_t)v.GetUint64();
  } else if (v.IsDouble()){
    ret = (int64_t)v.GetDouble();
  }
  return ret;
}

static inline int32_t GetInt32(const Value &v) {
  int32_t ret = 0;
  int64_t i64 = GetInt64(v);
  if (i64 <= INT_MAX && i64 > INT_MIN) ret = (int32_t)i64;
  return ret;
}

static void Value2Field(const Value &v, const FieldDescriptor *field,
                        const Reflection *reflection, Message *message) {
  switch (field->cpp_type()) {
    case FieldDescriptor::CPPTYPE_INT32:
      reflection->SetInt32(message, field, GetInt32(v));
      break;
    case FieldDescriptor::CPPTYPE_INT64:
      reflection->SetInt64(message, field, GetInt64(v));
      break;
    case FieldDescriptor::CPPTYPE_UINT32:
      reflection->SetUInt32(message, field, (uint32_t)GetInt32(v));
      break;
    case FieldDescriptor::CPPTYPE_UINT64:
      reflection->SetUInt64(message, field, (uint64_t)GetInt64(v));
      break;
    case FieldDescriptor::CPPTYPE_DOUBLE:
      if (v.IsNumber()) reflection->SetDouble(message, field, v.GetDouble());
      break;
    case FieldDescriptor::CPPTYPE_FLOAT:
      if (v.IsNumber())
        reflection->SetFloat(message, field, (float)v.GetDouble());
      break;
    case FieldDescriptor::CPPTYPE_BOOL:
      if (v.IsBool()) reflection->SetBool(message, field, v.GetBool());
      break;
    case FieldDescriptor::CPPTYPE_ENUM:
    {
      const EnumValueDescriptor *enum_value =
          reflection->GetEnum(*message, field);
      const EnumDescriptor *enum_desc = enum_value->type();
      const EnumValueDescriptor *real_enum_value =
          enum_desc->FindValueByNumber(GetInt32(v));
      reflection->SetEnum(message, field, real_enum_value);
    }
      break;
    case FieldDescriptor::CPPTYPE_STRING:
      if (v.IsString()) {
        string s = v.GetString();
        reflection->SetString(message, field, s);
      }
      break;
    case FieldDescriptor::CPPTYPE_MESSAGE:
    {
      Message *sub_message = reflection->MutableMessage(message, field);
      ParseJson(v, sub_message);
    }
      break;
    default:
      break;
  }
}

static void ValueAppend2Field(const Value &v, const FieldDescriptor *field,
                              const Reflection *reflection, Message *message) {
  switch (field->cpp_type()) {
    case FieldDescriptor::CPPTYPE_INT32:
      reflection->AddInt32(message, field, GetInt32(v));
      break;
    case FieldDescriptor::CPPTYPE_INT64:
      reflection->AddInt64(message, field, GetInt64(v));
      break;
    case FieldDescriptor::CPPTYPE_UINT32:
      reflection->AddUInt32(message, field, (uint32_t)GetInt32(v));
      break;
    case FieldDescriptor::CPPTYPE_UINT64:
      reflection->AddUInt64(message, field, (uint64_t)GetInt64(v));
      break;
    case FieldDescriptor::CPPTYPE_DOUBLE:
      if (v.IsNumber()) reflection->AddDouble(message, field, v.GetDouble());
      break;
    case FieldDescriptor::CPPTYPE_FLOAT:
      if (v.IsNumber()) reflection->AddFloat(message, field, (float)v.GetDouble());
      break;
    case FieldDescriptor::CPPTYPE_BOOL:
      if (v.IsBool()) reflection->AddBool(message, field, v.GetBool());
      break;
    case FieldDescriptor::CPPTYPE_ENUM:
    {
      reflection->AddEnumValue(message, field, v.GetInt());
    }
      break;
    case FieldDescriptor::CPPTYPE_STRING:
      if (v.IsString()) {
        string s = v.GetString();
        reflection->AddString(message, field, s);
      }
      break;
    case FieldDescriptor::CPPTYPE_MESSAGE:
    {
      Message *sub_message = reflection->AddMessage(message, field);
      ParseJson(v, sub_message);
    }
      break;
    default:
      break;
  }
}

static void ParseJson(const Value &v, Message *message) {
  if (v.IsObject()) {
    for (Value::ConstMemberIterator it = v.MemberBegin(); it != v.MemberEnd(); ++it) {
      string k = it->name.GetString();
      const FieldDescriptor *field = message->GetDescriptor()->FindFieldByName(k);
      const Value &sub_v = it->value;
      if (!sub_v.IsNull() && field) {
        const Reflection *reflection = message->GetReflection();
        if (sub_v.IsArray() && field->is_repeated()) {
          for (Value::ConstValueIterator it = sub_v.Begin(); it != sub_v.End(); ++it) {
            ValueAppend2Field(*it, field, reflection, message);
          }
        } else if (!sub_v.IsArray() && !field->is_repeated()) {
          Value2Field(sub_v, field, reflection, message);
        }  // no else: type error
      }
    }
  }
}

static string Value2Jsonstr(Value &v, bool pretty) {
  StringBuffer sb;
  if (pretty) {
    PrettyWriter<StringBuffer> w(sb);
    v.Accept(w);
  } else {
    Writer<StringBuffer> w(sb);
    v.Accept(w);
  }
  return sb.GetString();
}

int Pb2Jsonstr(const Message &input, string *output) {
  Document doc;
  Pb2Json(input, &doc);
  string s = Value2Jsonstr(doc, false);
  output->swap(s);
  return 0;
}

int Pb2FullJsonstr(const Message &input, string *output) {
  Document doc;
  Pb2FullJson(input, &doc);
  string s = Value2Jsonstr(doc, false);
  output->swap(s);
  return 0;
}

int Jsonstr2Pb(const string &input, Message *output) {
  Document doc;
  if (doc.Parse(input.c_str(), input.size()).HasParseError()) return -1;
  return Json2Pb(doc, output);
}

int Pb2Json(const Message &input, Document *output) {
  Document::AllocatorType &allocator = output->GetAllocator();
  ParseMessage(input, *output, allocator);
  return 0;
}

int Pb2FullJson(const Message &input, Document *output) {
  Document::AllocatorType &allocator = output->GetAllocator();
  vector<string> parent_keys;
  ParseFullMessage(input, *output, allocator, &parent_keys);
  return 0;
}

int Json2Pb(const Document &input, Message *output) {
  ParseJson(input, output);
  return 0;
}

int Pb2JsonstrPretty(const Message &input, string *output) {
  Document doc;
  Pb2Json(input, &doc);
  string s = Value2Jsonstr(doc, true);
  output->swap(s);
  return 0;
}

int Pb2FullJsonstrPretty(const Message &input, string *output) {
  Document doc;
  Pb2FullJson(input, &doc);
  string s = Value2Jsonstr(doc, true);
  output->swap(s);
  return 0;
}
