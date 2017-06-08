/***
*  * WHAT
*  *  
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/eval/gencode/SCHDTP.h,v $
*  * VERSION
*  *    $Revision: 1.6 $
*  * DATE
*  *    $Date: 2000/06/19 09:37:19 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: erik $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __schdtp_h__
#define __schdtp_h__

#include "metaiv.h"
class EvaluatorStatus;
typedef VDMContainer<EvaluatorStatus> TYPE_STKM_EvaluatorStatus;

#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "SCHDTP-sl.h"
#endif

#ifdef VDMPP
#ifdef VICE
#define INCLFILE "SCHDTP-rt.h"
#else
#define INCLFILE "SCHDTP-pp.h"
#endif //VICE
#endif // VDMPP

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE

#endif // __schdtp_h__
