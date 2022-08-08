#ifndef COMMON_VIVOLOG_MEM_POOL_H_
#define COMMON_VIVOLOG_MEM_POOL_H_

#include "vivolock.h"

namespace vivo {

class CMemPool {
public:
	CMemPool(int len = 1024 * 1024);
	~CMemPool();

	void *Malloc();
	void Free(void *ptr);
	int len() { return m_len; }
	long size() { return m_malloc; }
	long calculate();

public:
	int m_len;
	void *m_idle;
	CMutexLock m_lock;
	long m_malloc;
};

typedef struct mem_pool_node_t {
	struct mem_pool_node_t *next;
	int capacity;
	int datalen;
	char data[0];
} mem_pool_node_t;

} // vivo

#endif // COMMON_VIVOLOG_MEM_POOL_H_

