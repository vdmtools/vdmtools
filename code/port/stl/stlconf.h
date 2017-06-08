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
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */

#ifndef __SGI_STL_STLCONF_H
# define __SGI_STL_STLCONF_H

# undef __AUTO_CONFIGURED

//==========================================================
// Getting proper values of autoconf flags
// if you ran 'configure', __AUTO_CONFIGURED is set to 1 and
// specific compiler fetures will be used.
// Otherwise, the <stlcomp.h> header will be included for per-version
// features recognition.
//==========================================================
# if defined ( BYPASS_AUTOCONF_SETTINGS ) || ! defined (__AUTO_CONFIGURED)
// per-version compiler features recognition
#  include <stlcomp.h>
# else
// auto-configured section

// define that to disable these features
# undef __STL_NO_EXCEPTIONS
# undef __STL_NO_NAMESPACES

// select allocation method you like
# undef __STL_USE_MALLOC
# undef __STL_USE_NEWALLOC

// this one is not mandatory, just enabled
# undef __STL_USE_DEFALLOC

// define NO_USING_STD if don't want using STL namespace by default
// new-style-headers define that to get proper behaviour
# undef  __STL_NO_USING_STD

// define __STL_USE_ABBREVS if your linker has trouble with long 
// external symbols
# undef __STL_USE_ABBREVS


// unsigned 32-bit integer type
#  define __STL_UINT32_T unsigned
#  undef __STL_BOOL_KEYWORD
#  undef __STL_RESERVED_BOOL_KEYWORD
#  undef __STL_YVALS_H
#  undef __STL_DEFAULT_TEMPLATE_PARAM
#  undef __STL_DEFAULT_TYPE_PARAM
#  undef __STL_STATIC_TEMPLATE_DATA
#  undef __STL_RAND48
#  undef __STL_LOOP_INLINE_PROBLEMS
#  undef __STL_NAMESPACES
#  undef __STL_TYPENAME
#  undef __STL_EXPLICIT
#  undef __STL_USE_EXCEPTIONS
#  undef __STL_EXCEPTION_SPEC
#  undef __STL_WEAK_ATTRIBUTE
#  undef __STL_BASE_MATCH_BUG
#  undef __STL_NONTEMPL_BASE_MATCH_BUG
#  undef __STL_NESTED_TYPE_PARAM_BUG
#  undef __STL_UNUSED_REQUIRED_BUG
#  undef __STL_UNINITIALIZABLE_PRIVATE
#  undef __STL_BASE_TYPEDEF_BUG
#  undef __STL_BASE_TYPEDEF_OUTSIDE_BUG
#  undef __STL_CONST_CONSTRUCTOR_BUG

#  undef __STL_NEW_STYLE_CASTS
#  undef __STL_WCHAR_T
#  undef __STL_LONG_LONG
#  undef __STL_LONG_DOUBLE
#  undef __STL_MUTABLE
#  undef __STL_FULL_SPEC_SYNTAX
#  undef __STL_BAD_ALLOC_DEFINED
#  undef __STL_DEBUG_ALLOC
#  undef __STL_MEMBER_TEMPLATES
#  undef __STL_MEMBER_CLASS_TEMPLATES
#  undef __STL_FRIEND_TEMPLATES
#  undef __STL_CLASS_PARTIAL_SPECIALIZATION
#  undef __STL_FUNC_PARTIAL_ORDERING
#  undef __STL_AUTOMATIC_TYPE_TRAITS
#  undef __STL_MEMBER_POINTER_PARAM_BUG
#  undef __STL_NON_TYPE_TMPL_PARAM_BUG
#  undef __STL_NO_DEFAULT_NON_TYPE_PARAM
#  undef __STL_METHOD_SPECIALIZATION
#  undef __STL_STATIC_ARRAY_BUG
#  undef __STL_TRIVIAL_CONSTRUCTOR_BUG
#  undef __STL_TRIVIAL_DESTRUCTOR_BUG
#  undef __STL_BROKEN_USING_DIRECTIVE
# endif /* AUTO_CONFIGURED */

//==========================================================


#endif /* __STLCONF_H */
