#ifndef COMMON_VIVOLOG_VIVO_LOCK_H
#define COMMON_VIVOLOG_VIVO_LOCK_H

#include <pthread.h>
#include <time.h>

namespace vivo {

class CMutexLock {
public:
	CMutexLock();
	virtual ~CMutexLock();

	int lock();
	int trylock();
	int unlock();

public:
	pthread_mutex_t m_lock;
};

class CCond : public CMutexLock {
public:
	CCond();
	~CCond();

	int signal();
	int boardcast();
	int wait();
	int timedwait(struct timespec *ts);
	int timedwait(int ms);

public:
	pthread_cond_t m_cond;
	volatile int   m_waiters;
	volatile unsigned char m_signal;
};

class CAutoLock {
public:
	CAutoLock(CMutexLock * lock)
	{
		m_lock = lock;
		m_lock->lock();
	}

	~CAutoLock()
	{
		m_lock->unlock();
	}

public:
	CMutexLock *m_lock;
};

/*
struct timespec {
	time_t tv_sec; // seconds
	long tv_nsec;  // and nanoseconds
};
*/

}

#endif // COMMON_VIVOLOG_VIVO_LOCK_H
