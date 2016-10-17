/***
*  * WHAT
*  *    Exported functions from debug.cc
*  * FILE
*  *    $Id: evalexpr.h,v 1.25 2006/09/22 11:14:03 vdmtools Exp $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef EVALEXPR_H
#define EVALEXPR_H

#include "AS.h"
#include "semvalues.h"

class EXPR
{
public:
  static TYPE_SEM_VAL EvalSetRangeExpr (const TYPE_SEM_VAL & lb_v, const TYPE_SEM_VAL & ub_v);
  static TYPE_SEM_VAL EvalSubSequenceExpr (const TYPE_SEM_VAL & seq_v,
                                           const TYPE_SEM_VAL & from_v,
                                           const TYPE_SEM_VAL & to_v);
  static TYPE_SEM_VAL EvalSeqModifyMapOverrideExpr (const TYPE_SEM_VAL & seqmap_v, const TYPE_SEM_VAL & map_v);
  static TYPE_SEM_VAL EvalMapOverrideExpr (const TYPE_SEM_VAL & VAL1, const TYPE_SEM_VAL & VAL2);
  static TYPE_SEM_VAL EvalSeqModifyExpr (const TYPE_SEM_VAL & seq_v, const TYPE_SEM_VAL & modifiers_v);
  static TYPE_SEM_VAL EvalTupleModifyExpr (const TYPE_SEM_VAL & seq_v, const TYPE_SEM_VAL & modifiers_v);
  static TYPE_DYNSEM_SEM_SemRecord ConstructSEMRecFields(const TYPE_AS_Name & tag, const SEQ<TYPE_SEM_VAL> & val_l);
  static TYPE_SEM_VAL EvalRecordConstructorExpr (const TYPE_AS_Name & tag, const SEQ<TYPE_SEM_VAL> & val_l);
  static TYPE_SEM_VAL EvalRecordModifierExpr(const TYPE_SEM_VAL & rec_v,
                                             const SEQ<TYPE_AS_Name> & fid_l,
                                             const SEQ<TYPE_SEM_VAL> & val_l);
  static TYPE_SEM_VAL EvalSeqApply (const TYPE_SEM_SEQ & SeqV, const SEQ<TYPE_SEM_VAL> & arg_lv);
  static TYPE_SEM_VAL EvalMapApply (const TYPE_SEM_MAP & MapV, const SEQ<TYPE_SEM_VAL> & arg_lv);

  static TYPE_SEM_VAL EvalFieldSelectExpr (const TYPE_SEM_VAL & record_v, const TYPE_AS_Name & field);
#ifdef VDMPP
  static TYPE_SEM_VAL EvalFieldOBJRefSelect(const TYPE_SEM_OBJ_uRef & objref, const TYPE_AS_Name & field);
#endif // VDMPP
  static TYPE_SEM_VAL EvalFieldRecordSelect(const TYPE_DYNSEM_SEM_SemRecord & record_v, const TYPE_AS_Name & field);
  static TYPE_SEM_CompExplFN ConvertPolyToFn (const TYPE_SEM_ExplPOLY & Poly, const SEQ<TYPE_AS_Type> & inst);
  static TYPE_AS_Type SubstType (const TYPE_AS_Type & tp,
                                 const SEQ<TYPE_AS_TypeVar> & tv_l,
                                 const SEQ<TYPE_AS_Type> & tp_l);
#ifdef VDMSL
  static bool IsFunction (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name);
  static TYPE_SEM_VAL GetFunctionVal (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name);
  static bool IsOperation (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name);
  static TYPE_SEM_VAL GetOperationVal (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name);
  static bool IsPoly (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name);
  static TYPE_SEM_VAL GetPolyVal (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name);
#endif //VDMSL

  static TYPE_SEM_VAL EvalMapInverse (const TYPE_SEM_VAL & val_v);
  static TYPE_SEM_VAL EvalNumUnaryExpr (const Int & opr, const TYPE_SEM_VAL & op_v);
//  EvalPlusMinusAbs
//  EvalFloor
  static TYPE_SEM_VAL EvalNumBinaryExpr (const TYPE_SEM_VAL & op1_v, const Int & opr, const TYPE_SEM_VAL & op2_v);
  static TYPE_SEM_VAL EvalNumBinOp (const TYPE_SEM_VAL & val1, int opr, const TYPE_SEM_VAL & val2);
  static TYPE_SEM_VAL EvalIntDiv (const TYPE_SEM_VAL & val1, const TYPE_SEM_VAL & val2);
  static TYPE_SEM_VAL EvalNumRem (const TYPE_SEM_VAL & val1, const TYPE_SEM_VAL & val2);
  static TYPE_SEM_VAL EvalNumMod (const TYPE_SEM_VAL & val1, const TYPE_SEM_VAL & val2);
  static TYPE_SEM_VAL EvalLogUnaryExpr (const Int & opr, const TYPE_SEM_VAL & op_v);
  static TYPE_SEM_VAL EvalEqNeBinaryExpr(const TYPE_SEM_VAL& op1_v, const Int & opr, const TYPE_SEM_VAL& op2_v);
  static TYPE_SEM_VAL EvalSetUnaryExpr (const Int & opr, const TYPE_SEM_VAL & op_v);
  static TYPE_SEM_VAL EvalSetBinaryExpr (const TYPE_SEM_VAL & op1_v, const Int & opr, const TYPE_SEM_VAL & op2_v);
  static TYPE_SEM_VAL EvalSeqHead (const TYPE_SEM_VAL & op1_v);
  static TYPE_SEM_VAL EvalSeqTail (const TYPE_SEM_VAL & op1_v);
  static TYPE_SEM_VAL EvalSeqLen (const TYPE_SEM_VAL & op1_v);
  static TYPE_SEM_VAL EvalSeqElems (const TYPE_SEM_VAL & op1_v);
  static TYPE_SEM_VAL EvalSeqIndices (const TYPE_SEM_VAL & op1_v);
  static TYPE_SEM_VAL EvalSeqDistrConc (const TYPE_SEM_VAL & op1_v);
  static TYPE_SEM_VAL EvalSeqReverse (const TYPE_SEM_VAL & op1_v);
  static TYPE_SEM_VAL EvalSetCard (const TYPE_SEM_VAL & op1_v);
  static TYPE_SEM_VAL EvalSetPower (const TYPE_SEM_VAL & op1_v);
  static TYPE_SEM_VAL EvalSetDistrUnion (const TYPE_SEM_VAL & op1_v);
  static TYPE_SEM_VAL EvalSetDistrIntersect (const TYPE_SEM_VAL & op1_v);
  static TYPE_SEM_VAL EvalInSet (const TYPE_SEM_VAL & op1_v, const TYPE_SEM_VAL & val2);
  static TYPE_SEM_VAL EvalNotInSet (const TYPE_SEM_VAL & op1_v, const TYPE_SEM_VAL & val2);
  static TYPE_SEM_VAL EvalSetUnion (const TYPE_SEM_VAL & val1, const TYPE_SEM_VAL & val2);
  static TYPE_SEM_VAL EvalSetIntersect (const TYPE_SEM_VAL & val1, const TYPE_SEM_VAL & val2);
  static TYPE_SEM_VAL EvalSetMinus (const TYPE_SEM_VAL & val1, const TYPE_SEM_VAL & val2);
  static TYPE_SEM_VAL EvalSubSet (const TYPE_SEM_VAL & val1, const TYPE_SEM_VAL & val2);
  static TYPE_SEM_VAL EvalProperSubSet (const TYPE_SEM_VAL & val1, const TYPE_SEM_VAL & val2);
  static TYPE_SEM_VAL EvalSeqUnaryExpr (const Int & opr, const TYPE_SEM_VAL & op_v);
  static TYPE_SEM_VAL EvalSeqBinaryExpr (const TYPE_SEM_VAL & op1_v, const Int & opr, const TYPE_SEM_VAL & op2_v);
  static TYPE_SEM_VAL EvalSeqConc (const TYPE_SEM_VAL & op1_v, const TYPE_SEM_VAL & op2_v);
  static TYPE_SEM_VAL EvalMapUnaryExpr (const Int & opr, const TYPE_SEM_VAL & op_v);
  static TYPE_SEM_VAL EvalMapBinaryExpr (const TYPE_SEM_VAL & op1_v, const Int & opr, const TYPE_SEM_VAL & op2_v);
  static TYPE_SEM_VAL EvalMapMerge (const TYPE_SEM_VAL & val1, const TYPE_SEM_VAL & val2);
  static TYPE_SEM_VAL EvalMapDom (const TYPE_SEM_VAL & val1);
  static TYPE_SEM_VAL EvalMapRng (const TYPE_SEM_VAL & val1);
  static TYPE_SEM_VAL EvalMapDistrMerge (const TYPE_SEM_VAL & val1);
  static TYPE_SEM_VAL EvalMapDomRestTo (const TYPE_SEM_VAL & val_s, const TYPE_SEM_VAL & val_m);
  static TYPE_SEM_VAL EvalMapDomRestBy (const TYPE_SEM_VAL & val_s, const TYPE_SEM_VAL & val_m);
  static TYPE_SEM_VAL EvalMapRngRestTo (const TYPE_SEM_VAL & val_m, const TYPE_SEM_VAL & val_s);
  static TYPE_SEM_VAL EvalMapRngRestBy (const TYPE_SEM_VAL & val_m, const TYPE_SEM_VAL & val_s);
  static TYPE_SEM_VAL EvalComposeExpr (const TYPE_SEM_VAL & op1_v, const TYPE_SEM_VAL & op2_v);
  static TYPE_SEM_VAL EvalComposeMap (const TYPE_SEM_MAP & opv1_v, const TYPE_SEM_MAP & op2_v);
  static TYPE_SEM_VAL EvalComposeFctExpr (const TYPE_SEM_CompExplFN & fvl1R, const TYPE_SEM_CompExplFN & fvl2R);
  static TYPE_SEM_VAL EvalIterateExpr (const TYPE_SEM_VAL & op1_v, const TYPE_SEM_VAL & op2_v);
  static TYPE_SEM_VAL IterateFct (const TYPE_SEM_CompExplFN & fn_v, const TYPE_SEM_NUM & num);
  static TYPE_SEM_VAL IterateMap (const TYPE_SEM_MAP & mapv, const TYPE_SEM_NUM & num);
  static TYPE_SEM_VAL EvalEquals (const TYPE_SEM_VAL & val1, const TYPE_SEM_VAL & val2);
// IsExtFnOp
// GetExtFnOpVal
// IsExtVal
// GetExtVal
// EvalExtBody
// IsDefApproxVal
// GetApproxVal

// private
//private:
  static MAP<TYPE_AS_TypeVar,TYPE_AS_Type> MakeTypeMap (const SEQ<TYPE_AS_TypeVar> & typevars,
                                                        const SEQ<TYPE_AS_Type> & types);
  static SEQ<TYPE_AS_Type> SubstTypeList (const SEQ<TYPE_AS_Type> & tp,
                                          const SEQ<TYPE_AS_TypeVar> & tv_l,
                                          const SEQ<TYPE_AS_Type> & tp_l);
};

#endif // EVALEXPR_H

// Force emacs to use C++ mode for this file.
//// Local Variables:
//// mode: c++
//// End:
