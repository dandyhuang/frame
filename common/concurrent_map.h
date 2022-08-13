#pragma once
#include <mutex>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include <atomic>
#include <set>
#include "boost/thread/shared_mutex.hpp"

namespace Rec {
namespace Util {
int64_t get_cur_time_us();    
}
}

namespace rec {
template<typename T>
class DoubleBuffer {
private:
    std::vector<std::shared_ptr<T>> vec;
    std::atomic<uint32_t> index;
public:
    DoubleBuffer() {
        vec.resize(2);
        vec[0].reset(new T());
        vec[1].reset(new T());
        index = 0;
    }
    std::shared_ptr<T> get_current_obj() {
        return vec[index.load()];
    }
    std::shared_ptr<T> get_bak_obj() {
        return vec[1 - index.load()];
    }
    void change() {
        index.store(1 - index.load());
    }
    std::atomic<uint32_t>* get_index_ptr() {
        return &index;
    }
    int get_cur_index() {
        return index.load();
    }
};
template<typename Value>
class CacheInfo {
public:
    Value value;
    // int64_t timestamp; // 当value为nullptr时，记录时间戳
};
template<typename T>
class HashFunc {
public:
    std::hash<T> h_f;
    size_t operator()(const T & t)  const noexcept {
        return h_f(t);
    }
};

class CacheKey {
public:
    CacheKey(uint64_t item_id, int costtype) : item_id(item_id), costtype(costtype){
    }
    uint64_t item_id;
    int costtype;
    bool operator== (const CacheKey & t)  const noexcept {
        return item_id == t.item_id && costtype == t.costtype;
    }
    bool operator < (const CacheKey & t)  const noexcept {
        if (item_id < t.item_id) return true;
        else if (item_id > t.item_id) return false;
        else if (costtype < t.costtype) return true;
        else return false; 
    }
};
template<>
class HashFunc <CacheKey> {
public:
    size_t operator() (const CacheKey& k)  const noexcept {
        std::size_t h1 = std::hash<uint64_t>{}(k.item_id);
        std::size_t h2 = std::hash<int>{}(k.costtype);
        return h1 ^ (h2 << 1); // or use boost::hash_combine
    }
};
template <typename Key, typename Value>
class ConcurrentMap {
private:
    std::vector<std::unordered_map<Key, CacheInfo<Value>, HashFunc<Key>>> map_vec;
    std::vector<std::shared_ptr<boost::shared_mutex>> mtx_vec;
    HashFunc<Key> hash_func;
    int retry_time = 10;
public:
    ConcurrentMap(int seg_size = 1024) {
        map_vec.resize(seg_size);
        for (int i = 0; i < seg_size; i++) {
            mtx_vec.push_back(std::make_shared<boost::shared_mutex>());
        }
    }

    void put(const std::unordered_map<Key, Value, HashFunc<Key>>& map) {
        std::vector<std::unordered_map<Key, Value, HashFunc<Key>>> tmp_map_vec;
        tmp_map_vec.resize(map_vec.size());
        for (auto & pair : map) {
            const Key& k = pair.first;
            const Value& v = pair.second;
            size_t map_index = hash_func(k) % map_vec.size();
            tmp_map_vec[map_index][k] = v;
        }
        for (size_t i = 0; i < tmp_map_vec.size(); ++i) {
            auto & tmp_map = tmp_map_vec[i];
            if (tmp_map.size() == 0) {
                continue;
            }
            auto mtx_ptr = mtx_vec[i];
            {
                // hold write lock
                mtx_ptr->lock();
                auto& target_map = map_vec[i];
                for (auto & pair : tmp_map) {
                    target_map[pair.first] = {pair.second};
                }
                mtx_ptr->unlock();
            }

        }
    }
    std::shared_ptr<std::unordered_map<Key, Value, HashFunc<Key>>> get(const std::vector<Key> key_vec) {
        std::vector<std::vector<Key>> tmp_key_vec;
        tmp_key_vec.resize(map_vec.size());

        for (const Key& key : key_vec) {
            size_t index = hash_func(key) % map_vec.size();
            auto & vec = tmp_key_vec[index];
            if (vec.size() == 0) {
                vec.reserve(key_vec.size() / 2);
            }
            vec.push_back(key);
        }
        std::unordered_map<Key, Value, HashFunc<Key>> ret;
        for (size_t i = 0; i < tmp_key_vec.size(); ++i) {
            auto & k_vec = tmp_key_vec[i];
            if (k_vec.size() == 0) {
                continue;
            }
            auto & map = map_vec[i];
            if (map.size() == 0) {
                continue;
            }
            auto mtx_ptr = mtx_vec[i];
            int try_time = 0;
            bool got_lock = mtx_ptr->try_lock_shared();
            while(!got_lock && try_time < retry_time) {
                ++try_time;
                got_lock = mtx_ptr->try_lock_shared();
            }
            if (got_lock) {
                for (auto & k : k_vec) {
                    auto it = map.find(k);
                    if (it == map.end()) {
                        continue;
                    }
                    ret.insert({it->first, it->second.value});
                }
                mtx_ptr->unlock_shared();
            }
        }
        return std::make_shared<std::unordered_map<Key, Value, HashFunc<Key>>>(std::move(ret));
    }
    int size() {
        int size = 0;
        for (int i = 0; i < map_vec.size(); i++) {
            auto mtx_ptr = mtx_vec[i];
            mtx_ptr->lock();
            size += map_vec[i].size();
            mtx_ptr->unlock();
        }
        return size;
    }
};
/* embedding cache api*/
std::shared_ptr<std::unordered_map<CacheKey, std::shared_ptr<std::vector<double>>, HashFunc<CacheKey>>>
get_item_feature_from_cache(const std::vector<CacheKey> & items, std::vector<CacheKey>& miss_items);
void put_item_feature_into_bak_cache(std::unordered_map<CacheKey, std::shared_ptr<std::vector<double>>, HashFunc<CacheKey>>& map);
void put_item_feature_into_cur_cache(std::unordered_map<CacheKey, std::shared_ptr<std::vector<double>>, HashFunc<CacheKey>>& map);
std::atomic<uint32_t>* get_double_buffer_index_ptr();
void switch_emb_cache();
int get_cur_cache_size();
int get_bak_cache_size();
/* embedding cache api*/
} // end of namespace rec
