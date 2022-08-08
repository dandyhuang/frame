//
// Created by 陈嘉豪 on 2019/12/5.
//

#include "thirdparty/monitor_sdk_common/libhawking/hash_code_utils.h"

using namespace common;

std::string HashCodeUtils::GetMd5(const std::string &str) {
    return MD5::HexDigest(str);
}

std::string HashCodeUtils::GetSha1(const std::string &str) {
    return SHA1::HexDigest(str);
}

int HashCodeUtils::HashCode(const std::string &str) {
    int hash_code = 0;
    for (char c : str) {
        hash_code = hash_code * 31 + static_cast<int>(c);
    }
    return hash_code;
}

int HashCodeUtils::GetHashCode(const std::string &str, const std::string &hash_method) {
    if (hash_method == "MD5" || hash_method == "md5") {
        return HashCode(GetMd5(str));
    } else if (hash_method == "SHA1" || hash_method == "sha1"){
        return HashCode(GetSha1(str));
    }
    return 0;
}

std::string HashCodeUtils::GetHashOnePercent(const std::string &str, const std::string &hash_method) {
    int tail = abs(GetHashCode(str, hash_method) % 100);
    if (tail < 10) {
        return "0" + std::to_string(tail);
    }
    return std::to_string(tail);
}

std::string HashCodeUtils::GetHashOneThousandth(const std::string &str, const std::string &hash_method) {
    int tail = abs(GetHashCode(str, hash_method) % 1000);
    std::string tail_str = std::to_string(tail);
    if (tail_str.length() == 2) {
        return "0" + tail_str;
    } else if (tail_str.length() == 1) {
        return "00" + tail_str;
    }
    return tail_str;
}
