/********************************************************************************
* Description: 32λ��ϣ�㷨ʵ�֣�Ŀǰ��ѭ��У����ʵ��.
* crawler.
* @version     %I%, %G%
* @since       1.0
********************************************************************************/

#include "thirdparty/monitor_sdk_common/crypto/hash/string_hashes.h"
#include <string.h>

namespace common {

// Դ��UNIX System V���ų�"ʵ������ɢ�к����еĵ���ħ��"�������ֲ�����
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

// ���ֺ�ɺ�л��ï�����۵�������Ƶģ������������������У��о�����ƽ��Ч����
// �Ϻã���û��ϵͳ��������
unsigned int HfIp(const char* url, unsigned int hash_size)
{
    unsigned int n = 0;
    unsigned int url_length = strlen(url);
    unsigned char* b = (unsigned char*)&n;

    for (unsigned int i = 0; i < url_length; i++)
        b[i%4] ^= url[i];

    return n % hash_size;
}

// л��ïΪ�ڶ�����ݿ��зֲ���ҳ��Ƶģ�Ҳ�ǳ��ڸ���ƽ���Ŀ�ģ���Ҫ��һ��
// ����������ʵ�ּ򵥡�
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
