/***
*  * WHAT
*  *    Exported functions from debug.cc
*  * FILE
*  *    $Source:$
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __freevars_h__
#define __freevars_h__

/*****************************************************************************/
/** free.vdm *****************************************************************/
/*****************************************************************************/

#include "semvalues.h"

class Free {
public:
  static SET<TYPE_AS_Name> IdentInPattern (const TYPE_AS_PatternBind &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInExpr (
                                           const TYPE_AS_Expr &,
                                           const SET<TYPE_AS_Name> &);
  static TYPE_SEM_BlkEnv FreeMapToBlkEnv (const MAP<TYPE_AS_Name, TYPE_SEM_VAL> &);

private:
  static SET<TYPE_AS_Name> eset;
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> emap;

  static SET<TYPE_AS_Name> IdentInBind (const TYPE_AS_Bind &);
  static SET<TYPE_AS_Name> IdentInMultBind (const TYPE_AS_MultBind &);
  static SET<TYPE_AS_Name> IdentInMultBindSeq (const SEQ<TYPE_AS_MultBind> &);

  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInBind (
                                           const TYPE_AS_Bind &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInMultBindSeq (
                                           const SEQ<TYPE_AS_MultBind> &,
                                           const SET<TYPE_AS_Name> &);
  
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInDefExpr (
                                           const TYPE_AS_DefExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInLetExpr (
                                           const TYPE_AS_LetExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInFnDef (
                                           const TYPE_AS_FnDef &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInPreExpr (
                                           const Generic &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInPostExpr (
                                           const Generic &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInLetBeSTExpr (
                                           const TYPE_AS_LetBeSTExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInIfExpr (
                                           const TYPE_AS_IfExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInElseifExpr (
                                           const SEQ<TYPE_AS_ElseifExpr> &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInCasesExpr (
                                           const TYPE_AS_CasesExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInPrefixExpr (
                                           const TYPE_AS_PrefixExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInBinaryExpr (
                                           const TYPE_AS_BinaryExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInAllOrExistsExpr (
                                           const TYPE_AS_AllOrExistsExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInExistsUniqueExpr (
                                           const TYPE_AS_ExistsUniqueExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInSetEnumerationExpr (
                                           const TYPE_AS_SetEnumerationExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInSetComprehensionExpr (
                                           const TYPE_AS_SetComprehensionExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInSetRangeExpr (
                                           const TYPE_AS_SetRangeExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInSeqEnumerationExpr (
                                           const TYPE_AS_SeqEnumerationExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInSeqComprehensionExpr (
                                           const TYPE_AS_SeqComprehensionExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInSubSequenceExpr (
                                           const TYPE_AS_SubSequenceExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInSeqModifyMapOverrideExpr (
                                           const TYPE_AS_SeqModifyMapOverrideExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInMapEnumerationExpr (
                                           const TYPE_AS_MapEnumerationExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInMapComprehensionExpr (
                                           const TYPE_AS_MapComprehensionExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInTupleConstructorExpr (
                                           const TYPE_AS_TupleConstructorExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInRecordConstructorExpr (
                                           const TYPE_AS_RecordConstructorExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInRecordModifierExpr (
                                           const TYPE_AS_RecordModifierExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInTokenConstructorExpr(
                                           const TYPE_AS_TokenConstructorExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInTupleSelectExpr(
                                           const TYPE_AS_TupleSelectExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInTypeJudgementExpr(
                                           const TYPE_AS_TypeJudgementExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInPreConditionApplyExpr(
                                           const TYPE_AS_PreConditionApplyExpr &,
                                           const SET<TYPE_AS_Name>&);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInApplyExpr (
                                           const TYPE_AS_ApplyExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInLambdaExpr (
                                           const TYPE_AS_LambdaExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInFieldSelectExpr (
                                           const TYPE_AS_FieldSelectExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInFctTypeInstExpr (
                                           const TYPE_AS_FctTypeInstExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInIsExpr (
                                           const TYPE_AS_IsExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInNarrowExpr (
                                           const TYPE_AS_NarrowExpr &,
                                           const SET<TYPE_AS_Name> &);
  
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInName (
                                           const TYPE_AS_Name &,
                                           const SET<TYPE_AS_Name> &);
  
#ifdef VDMSL
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInOldName (
                                           const TYPE_AS_OldName &,
                                           const SET<TYPE_AS_Name> &);
#endif //VDMSL
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInBracketedExpr (
                                           const TYPE_AS_BracketedExpr &,
                                           const SET<TYPE_AS_Name> &);
  static MAP<TYPE_AS_Name, TYPE_SEM_VAL> FreeInIotaExpr (
                                           const TYPE_AS_IotaExpr & IotaE,
                                           const SET<TYPE_AS_Name> & id_s);
  
  static Generic LookUpInTopEnv (const TYPE_AS_Name &);
};

#endif // __freevars_h__
  
// Force emacs to use C++ mode for this file.
//// Local Variables:
//// mode: c++
//// End:
  
