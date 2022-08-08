//
// Created by 吴婷 on 2020-05-14.
//

#ifndef COMMON_LIBSSO_CRYPTOAES_H
#define COMMON_LIBSSO_CRYPTOAES_H

#include <iostream>

#include "thirdparty/monitor_sdk_common/base/singleton.h"
#include "thirdparty/cryptopp/config.h"

namespace common {
/**
 * AES加密
 */
class CryptoAes : public SingletonBase<CryptoAes>{
public:
    /**
     * 业务参数进行AES加密
     * @param sign 加密前的sign字符串
     * @param key 密钥
     * @param iv 偏移量
     * @return 加密后的sign字符串
     */
    static std::string GetEncodedSign(const std::string & sign, const std::string & key, const std::string & iv);

private:
    /**
     * 将string -> 字节数组
     * @param str
     * @param bytes
     * @param size 数组固定大小
     */
    static void string_to_bytes(const std::string & str, CryptoPP::byte * bytes, int size);
};
} //namespace common

#endif //COMMON_LIBSSO_CRYPTOAES_H
