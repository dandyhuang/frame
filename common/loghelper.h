#pragma once


#include <gflags/gflags.h>
#include <glog/logging.h>
#include <stdio.h>
#include <time.h>

#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>
#include <chrono>
#include <condition_variable>
#include <list>
#include <map>
#include <mutex>
#include <string>
#include <thread>

namespace vlog {

using namespace std;
using namespace google;

struct LogMessage {
  string _log_msg;
  struct ::tm _tm;
  int64_t _time_ms;
};

class DayLogSink : public google::LogSink {
 public:
  explicit DayLogSink(const string& strFileName, const string& strExt, int max_file_num = 5);

  virtual ~DayLogSink();

  virtual void send(LogSeverity severity, const char* full_filename, const char* base_filename,
                    int line, const struct ::tm* tm_time, const char* message, size_t message_len);

  virtual void send(LogSeverity severity, const char* full_filename, const char* base_filename,
                    int line, const struct ::tm* tm_time, const char* message, size_t message_len,
                    int32 usecs);

  void setMaxFileSize(long maxFileSize);

  void setMaxFileNum(int fileNum);

  void Terminate();

  bool isNeedLog(LogSeverity severity) { return severity >= _log_level; }

  void setLogLevel(LogSeverity severity) { _log_level = severity; }

 private:
  void PushData(const LogMessage& logMsg);

  void AsyncWriteData();

  void Write(const LogMessage& logMsg);

  void Flush(int64_t time_ms);

  void openLogFile(const struct ::tm* tm_time, int64_t time_ms);

  void shift(const struct ::tm* tm_time, int64_t time_ms);

  string fileName(int index);

  int cmpDayTime(const struct ::tm* left, const struct ::tm* right);

  string _file_name;
  string _file_ext;
  FILE* _file;
  struct tm _tm_day;

  long _file_size;
  int _max_file_num;
  long _max_file_size;
  long _bytes_since_flush;
  int64_t _last_flush_time;

  volatile bool _terminate;
  thread _async_work;

  int _max_msg_size;
  mutex _mtx;
  condition_variable _cond;
  list<LogMessage> _msg_list;
  LogSeverity _log_level;
};

class LogHelper : public boost::noncopyable {
 public:
  explicit LogHelper() {
    _sys_log = NULL;
    _app_log = NULL;
    _monitor_log = NULL;
    _inspect_log = NULL;
    _model_log = NULL;
  }

  virtual ~LogHelper() {
    google::ShutdownGoogleLogging();
    if (_sys_log) delete _sys_log;
    if (_app_log) delete _app_log;
    if (_monitor_log) delete _monitor_log;
    if (_inspect_log) delete _inspect_log;
    if (_model_log) delete _model_log;
  }

  void initialize(int argc, char* argv[]) {
    FLAGS_log_dir = "./log";

    google::InitGoogleLogging(argv[0]);

    boost::filesystem::path p(FLAGS_log_dir);
    if (!boost::filesystem::exists(p)) {
      boost::filesystem::create_directory(p);
      CHECK(boost::filesystem::exists(p))
          << "create log directory: " << FLAGS_log_dir << " failed.";
    }

    string appName = string(argv[0]);
    size_t pos = appName.find_last_of("/");
    if (pos != std::string::npos) {
      appName = appName.substr(pos + 1);
    }

    _sys_log = new DayLogSink(appName, "sys");
    _app_log = new DayLogSink(appName, "app");
    _inspect_log = new DayLogSink(appName, "inspect");
    _monitor_log = new DayLogSink(appName, "monitor");
    _model_log = new DayLogSink(appName, "model");
  }

  void setSysLogLevel(LogSeverity severity) { _sys_log->setLogLevel(severity); }
  void setAppLogLevel(LogSeverity severity) { _app_log->setLogLevel(severity); }
  void setInspectLogLevel(LogSeverity severity) { _inspect_log->setLogLevel(severity); }
  void setMonitorLogLevel(LogSeverity severity) { _monitor_log->setLogLevel(severity); }
  void setModelLogLevel(LogSeverity severity) { _model_log->setLogLevel(severity); }

  DayLogSink* getSystemLog() { return _sys_log; }
  DayLogSink* getAppLog() { return _app_log; }
  DayLogSink* getInspectLog() { return _inspect_log; }
  DayLogSink* getMonitorLog() { return _monitor_log; }
  DayLogSink* getModelLog() { return _model_log; }

  void Terminate() {
    if (_sys_log) _sys_log->Terminate();
    if (_app_log) _app_log->Terminate();
    if (_monitor_log) _monitor_log->Terminate();
    if (_inspect_log) _inspect_log->Terminate();
    if (_model_log) _model_log->Terminate();
  }

 protected:
  DayLogSink* _sys_log;
  DayLogSink* _app_log;
  DayLogSink* _inspect_log;
  DayLogSink* _monitor_log;
  DayLogSink* _model_log;
};

inline LogHelper& global_vlog_helper() {
  static LogHelper logHelper;
  return logHelper;
}

inline std::map<std::string, int>& global_log_level() {
  static std::map<std::string, int> a = {{"INFO", google::GLOG_INFO},
                                         {"WARNING", google::GLOG_WARNING},
                                         {"ERROR", google::GLOG_ERROR},
                                         {"FATAL", google::GLOG_FATAL}};
  return a;
}

};  // namespace vlog

#define VLOG_SYS(severity)                                                      \
  static_cast<void>(0),                                                         \
      !(vlog::global_vlog_helper().getSystemLog()->isNeedLog(google::severity)) \
          ? (void)0                                                             \
          : google::LogMessageVoidify() & LOG_TO_SINK_BUT_NOT_TO_LOGFILE(       \
                                              vlog::global_vlog_helper().getSystemLog(), severity)

#define VLOG_APP(severity)                                                   \
  static_cast<void>(0),                                                      \
      !(vlog::global_vlog_helper().getAppLog()->isNeedLog(google::severity)) \
          ? (void)0                                                          \
          : google::LogMessageVoidify() &                                    \
                LOG_TO_SINK_BUT_NOT_TO_LOGFILE(vlog::global_vlog_helper().getAppLog(), severity)

#define VLOG_INSPECT(severity)                                                                     \
  static_cast<void>(0), !(vlog::global_vlog_helper().getInspectLog()->isNeedLog(google::severity)) \
                            ? (void)0                                                              \
                            : google::LogMessageVoidify() &                                        \
                                  LOG_TO_SINK_BUT_NOT_TO_LOGFILE(                                  \
                                      vlog::global_vlog_helper().getInspectLog(), severity)

#define VLOG_MONITOR(severity)                                                                     \
  static_cast<void>(0), !(vlog::global_vlog_helper().getMonitorLog()->isNeedLog(google::severity)) \
                            ? (void)0                                                              \
                            : google::LogMessageVoidify() &                                        \
                                  LOG_TO_SINK_BUT_NOT_TO_LOGFILE(                                  \
                                      vlog::global_vlog_helper().getMonitorLog(), severity)

#define VLOG_MODEL(severity)                                                   \
  static_cast<void>(0),                                                        \
      !(vlog::global_vlog_helper().getModelLog()->isNeedLog(google::severity)) \
          ? (void)0                                                            \
          : google::LogMessageVoidify() &                                      \
                LOG_TO_SINK_BUT_NOT_TO_LOGFILE(vlog::global_vlog_helper().getModelLog(), severity)

