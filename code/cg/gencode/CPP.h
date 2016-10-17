/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/gencode/CPP.h,v $
*  * VERSION
*  *    $Revision: 1.3 $
*  * DATE
*  *    $Date: 1999/04/07 08:35:03 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: jojo $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __cpp_h__
#define __cpp_h__

#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "CPP-sl.h"
#endif // VDMSL

#ifdef VDMPP
#ifdef VICE 
#define INCLFILE "CPP-rt.h"
#else
#define INCLFILE "CPP-pp.h"
#endif // VICE
#endif // VDMPP

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE

#endif // __cpp_h__
