//
// Mutex_POSIX.h
//
// $Id: //poco/1.4/Foundation/include/Poco/Mutex_POSIX.h#2 $
//
// Library: Foundation
// Package: Threading
// Module:  Mutex
//
// Definition of the MutexImpl and FastMutexImpl classes for POSIX Threads.
//
// Copyright (c) 2004-2008, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#ifndef Foundation_Mutex_POSIX_INCLUDED
#define Foundation_Mutex_POSIX_INCLUDED


#include "Poco/Foundation.h"
#include "Poco/Exception.h"
#include <pthread.h>
#include <errno.h>
#include <string>


namespace Poco {


class Foundation_API MutexImpl
{
protected:
	MutexImpl();
	MutexImpl(bool fast);
	~MutexImpl();
	void lockImpl();
	bool tryLockImpl();
	bool tryLockImpl(long milliseconds);
	void unlockImpl();
	
private:
	pthread_mutex_t _mutex;
};


class Foundation_API FastMutexImpl: public MutexImpl
{
protected:
	FastMutexImpl();
	~FastMutexImpl();
};


//
// inlines
//
inline void MutexImpl::lockImpl()
{
    int n=0;
	if ((n=pthread_mutex_lock(&_mutex)))
    {
//        perror("pthread_mutex_lock");
//        int mm = errno;
//        int m2=EDEADLK;
//        int m3=EINVAL;
//        std::string s = strerror(errno);
        throw SystemException("cannot lock mutex");
    }
}


inline bool MutexImpl::tryLockImpl()
{
	int rc = pthread_mutex_trylock(&_mutex);
	if (rc == 0)
		return true;
	else if (rc == EBUSY)
		return false;
	else
		throw SystemException("cannot lock mutex");
}


inline void MutexImpl::unlockImpl()
{
	if (pthread_mutex_unlock(&_mutex))
		throw SystemException("cannot unlock mutex");
}


} // namespace Poco


#endif // Foundation_Mutex_POSIX_INCLUDED