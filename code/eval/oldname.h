/***
*  * WHAT
*  *    Exported functions from debug.cc
*  * FILE
*  *    $Id: oldname.h,v 1.3 2005/12/02 06:28:32 vdmtools Exp $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __oldname_h__
#define __oldname_h__

#include "metaiv.h"
#include "AS.h"

/*****************************************************************************/
/** old.vdm ******************************************************************/
/*****************************************************************************/

class OLD
{
public:
  static TYPE_AS_Expr OldNameInExpr (const TYPE_AS_Expr &);

private:
  static TYPE_AS_PatternBind OldNameInPattern (const TYPE_AS_PatternBind &);
  static TYPE_AS_Bind OldNameInBind (const TYPE_AS_Bind &);
  static SEQ<TYPE_AS_MultBind> OldNameInMultBindSeq (const SEQ<TYPE_AS_MultBind> &);
  static Generic OldNameInGenericExpr (const Generic &);
  static TYPE_AS_Expr OldNameInDefExpr (const TYPE_AS_DefExpr &);
  static TYPE_AS_Expr OldNameInLetExpr (const TYPE_AS_LetExpr &);
  static TYPE_AS_Expr OldNameInLetBeSTExpr (const TYPE_AS_LetBeSTExpr &);
  static TYPE_AS_Expr OldNameInIfExpr (const TYPE_AS_IfExpr &);
  static TYPE_AS_Expr OldNameInCasesExpr (const TYPE_AS_CasesExpr &);
  static TYPE_AS_Expr OldNameInPrefixExpr (const TYPE_AS_PrefixExpr &);
  static TYPE_AS_Expr OldNameInBinaryExpr (const TYPE_AS_BinaryExpr &);
  static TYPE_AS_Expr OldNameInAllOrExistsExpr (const TYPE_AS_AllOrExistsExpr &);
  static TYPE_AS_Expr OldNameInExistsUniqueExpr (const TYPE_AS_ExistsUniqueExpr &);
  static TYPE_AS_Expr OldNameInSetEnumerationExpr (const TYPE_AS_SetEnumerationExpr &);
  static TYPE_AS_Expr OldNameInSetComprehensionExpr (const TYPE_AS_SetComprehensionExpr &);
  static TYPE_AS_Expr OldNameInSetRangeExpr (const TYPE_AS_SetRangeExpr &);
  static TYPE_AS_Expr OldNameInSeqEnumerationExpr (const TYPE_AS_SeqEnumerationExpr &);
  static TYPE_AS_Expr OldNameInSeqComprehensionExpr (const TYPE_AS_SeqComprehensionExpr &);
  static TYPE_AS_Expr OldNameInSubSequenceExpr (const TYPE_AS_SubSequenceExpr &);
  static TYPE_AS_Expr OldNameInSeqModifyMapOverrideExpr (const TYPE_AS_SeqModifyMapOverrideExpr &);
  static TYPE_AS_Maplet OldNameInMaplet (const TYPE_AS_Maplet &);
  static TYPE_AS_Expr OldNameInMapEnumerationExpr (const TYPE_AS_MapEnumerationExpr &);
  static TYPE_AS_Expr OldNameInMapComprehensionExpr (const TYPE_AS_MapComprehensionExpr &);
  static TYPE_AS_Expr OldNameInTupleConstructorExpr (const TYPE_AS_TupleConstructorExpr &);
  static TYPE_AS_Expr OldNameInRecordConstructorExpr (const TYPE_AS_RecordConstructorExpr &);
  static TYPE_AS_Expr OldNameInRecordModifierExpr (const TYPE_AS_RecordModifierExpr &);
  static TYPE_AS_Expr OldNameInTokenConstructorExpr (const TYPE_AS_TokenConstructorExpr &);
  static TYPE_AS_Expr OldNameInApplyExpr (const TYPE_AS_ApplyExpr &);
  static TYPE_AS_Expr OldNameInLambdaExpr (const TYPE_AS_LambdaExpr &);
  static TYPE_AS_Expr OldNameInFieldSelectExpr (const TYPE_AS_FieldSelectExpr &);
  static TYPE_AS_Expr OldNameInFctTypeInstExpr (const TYPE_AS_FctTypeInstExpr &);
  static TYPE_AS_Expr OldNameInIsExpr (const TYPE_AS_IsExpr &);
  static TYPE_AS_Expr OldNameInNarrowExpr (const TYPE_AS_NarrowExpr &);
  static TYPE_AS_Expr OldNameInOldName (const TYPE_AS_OldName &);
  static TYPE_AS_Expr OldNameInBracketedExpr (const TYPE_AS_BracketedExpr &);
  static TYPE_AS_Expr OldNameInIotaExpr (const TYPE_AS_IotaExpr &);
};

#endif // __oldname_h__

// Force emacs to use C++ mode for this file.
//// Local Variables:
//// mode: c++
//// End:

