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

#ifndef __SGI_STL_MSL_ITERATOR_H
# define __SGI_STL_MSL_ITERATOR_H
// fbp : the only purpose of this file is  to get 
// stream/streambuf iterators / global_iterator / iterator_trait 
// from MSL library. It is to be included from within adaptation <iterator.h>



// fbp : get stream /streambuf iterators / global_iterator / iterator_trait from MSL library
# define input_iterator_tag _msl_input_iterator_tag
# define output_iterator_tag _msl_output_iterator_tag
# define forward_iterator_tag _msl_forward_iterator_tag
# define bidirectional_iterator_tag _msl_bidirectional_iterator_tag
# define random_access_iterator_tag _msl_random_access_iterator_tag
# define output_iterator _msl_output_iter
# define input_iterator _msl_input_iter
# define forward_iterator _msl_forward_iter
# define bidirectional_iterator _msl_bidirectional_iter
# define random_access_iterator _msl_random_access_iter
# define distance __msl_distance
# define __distance __msl___distance
# define advance __msl_advance
# define __advance __msl___advance
# define reverse_bidirectional_iterator __msl_reverse_bidirectional_iterator
# define reverse_iterator __msl_reverse_iterator
# define back_insert_iterator __msl_back_insert_iterator
# define back_inserter __msl_back_inserter
# define front_insert_iterator __msl_front_insert_iterator
# define front_inserter __msl_front_inserter
# define insert_iterator __msl_insert_iterator
# define inserter __msl_inserter
# define istream_iterator __msl_istream_iterator
# define ostream_iterator __msl_ostream_iterator

// Include MSL compiler settings to get platform defined
# include <mcompile.h>

// IMPORTANT!
//  You may have to edit the path to match your CW installation 
# if defined (_Windows)
// MS_Windows short path
#  include <..\MSL C++\Include\Library Support\iterator.h>
// If not using "recursive inclusion" option, you might want to use this:
// #  include <..\Library Support\iterator.h>
// MS-Windows long example
// # include "D:\Program Files\MSL C++\Include\Library Support\iterator.h"
# else
// Typical Mac installation
# include "::MSL C++:Include:Library Support:iterator.h"
// If not using "recursive inclusion" option, you might want to use this:
// #  include <::Library Support:iterator.h>
// Yet another (long ) Mac example
// #include "Hard Disk:Metrowerks:Metrowerks CodeWarrior:Metrowerks Standard Library:MSL C++:Include:Library Support:iterator.h"
# endif

# undef input_iterator_tag
# undef output_iterator_tag
# undef forward_iterator_tag
# undef bidirectional_iterator_tag
# undef random_access_iterator_tag
# undef output_iterator
# undef input_iterator
# undef forward_iterator
# undef bidirectional_iterator
# undef random_access_iterator 
# undef distance 
# undef __distance 
# undef advance 
# undef __advance
# undef reverse_bidirectional_iterator
# undef reverse_iterator
# undef back_insert_iterator
# undef back_inserter
# undef front_insert_iterator
# undef front_inserter
# undef insert_iterator
# undef inserter
# undef istream_iterator
# undef ostream_iterator

#endif /* __SGI_STL_MSL_ITERATOR_H */
