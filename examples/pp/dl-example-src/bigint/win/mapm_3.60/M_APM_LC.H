
/* 
 *  M_APM  -  m_apm_lc.h
 *
 *  Copyright (C) 1999, 2000   Michael C. Ring
 *
 *  Permission to use, copy, and distribute this software and its
 *  documentation for any purpose with or without fee is hereby granted, 
 *  provided that the above copyright notice appear in all copies and 
 *  that both that copyright notice and this permission notice appear 
 *  in supporting documentation.
 *
 *  Permission to modify the software is granted, but not the right to
 *  distribute the modified code.  Modifications are to be distributed 
 *  as patches to released version.
 *  
 *  This software is provided "as is" without express or implied warranty.
 */

/*
 *      This is the local header file needed to build the library
 *
 *      $Log: M_APM_LC.H,v $
 *      Revision 1.1.1.1  2001/10/04 13:02:57  paulm
 *      Import of dir
 *
 *      Revision 1.21  2000/08/01 22:21:55  mike
 *      add prototype
 *
 *      Revision 1.20  2000/07/19 17:21:26  mike
 *      add ifdef for older Borland compilers
 *
 *      Revision 1.19  2000/07/11 20:09:30  mike
 *      add new prototype
 *
 *      Revision 1.18  2000/05/19 17:09:57  mike
 *      add local copies for PI variables
 *
 *      Revision 1.17  2000/05/04 23:21:56  mike
 *      change/add new global internal MAPM values
 *
 *      Revision 1.16  2000/04/11 18:44:43  mike
 *      no longer need the constant 'Fifteen'
 *
 *      Revision 1.15  2000/04/03 17:27:08  mike
 *      added cbrt prototype
 *
 *      Revision 1.14  2000/02/03 22:41:34  mike
 *      add MAPM_* memory function defines
 *
 *      Revision 1.13  1999/07/09 22:46:10  mike
 *      add skip limit integer
 *
 *      Revision 1.12  1999/07/08 23:35:20  mike
 *      change constant
 *
 *      Revision 1.11  1999/07/08 22:55:38  mike
 *      add new constant
 *
 *      Revision 1.10  1999/06/23 01:08:11  mike
 *      added constant '15'
 *
 *      Revision 1.9  1999/06/20 23:38:11  mike
 *      updated for new prototypes
 *
 *      Revision 1.8  1999/06/20 23:30:03  mike
 *      added new constants
 *
 *      Revision 1.7  1999/06/20 19:23:12  mike
 *      delete constants no longer needed
 *
 *      Revision 1.6  1999/06/20 18:50:21  mike
 *      added more constants
 *
 *      Revision 1.5  1999/06/19 20:37:30  mike
 *      add stack prototypes
 *
 *      Revision 1.4  1999/05/31 23:01:38  mike
 *      delete some unneeded constants
 *
 *      Revision 1.3  1999/05/15 02:23:28  mike
 *      fix define for M_COS
 *
 *      Revision 1.2  1999/05/15 02:16:56  mike
 *      add check for number of decimal places
 *
 *      Revision 1.1  1999/05/12 20:51:22  mike
 *      Initial revision
 *
 *      $Id: M_APM_LC.H,v 1.1.1.1 2001/10/04 13:02:57 paulm Exp $
 */

#ifndef M__APM_LOCAL_INC
#define M__APM_LOCAL_INC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "m_apm.h"

/* 
 *   this supports older (and maybe newer?) Borland compilers.
 *   these Borland compilers define __MSDOS__
 */

#ifndef MSDOS
#ifdef __MSDOS__
#define MSDOS
#endif
#endif


/*
 *      use your own memory management functions if desired.
 *      re-define MAPM_* below to point to your functions.
 *      an example is shown below.
 */

/*
extern   void   *memory_allocate(unsigned int);
extern   void   *memory_reallocate(void *, unsigned int);
extern   void   memory_free(void *);

#define  MAPM_MALLOC memory_allocate
#define  MAPM_REALLOC memory_reallocate
#define  MAPM_FREE memory_free
*/

/* default: use the standard C library memory functions ... */

#define  MAPM_MALLOC malloc
#define  MAPM_REALLOC realloc
#define  MAPM_FREE free

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define	M_APM_IDENT 0x6BCC9AE5

/* number of digits in the global constants, PI, E, etc */

#define	VALID_DECIMAL_PLACES 128

extern  int     MM_skip_limit_PI_check;
extern  int     MM_lc_PI_digits;

/*
 *   constants not in m_apm.h
 */

extern	M_APM	MM_0_5;
extern	M_APM	MM_0_85;
extern	M_APM	MM_5x_125R;
extern	M_APM	MM_5x_64R;
extern	M_APM	MM_5x_256R;
extern	M_APM	MM_5x_Eight;
extern	M_APM	MM_5x_Sixteen;
extern	M_APM	MM_5x_Twenty;
extern	M_APM	MM_lc_PI;
extern	M_APM	MM_lc_HALF_PI;
extern	M_APM	MM_lc_2_PI;

/*
 *   prototypes for internal functions
 */

extern	void	M_init_trig_globals(void);
extern	void	M_raw_exp(M_APM, int, M_APM);
extern	void	M_raw_sin(M_APM, int, M_APM);
extern	void	M_raw_cos(M_APM, int, M_APM);
extern	void	M_5x_sin(M_APM, int, M_APM);
extern	void	M_4x_cos(M_APM, int, M_APM);
extern	void	M_5x_do_it(M_APM, int, M_APM);
extern	void	M_4x_do_it(M_APM, int, M_APM);

extern	M_APM	M_get_stack_var(void);
extern	void	M_restore_stack(int);
extern	int 	M_get_sizeof_int(void);

extern	void	M_cos_to_sin(M_APM, int, M_APM);
extern	void	M_limit_angle_to_pi(M_APM, int, M_APM);
extern	void	M_get_sqrt_guess(M_APM, M_APM);
extern	void	M_get_cbrt_guess(M_APM, M_APM);
extern	void	M_get_log_guess(M_APM, M_APM);
extern	void	M_get_asin_guess(M_APM, M_APM);
extern	void	M_get_acos_guess(M_APM, M_APM);

extern	void	M_init_util_data(void);
extern	void	M_get_div_rem(int,UCHAR *,UCHAR *);
extern	void	M_get_div_rem_10(int,UCHAR *,UCHAR *);
extern	void	M_apm_normalize(M_APM);
extern	void	M_apm_scale(M_APM, int);
extern	void	M_apm_pad(M_APM, int);
extern	void	M_check_PI_places(int);
extern  void    M_calculate_PI_AGM(M_APM, int);
extern	int	M_strposition(char *,char *);
extern	char	*M_lowercase(char *);

#endif

