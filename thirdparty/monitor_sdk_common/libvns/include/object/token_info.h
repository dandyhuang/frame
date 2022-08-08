//
// Created by 吴婷 on 2020-07-22.
//

#ifndef COMMON_LIBVNS_TOKEN_INFO_H
#define COMMON_LIBVNS_TOKEN_INFO_H

#include <string>
#include <map>

class TokenInfo {
public:
    std::string toString();

public:
    int retcode;
    std::string message;
    std::map<std::string, std::string> data;
};

class ResponseError {
public:
    std::string toString();

public:
    std::string timestamp;
    int status;
    std::string error;
    std::string message;
    std::string path;
};

#endif //COMMON_LIBVNS_TOKEN_INFO_H
