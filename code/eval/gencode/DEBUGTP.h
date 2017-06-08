/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/eval/gencode/DEBUGTP.h,v $
*  * VERSION
*  *    $Revision: 1.1 $
*  * DATE
*  *    $Date: 2000/03/20 16:19:13 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: blackie $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __DEBUGTP_h__
#define __DEBUGTP_h__

#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "DEBUGTP-sl.h"
#endif // VDMSL

#ifdef VDMPP
#ifdef VICE
#define INCLFILE "DEBUGTP-rt.h"
#else
#define INCLFILE "DEBUGTP-pp.h"
#endif // VICE
#endif // VDMPP

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE

#endif // __DEBUGTP_h__
