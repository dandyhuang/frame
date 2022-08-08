/********************************************************************************
* Description: 32位哈希算法实现，目前用循环校验码实现.
* crawler.
* @version     %I%, %G%
* @since       1.0
********************************************************************************/

#include "thirdparty/monitor_sdk_common/crypto/hash/string_hashes.h"
#include <string.h>

namespace common {

// 源于UNIX System V，号称"实际生活散列函数中的典型魔术"，但发现不理想
unsigned int ELFhash(const char* url, unsigned int hash_size)
{
    unsigned int h = 0;

    while (*url)
    {
        h = (h << 4) + *url++;
        unsigned int g = h & 0xF0000000;

        if (g)
            h ^= g >> 24;

        h &= ~g;
    }

    return h % hash_size;
}

// 由阎宏飞和谢正茂采用折叠方法设计的，用在天网搜索引擎中，感觉负载平衡效果比
// 较好，但没有系统评估过。
unsigned int HfIp(const char* url, unsigned int hash_size)
{
    unsigned int n = 0;
    unsigned int url_length = strlen(url);
    unsigned char* b = (unsigned char*)&n;

    for (unsigned int i = 0; i < url_length; i++)
        b[i%4] ^= url[i];

    return n % hash_size;
}

// 谢正茂为在多个数据库中分布网页设计的，也是出于负载平衡的目的，主要的一个
// 考虑因素是实现简单。
unsigned int hf(const char* url, unsigned int hash_size)
{
    int result = 0;
    const char* ptr = url;
    int c;

    for (int i = 1; (c = *ptr++); i++)
        result += c * 3 * i;

    if (result < 0)
        result = -result;

    return result % hash_size;
}

} // namespace common
