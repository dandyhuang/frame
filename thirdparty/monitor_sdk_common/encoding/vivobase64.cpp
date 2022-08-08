//
// Created by 王锦涛 on 2019-11-13.
//
#include <string>
#include <cstring>
#include <vector>

#include "vivobase64.h"

using namespace common;

std::string VivoBase64::Encode(std::vector<char> &data) {
    const char base64_encode_chars_[] = {'Q', '8', 'v', 'N', '-', 'r', 'y', 'a', 'E', 'J', 'G', 'o', 'T', 'W', 'O', 't', 'K', '_', 'q', 'M', 'k', 'h',
                                         '5', 'R', 'Z', '6', 'L', 'x', 'c', 'U', 'A', '3', 'd', 'n', 'z', 'e', 'H', 'u', '2', 'X', 'j', 'S', 'b', 'V',
                                         's', 'F', 'Y', 'w', 'f', 'P', 'D', '9', '4', 'C', '0', 'l', 'm', '1', 'I', 'p', '7', 'g', 'B', 'i'
                                        };

    std::string sb = "";
    int len = data.size();
    int i = 0;
    int b1, b2, b3;

    while (i < len) {
        b1 = data[i++] & 0xff;
        if (i == len) {
            sb += base64_encode_chars_[((unsigned char)b1) >> 2];
            sb += base64_encode_chars_[(b1 & 0x3) << 4];
            break;
        }
        b2 = data[i++] & 0xff;
        if (i == len) {
            sb += base64_encode_chars_[((unsigned char)b1) >> 2];
            sb += base64_encode_chars_[((b1 & 0x03) << 4) | (((unsigned char)(b2 & 0xf0)) >> 4)];
            sb += base64_encode_chars_[(b2 & 0x0f) << 2];
            break;
        }
        b3 = data[i++] & 0xff;
        sb += base64_encode_chars_[((unsigned char)b1) >> 2];
        sb += base64_encode_chars_[((b1 & 0x03) << 4) | (((unsigned char)(b2 & 0xf0)) >> 4)];
        sb += base64_encode_chars_[((b2 & 0x0f) << 2) | (((unsigned char)(b3 & 0xc0)) >> 6)];
        sb += base64_encode_chars_[b3 & 0x3f];
    }

    return sb;
}

std::vector<char> VivoBase64::Decode(std::string &str) {
    const char base64_dncode_chars_[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                          -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 4, -1, -1, 54, 57, 38, 31, 52, 22, 25, 60, 1, 51, -1, -1, -1, -1, -1, -1,
                                          -1, 30, 62, 53, 50, 8, 45, 10, 36, 58, 9, 16, 26, 19, 3, 14, 49, 0, 23, 41, 12, 29, 43, 13, 39, 46, 24, -1, -1, -1, -1, 17, -1,
                                          7, 42, 28, 32, 35, 48, 61, 21, 63, 40, 20, 55, 56, 33, 11, 59, 18, 5, 44, 15, 37, 2, 47, 27, 6, 34, -1, -1, -1, -1, -1
                                        };

    std::vector<char> ret;
    int len = strlen(str.c_str());

    int i = 0;
    int b1, b2, b3, b4;
    while (len - i >= 4) {
        b1 = base64_dncode_chars_[(int)str[i++]];

        b2 = base64_dncode_chars_[(int)str[i++]];
        ret.push_back((char) ((b1 << 2) | (((unsigned char)(b2 & 0x30)) >> 4)));

        b3 = base64_dncode_chars_[(int)str[i++]];
        ret.push_back((char) (((b2 & 0x0f) << 4) | (((unsigned char)(b3 & 0x3c)) >> 2)));

        b4 = base64_dncode_chars_[(int)str[i++]];
        ret.push_back((char) (((b3 & 0x03) << 6) | b4));
    }

    int dis = len - i;
    if (1 == dis) {
        b1 = base64_dncode_chars_[(int)str[i++]];
        int temp = b1 << 2;
        if (temp > 0) {
            ret.push_back((char) temp);
        }
    } else if (2 == dis) {
        b1 = base64_dncode_chars_[(int)str[i++]];
        b2 = base64_dncode_chars_[(int)str[i++]];
        ret.push_back((char) ((b1 << 2) | (((unsigned char)(b2 & 0x30)) >> 4)));
        int temp = (b2 & 0x0f) << 4;
        if (temp > 0) {
            ret.push_back((char) temp);
        }
    } else if (3 == dis) {
        b1 = base64_dncode_chars_[(int)str[i++]];
        b2 = base64_dncode_chars_[(int)str[i++]];
        ret.push_back((char) ((b1 << 2) | (((unsigned char)(b2 & 0x30)) >> 4)));
        b3 = base64_dncode_chars_[(int)str[i++]];
        ret.push_back((char) (((b2 & 0x0f) << 4) | (((unsigned char)(b3 & 0x3c)) >> 2)));
        int temp = (b3 & 0x03) << 6;
        if (temp > 0) {
            ret.push_back((char) temp);
        }
    }

    return ret;
}
