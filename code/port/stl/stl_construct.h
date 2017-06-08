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
 *
 * Copyright (c) 1996,1997
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

#ifndef __SGI_STL_INTERNAL_CONSTRUCT_H
#define __SGI_STL_INTERNAL_CONSTRUCT_H

#include <new.h>

__STL_BEGIN_NAMESPACE

template <class T>
inline void destroy(T* pointer) {
# if _MSC_VER >= 1010
  pointer;
# endif	// _MSC_VER >= 1000
# if ( defined (__BORLANDC__) && ( __BORLANDC__ < 0x500 ) )
    pointer->T::~T();
# else
    pointer->~T();
# endif
# ifdef __STL_SHRED_BYTE
	fill_n((char*)pointer, sizeof(T), __STL_SHRED_BYTE);
# endif
}

template <class T1, class T2>
inline void construct(T1* p, const T2& value) {
# ifdef __STL_SHRED_BYTE
	fill_n((char*)p, sizeof(T1), __STL_SHRED_BYTE);
# endif
// MFC's DEBUG_NEW mechanism fails for placement new.
# if defined(_MFC_VER) && defined(DEBUG_NEW)
	#undef new
# endif // _MFC_VER && DEBUG_NEW

    new (p) T1(value);

# if defined(_MFC_VER) && defined(DEBUG_NEW)
	#define new DEBUG_NEW
# endif // _MFC_VER && DEBUG_NEW
}

#ifdef __STL_TRIVIAL_DESTRUCTOR_BUG       // JKR addition start
inline void destroy(char*) {}
inline void destroy(char**) {}
inline void destroy(unsigned char*) {}
inline void destroy(short*) {}
inline void destroy(unsigned short*) {}
inline void destroy(int*) {}
inline void destroy(unsigned int*) {}
inline void destroy(long*) {}
inline void destroy(unsigned long*) {}
inline void destroy(float*) {}
inline void destroy(double*) {}
inline void destroy(void**) {}
inline void destroy(const void**) {}

inline void destroy(const char*) {}
inline void destroy(const char**) {}
inline void destroy(const unsigned char*) {}
inline void destroy(const short*) {}
inline void destroy(const unsigned short*) {}
inline void destroy(const int*) {}
inline void destroy(const unsigned int*) {}
inline void destroy(const long*) {}
inline void destroy(const unsigned long*) {}
inline void destroy(const float*) {}
inline void destroy(const double*) {}
#endif // JKR addition end

template <class ForwardIterator>
INLINE_LOOP void
__destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) {
  for ( ; first < last; ++first)
    destroy(&*first);
}

template <class ForwardIterator> 
inline void __destroy_aux(ForwardIterator, ForwardIterator, __true_type) {}

template <class ForwardIterator, class T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*) {
  typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
  __destroy_aux(first, last, trivial_destructor());
}

template <class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
  __destroy(first, last, value_type(first));
}

// inline void destroy(char*, char*) {}
# ifdef __STL_WCHAR_T // dwa 8/15/97
inline void destroy(wchar_t*, wchar_t*) {}
inline void destroy(const wchar_t*, const wchar_t*) {}
# endif

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_CONSTRUCT_H */

// Local Variables:
// mode:C++
// End:
