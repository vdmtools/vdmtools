/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/gencode/AS.h,v $
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

#ifndef __as_h__
#define __as_h__

#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "AS-sl.h"
#endif // VDMSL

#ifdef VDMPP
#ifdef VICE
#define INCLFILE "AS-rt.h"
#else
#define INCLFILE "AS-pp.h"
#endif // VICE
#endif // VDMPP

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE

#define mk_AS_Name(ids, cid) \
        Record(TAG_TYPE_AS_Name, length_AS_Name)\
        .SetField(pos_AS_Name_ids, ids)\
        .SetField(pos_AS_Name_cid, cid)
#endif // __as_h__
