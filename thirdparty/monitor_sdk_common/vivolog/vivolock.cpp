#include <errno.h>
#include <sys/time.h>

#include "vivolock.h"

namespace vivo {

enum  { nosig, normalsig, castsig };	
#define NOSIGNAL(x) (!((x)&0x3))
#define CASTSIGNAL(x) ((x)&castsig)
#define NORMALSIGNAL(x) ((x)&normalsig)

CMutexLock::CMutexLock()
{
	pthread_mutex_init(&m_lock, NULL);
}

CMutexLock::~CMutexLock()
{
	pthread_mutex_destroy(&m_lock);
}

int CMutexLock::lock()
{
	return pthread_mutex_lock(&m_lock);
}

int CMutexLock::trylock()
{
	return pthread_mutex_trylock(&m_lock);
}

int CMutexLock::unlock()
{
	return pthread_mutex_unlock(&m_lock);
}


CCond::CCond()
{
	m_waiters = 0;
	m_signal = nosig;

	pthread_cond_init(&m_cond, NULL);
}

CCond::~CCond()
{
	int retry = 0;
	while ((pthread_cond_destroy(&m_cond) == EBUSY) && (retry++ < 10)) {
		boardcast();
	}
}

int CCond::signal()
{
	lock();
	if (m_waiters) {
		m_signal = normalsig;
		pthread_cond_signal(&m_cond);
	}
	unlock();
	return 0;
}

int CCond::boardcast()
{
	lock();
	if (m_waiters) {	
		m_signal = castsig;
		pthread_cond_broadcast(&m_cond);
	}
	unlock();
	return 0;

}

int CCond::wait()
{
	lock();
	m_waiters++;
	while (NOSIGNAL(m_signal)) {
		pthread_cond_wait(&m_cond, &m_lock);
	}
	m_waiters--;
	if ((0 == m_waiters) || (NORMALSIGNAL(m_signal))) {
		m_signal = nosig;
	}
	unlock();

	return 0;
}

int CCond::timedwait(struct timespec *ts)
{
	int ret = 0, result = 0;
	
	lock();
	m_waiters++;
	while (NOSIGNAL(m_signal)) {
		result = ret = pthread_cond_timedwait(&m_cond, &m_lock, ts);
		if (ETIMEDOUT == ret) { // timeout
			ret = 0;
			break;
		} else if (0 == ret) {  // signal
			
		} else { // other
			break;
		}
	}
	m_waiters--;
	if ((0 == result) && (NORMALSIGNAL(m_signal) || (0 == m_waiters))) {
		m_signal = nosig;
	}
	unlock();

	return ret;
}

int CCond::timedwait(int ms)
{
	struct timeval tv;
	struct timespec ts;

	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec;
	ts.tv_nsec = tv.tv_usec * 1000;

	ts.tv_sec  += ms / 1000;
	ts.tv_nsec += (ms % 1000) * 1000000;

	if (ts.tv_nsec >= 1000000000) {
		ts.tv_sec++;
		ts.tv_nsec -= 1000000000;
	}

	return timedwait(&ts);
}

}// libindex

