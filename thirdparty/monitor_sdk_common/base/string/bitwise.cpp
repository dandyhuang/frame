//
// Created by 王锦涛 on 2019-11-18.
//
#include "bitwise.h"

using namespace common;

BitWise::BitWise(std::string &max_str, std::string &min_str) {
    size_t max_len = max_str.size();
    size_t min_len = min_str.size();

    if (max_len < min_len) {
        std::swap(max_len, min_len);
        std::swap(max_str, min_str);
    }

    max_len_ = max_len;
    min_len_ = min_len;
    max_str_ = max_str;
    min_str_ = min_str;
    diff_ = max_len - min_len;
}


BitWise::~BitWise() {}

void BitWise::And(unsigned char* result) {
    for (size_t i = 0; i < max_len_; ++i) {
        if (i + min_len_ < max_len_) {
            result[i] = 0;
        } else {
            result[i] = max_str_[i] & min_str_[i - diff_];
        }
    }
}

void BitWise::Or(unsigned char* result) {
    for (size_t i = 0; i < max_len_; ++i) {
        if (i + min_len_ < max_len_) {
            result[i] = max_str_[i];
        } else {
            result[i] = max_str_[i] | min_str_[i - diff_];
        }
    }
}

void BitWise::Xor(unsigned char* result) {
    for (size_t i = 0; i < max_len_; ++i) {
        if (i + min_len_ < max_len_) {
            result[i] = max_str_[i];
        } else {
            result[i] = max_str_[i] ^ min_str_[i - diff_];
        }
    }
}
