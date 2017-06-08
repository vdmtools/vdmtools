//  -*- Mode: C++ -*-
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

// The purpose of this file is to import streambuf iterators from
// Plum Hall standard library ( MSVC++ 5.0)

#ifndef __SGI_STL_PH_ITERATOR_H
# define __SGI_STL_PH_ITERATOR_H 
# include <iosfwd>
# define pair _ms_pair
# define make_pair _ms_make_pair
# define input_iterator_tag _ms_input_iterator_tag
# define output_iterator_tag _ms_output_iterator_tag
# define forward_iterator_tag _ms_forward_iterator_tag
# define bidirectional_iterator_tag _ms_bidirectional_iterator_tag
# define random_access_iterator_tag _ms_random_access_iterator_tag
# define iterator_traits _ms_iterator_traits
# define distance _ms_distance
# define reverse_iterator _ms_reverse_iterator
# define iterator _ms_iter
# define rel_ops  _ms_rel_ops

// That explicit inclusion should fit typical MSVC installation
# include <..\..\Vc98\Include\Utility>
//# include <Utility>
// IMPORTANT : use long path fitting your MSVC++ 5.x installation if
// running into problems. It's generally a bad idea to put SGI STL files in 
// "...\Include\" directory.
// # include <D:\Program Files\DevStudio\Vc\Include\Utility>

# undef pair 
# undef make_pair 
# undef input_iterator_tag 
# undef output_iterator_tag 
# undef forward_iterator_tag 
# undef bidirectional_iterator_tag 
# undef random_access_iterator_tag 
# undef iterator_traits 
# undef distance 
# undef reverse_iterator 
# undef iterator 
# undef rel_ops
# endif /* ___STL_PH_ITERATOR_H */
