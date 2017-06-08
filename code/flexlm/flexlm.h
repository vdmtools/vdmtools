/***
*  * WHAT
*  *    vpp interface to flexlm
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/flexlm/flexlm.h,v $
*  * VERSION
*  *    $Revision: 1.15 $
*  * DATE
*  *    $Date: 2001/10/19 15:49:39 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VPP, Afrodite - ESPRIT programme, project no. 6500.
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    Henrik Voss + $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef flexlm_h_
#define flexlm_h_

#ifdef FLM
#include "tb_string.h"

#ifdef VDMPP
// The Toolbox version is defined in the Makefile
#define FEATURE "vdmpp"
#define CG_FEATURE "vppcg"
#define JAVACG_FEATURE "vppjcg"
#define DL_FEATURE "vppdl"
#define J2V_FEATURE "vppj2v"
#endif // VDMPP

#ifdef VDMSL
#define FEATURE "vdmsl"
#define CG_FEATURE "vdmcg"
#define DL_FEATURE "vdmdl"
#endif // VDMSL

wstring InitFLM ();              /* Empty wstring means ok. */
void CheckInLicense (char* feature);
int CheckOutFeature (char* feature, char* version);
void Check_lm_periodically ();

#if DL_MODULE_ENABLED
bool FLM_GetDLLicense(); 
void FLM_ResetDLLicense(); 
#endif // DL_MODULE_ENABLED

#endif // FLM
#endif // flexlm_h_
