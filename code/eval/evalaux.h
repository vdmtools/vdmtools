/***
*  * WHAT
*  *    Header for auxiliary functions
*  * FILE
*  *    $Source:$
*  * VERSION
*  *    $Revision:$
*  * DATE
*  *    $Date:$
*  * STATUS
*  *    $State:$
*  * PROJECT
*  *    VDM-SL Toolbox
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __evalaux_h__
#define __evalaux_h__
#include "metaiv.h"
#include "AS.h"

class AUX
{
public:
  static TYPE_SEM_BlkEnv CombineBlkEnv (const TYPE_SEM_BlkEnv & blk1, const TYPE_SEM_BlkEnv & blk2);
  static SET<TYPE_SEM_BlkEnv> DistribCombineBlkEnv(const SET<TYPE_SEM_BlkEnv> & blkenv_s);
  static TYPE_SEM_BlkEnv MkBlkEnv (const TYPE_AS_Name & id,
                                   const TYPE_SEM_VAL & val_v,
                                   const Generic & tp,
                                   const TYPE_SEM_Permission & perm );
  static TYPE_SEM_BlkEnv MkBlkEnvFromIdMap (const MAP<TYPE_AS_Name, TYPE_SEM_VAL> & id_m,
                                            const TYPE_SEM_Permission & perm );
  static TYPE_SEM_BlkEnv MkEmptyBlkEnv (const TYPE_SEM_Permission & perm);
#ifdef VDMSL
  static bool IsTypeStructExported (const TYPE_AS_Name & name);
  static bool IsConstructExported (const TYPE_AS_Name & loc, const TYPE_AS_Name & mod_name);
  static TYPE_AS_Name UniqueTagName (const TYPE_AS_Name & tname);
#endif // VDMSL
  static Tuple ExtractTagName (const TYPE_AS_Name& name, const SET<TYPE_AS_Name> & nm_s);
  static Tuple IsTypeDef (const TYPE_AS_Name & name);
#ifdef VDMPP
  static TYPE_AS_TypeDef ExtComp(const TYPE_AS_TypeDef & td, const TYPE_AS_Name & clnm);
#endif // VDMPP
#ifdef VDMSL
  static Tuple LookUpRenRecSel (const TYPE_AS_Name & name);
  static Tuple LookUpRename (const TYPE_AS_Name & name);
#endif // VDMSL
  static Tuple LookUpRecSel (const TYPE_AS_Name & name);
  static bool IsNat (const TYPE_SEM_VAL & val_v);
  static bool IsNatOne (const TYPE_SEM_VAL & val_v);
  static bool IsInt (const TYPE_SEM_VAL & val_v);
  static bool IsReal (const TYPE_SEM_VAL & val_v);
  static bool IsRat (const TYPE_SEM_VAL & val_v);
  static bool IsStmt(const Record& se);
  static SET<Sequence> AUX_Permute (const SEQ<TYPE_SEM_VAL> & l);
  static SEQ<TYPE_SEM_VAL> ValSetToSeq (const SET<TYPE_SEM_VAL> & set_sv);
  static SEQ<TYPE_SEM_VAL> SetToSeq (const SET<TYPE_SEM_VAL> & s);
  static SET<Sequence> SeqOfSetOf2SetOfSeqOf (const SEQ<Set> & seq_ls);
  static SEQ<Sequence> SeqOfSetOf2SeqOfSeqOf (const SEQ<Set> & seq_ls);
#ifdef VDMSL
  static bool FindTagName (const TYPE_AS_Name& name, const TYPE_AS_Type & tp);
  static bool IsDefinedInModule (const TYPE_AS_Name& name);
#endif // VDMSL
  static TYPE_AS_Name PreName(const TYPE_AS_Name & nm);
  static TYPE_AS_Name PostName(const TYPE_AS_Name & nm);
  static TYPE_AS_Name InvName(const TYPE_AS_Name & nm);
  static TYPE_AS_Name InitName(const TYPE_AS_Name & nm);
  static TYPE_AS_Name OrderName(const TYPE_AS_Name & nm);
  static TYPE_AS_Name MaxName(const TYPE_AS_Name & nm);
  static TYPE_AS_Name MinName(const TYPE_AS_Name & nm);
  static TYPE_AS_Name EqualityName(const TYPE_AS_Name & nm);
  static TYPE_AS_Id ExtractId (const TYPE_AS_Name& nm);
  static TYPE_AS_Name ExtractName(const TYPE_AS_Name & name);
  static SEQ<Char> SingleNameToString(const TYPE_AS_Name & nm);
  static TYPE_AS_Name ConstructDoubleName(const TYPE_AS_Name & nm1, const TYPE_AS_Name & nm2);
#ifdef VDMSL
  static bool IsRealName (const TYPE_AS_Name & name);
#endif // VDMSL
  static bool EqualNames (const TYPE_AS_Name & name1, const TYPE_AS_Name & name2);
#ifdef VDMSL
  static bool IsInDLDeclared (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name);
  static Tuple RenameExtractModule(const TYPE_AS_Name & name);
#endif // VDMSL

  static SET<TYPE_AS_Name> ClModNmInPatternBindList(const SEQ<TYPE_AS_PatternBind> & pat_l);
  static SET<TYPE_AS_Name> ClModNmInPatternBind(const TYPE_AS_PatternBind & patb);
  static SET<TYPE_AS_Name> ClModNmInMultBind(const TYPE_AS_MultBind & bind);
  static SET<TYPE_AS_Name> ClModNmInMultBindList(const SEQ<TYPE_AS_MultBind> & bind_l);
  static SET<TYPE_AS_Name> ClModNmInLocalDef(const SEQ<TYPE_AS_LocalDef> & localdef_l);
  static SET<TYPE_AS_Name> ClModNmInElseifExprList(const SEQ<TYPE_AS_ElseifExpr> & elsif_l);
  static SET<TYPE_AS_Name> ClModNmInCaseAltnList(const SEQ<TYPE_AS_CaseAltn> & altns);
  static SET<TYPE_AS_Name> ClModNmInExprList(const SEQ<TYPE_AS_Expr> & expr_l);
  static SET<TYPE_AS_Name> ClModNmInExpr(const TYPE_AS_Expr & e);

private:
  static SET<TYPE_SEM_BlkEnv> eset;
};
#endif // __evalaux_h__
