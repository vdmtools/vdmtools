/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/pog/gencode/PURE_uPAT.h,v $
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

#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "PURE_uPAT-sl.h"
#endif
#ifdef VICE
#define INCLFILE "PURE_uPAT-rt.h"
#else
#ifdef VDMPP
#define INCLFILE "PURE_uPAT-pp.h"
#endif
#endif //VICE

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE
