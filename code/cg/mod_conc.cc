/***
*  * WHAT
*  *  This version contains:
*  *    mod_conc.cc: Implementation of mod_conc.vdm 1.20
*  *
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_conc.cc,v $
*  * VERSION
*  *    $Revision: 1.24 $
*  * DATE
*  *    $Date: 2006/07/03 05:21:15 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *
*  * PROJECT
*  *    INFORMA
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "vdmcg.h"
#include "asquotes.h"
#include "astaux.h"
#include "tools.h"
#include "MANGLE.h"
#include "CONC.h"
#include "BC.h"

// InitSpecState
void vdmcg::InitSpecState ()
{
  this->classDefs  = MAP<TYPE_AS_Name,TYPE_AS_Class>();
  this->classInfos = MAP<TYPE_AS_Name,TYPE_CONC_ClassInfo>();
  this->operConst  = MAP<TYPE_AS_Name,Int>();
}

// InitClassState
// name : AS`Name
void vdmcg::InitClassState (const TYPE_AS_Name& name)
{
  this->currentClass = name;
}

// SetDefaultCast
// n : AS`Name
void vdmcg::SetDefaultCast(const TYPE_AS_Name& n)
{
  this->defaultCast =  n;
}

// AssignOperConstants
// s : nat
// opers : map AS`Name to AS`OpDef
void vdmcg::AssignOperConstants(int s, const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & opers)
{
  int i = s;
  Set dom_opers (opers.Dom());
  Generic o;
  for (bool bb = dom_opers.First(o); bb; bb = dom_opers.Next(o))
  {
    this->operConst.ImpModify(QualifyName(o), Int(i));
    i = i + 1;
  }
}

// GetOperConst
// name : AS`Name
// ==> nat
Int vdmcg::GetOperConst(const TYPE_AS_Name & name)
{
  return this->operConst[name];
}

// ExtractGlobalInformation
// doc : AS`Document
// classes : set of AS`Name
void vdmcg::ExtractGlobalInformation(const SEQ<TYPE_AS_Class> & doc,
                                     const SET<TYPE_AS_Name> & classes_)
{
  size_t len_doc = doc.Length();
  for (size_t idx = 1; idx <= len_doc; idx++)
  {
    const TYPE_AS_Class & cl (doc[idx]);
    this->classDefs.Insert(cl.get_nm(), cl);
  }

  SET<TYPE_AS_Name> classes (classes_);
  Generic x;
  for (bool cc = classes.First(x); cc; cc = classes.Next(x) )
  {
    TYPE_CONC_ClassInfo ci;
    ci.Init(Bool(false),           // topClass
            Bool(false),           // topPerThread
            Bool(false),           // topProcThread
            Bool(false),           // newGuards
            SEQ<TYPE_AS_Name>(),   // baseClasses
            Map(),                 // accPermPred  map AS`Name to seq of AS`Expr
            Map(),                 // mutexTable   map AS`Name to set of AS`Name
            Int(0),                // nrOpers
            Int(0));               // startCount

    this->classInfos.Insert(x, ci);
  }
  ScanInheritance(classes);
  ScanSyncDefs(classes);
}

// ScanInheritance
// classes : set of AS`Name
void vdmcg::ScanInheritance(const SET<TYPE_AS_Name> & classes)
{
  SET<TYPE_AS_Name> classes_q (classes);
  Generic cl;
  for (bool bb = classes_q.First(cl); bb; bb = classes_q.Next(cl) )
  {
    SEQ<TYPE_AS_Name> inhl (GetOrderedSupers(cl));
    TYPE_CONC_ClassInfo ci (this->classInfos[cl]);
    ci.set_baseClasses(inhl);
    TYPE_AS_Class cls (this->classDefs[cl]);
    TYPE_AS_Definitions defs (cls.get_defs());
    Map opm (defs.get_opm());
    ci.set_nrOpers(Int(opm.Dom().Card()));
    this->classInfos.ImpModify(cl, ci);
  }
}

// ScanSyncDefs
// classes : set of AS`Name
void vdmcg::ScanSyncDefs(const SET<TYPE_AS_Name> & classes)
{
  Int level(0);
  SET<TYPE_AS_Name> toScan (classes);

  Int startCount;

  while(!toScan.IsEmpty())
  {
    SET<TYPE_AS_Name> top;
    Generic c;
    for (bool bb = toScan.First(c); bb; bb = toScan.Next(c) )
    {
      SEQ<TYPE_AS_Name> cl (this->classInfos[c].get_baseClasses());
      if ((level == (int)cl.Length()) && !isInterface(c))
        top.Insert(c);
    }

    Generic cl;
    for (bool cc = top.First(cl); cc; cc = top.Next(cl) )
    {
      this->defaultCast = TYPE_AS_Name(cl);
      SET<TYPE_AS_Permission> ppDef;
      SET<TYPE_AS_Mutex> mutexDef;

      TYPE_AS_Class cls (this->classDefs[cl]);

      TYPE_AS_Definitions defs (cls.get_defs());
      SEQ<TYPE_AS_SyncDef> elems (defs.get_syncs());
      Generic p;
      for (bool dd = elems.First(p); dd; dd = elems.Next(p) )
      {
        TYPE_AS_SyncDef sd (p);
        switch(sd.GetTag()) {
          case TAG_TYPE_AS_Permission: {
            ppDef.Insert(p);
            break;
          }
          case TAG_TYPE_AS_Mutex: {
            mutexDef.Insert(p);
            break;
          }
        }
      }

      MAP<TYPE_AS_Name,Sequence> guards;
      Generic pelem;
      for (bool ee = ppDef.First(pelem); ee; ee = ppDef.Next(pelem) )
      {
        TYPE_AS_Permission permpred (pelem);
        const TYPE_AS_Name & spec (permpred.GetRecord(pos_AS_Permission_spec));
        const TYPE_AS_Expr & guard (permpred.GetRecord(pos_AS_Permission_guard));
        guards.Insert(QualifyName(spec), Sequence().ImpAppend(TYPE_AS_GuardExpr().Init(guard, cl)));
      }

      MAP<TYPE_AS_Name,Set> mutexTbl (CreateMutexTable(mutexDef));
      Bool hasNewGuards = ( !mutexTbl.Dom().IsEmpty() || !guards.Dom().IsEmpty() );
      if (level == Int(0)) {
        startCount = Int(0);
      }
      else
      {
        Map guardstemp (guards);
        TYPE_AS_Name directBase (this->classInfos[cl].get_baseClasses().Hd());
        if (this->classInfos.DomExists(directBase)) {
          TYPE_CONC_ClassInfo dci (this->classInfos[directBase]);
          guards = MergePermPreds(dci.get_accPermPred(), guardstemp);
          mutexTbl = MergeMutexTable(dci.get_mutexTable(), mutexTbl);
          startCount = dci.get_startCount() + dci.get_nrOpers();
        }
      }

      Set basecls (this->classInfos[cl].get_baseClasses().Elems());

      bool doexist1 = false;
      bool doexist2 = false;
      Generic a;
      for (bool ff = basecls.First(a); ff; ff = basecls.Next(a))
      {
// 20081114 -->
        if (!this->classInfos.DomExists(a)) continue;
// <-- 20081114
        if (this->classInfos[a].get_topPerThread().GetValue())
          doexist1 = true;
        if (this->classInfos[a].get_topProcThread().GetValue())
          doexist2 = true;
      }

      Bool isTopPerThread;
      Bool isTopProcThread;
      Generic thread (defs.get_threaddef());
      if (thread.IsNil()) {
        isTopPerThread = false;
        isTopProcThread = false;
      }
      else
      {
        TYPE_AS_ThreadDef td (thread);
        switch(td.GetTag()) {
          case TAG_TYPE_AS_PerObl: {
            isTopPerThread = !doexist1;
            isTopProcThread = false;
            break;
          }
          default: { // AS`Stmt
            isTopPerThread = false;
            isTopProcThread = !doexist2;
            break;
          }
        }
      }

      AssignOperConstants(startCount, defs.get_opm());
      bool isTopClass = true;
      SEQ<TYPE_AS_Name> posSupers (GetOrderedSupers(cl));
      Generic g;
      for (bool gg = posSupers.First(g); gg && isTopClass; gg = posSupers.Next(g))
        isTopClass = isInterface(g);

      TYPE_CONC_ClassInfo ci (this->classInfos[cl]);
      ci.set_topClass(Bool(isTopClass));
      ci.set_topPerThread(Bool(isTopPerThread));
      ci.set_topProcThread(Bool(isTopProcThread));
      ci.set_newGuards(Bool(hasNewGuards));
      ci.set_accPermPred(guards);
      ci.set_mutexTable(mutexTbl);
      ci.set_startCount(startCount);

      this->classInfos.ImpModify(cl,ci);
    }
    toScan.ImpDiff(top);

    Set interfaces;
    for (bool hh = toScan.First(c); hh; hh = toScan.Next(c) )
    {
      SEQ<TYPE_AS_Name> c_l (this->classInfos[c].get_baseClasses());
      if ((level == (int)c_l.Length()) && isInterface(c))
        interfaces.Insert(c);
    }

    for (bool ii = interfaces.First(cl); ii; ii = interfaces.Next(cl))
    {
      TYPE_CONC_ClassInfo ci (this->classInfos[cl]);
      ci.set_topClass(Bool(false));
      ci.set_topPerThread(Bool(false));
      ci.set_topProcThread(Bool(false));
      ci.set_newGuards(Bool(false));
      ci.set_accPermPred(Map());
      ci.set_mutexTable(Map());
      ci.set_startCount(Int(0));

      this->classInfos.ImpModify(cl,ci);
    }
    toScan.ImpDiff(interfaces);
    level = level + (Int) 1;
  }
}

// MergePermPreds
// base : PredTable (map AS`Name to seq of AS`Expr)
// derived : PredTable (map AS`Name to seq of AS`Expr)
// ==> PredTable
MAP<TYPE_AS_Name,Sequence> vdmcg::MergePermPreds (
                            const MAP<TYPE_AS_Name,Sequence> & base,
                            const MAP<TYPE_AS_Name,Sequence> & derived)
{
  MAP<TYPE_AS_Name,Sequence> combined (base);

  Set dom_derived (derived.Dom());
  Generic nm;
  for (bool bb = dom_derived.First(nm); bb; bb = dom_derived.Next(nm) )
  {
    if (combined.DomExists(nm))
    {
      Sequence s (combined[nm]);
      combined.ImpModify(nm, s.ImpConc(derived[nm]));
    }
    else
      combined.ImpModify(nm, derived[nm]);
  }
  return combined;
}

// CreateMutexTable
// rawMutexDef : set of AS`Mutex
// ==> MutexTable
MAP<TYPE_AS_Name,Set> vdmcg::CreateMutexTable (const SET<TYPE_AS_Mutex> & rawMutexDef)
{
  SET<TYPE_AS_Mutex> mutexDef;
  SET<TYPE_AS_Mutex> rawMutexDef_q (rawMutexDef);
  Generic mu;
  for (bool bb = rawMutexDef_q.First(mu); bb; bb = rawMutexDef_q.Next(mu) )
  {
    TYPE_AS_Mutex mutex (mu);
    const Generic & x (mutex.GetField(pos_AS_Mutex_ops));
    const TYPE_CI_ContextId & cid (mutex.GetInt(pos_AS_Mutex_cid));
    SEQ<TYPE_AS_Name> ops;
    if (x.IsNil())
      ops = AllOperations(this->defaultCast);
    else
      ops = x;

    mutexDef.Insert(TYPE_AS_Mutex().Init(QualifyNameSeq(ops), cid));
  }

  SET<TYPE_AS_Name> dunion;
  Generic y;
  for (bool cc = mutexDef.First(y); cc; cc = mutexDef.Next(y) )
  {
    TYPE_AS_Mutex mut (y);
    const SEQ<TYPE_AS_Name> & ops (mut.GetSequence(pos_AS_Mutex_ops));
    dunion.ImpUnion(ops.Elems());
  }

  MAP<TYPE_AS_Name,Set> mutexTbl;
  Generic x;
  for (bool dd = dunion.First(x); dd; dd = dunion.Next(x) )
  {
    mutexTbl.Insert(x, Set());
  }

  Generic outer;
  for (bool ee = mutexDef.First(outer); ee; ee = mutexDef.Next(outer) )
  {
    TYPE_AS_Mutex mut (outer);
    const SEQ<TYPE_AS_Name> & ops (mut.GetSequence(pos_AS_Mutex_ops));
    size_t len_ops = ops.Length();
    for (size_t idx = 1; idx <= len_ops; idx++) 
    {
      const TYPE_AS_Name & is (ops[idx]);
      SET<TYPE_AS_Name> u (mutexTbl[is]);
      u.ImpUnion(ops.Elems());
      mutexTbl.ImpModify(is, u);
    }
  }
  return mutexTbl;
}

// MergeMutexTable
// base : MutexTable
// derived : MutexTable
// ==> MutexTable
MAP<TYPE_AS_Name,Set> vdmcg::MergeMutexTable (const MAP<TYPE_AS_Name,Set> & base,
                                              const MAP<TYPE_AS_Name,Set> & derived)
{
  MAP<TYPE_AS_Name,Set> combined (base);
  Set dom_derived (derived.Dom());
  Generic nm;
  for (bool bb = dom_derived.First(nm); bb; bb = dom_derived.Next(nm) )
  {
    if (combined.DomExists(nm))
    {
      Set s (combined[nm]);
      combined.ImpModify(nm, s.ImpUnion(derived[nm]));
    }
    else
      combined.ImpModify(nm, derived[nm]);
  }
  return combined;
}

// ClassDefiningOper
// opnm : AS`Name
// ==> AS`Id
TYPE_AS_Id vdmcg::ClassDefiningOper (const TYPE_AS_Name & opnm)
{
  SEQ<TYPE_AS_Name> searchList;
  searchList.ImpAppend(this->defaultCast);
  searchList.ImpConc(this->classInfos[this->defaultCast].get_baseClasses());

  size_t len_searchList = searchList.Length();
  for (size_t i = 1; i <= len_searchList; i++) {
    const TYPE_AS_Name & clsnm (searchList[i]);
    const TYPE_AS_Class & cls (this->classDefs[clsnm]);
    const TYPE_AS_Definitions & defs (cls.GetRecord(pos_AS_Class_defs));
    const Map & opm (defs.GetMap(pos_AS_Definitions_opm));
    if(opm.DomExists(opnm))
      return ASTAUX::GetFirstId(clsnm);
  }
  ReportError(L"ClassDefiningOper in mod_conc.cc");
  return TYPE_AS_Name(); // dummy
}

// AllOperations
// cnamee : AS`Name
// ==> seq of AS`Name
SEQ<TYPE_AS_Name> vdmcg::AllOperations (const TYPE_AS_Name & cname)
{
  SEQ<TYPE_AS_Name> searchList;
  searchList.ImpAppend(cname);
  searchList.ImpConc(this->classInfos[cname].get_baseClasses());

  SET<TYPE_AS_Name> operSet;
  size_t len_searchList = searchList.Length();
  for (size_t idx = 1; idx <= len_searchList; idx++)
  {
    TYPE_AS_Class cls (this->classDefs[searchList[idx]]);
    const TYPE_AS_Definitions & defs (cls.GetRecord(pos_AS_Class_defs));
    const Map & opm (defs.GetMap(pos_AS_Definitions_opm));
    operSet.ImpUnion(opm.Dom());
  }
  return SetToSeq(operSet);
}

// QualifyName
// opnm : AS`Name
// ==> AS`Name
TYPE_AS_Name vdmcg::QualifyName(const TYPE_AS_Name & opnm)
{
  const TYPE_AS_Ids & ids (opnm.GetSequence(pos_AS_Name_ids));

  if (ids.Length() == 1) {
    TYPE_AS_Ids seq;
    seq.ImpAppend(ClassDefiningOper(opnm));
    seq.ImpConc(ids);
    return ASTAUX::MkNameFromIds(seq, opnm.GetInt(pos_AS_Name_cid));
  }
  else
    return opnm;
}

// QualifyNameSeq
// ns : seq of AS`Name
// ==> seq of AS`Name
SEQ<TYPE_AS_Name> vdmcg::QualifyNameSeq(const SEQ<TYPE_AS_Name> & ns)
{
  SEQ<TYPE_AS_Name> res;
  size_t len_ns = ns.Length();
  for (size_t idx = 1; idx <= len_ns; idx++)
    res.ImpAppend(QualifyName(ns[idx]));
  return res;
}

// SetToSeq
// nameset : set of AS`Name
// ==> seq of AS`Name
SEQ<TYPE_AS_Name> vdmcg::SetToSeq(const SET<TYPE_AS_Name> & nameset)
{
  SET<TYPE_AS_Name> nameset_q (nameset);
  SEQ<TYPE_AS_Name> nameseq;
  Generic nm;
  for (bool bb = nameset_q.First(nm); bb; bb = nameset_q.Next(nm) )
      nameseq.ImpAppend(nm);
  return nameseq;
}

// GenConcOper
// nm : AS`Name
// fb : seq of CPP`Stmt
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenConcOper(const TYPE_AS_Name & nm, const SEQ<TYPE_CPP_Stmt> & fb)
{
  TYPE_AS_Id id (ASTAUX::GetFirstId(this->currentClass).ImpConc(ASTAUX::MkId(L"Sentinel")));

  TYPE_CPP_Expr sentinel (vdm_BC_GenIdentifier(ASTAUX::MkId(L"sentinel")));
  TYPE_CPP_Expr castedSentinel (this->classInfos[this->currentClass].get_topClass().GetValue()
                      ? sentinel
                      : vdm_BC_GenCastExpr(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier( id )), sentinel));

  TYPE_AS_Name javaCompliantNm;
  if (MANGLE::IsMangled(nm))
    javaCompliantNm = MakeNameJavaCompliant(nm);
  else
    javaCompliantNm = nm;

  TYPE_CPP_Stmt fenter (vdm_BC_GenFctCallObjMemAccStmt(
                          vdm_BC_GenIdentifier(ASTAUX::MkId(L"sentinel")),
                          ASTAUX::MkId(L"entering"),
                          SEQ<TYPE_CPP_Expr>().ImpAppend(
                            vdm_BC_GenObjectMemberAccess(castedSentinel, vdm_BC_Rename(javaCompliantNm)))));

  TYPE_CPP_Stmt fleave (vdm_BC_GenFctCallObjMemAccStmt(
                          vdm_BC_GenIdentifier(ASTAUX::MkId(L"sentinel")),
                          ASTAUX::MkId(L"leaving"),
                          SEQ<TYPE_CPP_Expr>().ImpAppend(
                            vdm_BC_GenObjectMemberAccess(castedSentinel, vdm_BC_Rename(javaCompliantNm)))));
  TYPE_CPP_Stmt cs (vdm_BC_GenBlock(type_dL().ImpAppend(fleave)));

  SEQ<TYPE_CPP_Stmt> res;
  res.ImpAppend(fenter).ImpAppend(vdm_BC_GenTryBlock(fb, SEQ<TYPE_CPP_Handler>(), cs));
  return res;
}

// GenGuardExpr
// expr : AS`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenGuardExpr(const TYPE_AS_Expr & expr)
{
  wstring name;
  Set elems; // set of AS`Name
  switch (expr.GetTag ()) {
    case TAG_TYPE_AS_ActExpr: {
        name = L"act";
        elems = expr.GetSequence(pos_AS_ActExpr_mthd).Elems();
        break;
    }
    case TAG_TYPE_AS_FinExpr: {
        name = L"fin";
        elems = expr.GetSequence(pos_AS_FinExpr_mthd).Elems();
        break;
    }
    case TAG_TYPE_AS_ActiveExpr: {
        name = L"active";
        elems = expr.GetSequence(pos_AS_ActiveExpr_mthd).Elems();
        break;
    }
    case TAG_TYPE_AS_WaitingExpr: {
        name = L"waiting";
        elems = expr.GetSequence(pos_AS_WaitingExpr_mthd).Elems();
        break;
    }
    case TAG_TYPE_AS_ReqExpr: {
        name = L"req";
        elems = expr.GetSequence(pos_AS_ReqExpr_mthd).Elems();
        break;
    }
    default:
        ReportError(L"GenGuardExpr");
        return TYPE_CPP_Expr();
  }

  if (vdm_CPP_isCPP()) {
    return NotSupported(name + L". expression", expr);
  }
  else { // java
    TYPE_CPP_Identifier gname (vdm_BC_GenIdentifier(ASTAUX::MkId(name)));
    SEQ<TYPE_CPP_Expr> exprlist;
    Generic o;
    for (bool bb = elems.First(o); bb; bb = elems.Next(o) ) {
      TYPE_AS_Ids ids (((TYPE_AS_Name) o).get_ids());
      TYPE_CPP_Identifier mthdName (vdm_BC_GenIdentifier(ids[ids.Length()]));
      TYPE_CPP_Expr gexpr (vdm_BC_GenArrayApply(
                             vdm_BC_GenObjectMemberAccess(vdm_BC_GenIdentifier(ASTAUX::MkId(L"sentinel")), gname),
                             vdm_BC_GenObjectMemberAccess(GenCastedSentinel(o), mthdName)));
      exprlist.ImpAppend(gexpr);
    }

    return vdm_BC_GenNewExpr(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Integer"))),
                             SEQ<TYPE_CPP_Expr>().ImpAppend(BuildSumCPPExpr(exprlist)));
  }
}

// BuildSumCPPExpr
// exps : seq of CPP`Expr
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::BuildSumCPPExpr(const SEQ<TYPE_CPP_Expr> & exps)
{
  if (exps.Length() == 1)
    return exps.Hd();
  else
    return vdm_BC_GenPlus( exps.Hd(), BuildSumCPPExpr( exps.Tl()));
}

// GenCastedSentinel
// n : AS`Name
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenCastedSentinel(const TYPE_AS_Name& n)
{
  TYPE_AS_Id castTo;
  if (n.get_ids().Length() == 1)
    castTo = ASTAUX::GetFirstId(this->defaultCast).ImpConc(ASTAUX::MkId(L"Sentinel"));
  else
    castTo = ASTAUX::GetFirstId(n).ImpConc(ASTAUX::MkId(L"Sentinel"));

  TYPE_CPP_Expr sentinel (vdm_BC_GenIdentifier(ASTAUX::MkId(L"sentinel")));
  TYPE_CPP_Expr castedSentinel (this->classInfos[this->currentClass].get_topClass().GetValue()
                      ? sentinel
                      : vdm_BC_GenCastExpr(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier( castTo )), sentinel));
  return castedSentinel;
}

// GenEvaluatePP
// cname : AS`Name
// ==> seq of CPP`MemberDeclaration
SEQ<TYPE_CPP_MemberDeclaration> vdmcg::GenEvaluatePP(const TYPE_AS_Name & cname)
{
  Map accPermPred (this->classInfos[cname].get_accPermPred());
  if (!this->classInfos[cname].get_newGuards() && !this->classInfos[cname].get_topClass())
    return SEQ<TYPE_CPP_MemberDeclaration>();
  else
  {
    // map AS`Name to set of AS`Name
    Map mutexTable (this->classInfos[cname].get_mutexTable());
    TYPE_CI_ContextId natCid (GetCI().PushCGType(mk_REP_NumericTypeRep(Int(NAT))));
    TYPE_CI_ContextId boolCid (GetCI().PushCGType(mk_REP_BooleanTypeRep()));
    TYPE_AS_RealLit zero (TYPE_AS_RealLit().Init(Real(0), natCid));

    Set dom_mutexTable (mutexTable.Dom());
    Generic guardedOper;
    for (bool bb = dom_mutexTable.First(guardedOper); bb; bb = dom_mutexTable.Next(guardedOper) )
    {
      SEQ<TYPE_AS_Name> actseq;
      Generic n;
      SET<TYPE_AS_Name> mtgo (mutexTable[guardedOper]);
      for (bool cc = mtgo.First(n); cc; cc = mtgo.Next(n) )
      {
        actseq.ImpAppend(n);
      }

      TYPE_AS_BinaryExpr completeExpr;
      completeExpr.Init(TYPE_AS_ActiveExpr().Init(actseq , natCid), Int(EQ), zero, boolCid);

      TYPE_AS_Name fullOperName (guardedOper);
      if (accPermPred.Dom().InSet(fullOperName))
        accPermPred.ImpModify(fullOperName,((Sequence) accPermPred[fullOperName]).ImpAppend(completeExpr));
      else
        accPermPred.ImpModify(fullOperName, Sequence().ImpAppend(completeExpr));
    }

    SEQ<TYPE_CPP_Stmt> swst;
    if (!accPermPred.Dom().IsEmpty())
    {
      SEQ<TYPE_CPP_CaseStmt> stmt_l;
      Set dom_accPermPred (accPermPred.Dom());
      Generic nm;
      for (bool bb = dom_accPermPred.First(nm); bb; bb = dom_accPermPred.Next(nm) )
      {
        this->defaultCast = cname;
        Tuple cgee (CGExprExcl(BuildConjGuardExpr(accPermPred[nm]), ASTAUX::MkId(L"res"), nil));
        const TYPE_CPP_Expr & res_v (cgee.GetRecord(1));
        const SEQ<TYPE_CPP_Stmt> & res_stmt (cgee.GetSequence(2));
        stmt_l.ImpAppend(vdm_BC_GenCaseStmt(vdm_BC_GenIntegerLit( GetOperConst( nm ) ),
                                            vdm_BC_GenBlock(res_stmt.Conc(mk_sequence(vdm_BC_GenReturnStmt(res_v))))));
      }
      swst.ImpAppend(vdm_BC_GenSwitchStmt(vdm_BC_GenIdentifier(ASTAUX::MkId(L"fnr")), stmt_l ));
    }

    TYPE_CPP_Expr newExpr (GenBoolExpr(vdm_BC_GenBoolLit(Bool(true))));
    TYPE_CPP_Stmt retStmt (vdm_BC_GenReturnStmt( newExpr ));
    TYPE_CPP_Modifier pub (vdm_BC_GenModifier(quote_PUBLIC));
    SEQ<TYPE_CPP_MemberDeclaration> res;

    TYPE_CPP_FunctionDefinition fctdef (vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
                                          SEQ<TYPE_CPP_Modifier>().ImpAppend(pub),
                                          type_dL().ImpAppend(GenBoolType()),
                                          vdm_BC_GenFctDecl(
                                            vdm_BC_GenIdentifier(ASTAUX::MkId(L"evaluatePP")),
                                            type_dL().ImpAppend(
                                              vdm_BC_GenArgDecl(
                                                SEQ<TYPE_CPP_DeclSpecifier>().ImpAppend(GenSmallIntType()),
                                                vdm_BC_GenIdentifier(ASTAUX::MkId(L"fnr"))))),
                                          GenExceptionsHdr(),
                                          vdm_BC_GenBlock( swst.ImpAppend(retStmt) )));
    res.ImpAppend(fctdef);
    return res;
  }
}

// BuildConjGuardExpr
// exps : seq of AS`Expr
// ==> AS`Expr
TYPE_CPP_Expr vdmcg::BuildConjGuardExpr(const SEQ<TYPE_AS_Expr> & exps)
{
  if (exps.Length() == 1)
    return exps.Hd();
  else
  {
    TYPE_CI_ContextId boolCid (GetCI().PushCGType(mk_REP_BooleanTypeRep()));
    return TYPE_AS_BinaryExpr().Init(exps.Hd(), Int(AND), BuildConjGuardExpr(exps.Tl()), boolCid );
  }
}

// GenLocalSentinel
// cname : AS`Name
// defs : AS`Definitions
// ==> seq of CPP`MemberDeclaration
SEQ<TYPE_CPP_MemberDeclaration> vdmcg::GenLocalSentinel(const TYPE_AS_Name & cname,
                                                        const TYPE_AS_Definitions & defs)
{
  Map oper (defs.get_opm());
  SEQ<TYPE_CPP_MemberDeclaration> pr_l;
  type_dL cst_l;
  TYPE_AS_Id localClassName (ASTAUX::GetFirstId(cname));
  localClassName.ImpConc(ASTAUX::MkId(L"Sentinel"));

  InsertImport(SEQ<Char>(L"jp.vdmtools.VDM.Sentinel"));

  if (this->classInfos[cname].get_topClass().GetValue())
  {
     pr_l.ImpAppend(vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(),
                      type_dL().ImpAppend(vdm_BC_GenModifier(quote_VOLATILE)).ImpAppend(
                        vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(localClassName))),
                      Sequence().ImpAppend(
                        vdm_BC_GenInitDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"sentinel")), Nil() ))));
  }

  if (this->classInfos[cname].get_topPerThread().GetValue())
  {
    pr_l.ImpAppend(vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(),
                     type_dL().ImpAppend(
                       vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"PeriodicThread")))),
                       Sequence().ImpAppend(
                         vdm_BC_GenInitDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"perThread")), Nil())))) ;
    InsertImport(SEQ<Char>(L"jp.vdmtools.VDM.PeriodicThread"));
  }

  if (this->classInfos[cname].get_topProcThread().GetValue())
  {
      pr_l.ImpAppend(vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(),
                       type_dL().ImpAppend(
                         vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"VDMThread" )))),
                       Sequence().ImpAppend(
                         vdm_BC_GenInitDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"procThread")), Nil() ))));
    InsertImport(SEQ<Char>(L"jp.vdmtools.VDM.VDMThread"));
  }

  this->defaultCast = cname;

  Set dom_oper (oper.Dom());
  Generic op;
  for (bool bb = dom_oper.First(op); bb; bb = dom_oper.Next(op) )
  {
    TYPE_AS_Name javaCompliantOp;
    if (MANGLE::IsMangled(op))
      javaCompliantOp = MakeNameJavaCompliant(op);
    else
      javaCompliantOp = op;

    SEQ<TYPE_CPP_DeclSpecifier> mods;
    mods.ImpAppend(vdm_BC_GenModifier(quote_PUBLIC));
    mods.ImpAppend(vdm_BC_GenModifier(quote_FINAL));
    mods.ImpAppend(GenSmallIntType());
    TYPE_CPP_IdentDeclaration vari (vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(),
                                      mods,
                                      vdm_BC_Rename(javaCompliantOp),
                                      vdm_BC_GenAsgnInit(vdm_BC_GenIntegerLit(GetOperConst(QualifyName(op))))));
    cst_l.ImpAppend(vari);
  }

  SEQ<TYPE_CPP_DeclSpecifier> mods;
  mods.ImpAppend(vdm_BC_GenModifier(quote_PUBLIC));
  mods.ImpAppend(vdm_BC_GenModifier(quote_FINAL));
  mods.ImpAppend(GenSmallIntType());
  cst_l.ImpAppend(vdm_BC_GenIdentDecl(SEQ<TYPE_CPP_Annotation>(),
                    mods,
                    vdm_BC_GenIdentifier(ASTAUX::MkId(L"nr_functions")),
                    vdm_BC_GenAsgnInit(
                      vdm_BC_GenIntegerLit(cst_l.Length() + this->classInfos[cname].get_startCount()))));

  SEQ<TYPE_CPP_Expr> initArgs;
  initArgs.ImpAppend(vdm_BC_GenIdentifier(ASTAUX::MkId(L"nr_functions")));
  initArgs.ImpAppend(vdm_BC_GenIdentifier(ASTAUX::MkId(L"instance")));

  TYPE_CPP_Expr initCall (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"init")), initArgs));

  TYPE_CPP_FunctionDefinition constrDef (vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
                                           SEQ<TYPE_CPP_Modifier>().ImpAppend(vdm_BC_GenModifier(quote_PUBLIC)),
                                           type_dL(),
                                           vdm_BC_GenFctDecl(
                                             vdm_BC_GenIdentifier( localClassName ),
                                             type_dL().ImpAppend(
                                               vdm_BC_GenArgDecl(
                                                 SEQ<TYPE_CPP_DeclSpecifier>().ImpAppend(
                                                   vdm_BC_GenTypeSpecifier(
                                                     vdm_BC_GenIdentifier(ASTAUX::MkId(L"EvaluatePP")))),
                                                 vdm_BC_GenIdentifier(ASTAUX::MkId(L"instance"))) )),
                                           GenExceptionsHdr(),
                                           vdm_BC_GenBlock(type_dL().ImpAppend(vdm_BC_GenExpressionStmt(initCall)))));
 
  TYPE_CPP_FunctionDefinition defaultConstrDef (vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
                                                  SEQ<TYPE_CPP_Modifier>().ImpAppend(vdm_BC_GenModifier(quote_PUBLIC)),
                                                  type_dL(),
                                                  vdm_BC_GenFctDecl(vdm_BC_GenIdentifier( localClassName ), type_dL()),
                                                  GenExceptionsHdr(),
                                                  vdm_BC_GenBlock( type_dL() )));

  TYPE_CPP_Identifier parent;
  if (this->classInfos[cname].get_topClass().GetValue())
    parent = vdm_BC_GenIdentifier(ASTAUX::MkId(L"Sentinel"));
  else {
    SEQ<TYPE_AS_Name> superClasses (GetOrderedSupers(cname));
    Generic g;
    for (bool cc = superClasses.First(g); cc && isInterface(g); cc = superClasses.Next(g))
     ;
    parent = vdm_BC_GenIdentifier(ASTAUX::GetFirstId(g).ImpConc(ASTAUX::MkId(L"Sentinel")));
  }

  TYPE_CPP_ClassHead classHd (vdm_BC_GenJavaClassHead(
                                SEQ<TYPE_CPP_Annotation>(),
                                SEQ<TYPE_CPP_Modifier>(),
                                vdm_BC_GenIdentifier( localClassName ),
                                SEQ<TYPE_CPP_BaseSpecifier>().ImpAppend(vdm_BC_GenClass2Package(parent)),
                                Nil(), Nil()));
  TYPE_CPP_ClassSpecifier cs (vdm_BC_GenClassSpecifier(
                                classHd,
                                cst_l.ImpAppend(defaultConstrDef).ImpAppend(constrDef )));

  pr_l.ImpAppend(vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(),
                                            type_dL().ImpAppend(vdm_BC_GenTypeSpecifier(cs)), Nil()));

  return pr_l;
}

// MakeNameJavaCompliant
// p_nm : AS`Name
// -> AS`Name
TYPE_AS_Name vdmcg::MakeNameJavaCompliant(const TYPE_AS_Name& p_nm)
{
  TYPE_AS_Ids ids (p_nm.get_ids());
  TYPE_AS_Ids l_newIds;
  Generic id;
  for (bool bb = ids.First(id); bb; bb = ids.Next(id))
    l_newIds.ImpAppend(MakeStringJavaCompliant(id));

  TYPE_AS_Name result (p_nm);
  result.set_ids(l_newIds);
  return result;
}

// MakeStringJavaCompliant
// p_str : seq of char
// -> seq of char
TYPE_AS_Id vdmcg::MakeStringJavaCompliant(const TYPE_AS_Id & p_str)
{
  if (p_str.IsEmpty())
    return p_str;
  else
  {
    TYPE_AS_Id result;
    size_t len_p_str = p_str.Length();
    for (size_t idx = 1; idx <= len_p_str; idx++)
      result.ImpConc(MakeCharJavaCompliant(Char(p_str[idx])));
    return result;
  }
}

// MakeCharJavaCompliant
// p_c : char
// -> seq of char
TYPE_AS_Id vdmcg::MakeCharJavaCompliant(const Char& p_c)
{
  if (p_c.GetValue() == L'#')
    return ASTAUX::MkId(L"__");
  else if (p_c.GetValue() == L'|')
    return ASTAUX::MkId(L"_");
  else if (p_c.GetValue() == L'`')
    return ASTAUX::MkId(L"___");
  else if (p_c.GetValue() == L'-') // 20080215
    return ASTAUX::MkId(L"_");
  else
    return TYPE_AS_Id().ImpAppend(p_c);
}

// GenConcInterface
// cname : AS`Name
// ==> seq of CPP`PackageName
SEQ<TYPE_CPP_PackageName> vdmcg::GenConcInterface(const TYPE_AS_Name & cname)
{
  SEQ<TYPE_CPP_PackageName> interfaces;

  if (this->classInfos[cname].get_topClass().GetValue())
    interfaces.ImpAppend(
      vdm_BC_GenClass2Package(vdm_BC_GenIdentifier(ASTAUX::MkId(L"EvaluatePP"))));

  if (this->classInfos[cname].get_topProcThread().GetValue())
    interfaces.ImpAppend(vdm_BC_GenSimplePackageName(ASTAUX::MkId(L"Runnable")));

  return interfaces;
}

// GenSetSentinelMethod
// cname : AS`Name
// -> CPP`MemberDeclaration
TYPE_CPP_MemberDeclaration vdmcg::GenSetSentinelMethod(const TYPE_AS_Name & cname)
{
  TYPE_AS_Id localClassName (ASTAUX::GetFirstId(cname).ImpConc(ASTAUX::MkId(L"Sentinel")));

  TYPE_CPP_Stmt body (vdm_BC_GenAsgnStmt(vdm_BC_GenIdentifier(ASTAUX::MkId(L"sentinel")),
                                         vdm_BC_GenNewExpr(
                                           vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(localClassName)),
                                           SEQ<TYPE_CPP_Expr>().ImpAppend (GenThis()))));
  TYPE_CPP_ExceptionDeclaration expdecl (vdm_BC_GenExceptionDeclaration(
                                           SEQ<TYPE_CPP_TypeSpecifier>().ImpAppend(
                                             vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Exception")))),
                                             vdm_BC_GenIdentifier(ASTAUX::MkId(L"e"))));

  TYPE_CPP_Stmt stmt (vdm_BC_GenExpressionStmt(vdm_BC_GenFctCall(
                                                 vdm_BC_GenIdentifier(ASTAUX::MkId(L"System.out.println")),
                                                 SEQ<TYPE_CPP_Expr>().ImpAppend(
                                                   vdm_BC_GenFctCall(
                                                     vdm_BC_GenIdentifier(ASTAUX::MkId(L"e.getMessage")),
                                                     SEQ<TYPE_CPP_Expr>())))));

  TYPE_CPP_Identifier methodName (vdm_BC_GenIdentifier(ASTAUX::MkId(L"setSentinel")));

  SEQ<TYPE_CPP_Handler> handlers;
  handlers.ImpAppend(vdm_BC_GenHandler(expdecl, stmt));

  TYPE_CPP_FctDecl decl (vdm_BC_GenFctDecl(methodName, type_dL()));

  return vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
                          SEQ<TYPE_CPP_Modifier>().ImpAppend(vdm_BC_GenModifier(quote_PUBLIC)),
                          type_dL().ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenVoid())),
                          decl,
                          Nil(),
                          vdm_BC_GenTryBlock(type_dL().ImpAppend(body), handlers, Nil()));
}

// GenProceduralThread
// stmt : AS`ThreadDef
// ==> seq of CPP`FunctionDefinition
SEQ<TYPE_CPP_FunctionDefinition> vdmcg::GenProceduralThread(const TYPE_AS_ThreadDef & stmt)
{
/*
  public void run () {
    try {
      ...  // generateded thread statements
    }
    catch (Throwable e) {
      System.out.println(e.getMessage());
    }
  }
*/

  TYPE_CPP_ExceptionDeclaration expdecl (vdm_BC_GenExceptionDeclaration(
                                           SEQ<TYPE_CPP_TypeSpecifier>().ImpAppend(
                                             vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Throwable")))),
                                           vdm_BC_GenIdentifier(ASTAUX::MkId(L"e"))));

  TYPE_CPP_Stmt println (vdm_BC_GenExpressionStmt(vdm_BC_GenFctCall(
                                                    vdm_BC_GenIdentifier(ASTAUX::MkId(L"System.out.println")),
                                                    SEQ<TYPE_CPP_Expr>().ImpAppend(
                                                      vdm_BC_GenFctCall(
                                                        vdm_BC_GenIdentifier(ASTAUX::MkId(L"e.getMessage")),
                                                        SEQ<TYPE_CPP_Expr>())))));

  SEQ<TYPE_CPP_Handler> handlers;
  handlers.ImpAppend(vdm_BC_GenHandler(expdecl, println));

// 20110523 -->
  CurrentRType(TYPE_REP_UnitTypeRep());
// <-- 20110523

  TYPE_CPP_FunctionDefinition runMthd (vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
                                         SEQ<TYPE_CPP_Modifier>().ImpAppend(vdm_BC_GenModifier(quote_PUBLIC)),
                                         type_dL().ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenVoid())),
                                         vdm_BC_GenFctDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"run")),type_dL()),
                                         Nil(),
                                         vdm_BC_GenTryBlock(GenStmt(stmt, true), handlers, Nil())));
/*
  public void start () throws CGException {
    procThread = new VDMThread(this);
    procThread.start();
  }
*/

  TYPE_CPP_Expr newExpr (vdm_BC_GenNewExpr(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"VDMThread"))),
                                           SEQ<TYPE_CPP_Expr>().ImpAppend(GenThis())));

  InsertImport(SEQ<Char>(L"jp.vdmtools.VDM.VDMThread"));

  TYPE_CPP_Stmt asgnStmt (vdm_BC_GenAsgnStmt(vdm_BC_GenIdentifier(ASTAUX::MkId(L"procThread")), newExpr));

  TYPE_CPP_Stmt startStmt (vdm_BC_GenFctCallObjMemAccStmt(vdm_BC_GenIdentifier(ASTAUX::MkId(L"procThread")),
                                                          ASTAUX::MkId(L"start"),
                                                          SEQ<TYPE_CPP_Expr>()));

  TYPE_CPP_FunctionDefinition startMthd (vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
                                           SEQ<TYPE_CPP_Modifier>().ImpAppend(vdm_BC_GenModifier(quote_PUBLIC)),
                                           type_dL().ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenVoid())),
                                           vdm_BC_GenFctDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"start")), type_dL()),
                                           GenExceptionsHdr(),
                                           vdm_BC_GenBlock( type_dL().ImpAppend(asgnStmt).ImpAppend(startStmt))));

  TYPE_CPP_Stmt stopStmt (vdm_BC_GenFctCallObjMemAccStmt(vdm_BC_GenIdentifier(ASTAUX::MkId(L"procThread")),
                                                          ASTAUX::MkId(L"stop"),
                                                          SEQ<TYPE_CPP_Expr>()));
  TYPE_CPP_FunctionDefinition stopMthd (vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
                                           SEQ<TYPE_CPP_Modifier>().ImpAppend(vdm_BC_GenModifier(quote_PUBLIC)),
                                           type_dL().ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenVoid())),
                                           vdm_BC_GenFctDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"stop")), type_dL()),
                                           GenExceptionsHdr(),
                                           vdm_BC_GenBlock( type_dL().ImpAppend(stopStmt))));

  return SEQ<TYPE_CPP_FunctionDefinition>().ImpAppend(runMthd)
                                           .ImpAppend(startMthd)
                                           .ImpAppend(stopMthd);
}

// GenConstrExtensions
// cname : AS`Name
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenConstrExtensions(const TYPE_AS_Name & cname)
{
  SEQ<TYPE_CPP_Stmt> stmts;
  stmts.ImpConc(GenPeriodicThreads());

  if (this->classInfos[cname].get_topClass().GetValue())
    stmts.ImpAppend(vdm_BC_GenExpressionStmt(vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"setSentinel")),
                                                               SEQ<TYPE_CPP_Expr>())));

  return stmts;
}

// GenPeriodicThreads
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenPeriodicThreads()
{
  TYPE_AS_Class cls (this->classDefs[this->currentClass]);
  const TYPE_AS_Definitions & defs (cls.GetRecord(pos_AS_Class_defs));
  const Generic & thread (defs.GetField(pos_AS_Definitions_threaddef));

  SEQ<TYPE_CPP_Stmt> stmts;
  if (thread.Is(TAG_TYPE_AS_PerObl))
  {
    TYPE_AS_PerObl perObl (thread);
    const TYPE_AS_Expr & period (perObl.GetRecord (pos_AS_PerObl_period));
    const TYPE_AS_Expr & jit    (perObl.GetRecord (pos_AS_PerObl_jitter));
    const TYPE_AS_Expr & delay  (perObl.GetRecord (pos_AS_PerObl_delay));
    const TYPE_AS_Expr & offset (perObl.GetRecord (pos_AS_PerObl_offset));
  
    const TYPE_AS_Name & mtd    (perObl.GetRecord (pos_AS_PerObl_mtd));

    TYPE_CPP_Expr periodExpr (CGLiteral(period, mk_CG_VT(vdm_BC_GenIdentifier(ASTAUX::MkId(L"dummy")),
                                                      mk_REP_NumericTypeRep(Int(INTEGER)))));

    TYPE_CPP_FunctionDefinition memDecl (
      vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
        SEQ<TYPE_CPP_Modifier>().ImpAppend(vdm_BC_GenModifier(quote_PUBLIC)),
        type_dL().ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenVoid())),
        vdm_BC_GenFctDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"threadDef")), type_dL()),
        GenExceptionsHdr(),
        vdm_BC_GenExpressionStmt(vdm_BC_GenFctCall(vdm_BC_Rename(mtd), SEQ<TYPE_CPP_Expr>()))));

    SEQ<TYPE_CPP_Expr> anonArgs;
    anonArgs.ImpAppend(periodExpr);
    anonArgs.ImpAppend(vdm_BC_GenIdentifier(ASTAUX::MkId(L"perThread")));

    TYPE_CPP_Expr newExpr (
      vdm_BC_GenAnonymousClassExpr(
        vdm_BC_GenIdentifier(ASTAUX::MkId(L"PeriodicThread")),
        anonArgs,
//        SEQ<TYPE_DPP_MemberDeclaration>().ImpAppend(memDecl)));
        type_dL().ImpAppend(memDecl)));

    stmts.ImpAppend(vdm_BC_GenAsgnStmt(vdm_BC_GenIdentifier(ASTAUX::MkId(L"perThread")), newExpr));
  }
  return stmts;
}

// GenPeriodicStartMethod
// AS`PerObl
// ==> seq1 of CPP`FunctionDefinition
SEQ<TYPE_CPP_FunctionDefinition> vdmcg::GenPeriodicStartMethod()
{
  TYPE_CPP_Stmt invokeCall (vdm_BC_GenFctCallObjMemAccStmt(vdm_BC_GenIdentifier(ASTAUX::MkId(L"perThread")),
                                                           ASTAUX::MkId(L"invoke"),
                                                           SEQ<TYPE_CPP_Expr>()));

  TYPE_CPP_FunctionDefinition startMthd ( 
    vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
      SEQ<TYPE_CPP_Modifier>().ImpAppend(vdm_BC_GenModifier(quote_PUBLIC)),
      type_dL().ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenVoid())),
      vdm_BC_GenFctDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"start")), type_dL()),
      GenExceptionsHdr(),
      vdm_BC_GenBlock(type_dL().ImpAppend(invokeCall))));

  return SEQ<TYPE_CPP_FunctionDefinition>().ImpAppend(startMthd);
}

// GenStartStmt
// sstmt : AS`StartStmt
// ==> CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenStartStmt(const TYPE_AS_StartStmt & sstmt, bool isLast)
{
  const TYPE_AS_Expr & expr (sstmt.GetRecord(pos_AS_StartStmt_expr));
  Tuple cgee (CGExprExcl(expr, ASTAUX::MkId(L"tmpObj"), Nil()));
  const TYPE_CPP_Expr & res_v (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & res_stmt (cgee.GetSequence(2));
  TYPE_CPP_Expr fctCallExpr (vdm_BC_GenFctCallObjMemAcc(res_v, ASTAUX::MkId(L"start"), SEQ<TYPE_CPP_Expr>()));

  SEQ<TYPE_CPP_Stmt> rb_l;
  rb_l.ImpConc(res_stmt);
  rb_l.ImpAppend(vdm_BC_GenExpressionStmt(fctCallExpr));

//  if (rb_l.Length() == 1)
//    return rb_l[1];
//  else
//    return vdm_BC_GenBlock(rb_l);
  return rb_l;
}

// TODO:
// GenStartlistStmt
// sstmt : AS`StartStmt
// ==> CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenStartlistStmt(const TYPE_AS_StartListStmt & sstmt, bool isLast)
{
  const TYPE_AS_Expr & expr (sstmt.GetRecord(pos_AS_StartListStmt_expr));

  Generic type (FindType(expr));

  Tuple cgee (CGExprExcl(expr, ASTAUX::MkId(L"tmpSet"), type));
  const TYPE_CPP_Expr & res_v (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & res_stmt (cgee.GetSequence(2));

  TYPE_CPP_Identifier tmpElem (vdm_BC_GiveName(ASTAUX::MkId(L"tmpElem")));

  TYPE_CGMAIN_VT elemVT (mk_CG_VT(tmpElem, FindSetElemType(type)));
  TYPE_CGMAIN_VT setVT (mk_CG_VT(res_v, mk_REP_SetTypeRep(FindSetElemType(type))));

  TYPE_CPP_Expr fctCallExpr (vdm_BC_GenFctCallObjMemAcc(tmpElem, ASTAUX::MkId(L"start"), SEQ<TYPE_CPP_Expr>()));

  SEQ<TYPE_CPP_Stmt> body_l;
  body_l.ImpAppend(vdm_BC_GenExpressionStmt(fctCallExpr));

  SEQ<TYPE_CPP_Stmt> rb_l;
  rb_l.ImpConc(res_stmt);

  if (type.IsNil() || !IsSetType(type)) {
    rb_l.ImpConc(GenSetTypeCheck(res_v, nil , L""));
  }
  rb_l.ImpConc(GenIterSet(setVT, nil, elemVT, body_l));

//  return vdm_BC_GenBlock(rb_l);
  return rb_l;
}

// GenStopStmt
// sstmt : AS`StopStmt
// ==> CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenStopStmt(const TYPE_AS_StopStmt & sstmt, bool isLast)
{
  const TYPE_AS_Expr & expr (sstmt.GetRecord(pos_AS_StopStmt_expr));
  Tuple cgee (CGExprExcl(expr, ASTAUX::MkId(L"tmpObj"), Nil()));
  const TYPE_CPP_Expr & res_v (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & res_stmt (cgee.GetSequence(2));
  TYPE_CPP_Expr fctCallExpr (vdm_BC_GenFctCallObjMemAcc(res_v, ASTAUX::MkId(L"stop"), SEQ<TYPE_CPP_Expr>()));

  SEQ<TYPE_CPP_Stmt> rb_l;
  rb_l.ImpConc(res_stmt);
  rb_l.ImpAppend(vdm_BC_GenExpressionStmt(fctCallExpr));
  return rb_l;
}

// TODO:
// GenStoplistStmt
// sstmt : AS`StopStmt
// ==> CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenStoplistStmt(const TYPE_AS_StopListStmt & sstmt, bool isLast)
{
  const TYPE_AS_Expr & expr (sstmt.GetRecord(pos_AS_StopListStmt_expr));

  Generic type (FindType(expr));

  Tuple cgee (CGExprExcl(expr, ASTAUX::MkId(L"tmpSet"), type));
  const TYPE_CPP_Expr & res_v (cgee.GetRecord(1));
  const SEQ<TYPE_CPP_Stmt> & res_stmt (cgee.GetSequence(2));

  TYPE_CPP_Identifier tmpElem (vdm_BC_GiveName(ASTAUX::MkId(L"tmpElem")));

  TYPE_CGMAIN_VT elemVT (mk_CG_VT(tmpElem, FindSetElemType(type)));
  TYPE_CGMAIN_VT setVT (mk_CG_VT(res_v, mk_REP_SetTypeRep(FindSetElemType(type))));

  TYPE_CPP_Expr fctCallExpr (vdm_BC_GenFctCallObjMemAcc(tmpElem, ASTAUX::MkId(L"stop"), SEQ<TYPE_CPP_Expr>()));

  SEQ<TYPE_CPP_Stmt> body_l;
  body_l.ImpAppend(vdm_BC_GenExpressionStmt(fctCallExpr));

  SEQ<TYPE_CPP_Stmt> rb_l;
  rb_l.ImpConc(res_stmt);

  if (type.IsNil() || !IsSetType(type)) {
    rb_l.ImpConc(GenSetTypeCheck(res_v, nil , L""));
  }
  rb_l.ImpConc(GenIterSet(setVT, nil, elemVT, body_l));

//  return vdm_BC_GenBlock(rb_l);
  return rb_l;
}

// GenThreadId
// ==> CPP`Expr
TYPE_CPP_Expr vdmcg::GenThreadId()
{
  InsertImport(SEQ<Char>(L"jp.vdmtools.VDM.VDMThread"));
  return vdm_BC_GenFctCallObjMemAcc(vdm_BC_GenIdentifier(ASTAUX::MkId(L"VDMThread")),
                                    ASTAUX::MkId(L"getThreadId"),
                                    SEQ<TYPE_CPP_Expr>());
}
