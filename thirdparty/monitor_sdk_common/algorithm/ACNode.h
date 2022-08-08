// Copyright (c) 2018, The Toft Authors.
// All rights reserved.
//
// Author: Zhao Pu
#ifndef COMMON_ALGORITHM_ACNODE_H_
#define COMMON_ALGORITHM_ACNODE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace Ahocorasick {

typedef unsigned char BYTE;
static const int MAX_BYTE_BUFF_LEN = 4;

class CUtf8Char {
 public:
  /**
     @see     CKeywordShield()
     @desc    构造函数
     @return  无
     @note
  */
  CUtf8Char();
  /**
 @see     CKeywordShield()
 @desc    析构函数
 @return  无
 @note
*/
  virtual ~CUtf8Char();

 public:
  /**
     @see     Parse()
     @desc    解析函数
     @param   BYTE* pbuf 按字节转化后的字符串
     @param   BYTE* pbuf 按字节转化后的字符串长度
     @return  <= 0 异常，其他值代表处理的字符个数
     @note
  */
  int Parse(const BYTE *pbuf, int bufLen);
  /**
     @see     GetBufLen()
     @desc    获取字符串长度函数
     @return  字符串长度
     @note
  */
  int GetBufLen() const;
  /**
 @see     IsEmpty()
 @desc    判空函数
 @return  true 为空，false 不为空
 @note
*/
  bool IsEmpty() const;
  /**
 @see     Equal()
 @desc    比较函数
 @param   CUtf8Char& other 另一个字符串对象
 @return  true 为相同，false 为不相同
 @note
*/
  bool Equal(const CUtf8Char &other) const;
  /**
     @see     <
     @desc    比较函数
     @param   BYTE* pbuf 按字节转化后的字符串

     @return  <= 0 异常，其他值代表处理的字符个数
     @note
  */
  bool operator<(const CUtf8Char &other) const;

 private:
  /**
      utf8串的字节数
  */
  BYTE m_bufLen;
  /**
      uft8转为字节串，最多不超过3个字节
  */
  BYTE m_buf[MAX_BYTE_BUFF_LEN];
};
}  // namespace Ahocorasick
#endif  // COMMON_ALGORITHM_ACNODE_H_