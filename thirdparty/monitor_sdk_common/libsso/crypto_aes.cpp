//
// Created by 吴婷 on 2020-05-14.
//

#include "crypto_aes.h"

// 用于CryptoPP解密
#include "thirdparty/cryptopp/cryptlib.h"
using CryptoPP::Exception;

#include "thirdparty/cryptopp/base64.h"
using CryptoPP::Base64Encoder;

#include "thirdparty/cryptopp/filters.h"
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;

#include "thirdparty/cryptopp/aes.h"
using CryptoPP::AES;

#include "thirdparty/cryptopp/modes.h"
using CryptoPP::CBC_Mode;

#include "thirdparty/cryptopp/config.h"
using CryptoPP::byte;

using namespace common;

std::string CryptoAes::GetEncodedSign(const std::string & sign, const std::string & key, const std::string & iv){
    std::string cipher_text, encoded;  //密文
    //1.key: string -> 字节数组
    int ksize = (int)CryptoPP::AES::DEFAULT_KEYLENGTH;
    byte bKey[AES::DEFAULT_KEYLENGTH];
    string_to_bytes(key, bKey, ksize);

    //2.iv: string -> 字节数组
    size_t vsize = CryptoPP::AES::BLOCKSIZE;
    byte bIv[AES::BLOCKSIZE];
    string_to_bytes(iv, bIv, vsize);

    //3.AES加密算法：采用CBC模式，pkcs7padding填充、数据块128位、输出base64、字符编码utf-8
    try {
        CBC_Mode< AES >::Encryption d;           //CFB模式，AES加密算法
        d.SetKeyWithIV(bKey, sizeof(bKey), bIv); // 给Decryption实例绑定key和iv
        // 开始加密
        StringSource s(sign, true, new StreamTransformationFilter(
                d, new StringSink(cipher_text), StreamTransformationFilter::PKCS_PADDING) //pkcspadding方法
        );
    }
    catch(const CryptoPP::Exception& e){
        std::cerr << e.what() << std::endl;
        return encoded;
        //exit(1);
    }
    //4.base64输出
    encoded.clear();
    StringSource(cipher_text, true,
                 new Base64Encoder(
                         new StringSink(encoded)
                 )
    );
    return encoded;
}

void CryptoAes::string_to_bytes(const std::string & str, byte * bytes, int size){
    int len = str.length();  //字符串长度
    bool isPadding = false;   //是否补足
    int cplen;               //拷贝位数
    //1.判断拷贝数
    if(len < size){ //不足位
        isPadding = true;
        cplen = len;
    } else {         //超过长度进行截取
        cplen = size;
    }
    //拷贝
    for (int i = 0; i < cplen; ++i) {
        bytes[i] = str[i];
    }
    //不足位用'\0'补足
    if(isPadding){
        for (int i = cplen; i < size; ++i) {
            bytes[i] = '\0';
        }
    }
}



