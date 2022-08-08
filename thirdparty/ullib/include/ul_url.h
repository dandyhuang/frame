/**
 * @file 
 * @brief URL处理库
 */
#include <stdio.h>
#ifndef __UL_URL_H__
#define __UL_URL_H__
#define  UL_MAX_URL_LEN_EX 1024
#define UL_MAX_SITE_LEN_EX 128
#define UL_MAX_PATH_LEN_EX 800
#define  UL_MAX_URL_LEN      256
#define  UL_MAX_SITE_LEN     48
#define  UL_MAX_PORT_LEN     7 //64k 5+:+1
#define UL_MAX_PATH_LEN 203

#define UL_MAX_URL_LEN_EX2 2048
#define UL_MAX_SITE_LEN_EX2 256
#define UL_MAX_PATH_LEN_EX2 1600


/**
 * 根据url解析出其中的各个部分
 *
 * @param input 输入的url
 * @param[out] site 站点名缓冲区
 * @param[out] port 端口
 * @param[out] path 路径
 * @return 1正常，0无效url格式
 */
int ul_parse_url(const char *input,char *site,char *port,char *path);

/**
 * 归一化URL路径
 *
 * @param[in,out] path 路径
 * @return 1正常，0无效url格式
 */
int ul_single_path(char *path);

/**
 * 根据url解析出其中的路径部分
 *
 * @param url 输入的url
 * @param[out] path 路径
 * @return NULL失败，否则为指向path的指针
 */
char *ul_get_path(const char *url, char *path);

/**
 * 根据url解析出其中的站点名部分
 *
 * @param url 输入的url
 * @param[out] site 站点名( be sure it is enough,larger than UL_MAX_SITE_LEN)
 * @return NULL失败，否则为指向site的指针
 */
char *ul_get_site(const char *url, char *site);

/**
 * 规范化路径的形式\n
 * 对'\\', './', '/.', '../', '/..', '//'等形式进行规范化
 *
 * @param[in,out] path 待转化的路径
 */
void ul_normalize_path(char *path);

/**
 * 从url中获取端口信息
 *
 * @param url 输入的url
 * @param[out] pport 端口
 * @return 1成功，0失败
 */
int ul_get_port(const char* url,int* pport);

/**
 * 从url中获取静态的部分（?或者;之前的部分）
 *
 * @param url 输入的url
 * @param[out] staticurl 静态部分缓冲区
 */
void ul_get_static_part(const char *url, char *staticurl);

/*
 * to compitable old version lib, still keep the non-const functions
 * see the above functions for detail
 */
int ul_parse_url(char *input,char *site,char *port,char *path);
char *ul_get_path(char *url, char *path);
char *ul_get_site(char *url, char *site);
int ul_get_port(char* url,int* pport);
void ul_get_static_part(char *url, char *staticurl);

/**
 * 判断url是否是动态url
 *
 * @param str url
 * @returnu 0不是动态url，非0，是动态url
 */
int ul_isdyn(char* str);

/**
 * 判断url是否合法
 *
 * @param url 输入url
 * @returnu 1合法，0不合法
 */
int ul_check_url(char* url);



/**
 * 从站点名中获取主干部分\n
 * @param site 站点名
 * @param[out] trunk 存放主干部分的缓冲区
 * @param size 缓冲区大小
 * @return 1成功，-1未知错误，-2站点没有主干部分，-3站点不包含'.'
 */
int ul_fetch_trunk(const char* site,char *trunk,int size);

/**
 * 检查站点名是否是IP地址
 *
 * @param sitename 站点名
 * @return 0不是，非0是
 */
int ul_is_dotip(const char *sitename);

/**
 * 从站点名中获取主干部分，功能类同@ref ul_fetch_trunk()
 *
 * @param site 站点名
 * @param[out] domain 存放主干部分的缓冲区
 * @param size 缓冲区大小
 * @return NULL失败，否则为指向site主干部分的指针
 */
const char* ul_fetch_maindomain(const char* site,char *domain,int size);

/**
 * 检查url是否规范化
 *
 * @param url 检查的url
 * @return 1是，0不是
 */
int ul_isnormalized_url(const char *url);

/**
 * 将url转化为统一的形式\n
 * 执行@ref ul_normalize_site, @ref ul_normalize_port, @ref ul_single_path, @ref ul_normalize_path
 *
 * @param url 待转化的url
 * @param[out] buf 转化后的url缓冲区
 * @return 1成功，0无效url
 * @note you can use ul_normalize_url(buf, buf) to save an extra buffer.
 */
int ul_normalize_url(const char* url, char* buf);

/**
 * 将站点名进行规范化（大写转为小写）
 *
 * @param site 站点名
 * @return 1成功，0失败
 */
int ul_normalize_site(char *site);

/**
 * 将端口字符串进行规范化（检查端口范围合法性，并去掉80端口的字符串）
 *
 * @param port 指向的端口的指针
 * @return 1成功，0失败
 */
int ul_normalize_port(char *port);



 
 
/**
 *  根据url解析出其中的各个部分,支持加长的url，最高可支持到1024，path最长可到800，site最长可到128
 *  
 *  @param[in]  input          输入的url
 *  @param[in]  site           site字段的存储buf指针
 *  @param[in]  site_size      site缓冲区的大小，可根据此字段设置合理的site长度,默认为128,使用时可适当调小.
 *  @param[in]  port           port字段的存储buf指针
 *  @param[in]  port_size      port字段的大小
 *  @param[in]  path           path字段的存储buf指针
 *  @param[in]  max_path_size  path字段的大小,可根据此字段设置合理的path长度,默认为800,使用时可适当调小.
 *  @param[out] site           site值
 *  @param[out] port           port值
 *  @param[out] path           path路径
 *  @return 函数操作结果
 *  - 1   表示正常
 *  - 0  无效url格式
 *  - @note 为保证程序安全,传入的buf请小于默认最大值
 */ 
int ul_parse_url_ex(const char *input, char *site,size_t site_size,
		                char *port, size_t port_size,char *path, size_t max_path_size);
                                
                                
                                
                                

/**
 *  根据url解析出其中的路径部分,支持加长的url，最高可支持到1024，path最长可到800，site最长可到128
 *  
 *  @param[in]  url          输入的url
 *  @param[in]  path         site字段的存储buf指针
 *  @param[in]  path_size    path字段的大小,可根据此字段设置合理的path长度,默认为800,使用时可适当调小.
 *  @param[out] path         path路径
 *  @return 函数操作结果
 *  - 非NULL   指向路径的指针
 *  - NULL     表示失败
 *  - @note 为保证程序安全,传入的path_size请小于默认最大值
 */
char *ul_get_path_ex(const char *url, char *path,size_t path_size);





/**
 *  根据url解析出其中的站点名部分,支持加长的url，最高可支持到1024，path最长可到800，site最长可到128
 *  
 *  @param[in]  url            输入的url
 *  @param[in]  site           site字段的存储buf指针
 *  @param[in]  site_size      site缓冲区的大小，可根据此字段设置合理的site长度,默认为128,使用时可适当调小.
 *  @param[out] site           site值
 *  @return 函数操作结果
 *  - 非NULL   指向site的指针
 *  - NULL     表示失败
 *  - @note 为保证程序安全,传入的site_size请小于默认最大值
 */ 
char *ul_get_site_ex(const char *url, char *site,  size_t site_size);




/**
 *  从url中获取端口信息,支持加长的url，最高可支持到1024，path最长可到800，site最长可到128
 *  
 *  @param[in]  input          输入的url
 *  @param[in]  pport          port字段的存储buf指针
 *  @param[out] pport          port值
 *  @return 函数操作结果
 *  - 1   表示成功
 *  - 0   表示失败
 */ 
int ul_get_port_ex(const char* url, int* pport);








/**
 *  将url转化为统一的形式\n,支持加长的url，最高可支持到1024，path最长可到800，site最长可到128
 *  执行@ref ul_normalize_site, @ref ul_normalize_port, @ref ul_single_path, @ref ul_normalize_path
 *
 *  @param[in]  url           待转化的url
 *  @param[in]  buf           转化后的url缓冲区
 *  @param[in]  buf_size      buf的大小
 *  @param[out] buf           转化后的url
 *  @return 函数操作结果
 *  - 1   成功
 *  - 0   无效url
 *  - @note 为保证程序安全,传入的site_size请小于默认最大值
 */ 
int ul_normalize_url_ex(const char* url, char* buf,size_t buf_size);






/**
 *  从url中获取静态的部分（?或者;之前的部分）,支持加长的url，最高可支持到1024，path最长可到800，site最长可到128
 *
 *  @param[in]  url                 输入的url
 *  @param[in]  staticurl           静态部分缓冲区
 *  @param[in]  staticurl_size      buf的大小
 *  @param[out] staticurl           静态部分
 *  @return 无
 */
void ul_get_static_part_ex(const char *url , char *staticurl ,size_t staticurl_size);







/**
 *  检查url是否规范化,支持加长的url，最高可支持到1024，path最长可到800，site最长可到128
 *
 *  @param[in]  url                 检查的url
 *  @param[out] 无
 *  @return 返回判断结果
 *  - 1   是
 *  - 0   不是
 */
int ul_isnormalized_url_ex(const char *url);




/**
 *  规范化路径的形式\n,支持加长的url，最高可支持到1024，path最长可到800，site最长可到128
 * 对'\\', './', '/.', '../', '/..', '//'等形式进行规范化
 *
 *  @param[in]  path           待转化的路径
 *  @param[out] path           转化后的路径
 *  @return 无
 */ 
void ul_normalize_path_ex(char *path);




/**
 *  归一化URL路径,支持加长的url，最高可支持到1024，path最长可到800，site最长可到128
 *
 *  @param[in]  path         path路径
 *  @param[out] path         归一化过的路径
 *  @return 返回归一化结果
 *  - 1   正常
 *  - 0   无效url格式路径
 */
int ul_single_path_ex(char *path);



/**
 *  判断url是否合法,支持加长的url，最高可支持到1024，path最长可到800，site最长可到128
 *
 *  @param[in]  url           待转化的url
 *  @param[out] 无
 *  @return 函数操作结果
 *  - 1   合法
 *  - 0   不合法
 */
int ul_check_url_ex(char *url);

/////////////////////////////////////////////////////////////////////
 
/**
 *  根据url解析出其中的各个部分,支持加长的url，最高可支持到2048，path最长可到1600，site最长可到256
 *  
 *  @param[in]  input          输入的url
 *  @param[in]  site           site字段的存储buf指针
 *  @param[in]  site_size      site缓冲区的大小，可根据此字段设置合理的site长度,默认为256,使用时可适当调小.
 *  @param[in]  port           port字段的存储buf指针
 *  @param[in]  port_size      port字段的大小
 *  @param[in]  path           path字段的存储buf指针
 *  @param[in]  max_path_size  path字段的大小,可根据此字段设置合理的path长度,默认为1600,使用时可适当调小.
 *  @param[out] site           site值
 *  @param[out] port           port值
 *  @param[out] path           path路径
 *  @return 函数操作结果
 *  - 1   表示正常
 *  - 0  无效url格式
 *  - @note 为保证程序安全,传入的buf请小于默认最大值
 */ 
int ul_parse_url_ex2(const char *input, char *site,size_t site_size,
		     char *port, size_t port_size,char *path, size_t max_path_size);
                                
                                
                                
                                

/**
 *  根据url解析出其中的路径部分,支持加长的url，最高可支持到2048，path最长可到1600，site最长可到256
 *  
 *  @param[in]  url          输入的url
 *  @param[in]  path         site字段的存储buf指针
 *  @param[in]  path_size    path字段的大小,可根据此字段设置合理的path长度,默认为1600,使用时可适当调小.
 *  @param[out] path         path路径
 *  @return 函数操作结果
 *  - 非NULL   指向路径的指针
 *  - NULL     表示失败
 *  - @note 为保证程序安全,传入的path_size请小于默认最大值
 */
char *ul_get_path_ex2(const char *url, char *path,size_t path_size);





/**
 *  根据url解析出其中的站点名部分,支持加长的url，最高可支持到2048，path最长可到1600，site最长可到256
 *  
 *  @param[in]  url            输入的url
 *  @param[in]  site           site字段的存储buf指针
 *  @param[in]  site_size      site缓冲区的大小，可根据此字段设置合理的site长度,默认为256,使用时可适当调小.
 *  @param[out] site           site值
 *  @return 函数操作结果
 *  - 非NULL   指向site的指针
 *  - NULL     表示失败
 *  - @note 为保证程序安全,传入的site_size请小于默认最大值
 */ 
char *ul_get_site_ex2(const char *url, char *site,  size_t site_size);




/**
 *  从url中获取端口信息,支持加长的url，最高可支持到2048，path最长可到1600，site最长可到256
 *  
 *  @param[in]  input          输入的url
 *  @param[in]  pport          port字段的存储buf指针
 *  @param[out] pport          port值
 *  @return 函数操作结果
 *  - 1   表示成功
 *  - 0   表示失败
 */ 
int ul_get_port_ex2(const char* url, int* pport);





/**
 *  将url转化为统一的形式\n,支持加长的url，最高可支持到2048，path最长可到1600，site最长可到256
 *  执行@ref ul_normalize_site, @ref ul_normalize_port, @ref ul_single_path, @ref ul_normalize_path
 *
 *  @param[in]  url           待转化的url
 *  @param[in]  buf           转化后的url缓冲区
 *  @param[in]  buf_size      buf的大小
 *  @param[out] buf           转化后的url
 *  @return 函数操作结果
 *  - 1   成功
 *  - 0   无效url
 *  - @note 为保证程序安全,传入的site_size请小于默认最大值
 */ 
int ul_normalize_url_ex2(const char* url, char* buf,size_t buf_size);






/**
 *  从url中获取静态的部分（?或者;之前的部分）,支持加长的url，最高可支持到2048，path最长可到1600，site最长可到256
 *
 *  @param[in]  url                 输入的url
 *  @param[in]  staticurl           静态部分缓冲区
 *  @param[in]  staticurl_size      buf的大小
 *  @param[out] staticurl           静态部分
 *  @return 无
 */
void ul_get_static_part_ex2(const char *url , char *staticurl ,size_t staticurl_size);







/**
 *  检查url是否规范化,支持加长的url，最高可支持到2048，path最长可到1600，site最长可到256
 *
 *  @param[in]  url                 检查的url
 *  @param[out] 无
 *  @return 返回判断结果
 *  - 1   是
 *  - 0   不是
 */
int ul_isnormalized_url_ex2(const char *url);



/**
 *  规范化路径的形式\n,支持加长的url，最高可支持到2048，path最长可到1600，site最长可到256
 * 对'\\', './', '/.', '../', '/..', '//'等形式进行规范化
 *
 *  @param[in]  path           待转化的路径
 *  @param[out] path           转化后的路径
 *  @return 无
 */ 
void ul_normalize_path_ex2(char *path);




/**
 *  归一化URL路径,支持加长的url，最高可支持到2048，path最长可到1600，site最长可到256
 *
 *  @param[in]  path         path路径
 *  @param[out] path         归一化过的路径
 *  @return 返回归一化结果
 *  - 1   正常
 *  - 0   无效url格式路径
 */
int ul_single_path_ex2(char *path);



/**
 *  判断url是否合法,支持加长的url，最高可支持到2048，path最长可到1600，site最长可到256
 *
 *  @param[in]  url           待转化的url
 *  @param[out] 无
 *  @return 函数操作结果
 *  - 1   合法
 *  - 0   不合法
 */
int ul_check_url_ex2(char *url);


#endif
