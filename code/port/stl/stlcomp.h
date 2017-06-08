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

#ifndef __STLCOMP_H
# define __STLCOMP_H

//==========================================================
// Features selection

// Uncomment that to disable exception handling
// #  define __STL_NO_EXCEPTIONS 1

// Uncomment that to disable std namespace usage
// #  define __STL_NO_NAMESPACES 1

// Comment that to not include defalloc.h ( no defaults changed )
#  define    __STL_USE_DEFALLOC   1

// Uncomment that to to use new-based allocator as default
// #  define    __STL_USE_NEWALLOC   1

// Uncomment this to use malloc-based allocator as default
// #  define __STL_USE_MALLOC 1

// Uncomment this to disable using std by default
// #  define __STL_NO_USING_STD 1

// define __STL_USE_ABBREVS if your linker has trouble with long 
// external symbols
// # define  __STL_USE_ABBREVS 1

// set this to force checked allocators
// #  define __STL_DEBUG_ALLOC 1

// Mostly correct guess, change it for Alpha (and other environments
// that has 64-bit "long")
#  define __STL_UINT32_T unsigned long
  
//==========================================================

//==========================================================
// default values of autoconf  flags
//==========================================================

// the values choosen here as defaults try to give
// maximum functionality on the most conservative settings

// Uncomment this if your compiler supports "bool"
// #  define  __STL_BOOL_KEYWORD 1

// Uncomment this if your compiler has "bool" keyword reserved
// #  define  __STL_RESERVED_BOOL_KEYWORD 1

// Uncomment this if your compiler doesn't support that
// #  define __STL_DEFAULT_TEMPLATE_PARAM 1
// Uncomment this if your compiler support only complete types as
// default parameters
// #  define __STL_DEFAULT_TYPE_PARAM 1

// Comment this if your compiler lacks static data 
// members template declarations 
// Uncomment next line if your compiler supports __attribute__((weak))
#  define __STL_STATIC_TEMPLATE_DATA 1

// #  define __STL_WEAK_ATTRIBUTE 1

// Uncomment this if your C library has lrand48() function
// #  define __STL_RAND48 1
// Uncomment this if your compiler can't inline while(), for()
// #  define __STL_LOOP_INLINE_PROBLEMS 1

// Uncomment this if your compiler supports namespaces 
// #  define __STL_NAMESPACES 1

// Uncomment this if your compiler supports typename
// #  define __STL_TYPENAME 1

// Uncomment this if your compiler supports mutable
// #  define __STL_MUTABLE 1

// Uncomment if const_cast<> is available
// #  define __STL_NEW_STYLE_CASTS 1

// Uncomment this if your compiler supports explicit constructors
// #  define __STL_EXPLICIT 1

// Uncomment this if your compiler supports exceptions
// #  define __STL_EXCEPTIONS 1

// Uncomment this if your compiler supports exception specifications
// with reduced overhead ( e.g. inlines them, not vice versa)
// #  define __STL_EXCEPTION_SPEC

// Uncomment if long long is available
// #  define __STL_LONG_LONG 1

// Uncomment this for wchar_t functinality
// #  define __STL_WCHAR_T  1

// Uncomment if needed full  specialization syntax : template <> struct ....
// #  define __STL_FULL_SPEC_SYNTAX  1

// Uncomment if bad_alloc defined in <new>
// #  define __STL_BAD_ALLOC_DEFINED  1

// Uncomment if member templates available
// #  define __STL_MEMBER_TEMPLATES   1

// Uncomment if member templates available
// #  define __STL_FRIEND_TEMPLATES   1

// Uncomment if available
// #  define __STL_CLASS_PARTIAL_SPECIALIZATION 1

// Uncomment if available
// #  define __STL_FUNC_PARTIAL_ORDERING 1

// Uncomment if available
// #  define __STL_AUTOMATIC_TYPE_TRAITS 1

// Uncomment if getting errors compiling mem_fun* adaptors
// #  define __STL_MEMBER_POINTER_PARAM_BUG 1

// Uncomment the following line if your compiler smart about
// "smart pointer" operator
// #   define __STL_ARROW_OPERATOR          1

// All these settings don't affect performance/functionality
// Uncomment them if your compiler has problems.
// #  define __STL_BASE_MATCH_BUG          1
//#  define  __STL_NONTEMPL_BASE_MATCH_BUG 1

// #  define __STL_NESTED_TYPE_PARAM_BUG   1
// #  define __STL_UNINITIALIZABLE_PRIVATE  1
// #  define __STL_BASE_TYPEDEF_OUTSIDE_BUG 1
// if your compiler have serious problems with typedefs, try this one
// #  define __STL_BASE_TYPEDEF_BUG          1


#  define __STL_CONST_CONSTRUCTOR_BUG    1
// #  define __STL_NON_TYPE_TMPL_PARAM_BUG 1
#  define __STL_UNUSED_REQUIRED_BUG     1

//==========================================================

//==========================================================
// per-version compiler features recognition
//==========================================================

// reporting of incompatibility
#  define __GIVE_UP_WITH_STL(message) void give_up() \
   { upgrade_the_compiler_to_use_STL;}


# if defined(__sgi) && !defined(__GNUC__)
#   if defined(_BOOL) || ! ((_MIPS_ISA < 2) || defined (_ABIO32))
#     define __STL_BOOL_KEYWORD
#   endif
#   define __EDG_SWITCHES
#   if !defined(_NOTHREADS) && !defined(_PTHREADS)
#     define __STL_SGI_THREADS
#   endif
// #  undef  __STL_NON_TYPE_TMPL_PARAM_BUG
#  define __STL_ARROW_OPERATOR
#  define __STL_METHOD_SPECIALIZATION
#  define __STL_DEFAULT_TEMPLATE_PARAM 1
# endif


// AIX xlC 3.1 , 3.0.1 ==0x301
// Visual Age C++ 3.x
// OS-390 C++
#if ( defined (__xlC__) && __xlC__ < 0x400 ) || \
    (defined(__MVS__) && defined ( __IBMCPP__ ) && (__IBMCPP__ < 23000 )) || \
    ( defined (  __IBMCPP__ ) && (  __IBMCPP__ < 400 ) )
#  define __STL_RESERVED_BOOL_KEYWORD 1
#  define __STL_UNINITIALIZABLE_PRIVATE 1
#  define __STL_BASE_TYPEDEF_OUTSIDE_BUG 1
#  define __STL_STATIC_ARRAY_BUG 1
#  define __STL_TRIVIAL_DESTRUCTOR_BUG  1  // AIX xlC, Visual Age 3.0 for OS/2 and MS 
#  if ( defined (__MULTI__) && defined (__WINDOWS__))
#   define  __STL_WIN32THREADS 1          // Only Visual Age 3.5 for Windows
#  endif
#  define __STL_UNUSED_REQUIRED_BUG 1
#  define __STL_NON_TYPE_TMPL_PARAM_BUG 1
#endif

// Microsoft Visual C++ 4.0, 4.1, 4.2, 5.0
# if defined(_MSC_VER)
#  if ( _MSC_VER>=1000 )
// VC++ 4.0 and higher
#   define __STL_NAMESPACES             1
// Actually, in 5.0, "using" nearly works, but in some cases fails:
#   define __STL_BROKEN_USING_DIRECTIVE 1
#   define __STL_NEW_STYLE_CASTS        1
#   define __STL_LONG_DOUBLE            1
#   define __STL_WCHAR_T                1
#   define __STL_METHOD_SPECIALIZATION  1
// This definition makes SGI reverse_iterator to be compatible with
// other parts of MSVC library. Define it for all VC++ versions
#   define __STL_MSVC50_COMPATIBILITY    1
#   define __STL_NON_TYPE_TMPL_PARAM_BUG 1

// <NBulteau@jouve.fr> : suppressed "truncated debug info" warning
#   pragma warning(disable:4786)

#   if ( _MSC_VER<=1010 )
// "bool" is reserved in MSVC 4.1 while <yvals.h> absent, so :
#    define __STL_RESERVED_BOOL_KEYWORD 1
#    define __STL_USE_ABBREVS           1
#   else
// VC++ 4.2 and higher
#    define __STL_YVALS_H 1
#    define __STL_USE_NEW_STYLE_HEADERS 1
#    if defined  (__STL_NEW_HEADER_NAMES) || (_MSC_VER < 1100)
#     define __STL_BAD_ALLOC_DEFINED 1
#    endif
#   endif /* 1010 */

#   if (_MSC_VER >= 1100)  // MSVC 5.0
#    define __STL_DEFAULT_TEMPLATE_PARAM 1
#    define __STL_TYPENAME      1
#    define __STL_EXPLICIT      1
#    define __STL_MUTABLE       1
#    define __STL_FULL_SPEC_SYNTAX 1
//#    undef __STL_CONST_CONSTRUCTOR_BUG
#   else
#    define __STL_NONTEMPL_BASE_MATCH_BUG 1
#   endif /* 1100 */

// these switches depend on compiler flags
#   ifdef _CPPUNWIND
#     define __STL_USE_EXCEPTIONS
#   endif
#   if defined ( _MT ) 
#     define __STL_WIN32THREADS
#   endif
#  endif /* 1000 */
# endif /* _MSC_VER */

// Borland C++ ( 5.x )
# if defined ( __BORLANDC__ )
#  define __STL_NEW_STYLE_CASTS 1
#  define __STL_NON_TYPE_TMPL_PARAM_BUG 1
// #   define __STL_ARROW_OPERATOR 1
#  if ( __BORLANDC__ < 0x501 )
#   define  __STL_NONTEMPL_BASE_MATCH_BUG 1
#  endif
#  if ( __BORLANDC__ < 0x500 )
#   define __STL_NESTED_TYPE_PARAM_BUG 1
#   define __STL_STATIC_ARRAY_BUG 1
#  else
#   define __STL_BOOL_KEYWORD 1
#   define __STL_DEFAULT_TYPE_PARAM 1
#   define __STL_NAMESPACES 1
#   define __STL_BROKEN_USING_DIRECTIVE 1
#   define __STL_EXPLICIT   1
#   define __STL_TYPENAME   1
#   define __STL_LONG_DOUBLE 1
#   define __STL_MUTABLE 1
#   define __STL_WCHAR_T 1
#  endif
#  define __STL_LOOP_INLINE_PROBLEMS 1
// empty exception spec make things worse in BC, so:
#  undef __STL_EXCEPTION_SPEC
//auto enable thread safety and debugging features:
#   ifdef _CPPUNWIND
#     define __STL_USE_EXCEPTIONS
#   endif
#  if defined(__MT__)
#   define __STL_WIN32THREADS
#  endif
#  if defined ( __DEBUG ) &&( __DEBUG > 0 )
#   define __STL_DEBUG
#  endif
// Stop complaints about functions not inlined
#  pragma option -w-inl
// Stop complaints about significant digits
#  pragma option -w-sig
// Stop complaint about constant being long
#  pragma option -w-cln
#  define __STL_METHOD_SPECIALIZATION
# endif

# if defined(__SUNPRO_CC)
// common SunPro features
#  define __STL_USE_EXCEPTIONS     1
#  define __STL_EXCEPTION_SPEC 1
#  define __STL_LONG_LONG  1
#  define __STL_WCHAR_T  1
#  define __STL_RAND48 1
#  define __STL_NON_TYPE_TMPL_PARAM_BUG 1
#  define __STL_UNINITIALIZABLE_PRIVATE 1
#  define __STL_STATIC_ARRAY_BUG 1

#  if ( __SUNPRO_CC > 0x401 )
#   if (__SUNPRO_CC==0x410)
#    define __STL_BASE_TYPEDEF_OUTSIDE_BUG  1
#   endif
#   if ( __SUNPRO_CC >= 0x420 )
#    define __STL_FULL_SPEC_SYNTAX 1
// #    define __STL_SEPARATE_TEMPLATE_BODY 1
#   endif
// # define __STL_NONTEMPL_BASE_MATCH_BUG 1
#  else
   // SUNPro C++ 4.0.1
#   define __STL_BASE_MATCH_BUG          1
#   define __STL_NONTEMPL_BASE_MATCH_BUG 1
#   define __STL_BASE_TYPEDEF_BUG        1
#     if ( __SUNPRO_CC < 0x401 )
        __GIVE_UP_WITH_STL(SUNPRO_401)
#     endif
#  endif /* 4.0.1 */
# endif /* __SUNPRO_CC */


// g++ 2.7.x and above 
# if defined (__GNUC__ )
#   define __STL_BOOL_KEYWORD 1
#   define __STL_EXPLICIT 1
#   define __STL_LONG_LONG  1
#   define __STL_WCHAR_T  1
#   define __STL_MUTABLE  1
#   define __STL_NEW_STYLE_CASTS 1

// gcc 2.7.2 settings
#  if ! ( __GNUC__ > 2 || __GNUC_MINOR__ > 7 || defined (__CYGWIN32__))
// Will it work with 2.6 ? I doubt it.
#   if ( __GNUC_MINOR__ < 6 )
    __GIVE_UP_WITH_STL(GCC_272);
#   endif
#   define __STL_DEFAULT_TYPE_PARAM 1

#   undef  __STL_STATIC_TEMPLATE_DATA
#  if !defined (__CYGWIN32__) 
#   define __STL_NESTED_TYPE_PARAM_BUG   1
#   define __STL_BASE_MATCH_BUG       1
//  unused operators are required (forward)
#   undef  __STL_CONST_CONSTRUCTOR_BUG 
#   define __STL_NO_DEFAULT_NON_TYPE_PARAM
#   undef __STL_METHOD_SPECIALIZATION
#  endif
// static template data members workaround strategy for gcc tries
// to use weak symbols.
// if you don't want to use that, #define __STL_WEAK_ATTRIBUTE=0 ( you'll
// have to put "#define __PUT_STATIC_DATA_MEMBERS_HERE" line in one of your
// compilation unit ( or CFLAGS for it ) _before_ including any STL header ).
#   if !(defined (__STL_STATIC_TEMPLATE_DATA) || defined (__STL_WEAK_ATTRIBUTE ))
// systems using GNU ld or format that supports weak symbols
// may use "weak" attribute
// Linux & Solaris ( x86 & SPARC ) are being auto-recognized here
#    if defined(__STL_GNU_LD) || defined(__ELF__) || \
     (( defined (__SVR4) || defined ( __svr4__ )) && \
      ( defined (sun) || defined ( __sun__ )))
#     define __STL_WEAK_ATTRIBUTE 1
#    endif
#   endif /* __STL_WEAK_ATTRIBUTE */
#  endif /* __GNUC__ > 2 */

// egcs, FSF snapshots, cygwin32
#  if defined ( __CYGWIN32__ ) || (__GNUC_MINOR__ > 7 )
#   define __STL_DEFAULT_TEMPLATE_PARAM 1
#   define __STL_CLASS_PARTIAL_SPECIALIZATION 1
#   define __STL_FUNC_PARTIAL_ORDERING 1
#   define __STL_FULL_SPEC_SYNTAX 1
#   define __STL_TYPENAME 1
#   ifdef __EXCEPTIONS
#     undef  __STL_USE_EXCEPTIONS
#     define __STL_USE_EXCEPTIONS  1
#   endif
#   define  __STL_BAD_ALLOC_DEFINED
#   define  __STL_METHOD_SPECIALIZATION
// #  undef   __STL_NON_TYPE_TMPL_PARAM_BUG
#   define  __STL_ARROW_OPERATOR
#   undef   __STL_UNUSED_REQUIRED_BUG
#   if (__GNUC_MINOR__ >=90)
// egcs - define it if not pushing them hard
#    define __STL_MEMBER_TEMPLATES
#   else
// if using older FSF snapshots, you'll probably have to define it
#    define __STL_PARTIAL_SPECIALIZATION_BUG
#   endif 
#  endif /* > 2.7 */

# endif /* __GNUC__ */


# if defined (__WATCOM_CPLUSPLUS__)
#   define __STL_BASE_TYPEDEF_OUTSIDE_BUG 1
// eyal.ben-david@aks.com : disabled
// #   define __STL_USE_NEW_STYLE_HEADERS 1
#   define __STL_LONG_DOUBLE 1
#   define __STL_NO_DEFAULT_NON_TYPE_PARAM 1
#   define __STL_METHOD_SPECIALIZATION 1
#   define __STL_NON_TYPE_TMPL_PARAM_BUG 1
#   define __STL_NONTEMPL_BASE_MATCH_BUG

// Get rid of Watcom's min and max macros 
#undef min 
#undef max

// for switches (-xs,  -xss,  -xst)
//
#if defined (__SW_XS) || defined (__SW_XSS) || defined(__SW_XST)
#    define __STL_EXCEPTIONS 1
#    define __STL_EXCEPTION_SPEC 1
# endif

#  define __STL_NESTED_TYPE_PARAM_BUG 1
//#  define __STL_CONST_CONSTRUCTOR_BUG 1

#  if (__WATCOM_CPLUSPLUS__ >= 1100 )
#   define __STL_BOOL_KEYWORD 1
#   define __STL_EXPLICIT 1
#   define __STL_MUTABLE 1
#   define __STL_NEW_STYLE_CASTS 1
#  endif
# endif /* __WATCOM_CPLUSPLUS__ */

# if defined(__COMO__)
#   define __STL_MEMBER_TEMPLATES
#   define __STL_CLASS_PARTIAL_SPECIALIZATION
#   define __STL_USE_EXCEPTIONS
#   define __STL_DEFAULT_TEMPLATE_PARAM 1
#   define __STL_METHOD_SPECIALIZATION
#   undef  __STL_UNUSED_REQUIRED_BUG
#   undef  __STL_NONTEMPL_BASE_MATCH_BUG
# endif

// Symantec 7.5
# if defined (__SC__)
// if not using maximum ANSI compatibility ( -A -Ab -Aw),
// comment the following two lines out:
#  define __STL_BOOL_KEYWORD 1
#  define __STL_WCHAR_T 1
#  define __STL_NON_TYPE_TMPL_PARAM_BUG 1
#  define __STL_USE_EXCEPTIONS 1
#  define __STL_DEFAULT_TEMPLATE_PARAM 1
// #  define __STL_BASE_MATCH_BUG          1
// #  define __STL_NONTEMPL_BASE_MATCH_BUG  1
#  define __STL_UNINITIALIZABLE_PRIVATE  1
// #  define __STL_BASE_TYPEDEF_OUTSIDE_BUG 1
#	define __STL_THROW_RETURN_BUG	1
# endif

// Metrowerks CodeWarrior
# if defined (__MWERKS__)

#  define __STL_USE_EXCEPTIONS	1	// Enable exception handling

// *** CodeWarrior Compiler Features ***
#  include <ansi_parms.h>
#  if defined(__MSL_LONGLONG_SUPPORT__)
#   define __STL_LONG_LONG	1
#  endif
#   define __STL_BOOL_KEYWORD	1
#   define __STL_EXPLICIT 1
#   define __STL_EXCEPTIONS 1
#   define __STL_MUTABLE 1
#   define __STL_NEW_STYLE_CASTS 1

// *** CodeWarrior Compiler Bugs ***
#  define __STL_MULTI_CONST_TEMPLATE_ARG_BUG	1
#  define __STL_THROW_RETURN_BUG	1
#  define __STL_INLINE_NAME_RESOLUTION_BUG	1
#  define __STL_TEMPLATE_PARAM_SUBTYPE_BUG	1
#  define __STL_FORCED_INLINE_INSTANTIATION_BUG

// *** Metrowerks Standard Library Features ***
#  define __STL_BAD_ALLOC_DEFINED  1
#  define __STL_USE_NEW_STYLE_HEADERS 1
// _always_ using new-style system headers
#  define __STL_NEW_HEADER_NAMES 1
#   define __STL_IOSFWD 1

// *** Metrowerks Standard Library Bugs ***
#  define __STL_MSVC50_COMPATIBILITY 1
#  define __STL_NO_NAMESPACES 1	// Namespaces not implemented in MSL

// Version-specific settings
#  if __MWERKS__ >= 0x2000			// v. 2.0 features

// typename not yet enabled in MSL, so don't use it for now
// #   define __STL_TYPENAME				1

#   define __STL_WCHAR_T                1
#  else								// Pre-2.0 bugs
#   define  __STL_NON_TYPE_TMPL_PARAM_BUG	1	// dwa 8/21/97 - this bug fixed for CWPro2
#   define __STL_UNINITIALIZABLE_PRIVATE  1		// dwa 10/23/97 - this bug fixed for CWPro2
#  endif

#  if __MWERKS__ >= 0x1900         				// dwa 8/19/97 - 1.9 Compiler feature defines
#   define __STL_DEFAULT_TEMPLATE_PARAM 1
#  else								// Pre-1.9 bugs
#   if __MWERKS__ < 0x1800
    __GIVE_UP_WITH_STL(CW_18)
#   endif
#   define __STL_BASE_TYPEDEF_BUG        1
#   define __STL_BASE_MATCH_BUG   1
#   define __STL_NONTEMPL_BASE_MATCH_BUG 1
#   define __STL_DEFAULT_TYPE_PARAM  1			// More limited template parameters
#  endif

# endif /* __MWERKS__ */

// HP compilers
# if defined(__hpux) && !defined(__GNUC__)
#  if __cplusplus >= 199707L
// it is aCC
// use the most conservative configuration as the base
#  define __STL_BOOL_KEYWORD 1
#  define __STL_DEFAULT_TEMPLATE_PARAM 1
#  define __STL_RAND48 1

// there were reports on problems with namespaces 
// in complex cases for 1.06. If run into one of them,
/// you may want to disable the feature.
#  define __STL_NAMESPACES 1

#  define __STL_TYPENAME 1
#  define __STL_EXPLICIT 1
#  define __STL_USE_EXCEPTIONS 1
#  define __STL_EXCEPTION_SPEC 1
#  define __STL_NEW_STYLE_CASTS 1
#  define __STL_WCHAR_T 1
#  define __STL_LONG_LONG 1
#  define __STL_LONG_DOUBLE 1
#  define __STL_MUTABLE 1
#  define __STL_FULL_SPEC_SYNTAX 1
#  define __STL_BAD_ALLOC_DEFINED 1
#  define __STL_CLASS_PARTIAL_SPECIALIZATION 1

// aCC bug ? need explicit args on constructors of partial specialized
// classes
#  define __STL_PARTIAL_SPEC_NEEDS_TEMPLATE_ARGS 1

//  1.06 claims to support these features. You may try enabling them.
//  If running into aCC bugs, disable them back.
// #  define __STL_MEMBER_TEMPLATES 1

#  define __STL_FUNC_PARTIAL_ORDERING 1
#  define __STL_METHOD_SPECIALIZATION 1
// ?? fbp: really needed ?
#  define __STL_STATIC_ARRAY_BUG 1
#  else
// it is HP's old cfront-based compiler.
#  endif /* cfront */
# endif /* __hpux */

// common switches for EDG front-end
# if defined (__EDG_SWITCHES)
#   if defined(_TYPENAME)
#     define __STL_TYPENAME
#   endif
#   ifdef _PARTIAL_SPECIALIZATION_OF_CLASS_TEMPLATES
#     define __STL_CLASS_PARTIAL_SPECIALIZATION
#   endif
#   ifdef _MEMBER_TEMPLATES
#     define __STL_MEMBER_TEMPLATES
#   endif
#   ifdef __EXCEPTIONS
#     define __STL_USE_EXCEPTIONS
#   endif
#   undef __EDG_SWITCHES
# endif /* EDG */

# undef __GIVE_UP_WITH_STL

#endif
