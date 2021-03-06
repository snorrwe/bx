/*
 * Copyright 2010-2018 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx#license-bsd-2-clause
 */

#include "bx_p.h"
#include <bx/mutex.h>

#if BX_CONFIG_SUPPORTS_THREADING

#if    BX_PLATFORM_ANDROID \
	|| BX_PLATFORM_LINUX   \
	|| BX_PLATFORM_IOS     \
	|| BX_PLATFORM_OSX     \
	|| BX_PLATFORM_PS4     \
	|| BX_PLATFORM_RPI
#	include <pthread.h>
#elif  BX_PLATFORM_WINDOWS \
	|| BX_PLATFORM_WINRT   \
	|| BX_PLATFORM_XBOXONE
#	include <windows.h>
#	include <errno.h>
#endif // BX_PLATFORM_

namespace bx
{
#if    BX_PLATFORM_WINDOWS \
	|| BX_PLATFORM_XBOXONE \
	|| BX_PLATFORM_WINRT
	typedef CRITICAL_SECTION pthread_mutex_t;
	typedef unsigned pthread_mutexattr_t;

	inline int pthread_mutex_lock(pthread_mutex_t* _mutex)
	{
		EnterCriticalSection(_mutex);
		return 0;
	}

	inline int pthread_mutex_unlock(pthread_mutex_t* _mutex)
	{
		LeaveCriticalSection(_mutex);
		return 0;
	}

	inline int pthread_mutex_trylock(pthread_mutex_t* _mutex)
	{
		return TryEnterCriticalSection(_mutex) ? 0 : EBUSY;
	}

	inline int pthread_mutex_init(pthread_mutex_t* _mutex, pthread_mutexattr_t* /*_attr*/)
	{
#if BX_PLATFORM_WINRT
		InitializeCriticalSectionEx(_mutex, 4000, 0);   // docs recommend 4000 spincount as sane default
#else
		InitializeCriticalSection(_mutex);
#endif // BX_PLATFORM_
		return 0;
	}

	inline int pthread_mutex_destroy(pthread_mutex_t* _mutex)
	{
		DeleteCriticalSection(_mutex);
		return 0;
	}
#endif // BX_PLATFORM_

	Mutex::Mutex()
	{
		BX_STATIC_ASSERT(sizeof(pthread_mutex_t) <= sizeof(m_internal) );

		pthread_mutexattr_t attr;

#if    BX_PLATFORM_WINDOWS \
	|| BX_PLATFORM_XBOXONE \
	|| BX_PLATFORM_WINRT
#else
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#endif // BX_PLATFORM_

		pthread_mutex_t* handle = (pthread_mutex_t*)m_internal;
		pthread_mutex_init(handle, &attr);
	}

	Mutex::~Mutex()
	{
		pthread_mutex_t* handle = (pthread_mutex_t*)m_internal;
		pthread_mutex_destroy(handle);
	}

	void Mutex::lock()
	{
		pthread_mutex_t* handle = (pthread_mutex_t*)m_internal;
		pthread_mutex_lock(handle);
	}

	void Mutex::unlock()
	{
		pthread_mutex_t* handle = (pthread_mutex_t*)m_internal;
		pthread_mutex_unlock(handle);
	}

} // namespace bx

#endif // BX_MUTEX_H_HEADER_GUARD
