#include <stdlib.h>
#include "mempool.h"
#include "vivolog.h"

namespace vivo {

CMemPool::CMemPool(int len)
{
	m_idle = NULL;
	m_len = len;
	m_malloc = 0;

	int align = 4096;

	if (0 != (m_len % align)) {
		m_len = ((m_len + align) / align) * align;
	}
}

CMemPool::~CMemPool()
{
	mem_pool_node_t *node = (mem_pool_node_t*) m_idle;
	mem_pool_node_t *next = NULL;
	long count = 0;

	m_idle = NULL;

	while (node) {
		next = node->next;
		free(node);
		node = next;

		count++;
	}

	LOG_CONSOLE("chunkbytes %d m_malloc %ld free %ld bytes %ld", m_len, m_malloc, count, m_len * m_malloc);
}

long CMemPool::calculate()
{
	mem_pool_node_t *node = (mem_pool_node_t*) m_idle;
	long count = 0;

	while (node) {
		node = node->next;
		count++;
	}

	return count;
}

void* CMemPool::Malloc()
{
	mem_pool_node_t *node = NULL;

	m_lock.lock();
	node = (mem_pool_node_t*) m_idle;
	if (node) {
		m_idle = node->next;
	}
	m_lock.unlock();

	if (NULL == node) {
		node = (mem_pool_node_t*) malloc(m_len);

		m_lock.lock();
		m_malloc++;
		m_lock.unlock();
	}

	if (node) {
		node->next = NULL;
		node->capacity = m_len - sizeof(mem_pool_node_t);
		node->datalen = 0;
	}

	return node;
}

void CMemPool::Free(void *ptr)
{
	mem_pool_node_t *node = (mem_pool_node_t*) ptr;
	if (node) {
		m_lock.lock();
		node->next = (mem_pool_node_t*) m_idle;
		m_idle = node;
		m_lock.unlock();
	}
}

} // vivo

