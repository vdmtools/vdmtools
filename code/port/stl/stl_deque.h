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
 * Copyright (c) 1997
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

#ifndef __SGI_STL_INTERNAL_DEQUE_H
#define __SGI_STL_INTERNAL_DEQUE_H

/* Class invariants:
 *  For any nonsingular iterator i:
 *    i.node is the address of an element in the map array.  The
 *      contents of i.node is a pointer to the beginning of a node.
 *    i.first == *(i.node) 
 *    i.last  == i.first + node_size
 *    i.cur is a pointer in the range [i.first, i.last).  NOTE:
 *      the implication of this is that i.cur is always a dereferenceable
 *      pointer, even if i is a past-the-end iterator.
 *  Start and Finish are always nonsingular iterators.  NOTE: this means
 *    that an empty deque must have one node, and that a deque
 *    with N elements, where N is the buffer size, must have two nodes.
 *  For every node other than start.node and finish.node, every element
 *    in the node is an initialized object.  If start.node == finish.node,
 *    then [start.cur, finish.cur) are initialized objects, and
 *    the elements outside that range are uninitialized storage.  Otherwise,
 *    [start.cur, start.last) and [finish.first, finish.cur) are initialized
 *    objects, and [start.first, start.cur) and [finish.cur, finish.last)
 *    are uninitialized storage.
 *  [map, map + map_size) is a valid, non-empty range.  
 *  [start.node, finish.node] is a valid range contained within 
 *    [map, map + map_size).  
 *  A pointer in the range [map, map + map_size) points to an allocated
 *    node if and only if the pointer is in the range [start.node, finish.node].
 */


/*
 * In previous versions of deque, node_size was fixed by the 
 * implementation.  In this version, however, users can select
 * the node size.  Deque has three template parameters; the third,
 * a number of type size_t, is the number of elements per node.
 * If the third template parameter is 0 (which is the default), 
 * then deque will use a default node size.
 *
 * The only reason for using an alternate node size is if your application
 * requires a different performance tradeoff than the default.  If,
 * for example, your program contains many deques each of which contains
 * only a few elements, then you might want to save memory (possibly
 * by sacrificing some speed) by using smaller nodes.
 *
 * Unfortunately, some compilers have trouble with non-type template 
 * parameters; stl_config.h defines __STL_NON_TYPE_TMPL_PARAM_BUG if
 * that is the case.  If your compiler is one of them, then you will
 * not be able to use alternate node sizes; you will have to use the
 * default value.
 */

# ifndef __SGI_STL_ALGOBASE_H
#  include <algobase.h>
# endif
# ifndef __SGI_STL_ALLOC_H
#  include <alloc.h>
# endif

# undef __deque__
# undef deque
# if defined ( __STL_NO_DEFAULT_NON_TYPE_PARAM )
#  define deque __deque
#  define __deque__ __deque
# else
#  define __deque__ __FULL_NAME(deque)
#  define deque __WORKAROUND_RENAME(deque)
# endif

__STL_BEGIN_NAMESPACE

# if defined ( __STL_USE_ABBREVS )
#  define __deque_iterator         _dQ__It
# endif

// Note: this function is simply a kludge to work around several compilers'
//  bugs in handling constant expressions.
inline size_t __deque_buf_size(size_t n, size_t sz)
{
  return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
}

// this helper class is needed to pass deque not-type parameter
// to its iterators. Probably it may use enum instead - this definition
// just may be more compiler-friendly. 
template <size_t BufSize>
struct __deque_traits {
    enum { buf_size = BufSize } ;
    static size_t buffer_size() { return BufSize; } 
};

template <class T, class Ref, class Ptr, class BufSize>
# if defined ( __STL_DEBUG )
struct __deque_iterator : public __safe_base {
# else
struct __deque_iterator {
# endif
  typedef __deque_iterator<T, T&, T*, BufSize>             iterator;
  typedef __deque_iterator<T, const T&, const T*, BufSize> const_iterator;
  static size_t buffer_size() { 
      return __deque_buf_size(BufSize::buffer_size(), sizeof(T)); 
  }

  typedef random_access_iterator_tag iterator_category;
  typedef T value_type;
  typedef Ptr pointer;
  typedef Ref reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T** map_pointer;

  typedef __deque_iterator<T, Ref, Ptr, BufSize> self;

  T* cur;
  T* first;
  T* last;
  map_pointer node;

# if defined ( __STL_DEBUG )
  bool unsafe;
  bool overrun_ok() const { return unsafe; } 
  void set_overrun(bool val) { unsafe = val; } 
  typedef const void* base_ptr; 
  const self& get_iterator() const { return *this; }  
  base_ptr owner() const {
      return base_ptr(__safe_base::owner());
  }
  bool dereferenceable() const;
  bool nonsingular(const __safe_server* o = 0) const;
  __deque_iterator(const __safe_server* root, T* x, map_pointer y) 
    : __safe_base(root),cur(x), first(*y), last(*y + buffer_size()), 
      node(y), unsafe(false) {}
  __deque_iterator() : __safe_base(0), cur(0), first(0), last(0), 
          node(0), unsafe(false) {}
  __deque_iterator(const iterator& x)
    : __safe_base((const __safe_base*)x.owner_),
      cur(x.cur), first(x.first), last(x.last), node(x.node), unsafe(x.unsafe) {}
# else
  __deque_iterator(T* x, map_pointer y) 
    : cur(x), first(*y), last(*y + buffer_size()), node(y) {}
  __deque_iterator() : cur(0), first(0), last(0), node(0) {}
  __deque_iterator(const iterator& x)
    : cur(x.cur), first(x.first), last(x.last), node(x.node) {}
# endif
  reference operator*() const { 
      __stl_debug_check(dereferenceable());    
      return *cur; 
  }
#ifndef __SGI_STL_NO_ARROW_OPERATOR
  pointer operator->() const { return &(operator*()); }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */

  difference_type operator-(const self& x) const {
    __stl_debug_check(__check_same_owner(*this,x));    
    return difference_type(buffer_size()) * (node - x.node - 1) +
      (cur - first) + (x.last - x.cur);
  }

  self& operator++() {
    ++cur;
    if (cur == last) {
      set_node(node + 1);
      cur = first;
    }
    return *this; 
  }
  self operator++(int)  {
    self tmp = *this;
    ++*this;
    return tmp;
  }

  self& operator--() {
    if (cur == first) {
      set_node(node - 1);
      cur = last;
    }
    --cur;
    return *this;
  }
  self operator--(int) {
    self tmp = *this;
    --*this;
    return tmp;
  }

  self& operator+=(difference_type n) {
    difference_type offset = n + (cur - first);
    if (offset >= 0 && offset < difference_type(buffer_size()))
      cur += n;
    else {
      difference_type node_offset =
        offset > 0 ? offset / difference_type(buffer_size())
                   : -difference_type((-offset - 1) / buffer_size()) - 1;
      set_node(node + node_offset);
      cur = first + (offset - node_offset * difference_type(buffer_size()));
    }
    return *this;
  }

  self operator+(difference_type n) const {
    self tmp = *this;
    return tmp += n;
  }

  self& operator-=(difference_type n) { return *this += -n; }
 
  self operator-(difference_type n) const {
    self tmp = *this;
    return tmp -= n;
  }

  reference operator[](difference_type n) const { return *(*this + n); }

  bool operator==(const self& x) const { 
      __stl_debug_check(__check_same_owner(*this,x));    
      return cur == x.cur; 
  }
  bool operator!=(const self& x) const { return !(*this == x); }
  bool operator<(const self& x) const {
      __stl_debug_check(__check_same_owner(*this,x));    
    return (node == x.node) ? (cur < x.cur) : (node < x.node);
  }

  void set_node(map_pointer new_node) {
    node = new_node;
    first = *new_node;
    last = first + difference_type(buffer_size());
  }
};

#ifndef __STL_CLASS_PARTIAL_SPECIALIZATION

template <class T, class Ref, class Ptr, class BufSize>
inline random_access_iterator_tag
iterator_category(const __deque_iterator<T, Ref, Ptr, BufSize>&) {
  return random_access_iterator_tag();
}

template <class T, class Ref, class Ptr, class BufSize>
inline T* value_type(const __deque_iterator<T, Ref, Ptr, BufSize>&) { return 0; }

template <class T, class Ref, class Ptr, class BufSize>
inline ptrdiff_t* distance_type(const __deque_iterator<T, Ref, Ptr, BufSize>&) {
  return 0;
}

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

// See __deque_buf_size().  The only reason that the default value is 0
//  is as a workaround for bugs in the way that some compilers handle
//  constant expressions.
# if defined ( __STL_NO_DEFAULT_NON_TYPE_PARAM )
template <class T, class Alloc, size_t BufSize> 
# else
template <class T, __DFL_TYPE_PARAM(Alloc,alloc), __DFL_NON_TYPE_PARAM(size_t, BufSize, 0)>
# endif 
class deque 
# if defined (__STL_DEBUG)
    : public __safe_server
# endif
{
  typedef deque<T, Alloc, BufSize> self;
public:                         // Basic types
  typedef T value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

public:                         // Iterators
  typedef __deque_traits<BufSize> deque_traits;
  typedef __deque_iterator<T, T&, T*, deque_traits> iterator;
  typedef __deque_iterator<T, const T&, const T*, deque_traits> const_iterator;

#if defined ( __STL_CLASS_PARTIAL_SPECIALIZATION ) && \
! defined (__STL_PARTIAL_SPECIALIZATION_BUG)
  typedef reverse_iterator<const_iterator> const_reverse_iterator;
  typedef reverse_iterator<iterator> reverse_iterator;
#else /* __STL_CLASS_PARTIAL_SPECIALIZATION */
# if defined (__STL_MSVC50_COMPATIBILITY)
    typedef reverse_iterator<const_iterator, value_type, const_reference, 
    const value_type*, difference_type>  const_reverse_iterator;
    typedef reverse_iterator<iterator, value_type, reference, pointer, 
        difference_type> reverse_iterator; 
# else
  typedef reverse_iterator<const_iterator, value_type, const_reference, 
                           difference_type>  
          const_reverse_iterator;
  typedef reverse_iterator<iterator, value_type, reference, difference_type>
          reverse_iterator; 
# endif /* __STL_MSVC50_COMPATIBILITY */
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

protected:                      // Internal typedefs
  typedef pointer* map_pointer;
  typedef simple_alloc<value_type, Alloc> data_allocator;
  typedef simple_alloc<pointer, Alloc> map_allocator;

  static size_type buffer_size() {
    return __deque_buf_size(BufSize, sizeof(value_type));
  }
  static size_type initial_map_size() { return 8; }

protected:                      // Data members
  iterator start;
  iterator finish;

  map_pointer map;
  size_type map_size;

public:                         // Basic accessors
  iterator begin() { return start; }
  iterator end() { return finish; }
  const_iterator begin() const { return start; }
  const_iterator end() const { return finish; }

  reverse_iterator rbegin() { return reverse_iterator(finish); }
  reverse_iterator rend() { return reverse_iterator(start); }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(finish);
  }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(start);
  }

  reference operator[](size_type n) { return start[difference_type(n)]; }
  const_reference operator[](size_type n) const {
    return start[difference_type(n)];
  }

  reference front() { return *start; }
  reference back() {
    iterator tmp = finish;
    --tmp;
    return *tmp;
  }
  const_reference front() const { return *start; }
  const_reference back() const {
    const_iterator tmp = finish;
    --tmp;
    return *tmp;
  }

  size_type size() const { return finish - start;; }
  size_type max_size() const { return size_type(-1); }
  bool empty() const { return finish == start; }

public:                         // Constructor, destructor.
  deque()
    : start(), finish(), map(0), map_size(0)
  {
    create_map_and_nodes(0);
  }

  deque(const self& x)
    : start(), finish(), map(0), map_size(0)
  {
    create_map_and_nodes(x.size());
    __STL_TRY {
      uninitialized_copy(x.begin(), x.end(), start);
    }
    __STL_UNWIND(destroy_map_and_nodes());
  }

  deque(size_type n, const value_type& value)
    : start(), finish(), map(0), map_size(0)
  {
    fill_initialize(n, value);
  }

  deque(int n, const value_type& value)
    : start(), finish(), map(0), map_size(0)
  {
    fill_initialize(n, value);
  }
 
  deque(long n, const value_type& value)
    : start(), finish(), map(0), map_size(0)
  {
    fill_initialize(n, value);
  }

  explicit deque(size_type n)
    : start(), finish(), map(0), map_size(0)
  {
    fill_initialize(n, value_type());
  }

#ifdef __STL_MEMBER_TEMPLATES

  template <class InputIterator>
  deque(InputIterator first, InputIterator last)
    : start(), finish(), map(0), map_size(0)
  {
    range_initialize(first, last, iterator_category(first));
  }

#else /* __STL_MEMBER_TEMPLATES */

  deque(const value_type* first, const value_type* last)
    : start(), finish(), map(0), map_size(0)
  {
    create_map_and_nodes(last - first);
    __STL_TRY {
      uninitialized_copy(first, last, start);
    }
    __STL_UNWIND(destroy_map_and_nodes());
  }

  deque(const_iterator first, const_iterator last)
    : start(), finish(), map(0), map_size(0)
  {
    create_map_and_nodes(last - first);
    __STL_TRY {
      uninitialized_copy(first, last, start);
    }
    __STL_UNWIND(destroy_map_and_nodes());
  }

#endif /* __STL_MEMBER_TEMPLATES */

  ~deque() {
    destroy(start, finish);
    destroy_map_and_nodes();
  }

  self& operator= (const self& x) {
    const size_type len = size();
    if (&x != this) {
      if (len >= x.size())
        erase(copy(x.begin(), x.end(), start), finish);
      else {
        const_iterator mid = x.begin() + difference_type(len);
        copy(x.begin(), mid, start);
        insert(finish, mid, x.end());
      }
    }
    __stl_debug_do(invalidate_all());
    return *this;
  }        

  void swap(self& x) {
    __STD::swap(start, x.start);
    __STD::swap(finish, x.finish);
    __STD::swap(map, x.map);
    __STD::swap(map_size, x.map_size);
    __stl_debug_do(swap_owners(x));
  }

public:                         // push_* and pop_*
  
  void push_back(const value_type& t) {
    if (finish.cur != finish.last - 1) {
      construct(finish.cur, t);
      ++finish.cur;
    }
    else
      push_back_aux(t);
    __stl_debug_do(invalidate_all());
  }

  void push_front(const value_type& t) {
    if (start.cur != start.first) {
      construct(start.cur - 1, t);
      --start.cur;
    }
    else
      push_front_aux(t);
    __stl_debug_do(invalidate_all());
  }

  void pop_back() {
    __stl_debug_do(invalidate_iterator(finish));
    if (finish.cur != finish.first) {
      --finish.cur;
      destroy(finish.cur);
    }
    else
      pop_back_aux();
  }

  void pop_front() {
    __stl_debug_do(invalidate_iterator(start));        
    if (start.cur != start.last - 1) {
      destroy(start.cur);
      ++start.cur;
    }
    else 
      pop_front_aux();
  }

public:                         // Insert

  iterator insert(iterator position, const value_type& x) {
    __stl_debug_check(__check_range(position,start,finish));    
    if (position.cur == start.cur) {
      push_front(x);
      return start;
    }
    else if (position.cur == finish.cur) {
      push_back(x);
      iterator tmp = finish;
      --tmp;
      return tmp;
    }
    else {
      return insert_aux(position, x);
    }
  }

  iterator insert(iterator position) { return insert(position, value_type()); }

  void insert(iterator pos, size_type n, const value_type& x); 

  void insert(iterator pos, int n, const value_type& x) {
    insert(pos, (size_type) n, x);
  }
  void insert(iterator pos, long n, const value_type& x) {
    insert(pos, (size_type) n, x);
  }

#ifdef __STL_MEMBER_TEMPLATES  

  template <class InputIterator>
  void insert(iterator pos, InputIterator first, InputIterator last) {
    insert(pos, first, last, iterator_category(first));
  }

#else /* __STL_MEMBER_TEMPLATES */

  void insert(iterator pos, const value_type* first, const value_type* last);
  void insert(iterator pos, const_iterator first, const_iterator last);

#endif /* __STL_MEMBER_TEMPLATES */

  void resize(size_type new_size, const value_type& x) {
    const size_type len = size();
    if (new_size < len) 
      erase(start + new_size, finish);
    else
      insert(finish, new_size - len, x);
  }

  void resize(size_type new_size) { resize(new_size, value_type()); }

public:                         // Erase
  iterator erase(iterator pos) {
    __stl_debug_check(pos.nonsingular(this));
    iterator next = pos;
    ++next;
    difference_type index = pos - start;
    if (index < (size() >> 1)) {
      copy_backward(start, pos, next);
      pop_front();
    }
    else {
      copy(next, finish, pos);
      pop_back();
    }
    return start + index;
  }

  iterator erase(iterator first, iterator last);
  void clear(); 

protected:                        // Internal construction/destruction

  void create_map_and_nodes(size_type num_elements);
  void destroy_map_and_nodes();
  void fill_initialize(size_type n, const value_type& value);

#ifdef __STL_MEMBER_TEMPLATES  

  template <class InputIterator>
  void range_initialize(InputIterator first, InputIterator last,
                        input_iterator_tag);

  template <class ForwardIterator>
  void range_initialize(ForwardIterator first, ForwardIterator last,
                        forward_iterator_tag);

#endif /* __STL_MEMBER_TEMPLATES */

protected:                        // Internal push_* and pop_*

  void push_back_aux(const value_type& t);
  void push_front_aux(const value_type& t);
  void pop_back_aux();
  void pop_front_aux();

protected:                        // Internal insert functions

#ifdef __STL_MEMBER_TEMPLATES  

  template <class InputIterator>
  void insert(iterator pos, InputIterator first, InputIterator last,
              input_iterator_tag);

  template <class ForwardIterator>
  void insert(iterator pos, ForwardIterator first, ForwardIterator last,
              forward_iterator_tag);

#endif /* __STL_MEMBER_TEMPLATES */

  iterator insert_aux(iterator pos, const value_type& x);
  void insert_aux(iterator pos, size_type n, const value_type& x);

#ifdef __STL_MEMBER_TEMPLATES  

  template <class ForwardIterator>
  void insert_aux(iterator pos, ForwardIterator first, ForwardIterator last,
                  size_type n);

#else /* __STL_MEMBER_TEMPLATES */
  
  void insert_aux(iterator pos,
                  const value_type* first, const value_type* last,
                  size_type n);

  void insert_aux(iterator pos, const_iterator first, const_iterator last,
                  size_type n);
 
#endif /* __STL_MEMBER_TEMPLATES */

  iterator reserve_elements_at_front(size_type n) {
    size_type vacancies = start.cur - start.first;
    if (n > vacancies) 
      new_elements_at_front(n - vacancies);
    return start - difference_type(n);
  }

  iterator reserve_elements_at_back(size_type n) {
    size_type vacancies = (finish.last - finish.cur) - 1;
    if (n > vacancies)
      new_elements_at_back(n - vacancies);
    return finish + difference_type(n);
  }

  void new_elements_at_front(size_type new_elements);
  void new_elements_at_back(size_type new_elements);

  void destroy_nodes_at_front(iterator before_start);
  void destroy_nodes_at_back(iterator after_finish);

protected:                      // Allocation of map and nodes

  // Makes sure the map has space for new nodes.  Does not actually
  //  add the nodes.  Can invalidate map pointers.  (And consequently, 
  //  deque iterators.)

  void reserve_map_at_back (size_type nodes_to_add = 1) {
    if (difference_type(nodes_to_add + 1) > map_size - (finish.node - map))
        reallocate_map(nodes_to_add, false);
//    __stl_assert(finish.node + nodes_to_add < map + map_size);
  }

  void reserve_map_at_front (size_type nodes_to_add = 1) {
    if (difference_type(nodes_to_add) > start.node - map)
        reallocate_map(nodes_to_add, true);
//    __stl_assert(start.node - nodes_to_add >= map);
  }

  void reallocate_map(size_type nodes_to_add, bool add_at_front);

  pointer allocate_node() { return data_allocator::allocate(buffer_size()); }
  void deallocate_node(pointer n) {
    data_allocator::deallocate(n, buffer_size());
  }

#ifdef __STL_NON_TYPE_TMPL_PARAM_BUG
public:
  bool operator==(const deque<T, Alloc, BufSize>& x) const {
    return size() == x.size() && equal(begin(), end(), x.begin());
  }
  bool operator!=(const deque<T, Alloc, BufSize>& x) const {
    return size() != x.size() || !equal(begin(), end(), x.begin());
  }
  bool operator<(const deque<T, Alloc, BufSize>& x) const {
    return lexicographical_compare(begin(), end(), x.begin(), x.end());
  }
#endif /* __STL_NON_TYPE_TMPL_PARAM_BUG */

# if defined (__STL_DEBUG)
public:
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

# if defined (__STL_DEBUG)
template <class T, class BufSize>
struct __deque_rep : public __safe_server {
    __deque_iterator<T,T&, T*, BufSize> start;
    __deque_iterator<T,T&, T*, BufSize> finish;
    T** map;
    size_t map_size;
};

template <class T, class Ref, class Ptr, class BufSize>
#ifdef __STL_INLINE_NAME_RESOLUTION_BUG
inline
#endif
bool __deque_iterator<T, Ref, Ptr, BufSize>::dereferenceable() const
{
    typedef __deque_rep<T, BufSize> deque_rep;
    const iterator& finish = (((const deque_rep*)owner())->finish);
    const iterator& start = (((const deque_rep*)owner())->start);
    __stl_verbose_return(valid(), __STL_MSG_INVALID_ITERATOR);

    if (overrun_ok()) return true;

    __stl_verbose_return(((node == finish.node) ? 
                          (cur < finish.cur) : (node < finish.node)) // *this < finish
                         && ((node == start.node) ? 
                             (cur >= start.cur) : (node >= start.node)),  // *this >= start 
                         __STL_MSG_NOT_DEREFERENCEABLE);    
    return true;
}

template <class T, class Ref, class Ptr, class BufSize>
bool __deque_iterator<T, Ref, Ptr, BufSize>::nonsingular(const __safe_server* o ) const {
    typedef __deque_rep<T, BufSize> deque_rep;
    const iterator& finish = (((const deque_rep*)owner())->finish);
    const iterator& start = (((const deque_rep*)owner())->start);
    if (o) {
        __stl_verbose_return(owner()==o, __STL_MSG_NOT_OWNER);
    } else {
        __stl_verbose_return(valid(), __STL_MSG_INVALID_ITERATOR);
    }

    if (overrun_ok()) return true;

    __stl_verbose_return(((node == finish.node) ? 
                          (cur <= finish.cur) : (node < finish.node)) // *this <= finish
                         && ((node == start.node) ? 
                             (cur >= start.cur) : (node >= start.node)),  // *this >= start 
                         __STL_MSG_SINGULAR_ITERATOR);    
    return true;
}
# endif /* __STL_DEBUG */

# if defined ( __STL_NESTED_TYPE_PARAM_BUG )
// qualified references 
#   define __iterator__           __deque_iterator<T, T&, T*, __deque_traits<BufSize> >
#   define const_iterator         __deque_iterator<T, const T&, const T*,  __deque_traits<BufSize> > 
#   define iterator               __iterator__
#   define size_type              size_t
#   define value_type             T
# else
#  define __iterator__           deque<T, Alloc, BufSize>::iterator
# endif

// Non-inline member functions

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert(iterator pos,
                                      size_type n, const value_type& x) {
  __stl_debug_check(pos.nonsingular(this));
  if (pos.cur == start.cur) {
    iterator new_start = reserve_elements_at_front(n);
    __stl_debug_do(new_start.set_overrun(true));
#     ifdef __STL_USE_EXCEPTIONS
    try {
#     endif /* __STL_USE_EXCEPTIONS */
        uninitialized_fill(new_start, start, x);
#     ifdef __STL_USE_EXCEPTIONS
    }
    catch(...) {
        destroy_nodes_at_front(new_start);      
        throw;
    }
#     endif /* __STL_USE_EXCEPTIONS */
    start = new_start;
    __stl_debug_do(start.set_overrun(false));
  }
  else if (pos.cur == finish.cur) {
    iterator new_finish = reserve_elements_at_back(n);
    __stl_debug_do(finish.set_overrun(true));
#     ifdef __STL_USE_EXCEPTIONS
    try {
#     endif /* __STL_USE_EXCEPTIONS */
        uninitialized_fill(finish, new_finish, x);
#     ifdef __STL_USE_EXCEPTIONS
    }
    catch(...) {
        destroy_nodes_at_back(new_finish);      
        __stl_debug_do(finish.set_overrun(false));
        throw;
    }
#     endif /* __STL_USE_EXCEPTIONS */
    finish = new_finish;
  }
  else 
    insert_aux(pos, n, x);
}

#ifndef __STL_MEMBER_TEMPLATES  

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert(iterator pos,
                                      const value_type* first,
                                      const value_type* last) {
  __stl_debug_check(pos.nonsingular(this));
  size_type n = last - first;
  if (pos.cur == start.cur) {
    iterator new_start = reserve_elements_at_front(n);
    __stl_debug_do(new_start.set_overrun(true));
#     ifdef __STL_USE_EXCEPTIONS
    try {
#     endif /* __STL_USE_EXCEPTIONS */
        uninitialized_copy(first, last, new_start);
#     ifdef __STL_USE_EXCEPTIONS
    }
    catch(...) {
        destroy_nodes_at_front(new_start);      
        throw;
    }
#     endif /* __STL_USE_EXCEPTIONS */
    start = new_start;
    __stl_debug_do(start.set_overrun(false));
  }
  else if (pos.cur == finish.cur) {
      iterator new_finish = reserve_elements_at_back(n);
      __stl_debug_do(finish.set_overrun(true));
#     ifdef __STL_USE_EXCEPTIONS
      try {
#     endif /* __STL_USE_EXCEPTIONS */
          uninitialized_copy(first, last, finish);
#     ifdef __STL_USE_EXCEPTIONS
      }
      catch(...) {
          destroy_nodes_at_back(new_finish);      
          __stl_debug_do(finish.set_overrun(false));
          throw;
      }
#     endif /* __STL_USE_EXCEPTIONS */
    finish = new_finish;
    __stl_debug_do(finish.set_overrun(false));
  }
  else
    insert_aux(pos, first, last, n);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert(iterator pos,
                                      const_iterator first,
                                      const_iterator last)
{
  __stl_debug_check(pos.nonsingular(this));
  size_type n = last - first;
  if (pos.cur == start.cur) {
    iterator new_start = reserve_elements_at_front(n);
    __stl_debug_do(new_start.set_overrun(true));
#     ifdef __STL_USE_EXCEPTIONS
    try {
#     endif /* __STL_USE_EXCEPTIONS */
        uninitialized_copy(first, last, new_start);
#     ifdef __STL_USE_EXCEPTIONS
    }
    catch(...) {
        destroy_nodes_at_front(new_start);      
        throw;
    }
#     endif /* __STL_USE_EXCEPTIONS */
    start = new_start;
    __stl_debug_do(start.set_overrun(false));
  }
  else if (pos.cur == finish.cur) {
    iterator new_finish = reserve_elements_at_back(n);
    __stl_debug_do(finish.set_overrun(true));
#     ifdef __STL_USE_EXCEPTIONS
    try {
#     endif /* __STL_USE_EXCEPTIONS */
        uninitialized_copy(first, last, finish);
#     ifdef __STL_USE_EXCEPTIONS
    }
    catch(...) {
        destroy_nodes_at_back(new_finish);      
        __stl_debug_do(finish.set_overrun(false));
        throw;
    }
#     endif /* __STL_USE_EXCEPTIONS */
    finish = new_finish;
  }
  else
    insert_aux(pos, first, last, n);
}

#endif /* __STL_MEMBER_TEMPLATES */

template <class T, class Alloc, size_t BufSize>
__iterator__
deque<T, Alloc, BufSize>::erase(iterator first, iterator last) {
  __stl_debug_check(__check_range(first,last, start, finish));    
  if (first == start && last == finish) {
    clear();
    return finish;
  }
  else {
    difference_type n = last - first;
    difference_type elems_before = first - start;
    if (elems_before < (size() - n) / 2) {
      copy_backward(start, first, last);
      iterator new_start = start + n;
      destroy(start, new_start);
      for (map_pointer cur = start.node; cur < new_start.node; ++cur)
        data_allocator::deallocate(*cur, buffer_size());
      start = new_start;
    }
    else {
      copy(last, finish, first);
      iterator new_finish = finish - n;
      destroy(new_finish, finish);
      for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
        data_allocator::deallocate(*cur, buffer_size());
      finish = new_finish;
    }
    return start + elems_before;
  }
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::clear() {
  __stl_debug_do(invalidate_all());
  for (map_pointer node = start.node + 1; node < finish.node; ++node) {
    destroy(*node, *node + buffer_size());
    data_allocator::deallocate(*node, buffer_size());
  }

  if (start.node != finish.node) {
    destroy(start.cur, start.last);
    destroy(finish.first, finish.cur);
    data_allocator::deallocate(finish.first, buffer_size());
  }
  else
    destroy(start.cur, finish.cur);

  finish = start;
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements) {
  size_type num_nodes = num_elements / buffer_size() + 1;

  map_size = max(initial_map_size(), num_nodes + 2);
  map = map_allocator::allocate(map_size);

  map_pointer nstart = map + (map_size - num_nodes) / 2;
  map_pointer nfinish = nstart + num_nodes - 1;
    
  map_pointer cur;
  __STL_TRY {
    for (cur = nstart; cur <= nfinish; ++cur)
      *cur = allocate_node();
  }
#     ifdef  __STL_USE_EXCEPTIONS 
  catch(...) {
    for (map_pointer n = nstart; n < cur; ++n)
      deallocate_node(*n);
    map_allocator::deallocate(map, map_size);
    throw;
  }
#     endif /* __STL_USE_EXCEPTIONS */

  start.set_node(nstart);
  finish.set_node(nfinish);
  start.cur = start.first;
  finish.cur = finish.first + num_elements % buffer_size();
  __stl_debug_do(safe_init(this));
  __stl_debug_do(init_bounds());
}

// This is only used as a cleanup function in catch clauses.
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_map_and_nodes() {
  __stl_debug_do(invalidate_all()); 
  __stl_debug_do(start.invalidate()); 
  __stl_debug_do(finish.invalidate());
  __stl_debug_do(invalidate()); 
  for (map_pointer cur = start.node; cur <= finish.node; ++cur)
    deallocate_node(*cur);
  map_allocator::deallocate(map, map_size);
}
  

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::fill_initialize(size_type n,
                                               const value_type& value) {
  create_map_and_nodes(n);
  map_pointer cur;
  __STL_TRY {
    for (cur = start.node; cur < finish.node; ++cur)
      uninitialized_fill(*cur, *cur + buffer_size(), value);
    uninitialized_fill(finish.first, finish.cur, value);
  }
#       ifdef __STL_USE_EXCEPTIONS
  catch(...) {
    for (map_pointer n = start.node; n < cur; ++n)
      destroy(*n, *n + buffer_size());
    destroy_map_and_nodes();
    throw;
  }
#       endif /* __STL_USE_EXCEPTIONS */
}

#ifdef __STL_MEMBER_TEMPLATES  

template <class T, class Alloc, size_t BufSize>
template <class InputIterator>
void deque<T, Alloc, BufSize>::range_initialize(InputIterator first,
                                                InputIterator last,
                                                input_iterator_tag) {
  create_map_and_nodes(0);
  for ( ; first != last; ++first)
    push_back(*first);
}

template <class T, class Alloc, size_t BufSize>
template <class ForwardIterator>
void deque<T, Alloc, BufSize>::range_initialize(ForwardIterator first,
                                                ForwardIterator last,
                                                forward_iterator_tag) {
  size_type n = 0;
  distance(first, last, n);
  create_map_and_nodes(n);
  __STL_TRY {
    uninitialized_copy(first, last, start);
  }
  __STL_UNWIND(destroy_map_and_nodes());
}

#endif /* __STL_MEMBER_TEMPLATES */

// Called only if finish.cur == finish.last - 1.
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_back_aux(const value_type& t) {
  value_type t_copy = t;
  reserve_map_at_back();
  *(finish.node + 1) = allocate_node();
  __STL_TRY {
    construct(finish.cur, t_copy);
    finish.set_node(finish.node + 1);
    finish.cur = finish.first;
  }
  __STL_UNWIND(deallocate_node(*(finish.node + 1)));
}

// Called only if start.cur == start.first.
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_front_aux(const value_type& t) {
  value_type t_copy = t;
  reserve_map_at_front();
  *(start.node - 1) = allocate_node();
  __STL_TRY {
    start.set_node(start.node - 1);
    start.cur = start.last - 1;
    construct(start.cur, t_copy);
  }
#     ifdef __STL_USE_EXCEPTIONS
  catch(...) {
    start.set_node(start.node + 1);
    start.cur = start.first;
    deallocate_node(*(start.node - 1));
    throw;
  }
#     endif /* __STL_USE_EXCEPTIONS */
} 

// Called only if finish.cur == finish.first.
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>:: pop_back_aux() {
  deallocate_node(finish.first);
  finish.set_node(finish.node - 1);
  finish.cur = finish.last - 1;
  destroy(finish.cur);
}

// Called only if start.cur == start.last - 1.  Note that if the deque
//  has at least one element (a necessary precondition for this member
//  function), and if start.cur == start.last, then the deque must have
//  at least two nodes.
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_front_aux() {
  destroy(start.cur);
  deallocate_node(start.first);
  start.set_node(start.node + 1);
  start.cur = start.first;
}      

#ifdef __STL_MEMBER_TEMPLATES  

template <class T, class Alloc, size_t BufSize>
template <class InputIterator>
void deque<T, Alloc, BufSize>::insert(iterator pos,
                                      InputIterator first, InputIterator last,
                                      input_iterator_tag) {
  __stl_debug_check(pos.nonsingular(this) &&
                    __check_range(first,last));    
  copy(first, last, inserter(*this, pos));
}

template <class T, class Alloc, size_t BufSize>
template <class ForwardIterator>
void deque<T, Alloc, BufSize>::insert(iterator pos,
                                      ForwardIterator first,
                                      ForwardIterator last,
                                      forward_iterator_tag) {
  __stl_debug_check(pos.nonsingular(this));
  size_type n = 0;
  distance(first, last, n);
  if (pos.cur == start.cur) {
    iterator new_start = reserve_elements_at_front(n);
    __stl_debug_do(new_start.set_overrun(true));
    __STL_TRY {
        uninitialized_copy(first, last, new_start);
    }
    __STL_UNWIND(destroy_nodes_at_front(new_start));
    start = new_start;
    __stl_debug_do(start.set_overrun(false));
  }
  else if (pos.cur == finish.cur) {
    iterator new_finish = reserve_elements_at_back(n);
    __stl_debug_do(finish.set_overrun(true));
#     ifdef __STL_USE_EXCEPTIONS
      try {
#     endif /* __STL_USE_EXCEPTIONS */
          uninitialized_copy(first, last, finish);
#     ifdef __STL_USE_EXCEPTIONS
      }
      catch(...) {
          destroy_nodes_at_back(new_finish);      
          __stl_debug_do(finish.set_overrun(false));
          throw;
      }
#     endif /* __STL_USE_EXCEPTIONS */
    finish = new_finish;
  }
  else
    insert_aux(pos, first, last, n);
}

#endif /* __STL_MEMBER_TEMPLATES */

template <class T, class Alloc, size_t BufSize>
__iterator__
deque<T, Alloc, BufSize>::insert_aux(iterator pos, const value_type& x) {
  difference_type index = pos - start;
  value_type x_copy = x;
  if (index < difference_type(size() / 2)) {
    push_front(front());
    iterator front1 = start;
    ++front1;
    iterator front2 = front1;
    ++front2;
    pos = start + index;
    iterator pos1 = pos;
    ++pos1;
    copy(front2, pos1, front1);
  }
  else {
    push_back(back());
    iterator back1 = finish;
    --back1;
    iterator back2 = back1;
    --back2;
    pos = start + index;
    copy_backward(pos, back2, back1);
  }
  *pos = x_copy;
  return pos;
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert_aux(iterator pos,
                                          size_type n, const value_type& x) {
  const size_type elems_before(pos - start);
  size_type length = size();
  value_type x_copy = x;
  if (elems_before < length / 2) {
    iterator new_start = reserve_elements_at_front(n);
    __stl_debug_do(new_start.set_overrun(true));
    iterator old_start = start;
    __stl_debug_do(old_start.set_overrun(true));
    pos = start + elems_before;
    __stl_debug_do(pos.set_overrun(true));
    __STL_TRY {
      if (elems_before >= difference_type(n)) {
        iterator start_n = start + difference_type(n);
        uninitialized_copy(start, start_n, new_start);
        start = new_start;
        __stl_debug_do(start.set_overrun(false));
        copy(start_n, pos, old_start);
        fill(pos - difference_type(n), pos, x_copy);
      }
      else {
        __uninitialized_copy_fill(start, pos, new_start, start, x_copy);
        start = new_start;
        __stl_debug_do(start.set_overrun(false));
        fill(old_start, pos, x_copy);
      }
    }
    __STL_UNWIND(destroy_nodes_at_front(new_start));
  }
  else {
    iterator new_finish = reserve_elements_at_back(n);
    __stl_debug_do(new_finish.set_overrun(true));
    __stl_debug_do(finish.set_overrun(true));
    iterator old_finish = finish;
    const difference_type elems_after = difference_type(length) - elems_before;
    pos = finish - elems_after;
    __STL_TRY {
      if (elems_after > difference_type(n)) {
        iterator finish_n = finish - difference_type(n);
        uninitialized_copy(finish_n, finish, finish);
        finish = new_finish;
        copy_backward(pos, finish_n, old_finish);
        fill(pos, pos + difference_type(n), x_copy);
      }
      else {
        __uninitialized_fill_copy(finish, pos + difference_type(n),
                                  x_copy,
                                  pos, finish);
        finish = new_finish;
        fill(pos, old_finish, x_copy);
      }
    }
#       ifdef __STL_USE_EXCEPTIONS
    catch(...) {
      destroy_nodes_at_back(new_finish);
      __stl_debug_do(finish.set_overrun(false));
      throw;
    }
#       endif /* __STL_USE_EXCEPTIONS */    
  }
  __stl_debug_do(invalidate_all());        
}

#ifdef __STL_MEMBER_TEMPLATES  

template <class T, class Alloc, size_t BufSize>
template <class ForwardIterator>
void deque<T, Alloc, BufSize>::insert_aux(iterator pos,
                                          ForwardIterator first,
                                          ForwardIterator last,
                                          size_type n)
{
  const difference_type elems_before = pos - start;
  size_type length = size();
  __stl_debug_do(pos.set_overrun(true));
  if (elems_before < length / 2) {
    iterator new_start = reserve_elements_at_front(n);
    __stl_debug_do(new_start.set_overrun(true));
    iterator old_start = start;
    pos = start + elems_before;
    __STL_TRY {
      if (elems_before >= difference_type(n)) {
        iterator start_n = start + difference_type(n); 
        uninitialized_copy(start, start_n, new_start);
        start = new_start;
        __stl_debug_do(start.set_overrun(false));
        copy(start_n, pos, old_start);
        copy(first, last, pos - difference_type(n));
      }
      else {
        ForwardIterator mid = first;
        advance(mid, difference_type(n) - elems_before);
        __uninitialized_copy_copy(start, pos, first, mid, new_start);
        start = new_start;
        __stl_debug_do(start.set_overrun(false));
        copy(mid, last, old_start);
      }
    }
    __STL_UNWIND(destroy_nodes_at_front(new_start));
  }
  else {
    iterator new_finish = reserve_elements_at_back(n);
    iterator old_finish = finish;
    const difference_type elems_after = difference_type(length) - elems_before;
    pos = finish - elems_after;
    __stl_debug_do(finish.set_overrun(true));

    __STL_TRY {
      if (elems_after > difference_type(n)) {
        iterator finish_n = finish - difference_type(n);
        uninitialized_copy(finish_n, finish, finish);
        finish = new_finish;
        copy_backward(pos, finish_n, old_finish);
        copy(first, last, pos);
      }
      else {
        ForwardIterator mid = first;
        advance(mid, elems_after);
        __uninitialized_copy_copy(mid, last, pos, finish, finish);
        finish = new_finish;
        copy(first, mid, pos);
      }
#       ifdef __STL_USE_EXCEPTIONS
    }
    catch(...) {
      destroy_nodes_at_back(new_finish);
      __stl_debug_do(finish.set_overrun(false));
      throw;
    }
#       endif /* __STL_USE_EXCEPTIONS */
  }
  __stl_debug_do(invalidate_all());        
}

#else /* __STL_MEMBER_TEMPLATES */

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert_aux(iterator pos,
                                          const value_type* first,
                                          const value_type* last,
                                          size_type n)
{
  const difference_type elems_before = pos - start;
  size_type length = size();
  __stl_debug_do(pos.set_overrun(true));
  if (elems_before < difference_type(length / 2)) {
    iterator new_start = reserve_elements_at_front(n);
    iterator old_start = start;
    pos = start + elems_before;
    __stl_debug_do(new_start.set_overrun(true));
    __STL_TRY {
      if (elems_before >= difference_type(n)) {
        iterator start_n = start + difference_type(n);
        uninitialized_copy(start, start_n, new_start);
        start = new_start;
        __stl_debug_do(start.set_overrun(false));
        copy(start_n, pos, old_start);
        copy(first, last, pos - difference_type(n));
      }
      else {
        const value_type* mid = first + (difference_type(n) - elems_before);
        __uninitialized_copy_copy(start, pos, first, mid, new_start);
        start = new_start;
        __stl_debug_do(start.set_overrun(false));
        copy(mid, last, old_start);
      }
    }
    __STL_UNWIND(destroy_nodes_at_front(new_start));
  }
  else {
    iterator new_finish = reserve_elements_at_back(n);
    iterator old_finish = finish;
    const difference_type elems_after = difference_type(length) - elems_before;
    pos = finish - elems_after;
    __stl_debug_do(finish.set_overrun(true));
    __STL_TRY {
      if (elems_after > difference_type(n)) {
        iterator finish_n = finish - difference_type(n);
        uninitialized_copy(finish_n, finish, finish);
        finish = new_finish;
        copy_backward(pos, finish_n, old_finish);
        copy(first, last, pos);
      }
      else {
        const value_type* mid = first + elems_after;
        __uninitialized_copy_copy(mid, last, pos, finish, finish);
        finish = new_finish;
        copy(first, mid, pos);
      }
    }
#       ifdef __STL_USE_EXCEPTIONS
    catch(...) {
      destroy_nodes_at_back(new_finish);
      __stl_debug_do(finish.set_overrun(false));
      throw;
    }
#       endif /* __STL_USE_EXCEPTIONS */
  }
  __stl_debug_do(invalidate_all());        
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert_aux(iterator pos,
                                          const_iterator first,
                                          const_iterator last,
                                          size_type n)
{
  const difference_type elems_before = pos - start;
  size_type length = size();
  __stl_debug_do(pos.set_overrun(true));
  if (elems_before < length / 2) {
    iterator new_start = reserve_elements_at_front(n);
    __stl_debug_do(new_start.set_overrun(true));
    iterator old_start = start;
    pos = start + elems_before;
    __STL_TRY {
      if (elems_before >= n) {
        iterator start_n = start + n;
        uninitialized_copy(start, start_n, new_start);
        start = new_start;
        __stl_debug_do(start.set_overrun(false));
        copy(start_n, pos, old_start);
        copy(first, last, pos - difference_type(n));
      }
      else {
        const_iterator mid = first + (n - elems_before);
        __uninitialized_copy_copy(start, pos, first, mid, new_start);
        start = new_start;
        __stl_debug_do(start.set_overrun(false));
        copy(mid, last, old_start);
      }
    }
    __STL_UNWIND(destroy_nodes_at_front(new_start));
  }
  else {
    iterator new_finish = reserve_elements_at_back(n);
    iterator old_finish = finish;
    const difference_type elems_after = length - elems_before;
    pos = finish - elems_after;
    __stl_debug_do(finish.set_overrun(true));
    __STL_TRY {
      if (elems_after > n) {
        iterator finish_n = finish - difference_type(n);
        uninitialized_copy(finish_n, finish, finish);
        finish = new_finish;
        copy_backward(pos, finish_n, old_finish);
        copy(first, last, pos);
      }
      else {
        const_iterator mid = first + elems_after;
        __uninitialized_copy_copy(mid, last, pos, finish, finish);
        finish = new_finish;
        copy(first, mid, pos);
      }
    }
#       ifdef __STL_USE_EXCEPTIONS
    catch(...) {
      destroy_nodes_at_back(new_finish);
      __stl_debug_do(finish.set_overrun(true));
      throw;
    }
#       endif /* __STL_USE_EXCEPTIONS */
  }
  __stl_debug_do(invalidate_all());        
}

#endif /* __STL_MEMBER_TEMPLATES */

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::new_elements_at_front(size_type new_elements) {
  size_type new_nodes = (new_elements + buffer_size() - 1) / buffer_size();
  reserve_map_at_front(new_nodes);
  size_type i;
  __STL_TRY {
    for (i = 1; i <= new_nodes; ++i)
      *(start.node - i) = allocate_node();
  }
#       ifdef __STL_USE_EXCEPTIONS
  catch(...) {
    for (size_type j = 1; j < i; ++j)
      deallocate_node(*(start.node - j));      
    throw;
  }
#       endif /* __STL_USE_EXCEPTIONS */
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::new_elements_at_back(size_type new_elements) {
  size_type new_nodes = (new_elements + buffer_size() - 1) / buffer_size();
  reserve_map_at_back(new_nodes);
  size_type i;
  __STL_TRY {
    for (i = 1; i <= new_nodes; ++i)
      *(finish.node + i) = allocate_node();
  }
#       ifdef __STL_USE_EXCEPTIONS
  catch(...) {
    for (size_type j = 1; j < i; ++j)
      deallocate_node(*(finish.node + j));      
    throw;
  }
#       endif /* __STL_USE_EXCEPTIONS */
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_nodes_at_front(iterator before_start) {
  for (map_pointer n = before_start.node; n < start.node; ++n)
    deallocate_node(*n);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_nodes_at_back(iterator after_finish) {
  for (map_pointer n = after_finish.node; n > finish.node; --n)
    deallocate_node(*n);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reallocate_map(size_type nodes_to_add,
                                              bool add_at_front) {
  size_type old_num_nodes = finish.node - start.node + 1;
  size_type new_num_nodes = old_num_nodes + nodes_to_add;

  map_pointer new_nstart;
  if (map_size > 2 * new_num_nodes) {
    new_nstart = map + (map_size - new_num_nodes) / 2 
                     + (add_at_front ? nodes_to_add : 0);
    if (new_nstart < start.node)
      copy(start.node, finish.node + 1, new_nstart);
    else
      copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
  }
  else {
    size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;

    map_pointer new_map = map_allocator::allocate(new_map_size);
    new_nstart = new_map + (new_map_size - new_num_nodes) / 2
                         + (add_at_front ? nodes_to_add : 0);
    copy(start.node, finish.node + 1, new_nstart);
    map_allocator::deallocate(map, map_size);

    map = new_map;
    map_size = new_map_size;
  }

  start.set_node(new_nstart);
  finish.set_node(new_nstart + old_num_nodes - 1);
  __stl_debug_do(invalidate_all());        
}


// Nonmember functions.

#ifndef __STL_NON_TYPE_TMPL_PARAM_BUG

template <class T, class Alloc, size_t BufSiz>
bool operator==(const deque<T, Alloc, BufSiz>& x,
                const deque<T, Alloc, BufSiz>& y) {
  return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <class T, class Alloc, size_t BufSiz>
bool operator<(const deque<T, Alloc, BufSiz>& x,
               const deque<T, Alloc, BufSiz>& y) {
  return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

#  endif /* __STL_NON_TYPE_TMPL_PARAM_BUG */

// do a cleanup
# undef deque
# undef __iterator__
# undef iterator
# undef const_iterator
# undef size_type
# undef value_type

# if !defined ( __STL_DEFAULT_TYPE_PARAM) || defined (__STL_NO_DEFAULT_NON_TYPE_PARAM)
// provide a "default" deque adaptor
# if defined (__STL_DEFAULT_TYPE_PARAM)
template <class T, class Alloc=alloc>
class deque : public __deque__<T,Alloc,0> {
    typedef __deque__<T,Alloc,0> super;
    typedef deque<T,Alloc> self;
# else
template <class T>
class deque : public __deque__<T,alloc,0> {
    typedef __deque__<T,alloc,0> super;
    typedef deque<T> self;
# endif
public:
    __CONTAINER_SUPER_TYPEDEFS
    __IMPORT_SUPER_COPY_ASSIGNMENT(deque)
    deque() : super() { }
    deque(size_type n, const T& value) : super(n, value) { }
    explicit deque(size_type n) : super(n) { }
    deque(const T* first, const T* last) : super(first, last) { }
    deque(const_iterator first, const_iterator last) : super(first, last) { }
    ~deque() { }
};

#  if defined (__STL_BASE_MATCH_BUG)
template <class T>
inline bool 
operator==(const deque<T>& x, const deque<T>& y) {
    return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <class T>
inline bool 
operator<(const deque<T>& x, const deque<T>& y) {
    return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}
#  endif /* BASE_MATCH_BUG */

# endif /* __STL_DEFAULT_TYPE_PARAM */

# if defined (__STL_CLASS_PARTIAL_SPECIALIZATION )
template <class T, class Alloc, size_t BufSiz>
inline void swap(__deque__<T,Alloc,BufSiz>& a, __deque__<T,Alloc,BufSiz>& b) { a.swap(b); }
# endif

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#endif
          
__STL_END_NAMESPACE 
  
#endif /* __SGI_STL_INTERNAL_DEQUE_H */

// Local Variables:
// mode:C++
// End:
