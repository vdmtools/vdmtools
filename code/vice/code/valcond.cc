#include "metaiv.h"
#include "libman.h"
#include "SCHDTP.h"
#include "TIMETRACE.h"
#include "TIMETRACETP.h"
#include "VC.h"
#include "valcond.h"
#include "rt_errmsg.h"

Generic VC::loginstvars = Quote(L"ALL"); // <ALL> | set of (AS`Name * AS`Name * AS`Name)
MAP<TYPE_AS_Name, TYPE_VC_OpHist> VC::ophistmap;
MAP<TYPE_AS_Name, TYPE_VC_InstVarHist> VC::instvarhistmap;
Map VC::instdecl; // map SEM`OBJ_Ref to (AS`Name * AS`Name)

// InitState
// ==> ()
void VC::InitState ()
{
  VC::ophistmap.Clear();
  VC::instvarhistmap.Clear();
  VC::instdecl.Clear();
}

// LogInstVarChange
// instnm : AS`Name
// val : SEM`VAL
// objref : SEM`OBJ_Ref
// thrid : nat
// ==> ()
void VC::LogInstVarChange (const TYPE_AS_Name & instnm,
                           const TYPE_SEM_VAL & val_,
                           const TYPE_SEM_OBJ_uRef & objref_,
                           const Int & thrid)
{
  TYPE_SEM_OBJ_uRef objref (TIMETRACE::ConvertObjRef(objref_));
  TYPE_SEM_VAL val (TIMETRACE::ConvertObjRef(val_));

  if (VC::instdecl.DomExists(objref))
  {
    Tuple idt (VC::instdecl[objref]);
    TYPE_AS_Name sysnm (idt.GetField(1));
    TYPE_AS_Name statinstnm (idt.GetField(2));
    if ((VC::loginstvars.IsQuote() ||
         (VC::loginstvars.IsSet() && Set(VC::loginstvars).InSet(mk_(sysnm, statinstnm, instnm)))) &&
        !val.Is(TAG_TYPE_SEM_UNDEF)) 
    {
      TYPE_AS_Name fullnm (VC::CombineNms(sysnm, statinstnm, instnm));
      TYPE_VC_Time t (theSystem().GetTime()); 

      TYPE_VC_InstVar tlog;
      tlog.Init(t, val, thrid);
      VC::AddInstVarHist(fullnm, tlog);

      TYPE_TIMETRACETP_InstVarChange ivc;
      ivc.Init(fullnm, val, objref, t, thrid);
      TIMETRACE::AddToLogfile(ivc); 
    }
  }
}

// AddInstVarLog
// nametup : <ALL> | (AS`Name * AS`Name * AS`Name)
// ==> ()
void VC::AddInstVarLog (const Generic & nametup)
{
  if (nametup.IsQuote() || VC::loginstvars.IsQuote())
  {
    VC::loginstvars = Quote(L"ALL");
  }
  else
  {
    Set s (VC::loginstvars);
    s.Insert(nametup);
    VC::loginstvars = s; 
  }
}

// AddInstDecl
// sysnm : AS`Name
// instnm : AS`Name
// objref : SEM`VAL
// ==> ()
void VC::AddInstDecl (const TYPE_AS_Name & sysnm, const TYPE_AS_Name & instnm, const TYPE_SEM_VAL & objref_)
{
  TYPE_SEM_OBJ_uRef objref (TIMETRACE::ConvertObjRef(objref_));

  VC::instdecl.ImpModify(objref, mk_(sysnm, instnm));
}

// AddOpReq
// opnm : AS`Name
// vall : [seq of SEM`VAL]
// thrid : nat
// time : nat
// ==> ()
void VC::AddOpReq (const TYPE_AS_Name & opnm, const Generic & vall, const Int & thrid, const Int & time)
{
  TYPE_VC_Req req;
  req.Init(time, vall, thrid);

  if (VC::ophistmap.DomExists(opnm))
  {
    TYPE_VC_OpHist oh (VC::ophistmap[opnm]);
    SEQ<TYPE_VC_Req> reqs (oh.get_reqs());
    reqs.ImpAppend(req);
    oh.set_reqs(reqs);
    VC::ophistmap.ImpModify(opnm, oh);
  }
  else
  {
    SEQ<TYPE_VC_Req> reqs;
    reqs.ImpAppend(req);
    TYPE_VC_OpHist oh;
    oh.Init(reqs, SEQ<TYPE_VC_Act>(), SEQ<TYPE_VC_Fin>());
    VC::ophistmap.ImpModify(opnm, oh);
  }
}

// AddOpAct
// opnm : AS`Name
// thrid : nat
// time : nat
// ==> ()
void VC::AddOpAct (const TYPE_AS_Name & opnm, const Int & thrid, const Int & time)
{
  if (VC::ophistmap.DomExists(opnm))
  {
    TYPE_VC_Act act;
    act.Init(time, thrid);

    TYPE_VC_OpHist oh (VC::ophistmap[opnm]);
    SEQ<TYPE_VC_Act> acts (oh.get_acts());
    acts.ImpAppend(act);
    oh.set_acts(acts);
    VC::ophistmap.ImpModify(opnm, oh);
  }
  else
    RTERR::Error(L"VC::AddOpAct", RTERR_NO_HISTORY_MAP, Nil(), Nil(), Sequence());
}

// AddOpFin
// opnm : AS`Name
// res : [SEM`VAL]
// thrid : nat
// time : nat
// ==> ()
void VC::AddOpFin (const TYPE_AS_Name & opnm, const Generic & res, const Int & thrid, const Int & time)
{
  if (VC::ophistmap.DomExists(opnm))
  {
    TYPE_VC_Fin fin;
    fin.Init(time, res, thrid);

    TYPE_VC_OpHist oh (VC::ophistmap[opnm]);
    SEQ<TYPE_VC_Fin> fins (oh.get_fins());
    fins.ImpAppend(fin);
    oh.set_fins(fins);
    VC::ophistmap.ImpModify(opnm, oh);
  }
  else
    RTERR::Error(L"VC::AddOpFin", RTERR_NO_HISTORY_MAP, Nil(), Nil(), Sequence());
}


// AddInstVarHist
// nm : AS`Name
// instvar : InstVar
// ==> ()
void VC::AddInstVarHist (const TYPE_AS_Name & nm,
                         const TYPE_VC_InstVar & instvar)
{
  if (VC::instvarhistmap.DomExists(nm))
  {
    TYPE_VC_InstVarHist ivh (VC::instvarhistmap[nm]);
    ivh.ImpAppend(instvar);
    VC::instvarhistmap.ImpModify(nm, ivh);
  }
  else
  {
    TYPE_VC_InstVarHist ivh;
    ivh.ImpAppend(instvar);
    VC::instvarhistmap.ImpModify(nm, ivh);
  }
}

// EvalValCond
// vc : ValCond
// ==> [nat * SCHDTP`ThreadId * [nat] * [SCHDTP`ThreadId]]
Generic VC::EvalValCond (const TYPE_VC_ValCond & vc)
{
  switch(vc.GetTag()) {
    case TAG_TYPE_VC_DeadlineMet:
      return EvalDeadlineMet(vc);
    case TAG_TYPE_VC_Separate:
      return EvalSeparate(vc);
    case TAG_TYPE_VC_SeparateReq:
      return EvalSeparateReq(vc);
    case TAG_TYPE_VC_Periodic:
      return EvalPeriodic(vc);
    default:
      RTERR::Error(L"VC::EvalValCond", RTERR_UNKNOWN_VALCOND, Nil(), Nil(), Sequence());
  }
  return Nil(); // dummy 
}

// EvalDeadlineMet
// dm : DeadlineMet
// ==> [nat * SCHDTP`ThreadId * [nat] * [SCHDTP`ThreadId]]
Generic VC::EvalDeadlineMet (const TYPE_VC_DeadlineMet & dm)
{
  TYPE_VC_EventExpr ev1 (dm.get_stimulus());
  Generic pred (dm.get_cond());
  TYPE_VC_EventExpr ev2 (dm.get_react());
  TYPE_VC_Time maxDelay (dm.get_maxDelay());
  Bool match (dm.get_match());
  
  if (match)
    return MatchCheck(ev1, pred, ev2, maxDelay, Quote(L"MAX"), false);
  else 
    return AnyCheck(ev1, pred, ev2, maxDelay, Quote(L"MAX"), false);
}

// MatchCheck
// ev1 : EventExpr
// pred : [Pred]
// ev2 : EventExpr
// delay : nat
// kind : Kind
// req : bool
// ==> [nat * SCHDTP`ThreadId * [nat] * [SCHDTP`ThreadId]]
Generic VC::MatchCheck (const TYPE_VC_EventExpr & ev1,
                        const Generic & pred,
                        const TYPE_VC_EventExpr & ev2,
                        const Int & delay,
                        const TYPE_VC_Kind & kind,
                        bool req)
{
  Sequence list1 (VC::FindList(ev1));
  Sequence list2 (VC::FindList(ev2));
  for (int index = 1; index <= list1.Length(); index++)
  {
    TYPE_VC_Time t1 (GetTim(list1[index]));
    Generic t2;
    if (index <= list2.Length())
      t2 = GetTim(list2[index]);
    else
      t2 = Quote(L"INF");

    if (!VC::PredSatisfied(pred, t1))
      continue;
    else if (t2.IsQuote() && req)
      return mk_(t1, GetThrId(list1[index]), Nil(), Nil());
    else if (!t2.IsQuote() && VC::Violation(t1, t2, delay, kind))
      return mk_(t1, GetThrId(list1[index]), t2, GetThrId(list2[index]));
  }
  return Nil();
}

// AnyCheck
// ev1 : EventExpr
// pred : [Pred]
// ev2 : EventExpr
// delay : nat
// kind : Kind
// req : bool
// ==> [nat * SCHDTP`ThreadId * [nat] * [SCHDTP`ThreadId]]
Generic VC::AnyCheck (const TYPE_VC_EventExpr & ev1,
                      const Generic & pred,
                      const TYPE_VC_EventExpr & ev2,
                      const Int & delay,
                      const TYPE_VC_Kind & kind,
                      bool req)
{
  Sequence list1 (VC::FindList(ev1));
  Sequence list2 (VC::FindList(ev2));
  for (int index1 = 1; index1 <= list1.Length(); index1++)
  {
    TYPE_VC_Time t1 (GetTim(list1[index1]));
    if (!VC::PredSatisfied(pred, t1))
      continue;
    else
    {
      bool notfound = true;
      for (int index2 = 1; index2 <= list2.Length() && notfound; index2++ )
      {
        TYPE_VC_Time t2 (GetTim(list2[index2]));
        if (t2.GetValue() > t1.GetValue())
        {
          if (Violation(t1, t2, delay, kind))
            return mk_(t1, GetThrId(list1[index1]),
                       t2, GetThrId(list2[index2]));
          else
            notfound = false;
        }
      }
      if (notfound && req)
        return mk_(t1, GetThrId(list1[index1]), Nil(), Nil());
    }
  }
  return Nil();
}

// Violation
// t1 : Time
// t2 : Time
// delay : Time
// kind : Kind
// +> bool
bool VC::Violation (const TYPE_VC_Time & t1_,
                    const TYPE_VC_Time & t2_,
                    const TYPE_VC_Time & delay_,
                    const TYPE_VC_Kind & kind)
{
  int t1 = t1_.GetValue();
  int t2 = t2_.GetValue();
  int delay = delay_.GetValue();
  if (Quote(L"MAX") == kind)
    return ((t2 - t1) > delay); 
  else if (Quote(L"MIN") == kind)
    return ((t2 - t1) < delay); 
  else
    RTERR::Error(L"VC::Violation", RTERR_UNKNOWN_VALCOND_KIND, Nil(), Nil(), Sequence());
  return false; // dummy
}

// EvalSeparate
// sep : Separate
// ==> [nat * SCHDTP`ThreadId * [nat] * [SCHDTP`ThreadId]]
Generic VC::EvalSeparate (const TYPE_VC_Separate & sep)
{
  TYPE_VC_EventExpr stimu (sep.get_stimu());
  Generic cond (sep.get_cond());
  TYPE_VC_EventExpr reac (sep.get_reac());
  TYPE_VC_Time minDelay (sep.get_minDelay());
  Bool match (sep.get_match());
  
  if (match)
    return MatchCheck(stimu, cond, reac, minDelay, Quote(L"MIN"), false);
  else 
    return AnyCheck(stimu, cond, reac, minDelay, Quote(L"MIN"), false);
}

// EvalSeparateReq
// sr : SeparateReq
// ==> [nat * SCHDTP`ThreadId * [nat] * [SCHDTP`ThreadId]]
Generic VC::EvalSeparateReq (const TYPE_VC_SeparateReq & sr)
{
  TYPE_VC_EventExpr stimu (sr.get_stimu());
  Generic cond (sr.get_cond());
  TYPE_VC_EventExpr reac (sr.get_reac());
  TYPE_VC_Time minDelay (sr.get_minDelay());
  Bool match (sr.get_match());
  
  if (match)
    return MatchCheck(stimu, cond, reac, minDelay, Quote(L"MIN"), true);
  else 
    return AnyCheck(stimu, cond, reac, minDelay, Quote(L"MIN"), true);
}

// EvalPeriodic
// per : Periodic
// ==> [nat * SCHDTP`ThreadId * [nat] * [SCHDTP`ThreadId]]
Generic VC::EvalPeriodic (const TYPE_VC_Periodic & per)
{
  TYPE_VC_EventExpr ev (per.get_event());

  Sequence list (VC::FindList(ev));
  if (!list.IsEmpty())
  {
    int peri = per.get_period().GetValue();
    int jit  = per.get_jitter().GetValue();
    int curtime = GetTim(list.Hd()).GetValue();
    for (int index = 2; index <= list.Length(); index++)
    {
      int t2 = GetTim(list[index]).GetValue();
      if (((t2 - curtime) > (peri - jit)) &&
          ((t2 - curtime) < (peri + jit)))
        return mk_(GetTim(list[index - 1]),
                   GetThrId(list[index - 1]),
                   GetTim(list[index]),
                   GetThrId(list[index]) );
      curtime = t2;
    }
  }
  return Nil();
}

// FindList
// ev : EventExpr
// ==> seq of (Req | Act | Fin | InstVar)
type_dL VC::FindList (const TYPE_VC_EventExpr & ev)
{
  switch(ev.GetTag()) {
    case TAG_TYPE_VC_OpEvent: {
      TYPE_AS_Name opnm (ev.GetRecord(pos_VC_OpEvent_opnm));
      if (VC::ophistmap.DomExists(opnm))
      {
        TYPE_VC_OpHist oh (VC::ophistmap[opnm]);
        Sequence l;
        switch(ev.GetRecord(pos_VC_OpEvent_kind).GetTag()) {
          case TAG_TYPE_INSTRTP_req: {
            l = oh.get_reqs();
            break;
          }
          case TAG_TYPE_INSTRTP_act: {
            l = oh.get_acts();
            break;
          }
          case TAG_TYPE_INSTRTP_fin: {
            l = oh.get_fins();
            break;
          }
        }
        if (ev.GetField(pos_VC_OpEvent_args).IsNil())
          return (Generic)l;
        else
        {
          type_dL res;
          Generic g;
          for (int i = 1; i <= l.Length(); i++)
          {
            if (MatchArg(ev.GetField(pos_VC_OpEvent_args),
                Int(i), l[i], oh.get_reqs()))
              res.ImpAppend(l[i]);
          }
          return res;
        }
      }
      else
        return type_dL();
    }
    case TAG_TYPE_VC_TransEvent: {
      TYPE_VC_Pred pred (ev.GetRecord(pos_VC_TransEvent_pred));
      TYPE_AS_Name instvar (pred.get_instvar());
      Generic op (pred.get_op());
      TYPE_SEM_VAL num (pred.get_num());

      if (VC::instvarhistmap.DomExists(instvar))
      {
        TYPE_VC_InstVarHist hist (VC::instvarhistmap[instvar]);
        TYPE_VC_InstVar iv (hist.Hd());
        TYPE_SEM_VAL firstv (iv.get_val());
        bool initval = VC::EvalOp(firstv, op, num);
        type_dL res;
        bool curbool = initval;
        for (int i = 2; i < hist.Length(); i++)
        {
          if (EvalOp(hist[i].get_val(), op, num) != curbool)
          {
            curbool = !curbool;
            res.ImpAppend(hist[i]);
          }
        }
        return res;
      }
      else
        return type_dL();
    }
  }
  return type_dL();
}

// PredSatisfied
// pred : [Pred]
// time : nat
// ==> bool
bool VC::PredSatisfied (const Generic & pred_,
                        const Int & time)
{
  if (pred_.IsNil())
    return true;
  
  TYPE_VC_Pred pred (pred_);
  TYPE_AS_Name var (pred.get_instvar());
  Generic op (pred.get_op());
  TYPE_SEM_VAL num (pred.get_num());

  if (VC::instvarhistmap.DomExists(var))
  {
    TYPE_VC_InstVarHist hist (VC::instvarhistmap[var]);
    TYPE_VC_InstVar iv (hist.Hd()); 
    TYPE_VC_Time firstt (iv.get_tim());
    if (time.GetValue() < firstt.GetValue())
      return false;
    else
    {
      for (int i = 2; i <= hist.Length(); i++)
      {
        if ((hist[i - 1].get_tim().GetValue() <= time.GetValue()) &&
            (time.GetValue() < hist[i].get_tim().GetValue()))
        {
          return EvalOp(hist[i - 1].get_val(), op, num);
        }
      }
      return EvalOp(hist[hist.Length()].get_val(), op, num);
    }
  }
  return false;
}

// EvalOp
// lhs : SEM`VAL
// op : (<LT> | <GT> | <LTE> | <GTE> | <EQ>)
// rhs : SEM`VAL
// ==> bool
bool VC::EvalOp (const TYPE_SEM_VAL & lhs, const Generic & op, const TYPE_SEM_VAL & rhs)
{
  int comp = lhs.Compare(rhs);
  if (Quote(L"LT") == op)
    return (comp < 0);
  else if (Quote(L"GT") == op)
    return (comp > 0);
  else if (Quote(L"LTE") == op)
    return (comp <= 0);
  else if (Quote(L"GTE") == op)
    return (comp >= 0);
  else if (Quote(L"EQ") == op)
    return (comp == 0);
    
  return false; // dummy
}

// CombineNms
// sysnm : AS`Name
// statinsnm : AS`Name
// instnm : AS`Name
// -> AS`Name
TYPE_AS_Name VC::CombineNms (const TYPE_AS_Name & sysnm, const TYPE_AS_Name & statinsnm, const TYPE_AS_Name & instnm)
{
  TYPE_AS_Ids ids;
  ids.ImpAppend(sysnm.get_ids().Hd());
  ids.ImpAppend(statinsnm.get_ids().Hd());
  ids.ImpAppend(instnm.get_ids().Hd());
  return TYPE_AS_Name().Init(ids, instnm.get_cid());
}

// MatchArg
// arg : seq of [SEM`VAL]
// index : nat
// ev : (Req | Act | Fin)
// reqs : seq of Req
// -> bool
bool VC::MatchArg (const Sequence & arg,
                   const Int & index,
                   const Record & ev,
                   const SEQ<TYPE_VC_Req> & reqs)
{
  if ((index.GetValue() >= 1) && (index.GetValue() <= reqs.Length()))
  {
    TYPE_VC_Req req (reqs[index]);
    Generic greqargs (req.get_arg());
    if (greqargs.IsNil())
      return arg.IsEmpty();
    else
    {
      Sequence reqargs (greqargs);
      bool forall = (reqargs.Length() == arg.Length());
      for (int i = 1; i <= arg.Length() && forall; i++)
        forall = (arg[i].IsNil() || (arg[i] == reqargs[i]));
      return forall;
    }
  }
  else
    RTERR::Error(L"VC::MatchArg", RTERR_ILLEGAL_VALCOND_INDEX, Nil(), Nil(), Sequence());
  return false; // dummy
}

// GetTim (not in spec)
// r : Req | Act | Fin | InstVar
// ==> Time
TYPE_VC_Time VC::GetTim(const Record & r)
{
  switch(r.GetTag()) {
    case TAG_TYPE_VC_Req:
      return r.GetInt(pos_VC_Req_tim);
    case TAG_TYPE_VC_Act:
      return r.GetInt(pos_VC_Act_tim);
    case TAG_TYPE_VC_Fin:
      return r.GetInt(pos_VC_Fin_tim);
    case TAG_TYPE_VC_InstVar:
      return r.GetInt(pos_VC_InstVar_tim);
    default:
      RTERR::Error(L"VC::GetTim", RTERR_UNKNOWN_VALCOND_RECORD, Nil(), Nil(), Sequence());
  }
  return TYPE_VC_Time(); // dummy
}

// GetThrId (not in spec)
// r : Req | Act | Fin | InstVar
// ==> SCHDTP`ThreadId
TYPE_SCHDTP_ThreadId VC::GetThrId(const Record & r)
{
  switch(r.GetTag()) {
    case TAG_TYPE_VC_Req:
      return r.GetInt(pos_VC_Req_thrid);
    case TAG_TYPE_VC_Act:
      return r.GetInt(pos_VC_Act_thrid);
    case TAG_TYPE_VC_Fin:
      return r.GetInt(pos_VC_Fin_thrid);
    case TAG_TYPE_VC_InstVar:
      return r.GetInt(pos_VC_InstVar_thrid);
    default:
      RTERR::Error(L"VC::GetThrId", RTERR_UNKNOWN_VALCOND_RECORD, Nil(), Nil(), Sequence());
  }
  return TYPE_SCHDTP_ThreadId(); // dummy
}

