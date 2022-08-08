// Copyright (c) 2015, Vivo Inc. All rights reserved.
// Description: Test aio operations.

#ifndef Win32

#include <fcntl.h>

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

#include "thirdparty/monitor_sdk_common/storage/file/aioframe/aioframe.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/event.h"

using namespace std;

namespace common {

const char* g_test_text = "a test data file.\n";

void ReadCallback(AutoResetEvent* event, char *buffer, int64_t size, uint32_t error_code) {
    EXPECT_EQ(static_cast<int64_t>(strlen(g_test_text)), size);
    LOG(INFO) << "read size:" << size;
    EXPECT_EQ(0u, error_code);
    LOG(INFO) << "error code:" << error_code;
    LOG(INFO) << "read content:" << buffer;

    EXPECT_STREQ(g_test_text, buffer);

    delete []buffer;
    // notify others
    event->Set();
}

void WriteCallback(AutoResetEvent* event, int64_t size, uint32_t error_code) {
    EXPECT_EQ(static_cast<int64_t>(strlen(g_test_text)), size);
    LOG(INFO) << "write size:" << size;
    EXPECT_EQ(0u, error_code);
    LOG(INFO) << "error code:" << error_code;

    event->Set();
}

class AIOFrameTest : public testing::Test {
public:
    AIOFrameTest() : m_aioframe(NULL), m_fd(-1), m_length(0) {}

    void SetUp() {
        LOG(INFO) << "setup env for every test to make sure no resource leak";
        m_aioframe = new common::AIOFrame();

        m_fd = open("url.dat", O_RDWR | O_CREAT, 0755);
        ASSERT_GE(m_fd, 0);
        m_length = strlen(g_test_text);
    }

    void TearDown() {
        LOG(INFO) << "teardown env";

        close(m_fd);

        delete m_aioframe;
        m_aioframe = NULL;
    }

public:
    common::AIOFrame* m_aioframe;
    int m_fd;
    int64_t m_length;
};


TEST_F(AIOFrameTest, AsyncWriteTest) {
    AutoResetEvent event;

    Closure<void, int64_t, uint32_t>* callback = NewClosure(WriteCallback, &event);

    EXPECT_EQ(true, m_aioframe->AsyncWrite(m_fd, g_test_text, m_length, 0, callback, NULL));

    EXPECT_TRUE(event.Wait(500)); // ms. Must be notifyed success

    LOG(INFO) << "Async read callback OK, so main thread ongoing";
}

TEST_F(AIOFrameTest, AsyncReadTest) {
    AutoResetEvent event;

    char *buffer = new char[m_length + 1];
    buffer[m_length] = '\0';

    Closure<void, int64_t, uint32_t>* callback = NewClosure(ReadCallback, &event, buffer);

    EXPECT_EQ(true, m_aioframe->AsyncRead(m_fd, buffer, m_length, 0, callback, NULL));

    EXPECT_TRUE(event.Wait(500)); // ms. Must be notifyed success

    LOG(INFO) << "Async read callback OK, so main thread ongoing";
}

TEST_F(AIOFrameTest, ReadWriteTest) {
    std::string text(g_test_text);
    text += text;

    int64_t expect_size = static_cast<int64_t>(text.size());
    AIOFrame::StatusCode code;
    int64_t real_size = 0;
    real_size = m_aioframe->Write(m_fd, text.c_str(), expect_size, 0, &code);
    EXPECT_EQ(expect_size, real_size);
    EXPECT_EQ(code, 0);

    LOG(INFO) << "after write, read again to check result.";
    scoped_array<char> buffer(new char[expect_size + 1]);
    buffer[expect_size] = '\0';

    real_size = m_aioframe->Read(m_fd, buffer.get(), expect_size, 0, &code);
    EXPECT_EQ(expect_size, real_size);
    EXPECT_EQ(code, 0);
    EXPECT_EQ(text, buffer.get());
}

} // namespace common
#endif
