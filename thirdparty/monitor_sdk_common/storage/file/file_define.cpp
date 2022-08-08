// Copyright (c) 2015, Vivo Inc. All rights reserved.
/**
 * @file file_define.cpp
 * @brief
 * @author chenzheng05@baidu.com
 * @date 2015-01-20
 */

#include "thirdparty/monitor_sdk_common/storage/file/file_define.h"

namespace common {

#ifndef _WIN32
    // don't define on windows
    const uint32_t OpenFileOptions::kDefaultCacheBufLen;
    const uint32_t OpenFileOptions::kDefaultCacheDataInterval;
    const uint8_t  OpenFileOptions::kDefaultFileBackupFactor;
    const bool     OpenFileOptions::kDefaultLocalizedFirst;
#endif

bool KeyValueInfoHelper::ParseKeyValueInfo(
    const std::string& info,
    std::map<std::string, std::string>* info_map) {
    if (info_map == NULL) return false;
    if (info_map->size() > 0) info_map->clear();

    // segment options by ':'
    std::vector<std::string> segs;
    std::string::size_type index = std::string::npos;
    std::string::size_type pre_index = 0;

    while ((index = info.find(':', pre_index)) != std::string::npos) {
        segs.push_back(info.substr(pre_index, index - pre_index));
        pre_index = index + 1;
    }
    if (pre_index < info.length()) {
        segs.push_back(info.substr(pre_index));
    }

    // parse each segment.
    for (std::vector<std::string>::size_type i = 0; i < segs.size(); ++i) {
        std::string::size_type eq_index = segs[i].find('=');
        if (eq_index == std::string::npos) return false; // no '=', wrong options.

        const std::string key = segs[i].substr(0, eq_index);
        const std::string value = segs[i].substr(eq_index + 1);
        if (key.length() == 0 || value.length() == 0) {
            return false; // wrong options.
        }

        info_map->insert(make_pair(key, value));
    }

    return true;
}

bool KeyValueInfoHelper::CreateKeyValueInfo(
    const std::map<std::string, std::string>& info_map,
    std::string* info) {
    if (info == NULL) return false;
    if (info->length() > 0) info->clear();

    std::string& info_ref = *info;

    std::map<std::string, std::string>::const_iterator it = info_map.begin();
    for ( ; it != info_map.end(); ++it) {
        if (info_ref.length() > 0)
            info_ref += ':';
        if (it->first.empty() || it->second.empty()) {
            return false;
        }

        info_ref += it->first;
        info_ref += '=';
        info_ref += it->second;
    }

    return true;
}

bool KeyValueInfoHelper::AppendKeyValueInfo(const std::string& key,
        const std::string& value, std::string* info) {
    if (info == NULL) return false;
    if (key.length() == 0 || value.length() == 0) return false;

    if (info->length() > 0) (*info) += ":";

    (*info) += key;
    (*info) += "=";
    (*info) += value;

    return true;
}

// for switch-case.
#define CONSIDER_CASE(type) case type: return #type

const char* GetFileTypeDesc(ENUM_FILE_TYPE type) {
    switch (static_cast<ENUM_FILE_TYPE>(type)) {
        CONSIDER_CASE(FILE_TYPE_UNKNOWNTYPE);
        CONSIDER_CASE(FILE_TYPE_NORMAL);
        CONSIDER_CASE(FILE_TYPE_DIR);
        CONSIDER_CASE(FILE_TYPE_RA);
    }
    return "---unknown file type---";
}

const char* GetFileErrorCodeStr(uint32_t error_code) {
    switch (static_cast<FILE_ERROR_CODE>(error_code)) {
        CONSIDER_CASE(ERR_FILE_OK);
        CONSIDER_CASE(ERR_FILE_FAIL);
        CONSIDER_CASE(ERR_FILE_RETRY);
        CONSIDER_CASE(ERR_FILE_REOPEN_FOR_WRITE);
        CONSIDER_CASE(ERR_FILE_ENTRY_EXIST);
        CONSIDER_CASE(ERR_FILE_ENTRY_NOT_EXIST);
        CONSIDER_CASE(ERR_FILE_NOT_INIT);
        CONSIDER_CASE(ERR_FILE_CLOSED);
        CONSIDER_CASE(ERR_FILE_OPENMODE);
        CONSIDER_CASE(ERR_FILE_PARAMETER);
        CONSIDER_CASE(ERR_FILE_PERMISSION_DENIED);
        CONSIDER_CASE(ERR_FILE_NOT_EMPTY_DIRECTORY);
        CONSIDER_CASE(ERR_FILE_QUOTA_EXCEED);
        CONSIDER_CASE(ERR_FILE_TOO_MANY_ENTRIES);
        CONSIDER_CASE(ERR_FILE_INPROGRESS);
        CONSIDER_CASE(ERR_FILE_PATH_TOO_LONG);
        CONSIDER_CASE(ERR_FILE_EXPECTED);
        CONSIDER_CASE(ERR_FILE_REMOVED);
        CONSIDER_CASE(ERR_FILE_CORRUPT);
        CONSIDER_CASE(ERR_FILE_OUT_OF_RANGE);
        CONSIDER_CASE(ERR_FILE_OVERWRITE_NON_DIRECTORY_WITH_DIRECTORY);
        CONSIDER_CASE(ERR_FILE_OVERWRITE_DIRECTORY_WITH_NON_DIRECTORY);
        CONSIDER_CASE(ERR_FILE_INVALID_ROLE_NAME);
        CONSIDER_CASE(ERR_FILE_EMPTY);
        CONSIDER_CASE(ERR_FILE_ANCESTOR_NOT_DIR);
        CONSIDER_CASE(ERR_FILE_INVALID_PERMISSION_VALUE);
        CONSIDER_CASE(ERR_FILE_EXCEED_MAX_DEPTH);
        CONSIDER_CASE(ERR_FILE_VERSION_INCOMPATIBLE);
        CONSIDER_CASE(ERR_FILE_DIR_EXPECTED);
        CONSIDER_CASE(ERR_FILE_NOSPACE);
        CONSIDER_CASE(ERR_FILE_FUNCTION_NO_IMPLEMENTATION);
        CONSIDER_CASE(ERR_FILE_FREEZED);
        CONSIDER_CASE(ERR_FILE_BREAK_DIRECTORY_SPECIFICATION);
        CONSIDER_CASE(ERR_FILE_DNS_FAILURE);
        CONSIDER_CASE(ERR_FILE_GET_ZK_CONTENT_FAILURE);
        CONSIDER_CASE(ERR_FILE_REMOVE_TOO_FAST);
        CONSIDER_CASE(ERR_WRONG_CLUSTER);
        CONSIDER_CASE(ERR_FILE_CROSS_DEVICE);
        // don't using default
    }
    return "ERR_FILE_UNKNOWN";
}

} // namespace common

