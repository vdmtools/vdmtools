/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/gencode/CGTYPES.h,v $
*  * VERSION
*  *    $Revision: 1.1 $
*  * DATE
*  *    $Date: 2000/10/17 09:56:35 $
*  * FORMAT
*  *    $State: Exp $
*  * AUTHOR
*  *    $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "CGTYPES-sl.h"
#endif
#ifdef VDMPP
#ifdef VICE
#define INCLFILE "CGTYPES-rt.h"
#else
#define INCLFILE "CGTYPES-pp.h"
#endif // VICE
#endif // VDMPP

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE
