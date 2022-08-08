// Copyright (c) 2018, The Toft Authors.
// All rights reserved.
//
// Author: Zhao Pu

#include "thirdparty/monitor_sdk_common/algorithm/PinYinMap.h"
#include <algorithm>
#include "thirdparty/monitor_sdk_common/algorithm/PinYinDict.h"

namespace pinyin {
// 映射表的大小.
const int G_PinYinTableSize = sizeof(pinyin_table) / sizeof(pinyin_table[0]);

// 全拼映射表
static struct MapItem FullPinYinMapInternal[G_PinYinTableSize];

// 简拼映射表
static struct MapItem HalfPinYinMapInternal[G_PinYinTableSize];

const struct MapItem *FullPinYinMap = FullPinYinMapInternal;

const struct MapItem *HalfPinYinMap = HalfPinYinMapInternal;

int ConstructMapItem(MapItem &item, const char *py) {
  item.str = py;
  int length = 0;
  int spliter_count = 0;
  int max_segment = 0;

  const char *ptr = py;
  const char *prev_spliter = ptr - 1;

  while ('\0' != *ptr) {
    if ('|' == *ptr) {
      // 判断分隔符索引数组的长度，目前 PinYInMap.h 中用到的分隔符个数最大为 5
      if (spliter_count < MAX_SPLITER_COUNT) {
        item.spliters[spliter_count] = ptr - item.str;

        max_segment = std::max(max_segment, (int)(ptr - prev_spliter - 1));
        prev_spliter = ptr;
      }
      ++spliter_count;
    }
    ++length;
    ++ptr;
  }

  if (spliter_count <= MAX_SPLITER_COUNT) {
    max_segment = std::max(max_segment, (int)(ptr - prev_spliter - 1));
  }

  item.length = length;
  item.max_segment = max_segment;
  item.segments = std::min(spliter_count, MAX_SPLITER_COUNT) + 1;

  /// 将用不到的分隔符索引设置为 -1.
  for (; spliter_count < MAX_SPLITER_COUNT; ++spliter_count) {
    item.spliters[spliter_count] = -1;
  }

  return spliter_count;
}

void ConstructMap(MapItem map[], const char *table[]) {
  for (int i = 0; i < G_PinYinTableSize; ++i) {
    ConstructMapItem(map[i], table[i]);
  }
}

class Initializer {
 public:
  Initializer() {
    ConstructMap(FullPinYinMapInternal, pinyin_table);
    ConstructMap(HalfPinYinMapInternal, py_first_alpha_table);
  }
};

Initializer init;

}  // namespace pinyin
