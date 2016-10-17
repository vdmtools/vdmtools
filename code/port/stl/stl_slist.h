/*
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

#ifndef __SGI_STL_INTERNAL_SLIST_H
#define __SGI_STL_INTERNAL_SLIST_H

#  define  slist  __WORKAROUND_RENAME(slist)

__STL_BEGIN_NAMESPACE

# if defined ( __STL_USE_ABBREVS )
#  define __slist_iterator         _L__It
# endif

struct __slist_node_base
{
  __slist_node_base* next;
};

inline __slist_node_base* __slist_make_link(__slist_node_base* prev_node,
                                            __slist_node_base* new_node)
{
  new_node->next = prev_node->next;
  prev_node->next = new_node;
  return new_node;
}

INLINE_LOOP __slist_node_base* __slist_previous(__slist_node_base* head,
                                           const __slist_node_base* node)
{
  while (head && head->next != node)
    head = head->next;
  return head;
}

INLINE_LOOP const __slist_node_base* __slist_previous(const __slist_node_base* head,
                                                 const __slist_node_base* node)
{
  while (head && head->next != node)
    head = head->next;
  return head;
}

inline void __slist_splice_after(__slist_node_base* pos,
                                 __slist_node_base* before_first,
                                 __slist_node_base* before_last)
{
  if (pos != before_first && pos != before_last) {
    __slist_node_base* first = before_first->next;
    __slist_node_base* after = pos->next;
    before_first->next = before_last->next;
    pos->next = first;
    before_last->next = after;
  }
}

INLINE_LOOP __slist_node_base* __slist_reverse(__slist_node_base* node)
{
  __slist_node_base* result = node;
  node = node->next;
  result->next = 0;
  while(node) {
    __slist_node_base* next = node->next;
    node->next = result;
    result = node;
    node = next;
  }
  return result;
}

template <class T>
struct __slist_node : public __slist_node_base
{
  T data;
};

struct __slist_iterator_base
# if defined ( __STL_DEBUG )
 : public __safe_base 
# endif
{
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef forward_iterator_tag iterator_category;

  __slist_node_base* node;

# if defined ( __STL_DEBUG )
  __slist_iterator_base(const __safe_base* root, __slist_node_base* x) :
      __safe_base(root), node(x) {}
  __slist_iterator_base() : __safe_base(0) {}
# else
  __slist_iterator_base(__slist_node_base* x) : node(x) {}
# endif
  void incr() { node = node->next; }

  bool operator==(const __slist_iterator_base& x) const {
    __stl_debug_check(__check_same_owner(*this,x));                         
    return node == x.node;
  }
  bool operator!=(const __slist_iterator_base& x) const {
    __stl_debug_check(__check_same_owner(*this,x));                         
    return node != x.node;
  }
};

template <class T, class Ref, class Ptr>
struct __slist_iterator : public __slist_iterator_base
{
  typedef __slist_iterator<T, T&, T*>             iterator;
  typedef __slist_iterator<T, const T&, const T*> const_iterator;
  typedef __slist_iterator<T, Ref, Ptr>           self;

  typedef T value_type;
  typedef Ptr pointer;
  typedef Ref reference;
  typedef __slist_node<T> list_node;

# if defined ( __STL_DEBUG )
  list_node* get_iterator() const { return (list_node*)node; }  
  __slist_iterator(const __safe_base* root, list_node* x) : __slist_iterator_base(root,x) {}
  __slist_iterator() : __slist_iterator_base(0,0) {}
  __slist_iterator(const iterator& x) : 
      __slist_iterator_base((const __safe_base*)x.owner_, x.node) {}
# else
  __slist_iterator(list_node* x) : __slist_iterator_base(x) {}
  __slist_iterator() : __slist_iterator_base(0) {}
  __slist_iterator(const iterator& x) : __slist_iterator_base(x.node) {}
# endif

  reference operator*() const { return ((list_node*) node)->data; }
#ifndef __SGI_STL_NO_ARROW_OPERATOR
  pointer operator->() const { return &(operator*()); }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */

  self& operator++()
  {
    __stl_verbose_assert(node != 0, __STL_MSG_INVALID_ADVANCE); 
    incr();
    return *this;
  }
  self operator++(int)
  {
    self tmp = *this;
    incr();
    return tmp;
  }
};

#ifndef __STL_CLASS_PARTIAL_SPECIALIZATION

inline ptrdiff_t*
distance_type(const __slist_iterator_base&)
{
  return 0;
}

inline forward_iterator_tag
iterator_category(const __slist_iterator_base&)
{
  return forward_iterator_tag();
}

template <class T, class Ref, class Ptr> 
inline T* 
value_type(const __slist_iterator<T, Ref, Ptr>&) {
  return 0;
}

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

INLINE_LOOP size_t __slist_size(__slist_node_base* node)
{
  size_t result = 0;
  for ( ; node != 0; node = node->next)
    ++result;
  return result;
}

template <class T, __DFL_TYPE_PARAM(Alloc,alloc) >
class slist
{
  typedef slist<T,Alloc> self;
public:
  typedef T value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  typedef __slist_iterator<T, T&, T*>             iterator;
  typedef __slist_iterator<T, const T&, const T*> const_iterator;

private:
  typedef __slist_node<T> list_node;
  typedef __slist_node_base list_node_base;
  typedef __slist_iterator_base iterator_base;
  typedef simple_alloc<list_node, Alloc> list_node_allocator;

  static list_node* __create_node(const value_type& x, __false_type) {
    list_node* node = list_node_allocator::allocate();
    __STL_TRY {
      construct(&node->data, x);
      node->next = 0;
    }
    __STL_UNWIND(list_node_allocator::deallocate(node));
    return node;
  }

  static list_node* __create_node(const value_type& x, __true_type) {
      list_node* node = list_node_allocator::allocate();
      construct(&node->data, x);
      node->next = 0;
      return node;
  }

  static list_node* create_node(const value_type& x) {
      typedef typename __type_traits<value_type>::is_POD_type is_POD_type;
      return __create_node(x, is_POD_type());
  }
  
# ifndef __STL_DEBUG
  static 
# endif
  void destroy_node(list_node* node) {
    __stl_debug_do(invalidate_node(node));
    destroy(&node->data);
    list_node_allocator::deallocate(node);
  }

  void fill_initialize(size_type n, const value_type& x) {
    __stl_debug_do(iter_list.safe_init(&head));
    head.next = 0;
    __STL_TRY {
      _insert_after_fill(&head, n, x);
    }
    __STL_UNWIND(clear());
  }    

#ifdef __STL_MEMBER_TEMPLATES
  template <class InputIterator>
  void range_initialize(InputIterator first, InputIterator last) {
    head.next = 0;
    __stl_debug_do(iter_list.safe_init(&head));
    __STL_TRY {
      _insert_after_range(&head, first, last);
    }
    __STL_UNWIND(clear());
  }
#else /* __STL_MEMBER_TEMPLATES */
  void range_initialize(const value_type* first, const value_type* last) {
    __stl_debug_do(iter_list.safe_init(&head));
    head.next = 0;
    __STL_TRY {
      _insert_after_range(&head, first, last);
    }
    __STL_UNWIND(clear());
  }
  void range_initialize(const_iterator first, const_iterator last) {
    head.next = 0;
    __stl_debug_do(iter_list.safe_init(&head));
    __STL_TRY {
      _insert_after_range(&head, first, last);
    }
    __STL_UNWIND(clear());
  }
#endif /* __STL_MEMBER_TEMPLATES */

private:
  list_node_base head;
# if defined (__STL_DEBUG)
protected:
    friend class __safe_base;
    mutable __safe_server iter_list;
    void invalidate_all() { iter_list.invalidate_all();}
# endif

public:
  slist() { 
      __stl_debug_do(iter_list.safe_init(&head));
      head.next = 0; }

  slist(size_type n, const value_type& x) { fill_initialize(n, x); }
  slist(int n, const value_type& x) { fill_initialize(n, x); }
  slist(long n, const value_type& x) { fill_initialize(n, x); }
  explicit slist(size_type n) { fill_initialize(n, value_type()); }

#ifdef __STL_MEMBER_TEMPLATES
  template <class InputIterator>
  slist(InputIterator first, InputIterator last) {
    range_initialize(first, last);
  }

#else /* __STL_MEMBER_TEMPLATES */
  slist(const_iterator first, const_iterator last) {
    range_initialize(first, last);
  }
  slist(const value_type* first, const value_type* last) {
    range_initialize(first, last);
  }
#endif /* __STL_MEMBER_TEMPLATES */

  slist(const slist<T,Alloc>& L) { range_initialize(L.begin(), L.end()); }

  self& operator= (const slist<T,Alloc>& L);

  ~slist() { clear(); }

public:

# if defined (__STL_DEBUG)
  iterator make_iterator(list_node* l) { return iterator(&iter_list, l); }
  const_iterator make_const_iterator(list_node* l) const { return const_iterator(&iter_list, l); }
  void invalidate_node(list_node* it) {__invalidate_iterator(&iter_list, it, end()); }
  void invalidate_iterator(const iterator& it) {__invalidate_iterator(&iter_list, it.node, it); }
# else
  iterator make_iterator(list_node* l) { return iterator(l); }
  const_iterator make_const_iterator(list_node* l) const { return const_iterator(l); }
# endif
  iterator begin() { return make_iterator((list_node*)head.next); }
  const_iterator begin() const { return make_const_iterator((list_node*)head.next);}

  iterator end() { return make_iterator(0); }
  const_iterator end() const { return make_const_iterator(0); }

  size_type size() const { return __slist_size(head.next); }
  
  size_type max_size() const { return size_type(-1); }

  bool empty() const { return head.next == 0; }

  void swap(slist<T,Alloc>& L)
  {
    __stl_debug_do(iter_list.swap_owners(L.iter_list));
    list_node_base* tmp = head.next;
    head.next = L.head.next;
    L.head.next = tmp;
  }

public:
  friend bool operator== __STL_NULL_TMPL_ARGS (const slist<T, Alloc>& L1, const slist<T, Alloc>& L2);

public:

  reference front() { return ((list_node*) head.next)->data; }
  const_reference front() const { return ((list_node*) head.next)->data; }
  void push_front(const value_type& x)   {
    __slist_make_link(&head, create_node(x));
  }
  void pop_front() {
    list_node* node = (list_node*) head.next;
    head.next = node->next;
    destroy_node(node);
  }

  iterator previous(const_iterator pos) {
    return make_iterator((list_node*) __slist_previous(&head, pos.node));
  }
  const_iterator previous(const_iterator pos) const {
    return make_const_iterator((list_node*) __slist_previous(&head, pos.node));
  }

private:
  list_node* _insert_after(list_node_base* pos, const value_type& x) {
    return (list_node*) (__slist_make_link(pos, create_node(x)));
  }

  void _insert_after_fill(list_node_base* pos,
                          size_type n, const value_type& x) {
    for (size_type i = 0; i < n; ++i)
      pos = __slist_make_link(pos, create_node(x));
  }

#ifdef __STL_MEMBER_TEMPLATES
  template <class InIter>
  void _insert_after_range(list_node_base* pos, InIter first, InIter last) {
    while (first != last) {
      pos = __slist_make_link(pos, create_node(*first));
      ++first;
    }
  }
#else /* __STL_MEMBER_TEMPLATES */
  void _insert_after_range(list_node_base* pos,
                           const_iterator first, const_iterator last) {
    while (first != last) {
      pos = __slist_make_link(pos, create_node(*first));
      ++first;
    }
  }
  void _insert_after_range(list_node_base* pos,
                           const value_type* first, const value_type* last) {
    while (first != last) {
      pos = __slist_make_link(pos, create_node(*first));
      ++first;
    }
  }
#endif /* __STL_MEMBER_TEMPLATES */

  list_node_base* erase_after(list_node_base* pos) {
    list_node* next = (list_node*) (pos->next);
    list_node_base* next_next = next->next;
    pos->next = next_next;
    destroy_node(next);
    return next_next;
  }
   
  list_node_base* erase_after(list_node_base* before_first,
                              list_node_base* last_node) {
    list_node* cur = (list_node*) (before_first->next);
    while (cur != last_node) {
      list_node* tmp = cur;
      cur = (list_node*) cur->next;
      destroy_node(tmp);
    }
    before_first->next = last_node;
    return last_node;
  }


public:

  iterator insert_after(iterator pos, const value_type& x) {
    return make_iterator(_insert_after(pos.node, x));
  }

  iterator insert_after(iterator pos) {
    return insert_after(pos, value_type());
  }

  void insert_after(iterator pos, size_type n, const value_type& x) {
    _insert_after_fill(pos.node, n, x);
  }
  void insert_after(iterator pos, int n, const value_type& x) {
    _insert_after_fill(pos.node, (size_type) n, x);
  }
  void insert_after(iterator pos, long n, const value_type& x) {
    _insert_after_fill(pos.node, (size_type) n, x);
  }

#ifdef __STL_MEMBER_TEMPLATES
  template <class InIter>
  void insert_after(iterator pos, InIter first, InIter last) {
    _insert_after_range(pos.node, first, last);
  }
#else /* __STL_MEMBER_TEMPLATES */
  void insert_after(iterator pos, const_iterator first, const_iterator last) {
    _insert_after_range(pos.node, first, last);
  }
  void insert_after(iterator pos,
                    const value_type* first, const value_type* last) {
    _insert_after_range(pos.node, first, last);
  }
#endif /* __STL_MEMBER_TEMPLATES */

  iterator insert(iterator pos, const value_type& x) {
    __stl_debug_check(__check_if_owner(&iter_list,pos));
    return make_iterator(_insert_after(__slist_previous(&head, pos.node), x));
  }

  iterator insert(iterator pos) {
    __stl_debug_check(__check_if_owner(&iter_list,pos));
    return make_iterator(_insert_after(__slist_previous(&head, pos.node),
                                  value_type()));
  }

  void insert(iterator pos, size_type n, const value_type& x) {
    __stl_debug_check(__check_if_owner(&iter_list,pos));
    _insert_after_fill(__slist_previous(&head, pos.node), n, x);
  } 
  void insert(iterator pos, int n, const value_type& x) {
    __stl_debug_check(__check_if_owner(&iter_list,pos));
    _insert_after_fill(__slist_previous(&head, pos.node), (size_type) n, x);
  } 
  void insert(iterator pos, long n, const value_type& x) {
    __stl_debug_check(__check_if_owner(&iter_list,pos));
    _insert_after_fill(__slist_previous(&head, pos.node), (size_type) n, x);
  } 
    
#ifdef __STL_MEMBER_TEMPLATES
  template <class InIter>
  void insert(iterator pos, InIter first, InIter last) {
    __stl_debug_check(__check_if_owner(&iter_list,pos));
    _insert_after_range(__slist_previous(&head, pos.node), first, last);
  }
#else /* __STL_MEMBER_TEMPLATES */
  void insert(iterator pos, const_iterator first, const_iterator last) {
    __stl_debug_check(__check_if_owner(&iter_list,pos));
    _insert_after_range(__slist_previous(&head, pos.node), first, last);
  }
  void insert(iterator pos, const value_type* first, const value_type* last) {
    __stl_debug_check(__check_if_owner(&iter_list,pos));
    _insert_after_range(__slist_previous(&head, pos.node), first, last);
  }
#endif /* __STL_MEMBER_TEMPLATES */

public:
  iterator erase_after(iterator pos) {
    __stl_debug_check(__check_if_owner(&iter_list,pos));
    return make_iterator((list_node*)erase_after(pos.node));
  }
  iterator erase_after(iterator before_first, iterator last) {
    __stl_debug_check(__check_if_owner(&iter_list,before_first));
    __stl_debug_check(__check_if_owner(&iter_list,last));
    return make_iterator((list_node*)erase_after(before_first.node, last.node));
  }

  iterator erase(iterator pos) {
    __stl_debug_check(__check_if_owner(&iter_list,pos));
    return make_iterator((list_node*)erase_after(__slist_previous(&head, pos.node)));
  }
  iterator erase(iterator first, iterator last) {
    __stl_debug_check(__check_if_owner(&iter_list,first));
    __stl_debug_check(__check_if_owner(&iter_list,last));
    return make_iterator((list_node*)erase_after(__slist_previous(&head, first.node),
                                    last.node));
  }

  void resize(size_type new_size, const T& x);
  void resize(size_type new_size) { resize(new_size, T()); }
  void clear() { 
      __stl_debug_do(invalidate_all());      
      erase_after(&head, 0);
  }

public:
  // Moves the range [before_first + 1, before_last + 1) to *this,
  //  inserting it immediately after pos.  This is constant time.
  void splice_after(iterator pos, 
                    iterator before_first, iterator before_last)
  {
      if (before_first != before_last) {
          __slist_splice_after(pos.node, before_first.node, before_last.node);
          __stl_debug_do(before_first++;
                         before_last++;
                         __invalidate_range(before_first.owner(), 
                                            before_first, before_last));
      }
  }

  // Moves the element that follows prev to *this, inserting it immediately
  //  after pos.  This is constant time.
  void splice_after(iterator pos, iterator prev)
  {
    __slist_splice_after(pos.node, prev.node, prev.node->next);
    __stl_debug_do(++prev;__invalidate_iterator(prev.owner(), prev.node, prev));
  }


  // Linear in distance(begin(), pos), and linear in L.size().
  void splice(iterator pos, slist<T,Alloc>& L) {
    __stl_verbose_assert(&L!=this, __STL_MSG_INVALID_ARGUMENT);
    __stl_debug_check(__check_if_owner(&iter_list,pos));
    if (L.head.next)
      __slist_splice_after(__slist_previous(&head, pos.node),
                           &L.head,
                           __slist_previous(&L.head, 0));
    __stl_debug_do(L.invalidate_all());
  }

  // Linear in distance(begin(), pos), and in distance(L.begin(), i).
  void splice(iterator pos, slist<T,Alloc>& L, iterator i) {
    __stl_verbose_assert(&L!=this, __STL_MSG_INVALID_ARGUMENT);
    __stl_debug_check(__check_if_owner(&iter_list,pos) && __check_if_owner(&L.iter_list ,i));
    __slist_splice_after(__slist_previous(&head, pos.node),
                         __slist_previous(&L.head, i.node),
                         i.node);
    __stl_debug_do(L.invalidate_iterator(i));
  }

  // Linear in distance(begin(), pos), in distance(L.begin(), first),
  // and in distance(first, last).
  void splice(iterator pos, slist<T,Alloc>& L, iterator first, iterator last)
  {
    __stl_verbose_assert(&L!=this, __STL_MSG_INVALID_ARGUMENT);
    __stl_debug_check(__check_if_owner(&iter_list,pos));
    if (first != last)
      __slist_splice_after(__slist_previous(&head, pos.node),
                           __slist_previous(&L.head, first.node),
                           __slist_previous(first.node, last.node));
    __stl_debug_do(__invalidate_range(&L.iter_list, first, last));
  }

public:
  void reverse() { if (head.next) head.next = __slist_reverse(head.next); }

  void remove(const T& val); 
  void unique(); 
  void merge(self& L);
  void sort();     

#ifdef __STL_MEMBER_TEMPLATES
  template <class Predicate> void remove_if(Predicate pred);
  template <class BinaryPredicate> void unique(BinaryPredicate pred); 
  template <class StrictWeakOrdering> void merge(slist&, StrictWeakOrdering); 
  template <class StrictWeakOrdering> void sort(StrictWeakOrdering comp); 
#endif /* __STL_MEMBER_TEMPLATES */
};

template <class T, class Alloc>
inline bool operator<(const slist<T, Alloc>& L1, const slist<T, Alloc>& L2)
{
  return lexicographical_compare(L1.begin(), L1.end(), L2.begin(), L2.end());
}


# if defined (__STL_NESTED_TYPE_PARAM_BUG) 
#  define iterator       __slist_iterator<T,T&,T*>
#  define const_iterator __slist_iterator<T, const T&, const T*>
#  define size_type      size_t
# endif


template <class T, class Alloc>
slist<T, Alloc>& slist<T,Alloc>::operator=(const slist<T, Alloc>& L)
{
  if (&L != this) {
    list_node_base* p1 = &head;
    list_node* n1 = (list_node*) head.next;
    const list_node* n2 = (const list_node*) L.head.next;
    while (n1 && n2) {
      n1->data = n2->data;
      p1 = n1;
      n1 = (list_node*) n1->next;
      n2 = (const list_node*) n2->next;
    }
    if (n2 == 0)
      erase_after(p1, 0);
    else
      _insert_after_range(p1,
                          make_const_iterator((list_node*)n2), make_const_iterator(0));
    __stl_debug_do(invalidate_all());
  }
  return *this;
} 

template <class T, class Alloc>
bool operator==(const slist<T, Alloc>& L1, const slist<T, Alloc>& L2)
{
  typedef typename slist<T,Alloc>::list_node list_node;
  list_node* n1 = (list_node*) L1.head.next;
  list_node* n2 = (list_node*) L2.head.next;
  while (n1 && n2 && n1->data == n2->data) {
    n1 = (list_node*) n1->next;
    n2 = (list_node*) n2->next;
  }
  return n1 == 0 && n2 == 0;
}

template <class T, class Alloc>
void slist<T, Alloc>::resize(size_type len, const T& x)
{
  list_node_base* cur = &head;
  while (cur->next != 0 && len > 0) {
    --len;
    cur = cur->next;
  }
  if (cur->next) 
    erase_after(cur, 0);
  else
    _insert_after_fill(cur, len, x);
}

template <class T, class Alloc>
void slist<T,Alloc>::remove(const T& val)
{
  list_node_base* cur = &head;
  while (cur && cur->next) {
    if (((list_node*) cur->next)->data == val)
      erase_after(cur);
    else
      cur = cur->next;
  }
}

template <class T, class Alloc> 
void slist<T,Alloc>::unique()
{
  list_node_base* cur = head.next;
  if (cur) {
    while (cur->next) {
      if (((list_node*)cur)->data == ((list_node*)(cur->next))->data)
        erase_after(cur);
      else
        cur = cur->next;
    }
  }
}

template <class T, class Alloc>
void slist<T,Alloc>::merge(slist<T,Alloc>& L)
{
  list_node_base* n1 = &head;
  while (n1->next && L.head.next) {
    if (((list_node*) L.head.next)->data < ((list_node*) n1->next)->data) 
      __slist_splice_after(n1, &L.head, L.head.next);
    n1 = n1->next;
  }
  if (L.head.next) {
    n1->next = L.head.next;
    L.head.next = 0;
  }
  __stl_debug_do(L.invalidate_all());
}

template <class T, class Alloc>
void slist<T,Alloc>::sort()
{
  if (head.next && head.next->next) {
    slist<T,Alloc> carry;
    slist<T,Alloc> counter[64];
    int fill = 0;
    while (!empty()) {
      __slist_splice_after(&carry.head, &head, head.next);
      int i = 0;
      while (i < fill && !counter[i].empty()) {
        counter[i].merge(carry);
        carry.swap(counter[i]);
        ++i;
      }
      carry.swap(counter[i]);
      if (i == fill)
        ++fill;
    }

    for (int i = 1; i < fill; ++i)
      counter[i].merge(counter[i-1]);
    this->swap(counter[fill-1]);
  }
}

#ifdef __STL_MEMBER_TEMPLATES

template <class T, class Alloc> 
template <class Predicate> void slist<T,Alloc>::remove_if(Predicate pred)
{
  list_node_base* cur = &head;
  while (cur->next) {
    if (pred(((list_node*) cur->next)->data))
      erase_after(cur);
    else
      cur = cur->next;
  }
}

template <class T, class Alloc> template <class BinaryPredicate> 
void slist<T,Alloc>::unique(BinaryPredicate pred)
{
  list_node* cur = (list_node*) head.next;
  if (cur) {
    while (cur->next) {
      if (pred(((list_node*)cur)->data, ((list_node*)(cur->next))->data))
        erase_after(cur);
      else
        cur = (list_node*) cur->next;
    }
  }
}

template <class T, class Alloc> template <class StrictWeakOrdering>
void slist<T,Alloc>::merge(slist<T,Alloc>& L, StrictWeakOrdering comp)
{
  list_node_base* n1 = &head;
  while (n1->next && L.head.next) {
    if (comp(((list_node*) L.head.next)->data,
             ((list_node*) n1->next)->data))
      __slist_splice_after(n1, &L.head, L.head.next);
    n1 = n1->next;
  }
  if (L.head.next) {
    n1->next = L.head.next;
    L.head.next = 0;
  }
}

template <class T, class Alloc> template <class StrictWeakOrdering> 
void slist<T,Alloc>::sort(StrictWeakOrdering comp)
{
  if (head.next && head.next->next) {
    slist<T,Alloc> carry;
    slist<T,Alloc> counter[64];
    int fill = 0;
    while (!empty()) {
      __slist_splice_after(&carry.head, &head, head.next);
      int i = 0;
      while (i < fill && !counter[i].empty()) {
        counter[i].merge(carry, comp);
        carry.swap(counter[i]);
        ++i;
      }
      carry.swap(counter[i]);
      if (i == fill)
        ++fill;
    }

    for (int i = 1; i < fill; ++i)
      counter[i].merge(counter[i-1], comp);
    this->swap(counter[fill-1]);
  }
}

#endif /* __STL_MEMBER_TEMPLATES */

# if defined ( __STL_NESTED_TYPE_PARAM_BUG )
#  undef  iterator
#  undef  const_iterator
#  undef  size_type
# endif

// do a cleanup
#  undef slist

#  define __slist__ __FULL_NAME(slist)

# if defined (__STL_FUNC_PARTIAL_ORDERING )
template <class T, class Alloc>
inline void swap(__slist__<T,Alloc>& a, __slist__<T,Alloc>& b) { a.swap(b); }
# endif

# if !defined ( __STL_DEFAULT_TYPE_PARAM )
// provide a "default" list adaptor
template <class T>
class slist : public __slist__<T,alloc>
{
    typedef slist<T> self;
public:
    typedef __slist__<T,alloc> super;
    __IMPORT_CONTAINER_TYPEDEFS(super)
    __IMPORT_ITERATORS(super)
    __IMPORT_SUPER_COPY_ASSIGNMENT(slist)
    slist() { }
    explicit slist(size_type n, const T& value) : super(n, value) { }
    explicit slist(size_type n) :  super(n) { } 
    slist(const T* first, const T* last) : super(first, last) { } 
    slist(const_iterator first, const_iterator last) : super(first, last) { }
};

#  if defined (__STL_BASE_MATCH_BUG)
template <class T>
inline bool operator==(const slist<T>& x, const slist<T>& y) {
    typedef typename slist<T>::super super;
    return operator == ((const super&)x,(const super&)y);
}

template <class T>
inline bool operator<(const slist<T>& x, const slist<T>& y) {
    typedef typename slist<T>::super super;
    return operator < ((const super&)x,(const super&)y);
}
#  endif
# endif /*  __STL_DEFAULT_TEMPLATE_PARAM */

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#endif

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_SLIST_H */

// Local Variables:
// mode:C++
// End:
