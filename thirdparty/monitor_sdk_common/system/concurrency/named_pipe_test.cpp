//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-08-29 18:57
//  @file:      named_pipe_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/concurrency/named_pipe.h"
#include "thirdparty/gtest/gtest.h"

#ifndef _WIN32

namespace common {

TEST(NamedPipe, OpenOpenClose) {
    const char *pipe_name = "test_pipe";

    NamedPipe pipe_4_read;
    ASSERT_TRUE(pipe_4_read.Open(pipe_name, NamedPipe::kReadOnly, true));

    NamedPipe pipe_4_write;
    ASSERT_TRUE(pipe_4_write.Open(pipe_name, NamedPipe::kWriteOnly, true));

    pipe_4_read.Close();
    pipe_4_write.Close();

    ASSERT_FALSE(pipe_4_write.Open(pipe_name, NamedPipe::kWriteOnly, true));
}

TEST(NamedPipe, ReadWrite) {
    const char *pipe_name = "test_pipe";

    char read_buffer[1024];
    size_t read_len = 1024;

    NamedPipe pipe_4_read;
    ASSERT_TRUE(pipe_4_read.Open(pipe_name, NamedPipe::kReadOnly, true));

    NamedPipe pipe_4_write;
    ASSERT_TRUE(pipe_4_write.Open(pipe_name, NamedPipe::kWriteOnly, true));

    ASSERT_LE(pipe_4_read.Read(read_buffer, read_len), 0);

    ASSERT_GT(pipe_4_write.Write(pipe_name, strlen(pipe_name) + 1), 0);
    ASSERT_GT(pipe_4_read.Read(read_buffer, read_len), 0);

    ASSERT_STRCASEEQ(pipe_name, read_buffer);
}

} // namespace common

#endif
