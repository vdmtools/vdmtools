/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/gencode/AS_anonym.h,v $
*  * VERSION
*  *    $Revision: 1.2 $
*  * DATE
*  *    $Date: 1999/12/02 15:43:25 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "AS_anonym-sl.h"
#endif

#ifdef VDMPP
#ifdef VICE
#define INCLFILE "AS_anonym-rt.h"
#else
#define INCLFILE "AS_anonym-pp.h"
#endif //VICE
#endif //VDMPP

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE
