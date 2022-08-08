//
// Created by 王锦涛 on 2019-11-13.
//
#ifndef COMMON_ENCODING_VIVOBASE64_H
#define COMMON_ENCODING_VIVOBASE64_H

#include <string>
#include <vector>

namespace common {

/*
* VivoBase64编码（与标准base64的码表稍有不同）
*/
class VivoBase64
{
public:
    VivoBase64() {};

    ~VivoBase64() {};

    std::string Encode(std::vector<char> &data);

    std::vector<char> Decode(std::string &str);
};

} // namespace common

#endif  // COMMON_ENCODING_VIVOBASE64_H
