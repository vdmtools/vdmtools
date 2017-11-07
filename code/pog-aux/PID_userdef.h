#ifndef __PID_userdef_h__
#define __PID_userdef_h__

#include "main_tag.h"
#include "AS.h"
#include "REP.h"

#define DEF_PID_USERIMPL 1

#define DEF_PID_Pattern2BindList 1
#define DEF_PID_Pattern2Pids 1
#define DEF_PID_NameInPattern 1
#define DEF_PID_NameInBindList 1
#define DEF_PID_NameInBind 1

#ifdef DEF_PID_USERIMPL
class PID
{
public:
  static SEQ<TYPE_AS_MultBind> Pattern2BindList (const TYPE_AS_Pattern & pat);
  static SET<TYPE_AS_PatternName> Pattern2Pids (const TYPE_AS_Pattern & pat);
  static bool NameInPattern (const TYPE_AS_Name &vdm_PID_nm, const TYPE_AS_Pattern & pat);
  static bool NameInBind (const TYPE_AS_Name &vdm_PID_nm, const TYPE_AS_Bind & bind);
  static bool NameInBindList (const TYPE_AS_Name &vdm_PID_nm, const SEQ<TYPE_AS_MultBind> & binds);

private:
  static SET<TYPE_AS_PatternName> PatternName2Pids (const TYPE_AS_PatternName & pat);
  static SET<TYPE_AS_PatternName> MatchVal2Pids (const TYPE_AS_MatchVal & pat);
  static SET<TYPE_AS_PatternName> SetEnumPattern2Pids (const TYPE_AS_SetEnumPattern & pat);
  static SET<TYPE_AS_PatternName> SetUnionPattern2Pids (const TYPE_AS_SetUnionPattern & pat);
  static SET<TYPE_AS_PatternName> SeqEnumPattern2Pids (const TYPE_AS_SeqEnumPattern & pat);
  static SET<TYPE_AS_PatternName> SeqConcPattern2Pids (const TYPE_AS_SeqConcPattern & pat);
  static SET<TYPE_AS_PatternName> MapEnumPattern2Pids (const TYPE_AS_MapEnumPattern & pat);
  static SET<TYPE_AS_PatternName> MapMergePattern2Pids (const TYPE_AS_MapMergePattern & pat);
  static SET<TYPE_AS_PatternName> RecordPattern2Pids (const TYPE_AS_RecordPattern & pat);
  static SET<TYPE_AS_PatternName> TuplePattern2Pids (const TYPE_AS_TuplePattern & pat);
#ifdef VDMPP
  static SET<TYPE_AS_PatternName> ObjectPattern2Pids (const TYPE_AS_ObjectPattern & pat);
#endif // VDMPP

  static bool NameInPatternBind (const TYPE_AS_Name & nm, const TYPE_AS_PatternBind & patbind);
  static bool NameInPatternSequence (const TYPE_AS_Name & nm, const SEQ<TYPE_AS_Pattern> & pats);

  static bool NameInMultBind (const TYPE_AS_Name & nm, const TYPE_AS_MultBind & mbind);

  static SEQ<TYPE_AS_PatternName> SetToSeq (const SET<TYPE_AS_PatternName> & s);
};
#endif // DEF_PID_USERIMPL

#endif // __PID_userdef_h__
