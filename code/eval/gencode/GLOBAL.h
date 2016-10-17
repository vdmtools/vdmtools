/***
*  * WHAT
*  *    Type definitions from module GLOBAL
*  * COPYRIGHT
*  *    (C) Kyushu University
*  * SPECIFICATION VERSION
*  *    global.vdm version 1.13.2.9
***/

// This file is needed by INSTRTP.h, as this file is code generated from 
// instrTP, and this module imports GLOBAL.

#ifndef __global_h__
#define __global_h__

#include "metaiv.h"
#include "main_tag.h"

#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "GLOBAL-sl.h"
#endif

#ifdef VDMPP
#ifdef VICE
#define INCLFILE "GLOBAL-rt.h"
#else
#define INCLFILE "GLOBAL-pp.h"
#endif // VICE
#endif // VDMPP

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE

#endif // __global_h__
