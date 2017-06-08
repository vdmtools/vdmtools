/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/eval/gencode/SCHDTP_anonym.h,v $
*  * VERSION
*  *    $Revision: 1.1 $
*  * DATE
*  *    $Date: 2000/01/07 10:29:33 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM++ Toolbox
*  * AUTHOR
*  *    $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

typedef Int TYPE_STKM_MessageId;

#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "SCHDTP_anonym-sl.h"
#endif // VDMSL

#ifdef VDMPP
#ifdef VICE
#define INCLFILE "SCHDTP_anonym-rt.h"
#else
#define INCLFILE "SCHDTP_anonym-pp.h"
#endif // VICE
#endif // VDMPP

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE
