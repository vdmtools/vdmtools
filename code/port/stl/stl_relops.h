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
 * Copyright (c) 1996,1997
 * Silicon Graphics
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */

/* NOTE: This is an internal header file, included by other STL headers.
 *   You should not attempt to use it directly.
 */

#ifndef __SGI_STL_INTERNAL_RELOPS
#define __SGI_STL_INTERNAL_RELOPS

//
//# if defined ( __STL_NAMESPACES ) && defined (__STL_NEW_HEADER_NAMES)
//namespace rel_ops {
//# endif

__STL_BEGIN_NAMESPACE
__STL_BEGIN_RELOPS_NAMESPACE

#if ! defined(_MSC_VER)
template <class T>
inline bool operator!=(const T& x, const T& y) {
  return !(x == y);
}
#endif

template <class T>
inline bool operator>(const T& x, const T& y) {
  return y < x;
}

template <class T>
inline bool operator<=(const T& x, const T& y) {
  return !(y < x);
}

template <class T>
inline bool operator>=(const T& x, const T& y) {
  return !(x < y);
}

__STL_END_RELOPS_NAMESPACE

#ifdef __STL_USE_NAMESPACE_FOR_RELOPS

using namespace __STD_RELOPS;
// Names from stl_relops.h
/*
using __STD_RELOPS::operator!=;
using __STD_RELOPS::operator>;
using __STD_RELOPS::operator<=;
using __STD_RELOPS::operator>=;
*/
#endif /* __STL_USE_NAMESPACE_FOR_RELOPS */

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_RELOPS */

// Local Variables:
// mode:C++
// End:
