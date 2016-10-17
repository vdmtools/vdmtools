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

/* NOTE: This is an internal header file, included by other STL headers.
 *   You should not attempt to use it directly.
 */

#ifndef __SGI_STL_INTERNAL_ALLOC_H
#define __SGI_STL_INTERNAL_ALLOC_H


// This implements some standard node allocators.  These are
// NOT the same as the allocators in the C++ draft standard or in
// in the original STL.  They do not encapsulate different pointer
// types; indeed we assume that there is only one pointer type.
// The allocation primitives are intended to allocate individual objects,
// not larger arenas as with the original STL allocators.

#ifndef __ALLOC
#   define __ALLOC alloc
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef __RESTRICT
#  define __RESTRICT
#endif

# ifndef __SGI_STL_CONFIG_H
#  include <stl_config.h>
# endif

# if defined (__STL_DEBUG) && ! defined ( __SGI_STL_DEBUG_H )
#  include <stldebug.h>
# endif

#if !defined ( __STL_NO_EXCEPTIONS )
# if defined (__STL_BAD_ALLOC_DEFINED)
#  if defined (__STL_NEW_HEADER_NAMES) && defined (__STL_USE_NEW_STYLE_HEADERS)
#   include <new>
#  else
#   include <new.h>
#  endif
# else
    struct bad_alloc {};
# endif
#   define __THROW_BAD_ALLOC throw bad_alloc()
#elif !defined(__THROW_BAD_ALLOC)
#   include <stdio.h>
#   define __THROW_BAD_ALLOC fprintf(stderr,"out of memory"); exit(1)
#endif

# if defined ( __STL_USE_ABBREVS )
// ugliness is intentional - to reduce conflicts probability
#  define __malloc_alloc   M__A
#  define __alloc  D__A
# endif

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#endif

__STL_BEGIN_NAMESPACE

// Allocator adaptor to check size arguments for debugging.
// Reports errors using assert.  Checking can be disabled with
// NDEBUG, but it's far better to just use the underlying allocator
// instead when no checking is desired.
// There is some evidence that this can confuse Purify.
template <class Alloc>
class debug_alloc {
public:
    typedef Alloc allocator_type;
    typedef typename Alloc::value_type value_type;
private:
    struct alloc_header {
        size_t magic: 16;
        size_t type_size:16;
        __STL_UINT32_T size;
    }; // that is 8 bytes for sure
    // Sunpro CC has bug on enums, so extra_before/after set explicitly
    enum { pad=8, magic=0xdeba, deleted_magic = 0xdebd,
           shred_byte=
#  ifdef __STL_SHRED_BYTE
           __STL_SHRED_BYTE   
# else
           0xFF 
# endif  
           };
    static int extra_before() { return 16; }
    static int extra_after()  { return 8; }
                        // Size of space used to store size.  Note
                        // that this must be large enough to preserve
                        // alignment.
    static size_t extra_before_chunk() {
        return extra_before()/sizeof(value_type)+
            (size_t)(extra_before()%sizeof(value_type)>0);
    }
    static size_t extra_after_chunk() {
        return extra_before()/sizeof(value_type)+
            (size_t)(extra_after()%sizeof(value_type)>0);
    }
public:
    static void * allocate(size_t n);
    static void deallocate(void *p, size_t n);
    static void deallocate(void *p) { deallocate(p, sizeof(value_type)); }
    static void * allocate() { return allocate(sizeof(value_type));}

    static void * reallocate(void *p, size_t old_sz, size_t new_sz); 
};

// That is an adaptor for working with any alloc provided below
template<class T, class Alloc>
class simple_alloc {
    typedef Alloc alloc_type;
public:
    typedef typename Alloc::value_type alloc_value_type;
    typedef T value_type;
    static size_t chunk() { 
        return sizeof(T)/sizeof(alloc_value_type)+
            (size_t)(sizeof(T)%sizeof(alloc_value_type)>0);
    }
    static T* allocate(size_t n)
		{ return 0 == n ? 0 : (T*) alloc_type::allocate(n * chunk()); }
    static T* allocate(void)
		{ return (T*) alloc_type::allocate(chunk()); }
    static void deallocate(T * p, size_t n)
		{ if (0 != n) alloc_type::deallocate(/* (alloc_value_type*) */ p, n * chunk()); }
    static void deallocate(T * p)
		{ alloc_type::deallocate(/* (alloc_value_type*) */p, chunk()); }
};

// New-based allocator.  Typically slower than default alloc below.
// Typically thread-safe and more storage efficient.
template <int inst>
class __new_alloc {
public:
    // this one is needed for proper simple_alloc wrapping
    typedef char value_type;
    static void*  allocate(size_t n) { return 0 == n ? 0 : ::operator new(n
    #if ( defined(__IBMCPP__) && defined(__DEBUG_ALLOC__) ) // Added
     , __FILE__, __LINE__                                   // Added
    #endif                                                  // Added
    );}
    static void*  reallocate(void *p, size_t old_sz, size_t new_sz) {
        void* result = allocate(new_sz);
        size_t copy_sz = new_sz > old_sz? old_sz : new_sz;
        memcpy(result, p, copy_sz);
        deallocate(p, old_sz);
        return result;
    }
    static void deallocate(void* p) { ::operator delete(p
    #if ( defined(__IBMCPP__) && defined(__DEBUG_ALLOC__) ) // Added
     , __FILE__, __LINE__                                   // Added
    #endif                                                  // Added
    ); }
    static void deallocate(void* p, size_t) { ::operator delete(p
    #if ( defined(__IBMCPP__) && defined(__DEBUG_ALLOC__) ) // Added
     , __FILE__, __LINE__                                   // Added
    #endif                                                  // Added
    ); }
};

typedef __new_alloc<0> new_alloc;

// Malloc-based allocator.  Typically slower than default alloc below.
// Typically thread-safe and more storage efficient.

typedef void (* __oom_handler_type)();

template <int inst>
class __malloc_alloc {
private:
    static void *oom_malloc(size_t);
    static void *oom_realloc(void *, size_t);
    static __oom_handler_type oom_handler;

public:
    // this one is needed for proper simple_alloc wrapping
    typedef char value_type;

    static void * allocate(size_t n)
    {
        void *result = malloc(n);
        if (0 == result) result = oom_malloc(n);
        return result;
    }

    static void deallocate(void *p, size_t /* n */)
    {
        free((char*)p);
    }

    static void * reallocate(void *p, size_t /* old_sz */, size_t new_sz)
    {
        void * result = realloc((char*)p, new_sz);
        if (0 == result) result = oom_realloc(p, new_sz);
        return result;
    }

    static __oom_handler_type set_malloc_handler(__oom_handler_type f)
    {
        __oom_handler_type old = oom_handler;
        oom_handler = f;
        return(old);
    }
    
};

// malloc_alloc out-of-memory handling
# if ( __STL_STATIC_TEMPLATE_DATA > 0 )
template <int inst>
__oom_handler_type __malloc_alloc<inst>::oom_handler=(__oom_handler_type)0 ;
#  else
__DECLARE_INSTANCE(__oom_handler_type, __malloc_alloc<0>::oom_handler,0);
# endif /* ( __STL_STATIC_TEMPLATE_DATA > 0 ) */

template <class Alloc>
void * debug_alloc<Alloc>::allocate(size_t n) {
    size_t real_n = n + extra_before_chunk() + extra_after_chunk();
    alloc_header *result = (alloc_header *)allocator_type::allocate(real_n);
    memset((char*)result, shred_byte, real_n*sizeof(value_type));
    result->magic = magic;
    result->type_size = sizeof(value_type);
    result->size = n;
    return ((char*)result) + extra_before();
}

template <class Alloc>
void debug_alloc<Alloc>::deallocate(void *p, size_t n) {
    alloc_header * real_p = (alloc_header*)((char *)p - extra_before());
    // check integrity
    __stl_verbose_assert(real_p->magic != deleted_magic, __STL_MSG_DBA_DELETED_TWICE);
    __stl_verbose_assert(real_p->magic == magic, __STL_MSG_DBA_NEVER_ALLOCATED);
    __stl_verbose_assert(real_p->type_size == sizeof(value_type), 
                         __STL_MSG_DBA_TYPE_MISMATCH);
    __stl_verbose_assert(real_p->size == n, __STL_MSG_DBA_SIZE_MISMATCH);
    // check pads on both sides
    unsigned char* tmp;
    for (tmp= (unsigned char*)(real_p+1); tmp < (unsigned char*)p; tmp++)
        __stl_verbose_assert(*tmp==shred_byte, __STL_MSG_DBA_UNDERRUN);
    size_t real_n= n + extra_before_chunk() + extra_after_chunk();
    for (tmp= ((unsigned char*)p)+n*sizeof(value_type); 
         tmp < ((unsigned char*)real_p)+real_n ; tmp++)
        __stl_verbose_assert(*tmp==shred_byte, __STL_MSG_DBA_OVERRUN);
    // that may be unfortunate, just in case
    real_p->magic=deleted_magic;
    memset((char*)p, shred_byte, n*sizeof(value_type));
    allocator_type::deallocate(real_p, real_n);
}

template <class Alloc>
void * 
debug_alloc<Alloc>::reallocate(void *p, size_t old_sz, size_t new_sz) {
    alloc_header * real_p = (alloc_header*)((char *)p - extra_before());
    size_t extra = extra_before_chunk() + extra_after_chunk();
    __stl_verbose_assert(real_p->magic != deleted_magic, __STL_MSG_DBA_DELETED_TWICE);
    __stl_verbose_assert(real_p->magic == magic, __STL_MSG_DBA_NEVER_ALLOCATED);
    __stl_verbose_assert(real_p->type_size == sizeof(value_type), 
                         __STL_MSG_DBA_TYPE_MISMATCH);
    __stl_verbose_assert(real_p->size == old_sz, __STL_MSG_DBA_SIZE_MISMATCH);
    real_p = (alloc_header*)allocator_type::reallocate(real_p, old_sz + extra, 
                                                       new_sz + extra);
    real_p->size = new_sz;
    return ((char*)real_p) + extra_before();
}

template <int inst>
void * __malloc_alloc<inst>::oom_malloc(size_t n)
{
    __oom_handler_type my_malloc_handler;
    void *result;

    for (;;) {
	my_malloc_handler = oom_handler;
	if (0 == my_malloc_handler) { __THROW_BAD_ALLOC; }
	(*my_malloc_handler)();
	result = malloc(n);
	if (result) return(result);
    }
}

template <int inst>
void * __malloc_alloc<inst>::oom_realloc(void *p, size_t n)
{
    __oom_handler_type my_malloc_handler;
    void *result;

    for (;;) {
	my_malloc_handler = oom_handler;
	if (0 == my_malloc_handler) { __THROW_BAD_ALLOC; }
	(*my_malloc_handler)();
	result = realloc((char*)p, n);
	if (result) return(result);
    }
}

typedef __malloc_alloc<0> malloc_alloc;

# if defined ( __STL_USE_NEWALLOC )
#  if defined ( __STL_DEBUG_ALLOC )
    typedef debug_alloc<new_alloc> alloc;
#  else
    typedef new_alloc alloc;
#  endif /* __STL_DEBUG_ALLOC */
typedef new_alloc single_client_alloc;
typedef new_alloc multithreaded_alloc;
#  else /* __STL_USE_NEWALLOC */
#  ifdef __STL_USE_MALLOC
#   if defined ( __STL_DEBUG_ALLOC )
     typedef debug_alloc<malloc_alloc> alloc;
#   else
     typedef malloc_alloc alloc;
#   endif /* __STL_DEBUG_ALLOC */
typedef malloc_alloc single_client_alloc;
typedef malloc_alloc multithreaded_alloc;
#  endif /* __STL_USE_MALLOC */
#  endif /* __STL_USE_NEWALLOC */

__STL_END_NAMESPACE

# if ! defined (__STL_USE_MALLOC) && ! defined ( __STL_USE_NEWALLOC )
// global-level stuff

// fbp : put all this stuff here
# undef __VOLATILE
# ifdef _NOTHREADS
//  Thread-unsafe
#   define __NODE_ALLOCATOR_LOCK
#   define __NODE_ALLOCATOR_UNLOCK
#   define __NODE_ALLOCATOR_THREADS false
#   define __VOLATILE
# else
#  ifdef __STL_SOLARIS_THREADS
#   include <synch.h>
#   define __NODE_ALLOCATOR_LOCK \
        if (threads) mutex_lock(&__node_allocator_lock)
#   define __NODE_ALLOCATOR_UNLOCK \
        if (threads) mutex_unlock(&__node_allocator_lock)
#   define __NODE_ALLOCATOR_THREADS true
#   define __VOLATILE volatile  // May not be needed
#  endif
#  ifdef _PTHREADS
    // POSIX Threads
    // This is dubious, since this is likely to be a high contention
    // lock.  The Posix standard appears to require an implemention
    // that makes convoy effects likely.  Performance may not be
    // adequate.
#   include <pthread.h>
//    pthread_mutex_t __node_allocator_lock = PTHREAD_MUTEX_INITIALIZER;
#   define __NODE_ALLOCATOR_LOCK \
        if (threads) pthread_mutex_lock(&__node_allocator_lock)
#   define __NODE_ALLOCATOR_UNLOCK \
        if (threads) pthread_mutex_unlock(&__node_allocator_lock)
#   define __NODE_ALLOCATOR_THREADS true
#   define __VOLATILE volatile  // Needed at -O3 on SGI
#  endif
# ifdef __STL_WIN32THREADS
    // The lock needs to be initialized by constructing an allocator
    // objects of the right type.  We do that here explicitly for alloc.
#   define __NODE_ALLOCATOR_LOCK \
        EnterCriticalSection(&__node_allocator_lock)
#   define __NODE_ALLOCATOR_UNLOCK \
        LeaveCriticalSection(&__node_allocator_lock)
#   define __NODE_ALLOCATOR_THREADS true
#   define __VOLATILE volatile  // may not be needed
# endif /* WIN32THREADS */
#  ifdef __STL_SGI_THREADS
    // This should work without threads, with sproc threads, or with
    // pthreads.  It is suboptimal in all cases.
    // It is unlikely to even compile on nonSGI machines.

    extern int __us_rsthread_malloc;
	// The above is copied from malloc.h.  Including <malloc.h>
	// would be cleaner but fails with certain levels of standard
	// conformance.
#   define __NODE_ALLOCATOR_LOCK if (threads && __us_rsthread_malloc) \
                { __lock(&__node_allocator_lock); }
#   define __NODE_ALLOCATOR_UNLOCK if (threads && __us_rsthread_malloc) \
                { __unlock(&__node_allocator_lock); }
#   define __NODE_ALLOCATOR_THREADS true
#   define __VOLATILE volatile  // Needed at -O3 on SGI
#  endif
# endif

// Specialised debug form of malloc which does not provide "false"
// memory leaks when run with debug CRT libraries.
#if defined(_MSC_VER) && _MSC_VER>=1020 && defined(_DEBUG)
#  include <crtdbg.h>
#  define   __STL_CHUNK_MALLOC(s)         _malloc_dbg(s, _CRT_BLOCK, __FILE__, __LINE__)
#else	// !_DEBUG
#  define   __STL_CHUNK_MALLOC(s)         malloc(s)
#endif	// !_DEBUG

__STL_BEGIN_NAMESPACE


// Default node allocator.
// With a reasonable compiler, this should be roughly as fast as the
// original STL class-specific allocators, but with less fragmentation.
// Default_alloc_template parameters are experimental and MAY
// DISAPPEAR in the future.  Clients should just use alloc for now.
//
// Important implementation properties:
// 1. If the client request an object of size > __MAX_BYTES, the resulting
//    object will be obtained directly from malloc.
// 2. In all other cases, we allocate an object of size exactly
//    alloc_round_up(requested_size).  Thus the client has enough size
//    information that we can return the object to the proper free list
//    without permanently losing part of the object.
//

// The first template parameter specifies whether more than one thread
// may use this allocator.  It is safe to allocate an object from
// one instance of a default_alloc and deallocate it with another
// one.  This effectively transfers its ownership to the second one.
// This may have undesirable effects on reference locality.
// The second parameter is unreferenced and serves only to allow the
// creation of multiple default_alloc instances.
// Node that containers built on different allocator instances have
// different types, limiting the utility of this approach.
#if defined ( __STL_STATIC_ARRAY_BUG )
// breaks if we make these template class members:
  enum {__ALIGN = 8};
  enum {__MAX_BYTES = 128};
// SunPro CC 4.0.1 has bug on enums
//  enum {__NFREELISTS = __MAX_BYTES/__ALIGN};
#  define  __NFREELISTS 16
#endif 

union __node_alloc_obj;

union __node_alloc_obj {
        union __node_alloc_obj * free_list_link;
        char client_data[1];    /* The client sees this.        */
};

template <bool threads, int inst>
class __alloc {

__PRIVATE:
  // Really we should use static const int x = N
  // instead of enum { x = N }, but few compilers accept the former.
#if ! defined ( __STL_STATIC_ARRAY_BUG )
    enum {__ALIGN = 8};
    enum {__MAX_BYTES = 128};
    enum {__NFREELISTS = __MAX_BYTES/__ALIGN};
# endif


private:
  static size_t alloc_round_up(size_t bytes) {
	  return ((bytes + __ALIGN-1) & ~(__ALIGN - 1));
  }
__PRIVATE:
  typedef __node_alloc_obj obj;
private:
  static obj * __VOLATILE free_list[__NFREELISTS]; 
  static  size_t FREELIST_INDEX(size_t bytes) {
	return ((bytes + __ALIGN-1)/__ALIGN - 1);
  }
  // Returns an object of size n, and optionally adds to size n free list.
  static void *refill(size_t n);
  // Allocates a chunk for nobjs of size size.  nobjs may be reduced
  // if it is inconvenient to allocate the requested number.
  static char *chunk_alloc(size_t size, int &nobjs);

  // Chunk allocation state.
  static char *start_free;
  static char *end_free;
  static size_t heap_size;

# ifdef __STL_SGI_THREADS
    static volatile unsigned long __node_allocator_lock;
    static void __lock(volatile unsigned long *); 
    static inline void __unlock(volatile unsigned long *);
# endif

# ifdef __STL_SOLARIS_THREADS
    static mutex_t __node_allocator_lock;
# endif

# ifdef _PTHREADS
    static pthread_mutex_t __node_allocator_lock;
# endif

# ifdef __STL_WIN32THREADS
    static CRITICAL_SECTION __node_allocator_lock;
    static bool __node_allocator_lock_initialized;

  public:
    __alloc() {
	// This assumes the first constructor is called before threads
	// are started.
        if (!__node_allocator_lock_initialized) {
            InitializeCriticalSection(&__node_allocator_lock);
            __node_allocator_lock_initialized = true;
        }
    }
  private:
# endif

    class lock {
        public:
            lock() { __NODE_ALLOCATOR_LOCK; }
            ~lock() { __NODE_ALLOCATOR_UNLOCK; }
    };
    friend class lock;

public:
  // this one is needed for proper simple_alloc wrapping
  typedef char value_type;

  /* n must be > 0      */
  inline
  static void * allocate(size_t n);
  /* p may not be 0 */
  inline
  static void deallocate(void *p, size_t n);
  static void * reallocate(void *p, size_t old_sz, size_t new_sz);

} ;

typedef __alloc<__NODE_ALLOCATOR_THREADS, 0> node_alloc;
#   if defined ( __STL_DEBUG_ALLOC )
      typedef debug_alloc<node_alloc> alloc;
#   else
      typedef node_alloc alloc;
#   endif
typedef __alloc<false, 0> single_client_alloc;
typedef __alloc<true, 0>  multithreaded_alloc;

template <bool threads, int inst>
  /* n must be > 0      */

inline 
void * 
__alloc<threads,inst>::allocate(size_t n)
{
    obj * __VOLATILE * my_free_list;
    obj * __RESTRICT result;

    if (n > (size_t)__MAX_BYTES) {
        return(malloc_alloc::allocate(n));
    }
    my_free_list = free_list + FREELIST_INDEX(n);
    // Acquire the lock here with a constructor call.
    // This ensures that it is released in exit or during stack
    // unwinding.
        /*REFERENCED*/
#       ifndef _NOTHREADS
        lock lock_instance;
#       endif
    result = *my_free_list;
    if (result == 0) {
        void *r = refill(alloc_round_up(n));
        return r;
    }
    *my_free_list = result -> free_list_link;
    return (result);
}

template <bool threads, int inst>
inline 
void 
__alloc<threads, inst>::deallocate(void *p, size_t n)
{
    obj *q = (obj *)p;
    obj * __VOLATILE * my_free_list;

    if (n > (size_t)__MAX_BYTES) {
        malloc_alloc::deallocate(p, n);
        return;
    }
    my_free_list = free_list + FREELIST_INDEX(n);
    // acquire lock
#       ifndef _NOTHREADS
        /*REFERENCED*/
        lock lock_instance;
#       endif /* _NOTHREADS */
    q -> free_list_link = *my_free_list;
    *my_free_list = q;
    // lock is released here
}

/* We allocate memory in large chunks in order to avoid fragmenting     */
/* the malloc heap too much.                                            */
/* We assume that size is properly aligned.                             */
/* We hold the allocation lock.                                         */
template <bool threads, int inst>
char*
__alloc<threads, inst>::chunk_alloc(size_t size, int& nobjs)
{
    char * result;
    size_t total_bytes = size * nobjs;
    size_t bytes_left = end_free - start_free;

    if (bytes_left >= total_bytes) {
        result = start_free;
        start_free += total_bytes;
        return(result);
    } else if (bytes_left >= size) {
        nobjs = bytes_left/size;
        total_bytes = size * nobjs;
        result = start_free;
        start_free += total_bytes;
        return(result);
    } else {
        size_t bytes_to_get = 2 * total_bytes + alloc_round_up(heap_size >> 4);
        // Try to make use of the left-over piece.
        if (bytes_left > 0) {
            obj * __VOLATILE * my_free_list =
                        free_list + FREELIST_INDEX(bytes_left);

            ((obj *)start_free) -> free_list_link = *my_free_list;
            *my_free_list = (obj *)start_free;
        }
        start_free = (char *)__STL_CHUNK_MALLOC(bytes_to_get);
        if (0 == start_free) {
            int i;
            obj * __VOLATILE * my_free_list;
            obj *p;
            // Try to make do with what we have.  That can't
            // hurt.  We do not try smaller requests, since that tends
            // to result in disaster on multi-process machines.
            for (i = size; i <=(size_t)__MAX_BYTES; i += __ALIGN) {
                my_free_list = free_list + FREELIST_INDEX(i);
                p = *my_free_list;
                if (0 != p) {
                    *my_free_list = p -> free_list_link;
                    start_free = (char *)p;
                    end_free = start_free + i;
                    return(chunk_alloc(size, nobjs));
                    // Any leftover piece will eventually make it to the
                    // right free list.
                }
            }
	    end_free = 0;	// In case of exception.
            start_free = (char *)malloc_alloc::allocate(bytes_to_get);
            // This should either throw an
            // exception or remedy the situation.  Thus we assume it
            // succeeded.
        }
        heap_size += bytes_to_get;
        end_free = start_free + bytes_to_get;
        return(chunk_alloc(size, nobjs));
    }
}


/* Returns an object of size n, and optionally adds to size n free list.*/
/* We assume that n is properly aligned.                                */
/* We hold the allocation lock.                                         */
template <bool threads, int inst>
void* __alloc<threads, inst>::refill(size_t n)
{
    int nobjs = 20;
    char * chunk = chunk_alloc(n, nobjs);
    obj * __VOLATILE * my_free_list;
    obj * result;
    obj * current_obj, * next_obj;
    int i;

    if (1 == nobjs) return(chunk);
    my_free_list = free_list + FREELIST_INDEX(n);

    /* Build free list in chunk */
      result = (obj *)chunk;
      *my_free_list = next_obj = (obj *)(chunk + n);
      for (i = 1; ; i++) {
        current_obj = next_obj;
        next_obj = (obj *)((char *)next_obj + n);
        if (nobjs - 1 == i) {
            current_obj -> free_list_link = 0;
            break;
        } else {
            current_obj -> free_list_link = next_obj;
        }
      }
    return(result);
}

template <bool threads, int inst>
void*
__alloc<threads, inst>::reallocate(void *p,
                                                      size_t old_sz,
                                                      size_t new_sz)
{
    void * result;
    size_t copy_sz;

    if (old_sz > (size_t) __MAX_BYTES && new_sz > (size_t) __MAX_BYTES) {
        return(malloc_alloc::reallocate(p, old_sz, new_sz));
    }
    if (alloc_round_up(old_sz) == alloc_round_up(new_sz)) return(p);
    result = allocate(new_sz);
    copy_sz = new_sz > old_sz? old_sz : new_sz;
    memcpy(result, p, copy_sz);
    deallocate(p, old_sz);
    return(result);
}

#ifdef __STL_SGI_THREADS
__STL_END_NAMESPACE
#include <mutex.h>
#include <time.h>
__STL_BEGIN_NAMESPACE
// Somewhat generic lock implementations.  We need only test-and-set
// and some way to sleep.  These should work with both SGI pthreads
// and sproc threads.  They may be useful on other systems.
#if __mips < 3 || !(defined (_ABIN32) || defined(_ABI64)) || defined(__GNUC__)
#   define __test_and_set(l,v) test_and_set(l,v)
#endif

template <bool threads, int inst>
void 
__alloc<threads, inst>::__lock(volatile unsigned long *lock)
{
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
}

template <bool threads, int inst>
inline void
__alloc<threads, inst>::__unlock(volatile unsigned long *lock)
{
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
}
# endif /* SGITHREADS */

# if ( __STL_STATIC_TEMPLATE_DATA > 0 )

#ifdef __STL_SOLARIS_THREADS
template <bool threads, int inst>
mutex_t
__alloc<threads, inst>::__node_allocator_lock = DEFAULTMUTEX;
#endif

#ifdef _PTHREADS
template <bool threads, int inst>
pthread_mutex_t
__alloc<threads, inst>::__node_allocator_lock 
= PTHREAD_MUTEX_INITIALIZER;
#endif

# ifdef __STL_SGI_THREADS
template <bool threads, int inst>
volatile unsigned long 
__alloc<threads, inst>::__node_allocator_lock = 0;
# endif

template <bool threads, int inst>
char *__alloc<threads, inst>::start_free = 0;

template <bool threads, int inst>
char *__alloc<threads, inst>::end_free = 0;

template <bool threads, int inst>
size_t __alloc<threads, inst>::heap_size = 0;

template <bool threads, int inst>
__node_alloc_obj * __VOLATILE
__alloc<threads, inst>::free_list[
#if ! defined ( __STL_STATIC_ARRAY_BUG )
    __alloc<threads, inst>::__NFREELISTS]
#   else
__NFREELISTS]
#   endif
 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// The 16 zeros are necessary to make version 4.1 of the SunPro
// compiler happy.  Otherwise it appears to allocate too little
// space for the array.

#ifdef __STL_WIN32THREADS
    template <bool threads, int inst> CRITICAL_SECTION
    __alloc<threads, inst>::__node_allocator_lock;

    template <bool threads, int inst> bool
    __alloc<threads, inst>::__node_allocator_lock_initialized
	= false;
#endif

# else /* ( __STL_STATIC_TEMPLATE_DATA > 0 ) */
__DECLARE_INSTANCE(char *, single_client_alloc::start_free,0);
__DECLARE_INSTANCE(char *, single_client_alloc::end_free,0);
__DECLARE_INSTANCE(size_t, single_client_alloc::heap_size,0);
# if defined ( __STL_STATIC_ARRAY_BUG )
__DECLARE_INSTANCE(__node_alloc_obj * __VOLATILE,
                   single_client_alloc::free_list[__NFREELISTS],
                   {0});
#   else
__DECLARE_INSTANCE(__node_alloc_obj * __VOLATILE,
                   single_client_alloc::free_list[single_client_alloc::__NFREELISTS],
                   {0});
#   endif
__DECLARE_INSTANCE(char *, multithreaded_alloc::start_free,0);
__DECLARE_INSTANCE(char *, multithreaded_alloc::end_free,0);
__DECLARE_INSTANCE(size_t, multithreaded_alloc::heap_size,0);
#   if defined ( __STL_STATIC_ARRAY_BUG )
__DECLARE_INSTANCE(__node_alloc_obj * __VOLATILE,
                   multithreaded_alloc::free_list[__NFREELISTS],
                   {0});
#   else
__DECLARE_INSTANCE(__node_alloc_obj * __VOLATILE,
                   multithreaded_alloc::free_list[multithreaded_alloc::__NFREELISTS],
                   {0});
#   endif

#ifdef __STL_SOLARIS_THREADS
__DECLARE_INSTANCE(mutex_t,
                   single_client_alloc::__node_allocator_lock,
                   DEFAULTMUTEX);
__DECLARE_INSTANCE(mutex_t,
                   multithreaded_alloc::__node_allocator_lock,
                   DEFAULTMUTEX);
#endif
#   ifdef _PTHREADS
     __DECLARE_INSTANCE(pthread_mutex_t,
                        single_client_alloc::__node_allocator_lock,
                        PTHREAD_MUTEX_INITIALIZER);
     __DECLARE_INSTANCE(pthread_mutex_t,
                        multithreaded_alloc::__node_allocator_lock,
                        PTHREAD_MUTEX_INITIALIZER);
#   endif
#   ifdef __STL_SGI_THREADS
     __DECLARE_INSTANCE(volatile unsigned long,
                        single_client_alloc::__node_allocator_lock,
                        0);
     __DECLARE_INSTANCE(volatile unsigned long,
                        multithreaded_alloc::__node_allocator_lock,
                        0);
#   endif

#  endif /* __STL_STATIC_TEMPLATE_DATA */

# ifdef __STL_WIN32THREADS
  // Create one to get critical section initialized.
  // We do this onece per file, but only the first constructor
  // does anything.
  static alloc __node_allocator_dummy_instance;
# endif
// #  endif /* ! __USE_MALLOC */
__STL_END_NAMESPACE

# endif /* ! __USE_MALLOC && ! __USE_NEWALLOC */

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#endif

# if defined ( __STL_USE_DEFALLOC )
#  include <defalloc.h>
# endif

#endif /* __SGI_STL_INTERNAL_ALLOC_H */

// Local Variables:
// mode:C++
// End:
