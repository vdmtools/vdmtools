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

#ifndef __SGI_STL_INTERNAL_BVECTOR_H
#define __SGI_STL_INTERNAL_BVECTOR_H

// #include <stddef.h>
# ifndef __SGI_STL_ALGOBASE_H
#  include <algobase.h>
# endif
# ifndef __SGI_STL_ALLOC_H
#  include <alloc.h>
# endif

__STL_BEGIN_NAMESPACE

#define __WORD_BIT (int(CHAR_BIT*sizeof(unsigned int)))

struct __bit_iterator;
struct __bit_const_iterator;

# if ! defined (__STL_CLASS_PARTIAL_SPECIALIZATION)
inline random_access_iterator_tag 
iterator_category(const __bit_iterator&) {return random_access_iterator_tag();}
inline random_access_iterator_tag 
iterator_category(const __bit_const_iterator&) {return random_access_iterator_tag();}
inline ptrdiff_t* 
distance_type(const __bit_iterator&) {return (ptrdiff_t*)0;}
inline ptrdiff_t* 
distance_type(const __bit_const_iterator&) {return (ptrdiff_t*)0;}
inline bool* value_type(const __bit_iterator&) {return (bool*)0;}
inline bool* value_type(const __bit_const_iterator&) {return (bool*)0;}
# endif

class __bit_reference {
  friend struct __bit_iterator;
  friend struct __bit_const_iterator;
  typedef __bit_reference reference;
protected:
  unsigned int* p;
  unsigned int mask;
  __bit_reference(unsigned int* x, unsigned int y) : p(x), mask(y) {}
public:
  __bit_reference() : p(0), mask(0) {}
  operator bool() const { return !(!(*p & mask)); }
  reference& operator=(bool x) {
    if (x)      
      *p |= mask;
    else 
      *p &= ~mask;
    return *this;
  }
  reference& operator=(const reference& x) { return *this = bool(x); }
  bool operator==(const reference& x) const {
    return bool(*this) == bool(x);
  }
  bool operator<(const reference& x) const {
    return int(bool(*this)) < int(bool(x));
  }
  void flip() { *p ^= mask; }
};


struct __bit_iterator : public random_access_iterator<bool, ptrdiff_t>
# if defined (__STL_DEBUG)
, public __safe_base
# endif
{
  typedef __bit_reference reference;
  typedef __bit_reference* pointer;
  typedef __bit_iterator iterator;
  typedef random_access_iterator_tag iterator_category;
  //    typedef bool const_reference;
  typedef bool value_type;
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;

  unsigned int* p;
  unsigned int offset;
  void bump_up() {
    if (offset++ == __WORD_BIT - 1) {
      offset = 0;
      ++p;
    }
  }
  void bump_down() {
    if (offset-- == 0) {
      offset = __WORD_BIT - 1;
      --p;
    }
  }

# if defined ( __STL_DEBUG )
  bool unsafe;
  bool overrun_ok() const { return unsafe; } 
  void set_overrun(bool val) { unsafe = val; } 
  inline bool dereferenceable() const;
  const __bit_iterator& get_iterator() const { return *this; }  
  __bit_iterator() : __safe_base(0), p(0), offset(0), unsafe(false) {}
  __bit_iterator(const __safe_base* root,unsigned int* x, 
		 unsigned int y, bool over = false) : 
    __safe_base(root), p(x), offset(y), unsafe(over) {}
  __bit_iterator(unsigned int* x, unsigned int y) : 
    __safe_base(0), p(x), offset(y), unsafe(false) {}
# else
  __bit_iterator() : p(0), offset(0) {}
  __bit_iterator(unsigned int* x, unsigned int y) : p(x), offset(y) {}
# endif
  __bit_reference operator*() const { 
    __stl_debug_check(dereferenceable());    
    return __bit_reference(p, 1U << offset); 
  }
  iterator& operator++() {
    bump_up();
    return *this;
  }
  iterator operator++(int) {
    iterator tmp = *this;
    bump_up();
    return tmp;
  }
  iterator& operator--() {
    bump_down();
    return *this;
  }
  iterator operator--(int) {
    iterator tmp = *this;
    bump_down();
    return tmp;
  }
  iterator& operator+=(difference_type i) {
    difference_type n = i + offset;
    p += n / __WORD_BIT;
    n = n % __WORD_BIT;
    if (n < 0) {
      offset = n + __WORD_BIT;
      --p;
    } else
      offset = n;
    return *this;
  }
  iterator& operator-=(difference_type i) {
    *this += -i;
    return *this;
  }
  iterator operator+(difference_type i) const {
    iterator tmp = *this;
    return tmp += i;
  }
  iterator operator-(difference_type i) const {
    iterator tmp = *this;
    return tmp -= i;
  }
  difference_type operator-(iterator x) const {
    __stl_debug_check(__check_same_owner(*this,x));    
    return __WORD_BIT * (p - x.p) + offset - x.offset;
  }
  __bit_reference operator[](ptrdiff_t i) const { return (*this + i).operator*(); }
  bool operator==(const iterator& x) const {
    __stl_debug_check(__check_same_owner(*this,x));    
    return p == x.p && offset == x.offset;
  }
  bool operator!=(const iterator& x) const {
    __stl_debug_check(__check_same_owner(*this,x));    
    return p != x.p || offset != x.offset;
  }
  bool operator<(iterator x) const {
    __stl_debug_check(__check_same_owner(*this,x));    
    return p < x.p || (p == x.p && offset < x.offset);
  }
};

struct __bit_const_iterator
  : public random_access_iterator<bool, ptrdiff_t>
# if defined (__STL_DEBUG)
  , public __safe_base
# endif
{
  typedef bool                 reference;
  typedef bool                 const_reference;
  typedef const bool*          pointer;
  typedef __bit_const_iterator const_iterator;
  typedef __bit_iterator iterator;
  typedef random_access_iterator_tag iterator_category;
  typedef bool value_type;
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;

  unsigned int* p;
  unsigned int offset;
  void bump_up() {
    if (offset++ == __WORD_BIT - 1) {
      offset = 0;
      ++p;
    }
  }
  void bump_down() {
    if (offset-- == 0) {
      offset = __WORD_BIT - 1;
      --p;
    }
  }

# if defined ( __STL_DEBUG )
  bool unsafe;
  bool overrun_ok() const { return unsafe; } 
  void set_overrun(bool val) { unsafe = val; } 
  inline bool dereferenceable() const;
  const __bit_const_iterator& get_iterator() const { return *this; }  
  __bit_const_iterator() : __safe_base(0), p(0), offset(0), unsafe(false) {}
  __bit_const_iterator(const __safe_base* root,unsigned int* x, 
		       unsigned int y, bool over = false) : 
    __safe_base(root), p(x), offset(y), unsafe(over) {}
  __bit_const_iterator(const iterator& x) :  __safe_base(x.owner_),
    p(x.p), offset(x.offset), unsafe(x.unsafe) {}
  __bit_const_iterator(unsigned int* x, unsigned int y) : 
    __safe_base(0), p(x), offset(y), unsafe(false) {}
# else
  __bit_const_iterator() : p(0), offset(0) {}
  __bit_const_iterator(unsigned int* x, unsigned int y) : p(x), offset(y) {}
  __bit_const_iterator(const iterator& x) : p(x.p), offset(x.offset) {}
# endif
  reference operator*() const {
    __stl_debug_check(dereferenceable());    
    return (value_type)__bit_reference(p, 1U << offset);
  }
  const_iterator& operator++() {
    bump_up();
    return *this;
  }
  const_iterator operator++(int) {
    const_iterator tmp = *this;
    bump_up();
    return tmp;
  }
  const_iterator& operator--() {
    bump_down();
    return *this;
  }
  const_iterator operator--(int) {
    const_iterator tmp = *this;
    bump_down();
    return tmp;
  }
  const_iterator& operator+=(difference_type i) {
    difference_type n = i + offset;
    p += n / __WORD_BIT;
    n = n % __WORD_BIT;
    if (n < 0) {
      offset = n + __WORD_BIT;
      --p;
    } else
      offset = n;
    return *this;
  }
  const_iterator& operator-=(difference_type i) {
    *this += -i;
    return *this;
  }
  const_iterator operator+(difference_type i) const {
    const_iterator tmp = *this;
    return tmp += i;
  }
  const_iterator operator-(difference_type i) const {
    const_iterator tmp = *this;
    return tmp -= i;
  }
  difference_type operator-(const_iterator x) const {
    __stl_debug_check(__check_same_owner(*this,x));    
    return __WORD_BIT * (p - x.p) + offset - x.offset;
  }
  reference operator[](difference_type i) const { 
    return *(*this + i); 
  }
  bool operator==(const const_iterator& x) const {
    __stl_debug_check(__check_same_owner(*this,x));    
    return p == x.p && offset == x.offset;
  }
  bool operator!=(const const_iterator& x) const {
    __stl_debug_check(__check_same_owner(*this,x));    
    return p != x.p || offset != x.offset;
  }
  bool operator<(const_iterator x) const {
    __stl_debug_check(__check_same_owner(*this,x));    
    return p < x.p || (p == x.p && offset < x.offset);
  }
};

// The next few lines are confusing.  What we're doing is declaring a
//  partial specialization of vector<T, Alloc> if we have the necessary
//  compiler support.  Otherwise, we define a class bit_vector which uses
//  the default allocator.  In either case, we typedef "data_allocator" 
//  appropriately.

#if defined(__STL_CLASS_PARTIAL_SPECIALIZATION) && defined(__STL_BOOL_KEYWORD)
#define __SGI_STL_VECBOOL_TEMPLATE
#define __BVECTOR vector
#else
#undef __SGI_STL_VECBOOL_TEMPLATE
#define __BVECTOR bit_vector
#endif

#      ifdef __SGI_STL_VECBOOL_TEMPLATE
__STL_END_NAMESPACE
#      include <stl_vector.h>
__STL_BEGIN_NAMESPACE
template<class Alloc> class vector<bool, Alloc>
#      else /* __SGI_STL_VECBOOL_TEMPLATE */
class bit_vector
#      endif /* __SGI_STL_VECBOOL_TEMPLATE */
# ifdef __STL_DEBUG
: public __safe_server
# endif
{
#      ifdef __SGI_STL_VECBOOL_TEMPLATE
  typedef simple_alloc<unsigned int, Alloc> data_allocator;
#      else /* __SGI_STL_VECBOOL_TEMPLATE */
  typedef simple_alloc<unsigned int, alloc> data_allocator;  
#      endif /* __SGI_STL_VECBOOL_TEMPLATE */
public:
  typedef bool value_type;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type; 
  typedef __bit_iterator iterator;
  typedef __bit_const_iterator const_iterator;
  typedef __bit_reference* pointer;
  typedef const bool* const_pointer;
  typedef __bit_reference reference;
  typedef bool const_reference;

#if defined ( __STL_CLASS_PARTIAL_SPECIALIZATION ) && \
  ! defined (__STL_PARTIAL_SPECIALIZATION_BUG)
    typedef reverse_iterator<const_iterator> const_reverse_iterator;
  typedef reverse_iterator<iterator> reverse_iterator;
#else /* __STL_CLASS_PARTIAL_SPECIALIZATION */
# if defined (__STL_MSVC50_COMPATIBILITY)
  // not really true, but harmless
  typedef reverse_iterator<const_iterator, value_type, bool, 
  const bool*, difference_type> const_reverse_iterator;
  typedef reverse_iterator<iterator, value_type, reference, reference*, 
  difference_type> reverse_iterator;
# else
  typedef reverse_iterator<const_iterator, value_type, const_reference, 
  difference_type> const_reverse_iterator;
  typedef reverse_iterator<iterator, value_type, reference, difference_type>
  reverse_iterator;
# endif
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */
protected:
  iterator start;
  iterator finish;
# if defined (__STL_DEBUG)
  iterator make_iterator(unsigned int* x, unsigned int y, 
			 bool over=false) { return iterator(this,x,y,over); }
  const_iterator make_const_iterator(unsigned int* x, unsigned int y,
				     bool over=false ) const { 
    return const_iterator(this,x,y,over); 
  }
# else
  iterator make_iterator(unsigned int* x, unsigned int y, 
			 bool = false) { return iterator(x,y); }
  const_iterator make_const_iterator(unsigned int* x, unsigned int y,
				     bool = false ) const { 
    return const_iterator(x,y); 
  }
# endif
  unsigned int* end_of_storage;
  unsigned int* bit_alloc(size_type n) {
    return data_allocator::allocate((n + __WORD_BIT - 1)/__WORD_BIT);
  }
  void deallocate() {
    if (start.p)
      data_allocator::deallocate(start.p, end_of_storage - start.p);
    __stl_debug_do(invalidate_all());
  }
  void initialize(size_type n) {
    unsigned int* q = bit_alloc(n);
    end_of_storage = q + (n + __WORD_BIT - 1)/__WORD_BIT;
    __stl_debug_do(safe_init(this));
    start = iterator(q, 0);
    finish = start+difference_type(n);
    __stl_debug_do(init_bounds());
  }
  void insert_aux(iterator position, bool x) {
    __stl_debug_do(finish.set_overrun(true));
    if (finish.p != end_of_storage) {
      copy_backward(position, finish, finish + 1);
      *position = x;
      ++finish;
    } else {
      size_type len = size() ? 2 * size() : __WORD_BIT;
      unsigned int* q = bit_alloc(len);
      iterator i = copy(begin(), position, make_iterator(q, 0, true));
      *i++ = x;
      finish = copy(position, end(), i);
      deallocate();
      end_of_storage = q + (len + __WORD_BIT - 1)/__WORD_BIT;
      start = make_iterator(q, 0);
    }
    __stl_debug_do(finish.set_overrun(false));
  }

#ifdef __STL_MEMBER_TEMPLATES
  template <class InputIterator>
  void initialize_range(InputIterator first, InputIterator last,
			input_iterator_tag) {
    __stl_debug_check(__check_range(first,last));
    __stl_debug_do(safe_init(this));
    start = iterator();
    finish = iterator();
    __stl_debug_do(init_bounds());
    end_of_storage = 0;
# if defined (__STL_DEBUG) && defined (__STL_USE_EXCEPTIONS)
    try {
# endif
      for ( ; first != last; ++first) 
        push_back(*first);
# if defined (__STL_DEBUG) && defined (__STL_USE_EXCEPTIONS)
    }
    catch (...) {
      __stl_debug_do(start.invalidate());
      __stl_debug_do(finish.invalidate());
      __stl_debug_do(invalidate());        
    }
# endif
  }

  template <class ForwardIterator>
  void initialize_range(ForwardIterator first, ForwardIterator last,
			forward_iterator_tag) {
    __stl_debug_check(__check_range(first,last));
    size_type n = 0;
    distance(first, last, n);
    initialize(n);
    copy(first, last, start);
  }

  template <class InputIterator>
  void insert_range(iterator pos,
		    InputIterator first, InputIterator last,
		    input_iterator_tag) {
    for ( ; first != last; ++first) {
      pos = insert(pos, *first);
      ++pos;
    }
  }

  template <class ForwardIterator>
  void insert_range(iterator position,
		    ForwardIterator first, ForwardIterator last,
		    forward_iterator_tag) {
    __stl_debug_check(__check_range(first,last));
    if (first != last) {
      size_type n = 0;
      distance(first, last, n);
      __stl_debug_do(finish.set_overrun(true));
      __stl_debug_do(position.set_overrun(true));
      if (capacity() - size() >= n) {
	copy_backward(position, end(), finish + n);
	copy(first, last, position);
	finish += n;
      }
      else {
	size_type len = size() + max(size(), n);
	unsigned int* q = bit_alloc(len);
	iterator i = copy(begin(), position, make_iterator(q, 0, true));
	i = copy(first, last, i);
	finish = copy(position, end(), i);
	deallocate();
	end_of_storage = q + (len + __WORD_BIT - 1)/__WORD_BIT;
	start = make_iterator(q, 0);
      }
    }
    __stl_debug_do(finish.set_overrun(false));
  }      

#endif /* __STL_MEMBER_TEMPLATES */

# ifdef __SGI_STL_VECBOOL_TEMPLATE
  typedef vector<bool> self;
# else
  typedef bit_vector self;
# endif
public:
  iterator begin() { return start; }
  const_iterator begin() const { return start; }
  iterator end() { return finish; }
  const_iterator end() const { return finish; }

  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const { 
    return const_reverse_iterator(end()); 
  }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const { 
    return const_reverse_iterator(begin()); 
  }

  size_type size() const { return size_type(end() - begin()); }
  size_type max_size() const { return size_type(-1); }
  size_type capacity() const {
    return size_type(make_const_iterator(end_of_storage, 0) - begin());
  }
  bool empty() const { return begin() == end(); }
  reference operator[](size_type n) { return *(begin() + difference_type(n)); }
  const_reference operator[](size_type n) const { return *(begin() + difference_type(n)); }
  __BVECTOR() : start(iterator()), finish(iterator()), end_of_storage(0) {
    __stl_debug_do(safe_init(this));        
    __stl_debug_do(init_bounds());
  }
  __BVECTOR(size_type n, bool value) {
    initialize(n);
    fill(start.p, end_of_storage, value ? ~0 : 0);
  }
  __BVECTOR(int n, bool value) {
    initialize(n);
    fill(start.p, end_of_storage, value ? ~0 : 0);
  }
  __BVECTOR(long n, bool value) {
    initialize(n);
    fill(start.p, end_of_storage, value ? ~0 : 0);
  }
  explicit __BVECTOR(size_type n) {
    initialize(n);
    fill(start.p, end_of_storage, 0);
  }
  __BVECTOR(const self& x) {
    initialize(x.size());
    copy(x.begin(), x.end(), start);
  }

#ifdef __STL_MEMBER_TEMPLATES
  template <class InputIterator>
  __BVECTOR(InputIterator first, InputIterator last) {
    initialize_range(first, last, iterator_category(first));
  }
#else /* __STL_MEMBER_TEMPLATES */
  __BVECTOR(const_iterator first, const_iterator last) {
    size_type n = 0;
    distance(first, last, n);
    initialize(n);
    copy(first, last, start);
  }
  __BVECTOR(const bool* first, const bool* last) {
    size_type n = 0;
    distance(first, last, n);
    initialize(n);
    copy(first, last, start);
  }
#endif /* __STL_MEMBER_TEMPLATES */

  ~__BVECTOR() { 
    deallocate(); 
    __stl_debug_do(invalidate());
    __stl_debug_do(start.invalidate()); 
    __stl_debug_do(finish.invalidate());
  }
  self& operator=(const self& x) {
    if (&x == this) return *this;
    __stl_debug_do(invalidate_all());
    if (x.size() > capacity()) {
      deallocate();
      initialize(x.size());
    }
    copy(x.begin(), x.end(), begin());
    finish = begin() + difference_type(x.size());
    return *this;
  }
  void reserve(size_type n) {
    if (capacity() < n) {
      unsigned int* q = bit_alloc(n);
      finish = copy(begin(), end(), make_iterator(q, 0, true));
      __stl_debug_do(finish.set_overrun(false));
      deallocate();
      start = make_iterator(q, 0);
      end_of_storage = q + (n + __WORD_BIT - 1)/__WORD_BIT;
    }
  }
  reference front() { return *begin(); }
  const_reference front() const { return *begin(); }
  reference back() { return *(end() - 1); }
  const_reference back() const { return *(end() - 1); }
  void push_back(bool x) {
    if (finish.p != end_of_storage) {
      __stl_debug_do(finish.set_overrun(true));
      *finish++ = x;
      __stl_debug_do(finish.set_overrun(false));
    }
    else
      insert_aux(end(), x);
  }
  void swap(self& x) {
    __STD::swap(start, x.start);
    __STD::swap(finish, x.finish);
    __STD::swap(end_of_storage, x.end_of_storage);
    __stl_debug_do(swap_owners(x));
  }
  iterator insert(iterator position, bool x = bool()) {
    __stl_debug_check(__check_if_owner(this,position)
		      &&__check_range(position,start,finish));
    size_type n = position - begin();
    if (finish.p != end_of_storage && position == end()) {
      __stl_debug_do(finish.set_overrun(true));
      *finish++ = x;
      __stl_debug_do(finish.set_overrun(false));
    }
    else
      insert_aux(position, x);
    return begin() + n;
  }

#ifdef __STL_MEMBER_TEMPLATES
  template <class InputIterator> void insert(iterator position,
					     InputIterator first,
					     InputIterator last) {
    insert_range(position, first, last, iterator_category(first));
  }
#else /* __STL_MEMBER_TEMPLATES */
  void insert(iterator position, const_iterator first, 
	      const_iterator last) {
    __stl_debug_check(__check_if_owner(this,position)
		      &&__check_range(position,start,finish));
    if (first == last) return;
    __stl_debug_check(__check_range(first,last));
    size_type n = 0;
    distance(first, last, n);
    __stl_debug_do(finish.set_overrun(true));
    __stl_debug_do(position.set_overrun(true));
    if (capacity() - size() >= n) {
      copy_backward(position, end(), finish + n);
      copy(first, last, position);
      finish += n;
    } else {
      size_type len = size() + max(size(), n);
      unsigned int* q = bit_alloc(len);
      iterator i = copy(begin(), position, make_iterator(q, 0, true));
      i = copy(first, last, i);
      finish = copy(position, end(), i);
      deallocate();
      end_of_storage = q + (len + __WORD_BIT - 1)/__WORD_BIT;
      start = make_iterator(q, 0);
    }
    __stl_debug_do(finish.set_overrun(false));
  }

  void insert(iterator position, const bool* first, const bool* last) {
    __stl_debug_check(__check_if_owner(this,position)
		      &&__check_range(position,start,finish));
    if (first == last) return;
    __stl_debug_check(__check_range(first,last));
    size_type n = 0;
    distance(first, last, n);
    __stl_debug_do(finish.set_overrun(true));
    __stl_debug_do(position.set_overrun(true));
    if (capacity() - size() >= n) {
      copy_backward(position, end(), finish + n);
      copy(first, last, position);
      finish += n;
    } else {
      size_type len = size() + max(size(), n);
      unsigned int* q = bit_alloc(len);
      iterator i = copy(begin(), position, make_iterator(q, 0, true));
      i = copy(first, last, i);
      finish = copy(position, end(), i);
      deallocate();
      end_of_storage = q + (len + __WORD_BIT - 1)/__WORD_BIT;
      start = make_iterator(q, 0);
    }
    __stl_debug_do(finish.set_overrun(false));
  }
#endif /* __STL_MEMBER_TEMPLATES */
  
  void insert(iterator position, size_type n, bool x) {
    __stl_debug_check(__check_if_owner(this,position)
		      &&__check_range(position,start,finish));
    if (n == 0) return;
    __stl_debug_do(finish.set_overrun(true));
    __stl_debug_do(position.set_overrun(true));
    if (capacity() - size() >= n) {
      copy_backward(position, end(), finish + difference_type(n));
      fill(position, position + difference_type(n), x);
      finish += difference_type(n);
    } else {
      size_type len = size() + max(size(), n);
      unsigned int* q = bit_alloc(len);
      iterator i = copy(begin(), position, make_iterator(q, 0, true));
      fill_n(i, n, x);
      finish = copy(position, end(), i + difference_type(n));
      deallocate();
      end_of_storage = q + (len + __WORD_BIT - 1)/__WORD_BIT;
      start = make_iterator(q, 0);
    }
    __stl_debug_do(finish.set_overrun(false));
  }

  void insert(iterator pos, int n, bool x)  { insert(pos, (size_type)n, x); }
  void insert(iterator pos, long n, bool x) { insert(pos, (size_type)n, x); }

  void pop_back() { 
    __stl_verbose_assert(!empty(), __STL_MSG_EMPTY_CONTAINER);
    --finish; 
  }
  iterator erase(iterator position) {
    __stl_debug_check(__check_if_owner(this,position));
    __stl_verbose_assert(position.dereferenceable(),__STL_MSG_NOT_DEREFERENCEABLE);
    if (position + 1 != end())
      copy(position + 1, end(), position);
    --finish;
    return position;
  }
  iterator erase(iterator first, iterator last) {
    __stl_debug_check(__check_if_owner(this,first)
		      &&__check_range(first,last,start,finish));
    finish = copy(last, end(), first);
    return first;
  }
  void resize(size_type new_size, bool x = bool()) {
    if (new_size < size()) 
      erase(begin() + new_size, end());
    else
      insert(end(), new_size - size(), x);
  }
  void clear() { erase(begin(), end()); }
# if defined (__STL_DEBUG)
  void init_bounds() {
    // make sure start & finish remained valid
    start.owner_=(__safe_server*)this;
    start.next_=start.prev_=&start;
    finish.owner_=(__safe_server*)this;
    finish.next_=finish.prev_=&finish;
  }
  void invalidate_iterator(const iterator& it) { 
    __invalidate_iterator(this,it,it);
    init_bounds();
  }
  void invalidate_all() {
    __safe_server::invalidate_all();
    init_bounds();
  }
# endif
};

#ifdef __SGI_STL_VECBOOL_TEMPLATE

typedef vector<bool, alloc> bit_vector;

#else /* __SGI_STL_VECBOOL_TEMPLATE */

inline bool operator==(const bit_vector& x, const bit_vector& y) {
  return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

inline bool operator<(const bit_vector& x, const bit_vector& y) {
  return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

inline void swap(bit_vector& a, bit_vector& b) { a.swap(b); }

#endif /* __SGI_STL_VECBOOL_TEMPLATE */

# if defined (__STL_DEBUG)

inline bool __bit_iterator::dereferenceable() const {
  bit_vector* vec = (bit_vector*)owner();
  __stl_verbose_return(valid(), __STL_MSG_INVALID_ITERATOR);
  if (overrun_ok()) return true;
  __stl_verbose_return(*this >= vec->begin() && *this < vec->end(),
		       __STL_MSG_NOT_DEREFERENCEABLE);    
  return true;
}

inline bool __bit_const_iterator::dereferenceable() const {
  const bit_vector* vec = (const bit_vector*)owner();
  __stl_verbose_return(valid(), __STL_MSG_INVALID_ITERATOR);
  if (overrun_ok()) return true;
  __stl_verbose_return(*this >= vec->begin() && *this < vec->end(),
		       __STL_MSG_NOT_DEREFERENCEABLE);    
  return true;
}
# endif

#undef __SGI_STL_VECBOOL_TEMPLATE
#undef __BVECTOR

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#endif

# undef __WORD_BIT

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_BVECTOR_H */

// Local Variables:
// mode:C++
// End:
