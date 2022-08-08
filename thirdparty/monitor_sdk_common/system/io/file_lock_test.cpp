//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 10:14
//  @file:      file_lock_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/io/file_lock.h"
#include <stdio.h>
#include <assert.h>
#include "thirdparty/monitor_sdk_common/system/io/file.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

class FileLockTest : public::testing::Test {
public:
    virtual void SetUp()
    {
        io::file::Touch("test.dat");
        io::file::Delete("test1.dat");
    }

    virtual void TearDown()
    {
        io::file::Delete("test.dat");
        io::file::Delete("test1.dat");
    }
};

TEST_F(FileLockTest, OpenExisted)
{
    FileLock lock;
    // 打开已存在的文件
    ASSERT_TRUE(lock.Open("test.dat"));
}

TEST_F(FileLockTest, Open)
{
    FileLock lock;
    // 打开不存在的文件
    ASSERT_TRUE(lock.Open("test1.dat"));
}

TEST_F(FileLockTest, Close)
{
    FileLock lock;
    ASSERT_TRUE(lock.Open("test.dat"));
    ASSERT_TRUE(lock.Close());
}

TEST_F(FileLockTest, ReaderLock)
{
    FileLock lock;
    EXPECT_TRUE(lock.Open("test.dat"));
    lock.ReaderLock();
    EXPECT_TRUE(lock.TryReaderLock());

    int pid = fork();
    if (pid == 0) {
        sleep(1);
        // 子进程也可以得到读锁
        lock.ReaderLock();
        EXPECT_TRUE(lock.TryReaderLock());
        // 但是不能得到写锁
        EXPECT_FALSE(lock.TryWriterLock());
    } else if (pid > 0) {
        waitpid(pid, NULL, 0);
        lock.WriterLock();
        EXPECT_TRUE(lock.TryWriterLock());
        EXPECT_TRUE(lock.TryWriterLock());
        lock.WriterLock();
    }
}

TEST_F(FileLockTest, WriterLock)
{
    FileLock lock;
    EXPECT_TRUE(lock.Open("test.dat"));

    lock.WriterLock();
    EXPECT_TRUE(lock.TryWriterLock());

    int pid = fork();
    if (pid == 0) {
        sleep(1);
        // 子进程，不可获得写锁读锁
        EXPECT_FALSE(lock.TryWriterLock());
        EXPECT_FALSE(lock.TryReaderLock());
    } else if (pid > 0) {
        waitpid(pid, NULL, 0);
        lock.WriterLock();
        // 本进程，可以获得读锁和写锁
        EXPECT_TRUE(lock.TryWriterLock());
        EXPECT_TRUE(lock.TryReaderLock());
    }
}

TEST_F(FileLockTest, ScopedLocker)
{
    FileLock lock;
    ASSERT_TRUE(lock.Open("test.dat"));

    {
        FileLock::ReaderLocker l(lock);
    }

    {
        FileLock::WriterLocker l(lock);
    }

    {
        FileLock::TryReaderLocker l(lock);
        EXPECT_TRUE(l.IsLocked());
    }

    {
        FileLock::TryWriterLocker l(lock);
        EXPECT_TRUE(l.IsLocked());
    }
}

TEST_F(FileLockTest, IsLockedTest) {
    FileLock lock;
    EXPECT_TRUE(lock.Open("test1.dat"));

    lock.WriterLock();
    EXPECT_TRUE(lock.TryWriterLock());
    int ppid = getpid();
    int pid = fork();
    if (pid == 0) {
        sleep(1);
        pid_t owner_pid = 0;
        FileLock lock2;
        EXPECT_TRUE(lock2.Open("test1.dat"));
        EXPECT_TRUE(lock2.IsLocked(&owner_pid));
        EXPECT_EQ(ppid, owner_pid);
    } else if (pid > 0) {
        waitpid(pid, NULL, 0);
    }
}

} // end of namespace common
