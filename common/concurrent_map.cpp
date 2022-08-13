#include "common/concurrent_map.h"
namespace rec {

DoubleBuffer<ConcurrentMap<CacheKey, std::shared_ptr<std::vector<double>>>> emb_cache;

std::shared_ptr<std::unordered_map<CacheKey, std::shared_ptr<std::vector<double>>, HashFunc<CacheKey>>>
get_item_feature_from_cache(const std::vector<CacheKey> & items, std::vector<CacheKey>& miss_items) {
    auto item_fea_cache = emb_cache.get_current_obj();
    auto data_map = item_fea_cache->get(items);
    for (auto & k : items) {
        if (data_map->find(k) == data_map->end()) {
            miss_items.push_back(k);
        }
    }
    return data_map;
}

void put_item_feature_into_bak_cache(std::unordered_map<CacheKey, std::shared_ptr<std::vector<double>>, HashFunc<CacheKey>>& map) {
    emb_cache.get_bak_obj()->put(map);
}
void put_item_feature_into_cur_cache(std::unordered_map<CacheKey, std::shared_ptr<std::vector<double>>, HashFunc<CacheKey>>& map) {
    emb_cache.get_current_obj()->put(map);
}
std::atomic<uint32_t>* get_double_buffer_index_ptr() {
    return emb_cache.get_index_ptr();
}

void switch_emb_cache() {
    emb_cache.change();
}
int get_cur_cache_size() {
    return emb_cache.get_current_obj()->size();
}
int get_bak_cache_size() {
    return emb_cache.get_bak_obj()->size();
}
} // end of namespace rec
