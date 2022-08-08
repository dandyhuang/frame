// Copyright (c) 2015, Vivo Inc. All rights reserved.
/**
 * @file abstract_stream_file_test.cpp
 * @brief
 * @author chenzheng05@baidu.com
 * @date 2015-01-20
 */

// An test example to show usage of AbstractStreamFile.

#include "thirdparty/monitor_sdk_common/storage/file/abstract_stream_file.h"
#include "thirdparty/monitor_sdk_common/base/module.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

// a AbstractStreamFile subclass, only implement necessary stream file operations.
class LogFatalStreamFile : public AbstractStreamFile {
public:
    virtual int64_t Read(void*     buffer,
                         int64_t   size,
                         uint32_t* error_code = NULL) {
        return -1;
    }

    virtual int32_t ReadLine(void* buffer, int32_t max_size) {
        return -1;
    }

    virtual int32_t AsyncReadFrom(void* buffer,
                                  int64_t size,
                                  int64_t start_position,
                                  Closure<void, int64_t, uint32_t>* callback,
                                  uint32_t  timeout = kDefaultAsyncTimeout,
                                  uint32_t* error_code = NULL) {
        return -1;
    }

    virtual int64_t Write(const void* buffer,
                          int64_t     size,
                          uint32_t*   error_code = NULL) {
        return -1;
    }

    virtual int32_t AsyncWrite(const void* buffer,
                               int64_t     size,
                               Closure<void, int64_t, uint32_t>* callback,
                               uint32_t    timeout = kDefaultAsyncTimeout,
                               uint32_t*   error_code = NULL) {
        return -1;
    }

    virtual bool SupportAsync() { return false; }

    virtual int32_t Flush(uint32_t* error_code = NULL) { return -1; }

    virtual int32_t Close(uint32_t* error_code = NULL) { return -1; }

    virtual int64_t Seek(int64_t   offset,
                         int32_t   whence,
                         uint32_t* error_code = NULL) {
        return -1;
    }

    virtual int64_t Tell(uint32_t* error_code = NULL) {
        return -1;
    }

    virtual int32_t Truncate(uint64_t length, uint32_t* error_code = NULL) {
        return -1;
    }

    virtual int32_t LocateData(uint64_t      start,
                               uint64_t      end,
                               std::vector<DataLocation>* buffer,
                               uint32_t*     error_code = NULL) {
        return -1;
    }
};

class AbstractStreamFileTest : public testing::Test {
};

TEST_F(AbstractStreamFileTest, EmptyClassTest) {
    LogFatalStreamFile stream_file;

    LOG(INFO) << "Compile OK is enough for LogFatalStreamFile";
}

} // namespace common

