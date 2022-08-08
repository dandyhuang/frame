// Copyright (c) 2015, Vivo Inc.
// All rights reserved.

#include "thirdparty/monitor_sdk_common/storage/file/file_mock2.h"

#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/gmock/gmock.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

using namespace testing;

TEST(MockFile2, Open)
{
    scoped_ptr<File> f1(File::Open(FILE_MOCK_PREFIX "test", "r"));
    FILE_EXPECT_CALL(*f1, Tell(_))
        .WillOnce(Return(1000))
        .WillRepeatedly(Return(-1));
    // On another file object with same path
    scoped_ptr<File> f2(File::Open(FILE_MOCK_PREFIX "test", "r"));
    EXPECT_EQ(1000, f2->Tell());
    EXPECT_EQ(-1, f2->Tell());
}

TEST(MockFile2, Unregister)
{
    {
        scoped_ptr<File> f1(File::Open(FILE_MOCK_PREFIX "test", "r"));
        FILE_EXPECT_CALL(*f1, Tell(_))
            .WillOnce(Return(1000))
            .WillRepeatedly(Return(-1));
        EXPECT_EQ(1000, f1->Tell());
        EXPECT_EQ(-1, f1->Tell());
    }
    scoped_ptr<File> f2(File::Open(FILE_MOCK_PREFIX "test", "r"));
    // The mock behavior set on f1 should not impact f2 because f1 has been
    // destructed.
    EXPECT_EQ(0, f2->Tell());
    FILE_EXPECT_CALL(*f2, Tell(_)).WillRepeatedly(Return(1));
    EXPECT_EQ(1, f2->Tell());
}

TEST(FileMock2, Read)
{
    scoped_ptr<File> file(File::Open(FILE_MOCK_PREFIX "test", "r"));

    FILE_EXPECT_CALL(*file, Read(_, _, _))
        .WillOnce(DoAll(FillArgBuffer<0>("hello", 5), Return(5)))
        .WillRepeatedly(Return(-1));

    char buffer[6] = {};
    EXPECT_EQ(5, file->Read(buffer, 5));
    EXPECT_STREQ("hello", buffer);
    EXPECT_EQ(-1, file->Read(buffer, 5));
}

TEST(FileMock2, Write)
{
    scoped_ptr<File> file(File::Open(FILE_MOCK_PREFIX "test", "w"));

    FILE_EXPECT_CALL(*file, Write(_, _, _))
        .WillOnce(Return(5))
        .WillRepeatedly(Return(-1));

    EXPECT_EQ(5, file->Write("hello", 5));
    EXPECT_EQ(-1, file->Write("hello", 5));
}

TEST(FileMock2, ReadLine)
{
    scoped_ptr<File> file(File::Open(FILE_MOCK_PREFIX "test", "r"));

    FILE_EXPECT_CALL(*file, Read(_, _, _))
        .WillOnce(DoAll(FillArgBuffer<0>("hello\n", 6), Return(6)))
        .WillRepeatedly(Return(-1));

    char buffer[7] = {};
    EXPECT_EQ(6, file->Read(buffer, 6));
    EXPECT_STREQ("hello\n", buffer);
    EXPECT_EQ(-1, file->Read(buffer, 6));
}

TEST(FileMock2, Flush)
{
    scoped_ptr<File> file(File::Open(FILE_MOCK_PREFIX "test", "w"));

    FILE_EXPECT_CALL(*file, Flush(_))
        .WillOnce(Return(0))
        .WillRepeatedly(Return(-1));

    EXPECT_EQ(0, file->Flush());
    EXPECT_EQ(-1, file->Flush());
}

TEST(FileMock2, Close)
{
    scoped_ptr<File> file(File::Open(FILE_MOCK_PREFIX "test", "w"));

    FILE_EXPECT_CALL(*file, Close(_))
        .WillOnce(Return(0))
        .WillRepeatedly(Return(-1));

    EXPECT_EQ(0, file->Close());
    EXPECT_EQ(-1, file->Close());
}

TEST(FileMock2, Seek)
{
    scoped_ptr<File> file(File::Open(FILE_MOCK_PREFIX "test", "w"));

    FILE_EXPECT_CALL(*file, Seek(_, _, _))
        .WillOnce(Return(1000))
        .WillRepeatedly(Return(-1));

    EXPECT_EQ(1000, file->Seek(SEEK_SET, 1000));
    EXPECT_EQ(-1, file->Seek(SEEK_CUR, 1000));
    EXPECT_EQ(-1, file->Seek(SEEK_END, -1000));
}

TEST(FileMock2, Tell)
{
    scoped_ptr<File> file(File::Open(FILE_MOCK_PREFIX "test", "w"));

    FILE_EXPECT_CALL(*file, Tell(_))
        .WillOnce(Return(1000))
        .WillRepeatedly(Return(-1));

    EXPECT_EQ(1000, file->Tell());
    EXPECT_EQ(-1, file->Tell());
    EXPECT_EQ(-1, file->Tell());
}

TEST(FileMock2, SupportAsync)
{
    scoped_ptr<File> file(File::Open(FILE_MOCK_PREFIX "test", "w"));

    FILE_EXPECT_CALL(*file, SupportAsync())
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));

    EXPECT_TRUE(file->SupportAsync());
    EXPECT_FALSE(file->SupportAsync());
    EXPECT_FALSE(file->SupportAsync());
}

TEST(FileMock2, Truncate)
{
    scoped_ptr<File> file(File::Open(FILE_MOCK_PREFIX "test", "w"));

    FILE_EXPECT_CALL(*file, Truncate(_, _))
        .WillOnce(Return(0))
        .WillRepeatedly(Return(-1));

    EXPECT_EQ(0, file->Truncate(0));
    EXPECT_EQ(-1, file->Truncate(0));
    EXPECT_EQ(-1, file->Truncate(0));
}

TEST(FileMock2, LocateData)
{
    scoped_ptr<File> file(File::Open(FILE_MOCK_PREFIX "test", "w"));

    FILE_EXPECT_CALL(*file, LocateData(_, _, _, _))
        .WillOnce(Return(0))
        .WillRepeatedly(Return(-1));

    std::vector<DataLocation> locations;
    EXPECT_EQ(0, file->LocateData(0, 100, &locations));
    EXPECT_EQ(-1, file->LocateData(0, 100, &locations));
    EXPECT_EQ(-1, file->LocateData(0, 100, &locations));
}

TEST(FileMock2, AsyncWrite)
{
    scoped_ptr<File> file(File::Open(FILE_MOCK_PREFIX "test", "w"));

    FILE_EXPECT_CALL(*file, AsyncWrite(_, _, _, _, _))
        .WillOnce(Return(0))
        .WillRepeatedly(Return(-1));

    EXPECT_EQ(0, file->AsyncWrite(NULL, 0, NULL, 0));
    EXPECT_EQ(-1, file->AsyncWrite(NULL, 0, NULL, 0));
    EXPECT_EQ(-1, file->AsyncWrite(NULL, 0, NULL, 0));
}

TEST(FileMock2, AsyncReadFrom)
{
    scoped_ptr<File> file(File::Open(FILE_MOCK_PREFIX "test", "w"));

    FILE_EXPECT_CALL(*file, AsyncReadFrom(_, _, _, _, _, _))
        .WillOnce(Return(0))
        .WillRepeatedly(Return(-1));

    EXPECT_EQ(0, file->AsyncReadFrom(NULL, 0, 0, NULL, 0));
    EXPECT_EQ(-1, file->AsyncReadFrom(NULL, 0, 0, NULL, 0));
    EXPECT_EQ(-1, file->AsyncReadFrom(NULL, 0, 0, NULL, 0));
}

/////////////////////////////////////////////////////////////////////////////
// Static mock tests.

TEST(FileMock2, Open)
{
    scoped_ptr<File> file(File::Open(FILE_MOCK_PREFIX "test", "r"));
    EXPECT_FALSE(!file);
}

TEST(FileMock2, OpenFailed)
{
    FileStaticMock file_static_mock;
    EXPECT_CALL(file_static_mock, Open(_, _, _, _))
        .WillOnce(Return(false))
        .WillRepeatedly(DoAll(SetArgPointee<3>(ERR_FILE_ENTRY_NOT_EXIST),
                              Return(false)));
    scoped_ptr<File> file(File::Open(FILE_MOCK_PREFIX "test", "r"));
    EXPECT_TRUE(!file);

    uint32_t error_code;
    file.reset(File::Open(FILE_MOCK_PREFIX "test", File::kOpenModeRead,
                          OpenFileOptions(), &error_code));
    EXPECT_TRUE(!file);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_ENTRY_NOT_EXIST), error_code);
}

TEST(FileMock2, Move)
{
    FileStaticMock file_static_mock;
    EXPECT_CALL(file_static_mock, Move(_, _, _))
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));
    EXPECT_EQ(0, File::Move(FILE_MOCK_PREFIX "a", FILE_MOCK_PREFIX "b"));
    EXPECT_EQ(-1, File::Move(FILE_MOCK_PREFIX "a", FILE_MOCK_PREFIX "b"));
    EXPECT_EQ(-1, File::Move(FILE_MOCK_PREFIX "a", FILE_MOCK_PREFIX "b"));
}

TEST(FileMock2, Rename)
{
    FileStaticMock file_static_mock;
    EXPECT_CALL(file_static_mock, Rename(_, _, _))
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));
    EXPECT_EQ(0, File::Rename(FILE_MOCK_PREFIX "a", FILE_MOCK_PREFIX "b"));
    EXPECT_EQ(-1, File::Rename(FILE_MOCK_PREFIX "a", FILE_MOCK_PREFIX "b"));
    EXPECT_EQ(-1, File::Rename(FILE_MOCK_PREFIX "a", FILE_MOCK_PREFIX "b"));
}

TEST(FileMock2, Remove)
{
    FileStaticMock file_static_mock;
    EXPECT_CALL(file_static_mock, Remove(_, _, _))
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));
    EXPECT_EQ(0, File::Remove(FILE_MOCK_PREFIX "test"));
    EXPECT_EQ(-1, File::Remove(FILE_MOCK_PREFIX "test"));
    EXPECT_EQ(-1, File::Remove(FILE_MOCK_PREFIX "test"));
}

TEST(FileMock2, AddDir)
{
    FileStaticMock file_static_mock;
    EXPECT_CALL(file_static_mock, AddDir(_, _))
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));
    EXPECT_EQ(0, File::AddDir(FILE_MOCK_PREFIX "test"));
    EXPECT_EQ(-1, File::AddDir(FILE_MOCK_PREFIX "test"));
    EXPECT_EQ(-1, File::AddDir(FILE_MOCK_PREFIX "test"));
}

TEST(FileMock2, List)
{
    FileStaticMock file_static_mock;
    EXPECT_CALL(file_static_mock, List(_, _, _, _))
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));

    std::vector<common::AttrsInfo> attrs;

    EXPECT_EQ(0, File::List(FILE_MOCK_PREFIX "test", ListOptions(), &attrs));
    EXPECT_EQ(-1, File::List(FILE_MOCK_PREFIX "test", ListOptions(), &attrs));
    EXPECT_EQ(-1, File::List(FILE_MOCK_PREFIX "test", ListOptions(), &attrs));
}

TEST(FileMock2, Du)
{
    FileStaticMock file_static_mock;
    EXPECT_CALL(file_static_mock, Du(_, _))
        .WillOnce(Return(1000))
        .WillRepeatedly(Return(-1));
    EXPECT_EQ(1000, File::Du(FILE_MOCK_PREFIX "test"));
    EXPECT_EQ(-1, File::Du(FILE_MOCK_PREFIX "test"));
    EXPECT_EQ(-1, File::Du(FILE_MOCK_PREFIX "test"));
}

TEST(FileMock2, GetContentSummary)
{
    FileStaticMock file_static_mock;
    EXPECT_CALL(file_static_mock, GetContentSummary(_, _, _, _))
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));

    FileContentSummaryMask mask;
    FileContentSummary summary;

    EXPECT_TRUE(File::GetContentSummary(FILE_MOCK_PREFIX "test", &mask, &summary));
    EXPECT_FALSE(File::GetContentSummary(FILE_MOCK_PREFIX "test", &mask, &summary));
    EXPECT_FALSE(File::GetContentSummary(FILE_MOCK_PREFIX "test", &mask, &summary));
}

TEST(FileMock2, GetSize)
{
    FileStaticMock file_static_mock;
    EXPECT_CALL(file_static_mock, GetSize(_, _))
        .WillOnce(Return(1000))
        .WillRepeatedly(Return(-1));
    EXPECT_EQ(1000, File::GetSize(FILE_MOCK_PREFIX "test"));
    EXPECT_EQ(-1, File::GetSize(FILE_MOCK_PREFIX "test"));
    EXPECT_EQ(-1, File::GetSize(FILE_MOCK_PREFIX "test"));
}

TEST(FileMock2, CheckExist)
{
    FileStaticMock file_static_mock;
    EXPECT_CALL(file_static_mock, CheckExist(_, _))
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));
    EXPECT_TRUE(File::CheckExist(FILE_MOCK_PREFIX "test"));
    EXPECT_FALSE(File::CheckExist(FILE_MOCK_PREFIX "test"));
    EXPECT_FALSE(File::CheckExist(FILE_MOCK_PREFIX "test"));
}

TEST(FileMock2, Chmod)
{
    FileStaticMock file_static_mock;
    EXPECT_CALL(file_static_mock, Chmod(_, _, _))
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));
    EXPECT_TRUE(File::Chmod(FILE_MOCK_PREFIX "test", 0755));
    EXPECT_FALSE(File::Chmod(FILE_MOCK_PREFIX "test", 0755));
    EXPECT_FALSE(File::Chmod(FILE_MOCK_PREFIX "test", 0755));
}

TEST(FileMock2, ChangeRole)
{
    FileStaticMock file_static_mock;
    EXPECT_CALL(file_static_mock, ChangeRole(_, _, _))
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));
    EXPECT_TRUE(File::ChangeRole(FILE_MOCK_PREFIX "test", "common"));
    EXPECT_FALSE(File::ChangeRole(FILE_MOCK_PREFIX "test", "common"));
    EXPECT_FALSE(File::ChangeRole(FILE_MOCK_PREFIX "test", "common"));
}

TEST(FileMock2, ChangeSecondRole)
{
    FileStaticMock file_static_mock;
    EXPECT_CALL(file_static_mock, ChangeSecondRole(_, _, _, _))
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));
    EXPECT_TRUE(File::ChangeSecondRole(FILE_MOCK_PREFIX "test", "common"));
    EXPECT_FALSE(File::ChangeSecondRole(FILE_MOCK_PREFIX "test", "common"));
    EXPECT_FALSE(File::ChangeSecondRole(FILE_MOCK_PREFIX "test", "common"));
}

TEST(FileMock2, GetDigest)
{
    FileStaticMock file_static_mock;
    EXPECT_CALL(file_static_mock, GetSize(_, _)).WillRepeatedly(Return(1000));
    EXPECT_CALL(file_static_mock, Open(_, _, _, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(file_static_mock, GetDigest(_, _, _, _))
        .WillOnce(DoAll(SetArgPointee<2>(0x19790626U), Return(true)))
        .WillRepeatedly(Return(false));
    uint32_t digest;
    EXPECT_TRUE(File::GetDigest(FILE_MOCK_PREFIX "test", &digest));
    EXPECT_FALSE(File::GetDigest(FILE_MOCK_PREFIX "test", &digest));
    EXPECT_FALSE(File::GetDigest(FILE_MOCK_PREFIX "test", &digest));
}

TEST(FileMock2, Snapshot)
{
    FileStaticMock file_static_mock;
    EXPECT_CALL(file_static_mock, Snapshot(_, _, _))
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));
    EXPECT_EQ(0, File::Snapshot(FILE_MOCK_PREFIX "a", "mock2/b"));
    EXPECT_EQ(-1, File::Snapshot(FILE_MOCK_PREFIX "a", "mock2/b"));
    EXPECT_EQ(-1, File::Snapshot(FILE_MOCK_PREFIX "a", "mock2/b"));
}

TEST(FileMock2, Freeze)
{
    FileStaticMock file_static_mock;
    EXPECT_CALL(file_static_mock, Freeze(_, _))
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));
    EXPECT_TRUE(File::Freeze(FILE_MOCK_PREFIX "test"));
    EXPECT_FALSE(File::Freeze(FILE_MOCK_PREFIX "test"));
    EXPECT_FALSE(File::Freeze(FILE_MOCK_PREFIX "test"));
}

/////////////////////////////////////////////////////////////////////////////
// Put all static mock death test here.

// Any mock method should die without a FileStaticMock object.
TEST(FileMock2, StaticDeathTest)
{
    testing::FLAGS_gtest_death_test_style = "threadsafe";

    EXPECT_DEATH(File::Move(FILE_MOCK_PREFIX "a", FILE_MOCK_PREFIX "b"),
                 "FileStaticMock must be defined");
    EXPECT_DEATH(File::Rename(FILE_MOCK_PREFIX "a", FILE_MOCK_PREFIX "b"),
                 "FileStaticMock must be defined");
    EXPECT_DEATH(File::Remove(FILE_MOCK_PREFIX "test"), "FileStaticMock must be defined");
    EXPECT_DEATH(File::AddDir(FILE_MOCK_PREFIX "test"), "FileStaticMock must be defined");

    std::vector<AttrsInfo> attrs;
    EXPECT_DEATH(File::List(FILE_MOCK_PREFIX "test", ListOptions(), &attrs),
                 "FileStaticMock must be defined");

    EXPECT_DEATH(File::Du(FILE_MOCK_PREFIX "test"), "FileStaticMock must be defined");

    FileContentSummaryMask mask;
    FileContentSummary summary;
    EXPECT_DEATH(File::GetContentSummary(FILE_MOCK_PREFIX "test", &mask, &summary),
                 "FileStaticMock must be defined");

    EXPECT_DEATH(File::GetSize(FILE_MOCK_PREFIX "test"), "FileStaticMock must be defined");
    EXPECT_DEATH(File::CheckExist(FILE_MOCK_PREFIX "test"), "FileStaticMock must be defined");
    EXPECT_DEATH(File::Chmod(FILE_MOCK_PREFIX "test", 0755), "FileStaticMock must be defined");
    EXPECT_DEATH(File::ChangeRole(FILE_MOCK_PREFIX "test", "common"),
                 "FileStaticMock must be defined");
    EXPECT_DEATH(File::ChangeSecondRole(FILE_MOCK_PREFIX "test", "common", true),
                 "FileStaticMock must be defined");

    uint32_t digest;
    EXPECT_DEATH(File::GetDigest(FILE_MOCK_PREFIX "test", &digest),
                 "FileStaticMock must be defined");

    EXPECT_DEATH(File::Snapshot(FILE_MOCK_PREFIX "a", FILE_MOCK_PREFIX "b"),
                 "FileStaticMock must be defined");
    EXPECT_DEATH(File::Freeze(FILE_MOCK_PREFIX "test"), "FileStaticMock must be defined");
}

TEST(FileMock2, NestedStaticMock)
{
    FileStaticMock file_static_mock1;
    EXPECT_CALL(file_static_mock1, GetSize(_, _)).WillRepeatedly(Return(1));
    EXPECT_EQ(1, File::GetSize(FILE_MOCK_PREFIX "test"));

    {
        FileStaticMock file_static_mock2;
        EXPECT_CALL(file_static_mock2, GetSize(_, _)).WillRepeatedly(Return(2));
        EXPECT_EQ(2, File::GetSize(FILE_MOCK_PREFIX "test"));
    }

    EXPECT_EQ(1, File::GetSize(FILE_MOCK_PREFIX "test"));
}

} // namespace common
