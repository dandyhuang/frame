#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

#include "mixer_common/limiter/interface_limiter.h"
#include "mixer_common/scope_guard/scope_guard.h"

namespace rec {
namespace common {
typedef float Limit;

struct limiter {
  bool ok;
  int tokens;
  int64_t timeToAct;
  // This is the Limit at reservation time, it can change later.
  Limit limit;
};

class TokenLimiter : public InterfaceLimiter {
 public:
  TokenLimiter(int qps, int16_t precision) {
    // us产生令牌的数据
    if (precision == 0) {
      rate_ = Limit(qps);
      last_time_ = butil::gettimeofday_s();
    } else if (precision == 1) {
      rate_ = Limit(qps / 1000.0);
      last_time_ = butil::gettimeofday_ms();
    } else {
      rate_ = Limit(qps / 1000000.0);
      last_time_ = butil::gettimeofday_us();
    }

    capacity_ = qps;
    tokens_ = qps;
    precision_ = precision;
  }
  virtual ~TokenLimiter() = default;
  // Every converts a minimum time interval between events to a Limit.
  Limit Every(int64_t interval) {
    if (interval <= 0) {
      return UINT_MAX;
    }
    return 1 / Limit(interval / 1000.0);
  }

  float TokensFromInterval(int64_t elapsed) {
    if (rate_ <= 0) {
      return 0;
    }
    return elapsed * float(rate_);
  }

  bool Allow(int take = 1) override;

  float GetQps() override {
    std::lock_guard<std::mutex> guard(mutex_);
    return tokens_;
  }
  virtual void UpdateStatus() {}

 private:
  limiter ReserveN(uint64_t now, int n);
  float Advance(int64_t now);
  // synchronization
  std::mutex mutex_;
  Limit rate_;    // limit
  int capacity_;  // burst
  int16_t precision_;
  float tokens_;
  // last is the last time the limiter's tokens field was updated
  uint64_t last_time_;
  // lastEvent is the latest time of a rate-limited event (past or future)
  uint64_t last_event_time_;

  std::condition_variable condition;
  bool stop;
};

}  // namespace common
}  // namespace rec
