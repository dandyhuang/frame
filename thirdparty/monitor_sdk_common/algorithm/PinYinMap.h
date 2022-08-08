// Copyright (c) 2018, The Toft Authors.
// All rights reserved.
//
// Author: Zhao Pu

#ifndef COMMON_ALGORITHM_PINYINMAP_H_
#define COMMON_ALGORITHM_PINYINMAP_H_
#include <stdint.h>
namespace pinyin {

#define MAX_SPLITER_COUNT 14

struct MapItem {
  // 指向 Map 的字符串，如 "c|zh".
  const char *str;
  // str 的长度.
  uint16_t length;
  // 以 '|' 分割的段的个数，等于 '|' 的个数 + 1
  uint16_t segments;
  // 最长的拼音的长度
  uint16_t max_segment;
  // '|' 标识符的下标。
  // 如: 'c|ch|s|sh|z|zh' 的分隔符下标 -
  // { 1, 5, 6, 9, 11, -1, ... }
  // 值为 -1 表示无效下标.
  int16_t spliters[MAX_SPLITER_COUNT];
};

// 映射表的大小.
extern const int G_PinYinTableSize;

// 全拼映射表
extern const struct MapItem *FullPinYinMap;

// 简拼映射表
extern const struct MapItem *HalfPinYinMap;

}  // namespace pinyin
#endif  // COMMON_ALGORITHM_PINYINMAP_H_
