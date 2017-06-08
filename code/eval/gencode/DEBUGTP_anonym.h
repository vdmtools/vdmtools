/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/eval/gencode/DEBUGTP_anonym.h,v $
*  * VERSION
*  *    $Revision: 1.1 $
*  * DATE
*  *    $Date: 2000/03/20 16:19:14 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: blackie $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/


#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "DEBUGTP_anonym-sl.h"
#endif // VDMSL

#ifdef VDMPP
#ifdef VICE
#define INCLFILE "DEBUGTP_anonym-rt.h"
#else
#define INCLFILE "DEBUGTP_anonym-pp.h"
#endif // VICE
#endif // VDMPP

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE
