#ifndef __PURE_uPAT_userdef_h__
#define __PURE_uPAT_userdef_h__

#include "main_tag.h"
#include "AS.h"

#define DEF_PURE_uPAT_USERIMPL

#define DEF_PURE_uPAT_Pattern2PurePattern 1
#define DEF_PURE_uPAT_BindList2PureBindList 1
#define DEF_PURE_uPAT_Bind2PureBind 1

#ifdef DEF_PURE_uPAT_USERIMPL
class PUREPAT
{
public:
  static Tuple Pattern2PurePattern (const TYPE_AS_Pattern & pat, const SET<TYPE_AS_Name> & ids);
  static Tuple BindList2PureBindList (const TYPE_AS_BindList & binds, const SET<TYPE_AS_Name> & ids);
  static Tuple Bind2PureBind (const TYPE_AS_Bind & bind, const SET<TYPE_AS_Name> & ids);
private:
  static Tuple Pattern2PurePatternSeq (const SEQ<TYPE_AS_Pattern> & pats, const SET<TYPE_AS_Name> & ids);
  static Tuple PatternName2PurePattern (const TYPE_AS_PatternName & pat, const SET<TYPE_AS_Name> & ids);
  static Tuple MatchVal2PurePattern (const TYPE_AS_MatchVal & pat, const SET<TYPE_AS_Name> & ids);
  static Tuple SetEnumPattern2PurePattern (const TYPE_AS_SetEnumPattern & pat, const SET<TYPE_AS_Name> & ids);
  static Tuple SetUnionPattern2PurePattern (const TYPE_AS_SetUnionPattern & pat, const SET<TYPE_AS_Name> & ids);
  static Tuple SeqEnumPattern2PurePattern (const TYPE_AS_SeqEnumPattern & pat, const SET<TYPE_AS_Name> & ids);
  static Tuple SeqConcPattern2PurePattern (const TYPE_AS_SeqConcPattern & pat, const SET<TYPE_AS_Name> & ids);
  static Tuple MapletPattern2PurePattern (const TYPE_AS_MapletPattern & pat, const SET<TYPE_AS_Name> & ids);
  static Tuple MapEnumPattern2PurePattern (const TYPE_AS_MapEnumPattern & pat, const SET<TYPE_AS_Name> & ids);
  static Tuple MapMergePattern2PurePattern (const TYPE_AS_MapMergePattern & pat, const SET<TYPE_AS_Name> & ids);
  static Tuple RecordPattern2PurePattern (const TYPE_AS_RecordPattern & pat, const SET<TYPE_AS_Name> & ids);
  static Tuple TuplePattern2PurePattern (const TYPE_AS_TuplePattern & pat, const SET<TYPE_AS_Name> & ids);
#ifdef VDMPP
  static Tuple ObjectPattern2PurePattern (const TYPE_AS_ObjectPattern & pat, const SET<TYPE_AS_Name> & ids);
  static Tuple FieldPattern2PurePattern (const TYPE_AS_FieldPattern & pat, const SET<TYPE_AS_Name> & ids);
#endif // VDMPP

  static Tuple SetBind2PureSetBind (const TYPE_AS_SetBind & bind, const SET<TYPE_AS_Name> & ids);
  static Tuple TypeBind2PureTypeBind (const TYPE_AS_TypeBind & bind, const SET<TYPE_AS_Name> & ids);
  static Tuple MultBind2PureMultBind (const TYPE_AS_MultBind & mbinds, const SET<TYPE_AS_Name> & ids);
  static Tuple MultSetBind2PureMultSetBind (const TYPE_AS_MultSetBind & bind, const SET<TYPE_AS_Name> & ids);
  static Tuple MultTypeBind2PureMultTypeBind (const TYPE_AS_MultTypeBind & bind, const SET<TYPE_AS_Name> & ids);
};
#endif // DEF_PURE_uPAT_USERIMPL

#endif // __PURE_uPAT_userdef_h__
