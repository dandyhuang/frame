//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:06
//  @file:      process_test.cpp
//  @author:
//  @brief:
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/process.h"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

#include "thirdparty/monitor_sdk_common/system/concurrency/this_thread.h"

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/monitor_sdk_common/system/time/timestamp.h"
#include "thirdparty/monitor_sdk_common/system/net/ip_address.h"

DEFINE_bool(test_mode, true, "run as gtest or normal process");
DEFINE_int32(loop_count, 1, "run loop count");

namespace common {

bool g_quit = false;

static void SignalIntHandler(int)
{
    g_quit = true;
}

class ProcessTest : public testing::Test
{
protected:
    virtual void SetUp()
    {
        m_exe_path = ThisProcess::BinaryPath();
        m_exe_args = "--test_mode=false";
    }

    virtual void TearDown()
    {
    }

    std::string m_exe_path;
    std::string m_exe_args;
    Process m_process;
};

TEST_F(ProcessTest, BinaryPath)
{
    std::cout << ThisProcess::BinaryPath() << "\n";
}

TEST_F(ProcessTest, BinaryName)
{
    std::cout << ThisProcess::BinaryName() << "\n";
}

TEST_F(ProcessTest, BinaryDirectory)
{
    std::cout << ThisProcess::BinaryDirectory() << "\n";
}

TEST_F(ProcessTest, StartTime)
{
    std::cout << ThisProcess::StartTime() << "\n";
}

TEST_F(ProcessTest, ElapsedTime)
{
    sleep(5);
    std::cout << ThisProcess::ElapsedTime() << "\n";
}

TEST_F(ProcessTest, StartAndTerminate)
{
    m_process = Process::Start(m_exe_path, m_exe_args);
    EXPECT_GT(m_process.GetId(), 0);
    EXPECT_TRUE(m_process.IsValid());
    EXPECT_TRUE(m_process.Terminate());
    EXPECT_FALSE(m_process.IsValid());
    m_exe_args == "--test_mode=false --loop_count=6";
    m_process = Process::Start(m_exe_path, m_exe_args);
    EXPECT_GT(m_process.GetId(), 0);
    EXPECT_TRUE(m_process.Terminate());
    EXPECT_FALSE(m_process.IsValid());

    m_process = Process::Start("abc", m_exe_args);
    EXPECT_GT(m_process.GetId(), 0);
    EXPECT_TRUE(m_process.IsValid());
    int exit_code = -1;
    EXPECT_TRUE(m_process.WaitForExit(&exit_code));
    EXPECT_EQ(127, exit_code);
}

TEST_F(ProcessTest, WaitForExit)
{
    m_process = Process::Start(m_exe_path, m_exe_args);
    EXPECT_GT(m_process.GetId(), 0);
    EXPECT_TRUE(m_process.IsValid());
    int exit_code = -1;
    EXPECT_TRUE(m_process.WaitForExit(&exit_code));
    EXPECT_EQ(0, exit_code);
    EXPECT_FALSE(m_process.IsValid());
}

TEST_F(ProcessTest, TimedWaitForExit)
{
    m_process = Process::Start(m_exe_path, m_exe_args);
    EXPECT_GT(m_process.GetId(), 0);
    EXPECT_TRUE(m_process.IsValid());
    int exit_code = -1;
    int64_t timeout_in_ms = 6000;
    EXPECT_TRUE(m_process.TimedWaitForExit(timeout_in_ms, &exit_code));
    EXPECT_EQ(0, exit_code);
    EXPECT_FALSE(m_process.IsValid());

    m_process = Process::Start(m_exe_path, m_exe_args);
    EXPECT_GT(m_process.GetId(), 0);
    EXPECT_TRUE(m_process.IsValid());
    timeout_in_ms = 100;
    EXPECT_FALSE(m_process.TimedWaitForExit(timeout_in_ms, &exit_code));
    EXPECT_EQ(-1, exit_code);
    EXPECT_TRUE(m_process.IsValid());
}

TEST_F(ProcessTest, CurrentProcess)
{
    m_process = Process::GetCurrentProcess();
    EXPECT_TRUE(m_process.IsCurrent());
}

TEST_F(ProcessTest, ProcBinaryPath)
{
    Process t_process(ThisProcess::GetId());
    EXPECT_EQ(ThisProcess::BinaryPath(), t_process.BinaryPath());
}

TEST_F(ProcessTest, ProcBinaryDirectory)
{
    Process t_process(ThisProcess::GetId());
    EXPECT_EQ(ThisProcess::BinaryDirectory(), t_process.BinaryDirectory());
}

TEST_F(ProcessTest, ProcBinaryName)
{
    Process t_process(ThisProcess::GetId());
    EXPECT_EQ(ThisProcess::BinaryName(), t_process.BinaryName());
}

TEST_F(ProcessTest, ProcStartTime)
{
    Process t_process(ThisProcess::GetId());
    EXPECT_EQ(ThisProcess::StartTime(), t_process.StartTime());
}

TEST_F(ProcessTest, ProcStartTimeDiff)
{
    Process t_process(ThisProcess::GetId());
    EXPECT_EQ(ThisProcess::StartTimeDiff(), t_process.StartTimeDiff());
}

TEST_F(ProcessTest, ProcElapsedTime)
{
    Process t_process(ThisProcess::GetId());
    EXPECT_EQ(ThisProcess::ElapsedTime(), t_process.ElapsedTime());
}

TEST_F(ProcessTest, ProcStat)
{
    Process t_process(ThisProcess::GetId());
    EXPECT_EQ('R', t_process.Stat());
}

TEST_F(ProcessTest, ProcStatNullTest)
{
    Process t_process(40000);
    EXPECT_EQ('G', t_process.Stat());
}

TEST_F(ProcessTest, ProcParentIdTest)
{
    Process t_process(ThisProcess::GetId());
    EXPECT_EQ(getppid(), t_process.GetParentId());
}

TEST_F(ProcessTest, ProcParentIdNullTest)
{
    Process t_process(40000);
    EXPECT_EQ(-1, t_process.GetParentId());
}

TEST_F(ProcessTest, ProcUidTest)
{
    Process t_process(ThisProcess::GetId());
    uid_t real_uid, effective_uid;
    EXPECT_TRUE(t_process.GetRealUid(&real_uid));
    EXPECT_TRUE(t_process.GetEffectiveUid(&effective_uid));
    EXPECT_EQ(getuid(), real_uid);
    EXPECT_EQ(geteuid(), effective_uid);
}

TEST_F(ProcessTest, HasPortTest)
{
    int sock_fd;
    ::sockaddr_in my_addr;
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(-1, sock_fd);
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(64443);
    my_addr.sin_addr.s_addr = IpAddress("127.0.0.1").ToInt();
    bzero(&(my_addr.sin_zero), 8);
    ASSERT_NE(-1, bind(sock_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)));
    LOG(ERROR) << "errno:" << errno;
    ASSERT_NE(-1, listen(sock_fd, 1));
    int arg = -1;
    ASSERT_NE(-1, fcntl(sock_fd, F_GETFD, arg));
    arg |= FD_CLOEXEC;
    ASSERT_NE(-1, fcntl(sock_fd, F_SETFD, arg));

    Process t_process(ThisProcess::GetId());
    EXPECT_TRUE(t_process.HasPort(IpAddress("127.0.0.1"), 64443));
    EXPECT_FALSE(t_process.HasPort(IpAddress("127.0.0.1"), static_cast<uint16_t>(65536)));

    Process t_process2 = Process::Start("sleep", "1s");
    EXPECT_NE(ThisProcess::GetId(), t_process2.GetId());

    EXPECT_FALSE(t_process2.HasPort(IpAddress("127.0.0.1"), 64443));
    int proc2_exit_code;
    EXPECT_TRUE(t_process2.WaitForExit(&proc2_exit_code));
    //int64_t start_time_stamp = GetTimeStampInUs();
    //for (int i = 0; i < 5000; i++) {
    //    t_process.HasPort(IpAddress("127.0.0.1"), 64443);
    //}
    //int64_t end_time_stamp = GetTimeStampInUs();

    //LOG(ERROR) << "Hasport total 5000 times, elapsed time " << end_time_stamp - start_time_stamp << " us avg: " << (end_time_stamp - start_time_stamp) / 5000 << " us";

    close(sock_fd);
}

TEST_F(ProcessTest, GetSessionIdTest){
    Process t_process(ThisProcess::GetId());
    EXPECT_EQ(getsid(0), t_process.GetSessionId());
}

} // end of namespace common

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    google::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);
    if (FLAGS_test_mode) {
        return RUN_ALL_TESTS();
    } else {
        signal(SIGINT, common::SignalIntHandler);
        signal(SIGTERM, common::SignalIntHandler);

        int count = 0;
        while (!common::g_quit && count < FLAGS_loop_count) {
            common::ThisThread::Sleep(500);
            ++count;
        }
        return EXIT_SUCCESS;
    }
}

