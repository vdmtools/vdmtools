/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/pog/gencode/REP2TYPE.h,v $
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
#define INCLFILE "REP2TYPE-sl.h"
#endif
#ifdef VICE
#define INCLFILE "REP2TYPE-rt.h"
#else
#ifdef VDMPP
#define INCLFILE "REP2TYPE-pp.h"
#endif
#endif //VICE

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE
