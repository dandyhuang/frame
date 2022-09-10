#include "common/limiter/token_limiter.h"

#include "brpc/server.h"

namespace rec {
namespace common {
bool TokenLimiter::Allow(int take) {
  if (rate_ == UINT_MAX) {
    return true;
  }
  return ReserveN(-1, take).ok;
}

limiter TokenLimiter::ReserveN(uint64_t now, int n) {
  std::lock_guard<std::mutex> guard(mutex_);
  // lock时间<last 问题
  if (precision_ == 0) {
    now = butil::gettimeofday_s();
  } else if (precision_ == 1) {
    now = butil::gettimeofday_ms();
  } else {
    now = butil::gettimeofday_us();
  }
  // now = butil::gettimeofday_s();
  if (rate_ == UINT_MAX) {
    limiter r = {
        .ok = true,
        .tokens = n,
        .timeToAct = now,
    };
    return r;
  }

  auto tokens = Advance(now);

  // Calculate the remaining number of tokens resulting from the request.
  tokens -= float(n);

  // Decide result
  auto ok = (tokens < 0) ? false : true;

  // Prepare reservation
  limiter r;
  r.ok = ok;
  r.tokens = rate_;
  if (ok) {
    r.tokens = n;
    r.timeToAct = now;
  }

  // Update state
  if (ok) {
    VLOG_APP(INFO) << " success tokens: " << tokens_ << " token: " << tokens
                   << " last_time:" << last_time_ << " now:" << now << " capacity: " << capacity_;
    tokens_ = tokens;
    last_event_time_ = now;
    last_time_ = now;
  } else {
    VLOG_APP(ERROR) << " failed tokens:" << tokens_ << " token:" << tokens
                    << "last_time:" << last_time_ << "now:" << now;
  }

  return std::move(r);
}

float TokenLimiter::Advance(int64_t now) {
  // auto last_time = last_time_;
  // 因为锁住，lock导致传进来的时间小于last_time时间, 第一次未初始化
  if ((now - last_time_) < 0) {
    last_time_ = now;
  }

  // Calculate the new number of tokens, due to time that passed.
  auto elapsed = now - last_time_;
  auto delta = TokensFromInterval(elapsed);
  auto tokens = tokens_ + delta;
  if (tokens >= capacity_) {
    tokens = capacity_;
  }
  return tokens;
}
}  // namespace common
}  // namespace rec
