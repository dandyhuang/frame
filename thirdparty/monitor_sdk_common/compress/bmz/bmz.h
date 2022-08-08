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
 * @brief bmz初始化，主要初始化lzo
 * 
 * @return 成功返回BMZ_E_OK
 */

int	bmz_init();

/** 
 * @brief 计算bmz编码所需要辅助内存大小
 * 
 * @param in_len 输入长度
 * @param fp_len 探针长度
 * 
 * @return 辅助内存长度
 */

size_t	bmz_pack_worklen(size_t in_len, size_t fp_len);

/** 
 * @brief 计算bmz解码所需要辅助内存大小
 * 
 * @param in_len 输出长度
 * 
 * @return 辅助内存长度
 */

size_t	bmz_unpack_worklen(size_t out_len);

/** 
 * @brief bmz编码
 * 
 * @param in 源地址
 * @param in_len 源长度
 * @param out 目标地址
 * @param out_len_p 目标长度指针
 * @param fp_len 探针长度
 * @param work_mem 辅助内存,内存大小通过bmz_pack_worklen计算
 * 
 * @return 成功返回BMZ_E_OK，失败返回相应的错误码
 */

int	bmz_pack(const void *in,  size_t in_len, void *out, size_t *out_len_p, size_t	fp_len, void *work_mem);

/** 
 * @brief bmz解码
 * 
 * @param in 源地址
 * @param in_len 源长度
 * @param out 目标地址
 * @param out_len_p 目标长度指针
 * @param work_mem 辅助内存，内存大小通过bmz_unpack_worklen计算
 * 
 * @return 成功返回BMZ_E_OK,失败返回相关的错误码
 */

int	bmz_unpack(const void *in, size_t in_len, void *out, size_t	*out_len_p,  void *work_mem);


#endif ///COMMON_COMPRESS_BMZ_H_
