/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/pog/gencode/PID.h,v $
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
#define INCLFILE "PID-sl.h"
#endif
#ifdef VICE
#define INCLFILE "PID-rt.h"
#else
#ifdef VDMPP
#define INCLFILE "PID-pp.h"
#endif
#endif //VICE

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE
#include "cg_aux.h"
