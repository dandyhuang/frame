// Copyright (c) 2015, Vivo Inc.
// All rights reserved.

#include "thirdparty/monitor_sdk_common/storage/file/file_mock2.h"
#include "thirdparty/glog/logging.h"

namespace common {

/////////////////////////////////////////////////////////////////////////////
// FileStaticMethodHandler staff

FileStaticMethodHandler::FileStaticMethodHandler() {}
FileStaticMethodHandler::~FileStaticMethodHandler() {}

/////////////////////////////////////////////////////////////////////////////
// FileStaticMock staff

FileStaticMock::FileStaticMock()
{
    m_old = FileMock2::SetStaticMethodHandler(this);
}

FileStaticMock::~FileStaticMock()
{
    // Restore previous static handler to support nested static mock
    FileMock2::SetStaticMethodHandler(m_old);
}

/////////////////////////////////////////////////////////////////////////////
// FileMock2 staff

const char FileMock2::kPrefix[] = FILE_MOCK_PREFIX;

FileStaticMethodHandler* FileMock2::s_static_methods = NULL;

FileMock2::FileMock2()
{
}

FileMock2::~FileMock2()
{
    m_mock.reset();
    FileMock2* mock_fs = static_cast<FileMock2*>(GetFileImplSingleton(FILE_MOCK_PREFIX));
    OpeningMocksMap& opening_mocks = mock_fs->m_opening_mocks;
    OpeningMocksMap::iterator it = opening_mocks.find(m_path);
    if (it != opening_mocks.end())
    {
        // Remove if is last reference of FileMock2Object
        if (opening_mocks[m_path].unique())
            opening_mocks.erase(it);
    }
}

FileStaticMethodHandler* FileMock2::SetStaticMethodHandler(
    FileStaticMethodHandler* new_handler)
{
    FileStaticMethodHandler* old = s_static_methods;
    s_static_methods = new_handler;
    return old;
}

FileMock2Object* FileMock2::FromFile(File* file, const char* filename, int line)
{
    FileMock2* mock = dynamic_cast<FileMock2*>(file); // NOLINT(runtime/rtti)
    CHECK(mock != NULL) << filename << ":" << line
        << ": First param of EXPECT_FILE_CALL is not a Mock File object";
    return mock->m_mock.get();
}

const FileMock2Object* FileMock2::FromFile(const File* file, const char* filename, int line)
{
    return FromFile(const_cast<File*>(file), filename, line);
}

int64_t FileMock2::Read(void* buffer, int64_t size, uint32_t* error_code)
{
    return m_mock->Read(buffer, size, error_code);
}

int64_t FileMock2::Write(const void* buffer, int64_t size, uint32_t* error_code)
{
    return m_mock->Write(buffer, size, error_code);
}

int32_t FileMock2::ReadLine(void* buffer, int32_t max_size)
{
    return m_mock->ReadLine(buffer, max_size);
}

int32_t FileMock2::Flush(uint32_t* error_code)
{
    return m_mock->Flush(error_code);
}

int32_t FileMock2::Close(uint32_t* error_code)
{
    return m_mock->Close(error_code);
}

int64_t FileMock2::Seek(int64_t offset, int32_t whence, uint32_t* error_code)
{
    return m_mock->Seek(offset, whence, error_code);
}

int64_t FileMock2::Tell(uint32_t* error_code)
{
    return m_mock->Tell(error_code);
}

bool FileMock2::SupportAsync()
{
    return m_mock->SupportAsync();
}

int32_t FileMock2::Truncate(uint64_t size, uint32_t* error_code)
{
    return m_mock->Truncate(size, error_code);
}

int32_t FileMock2::LocateData(uint64_t start,
                              uint64_t end,
                              std::vector<DataLocation>* buffer,
                              uint32_t* error_code)
{
    return m_mock->LocateData(start, end, buffer, error_code);
}

int32_t FileMock2::AsyncWrite(const void* buf,
                              int64_t buf_size,
                              Closure<void, int64_t, uint32_t>* callback,
                              uint32_t timeout,
                              uint32_t* error_code)
{
    return m_mock->AsyncWrite(buf, buf_size, callback, timeout, error_code);
}

int32_t FileMock2::AsyncReadFrom(void* buffer,
                                 int64_t size,
                                 int64_t start_position,
                                 Closure<void, int64_t, uint32_t>* callback,
                                 uint32_t  timeout,
                                 uint32_t* error_code)
{
    return m_mock->AsyncReadFrom(buffer, size, start_position,
                                 callback, timeout, error_code);
}

std::string FileMock2::GetFileImplName()
{
    return FILE_MOCK_PREFIX;
}

bool FileMock2::InitImpl()
{
    return true;
}

bool FileMock2::CleanupImpl()
{
    return true;
}

bool FileMock2::OpenImpl(const char *file_path, uint32_t flags,
                         const OpenFileOptions& options,
                         uint32_t *error_code)
{
    // Open is a special case, it allow be success even if not mocked.
    if (s_static_methods && !s_static_methods->Open(file_path, flags, options, error_code))
            return false;
    // If return mock Open return true, must bind with an opening_mocks entry.
    FileMock2* mock_fs = static_cast<FileMock2*>(GetFileImplSingleton(FILE_MOCK_PREFIX));
    m_path = file_path;

    // Bind with an FileMock2Object in opening_mocks.
    shared_ptr<FileMock2Object>& mock_obj = mock_fs->m_opening_mocks[file_path];
    if (!mock_obj)
        mock_obj.reset(new FileMock2Object());
    m_mock = mock_obj;
    return true;
}

#define CHECK_STATIC_MOCK() \
    CHECK(s_static_methods != NULL) << "FileStaticMock must be defined to test this method: " \
<< __PRETTY_FUNCTION__

int32_t FileMock2::MoveImpl(const char* src_name,
                            const char* dst_name, uint32_t* error_code)
{
    CHECK_STATIC_MOCK();
    return s_static_methods->Move(src_name, dst_name, error_code) ? 0 : -1;
}

int32_t FileMock2::RenameImpl(const char* old_path_name,
                              const char* new_path_name,
                              uint32_t*   error_code)
{
    CHECK_STATIC_MOCK();
    return s_static_methods->Rename(old_path_name, new_path_name, error_code) ? 0 : -1;
}

int32_t FileMock2::RemoveImpl(const char* path_name,
                              bool        is_recursive,
                              uint32_t*   error_code)
{
    CHECK_STATIC_MOCK();
    return s_static_methods->Remove(path_name, is_recursive, error_code) ? 0 : -1;
}

int32_t FileMock2::AddDirImpl(const char* path_name,
                              uint32_t* error_code)
{
    CHECK_STATIC_MOCK();
    return s_static_methods->AddDir(path_name, error_code) ? 0 : -1;
}

int32_t FileMock2::ListImpl(const char* pattern,
                            const ListOptions& list_option,
                            std::vector<AttrsInfo>* attrs,
                            uint32_t* error_code)
{
    CHECK_STATIC_MOCK();
    return s_static_methods->List(pattern, list_option, attrs, error_code) ? 0 : -1;
}

int64_t FileMock2::DuImpl(const char* path_name,
                          uint32_t* error_code)
{
    CHECK_STATIC_MOCK();
    return s_static_methods->Du(path_name, error_code);
}

bool FileMock2::GetContentSummaryImpl(
    const char* path_name,
    FileContentSummaryMask* mask,
    FileContentSummary* summary,
    uint32_t* error_code)
{
    CHECK_STATIC_MOCK();
    return s_static_methods->GetContentSummary(path_name, mask, summary, error_code);
}

int64_t FileMock2::GetSizeImpl(const char* file_name,
                               uint32_t*   error_code)
{
    CHECK_STATIC_MOCK();
    return s_static_methods->GetSize(file_name, error_code);
}

bool FileMock2::CheckExistImpl(const char* path_name,
                               uint32_t* error_code)
{
    CHECK_STATIC_MOCK();
    return s_static_methods->CheckExist(path_name, error_code);
}

bool FileMock2::ChmodImpl(const char* path_name,
                          const uint32_t permission,
                          uint32_t* error_code)
{
    CHECK_STATIC_MOCK();
    return s_static_methods->Chmod(path_name, permission, error_code);
}

bool FileMock2::ChangeRoleImpl(const char* path_name,
                               const char* role_name,
                               uint32_t* error_code)
{
    CHECK_STATIC_MOCK();
    return s_static_methods->ChangeRole(path_name, role_name, error_code);
}

bool FileMock2::ChangeSecondRoleImpl(const char* path_name,
                                     const char* second_role_name,
                                     bool is_recursive,
                                     uint32_t* error_code)
{
    CHECK_STATIC_MOCK();
    return s_static_methods->ChangeSecondRole(path_name, second_role_name,
                                              is_recursive, error_code);
}

bool FileMock2::GetDigestImpl(File* file_obj,
                              int64_t   file_size,
                              uint32_t* digest,
                              uint32_t* error_code)
{
    CHECK_STATIC_MOCK();
    return s_static_methods->GetDigest(file_obj, file_size, digest, error_code);
}

int32_t FileMock2::SnapshotImpl(const char* source_path,
                                const char* target_path,
                                uint32_t* error_code)
{
    CHECK_STATIC_MOCK();
    return s_static_methods->Snapshot(source_path, target_path, error_code) ? 0 : -1;
}

bool FileMock2::FreezeImpl(const std::string& file_name, uint32_t* error_code)
{
    CHECK_STATIC_MOCK();
    return s_static_methods->Freeze(file_name, error_code);
}

#undef CHECK_STATIC_MOCK

} // namespace common


