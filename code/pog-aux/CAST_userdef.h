#ifndef __CAST_userdef_h__
#define __CAST_userdef_h__

#include "main_tag.h"
#include "AS.h"

#define DEF_CAST_USERIMPL 1

#define DEF_CAST_Pattern2Expr 1
#define DEF_CAST_PatternName2Expr 1

#ifdef DEF_CAST_USERIMPL
class CAST
{
public:
  static TYPE_AS_Expr Pattern2Expr (const TYPE_AS_Pattern & pat);
  static TYPE_AS_Expr PatternName2Expr (const TYPE_AS_PatternName & pat);

private:
  static TYPE_AS_Expr MatchVal2Expr (const TYPE_AS_MatchVal & pat);
  static TYPE_AS_TupleConstructorExpr TuplePattern2Expr (const TYPE_AS_TuplePattern & pat);
  static TYPE_AS_RecordConstructorExpr RecordPattern2Expr (const TYPE_AS_RecordPattern & pat);
  static TYPE_AS_BinaryExpr SeqConcPattern2Expr (const TYPE_AS_SeqConcPattern & pat);
  static TYPE_AS_SeqEnumerationExpr SeqEnumPattern2Expr (const TYPE_AS_SeqEnumPattern & pat);
  static TYPE_AS_SetEnumerationExpr SetEnumPattern2Expr (const TYPE_AS_SetEnumPattern & pat);
  static TYPE_AS_BinaryExpr SetUnionPattern2Expr (const TYPE_AS_SetUnionPattern & pat);
  static TYPE_AS_Maplet MapletPattern2Expr (const TYPE_AS_MapletPattern & pat);
  static TYPE_AS_MapEnumerationExpr MapEnumPattern2Expr (const TYPE_AS_MapEnumPattern & pat);
  static TYPE_AS_BinaryExpr MapMergePattern2Expr (const TYPE_AS_MapMergePattern & pat);
#ifdef VDMPP
  static TYPE_AS_Expr ObjectPattern2Expr (const TYPE_AS_ObjectPattern & pat);
  static TYPE_AS_Expr FieldPattern2Expr (const TYPE_AS_FieldPattern & pat);
#endif // VDMPP
};
#endif // DEF_CAST_USERIMPL

#endif // __CAST_userdef_h__
