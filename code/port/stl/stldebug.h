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
#ifndef __SGI_STL_STLDEBUG_H
# define __SGI_STL_STLDEBUG_H

#if defined ( __STL_DEBUG )

// stderr
# include <stdio.h>
# include <stdlib.h>
# ifndef __SGI_STL_ITERATOR_H
# include <iterator.h>
# endif

# ifndef _NOTHREADS
#  include <stl_lock.h>
# endif

__STL_BEGIN_NAMESPACE

# ifndef __VOLATILE
#  define __VOLATILE
# endif
# ifndef __RESTRICT
#  define __RESTRICT
# endif

// syncronization 
# ifdef _NOTHREADS
#  define __SYNCHRONIZE(g,l)
# else
#  define __SYNCHRONIZE(g,l) __stl_guard g(l)
# endif

// uninitialized value filler
# ifndef __STL_SHRED_BYTE
#  define __STL_SHRED_BYTE 0xFF
# endif

# define __STL_MSG_INVALID_ARGUMENT     "Invalid argument to operation (see operation documentation)"

# define __STL_MSG_INVALID_CONTAINER    "Taking an iterator out of destroyed (or otherwise corrupted) container"
# define __STL_MSG_EMPTY_CONTAINER      "Trying to extract an object out from empty container"
# define __STL_MSG_ERASE_PAST_THE_END   "Past-the-end iterator could not be erased"
# define __STL_MSG_OUT_OF_BOUNDS        "Index out of bounds"
# define __STL_MSG_NOT_OWNER            "Container doesn't own the iterator"
# define __STL_MSG_INVALID_ITERATOR     "Uninitialized or invalidated (by mutating operation)"\
       " iterator used"
# define __STL_MSG_INVALID_LEFTHAND_ITERATOR     "Uninitialized or invalidated (by mutating operation)"\
       " lefthand iterator in expression"
# define __STL_MSG_INVALID_RIGHTHAND_ITERATOR     "Uninitialized or invalidated (by mutating operation)"\
       " righthand iterator in expression"
# define __STL_MSG_DIFFERENT_OWNERS     "Iterators used in expression are from different owners"
# define __STL_MSG_NOT_DEREFERENCEABLE  "Iterator could not be dereferenced (past-the-end ?)"
# define __STL_MSG_INVALID_RANGE        "Range [first,last) is invalid"
# define __STL_MSG_NOT_IN_RANGE_1       "Iterator is not in range [first,last)"
# define __STL_MSG_NOT_IN_RANGE_2       "Range [first,last) is not in range [start,finish)"
# define __STL_MSG_INVALID_ADVANCE      "The advance would produce invalid iterator"
# define __STL_MSG_SINGULAR_ITERATOR    "Iterator is singular (advanced beyond the bounds ?)"

// debug alloc messages
# define __STL_MSG_DBA_DELETED_TWICE    "Memory block deallocated twice"
# define __STL_MSG_DBA_NEVER_ALLOCATED  "Deallocating a block that was never allocated"
# define __STL_MSG_DBA_TYPE_MISMATCH    "Deallocating a memory block allocated for another type"
# define __STL_MSG_DBA_SIZE_MISMATCH    "Size of block passed to deallocate() doesn't match block size"
# define __STL_MSG_DBA_UNDERRUN         "Pointer underrun - safety margin at front of memory block overwritten"
# define __STL_MSG_DBA_OVERRUN          "Pointer overrrun - safety margin at back of memory block overwritten"

#  define __stl_verbose_check(expr,diagnostic) ((expr) ? true : \
(__stl_error_report( __FILE__, __LINE__, diagnostic ),false))

#  define __stl_verbose_return(expr,diagnostic) if (!(expr)) { \
    __stl_error_report( __FILE__, __LINE__, diagnostic ); return false; }

//============================================================================

// debug error reporting function.
// The advantage of it is that one could set a breakpoint on it
// Suggested by  Max Moroz <mmoroz@ucla.edu>
inline void __stl_error_report(const char* file, int line, 
                               const char* diagnostic) {
    fprintf(stderr, "%s:%d STL error : %s\n", file, line, diagnostic); 
}

inline bool __check_bounds(int index, int size) 
{
    __stl_verbose_return(index<size, __STL_MSG_OUT_OF_BOUNDS);
    return true;
}

//=============================================================
template <class Iterator>
inline bool __valid_range(const Iterator& i1 ,const Iterator& i2, 
                          random_access_iterator_tag)
{ 
    return i1<=i2;
}

# if 1 /* defined (__STL_BASE_MATCH_BUG) OBSOLETE by inheritance */
template <class Iterator>
inline bool __valid_range(const Iterator& i1 ,const Iterator& i2, 
                          bidirectional_iterator_tag)
{ 
    // check if comparable
    bool dummy(i1==i2);
    return (dummy==dummy); 
}
# endif

template <class Iterator>
inline bool __valid_range(const Iterator& i1 ,const Iterator& i2, 
                          forward_iterator_tag)
{ 
    // check if comparable
    bool dummy(i1==i2);
    return (dummy==dummy); 
}

template <class Iterator>
inline bool __valid_range(const Iterator&,const Iterator&, 
                          input_iterator_tag)
{ 
    return false; 
}

template <class Iterator>
inline bool __valid_range(const Iterator&,const Iterator&, 
                          output_iterator_tag)
{ 
    return false; 
}

template <class Iterator>
inline bool __valid_range(const Iterator& i1, const Iterator& i2)
{ 
    return __valid_range(i1,i2,iterator_category(i1));
}

// Note : that means in range [i1, i2].
template <class Iterator>
inline bool __in_range(const Iterator& it, const Iterator& i1, const Iterator& i2)
{ 
    return __valid_range(i1,it,iterator_category(i1)) && 
        __valid_range(it,i2,iterator_category(it));
}

template <class Iterator>
inline bool __in_range(const Iterator& first, const Iterator& last, 
                       const Iterator& start, const Iterator& finish)
{ 
    return __valid_range(first,last,iterator_category(first)) &&
        __valid_range(start,first,iterator_category(first)) && 
        __valid_range(last,finish,iterator_category(last));
}

//==========================================================

class __safe_server;

class __safe_base {
private:
    typedef __safe_base self;
    typedef self* self_ptr;
protected:
    typedef __safe_base* base_ptr;
    typedef void* owner_ptr;
public:
    __safe_base(const void* c) : 
        owner_(owner_ptr(c)) {
            attach();
        }
    const __safe_server* owner() const { return (const __safe_server*)owner_; }
    __safe_server* owner() { return (__safe_server*)owner_; }
    bool  valid() const               { return owner()!=0; }
    __safe_base(const self& rhs) : owner_(rhs.owner_) {
        attach();
    }
    self& operator=(const self& rhs) {
        if ( owner_ != rhs.owner_ ) {
            detach();
            owner_ = rhs.owner_;
            attach();
        }
        return *this;
    }
    ~__safe_base() {
        detach();
        owner_  = 0;
    }
    void invalidate() { owner_=0; }
    self* prev() { return prev_; }
    self* next() { return next_; }
    __safe_base() : owner_(0) {}
    inline void attach();
    inline void detach();
    mutable owner_ptr         owner_;
// protected:
    mutable base_ptr          next_;
    mutable base_ptr          prev_; 
    friend class __safe_server;
};


//==========================================================

class __safe_server : public __safe_base {
private:
    typedef __safe_server self;
public:
    __safe_server() {}
    __safe_server(const self&) {
    }
    self& operator=(const self&) {
      return *this;
    }
    ~__safe_server() {
      // that prevents detach
        owner_  = 0;
    }
    void safe_init(const void* c) { 
        owner_=owner_ptr(c); 
        next_=prev_=this;
    }
    void verify() {
        __safe_base* position;
        for (position = next(); 
             position!=this; position=position->next()) {
            __stl_assert(position->owner_==this || position->owner_==owner_);
        }
    }
    inline void swap_owners(self& x, bool swap_roots=false);
    void invalidate_all() {
      if (owner_) {
        for (__safe_base* position = next(); 
             position!=this; position=position->next()) {
            position->invalidate();
        }
      }
      next_=prev_=this;
    }
private:
# ifndef _NOTHREADS
    __stl_lock lock_;
# endif
    friend class __safe_base;
};

inline void 
__safe_server::swap_owners(__safe_server& x, bool swap_roots) {
  invalidate_all();
  x.invalidate_all();
  if (swap_roots) {
    owner_ptr tmp(owner_);
    owner_=x.owner_;
    x.owner_=tmp;
  }
}

//============================================================
inline void 
__safe_base::attach() {
   __safe_server __VOLATILE *  owner_node = owner();
   if (owner_node) {
    __stl_verbose_assert(owner_node->owner_!=0, 
			 __STL_MSG_INVALID_CONTAINER);
    __SYNCHRONIZE(guard,owner()->lock_);
    self* list_begin = owner_node->next_;
    next_=list_begin;
    prev_ = list_begin->prev_;
    (list_begin->prev_)->next_=this;
    list_begin->prev_=this;
  }   
}

inline void 
__safe_base::detach() {
  __safe_server*  owner_node = owner();
  if (owner_node) {
    __SYNCHRONIZE(guard,owner_node->lock_);
    prev_->next_ = next_ ;
    next_->prev_ = prev_ ;
    owner_=0;
  }
}


//============================================================
    
// forward declaratioins
template <class Iterator>
bool __check_range(const Iterator& first, const Iterator& last);
template <class Iterator>
bool __check_range(const Iterator& it, 
                   const Iterator& start, const Iterator& finish);
template <class Iterator>
bool __check_range(const Iterator& first, const Iterator& last, 
                   const Iterator& start, const Iterator& finish);
template <class Iterator>
void __invalidate_range(__safe_server* base, 
                        const Iterator& first,
                        const Iterator& last);
template <class Iterator, class SafeIterator>
void __invalidate_iterator(__safe_server* base, 
                           const Iterator& it, const SafeIterator&);
template <class Iterator>
bool __check_same_owner( const Iterator& i1, const Iterator& i2);
template <class Pointer, class Iterator>
bool __check_if_owner( const Pointer* owner, const Iterator& it);

// [ i1, i2)
template <class Iterator>
inline bool __in_range_aux(const Iterator& it, const Iterator& first,
                           const Iterator& last, random_access_iterator_tag) {
    return ( it.get_iterator() >= first.get_iterator() && 
             it.get_iterator() < last.get_iterator() );
}

template <class Iterator1, class Iterator>
# if defined (_MSC_VER) && (_MSC_VER >= 1100)
inline bool __in_range_aux(Iterator1 it, const Iterator& first,
# else
inline bool __in_range_aux(const Iterator1& it, const Iterator& first,
# endif
                           const Iterator& last, forward_iterator_tag) {
    Iterator1 i(first);
    for (;  i != last && i.get_iterator() != it.get_iterator(); ++i);
    return (i.get_iterator()!=last.get_iterator());
}

# if defined (__STL_NONTEMPL_BASE_MATCH_BUG) /* OBSOLETE by inheritance */
template <class Iterator1, class Iterator>
inline bool __in_range_aux(const Iterator1& it, const Iterator& first,
                           const Iterator& last, bidirectional_iterator_tag) {
    Iterator1 i(first);
    for (;  i != last && i.get_iterator() != it.get_iterator(); ++i);
    return (i.get_iterator()!=last.get_iterator());
}
# endif

//============================================================

//=============================================================

template <class Iterator>
bool __check_range(const Iterator& first, const Iterator& last) {
    __stl_verbose_return(__valid_range(first,last), __STL_MSG_INVALID_RANGE );
    return true;
}

template <class Iterator>
bool __check_range(const Iterator& it, 
                   const Iterator& start, const Iterator& finish) {
    __stl_verbose_return(__in_range(it,start, finish), 
                         __STL_MSG_NOT_IN_RANGE_1);
    return true;
}

template <class Iterator>
bool __check_range(const Iterator& first, const Iterator& last, 
                   const Iterator& start, const Iterator& finish) {
    __stl_verbose_return(__in_range(first, last, start, finish), 
                         __STL_MSG_NOT_IN_RANGE_2);
    return true;
}
//===============================================================

template <class Iterator>
void __invalidate_range(__safe_server* base, 
                        const Iterator& first,
                        const Iterator& last)
{
    typedef __safe_base* base_ptr;
    typedef Iterator* safe_iterator_ptr;
    base_ptr list_root = base;
	base_ptr pos;
	safe_iterator_ptr position;
    for (pos= list_root->next(); 
         pos!=list_root;) {	    
        position=(safe_iterator_ptr)pos;
        if ((!(&first == position || &last == position))
            &&(__in_range_aux(*position,
                              first,last, iterator_category(first)))) {
                base_ptr remove_position=pos;
                pos=pos->next();
                remove_position->detach();
            }
            else
                pos=pos->next();
    }
    
}

template <class Iterator, class SafeIterator>
void __invalidate_iterator(__safe_server* base, 
                           const Iterator& it, const SafeIterator&)
{
    typedef __safe_base* base_ptr;
    typedef SafeIterator* safe_iterator_ptr;
    base_ptr list_root = base;
    for (base_ptr position = list_root->next(); 
         position!=list_root;) {
        if (((safe_iterator_ptr)position)->get_iterator()==it) {
            base_ptr remove_position=position;
            position=position->next();
            remove_position->detach();
        }
        else
            position=position->next();
    }
}

template <class Iterator>
bool __check_same_owner( const Iterator& i1, const Iterator& i2)
{
    __stl_verbose_return(i1.valid(), __STL_MSG_INVALID_LEFTHAND_ITERATOR);
    __stl_verbose_return(i2.valid(), __STL_MSG_INVALID_RIGHTHAND_ITERATOR);
    __stl_verbose_return(i1.owner()==i2.owner(), __STL_MSG_DIFFERENT_OWNERS);
    return true;
}

template <class Pointer, class Iterator>
bool __check_if_owner( const Pointer* owner, const Iterator& it)
{
    const void* owner_ptr = it.owner();
    __stl_verbose_return(owner_ptr!=0, __STL_MSG_INVALID_ITERATOR);
    __stl_verbose_return((const void*)owner==owner_ptr, __STL_MSG_NOT_OWNER);
    return true;
}

//============================================================

__STL_END_NAMESPACE

# endif /* __STL_DEBUG */

#endif /* DEBUG_H */



