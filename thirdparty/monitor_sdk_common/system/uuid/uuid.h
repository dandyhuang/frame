//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-06-24 10:44
//  @file:      uuid.h
//  @author:    杨一飞(yangyifei@baidu.com)	
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_UUID_UUID_H
#define COMMON_SYSTEM_UUID_UUID_H

#include <string>
#include <stdexcept>
#include "thirdparty/monitor_sdk_common/system/uuid/libuuid/uuid.h"
#include "thirdparty/monitor_sdk_common/system/uuid/libuuid/uuidP.h"


namespace common {

class Uuid
{
public:
    Uuid()
    {
        Clear();
    }
    
    explicit Uuid(const char* str)
    {
        if (!Parse(str))
            throw std::runtime_error("Invalid UUID string");
    }
    
    explicit Uuid(const std::string& str)
    {
        if (!Parse(str))
            throw std::runtime_error("Invalid UUID string");
    }
    
    std::string ToString() const
    {
        char buf[37];
        uuid_unparse(m_buf, buf);
        return std::string(buf, 36);
    }
    
    bool Parse(const char* str)
    {
        return uuid_parse(str, m_buf) == 0;
    }
    
    bool Parse(const std::string& str)
    {
        return Parse(str.c_str());
    }
    
    void Generate()
    {
        return uuid_generate(m_buf);
    }
    
    bool IsNull() const
    {
        return uuid_is_null(m_buf) != 0;
    }
    
    void Clear()
    {
        uuid_clear(m_buf);
    }
    
    int Compare(const Uuid& that) const
    {
        return uuid_compare(this->m_buf, that.m_buf);
    }
    
    const uuid_t& Bytes() const
    {
        return m_buf;
    }
private:
    uuid_t m_buf;
};
    
inline bool operator<(const Uuid& lhs, const Uuid& rhs)
{
    return lhs.Compare(rhs) < 0;
}
inline bool operator<=(const Uuid& lhs, const Uuid& rhs)
{
    return lhs.Compare(rhs) <= 0;
}
inline bool operator==(const Uuid& lhs, const Uuid& rhs)
{
    return lhs.Compare(rhs) == 0;
}
inline bool operator!=(const Uuid& lhs, const Uuid& rhs)
{
    return lhs.Compare(rhs) != 0;
}
inline bool operator>(const Uuid& lhs, const Uuid& rhs)
{
    return lhs.Compare(rhs) > 0;
}
inline bool operator>=(const Uuid& lhs, const Uuid& rhs)
{
    return lhs.Compare(rhs) >= 0;
}
} // end of namespace common

#endif // COMMON_SYSTEM_UUID_UUID_H
