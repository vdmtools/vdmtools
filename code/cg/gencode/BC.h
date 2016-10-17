/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/gencode/BC.h,v $
*  * VERSION
*  *    $Revision: 1.4 $
*  * DATE
*  *    $Date: 2001/02/12 10:34:03 $
***/

#ifndef __bc_h__
#define __bc_h__

#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "BC-sl.h"
#endif // VDMSL

#ifdef VDMPP
#ifdef VICE
#define INCLFILE "BC-rt.h"
#else
#define INCLFILE "BC-pp.h"
#endif // VICE
#endif

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE

#endif // __bc_h__
