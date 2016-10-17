/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/pog-pretty/gencode/PP_uTRA_uEXPR_anonym.h,v $
*  * VERSION
*  *    $Revision: 1.1 $
*  * DATE
*  *    $Date: 2001/10/08 14:56:38 $
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
#define INCLFILE "PP_uTRA_uEXPR_anonym-sl.h"
#endif
#ifdef VICE
#define INCLFILE "PP_uTRA_uEXPR_anonym-rt.h"
#else
#ifdef VDMPP
#define INCLFILE "PP_uTRA_uEXPR_anonym-pp.h"
#endif
#endif //VICE

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE
