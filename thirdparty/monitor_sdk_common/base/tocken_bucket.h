#ifndef COMMON_BASE_TOCKEN_BUCKET_H
#define COMMON_BASE_TOCKEN_BUCKET_H

#pragma once
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

namespace common {

class TockenBucket {
 public:
  TockenBucket(double speed) : m_refill_speed(speed) {
    m_capacity = (int64_t)(speed * 4000LL);
    m_avail_tocken = m_capacity;
  }

  TockenBucket(double speed, int64_t capacity)
      : m_refill_speed(speed), m_capacity{capacity}, m_avail_tocken(capacity) {}

  ~TockenBucket() = default;

  void ReSet(double speed) { ReSet(speed, (int64_t)(speed * 4000LL)); }

  void ReSet(double speed, int64_t capacity) {
    m_refill_speed = speed;
    m_capacity = capacity;
    if (m_avail_tocken > capacity) m_avail_tocken = capacity;
  }

  bool TryConsume(int tocken_num) {
    Refill();
    if (m_avail_tocken > tocken_num) {
      m_avail_tocken -= tocken_num;
      return true;
    }
    return false;
  }

 private:
  int64_t MilliSeconds() {
    timeval tv;
    if (gettimeofday(&tv, NULL) != 0) return -1;
    return tv.tv_sec * 1000LL + tv.tv_usec / 1000LL;
  }
  void Refill() {
    int64_t now_ms = MilliSeconds();
    int64_t fill_cnt = now_ms - m_last_ts > 4000
                           ? m_capacity
                           : (int64_t)((now_ms - m_last_ts) * m_refill_speed);
    m_avail_tocken += fill_cnt;
    if (m_avail_tocken > m_capacity) m_avail_tocken = m_capacity;
    m_last_ts = now_ms;
  }

 private:
  double m_refill_speed{0.0};  // per ms
  int64_t m_last_ts{0};        // in ms
  int64_t m_capacity{0};
  int64_t m_avail_tocken{0};
};
}  // namespace common
#endif
