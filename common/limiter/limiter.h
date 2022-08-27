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

namespace common {
typedef float Limit;
struct limiter {
  bool ok;
  int tokens;
  int64_t timeToAct;
  // This is the Limit at reservation time, it can change later.
  Limit limit;
};

class TokenLimiter {
 public:
  TokenLimiter(float rate, int capacity) : rate_(rate), capacity_(capacity), tokens_(0) {}
  ~TokenLimiter();
  // Every converts a minimum time interval between events to a Limit.
  Limit Every(int64_t interval) {
    if (interval <= 0) {
      return UINT_MAX;
    }
    return 1 / Limit(interval / 1000.0);
  }

  float TokensFromInterval(int32_t elapsed) {
    if (rate_ <= 0) {
      return 0;
    }
    return elapsed * 1000 * float(rate_);
  }

  bool Allow(int take = 1);

 private:
  limiter ReserveN(uint64_t now, int n);
  // synchronization
  std::mutex mutex_;
  Limit rate_;    // limit
  int capacity_;  // burst
  float tokens_;
  // last is the last time the limiter's tokens field was updated
  uint64_t last_time_;
  // lastEvent is the latest time of a rate-limited event (past or future)
  uint64_t last_event_time_;

  std::condition_variable condition;
  bool stop;
};

class TokenLimiterManager {};

}  // namespace common
