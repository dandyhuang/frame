#pragma once

#include <mutex>
#include <unordered_map>

namespace common {
template <typename K, typename V>
class hash_map {
 public:
  void emplace(const K& key, const V& v) {
    std::unique_lock<std::mutex> lck(mutex_);
    hm_map_[key] = v;
  }

  void emplace(const K& key, V&& v) {
    std::unique_lock<std::mutex> lck(mutex_);
    hm_map_[key] = move(v);
  }

  void erase(const K& key) {
    std::unique_lock<std::mutex> lck(mutex_);
    if (hm_map_.find(key) != hm_map_.end()) {
      hm_map_.erase(key);
    }
  }

  bool get_val(const K& key, V& value) {
    std::unique_lock<std::mutex> lck(mutex_);
    if (hm_map_.find(key) != hm_map_.end()) {
      value = hm_map_[key];
      return true;
    }
    return false;
  }

  bool is_key_exist(const K& key) {
    std::unique_lock<std::mutex> lck(mutex_);
    return hm_map_.find(key) != hm_map_.end();
  }

  size_t size() {
    std::unique_lock<std::mutex> lck(mutex_);
    return hm_map_.size();
  }

 private:
  std::unordered_map<K, V> hm_map_;
  std::mutex mutex_;
};
}