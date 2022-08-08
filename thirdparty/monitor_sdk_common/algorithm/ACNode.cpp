// Copyright (c) 2018, The Toft Authors.
// All rights reserved.
//
// Author: Zhao Pu
#include "thirdparty/monitor_sdk_common/algorithm/ACNode.h"

namespace Ahocorasick {
CUtf8Char::CUtf8Char() : m_bufLen(0) {}

CUtf8Char::~CUtf8Char() {}

int CUtf8Char::Parse(const BYTE *pbuf, int bufLen) {
  if ((NULL == pbuf) || (bufLen <= 0)) {
    // printf("%S: NULL == pbuf || bufLen <= 0", __FUNCTION__);
    return 0;
  }

  int retval = 0;

  if (pbuf[0] <= 0X7F)  // 0xxx xxxx
  {
    retval = 1;
    m_bufLen = 1;
    m_buf[0] = pbuf[0];
  } else if (0XC0 == (pbuf[0] & 0XE0))  // 110x xxxx 10xx xxxx
  {
    if (bufLen < 2) {
      return -1;
    }
    retval = 2;
    m_bufLen = 2;
    m_buf[0] = pbuf[0];
    m_buf[1] = pbuf[1];
  } else if (0XE0 == (pbuf[0] & 0XF0))  // 1110 xxxx 10xx xxxx 10xx xxxx
  {
    if (bufLen < 3) {
      return -1;
    }
    retval = 3;
    m_bufLen = 3;
    m_buf[0] = pbuf[0];
    m_buf[1] = pbuf[1];
    m_buf[2] = pbuf[2];
  } else if (0XF0 ==
             (pbuf[0] & 0XF8))  // 1111 0xxx 10xx xxxx 10xx xxxx 10xx xxxx
  {
    if (bufLen < 4) {
      return -1;
    }
    retval = 4;
    m_bufLen = 4;
    m_buf[0] = pbuf[0];
    m_buf[1] = pbuf[1];
    m_buf[2] = pbuf[2];
    m_buf[3] = pbuf[3];
  } else {
    return -1;
  }

  return retval;
}

int CUtf8Char::GetBufLen() const { return m_bufLen; }

bool CUtf8Char::IsEmpty() const { return (0 == m_bufLen); }

bool CUtf8Char::Equal(const CUtf8Char &other) const {
  return (m_bufLen == other.m_bufLen) &&
         (0 == memcmp(static_cast<const void *>(m_buf),
                      static_cast<const void *>(other.m_buf), m_bufLen));
}

bool CUtf8Char::operator<(const CUtf8Char &other) const {
  if (m_bufLen != other.m_bufLen) {
    return m_bufLen < other.m_bufLen;
  } else {
    return memcmp(static_cast<const void *>(m_buf),
                  static_cast<const void *>(other.m_buf), m_bufLen) < 0;
  }
}

}  // namespace Ahocorasick