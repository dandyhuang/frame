// Copyright (c) 2018, The Toft Authors.
// All rights reserved.
//
// Author: Zhao Pu

#include "thirdparty/monitor_sdk_common/algorithm/ChinesePinYin.h"
#include <assert.h>
#include <stdio.h>
#include <algorithm>
#include <string>

namespace pinyin {

bool Hanzi2PinYin::IsEnStr(const string &strInput) {
  const char *buf = strInput.c_str();
  int bufLen = strInput.size();
  for (int i = 0; i < bufLen; ++i) {
    if (!IsEnChar(buf[i])) return false;
  }

  return true;
}

bool Hanzi2PinYin::IsDigitalStr(const string &strInput) {
  const char *buf = strInput.c_str();
  int bufLen = strInput.size();
  for (int i = 0; i < bufLen; ++i) {
    if (!IsDigitalChar(buf[i])) return false;
  }

  return true;
}

int Hanzi2PinYin::BytesOfUtf8(const char *str) {
  // U-00000000 - U-0000007F: 0xxxxxxx
  // U-00000080 - U-000007FF: 110xxxxx 10xxxxxx
  // U-00000800 - U-0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx
  // U-00010000 - U-001FFFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
  // U-00200000 - U-03FFFFFF: 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
  // U-04000000 - U-7FFFFFFF: 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
  // 10xxxxxx

  assert(NULL != str);

  char firstCh = *str;
  if ((firstCh & 0x80) == 0x0)
    return 1;
  else if ((firstCh & 0xE0) == 0xC0)
    return 2;
  else if ((firstCh & 0xF0) == 0xE0)
    return 3;
  else if ((firstCh & 0xF8) == 0xF0)
    return 4;
  else if ((firstCh & 0xFC) == 0xF8)
    return 5;
  else if ((firstCh & 0xFE) == 0xFC)
    return 6;

  // 非法的编码( [0x80, 0xC0), 0xFE, 0xFF )返回 -1
  return -1;
}

int Hanzi2PinYin::Utf8ToUnicode(const char *str, unsigned int &unicode) {
  assert(NULL != str);

  unicode = 0xFFFFFFFF;
  int nbytes = BytesOfUtf8(str);
  if (-1 == nbytes) {
    // 非法编码
    return -1;
  }

  char prefix_byte = 0xFF;
  switch (nbytes) {
    case 1: {
      prefix_byte = *str;
      break;
    }
    case 2: {
      prefix_byte = (*str) & 0x1F;
      break;
    }
    case 3: {
      prefix_byte = (*str) & 0xF;
      break;
    }
    case 4: {
      prefix_byte = (*str) & 0x7;
      break;
    }
    case 5: {
      prefix_byte = (*str) & 0x3;
      break;
    }
    case 6: {
      prefix_byte = (*str) & 0x1;
      break;
    }
    default:
      // 非法编码
      assert(false && "Can't get here");
  }

  unicode = (unsigned char)prefix_byte;
  for (int i = 1; i < nbytes; ++i) {
    if ('\0' == str[i]) {
      unicode = 0xFFFFFFFF;
      return -2;
    }
    if ((str[i] & 0xC0) != 0x80) {
      // 后续的字节是非法编码
      unicode = 0xFFFFFFFF;
      return -1;
    }

    unicode <<= 6;
    unicode |= (unsigned char)str[i] & 0x3F;
  }

  return nbytes;
}

int Hanzi2PinYin::GetPinYinByIndex(const MapItem *item, int index,
                                   const char *&begin, const char *&end) {
  assert(NULL != item);
  if (index >= item->segments || index < 0) {
    begin = NULL;
    end = NULL;
    return -1;
  }

  if (0 == index) {
    begin = item->str;
    if (1 == item->segments) {
      end = item->str + item->length;
    } else {
      end = item->str + item->spliters[0];
    }
  } else if (index == item->segments - 1) {
    begin = item->str + item->spliters[index - 1] + 1;
    end = item->str + item->length;
  } else {
    begin = item->str + item->spliters[index - 1] + 1;
    end = item->str + item->spliters[index];
  }

  return end - begin;
}

int Hanzi2PinYin::SpliteToSegments(vector<SegItem> &segs, const char *utf8,
                                   bool toHalfPinYin) {
  int seg_count = 0;
  const char *nonChinese = NULL;

  while ('\0' != *utf8) {
    unsigned int unicode = 0xFFFFFFFF;
    int nbytes = Utf8ToUnicode(utf8, unicode);

    if (nbytes < 1) {
      // 转换失败，遇到无效编码或者长度不够
      if (NULL == nonChinese) nonChinese = utf8;
      ++utf8;
      continue;
    }

    if (!IsChinese(unicode)) {
      // 非汉字
      if (NULL == nonChinese) nonChinese = utf8;
      utf8 += nbytes;
      continue;
    }

    if (NULL != nonChinese) {
      // 遇到了一个汉字，它前面可能是非汉字段.
      segs[seg_count].type = NonChinese;
      segs[seg_count].raw.nonChinese.str = nonChinese;
      segs[seg_count].raw.nonChinese.length = utf8 - nonChinese;

      nonChinese = NULL;
      ++seg_count;
    }

    segs[seg_count].type = Chinese;
    if (toHalfPinYin) {
      segs[seg_count].raw.item = &HalfPinYinMap[unicode - 0x4E00];
    } else {
      segs[seg_count].raw.item = &FullPinYinMap[unicode - 0x4E00];
    }

    ++seg_count;
    utf8 += nbytes;
  }

  if (NULL != nonChinese) {
    // 保存最后一个非汉字段
    segs[seg_count].type = NonChinese;
    segs[seg_count].raw.nonChinese.str = nonChinese;
    segs[seg_count].raw.nonChinese.length = utf8 - nonChinese;

    nonChinese = NULL;
    ++seg_count;
  }

  return seg_count;
}

int Hanzi2PinYin::CountResultsMaybe(const vector<SegItem> &segs, const int used,
                                    int &max_length) {
  int count = 1;

  for (int i = 0; i < used; ++i) {
    const SegItem &seg = segs[i];
    if (Chinese == seg.type) {
      count *= seg.raw.item->segments;
      max_length += seg.raw.item->max_segment;
    } else {
      max_length += seg.raw.nonChinese.length;
    }
    if (count > MAXOUTPUT_DEFAULT) {
      printf("count = %d\n", count);
      count = -1;
      break;
    }
  }

  return count;
}

int Hanzi2PinYin::CopyNonChineseSeg(const SegItem &seg, const string &spliter,
                                    char *buffer) {
  char *next =
      std::copy(seg.raw.nonChinese.str,
                seg.raw.nonChinese.str + seg.raw.nonChinese.length, buffer);

  if (!spliter.empty()) {
    next = std::copy(spliter.begin(), spliter.end(), next);
  }

  return (next - buffer);
}

int Hanzi2PinYin::CopyPinYin(const MapItem *item, int index,
                             const string &spliter, char *buffer) {
  const char *begin = NULL;
  const char *end = NULL;

  GetPinYinByIndex(item, index, begin, end);
  char *next = std::copy(begin, end, buffer);

  if (!spliter.empty()) {
    next = std::copy(spliter.begin(), spliter.end(), next);
  }

  return (next - buffer);
}

void Hanzi2PinYin::ExpandToStrings(const Immutable &var, const int depth,
                                   int buffer_used) {
  const vector<SegItem> &segs = var.segs;
  const int seg_count = var.seg_count;
  const string &spliter = var.spliter;
  char *buffer = var.buffer;
  vector<string> &result = var.result;

  if (depth == seg_count) {
    buffer[buffer_used] = '\0';
    result.push_back(string(buffer, buffer_used));

    return;
  }

  const SegItem &seg = segs[depth];
  if (NonChinese == seg.type) {
    int len = CopyNonChineseSeg(seg, spliter, buffer + buffer_used);
    ExpandToStrings(var, depth + 1, buffer_used + len);
  } else {
    // 这一段是汉字，可能存在多个拼音.
    const MapItem *item = seg.raw.item;

    for (int i = 0; i < item->segments; ++i) {
      int len = CopyPinYin(item, i, spliter, buffer + buffer_used);
      ExpandToStrings(var, depth + 1, buffer_used + len);
    }
  }
}

bool Hanzi2PinYin::ChineseToPinYin(
    std::vector<std::string> &ResultList, const std::string &stText,
    bool toHalfPinYin, /* = true */ const char *spliter /* = NULL */) {
  if (stText.empty()) {
    return false;
  }

  if ((int)stText.length() > MAXINPUT_DEFAULT) {
    printf("The length of the input text exceed PinYinMaxInput(%d).\n",
           MAXINPUT_DEFAULT);
    return false;
  }

  vector<SegItem> segs;

  // 1.先分配足够长度的内存，避免多次分配影响性能和造成内存碎片.
  segs.resize(stText.length());

  // 2.分割输入的 utf8 字符串.
  int seg_count = SpliteToSegments(segs, stText.c_str(), toHalfPinYin);

  int max_length = 0;
  // 3.计算结果的数量以及最长字符串的长度(不带分隔符的情况下).
  int result_count = CountResultsMaybe(segs, seg_count, max_length);

  if (result_count < 0) {
    printf("The number of convertion result exceed PinYinMaxOutput(%d)\n",
           MAXOUTPUT_DEFAULT);
    return false;
  }

  // 4.预分配结果集的空间，避免多次内存分配.
  ResultList.reserve(result_count);

  vector<char> buffer;
  const string str_spliter(NULL == spliter ? "" : spliter);

  // 5.分配缓冲区.
  buffer.resize(seg_count * str_spliter.size() + max_length + 1);

  // 6.展开树.
  Immutable variable = {segs, seg_count, str_spliter, &buffer[0], ResultList};

  ExpandToStrings(variable, 0, 0);

  int nSize = (int)ResultList.size();
  return (nSize > 0) ? true : false;
}

int Hanzi2PinYin::TextTokenizer(std::string &utf8Text,
                                std::vector<TokenWord> &tokenStream) {
  //先分配内存，用下标直接访问
  tokenStream.resize(utf8Text.size());
  int token_count = 0;
  const char *utf8 = utf8Text.c_str();
  const char *nonChinese = NULL;

  while ('\0' != *utf8) {
    unsigned int unicode = 0xFFFFFFFF;
    int nbytes = Utf8ToUnicode(utf8, unicode);

    if (nbytes < 1) {
      // 转换失败，遇到无效编码或者长度不够
      if (NULL == nonChinese) {
        nonChinese = utf8;
      }
      ++utf8;
      continue;
    }

    if (!IsChinese(unicode)) {
      // 非汉字
      if (isLetter(unicode) || isDigital(unicode)) {
        if (NULL == nonChinese) {
          nonChinese = utf8;
        }
        utf8 += nbytes;
        continue;
      }
    }

    if (NULL != nonChinese) {
      int len = utf8 - nonChinese;
      tokenStream[token_count].word = string(nonChinese, len);

      if (IsEnStr(tokenStream[token_count].word)) {
        tokenStream[token_count].type = English;
      } else if (IsDigitalStr(tokenStream[token_count].word)) {
        tokenStream[token_count].type = Digital;
      } else {
        tokenStream[token_count].type = other;
      }

      nonChinese = NULL;
      ++token_count;
    }

    if (!IsChinese(unicode)) {
      tokenStream[token_count].type = symbol;
    } else {
      tokenStream[token_count].type = Chinese;
    }
    tokenStream[token_count].word = string(utf8, nbytes);

    ++token_count;
    utf8 += nbytes;
  }

  if (NULL != nonChinese) {
    // 保存最后一个非汉字段
    int len = utf8 - nonChinese;
    tokenStream[token_count].word = string(nonChinese, len);

    if (IsEnStr(tokenStream[token_count].word)) {
      tokenStream[token_count].type = English;
    } else if (IsDigitalStr(tokenStream[token_count].word)) {
      tokenStream[token_count].type = Digital;
    } else {
      tokenStream[token_count].type = other;
    }

    nonChinese = NULL;
    ++token_count;
  }
  tokenStream.resize(token_count);
  return token_count;
}

int Hanzi2PinYin::TextTokenizerExternal(std::string &utf8Text,
                                        std::vector<TokenWord> &tokenStream) {
  //先分配内存，用下标直接访问
  tokenStream.resize(utf8Text.size());
  int token_count = 0;
  const char *utf8 = utf8Text.c_str();
  const char *nonChinese = NULL;

  while ('\0' != *utf8) {
    unsigned int unicode = 0xFFFFFFFF;
    int nbytes = Utf8ToUnicode(utf8, unicode);

    if (nbytes < 1) {
      // 转换失败，遇到无效编码或者长度不够
      if (NULL == nonChinese) nonChinese = utf8;
      ++utf8;
      continue;
    }

    if (!IsChinese(unicode)) {
      // 非汉字
      if (NULL == nonChinese) nonChinese = utf8;
      utf8 += nbytes;
      continue;
    }

    if (NULL != nonChinese) {
      int len = utf8 - nonChinese;
      tokenStream[token_count].word = string(nonChinese, len);
      tokenStream[token_count].type = English;

      nonChinese = NULL;
      ++token_count;
    }

    tokenStream[token_count].type = Chinese;
    tokenStream[token_count].word = string(utf8, nbytes);

    ++token_count;
    utf8 += nbytes;
  }

  if (NULL != nonChinese) {
    int len = utf8 - nonChinese;
    tokenStream[token_count].word = string(nonChinese, len);
    nonChinese = NULL;
    ++token_count;
  }
  tokenStream.resize(token_count);
  return token_count;
}

}  // namespace pinyin
