#include "common/loghelper.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>

namespace vlog {

vlog::DayLogSink::DayLogSink(const string& strFileName, const string& strExt, int max_file_num) {
  _file_name = strFileName;
  _file_ext = strExt;
  _max_file_size = 1024 * 1024 * 1024;  // 1G
  _max_file_num = max_file_num;
  _file = NULL;
  _bytes_since_flush = 0;
  _last_flush_time = 0;

  _terminate = false;
  _max_msg_size = 20000;
  _log_level = google::GLOG_INFO;
  _async_work = std::thread(&DayLogSink::AsyncWriteData, this);
}

DayLogSink::~DayLogSink() {
  Terminate()
  if (_file) {
    fclose(_file);
  }
}

void DayLogSink::send(LogSeverity severity, const char* full_filename, const char* base_filename,
                      int line, const struct ::tm* tm_time, const char* message,
                      size_t message_len) {
  send(severity, full_filename, base_filename, line, tm_time, message, message_len, 0);
}

void DayLogSink::send(LogSeverity severity, const char* full_filename, const char* base_filename,
                      int line, const struct ::tm* tm_time, const char* message, size_t message_len,
                      int32 usecs) {
  LogMessage logMsg;
  struct ::tm tmpTm = *tm_time;

  logMsg._log_msg =
      ToString(severity, base_filename, line, tm_time, message, message_len + 1, usecs);
  logMsg._tm = tmpTm;
  logMsg._time_ms = mktime(&tmpTm) * 1000 + usecs / 1000;

  PushData(logMsg);
}

void DayLogSink::PushData(const LogMessage& logMsg) {
  std::unique_lock<std::mutex> lck(_mtx);

  if (_msg_list.size() > (size_t)_max_msg_size) {
    LOG(ERROR) << "PushMessage drop message";
    return;
  }

  _msg_list.push_back(logMsg);

  _cond.notify_one();
}

void DayLogSink::Terminate() {
  _terminate = true;

  {
    std::unique_lock<std::mutex> lck(_mtx);
    _cond.notify_one();
  }

  _async_work.join();
}

void DayLogSink::AsyncWriteData() {
  while (!_terminate) {
    list<LogMessage> tmp;
    {
      std::unique_lock<std::mutex> lck(_mtx);

      if (_msg_list.empty()) {
        _cond.wait_for(lck, std::chrono::microseconds(500));
      }

      tmp.swap(_msg_list);
    }

    if (tmp.empty()) {
      struct timeval tv;
      gettimeofday(&tv, NULL);
      int64_t time_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;

      Flush(time_ms);
      continue;
    }

    while (!tmp.empty()) {
      LogMessage& logmsg = tmp.front();

      Write(logmsg);

      tmp.pop_front();
    }
  }
}

void DayLogSink::Write(const LogMessage& logMsg) {
  if (_file == NULL) {
    openLogFile(&logMsg._tm, logMsg._time_ms);
  } else {
    shift(&logMsg._tm, logMsg._time_ms);
  }

  // check open success
  if (_file == NULL) {
    LOG(ERROR) << "Open file failed, Drop message.[" << _file_ext << "]";
    return;
  }

  // fwrite() doesn't return an error when the disk is full, for
  // messages that are less than 4096 bytes. When the disk is full,
  // it returns the message length for messages that are less than
  // 4096 bytes. fwrite() returns 4096 for message lengths that are
  // greater than 4096, thereby indicating an error. (from GLOG)
  fwrite(logMsg._log_msg.c_str(), 1, logMsg._log_msg.length(), _file);
  if (errno == ENOSPC) {
    return;
  }

  _file_size += logMsg._log_msg.length();
  _bytes_since_flush += logMsg._log_msg.length();

  Flush(logMsg._time_ms);
}

void DayLogSink::Flush(int64_t time_ms) {
  if (_file == NULL) {
    return;
  }

  if (_bytes_since_flush >= 1000000 || (_bytes_since_flush > 0 && time_ms > _last_flush_time &&
                                        time_ms - _last_flush_time >= 500))  // 500ms or msg > 1M
  {
    _bytes_since_flush = 0;
    _last_flush_time = time_ms;
    fflush(_file);
  }
}

void DayLogSink::setMaxFileSize(long maxFileSize) {
  if (maxFileSize >= 100 * 1024 * 1024) {
    _max_file_size = maxFileSize;
  }
}

void DayLogSink::setMaxFileNum(int fileNum) { _max_file_num = fileNum; }

string DayLogSink::fileName(int index) {
  char szBuf[128] = {0};
  int iLen = snprintf(szBuf, sizeof(szBuf) - 1, "%s/%s.%s.%04d%02d%02d%02d", FLAGS_log_dir.c_str(),
                      _file_name.c_str(), _file_ext.c_str(), _tm_day.tm_year + 1900,
                      _tm_day.tm_mon + 1, _tm_day.tm_mday, _tm_day.tm_hour);
  // int iLen = snprintf(szBuf, sizeof(szBuf) - 1, "%s/%s.%s.%04d%02d%02d%02d%02d",
  // FLAGS_log_dir.c_str(), _file_name.c_str(), _file_ext.c_str(), _tm_day.tm_year + 1900,
  // _tm_day.tm_mon + 1, _tm_day.tm_mday, _tm_day.tm_hour, _tm_day.tm_min);

  if (index > 0) {
    snprintf(szBuf + iLen, sizeof(szBuf) - iLen - 1, ".%02d", index);
  }

  return szBuf;
}

void DayLogSink::shift(const struct ::tm* tm_time, int64_t time_ms) {
  // chang date
  if (cmpDayTime(tm_time, &_tm_day) > 0) {
    if (_file) {
      fclose(_file);
      _file = NULL;
    }

    openLogFile(tm_time, time_ms);
  } else if (_file_size >= _max_file_size && _max_file_num > 1) {
    if (_file) {
      fclose(_file);
      _file = NULL;
    }

    string strNewFile, strOldFile;
    strNewFile = fileName(_max_file_num - 1);

    if (access(strNewFile.c_str(), F_OK) == 0) {
      if (remove(strNewFile.c_str()) != 0) return;
    }

    for (int i = _max_file_num - 2; i >= 0; i--) {
      strOldFile = fileName(i);
      strNewFile = fileName(i + 1);

      if (access(strOldFile.c_str(), F_OK) == 0) {
        if (rename(strOldFile.c_str(), strNewFile.c_str()) != 0) return;
      }
    }

    openLogFile(tm_time, time_ms);
  }
}

int DayLogSink::cmpDayTime(const struct ::tm* left, const struct ::tm* right) {
  if (left->tm_year != right->tm_year) {
    return left->tm_year - right->tm_year;
  }

  if (left->tm_mon != right->tm_mon) {
    return left->tm_mon - right->tm_mon;
  }

  if (left->tm_mday != right->tm_mday) {
    return left->tm_mday - right->tm_mday;
  }

  return left->tm_hour - right->tm_hour;

  /*
  if(left->tm_hour != right->tm_hour)
  {
      return left->tm_hour - right->tm_hour;
  }
  return left->tm_min - right->tm_min;
  */
}

void DayLogSink::openLogFile(const struct ::tm* tm_time, int64_t time_ms) {
  _tm_day = *tm_time;

  string _fileName = fileName(0);

  int fd = open(_fileName.c_str(), O_WRONLY | O_CREAT, 0664);
  if (fd == -1) {
    return;
  }

  _file = fdopen(fd, "a");
  if (_file == NULL) {
    close(fd);
    return;
  }

  _file_size = ftell(_file);
  _bytes_since_flush = 0;
  _last_flush_time = time_ms;
}

};  // namespace vlog
