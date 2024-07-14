// This file is part of Notepad4.
// See License.txt for details about distribution and modification.
#pragma once

#include <windows.h>

#ifndef _WIN32_WINNT_VISTA
#define _WIN32_WINNT_VISTA	0x0600
#endif

#define USE_STD_ASYNC_FUTURE	0
#if USE_STD_ASYNC_FUTURE
#define USE_WIN32_PTP_WORK		0
#define USE_WIN32_WORK_ITEM		0
#elif _WIN32_WINNT >= _WIN32_WINNT_VISTA
#define USE_WIN32_PTP_WORK		1
#define USE_WIN32_WORK_ITEM		0
#else
#define USE_WIN32_PTP_WORK		0
#define USE_WIN32_WORK_ITEM		1
#endif

namespace Scintilla::Internal {

inline bool WaitableTimerExpired(HANDLE timer) noexcept {
	return WaitForSingleObject(timer, 0) == WAIT_OBJECT_0;
}

// MSVC Code Analysis
#ifndef _Acquires_lock_
#define _Acquires_lock_(x)
#define _Releases_lock_(x)
#endif
#ifndef _Acquires_shared_lock_
#define _Acquires_shared_lock_(x)
#define _Releases_shared_lock_(x)
#endif

// std::shared_mutex
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
class NativeMutex {
	SRWLOCK srwLock = SRWLOCK_INIT;
public:
	_Acquires_lock_(this->srwLock)
	void lock() noexcept {
		AcquireSRWLockExclusive(&srwLock);
	}
	_Releases_lock_(this->srwLock)
	void unlock() noexcept {
		ReleaseSRWLockExclusive(&srwLock);
	}
	_Acquires_shared_lock_(this->srwLock)
	void lock_shared() noexcept {
		AcquireSRWLockShared(&srwLock);
	}
	_Releases_shared_lock_(this->srwLock)
	void unlock_shared() noexcept {
		ReleaseSRWLockShared(&srwLock);
	}
};

#else
class NativeMutex {
	CRITICAL_SECTION section;
public:
	NativeMutex() noexcept {
		InitializeCriticalSectionAndSpinCount(&section, 4);
	}
	~NativeMutex() {
		DeleteCriticalSection(&section);
	}
	_Acquires_lock_(this->section)
	void lock() noexcept {
		EnterCriticalSection(&section);
	}
	_Releases_lock_(this->section)
	void unlock() noexcept {
		LeaveCriticalSection(&section);
	}
	_Acquires_shared_lock_(this->section)
	void lock_shared() noexcept {
		EnterCriticalSection(&section);
	}
	_Releases_shared_lock_(this->section)
	void unlock_shared() noexcept {
		LeaveCriticalSection(&section);
	}
};
#endif

// std::lock_guard
template <class Mutex>
class LockGuard {
	Mutex &mutex;
public:
	explicit LockGuard(Mutex& m) noexcept : mutex{m} {
		mutex.lock();
	}
	~LockGuard() {
		mutex.unlock();
	}
	LockGuard(LockGuard const&) = delete;
	LockGuard(LockGuard const&&) = delete;
	LockGuard& operator=(LockGuard const&) = delete;
	LockGuard& operator=(LockGuard const&&) = delete;
};

// https://stackoverflow.com/questions/13206414/why-slim-reader-writer-exclusive-lock-outperformance-the-shared-one
#if 0
template <class Mutex>
class SharedLockGuard {
	Mutex &mutex;
public:
	explicit SharedLockGuard(Mutex& m) noexcept : mutex{m} {
		mutex.lock_shared();
	}
	~SharedLockGuard() {
		mutex.unlock_shared();
	}
	SharedLockGuard(SharedLockGuard const&) = delete;
	SharedLockGuard(SharedLockGuard const&&) = delete;
	SharedLockGuard& operator=(SharedLockGuard const&) = delete;
	SharedLockGuard& operator=(SharedLockGuard const&&) = delete;
};
#endif

}
