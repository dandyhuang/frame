/** 
 * @file bmz.h
 * @brief 
 * @author kypoyin
 * @date 2010-05-04
 */

#ifndef COMMON_COMPRESS_BMZ_H_
#define COMMON_COMPRESS_BMZ_H_

#include <stdlib.h>
#include "thirdparty/monitor_sdk_common/base/stdint.h"

#define BMZ_E_OK    (0)
#define BMZ_E_ERROR (-1)
#define BMZ_E_INPUT_OVERRUN   (-4)
#define BMZ_E_OUTPUT_OVERRUN  (-5)
#define BMZ_E_ERROR_FORMAT	(-6)

/** 
 * @brief bmz��ʼ������Ҫ��ʼ��lzo
 * 
 * @return �ɹ�����BMZ_E_OK
 */

int	bmz_init();

/** 
 * @brief ����bmz��������Ҫ�����ڴ��С
 * 
 * @param in_len ���볤��
 * @param fp_len ̽�볤��
 * 
 * @return �����ڴ泤��
 */

size_t	bmz_pack_worklen(size_t in_len, size_t fp_len);

/** 
 * @brief ����bmz��������Ҫ�����ڴ��С
 * 
 * @param in_len �������
 * 
 * @return �����ڴ泤��
 */

size_t	bmz_unpack_worklen(size_t out_len);

/** 
 * @brief bmz����
 * 
 * @param in Դ��ַ
 * @param in_len Դ����
 * @param out Ŀ���ַ
 * @param out_len_p Ŀ�곤��ָ��
 * @param fp_len ̽�볤��
 * @param work_mem �����ڴ�,�ڴ��Сͨ��bmz_pack_worklen����
 * 
 * @return �ɹ�����BMZ_E_OK��ʧ�ܷ�����Ӧ�Ĵ�����
 */

int	bmz_pack(const void *in,  size_t in_len, void *out, size_t *out_len_p, size_t	fp_len, void *work_mem);

/** 
 * @brief bmz����
 * 
 * @param in Դ��ַ
 * @param in_len Դ����
 * @param out Ŀ���ַ
 * @param out_len_p Ŀ�곤��ָ��
 * @param work_mem �����ڴ棬�ڴ��Сͨ��bmz_unpack_worklen����
 * 
 * @return �ɹ�����BMZ_E_OK,ʧ�ܷ�����صĴ�����
 */

int	bmz_unpack(const void *in, size_t in_len, void *out, size_t	*out_len_p,  void *work_mem);


#endif ///COMMON_COMPRESS_BMZ_H_
