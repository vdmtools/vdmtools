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

#ifndef __SGI_STL_INTERNAL_SET_H
#define __SGI_STL_INTERNAL_SET_H

__STL_BEGIN_NAMESPACE

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#endif
#define set __WORKAROUND_RENAME(set)

template <class Key, __DFL_TMPL_PARAM(Compare,less<Key>), 
                     __DFL_TYPE_PARAM(Alloc,alloc) >
class set {
    typedef set<Key, Compare, Alloc> self;
public:
// typedefs:
    typedef Key key_type;
    typedef Key value_type;
    typedef Compare key_compare;
    typedef Compare value_compare;
    typedef rb_tree<key_type, value_type, 
                    identity<value_type>, key_compare, Alloc> rep_type;
  typedef typename rep_type::const_pointer pointer;
  typedef typename rep_type::const_pointer const_pointer;
  typedef typename rep_type::const_reference reference;
  typedef typename rep_type::const_reference const_reference;
  typedef typename rep_type::const_iterator const_iterator;
    // SunPro 4.1, 4.0.1 bug
  typedef const_iterator iterator;
  typedef typename rep_type::const_reverse_iterator reverse_iterator;
  typedef typename rep_type::const_reverse_iterator const_reverse_iterator;
  typedef typename rep_type::size_type size_type;
  typedef typename rep_type::difference_type difference_type;
private:
    typedef typename rep_type::iterator rep_iterator;
    rep_type t;  // red-black tree representing set
public:

  // allocation/deallocation

  set() : t(Compare()) {}
  explicit set(const Compare& comp) : t(comp) {}

#ifdef __STL_MEMBER_TEMPLATES
  template <class InputIterator>
  set(InputIterator first, InputIterator last)
    : t(Compare()) { t.insert_unique(first, last); }

  template <class InputIterator>
  set(InputIterator first, InputIterator last, const Compare& comp)
    : t(comp) { t.insert_unique(first, last); }
#else
  set(const value_type* first, const value_type* last) 
    : t(Compare()) { t.insert_unique(first, last); }
  set(const value_type* first, const value_type* last, const Compare& comp)
    : t(comp) { t.insert_unique(first, last); }

  set(const_iterator first, const_iterator last)
    : t(Compare()) { t.insert_unique(first, last); }
  set(const_iterator first, const_iterator last, const Compare& comp)
    : t(comp) { t.insert_unique(first, last); }
#endif /* __STL_MEMBER_TEMPLATES */

  set(const set<Key, Compare, Alloc>& x) : t(x.t) {}
  set<Key, Compare, Alloc>& operator=(const set<Key, Compare, Alloc>& x) { 
    t = x.t; 
    return *this;
  }

  // accessors:

  key_compare key_comp() const { return t.key_comp(); }
  value_compare value_comp() const { return t.key_comp(); }
  iterator begin() const { return t.begin(); }
  iterator end() const { return t.end(); }
  reverse_iterator rbegin() const { return t.rbegin(); } 
  reverse_iterator rend() const { return t.rend(); }
  bool empty() const { return t.empty(); }
  size_type size() const { return t.size(); }
  size_type max_size() const { return t.max_size(); }
  void swap(set<Key, Compare, Alloc>& x) { t.swap(x.t); }

  // insert/erase
  typedef  pair<iterator, bool> pair_iterator_bool; 
  pair<iterator,bool> insert(const value_type& x) { 
    pair<rep_iterator, bool> p = t.insert_unique(x); 
    return pair<iterator, bool>(p.first, p.second);
  }
  iterator insert(iterator position, const value_type& x) {
    return t.insert_unique((rep_iterator&)position, x);
  }
#ifdef __STL_MEMBER_TEMPLATES
  template <class InputIterator>
  void insert(InputIterator first, InputIterator last) {
    t.insert_unique(first, last);
  }
#else
  void insert(const_iterator first, const_iterator last) {
    t.insert_unique(first, last);
  }
  void insert(const value_type* first, const value_type* last) {
    t.insert_unique(first, last);
  }
#endif /* __STL_MEMBER_TEMPLATES */
  void erase(iterator position) { 
    t.erase((rep_iterator&)position); 
  }
  size_type erase(const key_type& x) { 
    return t.erase(x); 
  }
  void erase(iterator first, iterator last) { 
    t.erase((rep_iterator&)first, 
            (rep_iterator&)last); 
  }
  void clear() { t.clear(); }

  // set operations:

  iterator find(const key_type& x) const { return t.find(x); }
  size_type count(const key_type& x) const { return t.count(x); }
  iterator lower_bound(const key_type& x) const {
    return t.lower_bound(x);
  }
  iterator upper_bound(const key_type& x) const {
    return t.upper_bound(x); 
  }
  pair<iterator,iterator> equal_range(const key_type& x) const {
    return t.equal_range(x);
  }
  friend bool operator== __STL_NULL_TMPL_ARGS (const self&, const self&);
  friend bool operator< __STL_NULL_TMPL_ARGS (const self&, const self&);
};

// do a cleanup
# undef set
// provide a way to access full funclionality 
# define __set__  __FULL_NAME(set)

template <class Key, class Compare, class Alloc>
inline bool operator==(const __set__<Key, Compare, Alloc>& x, 
                       const __set__<Key, Compare, Alloc>& y) {
  return operator==(x.t,y.t);
}

template <class Key, class Compare, class Alloc>
inline bool operator<(const __set__<Key, Compare, Alloc>& x, 
                      const __set__<Key, Compare, Alloc>& y) {
  return x.t < y.t ;
}

# if defined (__STL_CLASS_PARTIAL_SPECIALIZATION )
template <class Key, class Compare, class Alloc>
inline void swap(__set__<Key, Compare, Alloc>& a,
                 __set__<Key, Compare, Alloc>& b) { a.swap(b); }
# endif

# ifndef __STL_DEFAULT_TYPE_PARAM
// provide a "default" set adaptor
template <class Key, class Compare>
class set : public __set__<Key, Compare, alloc>
{
    typedef set<Key, Compare> self;
public:
    typedef __set__<Key, Compare, alloc> super;
    __CONTAINER_SUPER_TYPEDEFS
    // copy & assignment from super
    __IMPORT_SUPER_COPY_ASSIGNMENT(set)
    // specific constructors
    explicit set() : super(Compare()) {}
    explicit set(const Compare& comp) : super(comp) {}
    set(const value_type* first, const value_type* last) : 
        super(first, last, Compare()) { }
    set(const value_type* first, const value_type* last, 
        const Compare& comp) : super(first, last, comp) { }
    set(const_iterator first, const_iterator last) : 
        super(first, last, Compare()) { }
    set(const_iterator first, const_iterator last, 
        const Compare& comp) : super(first, last, comp) { }
};

#  if defined (__STL_BASE_MATCH_BUG)
template <class Key, class Compare>
inline bool operator==(const set<Key, Compare>& x, 
                       const set<Key, Compare>& y) {
  typedef  typename set<Key, Compare>::super super;
  return operator==((const super&)x,(const super&)y);
}

template <class Key, class Compare>
inline bool operator<(const set<Key, Compare>& x, 
                      const set<Key, Compare>& y) {
  typedef  typename set<Key, Compare>::super super;
  return operator < ((const super&)x , (const super&)y);
}
#  endif
# endif /*  __STL_DEFAULT_TEMPLATE_PARAM */

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#endif

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_SET_H */

// Local Variables:
// mode:C++
// End:
