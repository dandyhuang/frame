#include "limiter.h"
#include "scope_guard/scope_guard.h"
namespace common {
bool TokenLimiter::Allow(int take) {
  if (rate_ == UINT_MAX) {
    return true;
  }
  return ReserveN(butil::gettimeofday_us(), take).ok;
}

limiter TokenLimiter::ReserveN(uint64_t now, int n)  {
	mutex_.lock();
   dandyhuang::ON_SCOPE_EXIT(([]() {
   mutex_.unlock();
  }));

	if (rate_ == UINT_MAX) {
		return limiter {
			ok = true,
      timeToAct = now,
      tokens = tokens_,
		};
	}

	auto tokens = Advance(now);

	// Calculate the remaining number of tokens resulting from the request.
	auto tokens -= float(n);

	// Decide result
	auto ok = (n <= capacity_);

	// Prepare reservation
	auto r = limiter{
		ok =  ok,
		limit = rate_,
	}
	if (ok) {
		r.tokens = n;
		r.timeToAct = now;
	}

	// Update state
	if (ok) {
		tokens_ = tokens;
		last_event_time_ = now;
	}
  last_time_ = now;

	return std::move(r);
}


float TokenLimiter::Advance(int64_t now) {
	auto last_time = last_time_;
  if ((now - last_time) < 0) {
    last_time = now;
  }

	// Calculate the new number of tokens, due to time that passed.
	auto elapsed = now - last_time;
	auto delta = TokensFromInterval(elapsed);
	auto tokens = tokens_ + delta;
	if (tokens_ > capacity_) {
		tokens = capacity_;
	}
	return tokens;
}
}
