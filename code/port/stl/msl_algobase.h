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

#ifndef __SGI_STL_MSL_ALGOBASE_H
# define __SGI_STL_MSL_ALGOBASE_H
// fbp : the only purpose of this file is  to get 
// auxiliary functions from MSL library. 
// It is to be included from within adaptation <algobase.h>

# define min __msl_min
# define max __msl_max
# define mismatch __msl_mismatch
# define equal __msl_equal
# define copy  __msl_copy
# define copy_backward  __msl_copy_backward
# define swap __msl_swap
# define iter_swap __msl_iter_swap
# define __iter_swap __msl___iter_swap
# define fill __msl_fill
# define fill_n __msl_fill_n
# define lexicographical_compare __msl_lexicographical_compare

// Include MSL compiler settings to get platform defined
# include <mcompile.h>

// IMPORTANT!
//  You may have to edit the path to match your CW installation 
# if defined (_Windows)
// MS_Windows short path
#  include <..\MSL C++\Include\Library Support\algobase.h>
// If not using "recursive inclusion" option, you might want to use this:
// #  include <..\Library Support\algobase.h>
// MS-Windows long example
// # include "D:\Program Files\MSL C++\Include\Library Support\algobase.h"
# else
// Typical Mac installation
#  include <::MSL C++:Include:Library Support:algobase.h>
// If not using "recursive inclusion" option, you might want to use this:
// #  include <::Library Support:algobase.h>
// Yet another (long ) Mac example
// #include "Hard Disk:Metrowerks:Metrowerks CodeWarrior:Metrowerks Standard Library:MSL C++:Include:Library Support:algobase.h"
# endif

# undef min 
# undef max 
# undef mismatch 
# undef equal
# undef copy
# undef copy_backward
# undef swap
# undef iter_swap
# undef __iter_swap
# undef fill
# undef fill_n
# undef lexicographical_compare

#endif /* __SGI_STL_MSL_ALGOBASE_H */
