// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: brpc/options.proto

#ifndef PROTOBUF_INCLUDED_brpc_2foptions_2eproto
#define PROTOBUF_INCLUDED_brpc_2foptions_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3007000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3007001 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
#include <google/protobuf/descriptor.pb.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_brpc_2foptions_2eproto

// Internal implementation detail -- do not use these members.
struct TableStruct_brpc_2foptions_2eproto {
  static const ::google::protobuf::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors_brpc_2foptions_2eproto();
namespace brpc {
class ChunkInfo;
class ChunkInfoDefaultTypeInternal;
extern ChunkInfoDefaultTypeInternal _ChunkInfo_default_instance_;
}  // namespace brpc
namespace google {
namespace protobuf {
template<> ::brpc::ChunkInfo* Arena::CreateMaybeMessage<::brpc::ChunkInfo>(Arena*);
}  // namespace protobuf
}  // namespace google
namespace brpc {

enum TalkType {
  TALK_TYPE_NORMAL = 0,
  TALK_TYPE_ONEWAY = 1
};
bool TalkType_IsValid(int value);
const TalkType TalkType_MIN = TALK_TYPE_NORMAL;
const TalkType TalkType_MAX = TALK_TYPE_ONEWAY;
const int TalkType_ARRAYSIZE = TalkType_MAX + 1;

const ::google::protobuf::EnumDescriptor* TalkType_descriptor();
inline const ::std::string& TalkType_Name(TalkType value) {
  return ::google::protobuf::internal::NameOfEnum(
    TalkType_descriptor(), value);
}
inline bool TalkType_Parse(
    const ::std::string& name, TalkType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<TalkType>(
    TalkType_descriptor(), name, value);
}
enum ConnectionType {
  CONNECTION_TYPE_UNKNOWN = 0,
  CONNECTION_TYPE_SINGLE = 1,
  CONNECTION_TYPE_POOLED = 2,
  CONNECTION_TYPE_SHORT = 4
};
bool ConnectionType_IsValid(int value);
const ConnectionType ConnectionType_MIN = CONNECTION_TYPE_UNKNOWN;
const ConnectionType ConnectionType_MAX = CONNECTION_TYPE_SHORT;
const int ConnectionType_ARRAYSIZE = ConnectionType_MAX + 1;

const ::google::protobuf::EnumDescriptor* ConnectionType_descriptor();
inline const ::std::string& ConnectionType_Name(ConnectionType value) {
  return ::google::protobuf::internal::NameOfEnum(
    ConnectionType_descriptor(), value);
}
inline bool ConnectionType_Parse(
    const ::std::string& name, ConnectionType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ConnectionType>(
    ConnectionType_descriptor(), name, value);
}
enum ProtocolType {
  PROTOCOL_UNKNOWN = 0,
  PROTOCOL_BAIDU_STD = 1,
  PROTOCOL_STREAMING_RPC = 2,
  PROTOCOL_HULU_PBRPC = 3,
  PROTOCOL_SOFA_PBRPC = 4,
  PROTOCOL_RTMP = 5,
  PROTOCOL_THRIFT = 6,
  PROTOCOL_HTTP = 7,
  PROTOCOL_PUBLIC_PBRPC = 8,
  PROTOCOL_NOVA_PBRPC = 9,
  PROTOCOL_REDIS = 10,
  PROTOCOL_NSHEAD_CLIENT = 11,
  PROTOCOL_NSHEAD = 12,
  PROTOCOL_HADOOP_RPC = 13,
  PROTOCOL_HADOOP_SERVER_RPC = 14,
  PROTOCOL_MONGO = 15,
  PROTOCOL_UBRPC_COMPACK = 16,
  PROTOCOL_DIDX_CLIENT = 17,
  PROTOCOL_MEMCACHE = 18,
  PROTOCOL_ITP = 19,
  PROTOCOL_NSHEAD_MCPACK = 20,
  PROTOCOL_DISP_IDL = 21,
  PROTOCOL_ERSDA_CLIENT = 22,
  PROTOCOL_UBRPC_MCPACK2 = 23,
  PROTOCOL_CDS_AGENT = 24,
  PROTOCOL_ESP = 25,
  PROTOCOL_H2 = 26,
  PROTOCAL_DANDY_RTRS_COMMON = 27
};
bool ProtocolType_IsValid(int value);
const ProtocolType ProtocolType_MIN = PROTOCOL_UNKNOWN;
const ProtocolType ProtocolType_MAX = PROTOCAL_DANDY_RTRS_COMMON;
const int ProtocolType_ARRAYSIZE = ProtocolType_MAX + 1;

const ::google::protobuf::EnumDescriptor* ProtocolType_descriptor();
inline const ::std::string& ProtocolType_Name(ProtocolType value) {
  return ::google::protobuf::internal::NameOfEnum(
    ProtocolType_descriptor(), value);
}
inline bool ProtocolType_Parse(
    const ::std::string& name, ProtocolType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ProtocolType>(
    ProtocolType_descriptor(), name, value);
}
enum CompressType {
  COMPRESS_TYPE_NONE = 0,
  COMPRESS_TYPE_SNAPPY = 1,
  COMPRESS_TYPE_GZIP = 2,
  COMPRESS_TYPE_ZLIB = 3,
  COMPRESS_TYPE_LZ4 = 4
};
bool CompressType_IsValid(int value);
const CompressType CompressType_MIN = COMPRESS_TYPE_NONE;
const CompressType CompressType_MAX = COMPRESS_TYPE_LZ4;
const int CompressType_ARRAYSIZE = CompressType_MAX + 1;

const ::google::protobuf::EnumDescriptor* CompressType_descriptor();
inline const ::std::string& CompressType_Name(CompressType value) {
  return ::google::protobuf::internal::NameOfEnum(
    CompressType_descriptor(), value);
}
inline bool CompressType_Parse(
    const ::std::string& name, CompressType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<CompressType>(
    CompressType_descriptor(), name, value);
}
// ===================================================================

class ChunkInfo :
    public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:brpc.ChunkInfo) */ {
 public:
  ChunkInfo();
  virtual ~ChunkInfo();

  ChunkInfo(const ChunkInfo& from);

  inline ChunkInfo& operator=(const ChunkInfo& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  ChunkInfo(ChunkInfo&& from) noexcept
    : ChunkInfo() {
    *this = ::std::move(from);
  }

  inline ChunkInfo& operator=(ChunkInfo&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::google::protobuf::Descriptor* descriptor() {
    return default_instance().GetDescriptor();
  }
  static const ChunkInfo& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const ChunkInfo* internal_default_instance() {
    return reinterpret_cast<const ChunkInfo*>(
               &_ChunkInfo_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(ChunkInfo* other);
  friend void swap(ChunkInfo& a, ChunkInfo& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline ChunkInfo* New() const final {
    return CreateMaybeMessage<ChunkInfo>(nullptr);
  }

  ChunkInfo* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<ChunkInfo>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const ChunkInfo& from);
  void MergeFrom(const ChunkInfo& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  #if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  static const char* _InternalParse(const char* begin, const char* end, void* object, ::google::protobuf::internal::ParseContext* ctx);
  ::google::protobuf::internal::ParseFunc _ParseFunc() const final { return _InternalParse; }
  #else
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  #endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(ChunkInfo* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required int64 stream_id = 1;
  bool has_stream_id() const;
  void clear_stream_id();
  static const int kStreamIdFieldNumber = 1;
  ::google::protobuf::int64 stream_id() const;
  void set_stream_id(::google::protobuf::int64 value);

  // required int64 chunk_id = 2;
  bool has_chunk_id() const;
  void clear_chunk_id();
  static const int kChunkIdFieldNumber = 2;
  ::google::protobuf::int64 chunk_id() const;
  void set_chunk_id(::google::protobuf::int64 value);

  // @@protoc_insertion_point(class_scope:brpc.ChunkInfo)
 private:
  class HasBitSetters;

  // helper for ByteSizeLong()
  size_t RequiredFieldsByteSizeFallback() const;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::HasBits<1> _has_bits_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  ::google::protobuf::int64 stream_id_;
  ::google::protobuf::int64 chunk_id_;
  friend struct ::TableStruct_brpc_2foptions_2eproto;
};
// ===================================================================

static const int kServiceTimeoutFieldNumber = 90000;
extern ::google::protobuf::internal::ExtensionIdentifier< ::google::protobuf::ServiceOptions,
    ::google::protobuf::internal::PrimitiveTypeTraits< ::google::protobuf::int64 >, 3, false >
  service_timeout;
static const int kRequestTalkTypeFieldNumber = 90001;
extern ::google::protobuf::internal::ExtensionIdentifier< ::google::protobuf::MethodOptions,
    ::google::protobuf::internal::EnumTypeTraits< ::brpc::TalkType, ::brpc::TalkType_IsValid>, 14, false >
  request_talk_type;
static const int kResponseTalkTypeFieldNumber = 90002;
extern ::google::protobuf::internal::ExtensionIdentifier< ::google::protobuf::MethodOptions,
    ::google::protobuf::internal::EnumTypeTraits< ::brpc::TalkType, ::brpc::TalkType_IsValid>, 14, false >
  response_talk_type;
static const int kMethodTimeoutFieldNumber = 90003;
extern ::google::protobuf::internal::ExtensionIdentifier< ::google::protobuf::MethodOptions,
    ::google::protobuf::internal::PrimitiveTypeTraits< ::google::protobuf::int64 >, 3, false >
  method_timeout;
static const int kRequestCompressionFieldNumber = 90004;
extern ::google::protobuf::internal::ExtensionIdentifier< ::google::protobuf::MethodOptions,
    ::google::protobuf::internal::EnumTypeTraits< ::brpc::CompressType, ::brpc::CompressType_IsValid>, 14, false >
  request_compression;
static const int kResponseCompressionFieldNumber = 90005;
extern ::google::protobuf::internal::ExtensionIdentifier< ::google::protobuf::MethodOptions,
    ::google::protobuf::internal::EnumTypeTraits< ::brpc::CompressType, ::brpc::CompressType_IsValid>, 14, false >
  response_compression;

// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// ChunkInfo

// required int64 stream_id = 1;
inline bool ChunkInfo::has_stream_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ChunkInfo::clear_stream_id() {
  stream_id_ = PROTOBUF_LONGLONG(0);
  _has_bits_[0] &= ~0x00000001u;
}
inline ::google::protobuf::int64 ChunkInfo::stream_id() const {
  // @@protoc_insertion_point(field_get:brpc.ChunkInfo.stream_id)
  return stream_id_;
}
inline void ChunkInfo::set_stream_id(::google::protobuf::int64 value) {
  _has_bits_[0] |= 0x00000001u;
  stream_id_ = value;
  // @@protoc_insertion_point(field_set:brpc.ChunkInfo.stream_id)
}

// required int64 chunk_id = 2;
inline bool ChunkInfo::has_chunk_id() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ChunkInfo::clear_chunk_id() {
  chunk_id_ = PROTOBUF_LONGLONG(0);
  _has_bits_[0] &= ~0x00000002u;
}
inline ::google::protobuf::int64 ChunkInfo::chunk_id() const {
  // @@protoc_insertion_point(field_get:brpc.ChunkInfo.chunk_id)
  return chunk_id_;
}
inline void ChunkInfo::set_chunk_id(::google::protobuf::int64 value) {
  _has_bits_[0] |= 0x00000002u;
  chunk_id_ = value;
  // @@protoc_insertion_point(field_set:brpc.ChunkInfo.chunk_id)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace brpc

namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::brpc::TalkType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::brpc::TalkType>() {
  return ::brpc::TalkType_descriptor();
}
template <> struct is_proto_enum< ::brpc::ConnectionType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::brpc::ConnectionType>() {
  return ::brpc::ConnectionType_descriptor();
}
template <> struct is_proto_enum< ::brpc::ProtocolType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::brpc::ProtocolType>() {
  return ::brpc::ProtocolType_descriptor();
}
template <> struct is_proto_enum< ::brpc::CompressType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::brpc::CompressType>() {
  return ::brpc::CompressType_descriptor();
}

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // PROTOBUF_INCLUDED_brpc_2foptions_2eproto
