/***
*  * WHAT
*  *    Type definitions from module SEM_anonym
*  * COPYRIGHT
*  *    (C) Kyushu University
*  * SPECIFICATION VERSION
*  *    SEM.vdm version 1.21.2.12
***/

#ifndef __SEM_anonym_H__
#define __SEM_anonym_H__

#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "SEM_anonym-sl.h"
#endif

#ifdef VDMPP
#ifdef VICE
#define INCLFILE "SEM_anonym-rt.h"
#else
#define INCLFILE "SEM_anonym-pp.h"
#endif // VICE
#endif // VDMPP

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE

#endif // __SEM_anonym_H__
