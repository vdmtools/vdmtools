/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/pog/gencode/POGTP.h,v $
*  * VERSION
*  *    $Revision: 1.1 $
*  * DATE
*  *    $Date: 2001/10/08 14:55:59 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM- Toolbox.
*  * AUTHOR
*  *    $Author: mikhail $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __pogtp_h__
#define __pogtp_h__

#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "POGTP-sl.h"
#endif
#ifdef VICE
#define INCLFILE "POGTP-rt.h"
#else
#ifdef VDMPP
#define INCLFILE "POGTP-pp.h"
#endif
#endif //VICE

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE

#endif // __pogtp_h__
