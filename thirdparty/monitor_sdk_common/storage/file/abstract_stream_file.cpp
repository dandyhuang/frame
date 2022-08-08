// Copyright (c) 2015, Vivo Inc. All rights reserved.
/**
 * @file abstract_stream_file.cpp
 * @brief
 * @author chenzheng05@baidu.com
 * @date 2015-01-20
 */
// Implementation AbstractStreamFile.

#include "thirdparty/monitor_sdk_common/storage/file/abstract_stream_file.h"

#include "thirdparty/glog/logging.h"

namespace common {

bool AbstractStreamFile::InitImpl() {
    LOG(FATAL) << "AbstractStreamFile::InitImpl not implemented";
    return false;
}

bool AbstractStreamFile::CleanupImpl() {
    LOG(FATAL) << "AbstractStreamFile::CleanupImpl not implemented";
    return false;
}

bool AbstractStreamFile::OpenImpl(
    const char *file_path, uint32_t flags,
    const OpenFileOptions& options,
    uint32_t *error_code) {
    LOG(FATAL) << "AbstractStreamFile::OpenImpl not implemented";
    return false;
}

int32_t AbstractStreamFile::MoveImpl(
    const char* src_name,
    const char* dst_name,
    uint32_t* error_code) {
    LOG(FATAL) << "AbstractStreamFile::MoveImpl not implemented";
    return -1;
}

int32_t AbstractStreamFile::RenameImpl(
    const char* old_path_name,
    const char* new_path_name,
    uint32_t*   error_code) {
    LOG(FATAL) << "AbstractStreamFile::RenameImpl not implemented";
    return -1;
}

int32_t AbstractStreamFile::RemoveImpl(
    const char* path_name,
    bool        is_recursive,
    uint32_t*   error_code) {
    LOG(FATAL) << "AbstractStreamFile::RemoveImpl not implemented";
    return -1;
}

int32_t AbstractStreamFile::AddDirImpl(
    const char* path_name,
    uint32_t* error_code) {
    LOG(FATAL) << "AbstractStreamFile::AddDirImpl not implemented";
    return -1;
}

int32_t AbstractStreamFile::ListImpl(
    const char* pattern,
    const ListOptions& list_option,
    std::vector<AttrsInfo>* buffer,
    uint32_t* error_code) {
    LOG(FATAL) << "AbstractStreamFile::ListImpl not implemented";
    return -1;
}

int64_t AbstractStreamFile::DuImpl(
    const char* path_name,
    uint32_t* error_code) {
    LOG(FATAL) << "AbstractStreamFile::DuImpl not implemented";
    return -1;
}

bool AbstractStreamFile::GetContentSummaryImpl(
    const char* path_name,
    FileContentSummaryMask* mask,
    FileContentSummary* summary,
    uint32_t* error_code) {
    LOG(FATAL) << "AbstractStreamFile::GetContentSummaryImpl not implemented";
    return false;
}

int64_t AbstractStreamFile::GetSizeImpl(
    const char* file_name,
    uint32_t*   error_code) {
    LOG(FATAL) << "AbstractStreamFile::GetSizeImpl not implemented";
    return -1;
}

bool AbstractStreamFile::CheckExistImpl(
    const char* path_name,
    uint32_t* error_code) {
    LOG(FATAL) << "AbstractStreamFile::CheckExistImpl not implemented";
    return false;
}

bool AbstractStreamFile::ChmodImpl(
    const char* path_name,
    const uint32_t permission,
    uint32_t* error_code) {
    LOG(FATAL) << "AbstractStreamFile::ChmodImpl not implemented";
    return false;
}

bool AbstractStreamFile::ChangeRoleImpl(
    const char* path_name,
    const char* role_name,
    uint32_t* error_code) {
    LOG(FATAL) << "AbstractStreamFile::ChangeRoleImpl not implemented";
    return false;
}

bool AbstractStreamFile::ChangeSecondRoleImpl(
    const char* path_name,
    const char* second_role_name,
    bool is_recursive,
    uint32_t* error_code) {
    LOG(FATAL) << "AbstractStreamFile::ChangeSecondRoleImpl not implemented";
    return false;
}

bool AbstractStreamFile::GetDigestImpl(
    File* file_obj,
    int64_t   file_size,
    uint32_t* digest,
    uint32_t* error_code) {
    LOG(FATAL) << "AbstractStreamFile::GetDigestImpl not implemented";
    return false;
}

} // namespace common
