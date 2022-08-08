/**
 * @file  bmdiff.h
 * @brief
 * @version 1.0
 * @date	2011-01-11 03:17:23 PM
 */

#ifndef __BM_DIFF_H__
#define __BM_DIFF_H__

#define BM_E_OK    0
#define BM_E_ERROR (-1)
#define BM_E_INPUT_OVERRUN   (-4)
#define BM_E_OUTPUT_OVERRUN  (-5)
#define BM_E_ERROR_FORMAT	(-6)

/**
 * @brief ����bm��������Ҫ�ĸ����ڴ��С
 *
 * @param in_len ���볤��
 * @param fp_len ̽�볤��
 *
 * @return �����ڴ��С
 */
size_t  bm_encode_worklen(size_t in_len, size_t fp_len);

/**
 * @brief ����bm��������Ҫ�����ڴ��С
 *
 * @param out_len �������
 *
 * @return �����ڴ��С
 */
size_t	bm_decode_worklen(size_t out_len);


/**
 * @brief bmѹ������
 *
 * @param src Դ��ַ
 * @param in_len Դ����
 * @param dst Ŀ���ַ
 * @param out_len_p �洢Ŀ�곤�ȵ�������ַ���ɹ��󳤶�ֵ�޸�Ϊʵ�ʵ�ѹ���󳤶�
 * @param fp_len ̽�볤�ȣ�����Ϊ[fp_len, 2*fp_len]֮��Ĺ��������ֱ�ѹ�������ȴ���2*fp_len��ض���ѹ��
 * @param work_mem �����ڴ棬���ڴ洢HashTable.�䳤������fp_len��in_len
 *
 * @return �ɹ�����BMZ_E_OK,ʧ�ܷ�����Ӧ�Ĵ�����
 */

int bm_encode_body(const void *src, size_t in_len, void *dst, size_t *out_len_p, size_t fp_len,  void *work_mem);

/**
 * @brief bm��ѹ����
 *
 * @param src Դ��ַ
 * @param in_len Դ����
 * @param dst Ŀ���ַ
 * @param out_len_p Ŀ�곤��ָ��,ֻ�е�Ŀ�곤�ȴ���Դ����+1ʱ�����ȫ
 *
 * @return �ɹ��󷵻�BMZ_E_OK,ʧ�ܷ�����Ӧ�Ĵ�����
 */

int bm_decode_body(const void *src, size_t in_len, void *dst, size_t *out_len_p);

#endif //end define __BM_DIFF_H__
