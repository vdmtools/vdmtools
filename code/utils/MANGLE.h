/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/gencode/MANGLE.h,v $
*  * VERSION
*  *    $Revision: 1.4 $
*  * DATE
*  *    $Date: 2000/01/07 14:47:39 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __mangle_h__
#define __mangle_h__

#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "MANGLE-sl.h"
#endif // VDMSL

#ifdef VDMPP
#ifdef VICE
#define INCLFILE "MANGLE-rt.h"
#else
#define INCLFILE "MANGLE-pp.h"
#endif // VICE
#endif // VDMPP

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE

#endif //__mangle_h__

