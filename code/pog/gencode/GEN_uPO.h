/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/pog/gencode/GEN_uPO.h,v $
*  * VERSION
*  *    $Revision: 1.2 $
*  * DATE
*  *    $Date: 2005/12/08 05:59:10 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM- Toolbox.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "GEN_uPO-sl.h"
#endif
#ifdef VICE
#define INCLFILE "GEN_uPO-rt.h"
#else
#ifdef VDMPP
#define INCLFILE "GEN_uPO-pp.h"
#endif
#endif //VICE

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include "cg_aux.h"
#include INCLFILE
