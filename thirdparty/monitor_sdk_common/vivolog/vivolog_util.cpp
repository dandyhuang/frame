#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "vivolog_util.h"

namespace vivo {

std::string get_host_name()
{
	char hostname[2048] = { 0 };
	if (gethostname(hostname, sizeof(hostname))) {
		printf("gethostname fail. %s\r\n", strerror(errno));
		return "";
	}
	return hostname;
}

void mkdir(const char *path)
{
	char buf[4096] = { 0 };
	char cmd[4096] = { 0 };

	snprintf(buf, sizeof(buf)-1, "%s", path);
	char *p = strrchr(buf, '/');
	if (p && (p != &(buf[0]))) {
		*p = '\0';
		snprintf(cmd, sizeof(cmd)-1, "mkdir -p %s", buf);
		system(cmd);
	}
}

std::string file2dir(const std::string & path)
{
	char buf[4096] = { 0 };

	snprintf(buf, sizeof(buf)-1, "%s", path.c_str());
	char *p = strrchr(buf, '/');
	if (p) {
		p++;
		*p = '\0';
		return buf;
	}

	return "";
}

int clear_expire_file(const std::string & path, int days)
{
	std::string dir = file2dir(path);
	if (dir == "") {
		return 1;
	}

	if ((0 != access(dir.c_str(), F_OK | R_OK))) {
		return -1;
	}

	// find /data/ads-indexserver/logs/biz/ -mtime +1 -name "*" -exec rm -rf {} \;	>/dev/null 2>&1

	char buf[4096] = { 0 };
	snprintf(buf, sizeof(buf)-1, " -mtime +%d -name \"*\" -exec rm -rf {} \\;	>/dev/null 2>&1", days);

	std::string cmd = "find ";
	cmd += dir;
	cmd += buf;

	system(cmd.c_str());

	//printf("%s\r\n", cmd.c_str());
	return 0;
}


std::string log_rolling_filename(std::string & filename, time_t t)
{
	struct tm newtime;
	char stamp[256] = { 0 };
	char buf[4096] = { 0 };

	t = (t / BIG_DATA_ROLL_LOG_SECONDS) * BIG_DATA_ROLL_LOG_SECONDS;
	localtime_r(&t, &newtime);
	strftime(stamp, sizeof(stamp)-1, "%Y%m%d%H", &newtime);

	snprintf(buf, sizeof(buf)-1, "%s.%s", filename.c_str(), stamp);

	return buf;
}

}


