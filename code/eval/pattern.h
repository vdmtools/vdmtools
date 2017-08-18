/***
*  * WHAT
*  *    Pattern match. Exported functions.
*  * FILE
*  *    $Id: pattern.h,v 1.6 2006/03/30 04:14:08 vdmtools Exp $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __pattern_h__
#define __pattern_h__

#include "semvalues.h"
#include "stackeval.h"
#include "AS.h"

/*****************************************************************************/
/** pat.vdm ******************************************************************/
/*****************************************************************************/

#define DONT_PARTITION 1
#define DO_PARTITION   2

class PAT
{
private:
  static Nil nil;
  static SET<TYPE_SEM_BlkEnv> eset;
   
public:
  static SET<TYPE_SEM_BlkEnv> PatternMatch (const TYPE_STKM_Pattern & pat_p, const TYPE_SEM_VAL & val_v);
  static Set EvalMultBindSeqAll (const SEQ<TYPE_STKM_Pattern> & pat_lp, const SEQ<TYPE_SEM_VAL> & seq_lv,
                                 const Int & partition);
  static Set EvalMultBindSeq (const SEQ<TYPE_STKM_Pattern> & pat_lp, const SEQ<TYPE_SEM_VAL> & seq_lv,
                              const Int & partition);
  static SET<TYPE_SEM_BlkEnv> MatchLists (const SEQ<TYPE_STKM_Pattern> & els_lp, const SEQ<TYPE_SEM_VAL> & val_lv);
  static Tuple ConstructFN (const TYPE_AS_Name & clmodName, const TYPE_AS_FnDef & fndef);
  static TYPE_AS_Pattern SelPattern (const TYPE_AS_Bind & bind);
  static TYPE_AS_Expr GetExpr (const TYPE_AS_Pattern & pat_p);
  static TYPE_AS_PatternBind DoCarePattern (const TYPE_AS_PatternBind & pat_p, const TYPE_AS_Name & id_base);

private:
  static SET<TYPE_SEM_BlkEnv> MatchPatternName(const TYPE_STKM_PatternName & pat, const TYPE_SEM_VAL & val_v);
  static SET<TYPE_SEM_BlkEnv> MatchMatchVal(const TYPE_STKM_MatchVal & pat, const TYPE_SEM_VAL & val_v);
  static SET<TYPE_SEM_BlkEnv> MatchSetEnumPattern (const TYPE_STKM_SetEnumPattern & pat, const TYPE_SEM_VAL & val_v);
  static SET<TYPE_SEM_BlkEnv> MatchSetUnionPattern (const TYPE_STKM_SetUnionPattern & pat, const TYPE_SEM_VAL & val_v);
  static SET<TYPE_SEM_BlkEnv> MatchSeqEnumPattern (const TYPE_STKM_SeqEnumPattern & pat, const TYPE_SEM_VAL & val_v);
  static SET<TYPE_SEM_BlkEnv> MatchSeqConcPattern (const TYPE_STKM_SeqConcPattern & pat, const TYPE_SEM_VAL & val_v);
  static SET<TYPE_SEM_BlkEnv> MatchMapEnumPattern (const TYPE_STKM_MapEnumPattern & pat, const TYPE_SEM_VAL & val_v);
  static SET<TYPE_SEM_BlkEnv> MatchMapMergePattern (const TYPE_STKM_MapMergePattern & pat, const TYPE_SEM_VAL & val_v);
  static SET<TYPE_SEM_BlkEnv> MatchMapletPattern (const TYPE_STKM_MapletPattern & pat, const TYPE_SEM_VAL & val_v);
  static SET<TYPE_SEM_BlkEnv> MatchTuplePattern (const TYPE_STKM_TuplePattern & pat, const TYPE_SEM_VAL & val_v);
  static SET<TYPE_SEM_BlkEnv> MatchRecordPattern (const TYPE_STKM_RecordPattern & pat, const TYPE_SEM_VAL & val_v);
#ifdef VDMPP
  static SET<TYPE_SEM_BlkEnv> MatchObjectPattern (const TYPE_STKM_ObjectPattern & pat, const TYPE_SEM_VAL & val_v);
#endif // VDMPP
  static Set PatternMatchSetVal(const TYPE_STKM_Pattern & pat, const TYPE_SEM_VAL & val);
  static Tuple ConstructExplFN (const TYPE_AS_Name & clmodName, const TYPE_AS_FnDef & fndef);
  static Tuple ConstructExtExplFN (const TYPE_AS_Name & clmodName, const TYPE_AS_FnDef & fndef);
  static Tuple ConstructImplFN (const TYPE_AS_Name & clmodName, const TYPE_AS_FnDef & fndef);
  static SET<Set> Partition (const SET<Set> & env_ls);
  static SET<TYPE_SEM_BlkEnv> Collapse (const SET<Set> & env_ls);
  static TYPE_AS_Name NewBase (const TYPE_AS_Name & nm, int n);

#ifdef VDMSL
public:
  static Record StripPatternTypeInfo(const Record & g);
#endif // VDMSL
};
#endif // __pattern_h__

// Force emacs to use C++ mode for this file.
//// Local Variables:
//// mode: c++
//// End:

