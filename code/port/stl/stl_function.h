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
 * Copyright (c) 1996
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

//# if !defined (__STL_STDDEF_H_INCLUDED)
//#  define __STL_STDDEF_H_INCLUDED
//#  include <stddef.h>
//# endif
//#include <pair.h>


#ifndef __SGI_STL_INTERNAL_FUNCTION_H
#define __SGI_STL_INTERNAL_FUNCTION_H

__STL_BEGIN_NAMESPACE

template <class Arg, class Result>
struct unary_function {
    typedef Arg argument_type;
    typedef Result result_type;
};

template <class Arg1, class Arg2, class Result>
struct binary_function {
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Result result_type;
};      

template <class T>
struct plus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x + y; }
};

template <class T>
struct minus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x - y; }
};

template <class T>
struct multiplies : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x * y; }
};

template <class T>
struct divides : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x / y; }
};

template <class T> inline T identity_element(plus<T>) { return T(0); }

template <class T> inline T identity_element(multiplies<T>) { return T(1); }

template <class T>
struct modulus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x % y; }
};

template <class T>
struct negate : public unary_function<T, T> {
    T operator()(const T& x) const { return -x; }
};

template <class T>
struct equal_to : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x == y; }
};

template <class T>
struct not_equal_to : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x != y; }
};

template <class T>
struct greater : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x > y; }
};

template <class T>
struct less : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x < y; }
};

template <class T>
struct greater_equal : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x >= y; }
};

template <class T>
struct less_equal : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x <= y; }
};

template <class T>
struct logical_and : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x && y; }
};

template <class T>
struct logical_or : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x || y; }
};

template <class T>
struct logical_not : public unary_function<T, bool> {
    bool operator()(const T& x) const { return !x; }
};

#  if defined (__STL_BASE_TYPEDEF_BUG)
// this workaround is needed for SunPro 4.0.1
// suggested by "Martin Abernethy" <gma@paston.co.uk>:

// We have to introduce the XXary_predicate_aux structures in order to
// access the argument and return types of predicate functions supplied
// as type parameters. SUN C++ 4.0.1 compiler gives errors for template type parameters
// of the form 'name1::name2', where name1 is itself a type parameter.
template <class Pair>
struct __pair_aux : private Pair
{
	typedef typename Pair::first_type first_type;
	typedef typename Pair::second_type second_type;
};

template <class Operation>
struct __unary_fun_aux : private Operation
{
	typedef typename Operation::argument_type argument_type;
	typedef typename Operation::result_type result_type;
};

template <class Operation>
struct __binary_fun_aux  : private Operation
{
	typedef typename Operation::first_argument_type first_argument_type;
	typedef typename Operation::second_argument_type second_argument_type;
	typedef typename Operation::result_type result_type;
};

#  define __UNARY_ARG(Operation,type)  __unary_fun_aux<Operation>::type
#  define __BINARY_ARG(Operation,type)  __binary_fun_aux<Operation>::type
#  define __PAIR_ARG(Pair,type)  __pair_aux<Pair>::type
# else
#  define __UNARY_ARG(Operation,type)  Operation::type
#  define __BINARY_ARG(Operation,type) Operation::type
#  define __PAIR_ARG(Pair,type) Pair::type
# endif

template <class Predicate>
class unary_negate : 
    public unary_function<typename __UNARY_ARG(Predicate,argument_type), bool> {
protected:
    Predicate pred;
public: 
    explicit unary_negate(const Predicate& x) : pred(x) {}
    bool operator()(const typename Predicate::argument_type& x) const { return !pred(x); }
};

template <class Predicate>
inline unary_negate<Predicate> not1(const Predicate& pred) {
  return unary_negate<Predicate>(pred);
}

template <class Predicate> 
class binary_negate 
    : public binary_function<typename __BINARY_ARG(Predicate,first_argument_type),
			     typename __BINARY_ARG(Predicate,second_argument_type), 
                             bool> {
protected:
    Predicate pred;
public:
    explicit binary_negate(const Predicate& x) : pred(x) {}
    bool operator()(const typename Predicate::first_argument_type& x, 
		    const typename Predicate::second_argument_type& y) const {
	return !pred(x, y); 
    }
};

template <class Predicate>
inline binary_negate<Predicate> not2(const Predicate& pred) {
  return binary_negate<Predicate>(pred);
}

template <class Operation> 
class binder1st : 
    public unary_function<typename __BINARY_ARG(Operation,second_argument_type),
                          typename __BINARY_ARG(Operation,result_type) > {
protected:
    Operation op;
    typename Operation::first_argument_type value;
public:
    binder1st(const Operation& x, 
              const typename Operation::first_argument_type& y)
	: op(x), value(y) {}
    typename Operation::result_type
        operator()(const typename Operation::second_argument_type& x) const {
	return op(value, x); 
    }
};

template <class Operation, class T>
inline binder1st<Operation> bind1st(const Operation& op, const T& x) {
    typedef typename Operation::first_argument_type arg_type;
    return binder1st<Operation>(op, arg_type(x));
}

template <class Operation> 
class binder2nd
  : public unary_function<typename __BINARY_ARG(Operation,first_argument_type),
                          typename __BINARY_ARG(Operation,result_type)> {
protected:
    Operation op;
    typename Operation::second_argument_type value;
public:
    binder2nd(const Operation& x,
              const typename Operation::second_argument_type& y) 
	: op(x), value(y) {}
    typename Operation::result_type
        operator()(const typename Operation::first_argument_type& x) const {
	return op(x, value); 
    }
};

template <class Operation, class T>
inline binder2nd<Operation> bind2nd(const Operation& op, const T& x) {
  typedef typename Operation::second_argument_type arg2_type;
  return binder2nd<Operation>(op, arg2_type(x));
}

template <class Operation1, class Operation2>
class unary_compose : 
    public unary_function<typename __UNARY_ARG(Operation2,argument_type),
                          typename __UNARY_ARG(Operation1,result_type)> {
protected:
    Operation1 op1;
    Operation2 op2;
public:
    unary_compose(const Operation1& x, const Operation2& y) : op1(x), op2(y) {}
    typename Operation1::result_type 
    operator()(const typename Operation2::argument_type& x) const {
	return op1(op2(x));
    }
};

template <class Operation1, class Operation2>
inline unary_compose<Operation1, Operation2> compose1(const Operation1& op1, 
                                                      const Operation2& op2) {
  return unary_compose<Operation1, Operation2>(op1, op2);
}

template <class Operation1, class Operation2, class Operation3>
class binary_compose : 
    public unary_function<typename __UNARY_ARG(Operation2,argument_type),
                          typename __BINARY_ARG(Operation1,result_type)> {
protected:
    Operation1 op1;
    Operation2 op2;
    Operation3 op3;
public:
    binary_compose(const Operation1& x, const Operation2& y, 
		   const Operation3& z) : op1(x), op2(y), op3(z) { }
    typename Operation1::result_type
        operator()(const typename Operation2::argument_type& x) const {
	return op1(op2(x), op3(x));
    }
};

template <class Operation1, class Operation2, class Operation3>
inline binary_compose<Operation1, Operation2, Operation3> 
compose2(const Operation1& op1, const Operation2& op2, const Operation3& op3) {
  return binary_compose<Operation1, Operation2, Operation3>(op1, op2, op3);
}

template <class Arg, class Result>
class pointer_to_unary_function : public unary_function<Arg, Result> {
protected:
    Result (*ptr)(Arg);
public:
    pointer_to_unary_function() {}
    explicit pointer_to_unary_function(Result (*x)(Arg)) : ptr(x) {}
    Result operator()(Arg x) const { return ptr(x); }
};

template <class Arg, class Result>
inline pointer_to_unary_function<Arg, Result> ptr_fun(Result (*x)(Arg)) {
  return pointer_to_unary_function<Arg, Result>(x);
}

template <class Arg1, class Arg2, class Result>
class pointer_to_binary_function : public binary_function<Arg1, Arg2, Result> {
protected:
    Result (*ptr)(Arg1, Arg2);
public:
    pointer_to_binary_function() {}
    explicit pointer_to_binary_function(Result (*x)(Arg1, Arg2)) : ptr(x) {}
    Result operator()(Arg1 x, Arg2 y) const { return ptr(x, y); }
};

template <class Arg1, class Arg2, class Result>
inline pointer_to_binary_function<Arg1, Arg2, Result> 
ptr_fun(Result (*x)(Arg1, Arg2)) {
  return pointer_to_binary_function<Arg1, Arg2, Result>(x);
}

template <class T>
struct identity : public unary_function<T, T> {
    const T& operator()(const T& x) const { return x; }
};

#ifdef __STL_MULTI_CONST_TEMPLATE_ARG_BUG
// fbp : sort of select1st just for maps
template <class Pair, class U>		
// JDJ (CW Pro1 doesn't like const when first_type is also const)
struct __select1st_hint : public unary_function<Pair, U> {
    const U& operator () (const Pair& x) const { return x.first; }
};
# endif

template <class Pair>
struct select1st : public unary_function<Pair, typename __PAIR_ARG(Pair,first_type)> {
  const typename Pair::first_type& operator()(const Pair& x) const
  {
    return x.first;
  }
};

template <class Pair>
struct select2nd : public unary_function<Pair, typename __PAIR_ARG(Pair,second_type)> {
  const typename Pair::second_type& operator()(const Pair& x) const
  {
    return x.second;
  }
};

template <class Arg1, class Arg2>
struct project1st : public binary_function<Arg1, Arg2, Arg1> {
  Arg1 operator()(const Arg1& x, const Arg2&) const { return x; }
};

template <class Arg1, class Arg2>
struct project2nd : public binary_function<Arg1, Arg2, Arg2> {
  Arg2 operator()(const Arg1&, const Arg2& y) const { return y; }
};

//  SGI extension (constant functions)

template <class Result>
struct constant_void_fun
{
  typedef Result result_type;
  result_type val;
  constant_void_fun(const result_type& v) : val(v) {}
  const result_type& operator()() const { return val; }
};  

template <class Result, __DFL_TMPL_PARAM(Argument, Result) >
struct constant_unary_fun : public unary_function<Argument, Result> {
  Result val;
  constant_unary_fun(const Result& v) : val(v) {}
  const Result& operator()(const Argument&) const { return val; }
};

template <class Result, __DFL_TMPL_PARAM(Arg1,Result), 
    __DFL_TMPL_PARAM(Arg2,Arg1) >
struct constant_binary_fun : public binary_function<Arg1, Arg2, Result> {
  typedef Result result_type;
  typedef Arg1 first_argument_type;
  typedef Arg2 second_argument_type;
  Result val;
  constant_binary_fun(const Result& v) : val(v) {}
  const Result& operator()(const Arg1&, const Arg2&) const {
    return val;
  }
};

template <class Result>
inline constant_void_fun<Result> constant0(const Result& val)
{
  return constant_void_fun<Result>(val);
}

template <class Result>
inline constant_unary_fun<Result __DFL_TMPL_ARG(Result) > constant1(const Result& val)
{
  return constant_unary_fun<Result,Result>(val);
}

template <class Result>
inline constant_binary_fun<Result,Result,Result> constant2(const Result& val)
{
  return constant_binary_fun<Result,Result,Result>(val);
}

//  SGI extension (subtractive range)

// Note: this code assumes that T is 32-bit unsigned integer.
template < class T >
class __subtractive_rng_t : public unary_function<T, T> {
private:
  T table[55];
  size_t index1;
  size_t index2;
public:
  __subtractive_rng_t(T seed) { initialize(seed); }
  __subtractive_rng_t() { initialize(161803398ul); }

  T operator()(T limit) {
    index1 = (index1 + 1) % 55;
    index2 = (index2 + 1) % 55;
    table[index1] = table[index1] - table[index2];
    return table[index1] % limit;
  }
  void initialize(T seed);
};

template <class T>
void __subtractive_rng_t<T>::initialize(T seed) {
    T k = 1;
    table[54] = seed;
    size_t i;
    for (i = 0; i < 54; i++) {
        size_t ii = (21 * (i + 1) % 55) - 1;
        table[ii] = k;
        k = seed - k;
        seed = table[ii];
    }
    for (int loop = 0; loop < 4; loop++) {
        for (i = 0; i < 55; i++)
            table[i] = table[i] - table[(1 + i + 30) % 55];
    }
    index1 = 0;
    index2 = 31;
  }

typedef __subtractive_rng_t<__STL_UINT32_T> subtractive_rng;

// Adaptor function objects: pointers to member functions.

// There are a total of 16 = 2^4 function objects in this family.
//  (1) Member functions taking no arguments vs member functions taking
//       one argument.
//  (2) Call through pointer vs call through reference.
//  (3) Member function with void return type vs member function with
//      non-void return type.
//  (4) Const vs non-const member function.

// Note that choice (4) is not present in the 8/97 draft C++ standard, 
//  which only allows these adaptors to be used with non-const functions.
//  This is likely to be recified before the standard becomes final.
// Note also that choice (3) is nothing more than a workaround: according
//  to the draft, compilers should handle void and non-void the same way.
//  This feature is not yet widely implemented, though.  You can only use
//  member functions returning void if your compiler supports partial
//  specialization.

// All of this complexity is in the function objects themselves.  You can
//  ignore it by using the helper function mem_fun, mem_fun_ref,
//  mem_fun1, and mem_fun1_ref, which create whichever type of adaptor
//  is appropriate.


template <class S, class T>
class mem_fun_t : public unary_function<T*, S> {
  typedef S (T::*fun_type)(void);
public:
  explicit mem_fun_t(fun_type pf) : f(pf) {}
  S operator()(T* p) const { return (p->*f)(); }
private:
  fun_type f;
};

template <class S, class T>
class const_mem_fun_t : public unary_function<const T*, S> {
  typedef S (T::*fun_type)(void) const;
public:
  explicit const_mem_fun_t(fun_type pf) : f(pf) {}
  S operator()(const T* p) const { return (p->*f)(); }
private:
  fun_type f;
};


template <class S, class T>
class mem_fun_ref_t : public unary_function<T, S> {
  typedef S (T::*fun_type)(void);
public:
  explicit mem_fun_ref_t(fun_type pf) : f(pf) {}
  S operator()(T& r) const { return (r.*f)(); }
private:
  fun_type f;
};

template <class S, class T>
class const_mem_fun_ref_t : public unary_function<T, S> {
  typedef S (T::*fun_type)(void) const;
public:
  explicit const_mem_fun_ref_t(fun_type pf) : f(pf) {}
  S operator()(const T& r) const { return (r.*f)(); }
private:
  fun_type f;
};

template <class S, class T, class A>
class mem_fun1_t : public binary_function<T*, A, S> {
  typedef S (T::*fun_type)(A);
public:
  explicit mem_fun1_t(fun_type pf) : f(pf) {}
  S operator()(T* p, A x) const { return (p->*f)(x); }
private:
  fun_type f;
};

template <class S, class T, class A>
class const_mem_fun1_t : public binary_function<const T*, A, S> {
  typedef S (T::*fun_type)(A) const;
public:
  explicit const_mem_fun1_t(fun_type pf) : f(pf) {}
  S operator()(const T* p, A x) const { return (p->*f)(x); }
private:
  fun_type f;
};

template <class S, class T, class A>
class mem_fun1_ref_t : public binary_function<T, A, S> {
  typedef S (T::*fun_type)(A);
public:
  explicit mem_fun1_ref_t(fun_type pf) : f(pf) {}
  S operator()(T& r, A x) const { return (r.*f)(x); }
private:
  fun_type f;
};

template <class S, class T, class A>
class const_mem_fun1_ref_t : public binary_function<T, A, S> {
  typedef S (T::*fun_type)(A) const;
public:
  explicit const_mem_fun1_ref_t(fun_type pf) : f(pf) {}
  S operator()(const T& r, A x) const { return (r.*f)(x); }
private:
  fun_type f;
};

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION

#ifdef __STL_PARTIAL_SPEC_NEEDS_TEMPLATE_ARGS
#define __STL_PSPEC2(t1,t2) <t1,t2>
#define __STL_PSPEC3(t1,t2,t3) <t1,t2,t3>
#else
#define __STL_PSPEC2(t1,t2)	/* nothing */
#define __STL_PSPEC3(t1,t2,t3)	/* nothing */

#endif

template <class T>
class mem_fun_t<void, T> : public unary_function<T*, void> {
  typedef void (T::*fun_type)(void);
public:
  explicit mem_fun_t __STL_PSPEC2(void,T) (fun_type pf) : f(pf) {}
  void operator()(T* p) const { (p->*f)(); }
private:
  fun_type f;
};

template <class T>
class const_mem_fun_t<void, T> : public unary_function<const T*, void> {
  typedef void (T::*fun_type)(void) const;
public:
  explicit const_mem_fun_t __STL_PSPEC2(void,T) (fun_type pf) : f(pf) {}
  void operator()(const T* p) const { (p->*f)(); }
private:
  fun_type f;
};

template <class T>
class mem_fun_ref_t<void, T> : public unary_function<T, void> {
  typedef void (T::*fun_type)(void);
public:
  explicit mem_fun_ref_t __STL_PSPEC2(void,T) (fun_type pf) : f(pf) {}
  void operator()(T& r) const { (r.*f)(); }
private:
  fun_type f;
};

template <class T>
class const_mem_fun_ref_t<void, T> : public unary_function<T, void> {
  typedef void (T::*fun_type)(void) const;
public:
  explicit const_mem_fun_ref_t __STL_PSPEC2(void,T) (fun_type pf) : f(pf) {}
  void operator()(const T& r) const { (r.*f)(); }
private:
  fun_type f;
};

template <class T, class A>
class mem_fun1_t<void, T, A> : public binary_function<T*, A, void> {
  typedef void (T::*fun_type)(A);
public:
  explicit mem_fun1_t __STL_PSPEC3(void,T,A) (fun_type pf) : f(pf) {}
  void operator()(T* p, A x) const { (p->*f)(x); }
private:
  fun_type f;
};

template <class T, class A>
class const_mem_fun1_t<void, T, A> : public binary_function<const T*, A, void> {
  typedef void (T::*fun_type)(A) const;
public:
  explicit const_mem_fun1_t __STL_PSPEC3(void,T,A) (fun_type pf) : f(pf) {}
  void operator()(const T* p, A x) const { (p->*f)(x); }
private:
  fun_type f;
};

template <class T, class A>
class mem_fun1_ref_t<void, T, A> : public binary_function<T, A, void> {
  typedef void (T::*fun_type)(A);
public:
  explicit mem_fun1_ref_t __STL_PSPEC3(void,T,A) (fun_type pf) : f(pf) {}
  void operator()(T& r, A x) const { (r.*f)(x); }
private:
  fun_type f;
};

template <class T, class A>
class const_mem_fun1_ref_t<void, T, A> : public binary_function<T, A, void> {
  typedef void (T::*fun_type)(A) const;
public:
  explicit const_mem_fun1_ref_t __STL_PSPEC3(void,T,A) (fun_type pf) : f(pf) {}
  void operator()(const T& r, A x) const { (r.*f)(x); }
private:
  fun_type f;
};

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

# if !defined (__STL_MEMBER_POINTER_PARAM_BUG)

// Mem_fun adaptor helper functions.  There are only four:
//  mem_fun, mem_fun_ref, mem_fun1, mem_fun1_ref.

template <class S, class T>
inline mem_fun_t<S,T> mem_fun(S (T::*f)()) { 
  return mem_fun_t<S,T>(f);
}

template <class S, class T>
inline const_mem_fun_t<S,T> mem_fun(S (T::*f)() const) {
  return const_mem_fun_t<S,T>(f);
}

template <class S, class T>
inline mem_fun_ref_t<S,T> mem_fun_ref(S (T::*f)()) { 
  return mem_fun_ref_t<S,T>(f);
}

template <class S, class T>
inline const_mem_fun_ref_t<S,T> mem_fun_ref(S (T::*f)() const) {
  return const_mem_fun_ref_t<S,T>(f);
}

template <class S, class T, class A>
inline mem_fun1_t<S,T,A> mem_fun1(S (T::*f)(A)) { 
  return mem_fun1_t<S,T,A>(f);
}

template <class S, class T, class A>
inline const_mem_fun1_t<S,T,A> mem_fun1(S (T::*f)(A) const) {
  return const_mem_fun1_t<S,T,A>(f);
}

template <class S, class T, class A>
inline mem_fun1_ref_t<S,T,A> mem_fun1_ref(S (T::*f)(A)) { 
  return mem_fun1_ref_t<S,T,A>(f);
}

template <class S, class T, class A>
inline const_mem_fun1_ref_t<S,T,A> mem_fun1_ref(S (T::*f)(A) const) {
  return const_mem_fun1_ref_t<S,T,A>(f);
}

# endif /* __STL_MEMBER_POINTER_PARAM_BUG */

__STL_END_NAMESPACE

#endif  /* __SGI_STL_INTERNAL_FUNCTION_H */
