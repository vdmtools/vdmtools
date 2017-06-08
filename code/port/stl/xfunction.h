/*
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

#ifndef __SGI_STL_XFUNCTION_H
#define __SGI_STL_XFUNCTION_H

#include <function.h>

// This header provides various non-standard functional extensions
// Some of them have to be specializations.

__STL_BEGIN_NAMESPACE


// Extension : void function

// used as adaptor's return/argument type, 
// to allow binders/composers usage
# ifndef __VOID_TAG_DEFINED
# define __VOID_TAG_DEFINED
  struct __void_tag {};
# endif

template <class Result>
class pointer_to_void_function {
protected:
    Result (*ptr)();
public:
    explicit pointer_to_void_function(Result (*x)()) : ptr(x) {}
    Result operator()() const { return ptr(); }
    Result operator()(__void_tag) const { return ptr(); }
};

// to feed composers
template <class Arg1>
struct projectvoid : public unary_function<Arg1,__void_tag> {
  __void_tag operator()(const Arg1& x) const { return __void_tag(); }
};

# if !defined (__STL_MEMBER_POINTER_PARAM_BUG)

template <class Result>
pointer_to_void_function<Result> ptr_fun(Result (*x)()) {
    return pointer_to_void_function<Result>(x);
}

// alternate name
template <class Result>
pointer_to_void_function<Result> ptr_gen(Result (*x)()) {
    return pointer_to_void_function<Result>(x);
}

# endif /*  !defined (__STL_MEMBER_POINTER_PARAM_BUG) */


// generators binding

#if !defined (__STL_BROKEN_PARAM_TYPEDEF)

template <class Operation, class Generator> 
class binder1st_gen : 
public unary_function<typename __BINARY_ARG(Operation,second_argument_type),
                      typename __BINARY_ARG(Operation,result_type)> {
protected:
    Operation op;
    Generator gen;
public:
    binder1st_gen(const Operation& x, const Generator& y) : op(x), gen(y) {}
    typename Operation::result_type 
        operator()(const typename Operation::second_argument_type& x) const {
	return op(gen(),x); 
    }
};

template <class Operation,class Generator>
inline binder1st_gen<Operation, Generator>
bind1st_gen(const Operation& op, const Generator& gen)
{
    return binder1st_gen<Operation,Generator>(op,gen);
} 

template <class Operation, class Generator> 
class binder2nd_gen : 
public unary_function<typename __BINARY_ARG(Operation,first_argument_type),
                      typename __BINARY_ARG(Operation,result_type)> {
protected:
    Operation op;
    Generator gen;
public:
    binder2nd_gen(const Operation& x, const Generator& y) : op(x), gen(y) {}
    typename Operation::result_type 
        operator()(const typename Operation::first_argument_type& x) const {
	return op(x, gen()); 
    }
};

template <class Operation,class Generator>
inline binder2nd_gen<Operation, Generator>
bind2nd_gen(const Operation& op, const Generator& gen)
{
    return binder2nd_gen<Operation,Generator>(op,gen);
} 

# endif

// macros to declare functional objects for pointers to members
#define mem_fun_macro(Result,Class,Func) \
struct : public unary_function<Class*,Result> \
{ Result operator()(Class* obj) const { return obj->Func(); }}

#define mem_fun1_macro(Result,Class,Func,Param) \
struct : public binary_function<Class*, Param,Result>  \
{ Result operator()(Class* obj, Param p) const { return obj->Func(p); }}

#define mem_fun_ref_macro(Result,Class,Func) \
struct : public unary_function<Class,Result> \
{ Result operator()(Class& obj) const { return obj.Func(); }}

#define mem_fun1_ref_macro(Result,Class,Func,Param) \
struct : public binary_function<Class, Param,Result>  \
{ Result operator()(Class& obj, Param p) const { return obj.Func(p); }}

__STL_END_NAMESPACE

#endif



