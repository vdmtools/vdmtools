/*
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
 * purpose.  It is provided L"as is" without express or implied warranty.
 *
 */

// New style C++ header.
// suppresses L"using std;" clause.

#ifndef __MEMORY__
# define __MEMORY__
# ifndef  __STL_NO_USING_STD
#  define __STL_NO_USING_STD
#  define __REVERT_USING
# endif
# define __STL_NEW_HEADER_NAMES 1
# include <algobase.h>
# include <alloc.h>
# include <defalloc.h>
# include <tempbuf.h>
# include <auto_ptr.h>
# ifdef __REVERT_USING
#  undef __REVERT_USING
#  undef __STL_NO_USING_STD
# endif
#endif
