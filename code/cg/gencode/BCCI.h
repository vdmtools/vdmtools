/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/gencode/BCCI.h,v $
*  * VERSION
*  *    $Revision: 1.1 $
*  * DATE
*  *    $Date: 2001/02/12 10:34:03 $
***/

#ifndef __bcci_h__
#define __bcci_h__

#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "BCCI-sl.h"
#endif // VDMSL

#ifdef VDMPP
#ifdef VICE
#define INCLFILE "BCCI-rt.h"
#else
#define INCLFILE "BCCI-pp.h"
#endif // VICE
#endif

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE

#endif // __bcci_h__
