#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>

#include "vivolog.h"
#include "vivotrace.h"
#include "vivolog_util.h"

using namespace vivo;

//namespace vivo {

CSession::CSession(CTrace *obj)
{
	m_trace = obj;
	m_next = NULL;
}

CSession::~CSession()
{
	for (int i = 0; i < (int)m_chunks.size(); i++) {
		m_trace->Free(m_chunks[i]);
	}
	m_chunks.clear();
}

mem_pool_node_t * CSession::chunk(int len)
{
	mem_pool_node_t *node = NULL;

	if (m_chunks.size() > 0) {
		node = m_chunks[ m_chunks.size() - 1 ];
		if ((node->datalen + len) >= node->capacity) {
			node = NULL;
		}
	}

	if (NULL == node) {
		node = (mem_pool_node_t*) m_trace->Malloc();
		if (NULL != node) {
			m_chunks.push_back(node);
		}
	}

	return node;
}

int CSession::add(long value)
{
	mem_pool_node_t *node = chunk(32);

	if (node) {
		node->datalen += snprintf(node->data + node->datalen, node->capacity - node->datalen, "%ld", value);
		return 0;
	}

	return -1;
}

int CSession::add(const std::vector<long> & value)
{
	mem_pool_node_t *node = chunk((int) (value.size() * 32));

	if (NULL == node) {
		return -1;
	}

	for (int i = 0; i < (int) value.size(); i++) {
		if (0 == i) {
			node->datalen += snprintf(node->data + node->datalen, node->capacity - node->datalen, "%ld", value[i]);
		} else {
			node->datalen += snprintf(node->data + node->datalen, node->capacity - node->datalen, SPLIT_COMMA "%ld", value[i]);
		}
	}

	return 0;
}

int CSession::add(const char * value)
{
	int len = strlen(value);
	return add(value, len);
}

int CSession::add(const char * value, int len)
{
	mem_pool_node_t *node = chunk(len);

	if (node) {
		node->datalen += snprintf(node->data + node->datalen, node->capacity - node->datalen, "%s", value);
		return 0;
	}

	return -1;
}

int CSession::add(const std::string & value)
{
	return add(value.c_str(), (int) value.size());
}

int CSession::add(const std::vector<std::string> & value)
{
	mem_pool_node_t *node = NULL;

	for (int i = 0; i < (int) value.size(); i++) {
		int length = value[i].size() + 4;

		if ((NULL == node) || ((node->capacity - node->datalen) < length)) {
			node = chunk(length);
		}

		if (NULL == node) {
			return -1;
		}

		if (0 == i) {
			node->datalen += snprintf(node->data + node->datalen, node->capacity - node->datalen, "[\"%s\"", value[i].c_str());
		} else {
			node->datalen += snprintf(node->data + node->datalen, node->capacity - node->datalen, SPLIT_COMMA "\"%s\"", value[i].c_str());
		}

		if ((int) value.size() == (i + 1)) {
			node->datalen += snprintf(node->data + node->datalen, node->capacity - node->datalen, "]");
		}
	}

	return 0;
}

int CSession::print(const char * format, ...)
{
	mem_pool_node_t *node = (mem_pool_node_t*) m_trace->Malloc();

	if (NULL == node) {
		return -1;
	}

	va_list valist;
	va_start(valist, format);
	node->datalen += vsnprintf(node->data + node->datalen, node->capacity - node->datalen, format, valist);
	va_end(valist);

	if ((node->datalen <= 0) || (node->datalen >= node->capacity)) {
		m_trace->Free(node);
		return -2;
	}

	m_chunks.push_back(node);

	return 0;
}


////////////////

CTrace::CTrace(const std::string & filename, int chunkbytes) : CMemPool(chunkbytes)
{
	m_tid = 0;
	m_running = 0;
	m_queue = NULL;
	m_filename = filename;

	int ret = pthread_create(&m_tid, NULL, CTrace::thread, this);
	if (0 != ret) {
		LOG_ERROR("pthread_create trace fail, app exit");
		exit(0);
	}
}

CTrace::~CTrace()
{
	m_running = 0;

	if (m_tid > 0) {
		m_cond.signal();
		pthread_join(m_tid, NULL);
		m_tid = 0;
	}

	CSession *node = NULL, *next = NULL;

	node = m_queue;
	m_queue = NULL;
	while (node) {
		next = node->m_next;
		delete node;
		node = next;
	}

	int chunkbytes = len();
	long nMalloc = calculate();

	LOG_INFO("chunkbytes %d nMalloc %ld bytes %ld", chunkbytes, nMalloc, chunkbytes * nMalloc);
	//LOG("chunkbytes %d nMalloc %ld bytes %ld", chunkbytes, nMalloc, chunkbytes * nMalloc);

}

void* CTrace::thread(void *param)
{
	CTrace *obj = (CTrace*) param;
	obj->run();
	return NULL;
}

void CTrace::run()
{
	CSession *node = NULL, *next = NULL;
	long tick = 0;
	FILE *fp = NULL;
	long stamp, now;

	now = stamp = time(NULL);
	m_running = 1;

	LOG_CONSOLE("trace thread m_running %d", m_running);

	while (m_running || m_queue) {
		tick = 0;
		now = time(NULL);

		m_queuelock.lock();
		node = m_queue;
		m_queue = NULL;
		m_queuelock.unlock();

		if (/*node &&*/ (NULL == fp)) { // 打开文件
			//   /data/ads-indexserver/logs/stat/stat_ads-index_recall-log-index.log
			//   /data/ads-indexserver/logs/stat/stat_ads-index_recall-log-index.log.2019051512

			std::string hour_filename = vivo::log_rolling_filename(m_filename, now);
			vivo::mkdir(hour_filename.c_str());
			vivo::clear_expire_file(hour_filename, 1); // 清理过期文件

			fp = fopen(hour_filename.c_str(), "a");
			if (NULL == fp) {
				LOG_ERROR("%s fopen fail.",hour_filename.c_str());
			}
		}

		node = reverse(node);

		while (node) {	// 遍历日志
			next = node->m_next;

			if (fp && m_running) {	// 一条日志
				for (int i = 0; i < (int)node->m_chunks.size(); i++) {
					mem_pool_node_t * pnode = node->m_chunks[i];
					fwrite(pnode->data, 1, pnode->datalen, fp);
				}

				//fwrite("\r\n", 1, 2, fp);
			}

			delete node; //

			node = next;
			tick++;
		}

		if (tick && fp) {
			fflush(fp);
		}

		if ((now / BIG_DATA_ROLL_LOG_SECONDS) != (stamp / BIG_DATA_ROLL_LOG_SECONDS) ) { // 大数据要求按小时切割文件
			stamp = now;

			if (NULL != fp) {  // 关闭文件
				fclose(fp);
				fp = NULL;
			}
		}

		if (m_running && !m_queue) {
			m_cond.timedwait(20);
			//INFO("timedwait m_queue %p tick %ld", m_queue, ++tick);
		}
	}

	m_running = 0;

	if (NULL != fp) {  // 关闭文件
		fclose(fp);
		fp = NULL;
	}

	LOG_CONSOLE("trace thread exit %d", m_running);
}

CSession * CTrace::reverse(CSession *node)
{
	CSession *head = NULL, *next = NULL;

	while (node) {
		next = node->m_next;

		node->m_next = head;
		head = node;

		node = next;
	}

	return head;
}


//} // vivo

