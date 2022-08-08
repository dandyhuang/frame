//
// Created by 王锦涛 on 2019-11-18.
//
#ifndef COMMON_BASE_STRING_BITWISE_H
#define COMMON_BASE_STRING_BITWISE_H

#include <string>

namespace common {

/*
* 用于长度不相等的两个string类型的字符串之间的位运算
* 在短字符串前补齐0，使短字符串和长字符串长度相等
*/
class BitWise
{
public:
    BitWise(std::string &str1, std::string &str2);

    ~BitWise();

    void And(unsigned char* result);

    void Or(unsigned char* result);

    void Xor(unsigned char* result);

private:
    std::string max_str_;
    std::string min_str_;

    size_t max_len_;
    size_t min_len_;

    size_t diff_;
};

} // namespace common

#endif // COMMON_BASE_STRING_BITWISE_H
