#ifndef COMMON_VIVOLOG_VIVO_TRACE_H_
#define COMMON_VIVOLOG_VIVO_TRACE_H_

#include <vector>
#include <string>
#include <map>

#include "mempool.h"
#include "vivolock.h"

#define SPLIT_BIG_DATA			"\001"
#define SPLIT_KV_PAIR			"\002"
#define SPLIT_KEY_VALUE			"\003"
#define SPLIT_COMMA				","

using namespace vivo;

//namespace vivo {

class CTrace;

class CSession {
public:
	CSession(CTrace *obj);
	virtual ~CSession();

	int add(long value);
	int add(const std::vector<long> & value);
	int add(const char * value);
	int add(const char * value, int len);
	int add(const std::string &value);
	int add(const std::vector<std::string> & value);
	int print(const char * format, ...);
	mem_pool_node_t *chunk(int len);

public:
	CSession * m_next;
	CTrace * m_trace;
	std::vector<mem_pool_node_t*> m_chunks;
};

class CTrace : public CMemPool {
public:
	CTrace(const std::string & filename, int chunkbytes = 1024 * 1024);
	virtual ~CTrace();

	//CSession *get();
	//int add(CSession * obj);

	virtual CSession * get() {
		if (0 == m_running) {
			return NULL;
		}
		return new CSession(this);
	}

	virtual int add(CSession * obj) {
		if (obj) {
			m_queuelock.lock();
			obj->m_next = m_queue;
			m_queue = obj;
			m_queuelock.unlock();

			m_cond.signal();
		}
		return 0;
	}

	void run();
	static void *thread(void *param);
	CSession * reverse(CSession *node);

public:
	std::string m_filename;

	//CSession *m_queue;
	CSession *m_queue;
	CMutexLock m_queuelock;

	CCond m_cond;
	pthread_t m_tid;
	int m_running;
};

//} // vivo

#endif // COMMON_VIVOLOG_VIVO_TRACE_H_

