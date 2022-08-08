//
// Created by 黄伟锋 on 2019-04-17.
//

#ifndef COMMON_LIBCFG_BLOCK_QUEUE_H_
#define COMMON_LIBCFG_BLOCK_QUEUE_H_

#include <algorithm>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <vector>

namespace common {

template <typename T>
class BlockQueue {

 public:

  BlockQueue(int capacity) { capacity_ = capacity; }
  ~BlockQueue() {}

  bool RemainingCapacity() {
    std::unique_lock<std::mutex> lock(this->d_mutex_);

    return capacity_ - d_queue_.size();
  }

  void Push(T const& value) {
    std::unique_lock<std::mutex> lock(this->d_mutex_);

    d_queue_.push_front(value);
    this->d_condition_.notify_one();
  }

  T Pop() {
    std::unique_lock<std::mutex> lock(this->d_mutex_);

    this->d_condition_.wait(lock, [=]{ return !this->d_queue_.empty(); });
    T rc(std::move(this->d_queue_.back()));
    this->d_queue_.pop_back();

    return rc;
  }

  std::vector<T> Drain(int max_elements) {
    std::unique_lock<std::mutex> lock(this->d_mutex_);

    std::vector<T> rs;
    rs.reserve(max_elements);

    while (!this->d_queue_.empty() && max_elements > 0) {
      T rc(std::move(this->d_queue_.back()));
      this->d_queue_.pop_back();
      rs.push_back(rc);
      max_elements --;
    }

    return rs;
  }

  int Size() {
    std::unique_lock<std::mutex> lock(this->d_mutex_);

    return d_queue_.size();
  }

 private:

  std::mutex d_mutex_;
  std::condition_variable d_condition_;
  std::deque<T> d_queue_;
  int capacity_;

};

} // namespace common

#endif //COMMON_LIBCFG_BLOCK_QUEUE_H_
