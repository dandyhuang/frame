//
// Created by 陈嘉豪 on 2019/12/5.
//

#ifndef COMMON_LIBHAWKING_HASH_CODE_UTILS_H
#define COMMON_LIBHAWKING_HASH_CODE_UTILS_H

#include <cmath>
#include <string>

#include "thirdparty/monitor_sdk_common/crypto/hash/md5.h"
#include "thirdparty/monitor_sdk_common/crypto/hash/sha1.h"

namespace common {

class HashCodeUtils {
public:
    HashCodeUtils() = default;
    
    ~HashCodeUtils() = default;
    
    static std::string GetMd5(const std::string& str);
    
    static std::string GetSha1(const std::string &str);
    
    static int HashCode(const std::string& str);
    
    static int GetHashCode(const std::string& str, const std::string& hash_method);
    
    static std::string GetHashOnePercent(const std::string& str, const std::string& hash_method);
    
    static std::string GetHashOneThousandth(const std::string& str, const std::string& hash_method);
};

} // namespace common

#endif //COMMON_LIBHAWKING_HASH_CODE_UTILS_H
