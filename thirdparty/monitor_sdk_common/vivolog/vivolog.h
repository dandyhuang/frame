#ifndef COMMON_VIVOLOG_VIVO_LOG_H
#define COMMON_VIVOLOG_VIVO_LOG_H

#include <stdio.h>

typedef enum vivolog_level {
  VIVO_LOG_LEVEL_DEBUG,
  VIVO_LOG_LEVEL_INFO,
  VIVO_LOG_LEVEL_WARN,
  VIVO_LOG_LEVEL_ERROR,
  VIVO_LOG_LEVEL_FATAL,
  VIVO_LOG_LEVEL_OFF,
} vivolog_level;

int  vivolog_init(const char *filename = "", int level = VIVO_LOG_LEVEL_INFO, int chunkbytes = 1024 * 64);
int  vivolog_stop();
int  vivolog_set_level(int level);
int  vivolog_should_log(int level);
void vivolog_log(int level, const char* file, const char *func, int line, const char* format, ...);

#define LOG_DEBUG(fmt,args...) 	if (vivolog_should_log(VIVO_LOG_LEVEL_DEBUG)) { vivolog_log(VIVO_LOG_LEVEL_DEBUG,__FILE__,__FUNCTION__,__LINE__,fmt,##args); }
#define LOG_INFO(fmt,args...) 	if (vivolog_should_log(VIVO_LOG_LEVEL_INFO))  { vivolog_log(VIVO_LOG_LEVEL_INFO,__FILE__,__FUNCTION__,__LINE__,fmt,##args); }
#define LOG_WARN(fmt,args...) 	if (vivolog_should_log(VIVO_LOG_LEVEL_WARN))  { vivolog_log(VIVO_LOG_LEVEL_WARN,__FILE__,__FUNCTION__,__LINE__,fmt,##args); }
#define LOG_ERROR(fmt,args...) 	if (vivolog_should_log(VIVO_LOG_LEVEL_ERROR)) { vivolog_log(VIVO_LOG_LEVEL_ERROR,__FILE__,__FUNCTION__,__LINE__,fmt,##args); }
#define LOG_FATAL(fmt,args...) 	if (vivolog_should_log(VIVO_LOG_LEVEL_ERROR)) { vivolog_log(VIVO_LOG_LEVEL_ERROR,__FILE__,__FUNCTION__,__LINE__,fmt,##args); }

//#ifndef LOG_CONSOLE
#define LOG_CONSOLE(fmt,args...) printf("%s %s %d\t" fmt "\r\n",__FILE__,__FUNCTION__,__LINE__,##args)
//#endif //

#endif // COMMON_VIVOLOG_VIVO_LOG_H

