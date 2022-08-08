#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "vivolock.h"
#include "vivolog.h"
#include "vivolog_util.h"
#include "mempool.h"

using namespace vivo;

static const char * LEVEL[VIVO_LOG_LEVEL_OFF] = { "DEBUG","INFO","WARN","ERROR","FATAL" };

std::string json_format(int level, const char *filename, const char *func, int line);

class CVivoLog {
public:
	CVivoLog()
	{
		m_stamp = 0;
		m_file = NULL;
		m_filename = "";
		m_level = VIVO_LOG_LEVEL_INFO;
		m_nRolling = BIG_DATA_ROLL_LOG_SECONDS;

		m_hostname = vivo::get_host_name();

		m_running = 0;
		m_tid = 0;

		m_queue = NULL;
		m_cache = NULL;
	}

	~CVivoLog()
	{
		stop();
		LOG_CONSOLE("%s", "log end");
	}

	void stop()
	{
		m_running = 0;

		if (m_tid > 0) {
			m_cond.signal();
			//m_cond.boardcast();

			pthread_join(m_tid, NULL);
			//pthread_cancel(m_tid);

			m_tid = 0;
		}

		int chunkbytes = 0;
		long nMalloc = 0;

		if (m_cache) {
			nMalloc = m_cache->calculate();
			chunkbytes = m_cache->len();

			delete m_cache;
			m_cache = NULL;
		}


		char buf[1024 * 10];
		std::string message = json_format(VIVO_LOG_LEVEL_INFO,"",__FUNCTION__,__LINE__);
		snprintf(buf, sizeof(buf) - 1, "%s chunkbytes %d nMalloc %ld bytes %ld\"}\r\n", message.c_str(), chunkbytes, nMalloc, chunkbytes * nMalloc);

		if (m_file) {
			fprintf(m_file, buf);

			fclose(m_file);
			m_file = NULL;

			//LOG("%s", buf);
		}

	}

	int init(const char * file, int level, int chunkbytes)
	{
		m_filename = file;
		m_level = level;

		time_t t = time(NULL);
		int ret = open(t);
		if (0 != ret) {
			LOG_CONSOLE("ret %d loglevel %d filename %s fail", ret, m_level, m_filename.c_str());
			return ret;
		}

		m_cache = new CMemPool(chunkbytes);
		if (NULL == m_cache) {
			LOG_CONSOLE("new CMemPool fail");
			return -2;
		}

		ret = pthread_create(&m_tid, NULL, CVivoLog::thread, this);
		if (0 != ret) {
			LOG_CONSOLE("pthread_create CVivoLog::thread fail, app exit");
			exit(0);
			return -3;
		} else {
			m_running = 1;
		}

		//LOG("level %d filename %s ret %d",m_level,m_filename.c_str(),ret);

		return ret ;
	}

	int open(time_t t)
	{
		if (m_file) {
			fclose(m_file);
			m_file = NULL;
		}

		t = (t / m_nRolling) * m_nRolling;
		m_stamp = t;

		std::string hour_filename = vivo::log_rolling_filename(m_filename, t);
		vivo::mkdir(hour_filename.c_str());
		vivo::clear_expire_file(hour_filename, 1);

		m_file = fopen(hour_filename.c_str(), "a");
	    if (!m_file) {
	        LOG_CONSOLE("fopen %s fail", hour_filename.c_str());
	        return -1;
	    }

		//LOG("%s",hour_filename.c_str());

		return 0;
	}

	inline int level()
	{
		return m_level;
	}

	inline int level(int level)
	{
		int ret = m_level;
		m_level = level;
		return ret;
	}

	int roll(long t)
	{
		return (t - m_stamp) > m_nRolling;
	}

	static void * thread(void *param)
	{
		CVivoLog * obj = (CVivoLog*) param;
		obj->run();
		return NULL;
	}

	void run()
	{
		mem_pool_node_t *node = NULL;

		m_running = 1;

		LOG_CONSOLE("log thread m_running %d",m_running);

		while (running() || m_queue) {
			m_lock.lock();
			node = m_queue;
			m_queue = NULL;
			m_lock.unlock();

			if (node) {
				flush(node);
			}

			time_t now = time(NULL);
			if (roll (now)) {
				open(now);
			}

			if (running()) {
				m_cond.timedwait(100);
				//LOG("timedwait m_queue %p tick %ld",m_queue,++tick);
			}
		}

		LOG_CONSOLE("log thread exit %d",m_running);
	}

	void flush(mem_pool_node_t *node)
	{
		mem_pool_node_t *next = NULL;
		long tick = 0;
		node = reverse(node);

		while (node) {
			next = node->next;

			if (m_file && node->data) {
				int cnt = 0;
				int len = node->datalen;
				while (len-- != 0) {
					if (node->data[len] == '%') ++cnt;
				}
				int old_len = node->datalen;
				int new_len = node->datalen + cnt;
				char *output = (char*)malloc(new_len);
				while(new_len >= 0) {
					if (node->data[old_len] != '%') {
						output[new_len] = node->data[old_len];
					} else {
						output[new_len--] = '%';
						output[new_len] = node->data[old_len];
					}
					old_len--;
					new_len--;
				}
				
				fprintf(m_file, output);

				if (output) {
					free(output);
					output = NULL;
				}
			} else {
				//LOG("%s", node->data);
			}

			m_cache->Free(node);

			node = next;

			tick++;
		}

		if (tick && m_file) {
			fflush(m_file);
		}

	}

	int add(mem_pool_node_t *node)
	{
		if (NULL == node) {
			return -1;
		}

		if (!running()) {
			m_cache->Free(node);
			return -2;
		}

		node->data[node->datalen] = '\0';
		node->next = NULL;

		//LOG("%s",node->data);

		m_lock.lock();

		//if (NULL != m_queue_tail) {
		//	m_queue_tail->next = node;
		//} else {
		//	m_queue_head = node;
		//	m_queue_tail = node;
		//}

		node->next = m_queue;
		m_queue = node;

		m_lock.unlock();

		m_cond.signal();

		return 0;
	}

	mem_pool_node_t * reverse(mem_pool_node_t *node)
	{
		mem_pool_node_t *head = NULL, *next = NULL;

		while (node) {
			next = node->next;

			node->next = head;
			head = node;

			node = next;
		}

		return head;
	}

	mem_pool_node_t* chunk()
	{
		mem_pool_node_t * node = NULL;
		if (NULL == m_cache) {
			return NULL;
		}
		node = (mem_pool_node_t*) m_cache->Malloc();
		if (NULL == node) {
			return NULL;
		}
		node->capacity = m_cache->len() - sizeof(mem_pool_node_t) - 1;
		node->datalen = 0;
		node->next = NULL;
		return node;
	}

	inline bool running()
	{
		return (1 == m_running);
	}

public:
	std::string m_filename;
	FILE * m_file;
	time_t m_stamp;
	int m_nRolling;
	CMutexLock m_lock;
	int m_level;
	std::string m_hostname;

	CCond m_cond;
	mem_pool_node_t *m_queue;
	mem_pool_node_t *m_queue_head;
	mem_pool_node_t *m_queue_tail;
	CMemPool *m_cache;
	pthread_t m_tid;
	int m_running;
};

CVivoLog g_vivo_log;

int vivolog_init(const char *filename, int level, int chunkbytes)
{
	return g_vivo_log.init(filename, level, chunkbytes);
}

int vivolog_stop()
{
	g_vivo_log.stop();
	return 0;
}

int vivolog_set_level(int level)
{
	return g_vivo_log.level(level);
}

int vivolog_should_log(int level)
{
	return (level >= g_vivo_log.level()) && g_vivo_log.running();
}

std::string json_format(int level, const char *filename, const char *func, int line)
{
	char buf[100] = { 0 };
	time_t now = time(NULL);
	struct tm tm;
	gmtime_r(&now, &tm);
	strftime(buf, sizeof(buf) - 1, "%Y-%m-%dT%H:%M:%S.000Z", &tm);

	std::string message = "{";

	message += "\"@timestamp\":\"";
	message += buf;

	message += "\",\"level\":\"";
	message += LEVEL[level%VIVO_LOG_LEVEL_OFF];

    sprintf(buf, "%u", (uint32_t)syscall(SYS_gettid));
	message += "\",\"thread_name\":\"";
    message += buf;
	message += "\",\"logger_name\":\"";
	message += filename;

	message += "\",\"source_host\":\"";
	message += g_vivo_log.m_hostname;

	message += "\",\"mdc\":{},";

	message += "\"file\":\"";
	message += filename;

	message += "\",\"method\":\"";
	message += func;

	sprintf(buf, "%d", line);
	message += "\",\"line_number\":\"";
	message += buf;

	message += "\",\"class\":\"";

	message += "\",\"@version\":\"1";

	message += "\",\"message\":\"";

	//message += "\"}";

	return message;
}

void vivolog_log(int level, const char* file, const char *func, int line, const char* format, ...)
{
	if (vivolog_should_log(level) == 0) {
		return;
	}

	// 文件路径
	const char * displayfile = strrchr(file, '/');
	if (NULL == displayfile) {
	  	displayfile = file;
	} else {
	  	displayfile = displayfile + 1;
	}

	// 拼 json 格式
	char buf[1024 * 20];
	std::string message = json_format(level, displayfile, func, line);
	snprintf(buf, sizeof(buf) - 1, "%s%s\"}\r\n", message.c_str(), format);

	// 块内存
	mem_pool_node_t * node = g_vivo_log.chunk();
	if (NULL == node) {
		return ;
	}

	// 日志写内存
	va_list valist;
	va_start(valist, format);
	node->datalen = vsnprintf(node->data, node->capacity, buf, valist);
	va_end(valist);

	if (node->datalen <= 0) {
		g_vivo_log.m_cache->Free(node);
		return ;
	}

	if (node->datalen >= node->capacity)
	{
		snprintf(node->data + node->capacity - 5, 5, "\"}\r\n");
		node->datalen = node->capacity - 1;
	}

	// 添加到队列
	g_vivo_log.add(node);
	//LOG("%s", node->data);
	//g_vivo_log.m_cache->Free(node);
}

