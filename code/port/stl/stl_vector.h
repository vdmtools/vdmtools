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

#ifndef __SGI_STL_INTERNAL_VECTOR_H
#define __SGI_STL_INTERNAL_VECTOR_H


#  define vector __WORKAROUND_RENAME(vector)

# if defined ( __STL_NESTED_TYPE_PARAM_BUG )
#  define __pointer__             T*
#  define __const_pointer__       const T*
#  define __size_type__           size_t
#  define __difference_type__     ptrdiff_t
# else
#  define __pointer__         pointer
#  define __const_pointer__   const_pointer
#  define __size_type__       size_type
#  define __difference_type__ difference_type
# endif

# if defined (__STL_DEBUG)
#  define __iterator__       __vector_iterator<T,Alloc, T&, T*>
#  define __const_iterator__ __vector_iterator<T,Alloc,const T&, const T*>
#  define __ptr(x) x.get_iterator()  
# else
#  define __iterator__       __pointer__
#  define __const_iterator__ __const_pointer__  
#  define __ptr(x) x  
# endif

__STL_BEGIN_NAMESPACE

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#endif

# if defined ( __STL_USE_ABBREVS )
#  define  __vector_iterator       _V__It
# endif

# if defined (__STL_DEBUG)
template <class T, class Alloc, class Ref, class Ptr>
struct __vector_iterator : public __safe_base {
private:
    typedef __vector_iterator<T,Alloc,Ref, Ptr> self;
    typedef __vector_iterator<T,Alloc,T&, T*> iterator;
    typedef __vector_iterator<T,Alloc,const T&, const T*> const_iterator;
    typedef __safe_base super;
public:
    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef ptrdiff_t difference_type;
    typedef random_access_iterator_tag iterator_category;
    pointer iterator_;
public:
    __vector_iterator() : __safe_base(0)  {}
    __vector_iterator(const super* c, pointer it) :
        __safe_base(c), iterator_(it) {}
    __vector_iterator(const iterator& it) :
        __safe_base(it), iterator_(it.get_iterator()) {}
    ~__vector_iterator() {}
    pointer  get_iterator() const { return iterator_; }
    bool dereferenceable() const;
    bool nonsingular(const __safe_server* o = 0) const;

    reference operator*() const {
        __stl_debug_check(dereferenceable());
        return *iterator_;
    }
#ifndef __SGI_STL_NO_ARROW_OPERATOR
    pointer operator->() const { return &(operator*()); }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */
    self& operator++() {
        ++iterator_;
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        ++iterator_;
        return tmp;
    }
    self& operator--() {
        --iterator_;
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        --iterator_;
        return tmp;
    }
    difference_type operator-(const self& y ) const {
    __stl_debug_check(nonsingular(y.owner()) && y.nonsingular(owner()));
    return get_iterator()-y.get_iterator();
    }
    self& operator+=(difference_type n) {
        iterator_+=n;
        return *this;
    }
    self& operator-=(difference_type n) {
    return *this+=-n;
    }
    self operator+(difference_type n) const {
        self tmp(*this);
        return tmp += n;
    }
    self operator-(difference_type n) const {
        self tmp(*this);
        return tmp -= n;
    }
    bool operator==(const self& y ) const {
        __stl_debug_check(nonsingular(y.owner()) && y.nonsingular(owner()));
        return get_iterator()==y.get_iterator();
    }
    bool operator!=(const self& y ) const {
        __stl_debug_check(nonsingular(y.owner()) && y.nonsingular(owner()));
        return get_iterator()!=y.get_iterator();
    }
    bool operator<(const self& y ) const {
        __stl_debug_check(nonsingular(y.owner()) && y.nonsingular(owner()));
        return get_iterator()<y.get_iterator();
    }
    reference operator[](difference_type n) const { return *(*this + n); }
};

template <class T, class Alloc, class Ref, class Ptr>
inline __vector_iterator<T,Alloc, Ref, Ptr> 
operator+(ptrdiff_t n, const __vector_iterator<T,Alloc, Ref, Ptr>& it) {
    __vector_iterator<T,Alloc, Ref, Ptr> tmp(it);
    return tmp += n;
}

# if !defined (__STL_CLASS_PARTIAL_SPECIALIZATION)
template <class T, class Alloc, class Ref, class Ptr>
inline T* 
value_type(const  __vector_iterator<T,Alloc, Ref, Ptr>&) { return (T*) 0; }
template <class T, class Alloc, class Ref, class Ptr>
inline ptrdiff_t* 
distance_type(const  __vector_iterator<T,Alloc, Ref, Ptr>&) { return (ptrdiff_t*) 0; }
template <class T, class Alloc, class Ref, class Ptr>
inline random_access_iterator_tag
iterator_category(const __vector_iterator<T,Alloc, Ref, Ptr>&) { 
    return random_access_iterator_tag();
}
#  endif
# endif

template <class T, __DFL_TYPE_PARAM(Alloc,alloc)>
# if defined (__STL_DEBUG)
class vector : public __safe_server {
# else
class vector {
# endif
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
# if defined (__STL_DEBUG)
    typedef __vector_iterator<T,Alloc,reference, pointer> iterator;
    typedef __vector_iterator<T,Alloc,const_reference, const_pointer> const_iterator;
# else
    typedef value_type* iterator;
    typedef const value_type* const_iterator;
# endif
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
#if defined ( __STL_CLASS_PARTIAL_SPECIALIZATION ) && \
! defined (__STL_PARTIAL_SPECIALIZATION_BUG)
    typedef reverse_iterator<const_iterator> const_reverse_iterator;
    typedef reverse_iterator<iterator> reverse_iterator;
#else /* __STL_CLASS_PARTIAL_SPECIALIZATION */
# if defined (__STL_MSVC50_COMPATIBILITY)
    typedef reverse_iterator<const_iterator, value_type, const_reference, 
            const_pointer, difference_type>  const_reverse_iterator;
    typedef reverse_iterator<iterator, value_type, reference, pointer, difference_type>
    reverse_iterator;
# else
    typedef reverse_iterator<const_iterator, value_type, const_reference, 
                             difference_type>  const_reverse_iterator;
    typedef reverse_iterator<iterator, value_type, reference, difference_type>
        reverse_iterator;
# endif
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */
protected:
    typedef simple_alloc<value_type, Alloc> data_allocator;
    pointer start;
    pointer finish;
    pointer end_of_storage;
    void insert_aux(pointer position, const T& x);
    void deallocate() {
      __stl_debug_do(invalidate_all());
      if (start) data_allocator::deallocate(start, end_of_storage - start);
    }

    void fill_initialize(size_type n, const T& value) {
      start = allocate_and_fill(n, value);
      finish = start + n;
      end_of_storage = finish;
      __stl_debug_do(safe_init(this));
    }
public:
    pointer begin_() { return start; }
    const_pointer begin_() const { return start; }
    pointer end_() { return finish; }
    const_pointer end_() const { return finish; }
# if defined (__STL_DEBUG)
    iterator begin() { return iterator(this,start); }
    const_iterator begin() const { return const_iterator(this,start); }
    iterator end() { return iterator(this,finish); }
    const_iterator end() const { return const_iterator(this,finish); }
    void invalidate_range(const iterator& first, const iterator& last) {
        __invalidate_range(this, first, last);
    }
# else
    iterator begin() { return start; }
    const_iterator begin() const { return start; }
    iterator end() { return finish; }
    const_iterator end() const { return finish; }
# endif
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { 
        return const_reverse_iterator(end()); 
    }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { 
        return const_reverse_iterator(begin()); 
    }
    size_type size() const { return size_type(end_() - begin_()); }
    size_type max_size() const { return size_type(-1) / sizeof(T); }
    size_type capacity() const { return size_type(end_of_storage - begin_()); }
    bool empty() const { return begin_() == end_(); }
    reference operator[](size_type n) { return *(begin() + n); }
    const_reference operator[](size_type n) const { return *(begin() + n); }

    vector() : start(0), finish(0), end_of_storage(0) {
          __stl_debug_do(safe_init(this));
    }
    vector(size_type n, const T& value) { fill_initialize(n, value); }
    vector(int n, const T& value) { fill_initialize(n, value); }
    vector(long n, const T& value) { fill_initialize(n, value); }
    explicit vector(size_type n) { fill_initialize(n, T()); }

    vector(const vector<T, Alloc>& x) {
      start = allocate_and_copy(x.end_() - x.begin_(), x.begin_(), x.end_());
      finish = start + x.size();
      end_of_storage = finish;
      __stl_debug_do(safe_init(this));
    }
#ifdef __STL_MEMBER_TEMPLATES
    template <class InputIterator>
    vector(InputIterator first, InputIterator last) :
      start(0), finish(0), end_of_storage(0) {
        range_initialize(first, last, iterator_category(first));
    }
#else /* __STL_MEMBER_TEMPLATES */
    vector(const_iterator first, const_iterator last) {
      __stl_debug_check(__check_range(first,last));
      __stl_debug_do(safe_init(this));
      size_type n = last - first;
      start = allocate_and_copy(n, __ptr(first), __ptr(last));
      finish = start + n;
      end_of_storage = finish;
    }
# if defined ( __STL_DEBUG )
    vector(const_pointer first, const_pointer last) {
      __stl_debug_check(__check_range(first,last));
      __stl_debug_do(safe_init(this));
      size_type n = last - first;
      start = allocate_and_copy(n, first, last);
      finish = start + n;
      end_of_storage = finish;
    }
# endif /* __STL_DEBUG */
#endif /* __STL_MEMBER_TEMPLATES */
    ~vector() { 
	destroy(start, finish);
	deallocate();
        __stl_debug_do(__safe_server::invalidate());
    }
    vector<T, Alloc>& operator=(const vector<T, Alloc>& x);
    void reserve(size_type n) {
      if (capacity() < n) {
        const size_type old_size = size();
        pointer tmp = allocate_and_copy(n, start, finish);
        destroy(start, finish);
        deallocate();
        start = tmp;
        finish = tmp + old_size;
        end_of_storage = start + n;
      }
    }
    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }
    reference back() { return *(end() - 1); }
    const_reference back() const { return *(end() - 1); }
    void push_back(const T& x) {
	if (finish != end_of_storage) {
	    construct(finish, x);
	    ++finish;
	} else
	    insert_aux(end_(), x);
    }
    void swap(vector<T, Alloc>& x) {
        __stl_debug_do(swap_owners(x));
	__STD::swap(start, x.start);
	__STD::swap(finish, x.finish);
	__STD::swap(end_of_storage, x.end_of_storage);
    }
    iterator insert(iterator position, const T& x) {
        __stl_debug_check(position.nonsingular(this));
	size_type n = __ptr(position) - begin_();
	if (finish != end_of_storage && __ptr(position) == end_()) {
	    construct(finish, x);
	    ++finish;
	} else
	    insert_aux(__ptr(position), x);
	return begin() + n;
    }
    iterator insert(iterator position) { return insert(position, T()); }
#ifdef __STL_MEMBER_TEMPLATES
    template <class InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last) {
      range_insert(position, first, last, iterator_category(first));
    }
#else /* __STL_MEMBER_TEMPLATES */
    void insert(iterator position,
                const_iterator first, const_iterator last)
# if defined ( __STL_DEBUG )
    {    
        __stl_debug_check(__check_range(first,last));
        insert(position, __ptr(first), __ptr(last));
    }
    void insert (iterator position, const_pointer first, 
                 const_pointer last);
# else
    ;
# endif /* __STL_DEBUG */
#endif /* __STL_MEMBER_TEMPLATES */

    void insert (iterator pos, size_type n, const T& x);
    void insert (iterator pos, int n, const T& x) {
      insert(pos, (size_type) n, x);
    }
    void insert (iterator pos, long n, const T& x) {
      insert(pos, (size_type) n, x);
    }

    void pop_back() {
        __stl_verbose_assert(!empty(), __STL_MSG_EMPTY_CONTAINER);
        --finish;
        __stl_debug_do(__invalidate_iterator(this,finish,iterator()));
        destroy(finish);
    }
    iterator erase(iterator position) {
        __stl_debug_check(position.nonsingular(this));
        __stl_verbose_assert(__ptr(position)!=finish,__STL_MSG_ERASE_PAST_THE_END);
	if (__ptr(position) + 1 != end_())
	    __array_copy(__ptr(position) + 1, finish, __ptr(position));
        __stl_debug_do(invalidate_range(position,end()));
	--finish;
	destroy(finish);
	return position;
    }
    iterator erase(iterator first, iterator last) {
        __stl_debug_check(__check_range(first,last,begin(), end()));
        pointer i = __array_copy(__ptr(last), finish, __ptr(first));
	destroy(i, finish);
        __stl_debug_do(invalidate_range(first,end()));
	finish = finish - (__ptr(last) - __ptr(first)); 
	return first;
    }
    void resize(size_type new_size, const T& x) {
      if (new_size < size()) 
        erase(begin() + new_size, end());
      else
        insert(end(), new_size - size(), x);
    }
    void resize(size_type new_size) { resize(new_size, T()); }
    void clear() { 
        erase(begin(), end()); 
        __stl_debug_do(invalidate_all());
    }

protected:
    pointer allocate_and_fill(size_type n, const T& x) {
      pointer result = data_allocator::allocate(n);
      __STL_TRY {
        uninitialized_fill_n(result, n, x);
      }
      __STL_UNWIND(data_allocator::deallocate(result, n));
      return result;
    }

#ifdef __STL_MEMBER_TEMPLATES
    template <class ForwardIterator>
    pointer allocate_and_copy(size_type n,
                               ForwardIterator first, ForwardIterator last) {
      pointer result = data_allocator::allocate(n);
      __STL_TRY {
        uninitialized_copy(first, last, result);
      }
      __STL_UNWIND(data_allocator::deallocate(result, n));
      return result;
    }
#else /* __STL_MEMBER_TEMPLATES */
    pointer __allocate_and_copy(size_type n,
                                const_pointer first, const_pointer last,
                                __false_type) {
      pointer result = data_allocator::allocate(n);
      __STL_TRY {
        __array_uninitialized_copy(first, last, result);
      }
      __STL_UNWIND(data_allocator::deallocate(result, n));
      return result;
    }
    pointer __allocate_and_copy(size_type n,
                                const_pointer first, const_pointer last,
                                __true_type) {
      pointer result = data_allocator::allocate(n);
      __array_uninitialized_copy(first, last, result);
      return result;
    }
    pointer allocate_and_copy(size_type n,
                              const_pointer first, const_pointer last) {
# ifdef _MSC_VER
        return __allocate_and_copy(n,first,last,
                                   __type_traits<value_type>::is_POD_type());
# else
        typedef typename __type_traits<value_type>::is_POD_type is_POD_type;
        return __allocate_and_copy(n,first,last,is_POD_type());
# endif
    }
#endif /* __STL_MEMBER_TEMPLATES */


#ifdef __STL_MEMBER_TEMPLATES
    template <class InputIterator>
    void range_initialize(InputIterator first, InputIterator last,
                          input_iterator_tag) {
      __stl_debug_check(__check_range(first,last));
      for ( ; first != last; ++first)
        push_back(*first);
      __stl_debug_do(safe_init(this));
    }

    // This function is only called by the constructor.  We have to worry
    //  about resource leaks, but not about maintaining invariants.
    template <class ForwardIterator>
    void range_initialize(ForwardIterator first, ForwardIterator last,
                          forward_iterator_tag) {
      __stl_debug_check(__check_range(first,last));
      size_type n = 0;
      distance(first, last, n);
      start = allocate_and_copy(n, first, last);
      finish = start + n;
      end_of_storage = finish;
      __stl_debug_do(safe_init(this));
    }

    template <class InputIterator>
    void range_insert(iterator pos,
                      InputIterator first, InputIterator last,
                      input_iterator_tag);

    template <class ForwardIterator>
    void range_insert(iterator pos,
                      ForwardIterator first, ForwardIterator last,
                      forward_iterator_tag);

#endif /* __STL_MEMBER_TEMPLATES */
};

# if defined (__STL_DEBUG)
template <class T,  class Alloc, class Ref, class Ptr>
bool __vector_iterator<T, Alloc, Ref, Ptr>::dereferenceable() const {
    typedef vector<T, Alloc> vec_type;
    const vec_type& vec = *((const vec_type*)owner());
    __stl_verbose_return(valid(), __STL_MSG_INVALID_ITERATOR);
    __stl_verbose_return(get_iterator() < vec.end_() && 
                         get_iterator() >= vec.begin_(),
                          __STL_MSG_NOT_DEREFERENCEABLE);    
    return true;
}

template <class T,  class Alloc, class Ref, class Ptr>
bool __vector_iterator<T, Alloc, Ref, Ptr>::nonsingular(const __safe_server* o) const {
    typedef vector<T, Alloc> vec_type;
    const vec_type& vec = *((const vec_type*)owner());
    if (o) {
        __stl_verbose_return(owner()==o, __STL_MSG_NOT_OWNER);
    } else {
        __stl_verbose_return(valid(), __STL_MSG_INVALID_ITERATOR);
    }
    __stl_verbose_return(get_iterator() <= vec.end_() && 
                         get_iterator() >= vec.begin_(),
                          __STL_MSG_SINGULAR_ITERATOR);    
    return true;
}

# endif /* __STL_DEBUG */

template <class T, class Alloc>
inline bool operator==(const vector<T, Alloc>& x, const vector<T, Alloc>& y) {
    return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <class T, class Alloc>
inline bool operator<(const vector<T, Alloc>& x, const vector<T, Alloc>& y) {
    return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}


template <class T, class Alloc>
vector<T, Alloc>& vector<T, Alloc>::operator=(const vector<T, Alloc>& x) {
  if (&x != this) {
    __stl_debug_do(invalidate_all());
    if (x.size() > capacity()) {
      pointer tmp = allocate_and_copy(x.end_() - x.begin_(),
                                             x.begin_(), x.end_());
      destroy(start, finish);
      deallocate();
      start = tmp;
      end_of_storage = start + (x.end_() - x.begin_());
    }
    else if (size() >= x.size()) {
      pointer i = __array_copy(x.start, x.finish, start);
      destroy(i, finish);
    }
    else {
      __array_copy(x.begin_(), x.begin_() + size(), start);
      __array_uninitialized_copy(x.begin_() + size(), x.end_(), finish);
    }
    finish = start + x.size();
  }
  return *this;
}

template <class T, class Alloc>
void vector<T, Alloc>::insert_aux(__pointer__ position, const T& x) {
  if (finish != end_of_storage) {
    construct(finish, *(finish - 1));
    ++finish;
    T x_copy = x;
    __array_copy_backward(position, finish - 2, finish - 1);
    *position = x_copy;
  }
  else {
    const size_type old_size = size();
    const size_type len = old_size != 0 ? 2 * old_size : 1;
    pointer new_start = data_allocator::allocate(len);
    pointer new_finish = new_start;
    __STL_TRY {
      new_finish = __array_uninitialized_copy(start, position, new_start);
      construct(new_finish, x);
      ++new_finish;
      new_finish = __array_uninitialized_copy(position, finish, new_finish);
    }
#       ifdef __STL_USE_EXCEPTIONS
    catch(...) {
      destroy(new_start, new_finish);
      data_allocator::deallocate(new_start, len);
      throw;
    }
#       endif /* __STL_USE_EXCEPTIONS */
    destroy(begin_(), end_());
    deallocate();
    start = new_start;
    finish = new_finish;
    end_of_storage = new_start + len;
  }
}

template <class T, class Alloc>
# if defined ( __STL_DEBUG )
void vector<T, Alloc>::insert(__iterator__ pos, __size_type__ n, const T& x) {
    __stl_debug_check(pos.nonsingular(this));
    pointer position=__ptr(pos);
# else
void vector<T, Alloc>::insert(__iterator__ position, __size_type__ n, const T& x) {
# endif
  if (n != 0) {
    if (size_type(end_of_storage - finish) >= n) {
      T x_copy = x;
      const size_type elems_after = finish - position;
      pointer old_finish = finish;
      if (elems_after > n) {
        __array_uninitialized_copy(finish - n, finish, finish);
        finish += n;
        __array_copy_backward(position, old_finish - n, old_finish);
        fill(position, position + n, x_copy);
      }
      else {
        uninitialized_fill_n(finish, n - elems_after, x_copy);
        finish += n - elems_after;
        __array_uninitialized_copy(position, old_finish, finish);
        finish += elems_after;
        fill(position, old_finish, x_copy);
      }
      __stl_debug_do(invalidate_range(pos,end()));
    }
    else {
      const size_type old_size = size();        
      const size_type len = old_size + max(old_size, n);
      pointer new_start = data_allocator::allocate(len);
      pointer new_finish = new_start;
      __STL_TRY {
        new_finish = __array_uninitialized_copy(start, position, new_start);
        new_finish = uninitialized_fill_n(new_finish, n, x);
        new_finish = __array_uninitialized_copy(position, finish, new_finish);
      }
#         ifdef __STL_USE_EXCEPTIONS
      catch(...) {
        destroy(new_start, new_finish);
        data_allocator::deallocate(new_start, len);
        throw;
      }
#         endif /* __STL_USE_EXCEPTIONS */
      destroy(start, finish);
      deallocate();
      start = new_start;
      finish = new_finish;
      end_of_storage = new_start + len;
    }
  }
}

#ifdef __STL_MEMBER_TEMPLATES

template <class T, class Alloc> template <class InputIterator>
void vector<T, Alloc>::range_insert(iterator pos,
                                    InputIterator first, InputIterator last,
                                    input_iterator_tag) {
  __stl_debug_check(__check_range(first,last));
  for ( ; first != last; ++first) {
    pos = insert(pos, *first);
    ++pos;
  }
}

template <class T, class Alloc> template <class ForwardIterator>
# ifdef __STL_DEBUG
void vector<T, Alloc>::range_insert(iterator pos,
# else
void vector<T, Alloc>::range_insert(iterator position,
# endif
                                    ForwardIterator first,
                                    ForwardIterator last,
                                    forward_iterator_tag) {
# ifdef __STL_DEBUG
  pointer position(__ptr(pos));
# endif
  if (first != last) {
    __stl_debug_check(__check_range(first,last));
    __stl_debug_check(pos.nonsingular(this));
    size_type n = 0;
    distance(first, last, n);
    if (size_type(end_of_storage - finish) >= n) {
      const size_type elems_after = finish - position;
      pointer old_finish = finish;
      if (elems_after > n) {
        uninitialized_copy(finish - n, finish, finish);
        finish += n;
        copy_backward(position, old_finish - n, old_finish);
        copy(first, last, position);
      }
      else {
        ForwardIterator mid = first;
        advance(mid, elems_after);
        uninitialized_copy(mid, last, finish);
        finish += n - elems_after;
        uninitialized_copy(position, old_finish, finish);
        finish += elems_after;
        copy(first, mid, position);
      }
      __stl_debug_do(invalidate_range(pos,end()));
    }
    else {
      const size_type old_size = size();
      const size_type len = old_size + max(old_size, n);
      pointer new_start = data_allocator::allocate(len);
      pointer new_finish = new_start;
      __STL_TRY {
        new_finish = uninitialized_copy(start, position, new_start);
        new_finish = uninitialized_copy(first, last, new_finish);
        new_finish = uninitialized_copy(position, finish, new_finish);
      }
#         ifdef __STL_USE_EXCEPTIONS
      catch(...) {
        destroy(new_start, new_finish);
        data_allocator::deallocate(new_start, len);
        throw;
      }
#         endif /* __STL_USE_EXCEPTIONS */
      destroy(start, finish);
      deallocate();
      start = new_start;
      finish = new_finish;
      end_of_storage = new_start + len;
    }
  }
}

#else /* __STL_MEMBER_TEMPLATES */

template <class T, class Alloc>
# if defined ( __STL_DEBUG )
void vector<T, Alloc>::insert(__iterator__ pos, __const_pointer__ first,
                              __const_pointer__ last) {
    __stl_debug_check(pos.nonsingular(this)
                      &&__check_range(first,last));
    pointer position=__ptr(pos);
# else
void vector<T, Alloc>::insert(__iterator__ position, 
                              __const_iterator__ first,
                              __const_iterator__ last) {
# endif
  if (first != last) {
    size_type n = 0;
    distance(first, last, n);
    if (size_type(end_of_storage - finish) >= n) {
      const size_type elems_after = finish - position;
      pointer old_finish = finish;
      if (elems_after > n) {
        __array_uninitialized_copy(finish - n, finish, finish);
        finish += n;
        __array_copy_backward(position, old_finish - n, old_finish);
        __array_copy(first, last, position);
      }
      else {
        __array_uninitialized_copy(first + elems_after, last, finish);
        finish += n - elems_after;
        __array_uninitialized_copy(position, old_finish, finish);
        finish += elems_after;
        __array_copy(first, first + elems_after, position);
      }
      __stl_debug_do(invalidate_range(pos,end()));
    }
    else {
      const size_type old_size = size();
      const size_type len = old_size + max(old_size, n);
      pointer new_start = data_allocator::allocate(len);
      pointer new_finish = new_start;
      __STL_TRY {
        new_finish = __array_uninitialized_copy(start, position, new_start);
        new_finish = (pointer)__array_uninitialized_copy(first, last, new_finish);
        new_finish = __array_uninitialized_copy(position, finish, new_finish);
      }
#         ifdef __STL_USE_EXCEPTIONS
      catch(...) {
        destroy(new_start, new_finish);
        data_allocator::deallocate(new_start, len);
        throw;
      }
#         endif /* __STL_USE_EXCEPTIONS */
      destroy(start, finish);
      deallocate();
      start = new_start;
      finish = new_finish;
      end_of_storage = new_start + len;
    }
  }
}

#endif /* __STL_MEMBER_TEMPLATES */

// do a cleanup
# undef  vector
# undef  __iterator__
# undef  __const_iterator__
# undef  __pointer__
# undef  __const_pointer__
# undef  __size_type__
# undef  __ptr
# undef  __difference_type__

// provide a uniform way to access full funclionality
#  define __vector__ __FULL_NAME(vector)

# if ! defined (__STL_DEFAULT_TYPE_PARAM)
// provide a "default" vector adaptor
template <class T>
class vector : public __vector__<T,alloc>
{
    typedef vector<T> self;
public:
    typedef __vector__<T,alloc> super;
    __CONTAINER_SUPER_TYPEDEFS
    __IMPORT_SUPER_COPY_ASSIGNMENT(vector)
    vector() {}
    explicit vector(size_type n, const T& value) : super(n, value) { }
    explicit vector(size_type n) : super(n) { }
# if defined ( __STL_DEBUG )
    vector(const value_type* first, const value_type* last) : super(first,last) { }
# endif
    vector(const_iterator first, const_iterator last) : super(first,last) { }
    ~vector() {}
};

#  if defined (__STL_BASE_MATCH_BUG)
template <class T>
    inline bool operator==(const vector<T>& x, const vector<T>& y) {
    typedef  __vector__<T,alloc> super;
    return operator == ((const super&)x,(const super&)y);
}

template <class T>
    inline bool operator<(const vector<T>& x, const vector<T>& y) {
    typedef  __vector__<T,alloc> super;
    return operator < ((const super&)x,(const super&)y);
}
#  endif /* __STL_BASE_MATCH_BUG */
# endif /* __STL_DEFAULT_TEMPLATE_PARAM */


# if defined (__STL_FUNC_PARTIAL_ORDERING )
template <class T, class Alloc>
    inline void swap(__vector__<T,Alloc>& a, __vector__<T,Alloc>& b) { a.swap(b); }
# endif

// close std namespace
__STL_END_NAMESPACE

#endif /* __SGI_STL_VECTOR_H */
