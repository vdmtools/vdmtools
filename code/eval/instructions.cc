// {{{ Header

/***
*  * WHAT
*  *    Implementation instructions.vdm
*  * Id
*  *    $Id: instructions.cc,v 1.120 2006/09/22 11:14:04 vdmtools Exp $
*  * SPECIFICATION VERSION
*  *    instructions.cc: Implementation of instructions.vdm 1.58
***/

// }}}

// {{{ includes

#include "astaux.h"
#include "compile.h"
#include "INSTRTP.h"
#include "astaux.h"
#include "asquotes.h"
#include "pattern.h"
#include "stackeval.h"
#include "libman.h"
#include "tbutils.h"
#include "rt_errmsg.h"
#include "evalexpr.h"
#include "evaldl.h"
#include "freevars.h"
#include "vdmstdlib.h"
#include "tb_exceptions.h"
#include "evalaux.h"
#include "settings.h"
#include "intconvquotes.h"
#include "val2x.h"
#include "statsem.h"
#ifdef VDMPP
#include "MANGLE.h"
#endif // VDMPP
#include "evalstate.h"

//wcout << L"intertp: " << INT2Q::h2gAS(intertp) << endl;

static wstring lambdaFnName = L"Lambda Application";

#define PROFILING 1
#ifdef PROFILING
#include <time.h>
#ifndef _MSC_VER
#include <sys/time.h>
int timediff(timeval tv1, timeval tv2)
{
  return ((tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec - tv1.tv_usec));
}
//timeval tvpre, tvpost;
//gettimeofday(&tvpre, NULL);
#endif // _MSC_VER
#endif // PROFILING

#if defined( PROFILELOG )
#if defined( _MSC_VER )
#include <sys/timeb.h>
#include <time.h>

static wofstream profileLog("vdmprofile");

void OutputTime()
{
  clock_t now = clock();
  wchar_t buf[100];
  swprintf(buf, L"%lu", (long) now * 1000 / CLOCKS_PER_SEC);
  profileLog << buf << endl;
}

void StartProfile(const TYPE_AS_Name& name)
{
  profileLog << ASName2String(name) << L" ";
  OutputTime();
}

void FinishProfile(const TYPE_AS_Name& name)
{
  profileLog << L"Finished " << ASName2String(name) << L" ";;
  OutputTime();
}
#else
void StartProfile(const TYPE_AS_Name& name) {}
void FinishProfile(const TYPE_AS_Name& name) {}
#endif // _MSC_VER
#endif // PROFILELOG

// 
#define POP() this->cs_eval_stack_p->Pop()
#define POPN(n) this->cs_eval_stack_p->Pop(n)
#define PUSH(item) this->cs_eval_stack_p->Push(item)
#define HEAD() this->cs_eval_stack_p->Head()
#define SETHEAD(item) this->cs_eval_stack_p->ImpModify(1, item)
#define GETNTH(n) this->cs_eval_stack_p->GetNth(n)
#define SETNTH(n, item) this->cs_eval_stack_p->ImpModify(n, item)
#define INCRPC(n) this->cs_shared_p->pc += n
#define REMOVENTH(n) this->cs_eval_stack_p->RemoveNth(n)
#define SWAP() this->cs_eval_stack_p->Swap()
#define COPY() this->cs_eval_stack_p->Copy();

#define PUSHEMPTYENV() this->cs_env_l_p->ImpPrepend(SEQ<TYPE_SEM_BlkEnv>())
#define PUSHENVL(env_l) this->cs_env_l_p->ImpPrepend(env_l)
#define PUSHBLKENV(env) this->cs_env_l_p->GetIndexRef(1).ImpPrepend(env)
#define TOPBLKENV() this->cs_env_l_p->Hd().Hd()
#define POPBLKENV() this->cs_env_l_p->GetIndexRef(1).ImpTl()
#define POPENVL() this->cs_env_l_p->ImpTl()

#ifdef VDMSL
#define PUSHMODULE(clmodName) this->cs_cur_mod_p->ImpPrepend(clmodName)
#define POPMODULE() if (!this->cs_cur_mod_p->IsEmpty()) this->cs_cur_mod_p->ImpTl()
#endif // VDMSL

// }}}
// {{{ CBR, CNBR, BR, ERRINST

// ExeCBR
// n : int
// ==> ()
void StackEval::ExeCBR(const Int & n)
{
  TYPE_SEM_VAL b (POP());
  if(b.Is(TAG_TYPE_SEM_BOOL)) {
    if(b.GetBoolValue(pos_SEM_BOOL_v)) {
      INCRPC(n);
    }
  }
  else {
    RTERR::Error(L"ExeCBR",RTERR_BOOL_EXPECTED, Nil(), Nil(), Sequence());
  }
}

// ExeCNBR
// n : int
// ==> ()
void StackEval::ExeCNBR(const Int & n)
{
  TYPE_SEM_VAL b (POP());
  if(b.Is(TAG_TYPE_SEM_BOOL)) {
    if(!b.GetBoolValue(pos_SEM_BOOL_v)) {
      INCRPC(n);
    }
  }
  else {
    RTERR::Error(L"ExeCNBR", RTERR_BOOL_EXPECTED, Nil(), Nil(), Sequence());
  }
}

// ExeBR
// n : int
// ==> ()
void StackEval::ExeBR(const Int & n)
{
  INCRPC(n);
}

// }}}
// {{{ ERRINST

// ExeERRINST
// err : RTERR`ERR
// ==> ()
void StackEval::ExeERRINST(const TYPE_RTERR_ERR & err)
{
  RTERR::Error(L"ExeERRINST", err, Nil(), Nil(), Sequence());
}

// }}}
// {{{ PRE, POST, POSTENV

// ExePRE
// ==> ()
void StackEval::ExePRE()
{
  PUSH(Settings.PreCheck() ? sem_true : sem_false);
}

// ExePOST
// ==> ()
void StackEval::ExePOST()
{
  PUSH(Settings.PostCheck() ? sem_true : sem_false);
}

// ExeASSERT
// ==> ()
void StackEval::ExeASSERT()
{
  PUSH(Settings.Assertion() ? sem_true : sem_false);
}

// ExeCASES
// ==> ()
void StackEval::ExeCASES()
{
  PUSH(Settings.CasesCheck() ? sem_true : sem_false);
}

// ExeMEASURE
// ==> ()
void StackEval::ExeMEASURE()
{
  PUSH(Settings.Measure() ? sem_true : sem_false);
}


// ==> ()
void StackEval::ExeMEASURETPINST(const SEQ<TYPE_AS_TypeVar> & tpparms)
{
  MAP<TYPE_AS_TypeVar, TYPE_AS_Type> typeInst (HdTypeInst());
  if (tpparms.Elems().Diff(typeInst.Dom()).IsEmpty()) {
    TYPE_SEM_VAL val (HEAD());
    if (val.Is(TAG_TYPE_SEM_ExplPOLY)) {
      SEQ<TYPE_AS_TypeVar> tp_l;
      size_t len_tpparms = tpparms.Length();
      for (size_t idx = 1; idx <= len_tpparms; idx++) {
        tp_l.ImpAppend(typeInst[tpparms[idx]]);
      }
      SETHEAD(EXPR::ConvertPolyToFn (val, tp_l));
    }
  }
}

// ExeMEASURECHECK
// ==> ()
void StackEval::ExeMEASURECHECK()
{
  TYPE_SEM_VAL curr_mv (POP());
  switch (curr_mv.GetTag()) {
    case TAG_TYPE_SEM_NUM: {
      if (curr_mv.GetReal(pos_SEM_NUM_v).IsNat() ) {
        theStackMachine().MeasureCheck(curr_mv);
      }
      else {
        RTERR::Error(L"ExeMEASURECHECK", RTERR_NAT_OR_TUPLE_OF_NAT_EXPECTED, Nil(), Nil(), Sequence());
      }
      break;
    }
    case TAG_TYPE_SEM_TUPLE: {
      const SEQ<TYPE_SEM_VAL> & v_l (curr_mv.GetSequence(pos_SEM_TUPLE_v));
      size_t len_v_l = v_l.Length ();
      bool forall = true;
      for (size_t i = 1; (i <= len_v_l) && forall; i++) {
        const TYPE_SEM_VAL & v (v_l[i]);
        forall = (v.Is(TAG_TYPE_SEM_NUM) ? v.GetReal(pos_SEM_NUM_v).IsNat() : false);
      }
      if (forall) {
        theStackMachine().MeasureCheck(curr_mv);
      }
      else {
        RTERR::Error(L"ExeMEASURECHECK", RTERR_NAT_OR_TUPLE_OF_NAT_EXPECTED, Nil(), Nil(), Sequence());
      }
      break;
    }
    default: {
      RTERR::Error(L"ExeMEASURECHECK", RTERR_NAT_OR_TUPLE_OF_NAT_EXPECTED, Nil(), Nil(), Sequence());
      break;
    }
  }
}

// ExeDTCMEASURE
// ==> ()
void StackEval::ExeDTCMEASURE()
{
  TYPE_SEM_VAL curr_mv (HEAD());
  switch (curr_mv.GetTag()) {
    case TAG_TYPE_SEM_NUM: {
      if (!curr_mv.GetReal(pos_SEM_NUM_v).IsNat() ) {
        RTERR::Error(L"ExeMEASURECHECK", RTERR_NAT_OR_TUPLE_OF_NAT_EXPECTED, Nil(), Nil(), Sequence());
      }
      break;
    }
    case TAG_TYPE_SEM_TUPLE: {
      const SEQ<TYPE_SEM_VAL> & v_l (curr_mv.GetSequence(pos_SEM_TUPLE_v));
      size_t len_v_l = v_l.Length ();
      bool exists = v_l.IsEmpty();
      for (size_t i = 1; (i <= len_v_l) && !exists; i++) {
        const TYPE_SEM_VAL & v (v_l[i]);
        exists = !(v.Is(TAG_TYPE_SEM_NUM) ? v.GetReal(pos_SEM_NUM_v).IsNat() : false);
      }
      if (exists) {
        RTERR::Error(L"ExeMEASURECHECK", RTERR_NAT_OR_TUPLE_OF_NAT_EXPECTED, Nil(), Nil(), Sequence());
      }
      break;
    }
    default: {
      RTERR::Error(L"ExeMEASURECHECK", RTERR_NAT_OR_TUPLE_OF_NAT_EXPECTED, Nil(), Nil(), Sequence());
      break;
    }
  }
}

// ExePOSTENV
// resnmtps : seq of AS`NameType
// ci : CI`ContextId
// ==> ()
void StackEval::ExePOSTENV(const SEQ<TYPE_AS_NameType> & resnmtps, const TYPE_CI_ContextId & ci)
{
  TYPE_SEM_VAL resval (HEAD());

  switch(resnmtps.Length()) {
    case 0: {
      PushBlkEnv(AUX::MkBlkEnv(ASTAUX::MkNameFromId(ASTAUX::MkId(L"RESULT"), ci), resval, Nil(), sem_read_only));
      break;
    }
    case 1: {
      const TYPE_AS_NameType & rnt (resnmtps[1]);
      PushBlkEnv(AUX::MkBlkEnv(rnt.GetRecord(pos_AS_NameType_nm),
                               resval,
                               rnt.GetRecord(pos_AS_NameType_tp),
                               sem_read_only));
      break;
    }
    default: { // resnmtps.Length() > 1
      if (resval.Is(TAG_TYPE_SEM_TUPLE) && (resnmtps.Length() == resval.GetSequence(pos_SEM_TUPLE_v).Length())) {
        const SEQ<TYPE_SEM_VAL> & resval_seq (resval.GetSequence(pos_SEM_TUPLE_v));
        TYPE_SEM_BlkEnv blkenv (AUX::MkEmptyBlkEnv(sem_read_only));
        Map & id_um ((Map &)(blkenv.GetFieldRef(pos_SEM_BlkEnv_id_um)));
        size_t len_resnmtps = resnmtps.Length();
        for (size_t idx = 1; idx <= len_resnmtps; idx++) {
          const TYPE_AS_NameType & resnmtp (resnmtps[idx]);
          id_um.ImpModify(resnmtp.GetRecord(pos_AS_NameType_nm),
                          TYPE_SEM_ValTp().Init(resval_seq[idx], resnmtp.GetRecord(pos_AS_NameType_tp)));
        }
        PUSHBLKENV(blkenv);
      }
      else {
        RTERR::Error(L"ExePOSTENV", RTERR_WRONG_NO_RES, Nil(), Nil(), Sequence());
      }
      break;
    }
  }
}

// ExeNOBODY
// err : seq of char
// modname : AS`Name
// name : AS`Name
// parms : AS`Parameters
// ==> ()
void StackEval::ExeNOBODY(const TYPE_RTERR_ERR & err,
                          const TYPE_AS_Name & modname,
                          const TYPE_AS_Name & name,
                          const SEQ<TYPE_AS_Pattern> & parms)
{
  SEQ<TYPE_SEM_VAL> arg_lv;
  size_t len_parms = parms.Length();
  bool forall = true;
  for (size_t index = 1; (index <= len_parms) && forall; index++) {
    const TYPE_AS_Pattern & pat (parms[index]);
    if (pat.Is(TAG_TYPE_AS_PatternName)) {
      const Generic & pnm (pat.GetField(pos_AS_PatternName_nm));
      if (pnm.IsNil()) {
        arg_lv.ImpAppend(sem_nil);
      }
      else {
        TYPE_SEM_ValTp valTp (theState().LookUp(pnm));
        arg_lv.ImpAppend(valTp.GetRecord(pos_SEM_ValTp_val));
      }
    }
    else {
      forall = false;
    }
  }

  if( forall ) {
    Tuple ivs (this->stdlib.IsVdmStdLib(modname, name, arg_lv));
    if(ivs.GetBoolValue(1)) {
      PUSH(ivs.GetRecord(2));
    }
    else {
      RTERR::Error(L"ExeNOBODY", err, Nil(), Nil(), Sequence());
    }
  }
  else {
    RTERR::Error(L"ExeNOBODY", err, Nil(), Nil(), Sequence());
  }
}

// TypeToSet
// tp : AS`Type
// ==> set of SEM`VAL
SET<TYPE_SEM_VAL> StackEval::TypeToSet(const TYPE_AS_Type & tp) 
{
  switch (tp.GetTag()) {
    case TAG_TYPE_AS_BracketedType: { 
      return TypeToSet(tp.GetRecord(pos_AS_BracketedType_tp));
    }
    case TAG_TYPE_AS_BooleanType: {
      return mk_set(sem_true, sem_false);
    }
    case TAG_TYPE_AS_QuoteType: {
      const TYPE_AS_QuoteLit & lit (tp.GetRecord(pos_AS_QuoteType_lit));
      return mk_set(TYPE_SEM_QUOTE().Init(lit.GetSequence(pos_AS_QuoteLit_val)));
    }
    case TAG_TYPE_AS_UnionType: {
      const SEQ<TYPE_AS_Type> & tps (tp.GetSequence(pos_AS_UnionType_tps));
      SET<TYPE_SEM_VAL> res;
      size_t len_tps = tps.Length();
      for (size_t idx = 1; idx <= len_tps; idx++) {
        res.ImpUnion(TypeToSet(tps[idx]));
      }
      return res;
    }
    case TAG_TYPE_AS_ProductType: {
      const SEQ<TYPE_AS_Type> & tps (tp.GetSequence(pos_AS_ProductType_tps));
      SEQ<Set> seqOfSet;
      size_t len_tps = tps.Length();
      for (size_t idx = 1; idx <= len_tps; idx++) {
        seqOfSet.ImpAppend(TypeToSet(tps[idx]));
      }
      SET<TYPE_SEM_VAL> res;
      SET<Sequence> varSet (AUX::SeqOfSetOf2SetOfSeqOf(seqOfSet));
      Generic e;
      for (bool bb = varSet.First(e); bb; bb = varSet.Next(e)) {
        res.Insert(TYPE_SEM_TUPLE().Init(e));
      }
      return res; 
    }
    case TAG_TYPE_AS_CompositeType: {
      const TYPE_AS_Name & tag (tp.GetRecord(pos_AS_CompositeType_name));
      const SEQ<TYPE_AS_Field> & fields (tp.GetSequence(pos_AS_CompositeType_fields));
      SEQ<Set> seqOfSet;
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++) {
        seqOfSet.ImpAppend(TypeToSet(fields[idx].GetRecord(pos_AS_Field_type)));
      }
      SET<TYPE_SEM_VAL> res;
      SET<Sequence> varSet (AUX::SeqOfSetOf2SetOfSeqOf(seqOfSet));
      Generic e;
      for (bool bb = varSet.First(e); bb; bb = varSet.Next(e)) {
        res.Insert(EXPR::EvalRecordConstructorExpr(tag, e));
      }
      return res; 
    }
    case TAG_TYPE_AS_TypeName: {
#ifdef VDMSL
      TYPE_AS_Name name (tp.GetRecord(pos_AS_TypeName_name));
      Tuple t (AUX::LookUpRename(name));
      if (t.GetBoolValue(1)) {
        name = t.GetRecord(2);
      }

//      Tuple itd (AUX::IsTypeDef(name)); //bool * [AS`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * 
      Tuple itd (theState().GetCachedTypeDef(name)); //bool * [AS`Type] * [AS`Invariant]
      if (!itd.GetBoolValue(1)) {
        RTERR::Error (L"TypeToSet", RTERR_TYPE_UNKNOWN, Nil(), tp, Sequence());
      }
#endif // VDMSL
//      return TypeToSet(itd.GetRecord(2));
#ifdef VDMPP
      const TYPE_AS_Name & name (tp.GetRecord(pos_AS_TypeName_name));

//      Tuple itd (AUX::IsTypeDef(name)); // bool * [GLOBAL`Type] * [AS`Equal] * [AS`Order] * [AS`Invariant] * [AS`Name] * [AS`Access];
      Tuple itd (theState().GetCachedTypeDef(name)); // bool * [GLOBAL`Type] * [AS`Invariant] * [AS`Name] * [AS`Access];
      if (!itd.GetBoolValue(1)) {
        const Generic & access (itd.GetField(7)); // [AS`Access]
        if (access.IsNil()) {
          RTERR::Error(L"TypeToSet", RTERR_TYPE_UNKNOWN, Nil(), tp, Sequence());
        }
        else {
          RTERR::Error(L"TypeToSet", RTERR_TYPE_NOT_IN_SCOPE, Nil(), tp, Sequence());
        }
      }
#endif // VDMPP
      if (itd.GetField(3).IsNil()) {
        return TypeToSet(itd.GetRecord(2));
      }
      else {
        const TYPE_AS_Invariant & invariant (itd.GetField(pos_AS_TypeDef_Inv));
        const TYPE_AS_Pattern & pat (invariant.GetRecord(pos_AS_Invariant_pat));
        if (pat.Is(TAG_TYPE_AS_PatternName) && !pat.GetField(pos_AS_PatternName_nm).IsNil()) {
          const TYPE_GLOBAL_Type & tp (itd.GetRecord(2));
          if (tp.Is(TAG_TYPE_AS_NumericType)) {
            const Int & qtp (tp.GetInt(pos_AS_NumericType_qtp));
            switch (qtp.GetValue()) {
              case NATONE:
              case NAT:
              case INTEGER: {
                const TYPE_AS_Name & nm (pat.GetRecord(pos_AS_PatternName_nm));
                const TYPE_AS_Expr & expr (invariant.GetRecord(pos_AS_Invariant_expr));
                Generic min = Nil();
                Generic max = Nil();
                switch (qtp.GetValue()) {
                  case NATONE: {
                    min = Int(1);
                    break;
                  }
                  case NAT: {
                    min = Int(0);
                    break;
                  }
                }
                Tuple t (ExprToSet(nm, expr, min, max));
                if (t.GetBoolValue(1) && !t.GetField(2).IsNil() && !t.GetField(3).IsNil()) {
                  int from = t.GetIntValue(2);
                  int to = t.GetIntValue(3);
                  SET<TYPE_SEM_VAL> res;
                  for (int i = from; i <= to; i++) {
                    res.Insert(mk_SEM_NUM(Real(i)));
                  }
                  return res;
                }
                break;
              }
            }
          }
        }
        RTERR::Error(L"TypeToSet", RTERR_TYPE_BIND_EVAL, Nil(), Nil(), Sequence());
        return Set(); // dummy
      }
    }
    case TAG_TYPE_AS_OptionalType: {
      SET<TYPE_SEM_VAL> res;
      res.Insert(sem_nil);
      res.ImpUnion(TypeToSet(tp.GetRecord(pos_AS_OptionalType_tp)));
      return res;
    }
    case TAG_TYPE_AS_CharType: {
      break;
    }
    case TAG_TYPE_AS_NumericType:
    case TAG_TYPE_AS_Set0Type:
    case TAG_TYPE_AS_Set1Type:
    case TAG_TYPE_AS_Seq0Type:
    case TAG_TYPE_AS_Seq1Type:
    case TAG_TYPE_AS_GeneralMap0Type:
    case TAG_TYPE_AS_GeneralMap1Type:
    case TAG_TYPE_AS_InjectiveMap0Type:
    case TAG_TYPE_AS_InjectiveMap1Type:
    case TAG_TYPE_AS_PartialFnType:
    case TAG_TYPE_AS_TotalFnType:
    case TAG_TYPE_AS_OpType:
    case TAG_TYPE_AS_TypeVar:
    case TAG_TYPE_AS_VoidType:
    case TAG_TYPE_AS_TokenType:
    case TAG_TYPE_AS_AllType: {
      break;
    }
    default: {
      break;
    }
  }
  RTERR::Error(L"TypeToSet", RTERR_TYPE_BIND_EVAL, Nil(), Nil(), Sequence());
  return Set(); // dummy
}

// IsIntNum
// expr : AS`Expr
// ==> bool
bool StackEval::IsIntNum(const TYPE_AS_Expr & expr) {
  TYPE_AS_Expr e (expr);
  while (e.Is(TAG_TYPE_AS_BracketedExpr)) {
    e = e.GetRecord(pos_AS_BracketedExpr_expr);
  }
  switch (e.GetTag()) {
    case TAG_TYPE_AS_NumLit: {
      return true;
    }
    case TAG_TYPE_AS_PrefixExpr: {
      const Int & opr (e.GetInt(pos_AS_PrefixExpr_opr)); 
      const TYPE_AS_Expr & arg (e.GetRecord(pos_AS_PrefixExpr_arg));
      switch (opr.GetValue()) {
        case NUMPLUS :
        case NUMMINUS : {
          return IsIntNum(arg);
        }
        default: {
          return false;
        }
      }
    }
    default: {
      return false;
    }
  }
}

// GetIntNum
// expr : AS`Expr
// ==> [int]
Generic StackEval::GetIntNum(const TYPE_AS_Expr & expr) {
  TYPE_AS_Expr e (expr);
  while (e.Is(TAG_TYPE_AS_BracketedExpr)) {
    e = e.GetRecord(pos_AS_BracketedExpr_expr);
  }
  switch (e.GetTag()) {
    case TAG_TYPE_AS_NumLit: {
      return Int(e.GetReal(pos_AS_NumLit_val).GetIntValue());
    }
    case TAG_TYPE_AS_PrefixExpr: {
      const Int & opr (e.GetInt(pos_AS_PrefixExpr_opr)); 
      const TYPE_AS_Expr & arg (e.GetRecord(pos_AS_PrefixExpr_arg));
      switch (opr.GetValue()) {
        case NUMPLUS : {
          return GetIntNum(arg);
        }
        case NUMMINUS : {
          Generic g (GetIntNum(arg));
          if (g.IsInt()) {
            return Int(-Int(g).GetValue());
          }
          return g;
        }
        default: {
          return Nil();
        }
      }
    }
    default: {
      return Nil();
    }
  }
}

// ExprToSet
// name : AS`Name
// expr : AS`Expr
// min : [int]
// max : [int]
// ==> bool * [int] * [int]
Tuple StackEval::ExprToSet(const TYPE_AS_Name & name, const TYPE_AS_Expr & expr,
                           const Generic & min, const Generic & max) {
//wcout << name << L" " << expr << L" " << min << L" " << max << endl;
  switch (expr.GetTag()) {
    case TAG_TYPE_AS_BracketedExpr: {
      return ExprToSet(name, expr.GetRecord(pos_AS_BracketedExpr_expr), min, max);
    }
    case TAG_TYPE_AS_BinaryExpr: {
      TYPE_AS_Expr left (expr.GetRecord(pos_AS_BinaryExpr_left));
      TYPE_AS_Expr right (expr.GetRecord(pos_AS_BinaryExpr_right));
      const Int & opr (expr.GetInt(pos_AS_BinaryExpr_opr));
      while (left.Is(TAG_TYPE_AS_BracketedExpr)) {
        left = left.GetRecord(pos_AS_BracketedExpr_expr);
      }
      while (right.Is(TAG_TYPE_AS_BracketedExpr)) {
        right = right.GetRecord(pos_AS_BracketedExpr_expr);
      }
      if (opr.GetValue() == AND) {
        Tuple t1 (ExprToSet(name, left, min, max));
        if (t1.GetBoolValue(1)) {
          return ExprToSet(name, right, t1.GetField(2), t1.GetField(3));
        }
        else {
          return t1;
        }
      }
      if (((left == name) && IsIntNum(right)) || (IsIntNum(left) && (right == name))) {
        switch (opr.GetValue()) {
          case NUMLT: {
            if (IsIntNum(right)) {
              int n = Int(GetIntNum(right)).GetValue() - 1;
              if (max.IsNil()) {
                return mk_(Bool(true), min, Int(n)); 
              }
              else if (n < Int(max).GetValue()) {
                return mk_(Bool(true), min, Int(n)); 
              }
              else {
                return mk_(Bool(true), min, max); 
              }
            }
            else {
              int n = Int(GetIntNum(left)).GetValue() + 1;
              if (min.IsNil()) {
                return mk_(Bool(true), Int(n), max); 
              }
              else if (n > Int(min).GetValue()) {
                return mk_(Bool(true), Int(n), max); 
              }
              else {
                return mk_(Bool(true), min, max); 
              }
            }
          }
          case NUMLE: {
            if (IsIntNum(right)) {
              int n = Int(GetIntNum(right)).GetValue();
              if (max.IsNil()) {
                return mk_(Bool(true), min, Int(n)); 
              }
              else if (n < Int(max).GetValue()) {
                return mk_(Bool(true), min, Int(n)); 
              }
              else {
                return mk_(Bool(true), min, max); 
              }
            }
            else {
              int n = Int(GetIntNum(left)).GetValue();
              if (min.IsNil()) {
                return mk_(Bool(true), Int(n), max); 
              }
              else if (n > Int(min).GetValue()) {
                return mk_(Bool(true), Int(n), max); 
              }
              else {
                return mk_(Bool(true), min, max); 
              }
            }
          }
          case NUMGT: {
            if (IsIntNum(right)) {
              int n = Int(GetIntNum(right)).GetValue() + 1;
              if (min.IsNil()) {
                return mk_(Bool(true), Int(n), max); 
              }
              else if (n > Int(min).GetValue()) {
                return mk_(Bool(true), Int(n), max); 
              }
              else {
                return mk_(Bool(true), min, max); 
              }
            }
            else {
              int n = Int(GetIntNum(left)).GetValue() - 1;
              if (max.IsNil()) {
                return mk_(Bool(true), min, Int(n)); 
              }
              else if (n > Int(max).GetValue()) {
                return mk_(Bool(true), min, Int(n)); 
              }
              else {
                return mk_(Bool(true), min, max); 
              }
            }
          }
          case NUMGE: {
            if (IsIntNum(right)) {
              int n = Int(GetIntNum(right)).GetValue();
              if (min.IsNil()) {
                return mk_(Bool(true), Int(n), max); 
              }
              else if (n > Int(min).GetValue()) {
                return mk_(Bool(true), Int(n), max); 
              }
              else {
                return mk_(Bool(true), min, max); 
              }
            }
            else {
              int n = Int(GetIntNum(left)).GetValue();
              if (max.IsNil()) {
                return mk_(Bool(true), min, Int(n)); 
              }
              else if (n > Int(max).GetValue()) {
                return mk_(Bool(true), min, Int(n)); 
              }
              else {
                return mk_(Bool(true), min, max); 
              }
            }
          }
        }
      }
      return mk_(Bool(false), Nil(), Nil());
    }
    default: {
      return mk_(Bool(false), Nil(), Nil());
    }
  }  
}

// ExeTPTOSET
// tp : AS`Type
void StackEval::ExeTPTOSET(const TYPE_AS_Type & tp)
{
  PUSH(TYPE_SEM_SET().Init(TypeToSet(tp)));
}

// ExeDLCALL
// clname : AS`Name
// name : AS`Name
// ==> ()
void StackEval::ExeDLCALL(const TYPE_AS_Name & clname, const TYPE_AS_Name & name)
{
#ifdef VDMPP
  SEQ<TYPE_SEM_VAL> args (POP());

  const TYPE_SEM_OBJ_uRef & curobj (GetCurObjRef());
  Tuple res (theState().DLCallOp(clname, name, args, curobj));
    
  if (res.GetBoolValue(1)) {
    PUSH(res.GetRecord(2));
  }
  else {
    RTERR::Error(L"ExeDLCALL", RTERR_DLCLASS_CALL_FAILED, Nil(), Nil(), Sequence());
  }
#endif //VDMPP
}

// }}}
// {{{ ExePUSHLIST

// ExePUSHLIST
// ==> ()
void StackEval::ExePUSHLIST(const Int & length)
{
  SEQ<TYPE_SEM_VAL> arg_l;
  if (length > 0) {
    arg_l.ImpConc(POPN(length));
  }
  PUSH(arg_l);
}

// }}}
// {{{ Context Stack Instructions

// ExeCONTEXT
// cid : CI`ContextId
// isStmt : bool
// ==> ()
void StackEval::ExeCONTEXT(const TYPE_CI_ContextId & cid, const Bool & isStmt)
{
  SetCid(cid);
  if (cid != NilContextId) {
    GetCI().IncTestCoverageInfo(cid);

#ifdef VDMSL
    if(GetUserBREAK()) {
#endif // VDMSL
#ifdef VDMPP
    if(!theScheduler().CheckingGuard() && GetUserBREAK()) {
#endif // VDMPP
      ResetUserBREAK();
      SetBREAK();
      return;
    }
    else if (ActiveBreakpoint(cid)) {
      SetBREAK();
      return;
    }
    else {
      TYPE_STKM_DebugFlag df (GetDebugFlag());
      switch (df.GetTag()) {
        case TAG_TYPE_STKM_Continue: {
          return;
        }
        case TAG_TYPE_STKM_Step: {
          if (isStmt && (CallStackLevel() <= df.GetIntValue(pos_STKM_Step_level)))
            SetBREAK();
          break;
        }
        case TAG_TYPE_STKM_StepIn: {
          SetBREAK();
          break;
        }
        case TAG_TYPE_STKM_SingleStep: {
          if (CallStackLevel() <= df.GetIntValue(pos_STKM_SingleStep_level))
            SetBREAK();
          break;
        }
        case TAG_TYPE_STKM_Finish: {
          if (CallStackLevel() <= (df.GetIntValue(pos_STKM_Finish_level) -1))
            SetBREAK();
          break;
        }
      }
    }
  }
}

// ExeISTART
// i :
// ==> ()
void StackEval::ExeISTART(const TYPE_AS_Id & txt, const TYPE_CI_ContextId & cid)
{
  PushCF(mk_(txt, cid));
}

// ExeIEND
// ==> ()
void StackEval::ExeIEND()
{
  PopCF();
}

// }}}
// {{{ Environment Instructions

// ExeSELBLKENV
// n : nat
// ==> ()
void StackEval::ExeSELBLKENV(const Int & n)
{
/*
  SEQ<Set> val_l (POPN(n));

  SET<TYPE_SEM_BlkEnv> val_l_hd (val_l.Hd());
  TYPE_SEM_BlkEnv env (val_l_hd.GetElem());
  val_l_hd.RemElem(env);

  PUSHBLKENV(env);
  PUSH(val_l_hd);

  for (int i = 2; i <= n; i++) {
    PUSH(val_l[i]);
  }
*/
  
  SET<TYPE_SEM_BlkEnv> val_l_hd (GETNTH(n));
  TYPE_SEM_BlkEnv env (val_l_hd.GetElem());
  val_l_hd.RemElem(env);

  PUSHBLKENV(env);

  SETNTH(n, val_l_hd);
}

// ExeAPPENDBLKENV
// id : AS`Name
// tp : [AS`Type]
// ==> ()
void StackEval::ExeAPPENDBLKENV(const TYPE_AS_Name & id, const Generic & tp)
{
  TYPE_SEM_VAL val (POP());

  // AS`Name * SEM`VAL * [AS`Type] ==> ()
  AppendToTopBlkEnv(id, val, tp);
}

// ExeREMSTACKELEM
// n : nat
// ==> ()
void StackEval::ExeREMSTACKELEM(const Int & n)
{
  REMOVENTH(n);
}

// ExeSWAP
// ==> ()
void StackEval::ExeSWAP()
{
  SWAP();
}

// ExeMULTBINDL
// n : nat
// part : PAT`PARTITION
// ==> ()
void StackEval::ExeMULTBINDL(const Int & n, const Int & part)
{
  SEQ<TYPE_STKM_Pattern> pat_lp (POPN(n));
  SEQ<TYPE_SEM_VAL> seq_lv (POPN(n));

  PUSH(PAT::EvalMultBindSeq(pat_lp, seq_lv, part)); // set of SEM`BlkEnv
}

// ExePOPBLKENV
// ==> ()
void StackEval::ExePOPBLKENV()
{
  POPBLKENV();
}

// ExeADDTOBLKENV
// ==> ()
void StackEval::ExeADDTOBLKENV()
{
  SET<TYPE_SEM_BlkEnv> found_s (HEAD());
  SETHEAD(found_s.Insert(TOPBLKENV()));
}

// }}}
// {{{ Unsorted Instructions

// ExeEXITVAL
// ==> ()
void StackEval::ExeEXITVAL()
{
  if (HdContext() == Int(PUREOP)) {
    RTERR::Error(L"ExeEXITVAL", RTERR_PURE_OP_EXIT, Nil(), Nil(), Sequence());
    return;
  }
  GotoTrapHandler(POP());
}

// ExeISCONT
// ==> ()
void StackEval::ExeISCONT()
{
  PUSH(HEAD().Is(TAG_TYPE_SEM_CONT) ? sem_true : sem_false);
}

// ExeMKEXIT
// isnil : bool * CI`ContextId
// ==> ()
void StackEval::ExeMKEXIT(const Bool & isnil, const TYPE_CI_ContextId & cid)
{
  PUSH(mk_SEM_EXIT(isnil ? (Generic)Nil() : POP()));
  SetLastExitCid(cid);
}

// ExeISEXIT
// ==> ()
void StackEval::ExeISEXIT()
{
  PUSH(HEAD().Is(TAG_TYPE_SEM_EXIT) ? sem_true : sem_false);
}

// ExeISNEEXIT
// ==> ()
void StackEval::ExeISNEEXIT()
{
  const TYPE_STKM_EvalStackItem & val (HEAD());
  if (val.Is(TAG_TYPE_SEM_EXIT))
    PUSH(Record(val).GetField(pos_SEM_EXIT_v).IsNil() ? sem_false : sem_true);
  else
    PUSH(sem_false);
}

// ExeREMEXITVAL
// ==> ()
void StackEval::ExeREMEXITVAL()
{
  const TYPE_SEM_VAL & ev (HEAD());
  PUSH(ev.GetRecord(pos_SEM_EXIT_v));
}

// ExePUSHTH
// movePC : int
// ==> ()
void StackEval::ExePUSHTH(const Int & movePC)
{
  PushTS(movePC);
}

// ExePOPTH
// ==> ()
void StackEval::ExePOPTH()
{
  PopTS();
}

// ExeLOOKUP
// name : AS`Name | AS`OldName
// ==> ()
void StackEval::ExeLOOKUP(const TYPE_AS_Expr & name)
{
  switch(name.GetTag()) {
    case TAG_TYPE_AS_Name: {
      TYPE_SEM_ValTp valTp(theState().LookUp(name));
      const TYPE_SEM_VAL & val (valTp.GetRecord(pos_SEM_ValTp_val));
      const Generic & tp (valTp.GetField(pos_SEM_ValTp_tp));
      
#ifdef VDMSL
      if (tp.Is(TAG_TYPE_AS_TypeName)) {
#endif // VDMSL
#ifdef VDMPP
      if (tp.Is(TAG_TYPE_AS_TypeName) && !theState().IsAClass(Record(tp).GetRecord(pos_AS_TypeName_name))) {
#endif // VDMPP
        //const TYPE_AS_Name & tag (Record(tp).GetRecord(pos_AS_TypeName_name));
        PUSH(val);
      }
      else {
        PUSH(val);
      }
      break;
    }
    case TAG_TYPE_AS_OldName: {
      PUSH(EvalOldName(name));
      break;
    }
    default:
      break;
  }
}

#ifdef VDMPP
// ExeLOOKUPSTATIC
// name : AS`Name
// ==> ()
void StackEval::ExeLOOKUPSTATIC(const TYPE_AS_Name & name)
{
  // bool * [SEM`VAL] * *[AS`Type] * [As`Name] * [AS`Access]
  Tuple lus (theState().LookUpStatic(name));
  if (lus.GetBoolValue(1) && !lus.GetRecord(2).IsNil()) {
    PUSH(lus.GetRecord(2));
  }
  else {
    RTERR::Error(L"ExeLOOKUPSTATIC", RTERR_STATIC_NOT_IN_SCOPE, Nil(), Nil(), Sequence());
  }
}

// ExeLOOKUPOP
// name : AS`Name
// ==> ()
void StackEval::ExeLOOKUPOP(const TYPE_AS_Name & name)
{
  const TYPE_AS_Name & clnm (GetCurCl());
  Map aop (theState().GetAllOps(clnm));
  if (aop.DomExists(name)) {
    PUSH(aop[name]);
  }
  else {
    Generic localover (theState().LookUpOverInClass(clnm, name));
    if (!localover.IsNil()) {
      PUSH(localover);
    }
    else {
      // TODO: regacy specification
      // the case of constructor is't locally defined
      Tuple lofp (theState().LookOpFctPoly(name));
      if (lofp.GetBoolValue(1) && lofp.GetBoolValue(2)) {
          PUSH(lofp.GetRecord(3));
      }
      else {
        RTERR::Error(L"ExeLOOKUPOP", RTERR_OP_NOT_IN_SCOPE, Nil(), Nil(), Sequence());
      }
    }
  }
}
#endif //VDMPP

// ExeAPPLY
// ==> ()
void StackEval::ExeAPPLY()
{
  SEQ<TYPE_STKM_EvalStackItem> item_l (POPN(2)); // [fct_v, arg_lv]

  //ApplyOpFnMapSeq(item_l[1], item_l[2]); // fct_v, arg_lv
  if (item_l[1].Is(TAG_TYPE_SEM_EQORD)) {
    ApplyOpFnMapSeq(Record(item_l[1]).GetRecord(pos_SEM_EQORD_v), item_l[2]); // fct_v, arg_lv
  }
  else {
    ApplyOpFnMapSeq(item_l[1], item_l[2]); // fct_v, arg_lv
  }
}

// ApplyOpFnMapSeq
// fct_v : SEM`VAL
// arg_lv : (seq of SEM`VAL | SEM`VAL)
// ==> ()
void StackEval::ApplyOpFnMapSeq(const TYPE_SEM_VAL & fct_v, const Generic & arg_lv)
{
//wcout << L"ApplyOpFnMapSeq: " << fct_v << L" " << arg_lv << endl;
  switch (fct_v.GetTag()) {
    case TAG_TYPE_SEM_MAP:        { PUSH(EXPR::EvalMapApply(fct_v, arg_lv)); break; }
    case TAG_TYPE_SEM_SEQ:        { PUSH(EXPR::EvalSeqApply(fct_v, arg_lv)); break; }
    case TAG_TYPE_SEM_CompExplFN: { EvalCompExplFnApply(fct_v, arg_lv); break; }
    case TAG_TYPE_SEM_ExplOP:     { EvalExplOpApply(fct_v, arg_lv); break; }
#ifdef VDMSL
    case TAG_TYPE_SEM_DLFN:       { EvalDLFnApply(fct_v, arg_lv); break; }
    case TAG_TYPE_SEM_DLOP:       { EvalDLOpApply(fct_v, arg_lv); break; }
#endif //VDMSL
#ifdef VDMPP
    case TAG_TYPE_SEM_OverOPFN:   { EvalOverOpFnApply(fct_v, arg_lv); break; }
#endif //VDMPP
    case TAG_TYPE_SEM_ExplPOLY:   { RTERR::Error(L"ExeAPPLY", RTERR_POLY_NOT_INST, Nil(), Nil(), Sequence()); break; }
    default: { RTERR::Error(L"ExeAPPLY", RTERR_APPLY_NONAPPVAL, Nil(), Nil(), Sequence()); break; }
  }
}

// EvalCompExplFnApply
// fns : SEM`CompExplFN
// arg : (SEM`VAL | seq of SEM`VAL)
// ==> ()
void StackEval::EvalCompExplFnApply (const TYPE_SEM_CompExplFN & fns, const Generic & arg)
{
  SEQ<TYPE_SEM_VAL> arg_lv (arg.IsSequence() ? SEQ<TYPE_SEM_VAL>(arg)
                                             : SEQ<TYPE_SEM_VAL>().ImpAppend(arg));

  // Unpack fns
  const SEQ<TYPE_SEM_ExplFN> & fn_l (fns.GetSequence(pos_SEM_CompExplFN_fl));
  const Generic & curobjref (fns.GetField(pos_SEM_CompExplFN_objref)); // always null in VDMSL

  if (fn_l.Length() > 1) {
    // set rest composed function to stack
    PUSH(TYPE_SEM_CompExplFN().Init(fn_l.Tl(), curobjref));
    INCRPC(-1);
  }

  EvalExplFnApply(fn_l.Hd(), arg_lv);
}

// EvalExplFnApply
// fct_v : SEM`ExplFN
// arg_lv : seq of SEM`VAL
// ==> ()
void StackEval::EvalExplFnApply(const TYPE_SEM_ExplFN & fct_v, const SEQ<TYPE_SEM_VAL> & arg_lv)
{
  // TypeVar for Lamba function in argument list must be converted here
  SEQ<TYPE_SEM_VAL> arg_lv_q (UpdateFunctionValue(arg_lv));

  if (fct_v.GetField(pos_SEM_ExplFN_instr).IsNil()) {
    if (arg_lv_q.Length() == 1) {
      PUSH(arg_lv_q.Hd());
    }
    else {
      PUSH(TYPE_SEM_TUPLE().Init(arg_lv_q));
    }
  }
  else {
    if (fct_v.GetSequence(pos_SEM_ExplFN_parms).Length() > 1) {
      // DO nothing;
      // A high order function. A new semantic value of a function
      // is now pushed on the stack.
    }
    else {
      const Generic & fnName (fct_v.GetField(pos_SEM_ExplFN_fnName));
      TYPE_AS_Name newNm;
      if (fnName.IsNil()) {
        newNm = ASTAUX::MkNameFromId(ASTAUX::MkId(lambdaFnName), NilContextId);
      }
      else {
        const TYPE_AS_Name & clmodName (fct_v.GetRecord(pos_SEM_ExplFN_modName));
        if (clmodName.GetSequence(pos_AS_Name_ids).IsEmpty()) {
          newNm = fnName;
        }
        else {
          newNm = AUX::ConstructDoubleName(clmodName, fnName);
        }
      }

#if defined( PROFILELOG )
      StartProfile(newNm);
#endif // PROFILELOG

      PushCS(fct_v, newNm, arg_lv_q, CallStackItemType::CS_FNOP);
    }
    EnvSetUpExplFnApply(fct_v, arg_lv_q);
  }
}

// UpdateFunctionValue
// arg_lv : SEM`VAL
// ==> SEM`VAL
SEQ<TYPE_SEM_VAL> StackEval::UpdateFunctionValue(const SEQ<TYPE_SEM_VAL> & arg_lv)
{
  Map tm (HdTypeInst());
  if (tm.IsEmpty()) {
    return arg_lv;
  }
  else {
    SEQ<TYPE_SEM_VAL> arg_lv_q (arg_lv);
    size_t len_arg_lv = arg_lv.Length();
    for (size_t index = 1; index <= len_arg_lv; index++) {
      TYPE_SEM_VAL val (arg_lv_q[index]);
      if (val.Is(TAG_TYPE_SEM_CompExplFN)) {
        const SEQ<TYPE_SEM_ExplFN> & fl (val.GetSequence(pos_SEM_CompExplFN_fl)); 
        SEQ<TYPE_SEM_ExplFN> new_fl;
        size_t len_fl = fl.Length();
        for (size_t j = 1; j <= len_fl; j++) {
          TYPE_SEM_ExplFN efn (fl[j]);
          const Record & tp (efn.GetRecord(pos_SEM_ExplFN_tp));
          const Sequence & parms (efn.GetSequence(pos_SEM_ExplFN_parms));
          Sequence new_parms; // seq of seq of STKM`Pattern
          size_t len_parms = parms.Length();
          for (size_t k = 1; k <= len_parms; k++) {
            const SEQ<TYPE_STKM_Pattern> & p_l (parms[k]);
            SEQ<TYPE_STKM_Pattern> new_p_l;
            size_t len_p_l = p_l.Length();
            for (size_t l = 1; l <= len_p_l; l++) {
              new_p_l.ImpAppend(theState().ConvertPattern(p_l[l], tm));
            }
            new_parms.ImpAppend(new_p_l);
          }
          efn.SetField(pos_SEM_ExplFN_tp, theState().ConvertTypeVarType(tp, tm));
          efn.SetField(pos_SEM_ExplFN_parms, new_parms);
          new_fl.ImpAppend(efn);
        }
        val.SetField(pos_SEM_CompExplFN_fl, new_fl);
        arg_lv_q.ImpModify(index, val);
      }
    }
    return arg_lv_q;
  }
}

// EnvSetUpExplFnApply
// fndef : SEM`ExplFN
// arg_lv : seq of SEM`VAL
// ==> ()
void StackEval::EnvSetUpExplFnApply(const TYPE_SEM_ExplFN & fndef, const SEQ<TYPE_SEM_VAL> & arg_lv)
{
  // Unpack fndef
  const SEQ<type_dL> & parms      (fndef.GetSequence(pos_SEM_ExplFN_parms));
  const TYPE_AS_Name & clmodName  (fndef.GetRecord(pos_SEM_ExplFN_modName));

#ifdef VDMSL
  PUSHMODULE(clmodName);
#endif //VDMSL
#ifdef VDMPP
  const Generic & objref (fndef.GetField(pos_SEM_ExplFN_objref));
  if (objref.IsNil()) {
    PushClNmCurObj(clmodName, clmodName);
  }
  else {
    PushCurObj(objref, clmodName, clmodName);
  }
#endif //VDMPP

//wcout << L"call fn: " << fndef.GetField(pos_SEM_ExplFN_fnName) << endl;
  PushOS();

  size_t len_arg_lv = arg_lv.Length();
  if (!parms.IsEmpty() && (len_arg_lv != (size_t)(parms[1].Length()))) {
    RTERR::Error(L"EnvSetUpExplFnApply", RTERR_WRONG_NO_OF_ARGS, arg_lv, Nil(), Sequence());
    return;
  }
  else if (Settings.DTC()) {
    const TYPE_AS_FnType & tp (fndef.GetRecord(pos_SEM_ExplFN_tp));
    const SEQ<TYPE_AS_Type> & fndom (tp.GetSequence(1)); // DiscretionaryType
    if (len_arg_lv != (size_t)(fndom.Length())) {
      RTERR::Error(L"EnvSetUpExplFnApply", RTERR_WRONG_NO_OF_ARGS, arg_lv, fndom, Sequence());
      return;
    }
    else {
      for (size_t i = 1; i <= len_arg_lv; i++) {
        if (!theState().SubType(arg_lv[i], fndom[i])) {
          RTERR::Error(L"EnvSetUpExplFnApply", RTERR_TYPE_INCOMP_IN_FNOP_APPLY, arg_lv[i], fndom[i], Sequence());
          return;
        }
      }
    }
  }

  SET<TYPE_SEM_BlkEnv> env_s (PAT::MatchLists(parms.Hd(), arg_lv));
  if (env_s.IsEmpty()) {
    RTERR::Error(L"EnvSetUpExplFnApply", RTERR_EMPTY_ENV_S, Nil(), Nil(), Sequence());
  }
  else {
    const TYPE_SEM_BlkEnv & closenv (fndef.GetRecord(pos_SEM_ExplFN_env));
    TYPE_SEM_BlkEnv env (env_s.GetElem());

    if (parms.Length() > 1) {
      // higher order function
      // only update tp, parms, env
      TYPE_SEM_ExplFN fn (fndef);
      fn.SetField(pos_SEM_ExplFN_tp,    fndef.GetRecord(pos_SEM_ExplFN_tp).GetRecord(2));
      fn.SetField(pos_SEM_ExplFN_parms, parms.Tl());
      fn.SetField(pos_SEM_ExplFN_env,   AUX::CombineBlkEnv(closenv, env));

      PUSH(SemRec::CompFN(fn));

      PopOS();
#ifdef VDMSL
      POPMODULE();
#endif //VDMSL
#ifdef VDMPP
      if (objref.IsNil()) {
        PopClNmCurObj();
      }
      else {
        PopCurObj();
      }
#endif //VDMPP
    }
    else {
      const Generic & fnName          (fndef.GetField(pos_SEM_ExplFN_fnName));
      TYPE_SEM_BlkEnv closenv_q (closenv);
      if (!fnName.IsNil()) { // not lambda function
        Tuple ilv (IsLocalVal(fnName)); // must check before modifying env_l(PUSHEMPTYENV)
        if (ilv.GetBoolValue(1)) {
          closenv_q = AUX::CombineBlkEnv(closenv, AUX::MkBlkEnv(fnName,
                 Record(ilv.GetRecord(2)).GetRecord(pos_SEM_ValTp_val),
                 Record(ilv.GetRecord(2)).GetField(pos_SEM_ValTp_tp), sem_read_only));
        }
      }
      //PUSHEMPTYENV();
      //PUSHBLKENV(closenv_q);
      //PUSHBLKENV(env);
      PUSHENVL(mk_sequence(env, closenv_q));

      PushTypeInst(fndef.GetMap(pos_SEM_ExplFN_tm));
#ifdef VDMPP
      if (!fnName.IsNil() && (!clmodName.GetSequence(pos_AS_Name_ids).IsEmpty())) {
        if (theState().IsDLOp(clmodName, fnName)) {
          PUSH(arg_lv);
        }
      }
#endif // VDMPP
    }
  }
}

// EvalExplOpApply
// opsem : SEM`ExplOP
// srg_lv : seq of SEM`VAL
// ==> ()
void StackEval::EvalExplOpApply(const TYPE_SEM_ExplOP & opsem, const SEQ<TYPE_SEM_VAL> & arg_lv)
{
  if (HdContext() == Int(PUREOP)) {
    if (!opsem.GetBoolValue(pos_SEM_ExplOP_oppure)) {
      RTERR::Error(L"EvalExplOpApply", RTERR_PURE_OP_CALL, Nil(), Nil(), Sequence());
      return;
    }
  }

  const TYPE_AS_Name & clmodName (opsem.GetRecord(pos_SEM_ExplOP_modName));
  const TYPE_AS_Name & opName    (opsem.GetRecord(pos_SEM_ExplOP_fnName));
  TYPE_AS_Name newNm (AUX::ConstructDoubleName(clmodName, opName));

#if defined( PROFILELOG )
  StartProfile(newNm);
#endif // PROFILELOG

#ifdef VDMPP
#ifdef VICE
  if( !opsem.GetBoolValue(pos_SEM_ExplOP_sync) && !theSystem().OnCurCPU(opsem.GetField(pos_SEM_ExplOP_objref)) ) {
    PUSH(sem_cont);
    return;
  }
#endif // VICE
  if (!theState().IsClassInit(clmodName)) {
    theState().InitClassName(clmodName);
  }
#endif // VDMPP

  PushCS(opsem, newNm, arg_lv, CallStackItemType::CS_FNOP);

  EnvSetUpExplOpApply(opsem, arg_lv);
}

// EnvSetUpExplOpApply
// fndef : SEM`ExplOP
// srg_lv : seq of SEM`VAL
// ==> ()
void StackEval::EnvSetUpExplOpApply(const TYPE_SEM_ExplOP & opsem, const SEQ<TYPE_SEM_VAL> & arg_lv)
{
  // Unpack fndef
  const SEQ<Sequence> & parms    (opsem.GetSequence(pos_SEM_ExplOP_parms));
  const TYPE_AS_Name & clmodName (opsem.GetRecord(pos_SEM_ExplOP_modName));

#ifdef VDMSL
  PUSHMODULE(clmodName);
#endif //VDMSL
#ifdef VDMPP
  const Generic & objref (opsem.GetField(pos_SEM_ExplOP_objref));
  if (!objref.IsNil()) {
    PushCurObj(objref, clmodName, clmodName);
  }
  else {
    PushClNmCurObj(clmodName, clmodName);
  }
#endif //VDMPP

//wcout << L"call op: " << opsem.GetRecord(pos_SEM_ExplOP_fnName) << endl;
  PushOS();

  size_t len_arg_lv = arg_lv.Length();
  if (len_arg_lv != (size_t)(parms.Length())) {
    RTERR::Error(L"EnvSetUpExplOpApply", RTERR_WRONG_NO_OF_ARGS, arg_lv, Nil(), Sequence());
    return;
  }
  else if (Settings.DTC()) {
    const TYPE_AS_OpType & tp (opsem.GetRecord(pos_SEM_ExplOP_tp));
    const SEQ<TYPE_AS_Type> & opdom (tp.GetSequence(pos_AS_OpType_opdom));
    if (len_arg_lv != (size_t)(opdom.Length())) {
      RTERR::Error(L"EnvSetUpExplOpApply", RTERR_WRONG_NO_OF_ARGS, arg_lv, opdom, Sequence());
      return;
    }
    else {
      for (size_t i = 1; i <= len_arg_lv; i++) {
        if (!theState().SubType(arg_lv[i], opdom[i])) {
          RTERR::Error(L"EnvSetUpExplOpApply", RTERR_TYPE_INCOMP_IN_FNOP_APPLY, arg_lv[i], opdom[i], Sequence());
          return;
        }
      }
    }
  } // end of if (Settings.DTC())

  SET<TYPE_SEM_BlkEnv> env_s (PAT::MatchLists(parms, arg_lv));
  if (env_s.IsEmpty()) {
    RTERR::Error(L"EnvSetUpExplOpApply", RTERR_EMPTY_ENV_S, Nil(), Nil(), Sequence());
  }
  else {
    //PUSHEMPTYENV();
    //PUSHBLKENV(env_s.GetElem());
    PUSHENVL(mk_sequence(env_s.GetElem()));

#ifdef VDMPP
    const TYPE_AS_Name & opName (opsem.GetRecord(pos_SEM_ExplOP_fnName));
    if (theState().IsDLOp(clmodName, opName)) {
      PUSH(arg_lv);
    }

    const Generic & manglenm (opsem.GetField(pos_SEM_ExplOP_manglenm));

    TYPE_AS_Name opnm (manglenm.IsNil() ? opName : TYPE_AS_Name(manglenm));
    TYPE_AS_Name fullopname (AUX::ConstructDoubleName(clmodName, opnm));

#ifdef VICE
    if( clmodName == opName ) { // use of constructor
      theState().UpdateHistCount(fullopname, EvalState::historyKind_req, objref, Bool(true), arg_lv);
    }
    theState().UpdateHistCount(fullopname, EvalState::historyKind_act, objref,
                               opsem.GetBool(pos_SEM_ExplOP_sync), Sequence());
#else
    theState().UpdateHistCount(fullopname, EvalState::historyKind_act, objref);
#endif // VICE
#endif //VDMPP
  }
}

#ifdef VDMSL
// EvalDLFnApply
// fndef : SEM`DLFN
// srg_l : seq of SEM`VAL
// ==> ()
void StackEval::EvalDLFnApply(const TYPE_SEM_DLFN & fndef, const SEQ<TYPE_SEM_VAL> & arg_l)
{
  // Unpack fndef
  TYPE_AS_FnType tp (fndef.get_tp());
  SEQ<TYPE_AS_Type> fndom (tp.GetSequence(1));
  TYPE_AS_Type rng (tp.GetRecord(2));

  TYPE_AS_Name clmodName (fndef.get_modName());
  TYPE_AS_Name fnname (fndef.get_fnName());
  TYPE_SEM_LOC body (fndef.get_body());

  PUSHMODULE(clmodName);

  if (arg_l.Length() != fndom.Length()) {
    RTERR::Error(L"EvalDLFnApply", RTERR_WRONG_NO_OF_ARGS, arg_l, Nil(), Sequence());
   return;
  }

  if (Settings.DTC()) {
    size_t len_fndom = fndom.Length();
    for (size_t i = 1; i <= len_fndom; i++) {
      if (!theState().SubType(arg_l[i], fndom[i])) {
        RTERR::Error(L"EvalDLFnApply", RTERR_NUMBER_ARG_NEQ_NUMBER_TYPE, arg_l[i], fndom[i], Sequence());
        return;
      }
    }
  }

//  TYPE_SEM_VAL res (theState().DLCallOp(clmodName, fnname, rng.IsNil(), arg_l));
  TYPE_SEM_VAL res (theState().DLCallOp(clmodName, fnname, false, arg_l));

  POPMODULE(); // FIXME etn 20000919 is it right to call this before SubType?

  if (Settings.DTC() && !theState().SubType(res, rng)) {
    RTERR::Error(L"EvalExtFnApply", RTERR_TYPE_INCOMP, res, rng, Sequence());
  }
  else {
    PUSH(res);
  }
}

// EvalDLOpApply
// fndef : SEM`DLOP
// arg_l : seq of SEM`VAL
// ==> ()
void StackEval::EvalDLOpApply(const TYPE_SEM_DLOP & fndef, const SEQ<TYPE_SEM_VAL> & arg_l)
{
  // Unpack fndef
  const TYPE_AS_OpType & tp      (fndef.GetRecord(pos_SEM_DLOP_tp));
  const TYPE_AS_Name & clmodName (fndef.GetRecord(pos_SEM_DLOP_modName));
  const TYPE_AS_Name & fnname    (fndef.GetRecord(pos_SEM_DLOP_fnName));
  const TYPE_AS_Type & rng       (tp.GetRecord(pos_AS_OpType_oprng));

  PUSHMODULE(clmodName);

  if (arg_l.Length() != tp.get_opdom().Length()) {
    RTERR::Error(L"EvalDLOpApply", RTERR_WRONG_NO_OF_ARGS, arg_l, tp.get_opdom(), Sequence());
    return;
  }

  if (Settings.DTC()) {
    size_t len_tp = tp.get_opdom().Length();
    for (size_t i = 1; i <= len_tp; i++) {
      if (!theState().SubType(arg_l[i], tp.get_opdom()[i])) {
        RTERR::Error(L"EvalDLOpApply", RTERR_NUMBER_ARG_NEQ_NUMBER_TYPE, arg_l[i], tp.get_opdom()[i], Sequence());
        return;
      }
    }
  }

  TYPE_SEM_VAL res (theState().DLCallOp(clmodName, fnname, rng.Is(TAG_TYPE_AS_VoidType), arg_l));

  POPMODULE();

  if (Settings.DTC() && !rng.Is(TAG_TYPE_AS_VoidType) &&
      !theState().SubType(res, rng)) {
     RTERR::Error(L"EvalExtOpApply", RTERR_TYPE_INCOMP, res, tp.get_oprng(), Sequence());
  }
  else {
    PUSH(res);
  }
}
#endif //VDMSL

#ifdef VDMPP
// EvalOverOpFnApply
// fct_v : SEM`OverOPFN
// arg_lv : seq of SEM`VAL
// ==> ()
void StackEval::EvalOverOpFnApply(const TYPE_SEM_OverOPFN & fct_v, const SEQ<TYPE_SEM_VAL> & arg_lv)
{
  const Map & over (fct_v.GetMap(pos_SEM_OverOPFN_overload));
                         // map (AS`Name * AS`Name) to ((seq of AS`Type) * AS`Access * [SEM`CompExplFN])
  const Generic & objref (fct_v.GetField(pos_SEM_OverOPFN_objref));

  Set dom_over (over.Dom());
  size_t arglen = arg_lv.Length();

  Map m;
  Generic g;
  for (bool bb = dom_over.First(g); bb; bb = dom_over.Next(g)) {
    Tuple domTup (g);        // (AS`Name * AS`Name)
    const TYPE_AS_Name & manglenm (domTup.GetRecord(1)); // AS`Name
    const TYPE_AS_Name & clsnm (domTup.GetRecord(2));    // AS`Name
    Tuple regTup (over[g]);  // ((seq of AS`Type) * AS`Access)
    const SEQ<TYPE_AS_Type> & tp_l (regTup.GetSequence(1));  // seq of AS`Type

    if ((size_t)(tp_l.Length()) == arglen) {
      bool allSubType = true;
      for (size_t i = 1; (i <= arglen) && allSubType; i++) {
        allSubType = (theState().RealSubType(arg_lv[i], tp_l[i], false));
      }
      if (allSubType) {
        if (m.Dom().InSet(clsnm)) { 
          RTERR::Error(L"EvalOverOpFnApply", RTERR_MULTIOVERLOADED, Nil(), Nil(), Sequence());
          return;
        }
        else {
          m.Insert(clsnm, manglenm);
        }
      }
    }
  }

  if (m.IsEmpty()) {
    RTERR::Error(L"EvalOverOpFnApply", RTERR_NOOVERLOADED, fct_v, Nil(), Sequence());
  }
  else if (!CheckMultiOverLoaded(m, fct_v, arg_lv)) {
    RTERR::Error(L"EvalOverOpFnApply", RTERR_MULTIOVERLOADED, Nil(), Nil(), Sequence());
  }
  else {
    Set nm_s (m.Dom());
    Tuple existsTup (theState().ExistsOneChild(nm_s, nm_s));
    if (existsTup.GetBoolValue(1)) {
      const TYPE_AS_Name & child (existsTup.GetRecord(2));
      TYPE_AS_Name manglenm (m[child]);
      Tuple lafop (theState().LookupAllFnsOpsPolys(child, manglenm));
      if (lafop.GetBool(1)) {
        Tuple t (lafop.GetTuple(2));
        const TYPE_SEM_VAL & opval (t.GetRecord(1)); // SEM`VAL
        if (theState().AccessOk(t.GetField(2), GetOrigCl(), child)) {
          TYPE_SEM_VAL f (opval); // SEM`FN | SEM`OP
          switch(f.GetTag()) {
            case TAG_TYPE_SEM_ExplOP: {
              f.SetField(pos_SEM_ExplOP_objref, objref);
              f.SetField(pos_SEM_ExplOP_manglenm, manglenm);
              break;
            }
            case TAG_TYPE_SEM_ExplFN: {
              f.SetField(pos_SEM_ExplFN_objref, objref);
              break;
            }
            case TAG_TYPE_SEM_CompExplFN: {
              f.SetField(pos_SEM_CompExplFN_objref, objref);
              const SEQ<TYPE_SEM_ExplFN> & fn_l (f.GetSequence(pos_SEM_CompExplFN_fl));
              SEQ<TYPE_SEM_ExplFN> new_fn_l;
              size_t len_fn_l = fn_l.Length();
              for (size_t idx = 1; idx <= len_fn_l; idx++) {
                TYPE_SEM_ExplFN fn (fn_l[idx]);
                fn.SetField(pos_SEM_ExplFN_objref, objref);
                new_fn_l.ImpAppend(fn);
              }
              f.SetField(pos_SEM_CompExplFN_fl, new_fn_l);
              break;
            }
            case TAG_TYPE_SEM_ExplPOLY: {
              Tuple t (over[mk_(manglenm, f.GetRecord(pos_SEM_ExplPOLY_modName))]);
              Generic fng (t.GetField(3));
              if (fng.IsRecord()) {
                TYPE_SEM_VAL fn (fng);
                if (fn.Is(TAG_TYPE_SEM_CompExplFN)) {
                  fn.SetField(pos_SEM_CompExplFN_objref, objref);
                  const SEQ<TYPE_SEM_ExplFN> & fn_l (fn.GetSequence(pos_SEM_CompExplFN_fl));
                  SEQ<TYPE_SEM_ExplFN> new_fn_l;
                  size_t len_fn_l = fn_l.Length();
                  for (size_t idx = 1; idx <= len_fn_l; idx++) {
                    TYPE_SEM_ExplFN fn (fn_l[idx]);
                    fn.SetField(pos_SEM_ExplFN_objref, objref);
                    new_fn_l.ImpAppend(fn);
                  }
                  fn.SetField(pos_SEM_CompExplFN_fl, new_fn_l);
                  f = fn;
                }
              }
              break;
            }
          }
          ApplyOpFnMapSeq(f, arg_lv);
        }
        else {
          RTERR::Error(L"EvalOverOpFnApply", RTERR_NOT_IN_SCOPE, Nil(), Nil(), Sequence());
        }
      }
      else {
        RTERR::Error(L"EvalOverOpFnApply", RTERR_NOT_IN_SCOPE, Nil(), Nil(), Sequence());
      }
    }
    else {
      RTERR::Error(L"EvalOverOpFnApply", RTERR_MULTIOVERLOADED, Nil(), Nil(), Sequence());
    }
  }
}

// CheckMultiOverLoaded
// m : map AS`Name to AS`Name
// fct_v : SEM`OverOPFN
// arg_lv :  seq of SEM`VAL
// ==> bool
bool StackEval::CheckMultiOverLoaded(const MAP<TYPE_AS_Name, TYPE_AS_Name> & m,
                                     const TYPE_SEM_OverOPFN & fct_v, const SEQ<TYPE_SEM_VAL> & arg_lv)
{
  const Map & over (fct_v.GetMap(pos_SEM_OverOPFN_overload));
                         // map (AS`Name * AS`Name) to ((seq of AS`Type) * AS`Access * [SEM`CompExplFN])
  const Generic & objref (fct_v.GetField(pos_SEM_OverOPFN_objref));
  Set dom_over (over.Dom());

  Set nm_s (m.Dom());
  Tuple t (dom_over.GetElem());
  const TYPE_AS_Name & manglenm (t.GetRecord(1));
  TYPE_AS_Name mnm (MANGLE::IsMangled(manglenm) ? MANGLE::GetUnmangledName(manglenm) : manglenm);
  bool defcon = (theState().IsAClass(mnm) && arg_lv.IsEmpty()); // default constructor

  bool stat = false;
  if (objref.IsNil() && theStackMachine().HasCurObjRef()) {
    TYPE_AS_Name n (theStackMachine().GetCurObjName());
    if (theState().GetAllSupers(n).Diff(nm_s).IsEmpty()) {
      stat = true;
    }
  }
  if ((!objref.IsNil() || theStackMachine().HasCurObjRef()) && !defcon && !stat) {
    Set done;
    Set notdone;
    if (!objref.IsNil()) {
       notdone.Insert(mk_sequence(TYPE_AS_Name(Record(objref).GetRecord(pos_SEM_OBJ_uRef_tp))));
    }
    else {
      notdone.Insert(mk_sequence(theStackMachine().GetCurObjName()));
    }
    while (!notdone.IsEmpty()) {
      Set newnd; // set of seq of AS`Name
      Generic s;
      for (bool bb = notdone.First(s); bb; bb = notdone.Next(s)) {
        Sequence ts (s);
        Set s_s (theState().GetSupers(ts[1]));
        if (s_s.IsEmpty()) {
          done.Insert(ts);
        }
        else {
          Generic n;
          for (bool cc = s_s.First(n); cc; cc = s_s.Next(n)) {
            newnd.Insert(Sequence(ts).ImpPrepend(n));
          }
        }
      }
      notdone = newnd;
    }
    bool multpath = false;
    Generic t;
    for (bool dd = done.First(t); dd && !multpath; dd = done.Next(t)) {
      Set s (nm_s.Intersect(Sequence(t).Elems()));
       multpath = (!s.IsEmpty() && (s != nm_s));
    }
    return !multpath;
  }
  else {
    return true;
  }
}
                                 
// ExeCALLGUARD
// hasobj : bool
// oprt : AS`Name
// ==> ()
void StackEval::ExeCALLGUARD(const Bool & hasobj, const TYPE_AS_Name & oprt)
{
  SEQ<TYPE_SEM_VAL> args (POP());

  Generic obj;
  if (hasobj) {
    obj = POP();
  }
  else {
//  if (HasCurCl())
    if (HasCurObjRef()) {
      obj = GetCurObjRef();
    }
    else {
      obj = Nil();
    }
  }

  Generic op_v (ConvertOverOPFNToExplOP(oprt, obj, hasobj, args));

  if( !obj.IsNil() && !obj.Is(TAG_TYPE_SEM_OBJ_uRef) ) {
    RTERR::Error(L"ExeCALLGUARD", RTERR_OBJ_REF_EXP_CALL, Nil(), Nil(), Sequence());
  }
  else if (!op_v.Is(TAG_TYPE_SEM_ExplOP)) {
    RTERR::Error(L"ExeCALLGUARD", RTERR_NOT_EXPL_OP_CALL, Nil(), Nil(), Sequence());
  }
  else {
    GuardCheck(op_v, obj, args, Bool(true));
    PUSH(args);
    PUSH(op_v);
  }
}

// ConvertOverOPFNToExplOP
// op : (SEM`VAL | AS`Name)
// obj : [SEM`OBJ_Ref]
// hasobj : bool
// args : seq of SEM`VAL
// ==> [SEM`VAL]
Generic StackEval::ConvertOverOPFNToExplOP(const Record & op, const Generic & obj,
                                           const Bool & hasobj, const SEQ<TYPE_SEM_VAL> & args)
{
  if (hasobj) {
    if (obj.Is(TAG_TYPE_SEM_OBJ_uRef)) {
      PushCurObj(obj, Nil(), GetCurCl());
    }
    else {
      SetCid(ASTAUX::GetCid(op));
      RTERR::Error(L"ConvertOverOPFNToExplOP", RTERR_OBJ_REF_EXP, obj, Nil(), Sequence());
      return Nil();
    }
  }

  Generic resval = Nil();
  if (op.Is(TAG_TYPE_AS_Name)) {
    if (hasobj) { // object
      Tuple lofp (theState().LookOpFctPoly(op));
      if (lofp.GetBoolValue(1) && lofp.GetBoolValue(2)) {
        resval = lofp.GetRecord(3);
      }
      else {
        SetCid(ASTAUX::GetCid(op));
        RTERR::Error(L"ConvertOverOPFNToExplOP", RTERR_OP_OR_FUN_NOT_IN_SCOPE, Nil(), Nil(), Sequence());
        return Nil();
      }
    }
    else
    { // TODO
      // static
      if ((Record(op).GetSequence(pos_AS_Name_ids).Length() == 1)) {
        TYPE_SEM_ValTp valTp (theState().LookUp(op));
        resval = valTp.GetRecord(pos_SEM_ValTp_val);
      }
      else if (Record(op).GetSequence(pos_AS_Name_ids).Length() == 2) {
        Tuple lsofp (theState().LookStaticOpFctPoly(ASTAUX::GetFirstName(op), AUX::ExtractName(op)));
        if (lsofp.GetBoolValue(1) && lsofp.GetBoolValue(2)) {
          resval = lsofp.GetRecord(3);
        }
        else {
          SetCid(ASTAUX::GetCid(op));
          RTERR::Error(L"ConvertOverOPFNToExplOP", RTERR_STATIC_NOT_IN_SCOPE, Nil(), Nil(), Sequence());
          return Nil();
        }
      }
      else {
        TYPE_SEM_ValTp valTp (theState().LookUp(op));
        resval = valTp.GetRecord(pos_SEM_ValTp_val);
      }
    }
  }
  else {
    resval = op;
  }

  Generic realnm = Nil(); // [AS`Name]
  if (resval.Is(TAG_TYPE_SEM_OverOPFN)) {
    // map (AS`Name * AS`Name) to ((seq of AS`Type) * AS`Access)
    const Map & over (Record(resval).GetMap(pos_SEM_OverOPFN_overload));
    TYPE_GLOBAL_OrigCl origcl (GetOrigCl()); 
    Set dom_over (over.Dom());
    Map m_s;
    Generic manglenm_clsnm; // (AS`Name * AS`Name)
    for (bool bb = dom_over.First(manglenm_clsnm); bb; bb = dom_over.Next(manglenm_clsnm)) {
      Tuple tp_l_access (over[manglenm_clsnm]); // ((seq of AS`Type) * AS`Access)
      const SEQ<TYPE_AS_Type> & tp_l (tp_l_access.GetSequence(1));
      if (tp_l.Length() == args.Length()) {
        size_t len = args.Length();
        bool forall = true;
        for(size_t i = 1; (i <= len) && forall; i++) {
          forall = theState().RealSubType(args[i], tp_l[i], false);
        } 
        if (forall) {
          Tuple tmp (manglenm_clsnm);  // AS`Name * AS`Name
          const TYPE_AS_Name & manglenm (tmp.GetRecord(1));
          const TYPE_AS_Name & clsnm (tmp.GetRecord(2));
          Tuple lafop (theState().LookupAllFnsOpsPolys(clsnm, manglenm));
          if (lafop.GetBoolValue(1)) {
            Tuple fnac (lafop.GetTuple(2)); // SEM`VAL * AS`Access
            const TYPE_SEM_VAL & opfn (fnac.GetRecord(1)); // SEM`VAL
            if (theState().AccessOk(fnac.GetField(2), origcl, clsnm)) {
              if(!m_s.DomExists(clsnm))
                m_s.Insert(clsnm, mk_(opfn, manglenm)); //
//              else
//                RTERR::Error(L"ConvertOverOPFNToExplOP", RTERR_MULTIOVERLOADED, Nil(), Nil(), Sequence());
            }
          }
        }
      }
    }
//wcout << L"m_s: " << m_s << endl;
    if (!m_s.IsEmpty())
    {
      Tuple eoc (theState().ExistsOneChild(m_s.Dom(), m_s.Dom()));
      if (eoc.GetBoolValue(1))
      {
        Tuple t (m_s[eoc.GetField(2)]); // SEM`VAL * AS`Name
        resval = t.GetField(1);
        realnm = t.GetField(2);
      }
    }
  } // if (resval.Is(TAG_TYPE_SEM_OverOPFN))

  //if (hasobj)
  if (hasobj && obj.Is(TAG_TYPE_SEM_OBJ_uRef)) //
    PopCurObj();

  if (resval.Is(TAG_TYPE_SEM_ExplOP))
  {
    TYPE_SEM_ExplOP eop (resval);
    if (eop.GetField(pos_SEM_ExplOP_objref).IsNil())
      eop.SetField(pos_SEM_ExplOP_objref, obj);
    eop.SetField(pos_SEM_ExplOP_manglenm, realnm);
    return eop;
  }
  else
    return resval;
}

// ExePPCALL
// ==> ()
void StackEval::ExePPCALL()
{
  SEQ<TYPE_STKM_EvalStackItem> item_l (POPN(2)); // [args, op_v]

  EvalExplOpApply(item_l[2], item_l[1]); // op_v, args
}
#endif //VDMPP

// ExeRETURN
// ==> ()
void StackEval::ExeRETURN()
{
  CleanFunctionApply();
}

// ExeGETPRE
// ==> ()
void StackEval::ExeGETPRE()
{
  TYPE_STKM_EvalStackItem val (POP());

  if (val.Is(TAG_TYPE_SEM_CompExplFN)) {
    TYPE_SEM_ExplFN fn (Record(val).GetSequence(pos_SEM_CompExplFN_fl).Hd());
    const TYPE_AS_Name & mnm (fn.GetField(pos_SEM_ExplFN_modName));
    const Generic & fnnm (fn.GetField(pos_SEM_ExplFN_fnName));

    if (!fnnm.IsNil()) {
      Tuple ilv (theStackMachine().IsLocalVal(AUX::PreName(fnnm)));
      if (ilv.GetBoolValue(1)) {
        PUSH(Record(ilv.GetRecord(2)).GetRecord(pos_SEM_ValTp_val));
        PUSH(sem_true);
      }
      else {
        if (!mnm.GetSequence(pos_AS_Name_ids).IsEmpty()) {
          Generic preval (theState().GetPreCond(mnm, fnnm)); // [SEM`CompExplFN]
          if (!preval.IsNil()) {
            PUSH(preval);
            PUSH(sem_true);
          }
          else
            PUSH(sem_false);
        }
        else
          PUSH(sem_false);
      }
    }
    else
      PUSH(sem_false);
  }
  else
    PUSH(sem_false);
}

// ExeISCHECK
// Type : AS`BasicType | AS`Name
// ==> ()
void StackEval::ExeISCHECK(const Record & Type)
{
  TYPE_SEM_VAL arg_v (POP());

  switch(Type.GetTag()) {
    case TAG_TYPE_AS_BooleanType: {
      PUSH(arg_v.Is(TAG_TYPE_SEM_BOOL) ? sem_true : sem_false);
      break;
    }
    case TAG_TYPE_AS_NumericType: {
      switch(Type.GetIntValue(pos_AS_NumericType_qtp)) {
        case NAT: { // is_nat
          PUSH(arg_v.Is(TAG_TYPE_SEM_NUM) ? (arg_v.GetReal(pos_SEM_NUM_v).IsNat() ? sem_true : sem_false) : sem_false);
          break;
        }
        case NATONE: { // is_nat1
          PUSH(arg_v.Is(TAG_TYPE_SEM_NUM) ? (arg_v.GetReal(pos_SEM_NUM_v).IsNatOne() ? sem_true : sem_false) : sem_false);
          break;
        }
        case INTEGER: { // is_int
          PUSH(arg_v.Is(TAG_TYPE_SEM_NUM) ? (arg_v.GetReal(pos_SEM_NUM_v).IsInt() ? sem_true : sem_false) : sem_false);
          break;
        }
        case REAL: { // is_real
          PUSH(arg_v.Is(TAG_TYPE_SEM_NUM) ? sem_true : sem_false);
          break;
        }
        case RAT: { // is_rat
          PUSH(arg_v.Is(TAG_TYPE_SEM_NUM) ? sem_true : sem_false);
          break;
        }
        default: {
          PUSH(sem_false);
          break;
        }
      }
      break;
    }
    case TAG_TYPE_AS_TokenType: {
      PUSH(arg_v.Is(TAG_TYPE_SEM_TOKEN) ? sem_true : sem_false);
      break;
    }
    case TAG_TYPE_AS_CharType: {
      PUSH(arg_v.Is(TAG_TYPE_SEM_CHAR) ? sem_true : sem_false);
      break;
    }
    case TAG_TYPE_AS_Name: {
      if (arg_v.Is(TAG_TYPE_DYNSEM_SEM_REC)) {
        const TYPE_AS_Name & tag (arg_v.GetRecord(pos_DYNSEM_SEM_SemRecord_tag));
        //if (AUX::LookUpRecSel(tag).GetBoolValue(1)) {
        if (theState().GetCachedRecSel(tag).GetBoolValue(1)) {
#ifdef VDMSL
          Tuple t (AUX::LookUpRename(Type));
          const TYPE_AS_Name & rtag (t.GetRecord(2));
          TYPE_AS_Name tagname (AUX::UniqueTagName(rtag));
#endif // VDMSL
#ifdef VDMPP
          //TYPE_AS_Name tagname (AUX::ExtractTagName(Type).GetRecord(1));
          TYPE_AS_Name tagname (theState().ExtractTagNameCached(Type).GetRecord(1));
#endif //VDMPP
          PUSH((tag == tagname) ? sem_true : sem_false);
        }
        else {
          RTERR::Error(L"ExeISCHECK", RTERR_TAG_UNKNOWN, Nil(), Nil(), Sequence());
        }
      }
      else {
        PUSH(sem_false);
      }
      break;
    }
    default: {
      PUSH(sem_false);
      break;
    }
  }
}

// ExeEOCL
// ==> ()
void StackEval::ExeEOCL()
{
  PopCS();
}

// CleanFunctionApply
// ==> ()
void StackEval::CleanFunctionApply()
{
  TYPE_SEM_VAL semval (HeadCS().GetRecord(pos_STKM_CallStackItem_code));

  switch(semval.GetTag()) {
    case TAG_TYPE_SEM_ExplFN: { CleanExplFnApply(semval); break; }
    case TAG_TYPE_SEM_ExplOP: { CleanExplOpApply(semval); break; }
    default: { RTERR::Error(L"CleanFunctionApply", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence()); break; }
  }
}

// CleanExplFnApply
// fndef : SEM`ExplFN
// ==> ()
void StackEval::CleanExplFnApply(const TYPE_SEM_ExplFN & fndef)
{
  if (Settings.DTC())
  {
    const TYPE_SEM_VAL & res_v (HEAD());
    const TYPE_AS_FnType & tp (fndef.GetRecord(pos_SEM_ExplFN_tp));
    if (!theState().SubType(res_v, tp.GetField(2)))
    {
      RTERR::Error(L"CleanExplFnApply", RTERR_TYPE_INCOMP_RETURN, res_v, tp.GetField(2), Sequence());
      return;
    }
  }

#if defined( PROFILELOG )
  Generic gfnName (fndef.get_fnName());
  TYPE_AS_Name opnm;
  if(gfnName.IsNil())
    opnm = MkNameFromId(MkId(lambdaFnName), NilContextId);
  else
    opnm = AUX::ConstructDoubleName(fndef.get_modName(), gfnName);

  FinishProfile(opnm);
#endif // PROFILELOG

  POPENVL();
//wcout << L"return fn: " <<  fndef.get_fnName() << endl;
  PopOS();

#ifdef VDMSL
  POPMODULE();
#endif //VDMSL
#ifdef VDMPP
  if (fndef.GetField(pos_SEM_ExplFN_objref).IsNil())
    PopClNmCurObj();
  else
    PopCurObj();
#endif //VDMPP

  PopTypeInst();
  PopCS();
}

// CleanExplOpApply
// eop : SEM`ExplOP
// ==> ()
void StackEval::CleanExplOpApply(const TYPE_SEM_ExplOP & eop)
{
  const TYPE_AS_OpType & tp (eop.GetRecord(pos_SEM_ExplOP_tp));
  const TYPE_SEM_VAL & res_v (HEAD());

#ifdef VDMPP
  const TYPE_AS_Name & fnName  (eop.GetRecord(pos_SEM_ExplOP_fnName));
  const TYPE_AS_Name & modName (eop.GetRecord(pos_SEM_ExplOP_modName));

// check for return value 
  if (fnName != modName) // do not check for constructor
  {
#endif // VDMPP
    const TYPE_AS_Type & oprng (tp.GetRecord(pos_AS_OpType_oprng));
    switch(res_v.GetTag()) {
      case TAG_TYPE_SEM_EXIT: {
        break;
      }
      case TAG_TYPE_SEM_CONT:
      case TAG_TYPE_SEM_RETURN: {
        if (!oprng.Is(TAG_TYPE_AS_VoidType)) {
          RTERR::Error(L"CleanExplOpApply", RTERR_OP_RETURNED_CONT, Nil(), Nil(), Sequence());
          return;
        }
        else if (HdContext() == Int(PUREOP)) {
          RTERR::Error(L"CleanExplOpApply", RTERR_OP_RETURNED_CONT, Nil(), Nil(), Sequence());
          return;
        }
        break;
      }
      default: {
        if (oprng.Is(TAG_TYPE_AS_VoidType)) {
          RTERR::Error(L"CleanExplOpApply", RTERR_OP_RETURNED_VALUE, Nil(), Nil(), Sequence());
          return;
        }
        else if (Settings.DTC() && !theState().SubType(res_v, oprng))
        {
          RTERR::Error(L"CleanExplOpApply", RTERR_TYPE_INCOMP_RETURN, res_v, oprng, Sequence());
          return;
        }
        break;
      }
    }
#ifdef VDMPP
  }
#endif // VDMPP

  if (res_v.Is(TAG_TYPE_SEM_RETURN)) {
    //(void) POP();
    //PUSH(sem_cont);
    SETHEAD(sem_cont);
  }

  POPENVL();
  PopOS();

#ifdef VDMSL
  POPMODULE();
#endif //VDMSL

#ifdef VDMPP
  const Generic & manglenm (eop.GetField(pos_SEM_ExplOP_manglenm));

  TYPE_AS_Name opnm (manglenm.IsNil() ? fnName : TYPE_AS_Name(manglenm));
  TYPE_AS_Name fullopname (AUX::ConstructDoubleName(modName, opnm));

#if defined( PROFILELOG )
  FinishProfile(fullopname);
#endif // PROFILELOG

  const Generic & objref (eop.GetField(pos_SEM_ExplOP_objref));
#ifdef VICE
  theState().UpdateHistCount(fullopname,
                             //TYPE_INSTRTP_fin(),
                             EvalState::historyKind_fin,
                             objref,
                             eop.GetBool(pos_SEM_ExplOP_sync),
                             Sequence().ImpAppend(res_v));
#else
  theState().UpdateHistCount(fullopname,
                             //TYPE_INSTRTP_fin(),
                             EvalState::historyKind_fin,
                             objref);
#endif // VICE

#ifdef VICE
  if( fnName == modName )
    theSystem().LateDeployInst();
#endif // VICE

  if (objref.IsNil())
    PopClNmCurObj();
  else
    PopCurObj();
#endif //VDMPP

  PopCS();
}

// EvalUNOP
// opr : AS`UnaryOp | <MAPINVERSE>
// ==> ()
void StackEval::EvalUNOP(const Int & opr)
{
  TYPE_SEM_VAL arg_v (POP());

  switch (opr.GetValue()) {
    case NUMPLUS:
    case NUMMINUS:
    case FLOOR:
    case NUMABS:         { PUSH(EXPR::EvalNumUnaryExpr(opr, arg_v)); break; } // PUSH
    case NOT:            { PUSH(EXPR::EvalLogUnaryExpr(opr, arg_v)); break; } // PUSH
    case SETCARD:
    case SETDISTRUNION:
    case SETDISTRINTERSECT:
    case SETPOWER:       { PUSH(EXPR::EvalSetUnaryExpr(opr, arg_v)); break; } // PUSH
    case SEQHEAD:
    case SEQTAIL:
    case SEQLEN:
    case SEQELEMS:
    case SEQINDICES:
    case SEQDISTRCONC:
    case SEQREVERSE:     { PUSH(EXPR::EvalSeqUnaryExpr(opr, arg_v)); break; } // PUSH
    case MAPDOM:
    case MAPRNG:
    case MAPDISTRMERGE:
    case MAPINVERSE:     { PUSH(EXPR::EvalMapUnaryExpr(opr, arg_v)); break; } // PUSH
    default: { break; }
  }
}

// EvalBINOP
// opr : AS`BinaryOp
// ==> ()
void StackEval::EvalBINOP(const Int & opr)
{
  TYPE_SEM_VAL right_v (POP());
  TYPE_SEM_VAL left_v (POP());

  switch (opr.GetValue()) {
    case EQ:
    case NE:           { PUSH(EXPR::EvalEqualityExpr(left_v, opr, right_v)); break; }
    case EQUIV:        { PUSH(EXPR::EvalEqNeBinaryExpr(left_v, opr, right_v)); break; }
    case NUMPLUS:
    case NUMMINUS:
    case NUMMULT:
    case NUMDIV:
    case NUMREM:
    case INTDIV:
    case NUMMOD:       { PUSH(EXPR::EvalNumBinaryExpr(left_v, opr, right_v)); break; }
    case NUMLT:
    case NUMLE:
    case NUMGT:
    case NUMGE:        { PUSH(EXPR::EvalOrderExpr(left_v, opr, right_v)); break; }
    case INSET:
    case NOTINSET:
    case SETUNION:
    case SETINTERSECT:
    case SETMINUS:
    case SUBSET:
    case PROPERSUBSET: { PUSH(EXPR::EvalSetBinaryExpr(left_v, opr, right_v)); break; }
    case SEQCONC:      { PUSH(EXPR::EvalSeqBinaryExpr(left_v, opr, right_v)); break; }
    case MAPMERGE:
    case MAPDOMRESTTO:
    case MAPDOMRESTBY:
    case MAPRNGRESTTO:
    case MAPRNGRESTBY: { PUSH(EXPR::EvalMapBinaryExpr(left_v, opr, right_v)); break; }
    case COMPOSE:      { PUSH(EXPR::EvalComposeExpr(left_v, right_v)); break; }
    case NUMEXP:       { PUSH(EXPR::EvalIterateExpr(left_v, right_v)); break; }
    default: { break; }
  }
}

// ExeAPPENDSEQ
// ==> ()
void StackEval::ExeAPPENDSEQ()
{
  TYPE_SEM_VAL val (POP());
  TYPE_SEM_VAL seqval (HEAD());
  SETHEAD(mk_SEM_SEQ(Sequence(seqval.GetSequence(pos_SEM_SEQ_v)).ImpAppend(val)));
}

// ExeADDSET
// ==> ()
void StackEval::ExeADDSET()
{
  TYPE_SEM_VAL val (POP());
  TYPE_SEM_SET setval (HEAD());
  SETHEAD(mk_SEM_SET(Set(setval.GetSet(pos_SEM_SET_v)).Insert(val)));
}

// ExeAPPENDMAP
// ==> ()
void StackEval::ExeAPPENDMAP()
{
  SEQ<TYPE_SEM_VAL> val_l (POPN(2));
//  TYPE_SEM_VAL mapval (POP());
  TYPE_SEM_VAL mapval (HEAD());

  MAP<TYPE_SEM_VAL, TYPE_SEM_VAL> v (mapval.GetMap(pos_SEM_MAP_v));
  const TYPE_SEM_VAL & domval (val_l[1]);
  const TYPE_SEM_VAL & rngval (val_l[2]);
  if (v.DomExists(domval) && (v[domval] != rngval))
    RTERR::Error(L"ExeAPPENDMAP", RTERR_DUPLICATES_NOT_EQUAL, mk_SEM_MAP(v), Nil(), Sequence());
  else {
    v.Insert(domval, rngval);
  }
  //PUSH(mk_SEM_MAP(v));
  SETHEAD(mk_SEM_MAP(v));
}

// ExeSELSEQELEM
// ==> ()
void StackEval::ExeSELSEQELEM()
{
  SEQ<TYPE_SEM_VAL> seqval (HEAD());   // SEM`VAL
  TYPE_SEM_VAL val (seqval.Hd());
  SETHEAD(seqval.ImpTl());
  PUSH(val);
}

// ExeSETRNG
// ==> ()
void StackEval::ExeSETRNG()
{
  SEQ<TYPE_SEM_VAL> val_l (POPN(2)); // [lbval,ubval]
  PUSH(EXPR::EvalSetRangeExpr(val_l[1], val_l[2])); // lbval, ubval
}

// ExeSUBSEQ
// ==> ()
void StackEval::ExeSUBSEQ()
{
  SEQ<TYPE_SEM_VAL> val_l (POPN(3)); // [seqval,lbval,ubval]
  PUSH(EXPR::EvalSubSequenceExpr(val_l[1], val_l[2], val_l[3])); // seqval, lbval, ubval
}

// ExeRECPATCONS
// length : nat
// ==> ()
void StackEval::ExeRECPATCONS(const TYPE_AS_Name & tag, const Int & length)
{
  SEQ<TYPE_STKM_Pattern> fields (POPN(length));
  PUSH(TYPE_STKM_RecordPattern().Init(tag, fields));
}

// ExeTUPPATCONS
// length : nat
// ==> ()
void StackEval::ExeTUPPATCONS(const Int & length)
{
  SEQ<TYPE_STKM_Pattern> fields (POPN(length));
  PUSH(TYPE_STKM_TuplePattern().Init(fields));
}

// ExeRECCONS
// tag : AS`Name
// length : nat
// ==> ()
void StackEval::ExeRECCONS(const TYPE_AS_Name & tag, const Int & length)
{
  SEQ<TYPE_SEM_VAL> val_l (POPN(length));
  PUSH(EXPR::EvalRecordConstructorExpr(tag, val_l));
}

// ExeRECMOD
// fid : seq1 of AS`Name
// ==> ()
void StackEval::ExeRECMOD(const SEQ<TYPE_AS_Name> & fid_l)
{
  SEQ<TYPE_SEM_VAL> val_l (POPN(fid_l.Length()));
  TYPE_SEM_VAL rec (POP());
  PUSH(EXPR::EvalRecordModifierExpr(rec, fid_l, val_l));
}

// ExeFIELDSEL
// field : AS`Name
// ==> ()
void StackEval::ExeFIELDSEL(const TYPE_AS_Name & field)
{
  TYPE_SEM_VAL recval (POP()); // [recval]
  PUSH(EXPR::EvalFieldSelectExpr(recval, field)); // recval, field
}

// ExeISEMPTYSET
// n : nat
// ==> ()
void StackEval::ExeISEMPTYSET(const Int & n)
{
  const TYPE_STKM_EvalStackItem & elm (GETNTH(n));
  PUSH((elm.IsSet() && Set(elm).IsEmpty()) ? sem_true : sem_false);
}

// ExeISEMPTYSEQ
// n : nat
// ==> ()
void StackEval::ExeISEMPTYSEQ(const Int & n)
{
  const TYPE_STKM_EvalStackItem & elm (GETNTH(n));
  PUSH((elm.IsSequence() && Sequence(elm).IsEmpty()) ? sem_true : sem_false);
}

// ExeISNOTEMPTYSET
// n : nat
// ==> ()
void StackEval::ExeISNOTEMPTYSET(const Int & n)
{
  const TYPE_STKM_EvalStackItem & elm (GETNTH(n));
  PUSH((elm.IsSet() && Set(elm).IsEmpty()) ? sem_false : sem_true);
}

// ExeISNOTEMPTYSEQ
// n : nat
// ==> ()
void StackEval::ExeISNOTEMPTYSEQ(const Int & n)
{
  const TYPE_STKM_EvalStackItem & elm (GETNTH(n));
  PUSH((elm.IsSequence() && Sequence(elm).IsEmpty()) ? sem_false : sem_true);
}

// ExeTOKENVAL
// ==> ()
void StackEval::ExeTOKENVAL()
{
  //TYPE_SEM_VAL val (POP());
  TYPE_SEM_VAL val (HEAD());

  //PUSH(mk_SEM_TOKEN(val));
  SETHEAD(mk_SEM_TOKEN(val));
}

// UpdateTypeInfo
// tp : GLOBAL`Type
// usedclass : AS`Name
// -> GLOBAL`Type
TYPE_GLOBAL_Type StackEval::UpdateTypeInfo(const TYPE_GLOBAL_Type & tp, const TYPE_AS_Name & usedclass) const
{
  switch (tp.GetTag()) {
    case TAG_TYPE_AS_AllType:
    case TAG_TYPE_AS_BooleanType:
    case TAG_TYPE_AS_NumericType:
    case TAG_TYPE_AS_TokenType:
    case TAG_TYPE_AS_CharType:
    case TAG_TYPE_AS_VoidType:
    case TAG_TYPE_AS_QuoteType:
    case TAG_TYPE_AS_TypeVar:
    case TAG_TYPE_AS_CompositeType: {
      return tp;
    }
    case TAG_TYPE_AS_BracketedType: {
      return TYPE_AS_BracketedType().Init(UpdateTypeInfo(tp.GetRecord(pos_AS_BracketedType_tp), usedclass),
                                          tp.GetInt(pos_AS_BracketedType_cid));
    }
    case TAG_TYPE_AS_UnionType: {
      const SEQ<TYPE_AS_Type> & typeset (tp.GetSequence(pos_AS_UnionType_tps));
      SEQ<TYPE_AS_Type> new_ts;
      size_t len = typeset.Length();
      for (size_t i = 1; i <= len; i++) {
        new_ts.ImpAppend(UpdateTypeInfo(typeset[i], usedclass));
      }
      return TYPE_AS_UnionType().Init(new_ts, tp.GetInt(pos_AS_UnionType_cid));
    }
    case TAG_TYPE_AS_ProductType: {
      const SEQ<TYPE_AS_Type> & typeseq (tp.GetSequence(pos_AS_ProductType_tps));
      SEQ<TYPE_AS_Type> new_ts;
      size_t len = typeseq.Length();
      for (size_t i = 1; i <= len; i++) {
        new_ts.ImpAppend(UpdateTypeInfo(typeseq[i], usedclass));
      }
      return TYPE_AS_ProductType().Init(new_ts, tp.GetInt(pos_AS_ProductType_cid));
    }
    case TAG_TYPE_AS_OptionalType: {
      return TYPE_AS_OptionalType().Init(UpdateTypeInfo(tp.GetRecord(pos_AS_OptionalType_tp), usedclass),
                                         tp.GetInt(pos_AS_OptionalType_cid));
    }
    case TAG_TYPE_AS_Set0Type: {
      return TYPE_AS_Set0Type().Init(UpdateTypeInfo(tp.GetRecord(pos_AS_Set0Type_elemtp), usedclass),
                                    tp.GetInt(pos_AS_Set0Type_cid));
    }
    case TAG_TYPE_AS_Set1Type: {
      return TYPE_AS_Set1Type().Init(UpdateTypeInfo(tp.GetRecord(pos_AS_Set1Type_elemtp), usedclass),
                                    tp.GetInt(pos_AS_Set1Type_cid));
    }
    case TAG_TYPE_AS_Seq0Type: {
      return TYPE_AS_Seq0Type().Init(UpdateTypeInfo(tp.GetRecord(pos_AS_Seq0Type_elemtp), usedclass),
                                     tp.GetInt(pos_AS_Seq0Type_cid));
    }
    case TAG_TYPE_AS_Seq1Type: {
      return TYPE_AS_Seq1Type().Init(UpdateTypeInfo(tp.GetRecord(pos_AS_Seq1Type_elemtp),usedclass),
                                     tp.GetInt(pos_AS_Seq1Type_cid));
    }
    case TAG_TYPE_AS_GeneralMap0Type: {
      return TYPE_AS_GeneralMap0Type().Init(UpdateTypeInfo(tp.GetRecord(pos_AS_GeneralMap0Type_mapdom), usedclass),
                                           UpdateTypeInfo(tp.GetRecord(pos_AS_GeneralMap0Type_maprng), usedclass),
                                           tp.GetInt(pos_AS_GeneralMap0Type_cid));
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      return TYPE_AS_GeneralMap1Type().Init(UpdateTypeInfo(tp.GetRecord(pos_AS_GeneralMap1Type_mapdom), usedclass),
                                           UpdateTypeInfo(tp.GetRecord(pos_AS_GeneralMap1Type_maprng), usedclass),
                                           tp.GetInt(pos_AS_GeneralMap1Type_cid));
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      return TYPE_AS_InjectiveMap0Type().Init(UpdateTypeInfo(tp.GetRecord(pos_AS_InjectiveMap0Type_mapdom), usedclass),
                                             UpdateTypeInfo(tp.GetRecord(pos_AS_InjectiveMap0Type_maprng), usedclass),
                                             tp.GetInt(pos_AS_InjectiveMap0Type_cid));
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      return TYPE_AS_InjectiveMap1Type().Init(UpdateTypeInfo(tp.GetRecord(pos_AS_InjectiveMap1Type_mapdom), usedclass),
                                             UpdateTypeInfo(tp.GetRecord(pos_AS_InjectiveMap1Type_maprng), usedclass),
                                             tp.GetInt(pos_AS_InjectiveMap1Type_cid));
    }
    case TAG_TYPE_AS_PartialFnType: {
      const SEQ<TYPE_AS_Type> & dt (tp.GetSequence(pos_AS_PartialFnType_fndom));
      SEQ<TYPE_AS_Type> new_dt;
      size_t len = dt.Length();
      for (size_t i = 1; i <= len; i++) {
        new_dt.ImpAppend(UpdateTypeInfo(dt[i], usedclass));
      }
      return TYPE_AS_PartialFnType().Init(new_dt,
                                          UpdateTypeInfo(tp.GetRecord(pos_AS_PartialFnType_fnrng), usedclass),
                                          tp.GetInt(pos_AS_PartialFnType_cid));
    }
    case TAG_TYPE_AS_TotalFnType: {
      const SEQ<TYPE_AS_Type> & dt (tp.GetSequence(pos_AS_TotalFnType_fndom));
      SEQ<TYPE_AS_Type> new_dt;
      size_t len = dt.Length();
      for (size_t i = 1; i<= len; i++) {
        new_dt.ImpAppend(UpdateTypeInfo(dt[i], usedclass));
      }
      return TYPE_AS_TotalFnType().Init(new_dt,
                                        UpdateTypeInfo(tp.GetRecord(pos_AS_TotalFnType_fnrng), usedclass),
                                        tp.GetInt(pos_AS_TotalFnType_cid));
    }
    case TAG_TYPE_AS_TypeName: {
      const TYPE_AS_Name & nm (tp.GetRecord(pos_AS_TypeName_name));
      if (nm.GetSequence(pos_AS_Name_ids).Length() == 1) {
#ifdef VDMSL
          return TYPE_AS_TypeName().Init(AUX::ConstructDoubleName(usedclass, nm), tp.GetInt(pos_AS_TypeName_cid));
#endif // VDMSL
#ifdef VDMPP
        if (!theState().IsAClass(nm)) {
          return TYPE_AS_TypeName().Init(AUX::ConstructDoubleName(usedclass, nm), tp.GetInt(pos_AS_TypeName_cid));
        }
        else
          return tp;
#endif // VDMPP
      } else {
        return tp;
      }
    }
    case TAG_TYPE_AS_OpType : {
      const SEQ<TYPE_AS_Type> & dt (tp.GetSequence(pos_AS_OpType_opdom));
      SEQ<TYPE_AS_Type> new_dt;
      size_t len = dt.Length();
      for (size_t i = 1; i <= len; i++ ) {
        new_dt.ImpAppend(UpdateTypeInfo(dt[i], usedclass));
      }
      return TYPE_AS_OpType().Init(new_dt,
                                   UpdateTypeInfo(tp.GetRecord(pos_AS_OpType_oprng), usedclass),
                                   tp.GetInt(pos_AS_OpType_cid));
    }
#ifdef VDMPP
    case TAG_TYPE_GLOBAL_ObjRefType: {
      return tp;
    }
#endif // VDMPP
    default: {
      vdm_iplog << L"unknown type: " << tp.GetTag() << endl;
      return tp;
    }
  }
}

// ExePOLYINST
// inst : seq of AS`Type
// nm : AS`name
// ==> ()
void StackEval::ExePOLYINST(const SEQ<TYPE_AS_Type> & inst, const TYPE_AS_Name & nm)
{
  SEQ<TYPE_AS_Type> inst_q;
  size_t len_inst = inst.Length();
  for (size_t idx = 1; idx <= len_inst; idx++)
    inst_q.ImpAppend(UpdateTypeInfo(inst[idx], nm)); 

  TYPE_SEM_VAL polyfn (POP());

  switch(polyfn.GetTag()) {
    case TAG_TYPE_SEM_ExplPOLY: {
      PUSH(theState().EvalFctTypeInstExpr(polyfn, inst_q));
      break;
    }
#ifdef VDMPP
    case TAG_TYPE_SEM_OverOPFN: {
      PUSH(theState().EvalOverFctTypeInstExpr(polyfn, inst_q));
      break;
    }
#endif // VDMPP
// the case of instantiated function called lately by curry
// instantiation is done at first call
    case TAG_TYPE_SEM_CompExplFN: {
      const SEQ<TYPE_SEM_ExplFN> & fn_l (polyfn.GetSequence(pos_SEM_CompExplFN_fl));
      bool forall = true;
      size_t len_fn_l = fn_l.Length();
      for (size_t i = 1; i <= len_fn_l && forall; i++)
      {
        const TYPE_SEM_ExplFN & fn (fn_l[i]);
        Set is (fn.GetMap(pos_SEM_ExplFN_tm).Dom());
        // check all TypeVar is already instantiated
        forall = inst.Elems().ImpDiff(is).IsEmpty(); 
      }
      if (forall)
        PUSH(polyfn);
      else
        RTERR::Error (L"ExePOLYINST", RTERR_NO_POLY_FUNC, Nil(), Nil(), Sequence());
      break;
    }
    default: {
      RTERR::Error (L"ExePOLYINST", RTERR_NO_POLY_FUNC, Nil(), Nil(), Sequence());
    }
  }
}

// ExeTUPSEL
// index : real
// ==> ()
void StackEval::ExeTUPSEL(const Real & index)
{
  TYPE_SEM_VAL tupval (POP());

  if (tupval.Is(TAG_TYPE_SEM_TUPLE))
  {
    const SEQ<TYPE_SEM_VAL> & v (tupval.GetSequence(pos_SEM_TUPLE_v));
    // Check that index is a nat, and that the index is within the limits of the tuple.
    if (v.OutOfRange(index))
      RTERR::Error(L"ExeTUPSEL", RTERR_TUPLE_OUTSIDE_INDEX, tupval, Nil(), Sequence());
    else
      PUSH(v[index.GetIntValue()]);
  }
  else
    RTERR::Error(L"ExeTUPSEL", RTERR_TUPLE_EXPECTED, tupval, Nil(), Sequence());
}

// ExeTYPEJUDGE
// tp : AS`Type
// ==> ()
void StackEval::ExeTYPEJUDGE(const TYPE_AS_Type & tp)
{
  TYPE_STKM_EvalStackItem val (POP());

  theState().SetTypeJudgement();
  PUSH(theState().RealSubType(val, tp, true) ? sem_true : sem_false);
  theState().UnsetTypeJudgement();
}

// ExeTUPCONS
// length : nat
// ==> ()
void StackEval::ExeTUPCONS(const Int & length)
{
  SEQ<TYPE_SEM_VAL> val_l (POPN(length));
  PUSH(TYPE_SEM_TUPLE().Init(val_l));
}

// ExeSETCONS
// length : nat
// ==> ()
void StackEval::ExeSETCONS(const Int & length)
{
  SEQ<TYPE_SEM_VAL> val_l (POPN(length));
  PUSH(mk_SEM_SET(val_l.Elems()));
}

// ExeSEQCONS
// length : nat
// ==> ()
void StackEval::ExeSEQCONS(const Int & length)
{
  SEQ<TYPE_SEM_VAL> val_l (POPN(length));
  PUSH(mk_SEM_SEQ(val_l));
}

// ExeMAPCONS
// length : nat
// ==> ()
void StackEval::ExeMAPCONS(const Int & length)
{
  SEQ<TYPE_SEM_VAL> val_l (POPN(length.GetValue() * 2));

  MAP<TYPE_SEM_VAL, TYPE_SEM_VAL> v;
  for (int idx = 1; idx <= length; idx++)
  {
    TYPE_SEM_VAL domval (val_l[idx*2-1]);
    TYPE_SEM_VAL rngval (val_l[idx*2]);
    if (v.DomExists(domval) && (v[domval] != rngval))
      RTERR::Error(L"ExeAPPENDMAP", RTERR_DUPLICATES_NOT_EQUAL, mk_SEM_MAP(v), Nil(), Sequence());
    else {
      v.Insert(domval, rngval);
     }
  }
  PUSH(mk_SEM_MAP(v));
}

// ExeSEQCOMPBIND
// isSet : bool
// ==> ()
void StackEval::ExeSEQCOMPBIND(const Bool & isSeq)
{
  TYPE_SEM_VAL val_v (POP());
//  TYPE_SEM_VAL pat (POP());
  TYPE_SEM_VAL pat (HEAD());

  if (isSeq) {
    if (val_v.Is(TAG_TYPE_SEM_SEQ)) {
      //PUSH(pat);
      PUSH(val_v.GetSequence(pos_SEM_SEQ_v));
    }
    else {
      RTERR::Error(L"ExeSEQCOMPBIND", RTERR_SEQ_EXPECTED, val_v, Nil(), Sequence());
    }
  }
  else {
    if (val_v.Is(TAG_TYPE_SEM_SET)) {
      if (!pat.Is(TAG_TYPE_STKM_PatternName)) {
        RTERR::Error(L"ExeSEQCOMPBIND", RTERR_PAT_NAME_IN_SEQCOMP, Nil(), Nil(), Sequence());
      }
      else {
        SET<TYPE_SEM_VAL> v (val_v.GetSet(pos_SEM_SET_v));
        bool forall = true;
        Generic elm;
        for (bool bb = v.First(elm); bb && forall; bb = v.Next(elm)) {
          forall = (elm.Is(TAG_TYPE_SEM_NUM) ? Record(elm).GetReal(pos_SEM_NUM_v).IsInt() : false);
        }
        if (forall) {
          //PUSH(pat);
          PUSH(AUX::SetToSeq(v));
        }
        else {
          RTERR::Error(L"ExeSEQCOMPBIND", RTERR_NUMERIC_SET, val_v, Nil(), Sequence());
        }
      }
    }
    else {
      RTERR::Error(L"ExeSEQCOMPBIND", RTERR_SET_EXPECTED, val_v, Nil(), Sequence());
    }
  }
}

// ExeSEQFOR
// ==> ()
void StackEval::ExeSEQFOR()
{
  TYPE_SEM_VAL seqval (POP());   // SEM`VAL

  if (!seqval.Is(TAG_TYPE_SEM_SEQ)) {
    RTERR::Error(L"ExeSELSEQELEM", RTERR_SEQ_EXPECTED, Nil(), Nil(), Sequence());
  }
  else {
    PUSH(seqval.GetSequence(pos_SEM_SEQ_v));
  }
}

// NameInPattern
// pat : STKM`Pattern
// ==> set of AS`Name
SET<TYPE_AS_Name> StackEval::NameInPattern(const TYPE_STKM_Pattern & pat)
{
  switch(pat.GetTag()) {
    case TAG_TYPE_STKM_PatternName: {
      SET<TYPE_AS_Name> res;
      const Generic & id (pat.GetField(pos_STKM_PatternName_nm));
      if (!id.IsNil())
        res.Insert(id);
      return res;
    }
    case TAG_TYPE_STKM_MatchVal: {
      return SET<TYPE_AS_Name>();
    }
    case TAG_TYPE_STKM_SetEnumPattern: {
      const SEQ<TYPE_STKM_Pattern> & els_l (pat.GetSequence(pos_STKM_SetEnumPattern_els));
      SET<TYPE_AS_Name> res;
      size_t len_els_l = els_l.Length();
      for (size_t index = 1; index <= len_els_l; index++)
        res.ImpUnion(NameInPattern(els_l[index]));
      return res;
    }
    case TAG_TYPE_STKM_SetUnionPattern: {
      SET<TYPE_AS_Name> res (NameInPattern(pat.GetRecord(pos_STKM_SetUnionPattern_lp)));
      res.ImpUnion(NameInPattern(pat.GetRecord(pos_STKM_SetUnionPattern_rp)));
      return res;
    }
    case TAG_TYPE_STKM_SeqEnumPattern: {
      const SEQ<TYPE_STKM_Pattern> & els_l (pat.GetSequence(pos_STKM_SeqEnumPattern_els));
      SET<TYPE_AS_Name> res;
      size_t len_els_l = els_l.Length();
      for (size_t index = 1; index <= len_els_l; index++)
        res.ImpUnion(NameInPattern(els_l[index]));
      return res;
    }
    case TAG_TYPE_STKM_SeqConcPattern: {
      SET<TYPE_AS_Name> res (NameInPattern(pat.GetRecord(pos_STKM_SeqConcPattern_lp)));
      res.ImpUnion(NameInPattern(pat.GetRecord(pos_STKM_SeqConcPattern_rp)));
      return res;
    }
    case TAG_TYPE_STKM_MapEnumPattern: {
      const SEQ<TYPE_STKM_MapletPattern> & mls_l (pat.GetSequence(pos_STKM_MapEnumPattern_mls));
      SET<TYPE_AS_Name> res;
      size_t len_mls_l = mls_l.Length();
      for (size_t index = 1; index <= len_mls_l; index++) {
        const TYPE_STKM_MapletPattern & mp (mls_l[index]);
        res.ImpUnion(NameInPattern(mp.GetRecord(pos_STKM_MapletPattern_dp)));
        res.ImpUnion(NameInPattern(mp.GetRecord(pos_STKM_MapletPattern_rp)));
      }
      return res;
    }
    case TAG_TYPE_STKM_MapMergePattern: {
      SET<TYPE_AS_Name> res (NameInPattern(pat.GetRecord(pos_STKM_MapMergePattern_lp)));
      res.ImpUnion(NameInPattern(pat.GetRecord(pos_STKM_MapMergePattern_rp)));
      return res;
    }
    case TAG_TYPE_STKM_RecordPattern: {
      const SEQ<TYPE_STKM_Pattern> & field_l (pat.GetSequence(pos_STKM_RecordPattern_fields));
      SET<TYPE_AS_Name> res;
      size_t len_field_l = field_l.Length();
      for (size_t index = 1; index <= len_field_l; index++) {
        res.ImpUnion(NameInPattern(field_l[index]));
      }
      return res;
    }
    case TAG_TYPE_STKM_TuplePattern: {
      const SEQ<TYPE_STKM_Pattern> & field_l (pat.GetSequence(pos_STKM_TuplePattern_fields));
      SET<TYPE_AS_Name> res;
      size_t len_field_l = field_l.Length();
      for (size_t index = 1; index <= len_field_l; index++) {
        res.ImpUnion(NameInPattern(field_l[index]));
      }
      return res;
    }
#ifdef VDMPP
    case TAG_TYPE_STKM_ObjectPattern: {
      const SEQ<TYPE_STKM_FieldPattern> & field_l (pat.GetSequence(pos_STKM_ObjectPattern_fields));
      SET<TYPE_AS_Name> res;
      size_t len_field_l = field_l.Length();
      for (size_t index = 1; index <= len_field_l; index++) {
        res.ImpUnion(NameInPattern(field_l[index].GetRecord(pos_AS_FieldPattern_pat)));
      }
      return res;
    }
#endif // VDMPP
    default: {
      RTERR::Error(L"NameInPattern", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
    }
  }
  return SET<TYPE_AS_Name>(); // dummy
}

// ExeSEQMAPOVER
// ==> ()
void StackEval::ExeSEQMAPOVER()
{
  SEQ<TYPE_SEM_VAL> val_l (POPN(2)); // [seqmap_v,map_v]

  PUSH(EXPR::EvalSeqModifyMapOverrideExpr(val_l[1], val_l[2])); // seqmap_v, map_v
}

// ExeSETENUM
// length : nat
// ==> ()
void StackEval::ExeSETENUM(const Int & length)
{
  SEQ<TYPE_STKM_Pattern> fields (POPN(length));

  SET<TYPE_STKM_Pattern> field_s;
  SEQ<TYPE_STKM_Pattern> fields_q;
  size_t len_fields = fields.Length();
  for (size_t idx = 1; idx <= len_fields; idx++) {
    const TYPE_STKM_Pattern & p (fields[idx]);
    if (!field_s.InSet(p)) {
      fields_q.ImpAppend(p);
      field_s.Insert(p);
    }
  }

  PUSH(TYPE_STKM_SetEnumPattern().Init(fields_q));
}

// ExeSEQENUM
// length : nat
// ==> ()
void StackEval::ExeSEQENUM(const Int & length)
{
  SEQ<TYPE_STKM_Pattern> fields (POPN(length));

  PUSH(TYPE_STKM_SeqEnumPattern().Init(fields));
}

// ExeMAPLET
// ==> ()
void StackEval::ExeMAPLET()
{
  SEQ<TYPE_STKM_Pattern> pat_l (POPN(2)); // [dompat,rngpat]

  PUSH(TYPE_STKM_MapletPattern().Init(pat_l[1], pat_l[2]));
}

// ExeMAPENUM
// length : nat
// ==> ()
void StackEval::ExeMAPENUM(const Int & length)
{
  SEQ<TYPE_STKM_MapletPattern> mls (POPN(length));

  SET<TYPE_STKM_MapletPattern> mp_s;
  SEQ<TYPE_STKM_MapletPattern> mls_q;
  size_t len_mls = mls.Length();
  for (size_t idx = 1; idx <= len_mls; idx++) {
    const TYPE_STKM_MapletPattern & p (mls[idx]);
    if (!mp_s.InSet(p)) {
      mls_q.ImpAppend(p);
      mp_s.Insert(p);
    }
  }

  PUSH(TYPE_STKM_MapEnumPattern().Init(mls_q));
}

// ExeSetUnion
// ==> ()
void StackEval::ExeSetUnion()
{
  SEQ<TYPE_STKM_Pattern> pat_l (POPN(2)); // [leftpat,rightpat]
  PUSH(TYPE_STKM_SetUnionPattern().Init(pat_l[1], pat_l[2])); // leftpat, rightpat
}

// ExeSeqConc
// ==> ()
void StackEval::ExeSeqConc()
{
  SEQ<TYPE_STKM_Pattern> pat_l (POPN(2)); // [leftpat,rightpat]
  const TYPE_STKM_Pattern & leftpat (pat_l[1]);
  const TYPE_STKM_Pattern & rightpat (pat_l[2]);
  if (rightpat.Is(TAG_TYPE_STKM_SeqEnumPattern)) {
    switch (leftpat.GetTag()) {
      case TAG_TYPE_STKM_SeqEnumPattern: {
        SEQ<TYPE_STKM_Pattern> els (leftpat.GetSequence(pos_STKM_SeqEnumPattern_els));
        els.ImpConc(rightpat.GetSequence(pos_STKM_SeqEnumPattern_els));
        PUSH(TYPE_STKM_SeqEnumPattern().Init(els));
        break;
      }
      case TAG_TYPE_STKM_SeqConcPattern: {
        const TYPE_STKM_Pattern & rp (leftpat.GetRecord(pos_STKM_SeqConcPattern_rp));
        if (rp.Is(TAG_TYPE_STKM_SeqEnumPattern)) {
          SEQ<TYPE_STKM_Pattern> els (rp.GetSequence(pos_STKM_SeqEnumPattern_els));
          els.ImpConc(rightpat.GetSequence(pos_STKM_SeqEnumPattern_els));
          PUSH(TYPE_STKM_SeqConcPattern().Init(leftpat.GetRecord(pos_STKM_SeqConcPattern_lp),
                                               TYPE_STKM_SeqEnumPattern().Init(els)));
        }
        else {
          PUSH(TYPE_STKM_SeqConcPattern().Init(leftpat, rightpat));
        }
        break;
      }
      default: {
        PUSH(TYPE_STKM_SeqConcPattern().Init(leftpat, rightpat));
        break;
      }
    }
  }
  else {
    PUSH(TYPE_STKM_SeqConcPattern().Init(leftpat, rightpat));
  }
}

// ExeMapMerge
// ==> ()
void StackEval::ExeMapMerge()
{
  SEQ<TYPE_STKM_Pattern> pat_l (POPN(2)); // [leftpat,rightpat]
  PUSH(TYPE_STKM_MapMergePattern().Init(pat_l[1], pat_l[2])); // leftpat, rightpat
}

#ifdef VDMPP
// ExeOBJECTPAT
// cls : AS`Name
// length : nat
void StackEval::ExeOBJECTPAT(const TYPE_AS_Name & cls, const Int & length)
{
  SEQ<TYPE_STKM_FieldPattern> fields (POPN(length));
  PUSH(TYPE_STKM_ObjectPattern().Init(cls, fields)); // sd, nm
}

// ExeFIELDPAT
// nm : AS`Name
void StackEval::ExeFIELDPAT(const TYPE_AS_Name & nm)
{
  TYPE_STKM_Pattern pat (POP());
  PUSH(TYPE_STKM_FieldPattern().Init(nm, pat)); // sd, nm
}
#endif // VDMPP

// ExeFREF
// nm : AS`Name
// ==> ()
void StackEval::ExeFREF(const TYPE_AS_Name & nm)
{
  TYPE_STKM_StateDesignator sd (POP());
  PUSH(TYPE_STKM_FieldRef().Init(sd, nm)); // sd, nm
}

// ExeMOSREF
// ==> ()
void StackEval::ExeMOSREF()
{
  SEQ<TYPE_STKM_EvalStackItem> items (POPN(2)); // [sd,val]
  PUSH(TYPE_STKM_MapOrSeqRef().Init(items[1], items[2])); // sd, val
}

// InvOK
// ==> ()
void StackEval::InvOK() /*notconst*/
{
  if (Settings.INV() && !theState().CheckGlobInv()) {
    if (theStackMachine().RuntimeErrorExceptionOccurred()) {
      theStackMachine().GotoTrapHandler(theStackMachine().RuntimeErrorVal());
    }
    else
#ifdef VDMSL
    RTERR::Error(L"InvOK", RTERR_STATE_INVARIANT_ERROR, Nil(), Nil(), Sequence());
#endif //VDMSL
#ifdef VDMPP
    RTERR::Error(L"InvOK", RTERR_INST_INV_BROKEN, Nil(), Nil(), Sequence());
#endif //VDMPP
  }
}

// ExeATOMIC
// no : nat1
// ==> ()
void StackEval::ExeATOMIC(const Int & no)
{
  int len = 2 * no.GetValue();
  SEQ<TYPE_STKM_EvalStackItem> assignl (POPN(len)); // Pop all data 
  // [ e1, sd1, e2, sd2, ...... eno, sdno ]
  for (int i = 1; i <= len; i++)
  {
    const TYPE_SEM_VAL & rhs (assignl[i]);
    if (rhs.Is(TAG_TYPE_SEM_CONT))
    {
      RTERR::Error(L"ExeATOMIC", RTERR_OP_RETURNED_CONT, rhs, Nil(), Sequence());
      return;
    }
    const TYPE_STKM_StateDesignator & lhs (assignl[++i]);
    theState().EvalStateDesignator(lhs, rhs);
  }
  PUSH(sem_cont);
  InvOK();
}

// ExeMACRO
// tp : AS`MacroTp
// cid : CI`ContextId
void StackEval::ExeMACRO(const Int & tp, const TYPE_CI_ContextId & cid)
{
  switch (tp.GetValue()) {
    case MACRO_FILE: {
      Sequence flnm (L"unknown");
      if (cid != NilContextId)
      {
        Tuple t (ContextInfo::SplitCid(cid));
        if (t.GetIntValue(1) >= 10)
        {
          Tuple gflcp (GetCI().GetFileLineColPos(cid));
          flnm = (gflcp.GetField(1).IsNil() ? Sequence(L"unknown") : gflcp.GetSequence(1));
        }
      }
      PUSH(EvalState::M42Sem(flnm, NULL)); 
      break;
    }
    case MACRO_LINE: {
      Int lno (-1);
      if (cid != NilContextId)
      {
        Tuple t (ContextInfo::SplitCid(cid));
        if (t.GetIntValue(1) >= 10)
        {
          Tuple gflcp (GetCI().GetFileLineColPos(cid));
          lno = (gflcp.GetField(1).IsNil() ? Int(-1) : gflcp.GetInt(2));
        }
      }
      PUSH(EvalState::M42Sem(lno, NULL)); 
      break;
    }
    case MACRO_COLUMN: {
      Int cno (-1);
      if (cid != NilContextId)
      {
        Tuple t (ContextInfo::SplitCid(cid));
        if (t.GetIntValue(1) >= 10)
        {
          Tuple gflcp (GetCI().GetFileLineColPos(cid));
          cno = (gflcp.GetField(1).IsNil() ? Int(-1) : gflcp.GetInt(3));
        }
      }
      PUSH(EvalState::M42Sem(cno, NULL)); 
      break;
    }
    case MACRO_CLMOD: {
      Sequence clmodnm (L"unknown");
#ifdef VDMSL
      TYPE_AS_Name mod (CurrentModule());
      if (mod != this->void_name)
      {
        const TYPE_AS_Ids & ids (mod.GetSequence(pos_AS_Name_ids));
        clmodnm = ids[1];
      }
#endif // VDMSL
#ifdef VDMPP
      TYPE_GLOBAL_OrigCl cl (GetCurCl());
      if (!cl.Is(TAG_TYPE_GLOBAL_Start))
      {
        const TYPE_AS_Ids & ids (cl.GetSequence(pos_AS_Name_ids));
        clmodnm = ids[1];
      }
#endif // VDMPP
      PUSH(EvalState::M42Sem(clmodnm, NULL)); 
      break;
    }
    case MACRO_FNOP: {
      Sequence fnopnm (L"unknown");
      if (theStackMachine().CallStackLevel() > 0)
      {
        TYPE_STKM_CallStackItem csi (theStackMachine().GetCS(Int(1)));
        if (csi.GetIntValue(pos_STKM_CallStackItem_type) == CallStackItemType::CS_FNOP)
        {
          const Generic & nmOrDesc (csi.GetField(pos_STKM_CallStackItem_nmOrDesc));
          if (nmOrDesc.Is(TAG_TYPE_AS_Name))
          {
            TYPE_AS_Name nm (nmOrDesc);
            const TYPE_AS_Ids & ids (nm.GetSequence(pos_AS_Name_ids));
            switch (ids.Length()) {
              case 2: { fnopnm = ids[2]; break; }
              case 1: { fnopnm = ids[1]; break; }
            }
          }
        }
      }
      PUSH(EvalState::M42Sem(fnopnm, NULL)); 
      break;
    }
    default: {
      RTERR::Error(L"ExeMACRO", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
      break;
    }
  }
}

// ExeASSIGNSD
// ==> ()
void StackEval::ExeASSIGNSD()
{
  TYPE_STKM_StateDesignator lhs (POP());
  TYPE_SEM_VAL rhs (POP());
#ifdef VDMSL
  if (lhs.Is(TAG_TYPE_AS_Name) && theState().IsTheState(lhs, CurrentModule()))
  {
    if (rhs.Is(TAG_TYPE_DYNSEM_SEM_REC)) {
      TYPE_AS_Name tag (AUX::UniqueTagName (lhs));
      if (tag == rhs.GetRecord(pos_DYNSEM_SEM_SemRecord_tag)) {
        theState().SetTheState(lhs, CurrentModule(), rhs);
        PUSH(sem_cont);
        InvOK();
        return;
      }
    }
    RTERR::Error (L"SetTheState", RTERR_ILLEGAL_STATE_INIT, Nil(), Nil(), Sequence());
    return;
  }
#endif //VDMSL
  theState().EvalStateDesignator(lhs, rhs);
  if (!RuntimeErrorExceptionOccurred()) {
    PUSH(sem_cont);
    InvOK();
  }
}

// ExeSEQELEMMATCH
// index : nat
// ==> ()
void StackEval::ExeSEQELEMMATCH(const Int & index)
{
  int idx = index.GetValue();
  TYPE_STKM_Pattern pat (GETNTH(idx));
  SEQ<TYPE_SEM_VAL> val_l (GETNTH(idx - 1));
  BindPat(pat, val_l.Hd());
  SETNTH(idx - 1, val_l.ImpTl());
}

// ExeEmptyBlkEnv
// permis : SEM`Permission
// ==> ()
void StackEval::ExeEmptyBlkEnv(const TYPE_SEM_Permission & permis)
{
  PushEmptyBlkEnv(permis);
}

// ExeMatchAndBind
// ==> ()
// used by Def Expression/Statement and Let Expression/Statement
void StackEval::ExeMatchAndBind()
{
  SEQ<TYPE_STKM_EvalStackItem> items (POPN(2)); // [val,pat]

  BindPat(items[2], items[1]); // pat, val
}

// BindPat
// pat : STKM`Pattern
// val : SEM`VAL
// ==> ()
void StackEval::BindPat(const TYPE_STKM_Pattern & pat, const TYPE_SEM_VAL & val)
{
  SET<TYPE_SEM_BlkEnv> env_s (PAT::PatternMatch(pat, val)); // one

  if (env_s.IsEmpty()) {
    if (pat.Is(TAG_TYPE_STKM_PatternName) && !pat.GetField(pos_STKM_PatternName_tp).IsNil()) {
      TYPE_AS_Type tp (pat.GetRecord(pos_STKM_PatternName_tp));
      if (tp.Is(TAG_TYPE_AS_TypeVar)) {
        Generic el;
        if (theStackMachine().HdTypeInst().DomExists(tp, el))
        tp = el;
      }
      if (theState().RealSubType(val, tp, false)) {
        // in this case 
        vdm_iplog << L"Type invariant of " << GetStatSem().ASType2Ascii(tp);
        vdm_iplog << L" was broken by " << VAL2X::val2asc(val);
        vdm_iplog << L" in pattern binding." << endl;
      }
      const TYPE_AS_Name & nm (pat.GetRecord(pos_STKM_PatternName_nm));
      RTERR::ReportError(L"BindPat", RTERR_INCOMPATIBLE_BIND_TYPE, val, tp,
                         nm.GetInt(pos_AS_Name_cid), Sequence());
    }
    else {
      RTERR::Error(L"BindPat", RTERR_EMPTY_ENV_S, Nil(), Nil(), Sequence());
    }
  }
  else {
    AddToTopBlkEnv(env_s.GetElem());
  }
}

// ExeCOPYVAL
// ==> ()
void StackEval::ExeCOPYVAL()
{
  COPY();
}

// ExeDTC
// tp : AS`Type
// ==> ()
void StackEval::ExeDTC(const TYPE_AS_Type & tp)
{
  if (Settings.DTC())
  {
    const TYPE_SEM_VAL & val (HEAD());

    if (!theState().SubType(val, tp)) {
      RTERR::Error(L"ExeDTC", RTERR_TYPE_INCOMP, val, tp, Sequence());
    }
  }
}

// ExeDTCSET
// ==> ()
void StackEval::ExeDTCSET()
{
  TYPE_SEM_SET sv (POP());

  const SET<TYPE_SEM_VAL> & setval (sv.GetSet(pos_SEM_SET_v));

  if (Settings.DTC()) {
    const TYPE_STKM_EvalStackItem & val (HEAD());
    if (!setval.InSet(val)) {
      RTERR::Error(L"ExeDTCSET", RTERR_VALUE_NOT_IN_SETBIND, setval, Nil(), Sequence());
    }
  }
}

// ExeSIZE
// n : nat
// ==> ()
void StackEval::ExeSIZE(const Int & n)
{
  const Set & val (HEAD());

  PUSH((val.Card() == n.GetValue()) ? sem_true : sem_false);
}

// ExeTRYANYMATCH
// ==> ()
void StackEval::ExeTRYANYMATCH()
{
  TYPE_STKM_Pattern pat (POP());
  TYPE_SEM_VAL val (POP());

  if (pat.Is(TAG_TYPE_STKM_PatternName))
  {
    if (!pat.GetField(pos_STKM_PatternName_nm).IsNil() && pat.GetField(pos_STKM_PatternName_tp).IsNil())
    {
      const TYPE_AS_Name & name (pat.GetRecord(pos_STKM_PatternName_nm));
#ifdef VDMSL
      //Tuple igv (theState().IsGlobalVal(name));
      Tuple igv (theState().GetCachedGlobalVal(name, CurrentModule()));
      if( igv.GetBoolValue(1) )
#endif // VDMSL
#ifdef VDMPP
      Tuple iv (theState().CachedIsValue(name));
      if (iv.GetBoolValue(1))
#endif // VDMPP
      {
        vdm_log << L"Pattern \"" << ASTAUX::ASName2String(name) << L"\" will override \""
                << ASTAUX::ASName2String(name) << L"\" defined in values section." << endl;
//        vdm_log << L"If you want to refer \"" << ASTAUX::ASName2String(name) << L"\" defined in values section," << endl;
//        vdm_log << L"use \"(" << ASTAUX::ASName2String(name) << ")\" insted of \"" << ASTAUX::ASName2String(name) << "\"." << endl;
      }
    }
  }

  SET<TYPE_SEM_BlkEnv> env_s (PAT::PatternMatch(pat, val)); // one

  if (env_s.IsEmpty()) {
    PUSH(sem_false);
  }
  else {
    PUSH(env_s);
    PUSH(sem_true);
  }
}

// ExeMATCHVAL
// ==> ()
void StackEval::ExeMATCHVAL()
{
  TYPE_SEM_VAL val (POP());

  PUSH(TYPE_STKM_MatchVal().Init(val));
}

// ExeCLOSENV
// blkenvCt : SEM`BlkEnv
// bodym : map AS`Name to (AS`Expr | <NOTYETSPEC>)
// ==> ()
void StackEval::ExeCLOSENV (const TYPE_SEM_BlkEnv & blkenv, const Map & bodym)
{
  // closenv_m
  SET<TYPE_AS_Name> dom_bodym (bodym.Dom());
  MAP<TYPE_AS_Name, TYPE_SEM_BlkEnv> closenv_m;
  Generic id;
  for (bool bb = dom_bodym.First(id); bb; bb = dom_bodym.Next(id)) {
    if (bodym[id] != Int(NOTYETSPEC)) {
      closenv_m.Insert(id, Free::FreeMapToBlkEnv(Free::FreeInExpr(bodym[id], Set())));
    }
  }

  if (closenv_m.IsEmpty()) {
    AddToTopBlkEnv(blkenv);
  }
  else {
    // newenv
    MAP<TYPE_AS_Name,TYPE_SEM_ValTp> blkenv_id_m (blkenv.GetMap(pos_SEM_BlkEnv_id_um));
    SET<TYPE_AS_Name> dom_blkenv_id_m (blkenv_id_m.Dom());
    dom_blkenv_id_m.ImpIntersect(closenv_m.Dom());

    MAP<TYPE_AS_Name,TYPE_SEM_ValTp> id_m;
    for (bool cc = dom_blkenv_id_m.First(id); cc; cc = dom_blkenv_id_m.Next(id)) {
      const TYPE_SEM_ValTp & vt (blkenv_id_m[id]);
      TYPE_SEM_VAL val (SemRec::UpdateClosEnv(vt.GetRecord(pos_SEM_ValTp_val), closenv_m[id]));
      id_m.Insert(id, TYPE_SEM_ValTp().Init(val, vt.GetField(pos_SEM_ValTp_tp)));
    }
    AddToTopBlkEnv(TYPE_SEM_BlkEnv().Init(id_m, blkenv.GetField(pos_SEM_BlkEnv_perm)));
  }
}

// ExeUPDATECLOSENV
// expr : AS`Expr
// ==> ()
void StackEval::ExeUPDATECLOSENV(const TYPE_AS_Expr & expr)
{
/*
//  TYPE_SEM_VAL fnval (POP());
//
//  TYPE_SEM_BlkEnv blkenv (Free::FreeMapToBlkEnv(Free::FreeInExpr(expr, Set())));
//  PUSH(SemRec::UpdateClosEnv(fnval, blkenv));
*/
  TYPE_SEM_VAL fnval (HEAD());

  TYPE_SEM_BlkEnv blkenv (Free::FreeMapToBlkEnv(Free::FreeInExpr(expr, Set())));
  SETHEAD(SemRec::UpdateClosEnv(fnval, blkenv));
}

// ExeVERIFYINDEXARGS
// ==> ()
void StackEval::ExeVERIFYINDEXARGS()
{
  const TYPE_SEM_VAL & lb_v (GETNTH(1));
  const TYPE_SEM_VAL & ub_v (GETNTH(2));
  const TYPE_SEM_VAL & step_v (GETNTH(3));

  if (!lb_v.Is(TAG_TYPE_SEM_NUM)) {
    RTERR::Error(L"ExeVERIFYINDEXARGS", RTERR_LOWER_BOUND_NOT_A_NUMBER, lb_v, Nil(), Sequence());
  }
  else if (!ub_v.Is(TAG_TYPE_SEM_NUM)) {
    RTERR::Error(L"ExeVERIFYINDEXARGS", RTERR_UPPER_BOUND_NOT_A_NUMBER, ub_v, Nil(), Sequence());
  }
  else {
    if (!step_v.Is(TAG_TYPE_SEM_NUM)) {
      RTERR::Error(L"ExeVERIFYINDEXARGS", RTERR_STEP_NOT_A_NUMBER, step_v, Nil(), Sequence());
    }
    else if (step_v.GetReal(pos_SEM_NUM_v).IsZero()) {
      RTERR::Error(L"ExeVERIFYINDEXARGS", RTERR_STEP_INDEX_IS_ZERO, Nil(), Nil(), Sequence());
    }
  }
}

// ExeTESTCOUNTER
// ==> ()
void StackEval::ExeTESTCOUNTER()
{
  const TYPE_SEM_VAL & current_s (GETNTH(2));
  const TYPE_SEM_VAL & last_s (GETNTH(3));
  const TYPE_SEM_VAL & step_s (GETNTH(4));

  const Real & current (current_s.GetReal(pos_SEM_NUM_v));
  const Real & last (last_s.GetReal(pos_SEM_NUM_v));
  const Real & step (step_s.GetReal(pos_SEM_NUM_v));

  PUSH(step.GreaterThan(Real(0)) ? (current.LessEqual(last) ? sem_true : sem_false)
                                 : (current.GreaterEqual(last) ? sem_true : sem_false));
}

// ExeINCRCOUNTER
// ==> ()
void StackEval::ExeINCRCOUNTER()
{
/*
  TYPE_SEM_VAL topElm (POP());
  TYPE_SEM_VAL current (POP());
  const TYPE_SEM_VAL & step (GETNTH(2));

  Real newnum (current.GetReal(pos_SEM_NUM_v).Add(step.GetReal(pos_SEM_NUM_v)));

  PUSH(TYPE_SEM_NUM().Init(newnum));
  PUSH(topElm);
*/
//  TYPE_SEM_VAL topElm (POP());
  TYPE_SEM_VAL current (GETNTH(2));
  const TYPE_SEM_VAL & step (GETNTH(4));

  Real newnum (current.GetReal(pos_SEM_NUM_v).Add(step.GetReal(pos_SEM_NUM_v)));

  SETNTH(2, TYPE_SEM_NUM().Init(newnum));
//  PUSH(topElm);
}

// ExeNONDETSTMT
// ==> ()
void StackEval::ExeNONDETSTMT()
{
  TYPE_SEM_VAL stmtval (POP());
  SEQ<Int> reljumps (POP());

  if (!stmtval.Is(TAG_TYPE_SEM_CONT)) {
    PUSH(stmtval);
  }
  else if (!reljumps.IsEmpty()) {
    //INCRPC(Int(reljumps[1].GetValue() + 1));
    INCRPC(reljumps[1].GetValue() + 1);
    PUSH(reljumps.Tl());
  }
  else {
    PUSH(stmtval);
  }
}

// ExeRANDOM
// ==> ()
void StackEval::ExeRANDOM()
{
  if (Settings.Random() != -1) {
    SEQ<Int> indices (HEAD());
    SETHEAD(Permute(indices));
  }
}

// Permute
// indices_o : seq of nat
// -> seq of nat
SEQ<Int> StackEval::Permute(const SEQ<Int> & indices_o)
{
  SEQ<Int> indices (indices_o);
  SEQ<Int> res;
  while (!indices.IsEmpty()) {
    // draw a random number 0..m-1
    size_t index = this->per_rnd.get_random(indices.Length());

    // convert the index to Sequence indexes
    index++;
    res.ImpAppend(indices[index]);
    indices.RemElem(index);
  }
  return res;
}

#ifdef VDMPP
// ExeNEWOBJ
// name : AS`Name
// dlobject : [token]
// ==> ()
void StackEval::ExeNEWOBJ (const TYPE_AS_Name & nm, const Generic & dlobject)
{
  TYPE_AS_Name name (nm);
  Tuple ecn (theState().ExpandClassName(nm, GetCurCl(), Set()));
  if (ecn.GetBoolValue(1)) {
    name = ecn.GetRecord(2);
  }

  if (Settings.DTC() && theState().CheckIfAbstractClass(name)) {
    RTERR::Error(L"ExeNEWOBJ", RTERR_INST_ABS_CL,
                 Nil(), Nil(), Sequence().ImpAppend(AUX::SingleNameToString(name)));
  }
  else {
    if (!theState().IsClassInit(name)) {
      theState().InitClassName(name);
    }
    ExeNEWPOSABSOBJ(name, dlobject);
  }
}

// CreateNEWDLClass (not in spec)
// name : AS`Name
// dlobject : [DLObject]
// ==> [VDMContainer<DlClassInstanceHolder>]
Generic StackEval::CreateNEWDLClass(const TYPE_AS_Name & name, const Generic & dlobject)
{
  Generic dlclassp = Nil();
  if (theState().IsDLClass(name)) {
    dlclassp = theState().DLCreateNEW(name, dlobject); // [DlClassInstance(VDMContainer<DlClassInstanceHolder>)]
  } else {
    if (! dlobject.IsNil()) {
      RTERR::Error(L"CreateNEWDLClass", RTERR_CLASS_NOT_DLCLASS,
                   Nil(), Nil(), Sequence().ImpAppend(AUX::SingleNameToString(name)));
      return Nil();
    }
  }
  return dlclassp;
}

// ExeNEWPOSABSOBJ
// name : AS`Name
// dlobject : [DLObject]
// ==> ()
void StackEval::ExeNEWPOSABSOBJ (const TYPE_AS_Name & name, const Generic & dlobject)
{
  if (!theState().IsAClass(name)) {
    RTERR::Error(L"ExeNEWPOSABSOBJ", RTERR_CLNM_NOT_DEFINED, Nil(), Nil(),
                 Sequence().ImpAppend(AUX::SingleNameToString(name)));
  }
  else {
//    TYPE_SEM_InsStrct insstrct (theState().GetInstInitVal(name));
//    TYPE_SEM_OBJ tmp_obj (TYPE_SEM_OBJ().Init(name, insstrct, MAP<TYPE_AS_Name,TYPE_SEM_History>()));
//    TYPE_SEM_OBJ_uRef tmp_ref (MakeNewObj( tmp_obj, CreateNEWDLClass(name, dlobject) ));
    TYPE_SEM_OBJ_uRef tmp_ref (MakeNewObj(theState().GetCachedOBJ(name), CreateNEWDLClass(name, dlobject)));

    PUSH(tmp_ref);
    PushCurObj(tmp_ref, Nil(), Nil());
  }
}

// ExeINITCLASS
// nm : AS`Name
// initno : nat
// ==> ()
void StackEval::ExeINITCLASS (const TYPE_AS_Name & nm, const Int & initno)
{
  TYPE_AS_Name name (nm);
  Tuple ecn (theState().ExpandClassName(nm, GetPrevCl(), Set()));
  if (ecn.GetBoolValue(1)) {
    name = ecn.GetRecord(2);
  }

  Generic obj_ref (POP());
  SEQ<TYPE_SEM_VAL> const_vals (POPN(initno));

  if (obj_ref.Is(TAG_TYPE_SEM_OBJ_uRef)) {
    theState().SetNewArgs(obj_ref, const_vals);
  }

  PUSH(obj_ref);
  PUSH(const_vals);

  TYPE_STKM_DebugCmd prog (theState().LookUpConstructor(name, const_vals));

  PushCS(prog, SEQ<Char>(L"Running constructor for ").ImpConc(AUX::SingleNameToString(name)), Nil(),
         CallStackItemType::CS_INTERNAL);
}

// ExeBINDINSTVAR
// clnm : AS`Name
// nm : AS`Name
// ==> ()
void StackEval::ExeBINDINSTVAR (const TYPE_AS_Name & clnm, const TYPE_AS_Name & nm)
{
  TYPE_SEM_VAL val (POP());

// for trace
  if (val.Is(TAG_TYPE_SEM_OBJ_uRef)) {
    theState().SetBindName(val, nm);
  }

  TYPE_SEM_OBJ obj (GetCurObj());
  const Map & istrct (obj.GetMap(pos_SEM_OBJ_ins));
  const Map & local_inst (istrct[clnm]);
  if (local_inst.DomExists(nm)) {
    const Tuple & t (local_inst[nm]);
    TYPE_AS_Type tp (theState().GetInstVarsTp(clnm)[nm]);
    Tuple res (mk_(Bool(true), Bool(true), t.GetRecord(1), tp, clnm, t.GetField(3)));
    theState().SetInstanceVar(nm, val, res);
  }
  else
  {
    // TODO: regacy specification
    // the case of constructor is't locally defined
    theState().SetInstanceVar(nm, val, theState().IsInObjScope(nm, Nil()));
  }
}

// ExeNEWCOMPL
// checkinv : bool
// ==> ()
void StackEval::ExeNEWCOMPL(const Bool & checkinv)
{
  if (checkinv.GetValue() && Settings.DTC() && !theState().CheckInstanceInvariant()) {
    if (theStackMachine().RuntimeErrorExceptionOccurred()) {
      theStackMachine().GotoTrapHandler(theStackMachine().RuntimeErrorVal());
    }
    else {
      RTERR::Error(L"ExeNEWCOMPL", RTERR_INST_INV_BROKEN, Nil(), Nil(), Sequence());
    }
  }
  else {
    PopCurObj();
  }
}

// ExePUSHCLNMCUROBJ
// cl : AS`Name
// origcl : AS`Name
// ==> ()
void StackEval::ExePUSHCLNMCUROBJ (const TYPE_AS_Name & cl, const TYPE_AS_Name & origcl)
{
  PushClNmCurObj(cl, origcl);
}

// ExePOPCLNMCUROBJ
// ==> ()
void StackEval::ExePOPCLNMCUROBJ()
{
  PopClNmCurObj();
}

// ExeSELFEXPR
// ==> ()
void StackEval::ExeSELFEXPR()
{
  if(HasCurObjRef()) {
    PUSH(GetCurObjRef());
  }
  else {
    RTERR::Error(L"ExeSELFEXPR", RTERR_NOOBJECT, Nil(), Nil(), Sequence());
  }
}

// ExeISOFCLASS
// clnm : AS`Name
// ==> ()
void StackEval::ExeISOFCLASS (const TYPE_AS_Name & clnm)
{
  TYPE_SEM_VAL arg_v (POP());

  if (arg_v.Is(TAG_TYPE_SEM_OBJ_uRef)) {
    if (theState().IsAClass(clnm)) {
      const TYPE_AS_Name & objnm (arg_v.GetRecord(pos_SEM_OBJ_uRef_tp));
      PUSH(((objnm == clnm) || theState().IsSubClass(objnm, clnm)) ? sem_true : sem_false);
    }
    else {
      RTERR::Error(L"ExeISOFCLASS", RTERR_CLNM_NOT_DEFINED, Nil(), Nil(),
                          Sequence().ImpAppend(AUX::SingleNameToString(clnm)));
    }
  }
  else {
    PUSH(sem_false);
  }
}

// ExeISOFBASECLASS
// clnm : AS`Name
// ==> ()
void StackEval::ExeISOFBASECLASS (const TYPE_AS_Name & clnm)
{
  TYPE_SEM_VAL arg_v (POP());

  if (arg_v.Is(TAG_TYPE_SEM_OBJ_uRef)) {
    if (theState().IsAClass(clnm)) {
      const TYPE_AS_Name & objnm (arg_v.GetRecord(pos_SEM_OBJ_uRef_tp));
      PUSH((theState().IsBaseClass(clnm) && ((objnm == clnm) || theState().IsSubClass(objnm, clnm)))
                  ? sem_true : sem_false);
    }
    else {
      RTERR::Error(L"ExeISOFBASECLASS", RTERR_CLNM_NOT_DEFINED, Nil(), Nil(),
                          Sequence().ImpAppend(AUX::SingleNameToString(clnm)));
    }
  }
  else {
    PUSH(sem_false);
  }
}

// ExeSAMEBASECLASS
// ==> ()
void StackEval::ExeSAMEBASECLASS()
{
  SEQ<TYPE_SEM_VAL> items (POPN(2)); // [expr1_v,expr2_v]

  if (items[1].Is(TAG_TYPE_SEM_OBJ_uRef) && items[2].Is(TAG_TYPE_SEM_OBJ_uRef)) {
    const TYPE_AS_Name & objnm1 (items[1].GetRecord(pos_SEM_OBJ_uRef_tp));
    const TYPE_AS_Name & objnm2 (items[2].GetRecord(pos_SEM_OBJ_uRef_tp));
      SET<TYPE_AS_Name> supers (theState().GetAllSupers(objnm1).ImpUnion(mk_set(objnm1)));
      SET<TYPE_AS_Name> roots;
      Generic cl;
      for (bool bb = supers.First(cl); bb; bb = supers.Next(cl)) {
        if (theState().IsBaseClass(cl) && ((objnm2 == cl) || theState().IsSubClass(objnm2, cl))) {
          roots.Insert(cl); // cl has't super class
        }
      }
      PUSH(roots.IsEmpty() ? sem_false : sem_true);
  }
  else {
    PUSH(sem_false);
  }
}

// ExeSAMECLASS
// ==> ()
void StackEval::ExeSAMECLASS()
{
  SEQ<TYPE_SEM_VAL> items (POPN(2)); // [expr1_v,expr2_v]

  if (items[1].Is(TAG_TYPE_SEM_OBJ_uRef) && items[2].Is(TAG_TYPE_SEM_OBJ_uRef)) {
    const TYPE_AS_Name & objnm1 (items[1].GetRecord(pos_SEM_OBJ_uRef_tp));
    const TYPE_AS_Name & objnm2 (items[2].GetRecord(pos_SEM_OBJ_uRef_tp));
    PUSH((objnm1 == objnm2) ? sem_true : sem_false);
  }
  else {
    PUSH(sem_false);
  }
}

// ExeHISTORY
// kind : INSTRTP`HistoryKind
// opnms : seq of AS`Name
// ==> ()
void StackEval::ExeHISTORY(const TYPE_INSTRTP_HistoryKind & kind, const SEQ<TYPE_AS_Name> & opnms)
{
  double sum = 0;
  size_t len_opnms = opnms.Length();
  for (size_t i = 1; i <= len_opnms; i++) {
    TYPE_SEM_NUM val (theState().LookUpHistory(kind, opnms[i]));
    sum += val.GetRealValue(pos_SEM_NUM_v);
  }
  PUSH(TYPE_SEM_NUM().Init(Real(sum)));
}

// ExeSTARTLIST
// isset : bool
// ==> ()
void StackEval::ExeSTARTLIST(const Bool & isset)
{
  TYPE_STKM_EvalStackItem instr (POP()); // SEM`VAL | set of SEM`VAL

  SET<TYPE_SEM_VAL> instr_s (isset ? TYPE_SEM_SET(instr).GetSet(pos_SEM_SET_v) : Set().Insert(instr));

// 20071024 change scan direction
  SEQ<TYPE_SEM_VAL> instr_l (ASTAUX::SetToSequenceR(instr_s));
  size_t len_instr_l = instr_l.Length(); 
  for (size_t index = 1; index <= len_instr_l; index++) {
    const TYPE_SEM_VAL & inst (instr_l[index]);
    if(inst.Is(TAG_TYPE_SEM_OBJ_uRef)) {
      theScheduler().StartNewThread(inst);
    }
    else {
      RTERR::Error(L"ExeSTARTLIST", RTERR_NOOBJECT, Nil(), Nil(), Sequence());
    }

    // HC: 990914
    // In the specification it says:
    // StartNewThread(inst_g, 1).
    // The '1' should be the priority of the thread.
    // However, the specification does not yet support prioritybased
    // threads, and thus I do not think there is no reason to have
    // dangling constants around the code or specification. I have
    // put a note in the specification suggesting that we should not
    // have any dangling constants!! The note is in schd.vdm.
  }
}

// ExeSTOPLIST
// isset : bool
// ==> ()
void StackEval::ExeSTOPLIST(const Bool & isset)
{
  TYPE_STKM_EvalStackItem instr (POP()); // SEM`VAL | set of SEM`VAL

  SET<TYPE_SEM_VAL> instr_s (isset ? TYPE_SEM_SET(instr).GetSet(pos_SEM_SET_v) : Set().Insert(instr));

  TYPE_SEM_OBJ_uRef curref (GetCurObjRef());
  bool found = false; 
  SEQ<TYPE_SEM_VAL> instr_l (ASTAUX::SetToSequenceR(instr_s));
  size_t len_instr_l = instr_l.Length(); 
  for (size_t index = 1; index <= len_instr_l; index++) {
    const TYPE_SEM_VAL & inst (instr_l[index]);
    if(inst.Is(TAG_TYPE_SEM_OBJ_uRef)) {
      if (inst == curref) {
        found = true;
      }
      else {
        theScheduler().StopThread(inst);
      }
    }
    else
      RTERR::Error(L"ExeSTOPLIST", RTERR_NOOBJECT, Nil(), Nil(), Sequence());
  }
  if (found) {
    theScheduler().StopThread(curref);
  }
}

// ExeGUARD
// ==> ()
void StackEval::ExeGUARD(const Bool & log)
{
  if (!UsesThreads()) return;

  const SEQ<TYPE_SEM_VAL> & argv (GETNTH(1));
  const TYPE_SEM_VAL & fct_v (GETNTH(2));

  switch(fct_v.GetTag()) {
    case TAG_TYPE_SEM_ExplOP: {
      GuardCheck(fct_v, fct_v.GetField(pos_SEM_ExplOP_objref), argv, log);
      break;
    }
    case TAG_TYPE_SEM_OverOPFN: {
      Generic val (ConvertOverOPFNToExplOP(fct_v, fct_v.GetField(pos_SEM_CompExplFN_objref), Bool(false), argv));
      if (val.Is(TAG_TYPE_SEM_ExplOP)) {
        GuardCheck(val, fct_v.GetField(pos_SEM_CompExplFN_objref), argv, log);
      }
      break;
    }
  }
}

// GuardCheck
// op_v : SEM`ExplOP
// obj : [SEM`OBJ_Ref]
// arglv : [seq of SEM`VAL]
// log : bool 
// ==> ()
void StackEval::GuardCheck(const TYPE_SEM_ExplOP & op_v,
                           const Generic & obj,
                           const SEQ<TYPE_SEM_VAL> & arglv,
                           const Bool & log)
{
  if (!UsesThreads()) return;

  const TYPE_AS_Name & modName (op_v.GetRecord(pos_SEM_ExplOP_modName));
  const TYPE_AS_Name & fnName (op_v.GetRecord(pos_SEM_ExplOP_fnName));
  const Generic & objref (op_v.GetField(pos_SEM_ExplOP_objref));
  const Generic & manglenm (op_v.GetField(pos_SEM_ExplOP_manglenm));
#ifdef VICE
  const Bool & sync (op_v.GetBool(pos_SEM_ExplOP_sync));
#endif // VICE

  TYPE_AS_Name opnm (AUX::ConstructDoubleName(modName, fnName));

#ifdef VICE
  Generic obj_ref ((obj.IsNil() || objref.IsNil()) ? TopClNmCurObj() : objref);

//  if( log.GetValue() )
  if( log.GetValue() && (modName != fnName) ) {
    TYPE_AS_Name realnm (!manglenm.IsNil() ? AUX::ConstructDoubleName(modName, manglenm) : opnm);
    //theState().UpdateHistCount(realnm, TYPE_INSTRTP_req(), objref, sync, arglv);
    theState().UpdateHistCount(realnm, EvalState::historyKind_req, objref, sync, arglv);
  }

  if( !theSystem().OnCurCPU(obj_ref) ) {
    TYPE_STKM_MessageId mid (theSystem().AddMessageToBUS(obj_ref, op_v, arglv));
    if( sync ) {
      theScheduler().SetThreadStatus(theScheduler().CurThreadId(), TYPE_SCHDTP_SyncOp().Init(mid));
    }
    return;
  }
  else if (theState().IsStatic(modName, fnName) && !sync && log.GetValue() ) {
    theSystem().AddBroadcastMessageToBUS(obj_ref, op_v, arglv);
    return;
  }
#else
  // VDMPP
  TYPE_AS_Name realnm (!manglenm.IsNil() ? AUX::ConstructDoubleName(modName, manglenm) : opnm);
//  theState().UpdateHistCount(realnm, TYPE_INSTRTP_req(), objref);
  theState().UpdateHistCount(realnm, EvalState::historyKind_req, objref);
#endif // VICE

  Generic refcl = Nil();
  if (!obj.IsNil()) {
    refcl = Record(obj).GetRecord(pos_SEM_OBJ_uRef_tp);
    if (theState().IsSubClass(refcl, modName)) {
      refcl = modName;
    }
  }

  TYPE_AS_Name permis_opnm (!refcl.IsNil() ? AUX::ConstructDoubleName(refcl, fnName) : opnm);

  SetGuard(permis_opnm, obj);
}

// ExeTHREADID
// ==> ()
void StackEval::ExeTHREADID()
{
  PUSH(TYPE_SEM_NUM().Init(Real(theScheduler().CurThreadId())));
}

#ifdef VICE
// ExeINCRTIME
// n : nat
// ==> ()
void StackEval::ExeINCRTIME (const Int & n)
{
  if (!theSystem().InDuration() || theScheduler().CheckingGuard()) {
    theSystem().IncrRelTime(n);
  }
}

// ExeINCRTIME_PREF
// opr : AS`UnaryOp
// oh : nat
// ==> ()
void StackEval::ExeINCRTIME_PREF (const TYPE_AS_UnaryOp & opr, const Int & oh)
{
  const TYPE_STKM_EvalStackItem & esval (Head());
  Generic valg (GetVal(esval));

  if (valg.IsNil()) {
    TimeError();
  }
  else {
    Record val (valg);
    Generic mult = Nil();
    int tag = ((Int) opr).GetValue();
    switch(tag){
      case SETDISTRUNION:
      case SETDISTRINTERSECT: {
        bool all_set = val.Is(TAG_TYPE_SEM_SET);
        if (all_set) {
          Set vs (val.GetSet(pos_SEM_SET_v));
          Generic g;
          for (bool bb = vs.First(g); bb; bb = vs.Next(g) && all_set) {
            all_set = g.Is(TAG_TYPE_SEM_SET);
          }
        }
        if (all_set) {
          mult = dsetsize(val);
        }
        break;
      }
      case SETPOWER: {
        if (val.Is(TAG_TYPE_SEM_SET))
//             mult = Int((int)pow((double)2, val.GetSet(pos_SEM_SET_v).Card()));
             mult = Int(2 << val.GetSet(pos_SEM_SET_v).Card());
          break;
      }
      case SEQDISTRCONC: {
        bool all_seq = (val.Is(TAG_TYPE_SEM_SEQ));
        if (all_seq) {
          Sequence vs (val.GetSequence(pos_SEM_SEQ_v));
          Generic g;
          for (bool ii = vs.First(g); ii; ii = vs.Next(g) && all_seq) {
            all_seq = g.Is(TAG_TYPE_SEM_SEQ);
          }
        }
        if (all_seq) {
          mult = dseqsize(val);
        }
        break;
      }
      case SEQELEMS:
      case SEQINDICES: {
        if (val.Is(TAG_TYPE_SEM_SEQ)) {
          mult = Int(val.GetSequence(pos_SEM_SEQ_v).Length());
        }
        break;
      }
      case SEQTAIL: {
        if (val.Is(TAG_TYPE_SEM_SEQ)) {
          mult = Int(val.GetSequence(pos_SEM_SEQ_v).Length()) - Int(1);
        }
        break;
      }
      case SEQREVERSE: {
        if (val.Is(TAG_TYPE_SEM_SEQ)) {
          mult = Int(val.GetSequence(pos_SEM_SEQ_v).Length());
        }
        break;
      }
      case MAPDOM: {
        if (val.Is(TAG_TYPE_SEM_MAP)) {
          mult = Int(val.GetMap(pos_SEM_MAP_v).Dom().Card());
        }
        break;
      }
      case MAPRNG: {
        if (val.Is(TAG_TYPE_SEM_MAP)) {
          mult = Int(val.GetMap(pos_SEM_MAP_v).Rng().Card());
        }
        break;
      }
      case MAPDISTRMERGE: {
        bool all_map = (val.Is(TAG_TYPE_SEM_SET));
        if (all_map) {
          Set vs (val.GetSet(pos_SEM_SET_v));
          Generic g;
          for (bool bb = vs.First(g); bb; bb = vs.Next(g) && all_map) {
            all_map = g.Is(TAG_TYPE_SEM_MAP);
          }
        }
        if (all_map) {
          mult = dmapsize(val);
        }
        break;
      }
      case MAPINVERSE: {
        if (val.Is(TAG_TYPE_SEM_MAP)) {
          mult = Int(val.GetMap(pos_SEM_MAP_v).Rng().Card());
        }
        break;
      }
      default: break;
    }
    if (!theSystem().InDuration()) {
      if (mult.IsNil()) {
        TimeError();
      }
      else {
        theSystem().IncrRelTime(((Int) mult).GetValue() * oh);
      }
    }
  }
}

// dsetsize
// s : SEM`SET
// -> nat
Int StackEval::dsetsize (const TYPE_SEM_SET & s)
{
  SET<TYPE_SEM_VAL> vdms (s.get_v());
  if (vdms.IsEmpty()) {
    return Int(0);
  }
  else {
    Generic g;
    Int num_elts (0);
    for (bool bb = vdms.First(g); bb; bb = vdms.Next(g)) {
      TYPE_SEM_SET sv (g);
      SET<TYPE_SEM_VAL> setv (sv.get_v());
      num_elts = num_elts + Int(setv.Card());
    }
    return num_elts;
  }
}

// dseqsize
// s : SEM`SEQ
// -> nat
Int StackEval::dseqsize (const TYPE_SEM_SEQ & s)
{
  SEQ<TYPE_SEM_VAL> vdms (s.get_v());
  if (vdms.IsEmpty()) {
    return Int(0);
  }
  else {
    Generic g;
    Int num_elts (0);
    for (bool bb = vdms.First(g); bb; bb = vdms.Next(g)) {
      TYPE_SEM_SEQ sv (g);
      SEQ<TYPE_SEM_VAL> seq (sv.get_v());
      num_elts = num_elts + Int(seq.Length());
    }
    return num_elts;
  }
}

// dmapsize
// s : SEM`SET
// -> nat
Int StackEval::dmapsize (const TYPE_SEM_SET & s)
{
  SET<TYPE_SEM_VAL> vdms (s.get_v());
  if (vdms.IsEmpty()) {
    return Int(0);
  }
  else {
    Generic g;
    Int num_elts (0);
    for (bool bb = vdms.First(g); bb; bb = vdms.Next(g)) {
      TYPE_SEM_MAP mv (g);
      MAP<TYPE_SEM_VAL,TYPE_SEM_VAL> m (mv.get_v());
      num_elts = num_elts + Int(m.Size());
    }
    return num_elts;
  }
}

// GetVal
// esval : STKM`EvalStackItem
// ==> [SEM`VAL]
Generic StackEval::GetVal (const TYPE_STKM_EvalStackItem & esval)
{
//  if (esval.IsRecord())
//    if ((((Record) esval).Is(TAG_TYPE_AS_Name)) ||
//        (((Record) esval).Is(TAG_TYPE_AS_OldName)))
//      return theState().LookUp(esval);

  if (esval.Is(TAG_TYPE_AS_Name) || esval.Is(TAG_TYPE_AS_OldName)) {
    return theState().LookUp(esval);
  }
  else if (SemRec::IsSemVal(esval)) {
    return esval;
  }
  else {
    return Nil();
  }
}

// ExeINCRTIME_BIN
// opr : AS`BinaryOp
// oh1 : nat
// oh2 : [nat]
// ==> ()
void StackEval::ExeINCRTIME_BIN (const TYPE_AS_BinaryOp & opr,
                                 const Int & oh1,
                                 const Generic & oh2)
{
  const TYPE_STKM_EvalStackItem & r_esval (GetES(Int(1)));
  const TYPE_STKM_EvalStackItem & l_esval (GetES(Int(2)));
  Generic rvalg (GetVal(r_esval));
  Generic lvalg (GetVal(l_esval));

  if (lvalg.IsNil() || rvalg.IsNil()) {
    TimeError();
  }
  else {
    Record lval (lvalg);
    Record rval (rvalg);
    Generic time = Nil();
    int tag = ((Int) opr).GetValue();
    switch (tag) {
    case NUMEXP:
      { if (rval.Is(TAG_TYPE_SEM_NUM))
          time = Int((int)rval.GetReal(pos_SEM_NUM_v).GetValue() * oh1);
        break;
      }
    case SETUNION:
    case SETINTERSECT:
    case PROPERSUBSET:
    case SETMINUS:
      { if (lval.Is(TAG_TYPE_SEM_SET) &&
            rval.Is(TAG_TYPE_SEM_SET))
          time = Int((lval.GetSet(pos_SEM_SET_v).Card() +
                      rval.GetSet(pos_SEM_SET_v).Card()) * oh1);
        break;
      }
    case SUBSET:
    case INSET:
    case NOTINSET:
      { if (lval.Is(TAG_TYPE_SEM_SET))
          time = Int(lval.GetSet(pos_SEM_SET_v).Card() * oh1);
        break;
      }
    case SEQCONC:
      { if (rval.Is(TAG_TYPE_SEM_SEQ) &&
            lval.Is(TAG_TYPE_SEM_SEQ))
           time = Int((lval.GetSequence(pos_SEM_SEQ_v).Length() +
                       rval.GetSequence(pos_SEM_SEQ_v).Length()) * oh1);
        break;
      }
    case MAPMERGE:
      { if (rval.Is(TAG_TYPE_SEM_MAP) &&
            lval.Is(TAG_TYPE_SEM_MAP))
        {
          Map ls (lval.GetMap(pos_SEM_MAP_v)),
              rs (rval.GetMap(pos_SEM_MAP_v));
          time = Int(ls.Dom().ImpIntersect(rs.Dom()).Card() * Int(oh2) +
                    (ls.Dom().Card() + rs.Dom().Card() * oh1));
        }
        break;
      }
    case MAPDOMRESTTO:
      { if (lval.Is(TAG_TYPE_SEM_SET))
          time = Int(lval.GetSet(pos_SEM_SET_v).Card() * oh1);
        break;
      }
    case MAPDOMRESTBY:
      { if (lval.Is(TAG_TYPE_SEM_SET) &&
            rval.Is(TAG_TYPE_SEM_MAP))
        {
          Set ls (lval.GetSet(pos_SEM_SET_v));
          Map rm (rval.GetMap(pos_SEM_MAP_v));
          time = Int(ls.ImpIntersect(rm.Dom()).Card() * Int(oh2) +
                     rm.Dom().ImpDiff(ls).Card() * oh1);
        }
        break;
      }
    case MAPRNGRESTTO:
      { if (lval.Is(TAG_TYPE_SEM_MAP) &&
            rval.Is(TAG_TYPE_SEM_SET))
        {
          Map lm (lval.GetMap(pos_SEM_MAP_v));
          Set rs (rval.GetSet(pos_SEM_SET_v));
          time = Int(lm.Rng().ImpIntersect(rs).Card() * oh1);
        }
        break;
      }
    case MAPRNGRESTBY:
      { if (lval.Is(TAG_TYPE_SEM_MAP) &&
            rval.Is(TAG_TYPE_SEM_SET))
        {
          Map lm (lval.GetMap(pos_SEM_MAP_v));
          Set rs (rval.GetSet(pos_SEM_SET_v));
          time = Int(lm.Rng().ImpIntersect(rs).Card() * Int(oh2) +
                     lm.Rng().ImpDiff(rs).Card() * oh1);
        }
        break;
      }
    case COMPOSE:
      { if (lval.Is(TAG_TYPE_SEM_MAP))
          time = Int(lval.GetMap(pos_SEM_MAP_v).Dom().Card() * oh1);
        break;
      }
    default: break;
    }
    if (!theSystem().InDuration())
    { if (time.IsNil())
        TimeError();
      else theSystem().IncrRelTime(time);
    }
  }
}

// ExeINCRTIME_NEW
// oh : nat
// nm : AS`Name
// ==> ()
void StackEval::ExeINCRTIME_NEW (const Int & oh, const TYPE_AS_Name & nm)
{
  if (!theSystem().InDuration())
  {
    Sequence inhstrct (theState().GetInhStrct(nm));
    inhstrct.ImpAppend(Set().Insert(nm));
    Sequence instvars;
    Generic g;
    for (bool bb = inhstrct.First(g); bb; bb = inhstrct.Next(g))
    {
      Set cnms(g);
      Generic h;
      Map m;
      for (bool cc = cnms.First(h); cc; cc = cnms.Next(h))
      {
        TYPE_AS_Name cnm (h);
        m.Insert(cnm, Int(theState().GetInstVars(cnm).Length()));
      }
      instvars.ImpAppend(m);
    }
    theSystem().IncrRelTime(Int(oh) * num_instvars(instvars));
  }
}

// num_instvars
// ivs : seq of (map AS`Name to nat)
// -> nat
Int StackEval::num_instvars (const Sequence & ivs)
{
  int result = 0;
  size_t len_ivs = ivs.Length();
  for (size_t idx = 1;idx <= len_ivs; idx++)
  {
    result += map_sum(Map(ivs[idx])).GetValue();
  }
  return Int(result);
}

// map_sum
// m : (map AS`Name to nat)
// -> nat
Int StackEval::map_sum (const Map & m)
{
  int result = 0;
  Set dom_m (m.Dom());
  Generic g;
  for (bool bb = dom_m.First(g); bb; bb = dom_m.Next(g))
  {
    result += ((Int) m[g]).GetValue();
  }
  return Int(result);
}

// ExeINCRTIME_SETSEQMAP
// oh : nat
// ==> ()
void StackEval::ExeINCRTIME_SETSEQMAP (const Int & oh)
{
  const TYPE_STKM_EvalStackItem & r_esval (GetES(Int(1)));
  const TYPE_STKM_EvalStackItem & l_esval (GetES(Int(2)));
  Generic lval (GetVal(l_esval));
  Generic rval (GetVal(r_esval));
  Generic mult = Nil();

  if (lval.Is(TAG_TYPE_SEM_NUM) && rval.Is(TAG_TYPE_SEM_NUM))
  {
    TYPE_SEM_NUM lnum (lval);
    TYPE_SEM_NUM rnum (rval);
    Int lval_v (lnum.get_v());
    Int rval_v (rnum.get_v());

    if (rval_v.GetValue() > lval_v.GetValue())
      mult = rval_v - lval_v;
    else mult = Int(0);
  }
  else if (rval.Is(TAG_TYPE_SEM_MAP))
  {
    TYPE_SEM_MAP rmap (rval);
    mult = Int(rmap.get_v().Dom().Card());
  }

  if (!theSystem().InDuration())
  {
    if (mult.IsNil())
      TimeError();
    else
      theSystem().IncrRelTime(((Int)mult).GetValue() * oh);
  }
}

// ExeINCRTIME_STARTLIST
// oh : nat
// ==> ()
void StackEval::ExeINCRTIME_STARTLIST (const Int & oh)
{
  const TYPE_STKM_EvalStackItem & esval (Head());
  Generic val (GetVal(esval));

//  if (val.IsRecord() && !Record(val).Is(TAG_TYPE_SEM_SET))
  if (!val.Is(TAG_TYPE_SEM_SET))
    TimeError();
  else if (!theSystem().InDuration())
  {
    TYPE_SEM_SET sv (val);
    theSystem().IncrRelTime(sv.get_v().Card() * Int(oh));
  }
}

// ExeINCRTIME_STOPLIST
// oh : nat
// ==> ()
void StackEval::ExeINCRTIME_STOPLIST (const Int & oh)
{
  const TYPE_STKM_EvalStackItem & esval (Head());
  Generic val (GetVal(esval));

  if (!val.Is(TAG_TYPE_SEM_SET))
    TimeError();
  else if (!theSystem().InDuration())
  {
    TYPE_SEM_SET sv (val);
    theSystem().IncrRelTime(sv.get_v().Card() * Int(oh));
  }
}

// This is called if a runtime value is of the wrong type. In this
// case the interpreter will itself signal a runtime error when the
// corresponding instruction is reached, so for now we do nothing.

// TimeError
// ==> ()
void StackEval::TimeError ()
{
}

// ExeRETMES
// id : STKM`MessageId
// prio : nat
// opnm : AS`Name
// tocpu : [AS`Name]
// threadid : SCHDTP`ThreadId
// ==> ()
void StackEval::ExeRETMES(const TYPE_STKM_MessageId & id,
                          const Int & prio,
                          const TYPE_AS_Name & opnm,
                          const Generic & tocpu,
                          const TYPE_SCHDTP_ThreadId & threadid)
{
  const TYPE_STKM_EvalStackItem & val (Head());

  theSystem().AddReturnMessageToBUS(id, Int(theSystem().GetTime()), prio, val, opnm, tocpu, threadid);

  TerminateProgram();
}
#endif //VICE
#endif //VDMPP
// }}}
