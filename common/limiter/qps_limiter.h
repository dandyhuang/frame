#pragma once

#include <bvar/bvar.h>
#include <glog/logging.h>
#include <stdint.h>
#include <time.h>

#include <atomic>

#include "common/limiter/interface_limiter.h"
#include "common/loghelper.h"

#include "common/timer/timer.h"

namespace rec {
namespace common {

class QpsLimiter : public InterfaceLimiter {
 public:
  QpsLimiter(int32_t window_size = 1, int max_qps=500)
      : _window_size(window_size),
        _query_recorder_window(&_query_recorder, (time_t)window_size),
        _is_allow(true),
        _qps(0),
        _max_qps(max_qps) {
          timer_.run();
        }

  void set_max_qps(int32_t qps) { _max_qps = qps; }

  void query_record() { _query_recorder << 1; }

  bool is_allow() {
    // LOG(INFO) << "qps status:" << (_is_allow.load() ? "allow" : "forbiden");
    return _is_allow.load();
  }

  virtual bool Allow(int take = 1) {
    _query_recorder << 1;
    return _is_allow.load();
  }

  virtual float GetQps() { return _qps.load(); }

  virtual void UpdateStatus() {
    // 1000ms
    timer_.run_repeated(1000, -1,
                            std::bind(&QpsLimiter::TimerStatus, this));
  }

   void TimerStatus() {
    bvar::detail::Sample<bvar::IntRecorder::value_type> tmp;
    if (_query_recorder_window.get_span(&tmp)) {
      // get highlatency_event_percent
      VLOG_APP(INFO) << "total query:" << tmp.data.sum
                     << ", qps: " << (double)(tmp.data.sum) / _window_size << ", status:"
                     << (((double)(tmp.data.sum) / _window_size >= _max_qps) ? "forbiden"
                                                                             : "allow");
      _qps.store(tmp.data.sum / _window_size);
      _is_allow.store((double)(tmp.data.sum) / _window_size <= _max_qps);
    } else {
      _is_allow.store(true);
    }
  }

 private:
  // qps统计，对service进行熔断保护
  int32_t _window_size;
  bvar::IntRecorder _query_recorder;
  bvar::Window<bvar::IntRecorder> _query_recorder_window;

  std::atomic<bool> _is_allow;
  std::atomic<int32_t> _qps;

  int32_t _max_qps;
  ::common::Timer timer_;
};

}  // namespace common
}  // namespace rec
