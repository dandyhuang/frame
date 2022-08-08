// Copyright (c) 2015, Vivo Inc. All rights reserved.
/**
 * @file file_define.h
 * @brief
 * @author chenzheng05@baidu.com
 * @date 2015-01-20
 */

#ifndef COMMON_FILE_FILE_DEFINE_H
#define COMMON_FILE_FILE_DEFINE_H

#include <string.h>
#include <map>
#include <string>
#include <vector>
#include "thirdparty/monitor_sdk_common/base/stdint.h"

namespace common {

// Define the options when open a file.
// Some options may NOT work for all file implementation.
struct OpenFileOptions {
    static const uint32_t   kDefaultCacheBufLen = 512 * 1024; // 512k
    static const uint32_t   kDefaultCacheDataInterval = 1000; // microsends
    static const uint8_t    kDefaultFileBackupFactor = 3;
    static const bool       kDefaultLocalizedFirst = false;
    static const uint32_t   kDefaultPermission = 0644;

    explicit OpenFileOptions():
        cache_buf_len(kDefaultCacheBufLen),
        cache_data_interval(kDefaultCacheDataInterval),
        backup_factor(kDefaultFileBackupFactor),
        localize_first(kDefaultLocalizedFirst),
        permission(kDefaultPermission) {
    }

    uint32_t    cache_buf_len; // Only use cache when buffer length > 0
    uint32_t    cache_data_interval; // cache invalidation time, microsends.
    uint8_t     backup_factor; // The number of file replica.
    bool        localize_first; // try to write a local backup
    uint32_t    permission;    // 9 bit:rwxrwxrwx,first three bits is for owner, middle three bits
                               // is for other,the last three bits is for second role.
    std::string second_role;
};

// A helper class to construct and parse key-value pairs infomation.
// The info is in the form of: key1=value1;key2=value2.
// This helper can be used in parse additional info.
// Such as additional info in  OpenFileOptions and AttrsInfo.
class KeyValueInfoHelper {
public:
    // parse the key-value info string to info_map.
    // Return true if all parse OK.
    static bool ParseKeyValueInfo(
        const std::string& info,
        std::map<std::string, std::string>* info_map);

    // construct the key-value info string from a map's key-value pairs.
    // The result is store in input info.
    // Return true if all parse OK.
    static bool CreateKeyValueInfo(
        const std::map<std::string, std::string>& info_map,
        std::string* info);

    // Append a key-value pair to the given info.
    // Return true if append OK.
    static bool AppendKeyValueInfo(const std::string& key,
        const std::string& value, std::string* info);
};

// 面向用户的文件信息结构
struct AttrsMask {
    // should has mask
    unsigned char   file_type: 1;
    unsigned char   file_id: 1;

    unsigned char   file_identity: 1;
    unsigned char   file_role: 1;
    // For second role.
    unsigned char   file_second_role: 1;

    unsigned char   file_permission: 1;
    unsigned char   modify_user: 1;

    unsigned char   create_time: 1;
    unsigned char   modify_time: 1;
    unsigned char   access_time: 1;

    unsigned char   file_size: 1;
    unsigned char   backup_factor: 1;
    unsigned char   num_chunks: 1;

    // request stat info of a file or directory
    // subdirs or files in a directory will not be returned
    unsigned char   stat_info: 1;

    // For data consistency check.
    unsigned char   last_chunk_version: 1;
    unsigned char   node_set_hash: 1;

    // Match filters are set here.
    // Sub dirs are matched.
    unsigned char   filter_subdirs: 1;
    // Sub files are matched.
    unsigned char   filter_subfiles: 1;

    // Recursive list directory.
    unsigned char   is_recursive: 1;

    // addtional_info as a string.
    unsigned char   additional_info: 1;

    // get the freeze stat
    unsigned char is_freezed: 1;

    AttrsMask() {
        memset(this, 0, sizeof(*this));
        filter_subdirs = 1;
        filter_subfiles = 1;
    }
};

struct FileListOptions {
    FileListOptions() : count(-1), last_file(NULL) {}

    explicit FileListOptions(const AttrsMask* attrs_mask) {
        SetMask(attrs_mask);
    }

    void SetMask(const AttrsMask* attrs_mask) {
        mask = *attrs_mask;
        count = -1;
        last_file = NULL;
    }
    // FileListOptions是为了兼容AttrsMask，last_file表示上次list返回的最后一个文件
    //     如果last_file为NULL，表示从头开始List
    //     否则表示从last_file开始list，但是最终返回的结果不包括last_file
    // count表示本次期望获取的最大文件个数
    //     -1表示获取全部文件
    AttrsMask mask;
    int32_t count;
    const char* last_file;
};

struct RenameOptions {
    // overwrite参数表示当目标文件存在的时候，源文件是否覆盖目标文件
    // 如果overwrite=false，并且目标文件存在，rename操作失败
    bool overwrite;
    RenameOptions() : overwrite(true) {}
};

typedef FileListOptions ListOptions;

enum ENUM_FILE_TYPE {
    FILE_TYPE_UNKNOWNTYPE = 0,
    FILE_TYPE_NORMAL = 1,
    FILE_TYPE_DIR = 2,
    FILE_TYPE_RA = 3, // Record Append type, currently no implementation.
};

const char* GetFileTypeDesc(ENUM_FILE_TYPE type);

//
// FILE Public Error Code
// File对外错误码
//
#ifndef FILE_ERR_START_VALUE
#define FILE_ERR_START_VALUE 0x5F5E100
#endif

enum FILE_ERROR_CODE {
    ERR_FILE_OK = 1,
    ERR_FILE_FAIL = FILE_ERR_START_VALUE + 1,
    ERR_FILE_REOPEN_FOR_WRITE,
    ERR_FILE_RETRY,
    ERR_FILE_ENTRY_EXIST,
    ERR_FILE_ENTRY_NOT_EXIST,
    ERR_FILE_NOT_INIT,
    ERR_FILE_CLOSED,
    ERR_FILE_OPENMODE,
    ERR_FILE_PARAMETER,
    ERR_FILE_PERMISSION_DENIED,
    ERR_FILE_NOT_EMPTY_DIRECTORY,
    ERR_FILE_QUOTA_EXCEED,
    ERR_FILE_TOO_MANY_ENTRIES,
    ERR_FILE_INPROGRESS,
    ERR_FILE_PATH_TOO_LONG,
    ERR_FILE_EXPECTED,
    ERR_FILE_REMOVED,
    ERR_FILE_CORRUPT,
    ERR_FILE_OUT_OF_RANGE,
    ERR_FILE_OVERWRITE_NON_DIRECTORY_WITH_DIRECTORY,
    ERR_FILE_OVERWRITE_DIRECTORY_WITH_NON_DIRECTORY,
    ERR_FILE_INVALID_ROLE_NAME,
    ERR_FILE_EMPTY,
    ERR_FILE_ANCESTOR_NOT_DIR,
    ERR_FILE_INVALID_PERMISSION_VALUE,
    ERR_FILE_EXCEED_MAX_DEPTH,
    ERR_FILE_VERSION_INCOMPATIBLE,
    ERR_FILE_DIR_EXPECTED,
    ERR_FILE_NOSPACE,
    ERR_FILE_FUNCTION_NO_IMPLEMENTATION,
    ERR_FILE_FREEZED,
    ERR_FILE_BREAK_DIRECTORY_SPECIFICATION,
    ERR_FILE_DNS_FAILURE,
    ERR_FILE_GET_ZK_CONTENT_FAILURE,
    ERR_FILE_REMOVE_TOO_FAST,
    ERR_WRONG_CLUSTER,
    ERR_FILE_CROSS_DEVICE,
};

const char* GetFileErrorCodeStr(unsigned int error_code);

struct AttrsInfo {
    std::string         file_name;
    ENUM_FILE_TYPE      file_type;
    uint64_t            file_id;

    std::string         file_identity; // For LocalFile is owner
    std::string         file_role; // For LocalFile is group
    std::string         file_second_role; // Only for xfs file.
    // The permission bits layout:
    // For LocalFile, use the last 9 bits as owner, group, others permission.
    //     | not used | owner    | group    | other group |
    //     | 15~9     | 8~6(rwx) | 5~3(rwx) | 2~0(rwx)    |
    // For XFSFile, use the last 9 bits bits as role, other roles, second role permission.
    //     | not used | role     | other role | second role |
    //     | 15~9     | 8~6(rwx) | 5~3(rwx)   | 2~0(rwx)    |
    uint16_t            file_permission;
    std::string         modify_user;

    // time in seconds since the epoch 1970.
    uint32_t            create_time;
    uint32_t            modify_time;
    uint32_t            access_time;

    // signed, so that the -1 indicate un assigned value.
    int64_t             file_size;
    // unsigned, 0 means init value.
    uint8_t             backup_factor;
    // chunk number
    uint32_t            num_chunks;
    // For data consistency check.
    uint32_t            last_chunk_version;
    uint32_t            node_set_hash;
    // if is_freezed equals true, the file can not be append
    bool                is_freezed;
    // error
    uint32_t            error_code;

    // additional info should be interpreted by file implementations.
    // Usually in the form of key1=value1;key2=value2
    std::string         additional_info;

    AttrsInfo() :
        file_type(FILE_TYPE_UNKNOWNTYPE),
        file_id(0),
        file_permission(0),
        create_time(0), modify_time(0), access_time(0),
        file_size(-1), backup_factor(0), num_chunks(0),
        last_chunk_version(0), node_set_hash(0), is_freezed(false),
        error_code(ERR_FILE_RETRY) {
    }
};

struct Attrs {
    AttrsMask       file_meta_mask;
    AttrsInfo       file_meta_info;
};

struct DataLocation {
    uint32_t        net_order_ip;
    uint16_t        net_order_port;
    uint64_t        start;
    uint64_t        end;
    uint32_t        chunk_index;
    uint32_t        total_bak_num;
    uint32_t        bak_sequence;
    uint32_t        chunk_checksum;

    // 数据占总文件的百分比
    float           size_percent;

    DataLocation() {
        memset(this, 0, sizeof(*this));
    }
};

// File content summary holds summrized information from given path, including sub-directories.
// This struct usually stores more detailed "du" result.
struct FileContentSummary {
    // Negative values are un-initialized.
    FileContentSummary() :
        total_size(-1),
        file_count(-1),
        dir_count(-1),
        chunk_count(-1),
        recycle_total_size(-1),
        recycle_file_count(-1),
        recycle_dir_count(-1),
        recycle_chunk_count(-1) {
    }

    FileContentSummary(int64_t total_sz, int64_t file_cnt, int64_t dir_cnt, int64_t chunk_cnt,
        int64_t re_total_sz, int64_t re_file_cnt, int64_t re_dir_cnt, int64_t re_chunk_cnt)
        : total_size(total_sz),
          file_count(file_cnt),
          dir_count(dir_cnt),
          chunk_count(chunk_cnt),
          recycle_total_size(re_total_sz),
          recycle_file_count(re_file_cnt),
          recycle_dir_count(re_dir_cnt),
          recycle_chunk_count(re_chunk_cnt) {
    }

    int64_t total_size; // size of files or directories, in bytes.
    int64_t file_count;
    int64_t dir_count;
    int64_t chunk_count;

    // summary for file or directories in recycle bin.
    int64_t recycle_total_size;
    int64_t recycle_file_count;
    int64_t recycle_dir_count;
    int64_t recycle_chunk_count;
};

// Use this mask class to indicate which fields are needed or returned.
struct FileContentSummaryMask {
    unsigned char total_size : 1;
    unsigned char file_count : 1;
    unsigned char dir_count : 1;
    unsigned char chunk_count : 1;

    unsigned char recycle_total_size : 1;
    unsigned char recycle_file_count : 1;
    unsigned char recycle_dir_count : 1;
    unsigned char recycle_chunk_count : 1;

    FileContentSummaryMask()
        : total_size(false),
          file_count(false),
          dir_count(false),
          chunk_count(false),
          recycle_total_size(false),
          recycle_file_count(false),
          recycle_dir_count(false),
          recycle_chunk_count(false) {
    }
};

} // namespace common

// TODO(aaronzou): this using is a quick solution to fix link problems with other projects.
//      Next version must delete this one and all projects use qualified names.
//      Including this header is dangerous and may introduce conflict names.

using common::OpenFileOptions;
using common::KeyValueInfoHelper;
using common::AttrsMask;
using common::ENUM_FILE_TYPE;
using common::AttrsInfo;
using common::Attrs;
using common::ListOptions;
using common::RenameOptions;
using common::DataLocation;
using common::FileContentSummary;
using common::FileContentSummaryMask;
using common::GetFileTypeDesc;
using common::FILE_ERROR_CODE;
using common::GetFileErrorCodeStr;

using common::FILE_TYPE_UNKNOWNTYPE;
using common::FILE_TYPE_NORMAL;
using common::FILE_TYPE_DIR;
using common::FILE_TYPE_RA;

using common::ERR_FILE_OK;
using common::ERR_FILE_FAIL;
using common::ERR_FILE_REOPEN_FOR_WRITE;
using common::ERR_FILE_RETRY;
using common::ERR_FILE_ENTRY_EXIST;
using common::ERR_FILE_ENTRY_NOT_EXIST;
using common::ERR_FILE_NOT_INIT;
using common::ERR_FILE_CLOSED;
using common::ERR_FILE_OPENMODE;
using common::ERR_FILE_PARAMETER;
using common::ERR_FILE_PERMISSION_DENIED;
using common::ERR_FILE_NOT_EMPTY_DIRECTORY;
using common::ERR_FILE_QUOTA_EXCEED;
using common::ERR_FILE_TOO_MANY_ENTRIES;
using common::ERR_FILE_INPROGRESS;
using common::ERR_FILE_PATH_TOO_LONG;
using common::ERR_FILE_EXPECTED;
using common::ERR_FILE_REMOVED;
using common::ERR_FILE_CORRUPT;
using common::ERR_FILE_OUT_OF_RANGE;
using common::ERR_FILE_OVERWRITE_NON_DIRECTORY_WITH_DIRECTORY;
using common::ERR_FILE_OVERWRITE_DIRECTORY_WITH_NON_DIRECTORY;
using common::ERR_FILE_INVALID_ROLE_NAME;
using common::ERR_FILE_EMPTY;
using common::ERR_FILE_ANCESTOR_NOT_DIR;
using common::ERR_FILE_INVALID_PERMISSION_VALUE;
using common::ERR_FILE_EXCEED_MAX_DEPTH;
using common::ERR_FILE_VERSION_INCOMPATIBLE;
using common::ERR_FILE_DIR_EXPECTED;
using common::ERR_FILE_NOSPACE;
using common::ERR_FILE_FUNCTION_NO_IMPLEMENTATION;
using common::ERR_FILE_FREEZED;
using common::ERR_FILE_BREAK_DIRECTORY_SPECIFICATION;
using common::ERR_FILE_DNS_FAILURE;
using common::ERR_FILE_GET_ZK_CONTENT_FAILURE;
using common::ERR_FILE_REMOVE_TOO_FAST;
using common::ERR_WRONG_CLUSTER;
using common::ERR_FILE_CROSS_DEVICE;

#endif // COMMON_FILE_FILE_DEFINE_H
