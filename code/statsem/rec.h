/***
*  * WHAT
*  * This module contains everything that is needed for the static checker to set
*  * the framework for recursive PO. It contains functions to build the recursion
*  * map, that will be used for generate warnings or errors and also will be later
*  * used by the POG to generate the POs.
*  * FILE
*  *    $Source:$
*  * PROJECT
*  *    VDM-SL Toolbox
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __ss_rec_h__
#define __ss_rec_h__

#include "metaiv.h"
#include "AS.h"

class SSREC
{
public:
  static Map BuildRecMap (const TYPE_AS_Name & clnm, const Map & fnm);

private:
  static Map main_RecMap (const Map & fnm);
  static Map disting_recMap(const Map & fnm, const Map & recmap);
  static Map recMapSet2Seq(const Map & fnm);
  static Map trans_RecMap(const Map & recm);
  static Map compose_RecMap(const Map & m, const Map & n);
  static Set compose_auxRecMap(const Set & trac, const Map & fnm);
  static Set compose_aux2RecMap(const SEQ<TYPE_AS_Name> & trace, const Map & fnm);

  static Map createDirectCallMap(const Map & fnm);
  static Map createIndirectCallMap(const Map & fnm);
  static Set createIndirectCallMapAux(const Set & s, const Map & fnm);
  static Map createRecMap(const Map & cm, const Map & recmap);

  static SET<TYPE_AS_Name> getFuncAppFnDef(const TYPE_AS_FnDef & fnd, const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppFnBody(const TYPE_AS_FnBody & fn, const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppExpr(const TYPE_AS_Expr & expr, const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppDefExpr(const TYPE_AS_DefExpr & expr, const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppPatternBind(const TYPE_AS_PatternBind & pb);
  static SET<TYPE_AS_Name> getFuncAppLetExpr(const TYPE_AS_LetExpr & expr, const SET<TYPE_AS_Name> & sn);
  static Tuple getFuncAppLocalDef(const SEQ<TYPE_AS_LocalDef> & vd, const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppApplyExpr(const TYPE_AS_ApplyExpr & expr, const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppIfExpr(const TYPE_AS_IfExpr & expr, const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppElseifExpr(const TYPE_AS_ElseifExpr & expr, const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppBinaryExpr(const TYPE_AS_BinaryExpr & expr, const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppTupleConstructorExpr(const TYPE_AS_TupleConstructorExpr & expr,
                                                          const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppPrefixExpr(const TYPE_AS_PrefixExpr & expr, const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppCasesExpr(const TYPE_AS_CasesExpr & expr, const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppCaseAltn(const TYPE_AS_CaseAltn & expr, const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppLetBeSTExpr(const TYPE_AS_LetBeSTExpr & expr, const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppMaplet(const TYPE_AS_Maplet & expr, const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppMapCampExpr(const TYPE_AS_MapComprehensionExpr & expr,
                                                 const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppRecordModification(const TYPE_AS_RecordModification & m,
                                                        const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppBind(const TYPE_AS_Bind & bind, const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppSetBind(const TYPE_AS_SetBind & bind, const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppTypeBind(const TYPE_AS_TypeBind & bind, const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppSeqBind(const TYPE_AS_SeqBind & bind, const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppMultBind(const TYPE_AS_MultBind & bind, const SET<TYPE_AS_Name> & sn);
  static SET<TYPE_AS_Name> getFuncAppPattern(const TYPE_AS_Pattern & pa, const SET<TYPE_AS_Name> & sn);
  static bool OtherClassApp(const TYPE_AS_Name & clnm, const TYPE_AS_Name & fnm);
  static Map removeNonRec(const Map & recmap);
  static Map recConstructMap(const SET<TYPE_AS_Name> & toVisit,
                             const SET<TYPE_AS_Name> & visited,
                             const Map & recMap);
  static TYPE_AS_Name NormalizeName(const TYPE_AS_Name & clnm, const TYPE_AS_Name & fnm);
  static TYPE_AS_Name DestroyCid(const TYPE_AS_Name & nm);
  static Tuple GetAppsFn(const TYPE_AS_Name & nm);

private:
  static SET<TYPE_AS_Name> eset;
  static Nil nil;
};
#endif // __ss_rec_h__
