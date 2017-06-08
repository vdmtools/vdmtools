/***
*  * WHAT
*  *    Type definitions from module GLOBAL_anonym
*  * COPYRIGHT
*  *    (C) Kyushu University
*  * SPECIFICATION VERSION
*  *    SEM.vdm version 1.21.2.12
***/

#ifndef __GLOBAL_anonym_H__
#define __GLOBAL_anonym_H__

// forward declaration
#define TYPE_INSTRTP_Instruction Record
typedef SEQ<TYPE_INSTRTP_Instruction> TYPE_STKM_SubProgram;
//typedef TYPE_STKM_SubProgram TYPE_STKM_DebugCmd;
#ifndef DECL_TYPE_STKM_DebugCmd
#define DECL_TYPE_STKM_DebugCmd
class TYPE_STKM_DebugCmd : public Record
{
public:
  TYPE_STKM_DebugCmd();
  TYPE_STKM_DebugCmd(const Common &c);
  TYPE_STKM_DebugCmd &Init(const TYPE_STKM_SubProgram & p1);
  TYPE_STKM_SubProgram get_instr() const;
  void set_instr(const TYPE_STKM_SubProgram &p);
};
#endif // DECL_TYPE_STKM_DebugCmd

#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "GLOBAL_anonym-sl.h"
#endif

#ifdef VDMPP
#ifdef VICE
#define INCLFILE "GLOBAL_anonym-rt.h"
#else
#define INCLFILE "GLOBAL_anonym-pp.h"
#endif // VICE
#endif // VDMPP

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE

#endif // __GLOBAL_anonym_H__
