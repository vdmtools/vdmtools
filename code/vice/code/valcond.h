#ifndef __valcond_h__
#define __valcond_h__

#include "metaiv.h"
#include "AS.h"
//#include "stackeval.h"
#include "TIMETRACETP.h"
#include "VC.h"

class VC
{
public:
  static void InitState ();
  static void LogInstVarChange (const TYPE_AS_Name & instnm,
                                const TYPE_SEM_VAL & val,
                                const TYPE_SEM_OBJ_uRef & objref,
                                const Int & thrid);
  static void AddInstVarLog (const Generic & nametup);
  static void AddInstDecl (const TYPE_AS_Name & sysnm, const TYPE_AS_Name & instnm, const TYPE_SEM_VAL & objref);
  static void AddOpReq (const TYPE_AS_Name & opnm, const Generic & vall, const Int & thrid, const Int & time);
  static void AddOpAct (const TYPE_AS_Name & opnm, const Int & thrid, const Int & time);
  static void AddOpFin (const TYPE_AS_Name & opnm, const Generic & res, const Int & thrid, const Int & time);
private:
  static void AddInstVarHist (const TYPE_AS_Name & nm, const TYPE_VC_InstVar & instvar);
  static Generic EvalValCond (const TYPE_VC_ValCond & vc);
  static Generic EvalDeadlineMet (const TYPE_VC_DeadlineMet & dm);
  static Generic MatchCheck (const TYPE_VC_EventExpr & ev1,
                             const Generic & pred,
                             const TYPE_VC_EventExpr & ev2,
                             const Int & delay,
                             const TYPE_VC_Kind & kind,
                             bool req); 
  static Generic AnyCheck (const TYPE_VC_EventExpr & ev1,
                           const Generic & pred,
                           const TYPE_VC_EventExpr & ev2,
                           const Int & delay,
                           const TYPE_VC_Kind & kind,
                           bool req);
  static bool Violation (const TYPE_VC_Time & t1,
                         const TYPE_VC_Time & t2,
                         const TYPE_VC_Time & delay,
                         const TYPE_VC_Kind & kind);
  static Generic EvalSeparate (const TYPE_VC_Separate & sep);
  static Generic EvalSeparateReq (const TYPE_VC_SeparateReq & sr);
  static Generic EvalPeriodic (const TYPE_VC_Periodic & per);
  static type_dL FindList (const TYPE_VC_EventExpr & ev);
  static bool PredSatisfied (const Generic & pred, const Int & time);
  static bool EvalOp (const TYPE_SEM_VAL & lhs, const Generic & op, const TYPE_SEM_VAL & rhs);
  static TYPE_AS_Name CombineNms (const TYPE_AS_Name & sysnm,
                                  const TYPE_AS_Name & statinsnm,
                                  const TYPE_AS_Name & instnm);
  static bool MatchArg (const Sequence & arg, const Int & index, const Record & ev, const SEQ<TYPE_VC_Req> & reqs);

  static TYPE_VC_Time GetTim(const Record & r);
  static TYPE_SCHDTP_ThreadId GetThrId(const Record & r);

private:
  static Generic loginstvars; // <ALL> | set of (AS`Name * AS`Name * AS`Name)
  static MAP<TYPE_AS_Name, TYPE_VC_OpHist> ophistmap;
  static MAP<TYPE_AS_Name, TYPE_VC_InstVarHist> instvarhistmap;
  static Map instdecl; // map SEM`OBJ_Ref to (AS`Name * AS`Name)
};

#endif // __valcond_h__
