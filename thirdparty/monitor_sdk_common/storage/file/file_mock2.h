// Copyright (c) 2015, Vivo Inc.
// All rights reserved.
//
// Description: New File Mock base on gmock.

#ifndef COMMON_FILE_FILE_MOCK2_H
#define COMMON_FILE_FILE_MOCK2_H
#pragma once

#include <map>
#include <string>
#include <vector>

#include "thirdparty/monitor_sdk_common/base/smart_ptr/shared_ptr.h"
#include "thirdparty/monitor_sdk_common/base/uncopyable.h"
#include "thirdparty/monitor_sdk_common/storage/file/file2.h"
#include "thirdparty/gmock/gmock.h"

#define FILE_MOCK_PREFIX "/mock2/"

namespace common {

// Make static methods of File to be mockable.
class FileStaticMethodHandler {
    COMMON_DECLARE_UNCOPYABLE(FileStaticMethodHandler);

public:
    FileStaticMethodHandler();
    virtual ~FileStaticMethodHandler();
    virtual bool Open(const std::string&, uint32_t, const common::OpenFileOptions&, uint32_t*) = 0;
    virtual bool Move(const std::string&, const std::string&, uint32_t*) = 0;
    virtual bool Rename(const std::string&, const std::string&, uint32_t*) = 0;
    virtual bool Remove(const std::string&, bool, uint32_t*) = 0;
    virtual bool AddDir(const std::string&, uint32_t*) = 0;
    virtual bool List(const std::string&, const common::ListOptions&,
                      std::vector<common::AttrsInfo>*, uint32_t*) = 0;
    virtual int64_t Du(const std::string&, uint32_t*) = 0;
    virtual bool GetContentSummary(const std::string&, common::FileContentSummaryMask*,
                                   common::FileContentSummary*, uint32_t*) = 0;
    virtual int64_t GetSize(const std::string&, uint32_t*) = 0;
    virtual bool CheckExist(const std::string&, uint32_t*) = 0;
    virtual bool Chmod(const std::string&, uint32_t, uint32_t*) = 0;
    virtual bool ChangeRole(const std::string&, const std::string&, uint32_t*) = 0;
    virtual bool ChangeSecondRole(const std::string& path_name,
                                  const std::string& second_role_name,
                                  bool is_recursive,
                                  uint32_t* error_code) = 0;
    virtual bool GetDigest(common::File*, int64_t, uint32_t*, uint32_t*) = 0;
    virtual bool Snapshot(const std::string& source_path,
                          const std::string& target_path,
                          uint32_t* error_code) = 0;
    virtual bool Freeze(const std::string& file_name, uint32_t* error_code) = 0;
};

// Mock object to mock static method of file.
//
// Example:
//
// TEST(FileMock2, Move)
// {
//     FileStaticMock file_static_mock;
//     EXPECT_CALL(file_static_mock, Move(_, _, _))
//         .WillOnce(Return(0));
//     EXPECT_EQ(0, File::Move("/mock2/a", "/mock2/b"));
// }
//
// In the duration of file_static_mock object, all static method of File class
// are mocked by this object, you are able to custom the behavior by gmock.
//
class FileStaticMock : FileStaticMethodHandler {
public:
    FileStaticMock();
    ~FileStaticMock();
    MOCK_METHOD4(Open, bool(const std::string&, uint32_t, const common::OpenFileOptions&,
                            uint32_t*));
    MOCK_METHOD3(Move, bool(const std::string&, const std::string&, uint32_t*));
    MOCK_METHOD3(Rename, bool(const std::string&, const std::string&, uint32_t*));
    MOCK_METHOD3(Remove, bool(const std::string&, bool, uint32_t*));
    MOCK_METHOD2(AddDir, bool(const std::string&, uint32_t*));
    MOCK_METHOD4(List, bool(const std::string&, const common::ListOptions&,
                            std::vector<common::AttrsInfo>*, uint32_t*));
    MOCK_METHOD2(Du, int64_t(const std::string&, uint32_t*));
    MOCK_METHOD4(GetContentSummary, bool(const std::string&, common::FileContentSummaryMask*,
                                         common::FileContentSummary*, uint32_t*));
    MOCK_METHOD2(GetSize, int64_t(const std::string&, uint32_t*));
    MOCK_METHOD2(CheckExist, bool(const std::string&, uint32_t*));
    MOCK_METHOD3(Chmod, bool(const std::string&, uint32_t, uint32_t*));
    MOCK_METHOD3(ChangeRole, bool(const std::string&, const std::string&, uint32_t*));
    MOCK_METHOD4(ChangeSecondRole, bool(const std::string& path_name,
                                        const std::string& second_role_name,
                                        bool is_recursive,
                                        uint32_t* error_code));
    MOCK_METHOD4(GetDigest, bool(File*, int64_t, uint32_t*, uint32_t*));
    MOCK_METHOD3(Snapshot, bool(const std::string& source_path,
                                const std::string& target_path,
                                uint32_t* error_code));
    MOCK_METHOD2(Freeze, bool(const std::string& file_name, uint32_t* error_code));

private:
    FileStaticMethodHandler* m_old;
};

class FileMock2Object : public File
{
public:
    MOCK_METHOD3(Read, int64_t(void* buffer, int64_t size, uint32_t* error_code));
    MOCK_METHOD3(Write, int64_t(const void* buffer, int64_t size, uint32_t* error_code));
    MOCK_METHOD2(ReadLine, int32_t(void* buffer, int32_t max_size));
    MOCK_METHOD1(Flush, int32_t(uint32_t* error_code));
    MOCK_METHOD1(Close, int32_t(uint32_t* error_code));
    MOCK_METHOD3(Seek, int64_t(int64_t offset, int32_t whence, uint32_t* error_code));
    MOCK_METHOD1(Tell, int64_t(uint32_t* error_code));
    MOCK_METHOD0(SupportAsync, bool());
    MOCK_METHOD2(Truncate, int32_t(uint64_t, uint32_t*));
    MOCK_METHOD4(LocateData, int32_t(uint64_t start,
                                     uint64_t end,
                                     std::vector<DataLocation>* buffer,
                                     uint32_t* error_code));
    MOCK_METHOD5(AsyncWrite, int32_t(const void* buf,
                                     int64_t buf_size,
                                     Closure<void, int64_t, uint32_t>* callback,
                                     uint32_t timeout,
                                     uint32_t* error_code));
    MOCK_METHOD6(AsyncReadFrom, int32_t(void* buffer,
                                        int64_t size,
                                        int64_t start_position,
                                        Closure<void, int64_t, uint32_t>* callback,
                                        uint32_t  timeout,
                                        uint32_t* error_code));
    virtual std::string GetFileImplName() { return FILE_MOCK_PREFIX; }
    virtual bool InitImpl() { return true; }
    virtual bool CleanupImpl()  { return true; }
    virtual bool OpenImpl(const char *file_path, uint32_t flags,
                          const OpenFileOptions& options,
                          uint32_t *error_code)
    {
        return false;
    }
    virtual int32_t MoveImpl(const char* src_name,
                             const char* dst_name, uint32_t* error_code)
    {
        return 0;
    }
    virtual int32_t  RenameImpl(const char* old_path_name,
                                const char* new_path_name,
                                uint32_t*   error_code)
    {
        return 0;
    }
    virtual int32_t  RemoveImpl(const char* path_name,
                                bool        is_recursive,
                                uint32_t*   error_code)
    {
        return 0;
    }
    virtual int32_t  AddDirImpl(const char* path_name, uint32_t* error_code)
    {
        return 0;
    }
    virtual int32_t ListImpl(const char* pattern,
                             const ListOptions& list_option,
                             std::vector<AttrsInfo>* attrs,
                             uint32_t* error_code)
    {
        return 0;
    }
    virtual int64_t  DuImpl(const char* path_name,
                            uint32_t* error_code)
    {
        return 0;
    }
    virtual bool GetContentSummaryImpl(
        const char* path_name,
        FileContentSummaryMask* mask,
        FileContentSummary* summary,
        uint32_t* error_code)
    {
        return true;
    }
    virtual int64_t  GetSizeImpl(const char* file_name,
                                 uint32_t*   error_code)
    {
        return 0;
    }
    virtual bool CheckExistImpl(const char* path_name,
                                uint32_t* error_code)
    {
        return true;
    }
    virtual bool ChmodImpl(const char* path_name,
                           const uint32_t permission,
                           uint32_t* error_code)
    {
        return true;
    }
    virtual bool ChangeRoleImpl(const char* path_name,
                                const char* role_name,
                                uint32_t* error_code)
    {
        return true;
    }
    virtual bool ChangeSecondRoleImpl(const char* path_name,
                                      const char* second_role_name,
                                      bool is_recursive,
                                      uint32_t* error_code)
    {
        return true;
    }
    virtual bool GetDigestImpl(File* file_obj,
                               int64_t   file_size,
                               uint32_t* digest,
                               uint32_t* error_code)
    {
        return true;
    }
    virtual int32_t SnapshotImpl(const char* source_path,
                                 const char* target_path,
                                 uint32_t* error_code)
    {
        return 0;
    }
    virtual bool FreezeImpl(const std::string& file_name, uint32_t* error_code)
    {
        return true;
    }
};

// Implemantation detail: The actual mock File object.
class FileMock2 : public File
{
public:
    FileMock2();
    ~FileMock2();
    int64_t Read(void* buffer, int64_t size, uint32_t* error_code);
    int64_t Write(const void* buffer, int64_t size, uint32_t* error_code);
    int32_t ReadLine(void* buffer, int32_t max_size);
    int32_t Flush(uint32_t* error_code);
    int32_t Close(uint32_t* error_code);
    int64_t Seek(int64_t offset, int32_t whence, uint32_t* error_code);
    int64_t Tell(uint32_t* error_code);
    bool SupportAsync();
    int32_t Truncate(uint64_t, uint32_t*);
    int32_t LocateData(uint64_t start,
                       uint64_t end,
                       std::vector<DataLocation>* buffer,
                       uint32_t* error_code);
    int32_t AsyncWrite(const void* buf,
                       int64_t buf_size,
                       Closure<void, int64_t, uint32_t>* callback,
                       uint32_t time_out,
                       uint32_t* error_code);
    int32_t AsyncReadFrom(void* buffer,
                          int64_t size,
                          int64_t start_position,
                          Closure<void, int64_t, uint32_t>* callback,
                          uint32_t  timeout,
                          uint32_t* error_code);

    static FileStaticMethodHandler* SetStaticMethodHandler(FileStaticMethodHandler* new_handler);

    // NOTE: To implement EXPECT_FILE_CALL only.
    static const FileMock2Object* FromFile(const File* file, const char* filename, int line);
    static FileMock2Object* FromFile(File* file, const char* filename, int line);

    static const char kPrefix[];

private:
    std::string GetFileImplName();

    /////////////////////////////////////////////////////////////////////////
    // Forward all *Impl to registered FileStaticMethodHandler object

    virtual bool InitImpl();
    virtual bool CleanupImpl();
    virtual bool OpenImpl(const char *file_path, uint32_t flags,
                          const OpenFileOptions& options,
                          uint32_t *error_code);
    virtual int32_t MoveImpl(const char* src_name,
                             const char* dst_name, uint32_t* error_code);
    virtual int32_t  RenameImpl(const char* old_path_name,
                                const char* new_path_name,
                                uint32_t*   error_code);
    virtual int32_t  RemoveImpl(const char* path_name,
                                bool        is_recursive,
                                uint32_t*   error_code);
    virtual int32_t  AddDirImpl(const char* path_name,
                                uint32_t* error_code);
    virtual int32_t ListImpl(const char* pattern,
                             const ListOptions& list_option,
                             std::vector<AttrsInfo>* attrs,
                             uint32_t* error_code);
    virtual int64_t  DuImpl(const char* path_name,
                            uint32_t* error_code);
    virtual bool GetContentSummaryImpl(
        const char* path_name,
        FileContentSummaryMask* mask,
        FileContentSummary* summary,
        uint32_t* error_code);
    virtual int64_t  GetSizeImpl(const char* file_name,
                                 uint32_t*   error_code);
    virtual bool CheckExistImpl(const char* path_name,
                                uint32_t* error_code);
    virtual bool ChmodImpl(const char* path_name,
                           const uint32_t permission,
                           uint32_t* error_code);
    virtual bool ChangeRoleImpl(const char* path_name,
                                const char* role_name,
                                uint32_t* error_code);
    virtual bool ChangeSecondRoleImpl(const char* path_name,
                                      const char* second_role_name,
                                      bool is_recursive,
                                      uint32_t* error_code);
    virtual bool GetDigestImpl(File* file_obj,
                               int64_t   file_size,
                               uint32_t* digest,
                               uint32_t* error_code);
    virtual int32_t SnapshotImpl(const char* source_path,
                                 const char* target_path,
                                 uint32_t* error_code);
    virtual bool FreezeImpl(const std::string& file_name, uint32_t* error_code);

private:
    static FileStaticMethodHandler* s_static_methods;
    std::string m_path;
    // Enable same opened mock file reference same FileMock2Object, so we can
    // open a mock file and set it's behavior out of tested function.
    shared_ptr<FileMock2Object> m_mock;
    typedef std::map<std::string, shared_ptr<FileMock2Object> > OpeningMocksMap;
    OpeningMocksMap m_opening_mocks;
};

// Action FillArgBuffer<k>(data, length) copies the elements in
// source range [data, data+length) to the buffer pointed to by the k-th
// (0-based) argument.
ACTION_TEMPLATE(FillArgBuffer,
                HAS_1_TEMPLATE_PARAMS(int, k),
                AND_2_VALUE_PARAMS(data, length)) {
    memcpy(::std::tr1::get<k>(args), data, length);
}

} // namespace common

// Similar to EXPECT_CALL, but the first param file must be a mock file object.
#define FILE_EXPECT_CALL(file, call) \
    EXPECT_CALL(*::common::FileMock2::FromFile(&(file), __FILE__, __LINE__), \
                call)

#define FILE_ON_CALL(file, call) \
    ON_CALL(*::common::FileMock2::FromFile(&(file), __FILE__, __LINE__), \
            call)

#endif // COMMON_FILE_FILE_MOCK2_H
