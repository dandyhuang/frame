//
// Created by 陈嘉豪 on 2019/12/16.
//

#ifndef COMMON_LIBHAWKING_RANDOM_STRING_UTILS_H
#define COMMON_LIBHAWKING_RANDOM_STRING_UTILS_H

#include <algorithm>
#include <string>

#include "thirdparty/monitor_sdk_common/libhawking/id_utils.h"

namespace common {

class RandomStringUtils {
public:
    static std::string Generate(std::string::size_type sz) {
        std::srand(IdUtils::GetNextId());
        std::string s;
        
        s.reserve (sz);
        generate_n(std::back_inserter(s), sz, RandAlnum);
        return s;
    }
    
    static char RandAlnum()
    {
        char c;
        while (!std::isalnum(c = static_cast<char>(std::rand())));
        return c;
    }
};

} // namespace common


#endif //COMMON_LIBHAWKING_RANDOM_STRING_UTILS_H
