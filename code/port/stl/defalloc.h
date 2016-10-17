/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */
//
//  Inclusion of this file is DEPRECATED.
//  This is the original HP default allocator.
//  DO NOT USE THIS FILE unless you have an old container implementation
//  that requires an allocator with the HP-style interface.
//  SGI STL uses a different allocator interface.
//  SGI-style allocators are not parametrized with respect to
//  the object type; they traffic in void * pointers.
//  This file is not included by any other SGI STL header.
//

// Adaptation note: THIS version of allocator<T> is fully compatible with
// SGI containers and works OK standalone. It is also as close to CD2 version
// as possible w/o member templates.
// However, explicit use of allocator<T>  is not recommended 
// unless you have to do so ( for example, compiling third-party code).

#ifndef __SGI_STL_DEFALLOC_H
#define __SGI_STL_DEFALLOC_H

# if !defined (__SGI_STL_ALLOC_H)
#  include <alloc.h>
# endif
# if !defined (__SGI_STL_ALGOBASE_H)
#  include <algobase.h>
# endif

__STL_BEGIN_NAMESPACE

template <class T>
inline T* allocate(size_t size, T*) {
    return ::operator new(size*sizeof(T));
}

template <class T>
inline void deallocate(T* buffer) {
    ::operator delete(buffer);
}

template <class T>
inline void deallocate(T* buffer, size_t) {
    ::operator delete(buffer);
}

template <class T>
class allocator {
    typedef alloc super;
    typedef allocator<T> self;
public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    static T* allocate(size_t n=1) { return (T*)super::allocate(n * sizeof(T));}
    static void deallocate(T *p, size_t n=1) { if (p !=0) super::deallocate(p, n * sizeof(T)); }
    static pointer address(reference x) { return (pointer)&x; }
    static const_pointer address(const_reference x) { 
	return (const_pointer)&x; 
    }
    static size_type max_size() { 
        size_type sz((size_t)(-1)/sizeof(T));
        size_type msz(1);
	return max(msz, sz);
    }
    // CD2 requires that
    static T* allocate(size_t n, const void* ) { return (T*)super::allocate(n * sizeof(T));}
    void construct(pointer p, const value_type& val) { __STD::construct(p, val); }
    void destroy(pointer p) { __STD::destroy(p); }
# if defined ( __STL_MEMBER_TEMPLATES )
    allocator() __STL_THROWS(()) {}
    template<class U>
    allocator(const allocator<U>&) __STL_THROWS(()) {}
    template<class U>
    self& operator=(const allocator<U>&) __STL_THROWS(()) { return *this; }
#endif

# if defined ( __STL_MEMBER_CLASS_TEMPLATES )
    template <class U>
    struct rebind {
        typedef allocator<U>  other; 
    };
# endif

};

template<class T> inline
bool operator==(const allocator<T>&, const allocator<T>&) { return true; }

//template<class T> inline
//bool operator!=(const allocator<T>&, const allocator<T>&) { return false; }

__STL_FULL_SPECIALIZATION
class __STLIMP allocator<void> {
public:
    typedef void* pointer;
    typedef const void* const_pointer;
};

__STL_END_NAMESPACE

// fbp: just for backwards compatibility,
// hope this doesn't break anything.
#ifdef __STL_USE_NAMESPACES

# ifdef __STL_BROKEN_USING_DIRECTIVE
using namespace __STD;
# else
using __STD::allocator;
# endif
#endif

#endif
