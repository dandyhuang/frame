// Copyright (c) 2018, The Toft Authors.
// All rights reserved.
//
// Author: Zhao Pu

#ifndef COMMON_ALGORITHM_CHINESEPINYIN_H_
#define COMMON_ALGORITHM_CHINESEPINYIN_H_

#include <string>
#include <vector>
#include "thirdparty/monitor_sdk_common/algorithm/PinYinMap.h"

using std::string;
using std::vector;

namespace pinyin {

#define MAXINPUT_DEFAULT 512
#define MAXOUTPUT_DEFAULT 256

enum SegmentType {
  Chinese = 0,     // 表示此段为汉字
  NonChinese = 1,  // 表示此段为非汉字的数字和符号
  English = 2,     //表示此段为英文字母
  Digital = 3,     //表示此段为数字
  symbol = 4,      //表示此段为符号
  other = 5        //表示英文+ 数字
};

// 代表 utf-8 字符串解析后的每个段
struct SegItem {
  union {
    const MapItem *item;
    struct {
      const char *str;  // 指向原始 utf8 字符串的非汉字字符串
      int length;       // 指向 utf8 字符串的长度
    } nonChinese;
  } raw;

  SegmentType type;
};

struct TokenWord {
  std::string word;
  SegmentType type;
};

// 在树递归过程中的不可变，避免栈开销太大
struct Immutable {
  const std::vector<SegItem> &segs;
  const int seg_count;
  const std::string &spliter;
  char *buffer;
  std::vector<std::string> &result;
};

class Hanzi2PinYin {
 public:
  Hanzi2PinYin() {}
  ~Hanzi2PinYin() {}

 private:
  //范围20901 = 40869 - 19968
  static inline bool IsChinese(unsigned int unicode) {
    return (unicode >= 0x4E00) && (unicode <= 0x9FA5);
  }

  static inline bool isLetter(unsigned int unicode) {
    return (((unicode >= 0x0041) && (unicode <= 0x005A)) ||
            ((unicode >= 0x0061) && (unicode <= 0x007A)));
  }

  static inline bool isDigital(unsigned int unicode) {
    return (unicode >= 0x0030) && (unicode <= 0x0039);
  }
  static inline bool IsEnChar(const char c) {
    return (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'A')));
  }

  static bool IsDigitalChar(const char c) { return (c >= '0') && (c <= '9'); }

  static bool IsEnStr(const string &strInput);

  static bool IsDigitalStr(const string &strInput);
  /**
  @desc 计算 str 所表示的字符由多少字节组成. 非法编码返回 -1.
  @param str
  @return 返回对应的字节数，非法编码返回 -1.
  */
  static int BytesOfUtf8(const char *str);
  /**
  @desc 将 str 编码字符转换为单个 Unicode 字符.
  @param[in]  str  将要转换的 utf8 序列.
  @param[out] unicode 转换后的 unicode 编码，如果转换失败，则为 0xFFFFFFFF.
  @return >0 返回 str 序列所表示的字符由多少个字节组成.
    -1: 无效编码.
    -2: 长度不够.
  */
  static int Utf8ToUnicode(const char *str, unsigned int &unicode);
  /**
  @desc 返回 MapItem 中某个段的指针范围 [begin, end).
  @param[in]  item
  @param[in]  index 段的索引( 从 0 开始 )
  @param[out] begin
  @param[out] end
  * @return >0 表示返回段的长度; -1 表示失败.
  */
  static int GetPinYinByIndex(const MapItem *item, int index,
                              const char *&begin, const char *&end);
  /**
  @desc 将 utf8 序列分割为 Seg，Seg 可能代表一个汉字，或者非汉字字符串.
  @param segs[] 分割后输出的 Seg 结果.
  @param utf8   将要分割的 utf8 序列.
  @param toHalfPinYin 转换为简拼.

  @return Seg 的数量 N (N <= segs.size()).
  */
  static int SpliteToSegments(vector<SegItem> &segs, const char *utf8,
                              bool toHalfPinYin);
  /**
  @desc 计算可能的结果 N( N <= 最终结果).
  @param[in] segs 分割后的 Seg，由 SpliteToSegments() 分割.
  @param[in] used segs 的使用数量，参考 SpliteToSegments().
  @param[out] avg_length 每个结果的平均长度.
  @return 可能的结果数量(剔除重复之前).
  */
  static int CountResultsMaybe(const vector<SegItem> &segs, const int used,
                               int &max_length);
  /**
  @desc 将 seg 指定的非汉字字符串拷贝至 buffer 中.
  @param seg
  @param spliter
  @param buffer
  @return 拷贝的字符串长度.
  */
  static int CopyNonChineseSeg(const SegItem &seg, const string &spliter,
                               char *buffer);
  /**
  @desc 将 item 中指定索引的拼音拷贝至buffer.
  @param item
  @param index
  @param spliter
  @param buffer
  @return 拷贝的字符串长度.
  */
  static int CopyPinYin(const MapItem *item, int index, const string &spliter,
                        char *buffer);
  /**
  @desc 将 segs 所表示的完全多叉树到叶子节点的路径展开为对应的字符串.
  @param result 将展开的的字符串经过去重检测后放入结果集.
  @param spliter 分隔符.
  @param depth 当前的深度，控制递归的条件.
  */
  static void ExpandToStrings(const Immutable &var, const int depth,
                              int buffer_used);

 public:
  /**
  @see  ChineseToPinYin
  @desc 将汉字转换为全拼或者简拼.
  @param[out]  ResultList   输出转换的结果. 如果转换的结果的数量超过
  PinYinMaxOutput，则返回空.
  @param[in]   stText     输入字符串，必须是 utf-8 编码的字符串.
  输入的大小不能超过 PinYinMaxInput.
  @param[in]   spliter    转换结果的分隔符. 默认为 NULL.
  @param     toHalfPinYin true 表示转换为简拼， false 转换为全拼。默认为
  true（即转为简拼).
  @return true 表示成功，否则为 false. 返回失败的情况：
  1. 输入长度超过 PinYinMaxInput. 2. 转换的结果的数量超过 PinYinMaxOutput.
  */
  static bool ChineseToPinYin(std::vector<std::string> &ResultList,
                              const std::string &stText,
                              bool toHalfPinYin = true,
                              const char *spliter = NULL);

  /**
  @see  TextTokenizer
  @desc 将输入的语句进行成分解析
  @param[out]  tokenStream   输出词元构成
  @param[in]   stText     输入字符串，必须是 utf-8 编码的字符串.
  @return int 表示：拆分后的语句成分组成数量
  */
  static int TextTokenizer(std::string &utf8Text,
                           std::vector<TokenWord> &tokenStream);

  //只识别中文，非中文，并分段
  static int TextTokenizerExternal(std::string &utf8Text,
                                   std::vector<TokenWord> &tokenStream);
};
}  // namespace pinyin

#endif  // COMMON_ALGORITHM_CHINESEPINYIN_H_
