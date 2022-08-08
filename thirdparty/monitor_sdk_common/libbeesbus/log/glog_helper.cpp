//
// Created by 杨民善 on 2020/6/18.
//

#include "glog_helper.h"

namespace vivo_bees_bus_cpp_report
{
    GlogHelper::GlogHelper()
    {}

    GlogHelper::~GlogHelper()
    {
        destroy();
    }

    void GlogHelper::signalHandle(const char* data, int size)
    {
        std::string glog(data, size);
        LOG(ERROR) << glog;
    }

    void GlogHelper::init(char* program, const int &logLevel)
    {
        char buf[256] = {0};
        getcwd(buf, sizeof(buf));
        std::string path{std::string("mkdir -p ") + std::string(buf)};
        path += std::string("/") + GLOG_DIR;
        system(path.c_str());
        google::InitGoogleLogging(program);

        setGlogLevel(logLevel);

        FLAGS_colorlogtostderr = true;                                       // 设置输出到屏幕的日志显示相应颜色
        FLAGS_logbufsecs = 0;                                                // 缓冲日志输出，默认为30秒，此处改为立即输出
        FLAGS_max_log_size = 100;                                            // 最大日志大小为 100MB
        FLAGS_stop_logging_if_full_disk = true;                              // 当磁盘被写满时，停止日志输出
        google::SetLogFilenameExtension("BEES_BUS_CPP_SDK_");                // 设置文件名扩展，如平台？或其它需要区分的信息
        google::InstallFailureSignalHandler();                               // 捕捉 core dumped
        google::InstallFailureWriter(&GlogHelper::signalHandle);             // 默认捕捉 SIGSEGV 信号信息输出会输出到 stderr，可以通过下面的方法自定义输出>方式：
    }

    void GlogHelper::setGlogLevel(const int &logLevel)
    {
        switch (logLevel)
        {
            case VIVO_BEES_GLOG_LEVEL_INFO:
                google::SetLogDestination(google::INFO, GLOG_DIR"/INFO_");            // 设置 google::INFO 级别的日志存储路径和文件名前缀
                google::SetStderrLogging(google::INFO);                               // 设置级别高于 google::INFO 的日志同时输出到屏幕
                break;
            case VIVO_BEES_GLOG_LEVEL_WARNING:
                google::SetLogDestination(google::WARNING, GLOG_DIR"/WARNING_");      // 设置 google::WARNING 级别的日志存储路径和文件名前缀
                google::SetStderrLogging(google::WARNING);                            // 设置级别高于 google::WARNING 的日志同时输出到屏幕
                break;
            case VIVO_BEES_GLOG_LEVEL_ERROR:
                google::SetLogDestination(google::ERROR, GLOG_DIR"/ERROR_");          // 设置 google::ERROR 级别的日志存储路径和文件名前缀
                google::SetStderrLogging(google::ERROR);                              // 设置级别高于 google::ERROR 的日志同时输出到屏幕
                break;
            case VIVO_BEES_GLOG_LEVEL_FATAL:
                google::SetLogDestination(google::FATAL, GLOG_DIR"/FATAL_");          // 设置 google::FATAL 级别的日志存储路径和文件名前缀
                google::SetStderrLogging(google::FATAL);                              // 设置级别高于 google::FATAL 的日志同时输出到屏幕
                break;
            default:
                google::SetLogDestination(google::ERROR, GLOG_DIR"/ERROR_");          // 设置 google::ERROR 级别的日志存储路径和文件名前缀
                google::SetStderrLogging(google::ERROR);                              // 设置级别高于 google::ERROR 的日志同时输出到屏幕
                break;
        }
    }

    void GlogHelper::destroy()
    {
        google::ShutdownGoogleLogging();
    }
}