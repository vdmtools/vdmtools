/*
 * Copyright (c) 1996-1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * Copyright (c) 1997
 * Moscow Center for SPARC Technology
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Moscow Center for SPARC Technology makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */

#ifndef __SGI_STL_LOCK_H
#define __SGI_STL_LOCK_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifndef __RESTRICT
#  define __RESTRICT
#endif

# ifndef __SGI_STL_CONFIG_H
#  include <stl_config.h>
# endif

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#endif

#  undef  __VOLATILE
// fbp : put all this stuff here
# ifdef _NOTHREADS
//  Thread-unsafe
#   define __VOLATILE
__STL_BEGIN_NAMESPACE
struct __void_lock { } ;
typedef __void_lock  __stl_mutex_type;
__STL_END_NAMESPACE
# else
#  define __VOLATILE volatile  // Needed at -O3 on SGI
#  ifdef _PTHREADS
#   include <pthread.h>
__STL_BEGIN_NAMESPACE
    typedef  pthread_mutex_t __stl_mutex_type;
__STL_END_NAMESPACE
#  endif
#  ifdef __STL_SGI_THREADS
#   include <mutex.h>
#   include <time.h>
__STL_BEGIN_NAMESPACE
    typedef volatile unsigned long __stl_mutex_type;
__STL_END_NAMESPACE
#  endif
#  ifdef __STL_WIN32THREADS
__STL_BEGIN_NAMESPACE
    typedef CRITICAL_SECTION __stl_mutex_type;
__STL_END_NAMESPACE
#  endif
#  ifdef __STL_SOLARIS_THREADS
#   include <synch.h>
__STL_BEGIN_NAMESPACE
    typedef mutex_t __stl_mutex_type;
__STL_END_NAMESPACE
#  endif
# endif


__STL_BEGIN_NAMESPACE

#ifdef __STL_SGI_THREADS
// Somewhat generic lock implementations.  We need only test-and-set
// and some way to sleep.  These should work with both SGI pthreads
// and sproc threads.  They may be useful on other systems.
#if __mips < 3 || !(defined (_ABIN32) || defined(_ABI64)) || defined(__GNUC__)
#   undef __test_and_set
#   define __test_and_set(l,v) test_and_set(l,v)
#endif
#endif /* SGI */

inline void 
__stl_do_lock(__stl_mutex_type *
# ifndef _NOTHREADS
lock
# endif
)
{
#  ifdef __STL_SOLARIS_THREADS
    mutex_lock(lock);
#  endif
# ifdef __STL_WIN32THREADS
    EnterCriticalSection(lock);
# endif
# ifdef _PTHREADS
    pthread_mutex_lock(lock);
# endif
# ifdef __STL_SGI_THREADS
    const unsigned low_spin_max = 30;  // spin cycles if we suspect uniprocessor
    const unsigned high_spin_max = 1000; // spin cycles for multiprocessor
    static unsigned spin_max = low_spin_max;
    unsigned my_spin_max;
    static unsigned last_spins = 0;
    unsigned my_last_spins;
    static struct timespec ts = {0, 1000};
    unsigned junk;
#   define __ALLOC_PAUSE junk *= junk; junk *= junk; junk *= junk; junk *= junk
    int i;

    if (!__test_and_set((unsigned long *)lock, 1)) {
        return;
    }
    my_spin_max = spin_max;
    my_last_spins = last_spins;
    for (i = 0; i < my_spin_max; i++) {
        if (i < my_last_spins/2 || *lock) {
            __ALLOC_PAUSE;
            continue;
        }
        if (!__test_and_set((unsigned long *)lock, 1)) {
            // got it!
            // Spinning worked.  Thus we're probably not being scheduled
            // against the other process with which we were contending.
            // Thus it makes sense to spin longer the next time.
            last_spins = i;
            spin_max = high_spin_max;
            return;
        }
    }
    // We are probably being scheduled against the other process.  Sleep.
    spin_max = low_spin_max;
    for (;;) {
        if (!__test_and_set((unsigned long *)lock, 1)) {
            return;
        }
        nanosleep(&ts, 0);
    }
# endif /* SGI */
}

inline void
__stl_do_unlock(__stl_mutex_type *
# ifndef _NOTHREADS
lock
# endif
)
{
#  ifdef __STL_SOLARIS_THREADS
    mutex_unlock(lock);
#  endif
# ifdef __STL_WIN32THREADS
    LeaveCriticalSection(lock);
# endif        
# ifdef _PTHREADS
    pthread_mutex_unlock(lock);
# endif
# ifdef __STL_SGI_THREADS
#   if defined(__GNUC__) && __mips >= 3
        asm("sync");
        *lock = 0;
#   elif __mips >= 3 && (defined (_ABIN32) || defined(_ABI64))
        __lock_release(lock);
#   else 
        *lock = 0;
        // This is not sufficient on many multiprocessors, since
        // writes to protected variables and the lock may be reordered.
#   endif
# endif /* SGITHREADS */
}

class __stl_guard;

# ifdef __STL_SOLARIS_THREADS
const mutex_t null_mutex=DEFAULTMUTEX;
# endif
# ifdef _PTHREADS
const __stl_mutex_type null_mutex= PTHREAD_MUTEX_INITIALIZER;
# endif

class __stl_lock {
public:
# ifdef __STL_SOLARIS_THREADS
    // apparently, no need to destroy mutex explicitly
    __stl_lock()  : lock_(null_mutex) {
    /* mutex_init(&lock_,USYNC_THREAD, NULL);*/
    }
# endif 
# ifdef __STL_SGI_THREADS
    __stl_lock() : lock_(0) {}
# endif
# ifdef _PTHREADS
    __stl_lock() : lock_(null_mutex) {}
# endif
# ifdef __STL_WIN32THREADS
__stl_lock() {
            InitializeCriticalSection(&lock_);
        }
    ~__stl_lock() {
            DeleteCriticalSection(&lock_);
        }
# endif

    void lock() {
        __stl_do_lock(&lock_);
    }

    void unlock() {
        __stl_do_unlock(&lock_);
    }
private:
    __stl_mutex_type lock_;
    friend class __stl_guard;
} ;

class __stl_guard {
public:
    __stl_guard(__stl_lock& l) : lock(l) { 
        l.lock();
    }
    ~__stl_guard() {
        lock.unlock();
    }
private:
    __stl_lock& lock;
};

__STL_END_NAMESPACE

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#endif

#endif /* __STL_LOCK_H */
