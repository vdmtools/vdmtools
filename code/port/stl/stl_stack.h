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

#ifndef __SGI_STL_INTERNAL_STACK_H
#define __SGI_STL_INTERNAL_STACK_H

__STL_BEGIN_NAMESPACE

#ifndef __STL_LIMITED_DEFAULT_TEMPLATES
template <class T, class Sequence_T = deque<T> >
#else
template <class T, class Sequence_T>
#endif
class stack {
  friend inline bool operator== __STL_NULL_TMPL_ARGS (const stack<T, Sequence_T>& x,
                                const stack<T, Sequence_T>& y);
  friend inline bool operator< __STL_NULL_TMPL_ARGS (const stack<T, Sequence_T>& x,
                               const stack<T, Sequence_T>& y);
public:
  typedef typename Sequence_T::value_type value_type;
  typedef typename Sequence_T::size_type size_type;
  typedef typename Sequence_T::reference reference;
  typedef typename Sequence_T::const_reference const_reference;
protected:
  Sequence_T c;
public:
  bool empty() const { return c.empty(); }
  size_type size() const { return c.size(); }
  reference top() { return c.back(); }
  const_reference top() const { return c.back(); }
  void push(const value_type& x) { c.push_back(x); }
  void pop() { c.pop_back(); }
};

template <class T, class Sequence_T>
inline
bool operator==(const stack<T, Sequence_T>& x, const stack<T, Sequence_T>& y) {
  return x.c == y.c;
}

template <class T, class Sequence_T>
inline
bool operator<(const stack<T, Sequence_T>& x, const stack<T, Sequence_T>& y) {
  return x.c < y.c;
}


# if defined  (__STL_DEFAULT_TEMPLATE_PARAM)
#  define simple_stack stack
# else
// provide ways to access short funclionality

// provide a "default" stack adaptor
template <class T>
class simple_stack : public stack<T, vector<T> > 
{
public:
    simple_stack() {}
    ~simple_stack() {}
};

# endif /* __STL_DEFAULT_TEMPLATE_PARAM */

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_STACK_H */

// Local Variables:
// mode:C++
// End:
