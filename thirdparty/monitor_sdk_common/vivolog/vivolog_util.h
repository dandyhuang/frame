#ifndef COMMON_VIVOLOG_VIVO_LOG_UTIL_H_
#define COMMON_VIVOLOG_VIVO_LOG_UTIL_H_

#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>

#define BIG_DATA_ROLL_LOG_SECONDS 3600

namespace vivo {

using namespace std;

std::string get_host_name();
void mkdir(const char *path);
std::string file2dir(const std::string & path);
int clear_expire_file(const std::string & path, int days);
std::string log_rolling_filename(std::string & filename, time_t t);

}

#endif // COMMON_VIVOLOG_VIVO_LOG_UTIL_H_

