#include "evalclass.h"
#include "stackeval.h"
#include "pattern.h"
#include "libman.h"
#include "compile.h"
#include "astaux.h"
#include "rt_errmsg.h"
#include "evaldef.h"
#include "evalaux.h"
#include "settings.h"
#include "tbutils.h"
#ifdef VDMPP
#include "CPP.h"
#endif // VDMPP
#include "as2ascii.h"
#include <sstream>

#ifdef VDMPP
// GenInsMap
// nm : AS`Name
// ==> ()
void CLASS::GenInsMap(const TYPE_AS_Name & nm)
{
  SEQ<TYPE_AS_InstAssignDef> instvars (theState().GetInstVars(nm));

  MAP<TYPE_AS_Name, type_dUU3P> tmp_m;   // GLOBAL`ValueMap
  size_t len_instvars = instvars.Length();
  for (size_t i = 1; i <= len_instvars; i++) {
    TYPE_AS_InstAssignDef iad (instvars[i]);
    const TYPE_AS_AssignDef & ad  (iad.GetRecord(pos_AS_InstAssignDef_ad));
    const TYPE_AS_Access & access (iad.GetField(pos_AS_InstAssignDef_access));
    const TYPE_AS_Name & ins (ad.GetRecord(pos_AS_AssignDef_var));
    const TYPE_AS_Type & tp (ad.GetRecord(pos_AS_AssignDef_tp));

    tmp_m.ImpModify(ins, (Generic)mk_(sem_undef, tp, access));
  }

//  TYPE_SEM_InsStrct own;      // map AS`Name to GLOBAL`ValueMap
//  own.ImpModify(nm, tmp_m);

  SET<TYPE_AS_Name> allsupers (theState().GetAllSupers(nm));
  TYPE_SEM_InsStrct supers;   // map AS`Name to GLOBAL`ValueMap
  Generic cl;
  for (bool cc = allsupers.First(cl); cc; cc = allsupers.Next(cl)) {
    supers.ImpOverride(theState().GetInstInitVal(cl));
  }

//  supers.ImpOverride(own);
  supers.ImpModify(nm, tmp_m);
  theState().SetInstInitVal(nm, supers);
}
#endif // VDMPP

// Note that this operation is identical to InitGV in the MOD module!!!
// InitGV
// val_l : seq of AS`ValueDef
// ==> GLOBAL`ValueMap
TYPE_GLOBAL_ValueMap CLASS::InitGV(const SEQ<TYPE_AS_ValueDef> & val_l, const TYPE_AS_Name & nm)
{
  TYPE_GLOBAL_ValueMap res_m;

  if (! val_l.IsEmpty()) {
    theStackMachine().PushEmptyBlkEnv(sem_read_only);
    size_t len_val_l = val_l.Length();
    for (size_t i = 1; i <= len_val_l; i++) {
      const TYPE_AS_ValueDef & vd (val_l[i]);
      const TYPE_AS_Pattern & pat (vd.GetRecord(pos_AS_ValueDef_pat));
      const Generic & tp (vd.GetField(pos_AS_ValueDef_tp));
      const TYPE_AS_Expr & exp_e (vd.GetRecord(pos_AS_ValueDef_val));
#ifdef VDMPP
      const TYPE_AS_Access & access (vd.GetField(pos_AS_ValueDef_access));
#endif // VDMPP

//      Tuple euc (theStackMachine().EvalUninterruptedCmd(exp_e,
//                                                        TYPE_STKM_SubProgram(),
//                                                        TYPE_STKM_SubProgram(),
//                                                        SEQ<Char>(L"Init of Global Values")));
      
      AS2ASCII as2as;
      wostringstream wostr;
      as2as.Pattern2ASCII(pat, wostr);
      SEQ<Char> id (L"Init of Global Value: " + ASTAUX::ASName2String (nm) + L"`" + wostr.str());

      Tuple euc (theStackMachine().EvalUninterruptedCmd(exp_e, TYPE_STKM_SubProgram(), TYPE_STKM_SubProgram(), id));

      const TYPE_STKM_EvaluationState & eval_state (euc.GetRecord(1));

      if (!eval_state.Is(TAG_TYPE_STKM_Success)) {
        RTERR::InitError(L"InitGV", RTERR_INTERNAL_ERROR, Nil(), Nil(), ASTAUX::GetCid(exp_e), Sequence());
      }

      const TYPE_SEM_VAL & expr_v (euc.GetRecord(2));

      if (Settings.DTC()) {
        if ( !tp.IsNil() && !theState().SubType(expr_v, tp) ) {
          RTERR::InitError(L"InitGV", RTERR_TYPE_INCOMP, expr_v, tp, ASTAUX::GetCid(exp_e), Sequence());
        }
      }
      theStackMachine().SetCid(ASTAUX::GetCid(pat));
      SET<TYPE_SEM_BlkEnv> env_s (PAT::PatternMatch(theCompiler().P2P(pat), expr_v)); // one
      if ( !env_s.IsEmpty() ) {
        TYPE_SEM_BlkEnv env (env_s.GetElem());
        const MAP<TYPE_AS_Name,TYPE_SEM_ValTp> & id_m (env.GetMap(pos_SEM_BlkEnv_id_um));

        Set dom_id_m (id_m.Dom()); 
        Generic id;
        for (bool cc = dom_id_m.First(id); cc; cc = dom_id_m.Next(id)) {
          const TYPE_SEM_ValTp & valtp (id_m[id]);
          const TYPE_SEM_VAL & val (valtp.GetRecord(pos_SEM_ValTp_val));
          const Generic & vtp (valtp.GetField(pos_SEM_ValTp_tp));

          if (res_m.DomExists (id)) {
            vdm_iplog << L"  Value: \"" << ASTAUX::ASName2String (id) << L"\" overwrite previous definition" << endl << flush;
          }

#ifdef VDMSL
          type_dU2P t (mk_(val, vtp));
#endif // VDMSL
#ifdef VDMPP
          type_dUU3P t (mk_(val, vtp, DEF::RealAccess(access, DEF::ACC_INST)));
#endif // VDMPP

          res_m.ImpModify(id, t);

#ifdef VDMPP
          if (val.Is(TAG_TYPE_SEM_OBJ_uRef)) {
            theState().SetBindName(val, id);
          }
#endif // VDMPP
        }
        theStackMachine().AddToTopBlkEnv(env);
      }
      else {
        RTERR::InitError (L"InitGV", RTERR_EMPTY_ENV_S, Nil(), Nil(), ASTAUX::GetCid(pat), Sequence());
      }
    }
    theStackMachine().PopBlkEnv ();
  }
  return res_m;
}

#ifdef VDMPP
// TransHierarchy
// ==> ()
void CLASS::TransHierarchy()
{
  MAP<TYPE_AS_Name,TYPE_GLOBAL_SigmaClass> classes (theState().GetClasses());
  SET<TYPE_AS_Name> dom_classes (classes.Dom());

  Map clhchy;
  SET<TYPE_AS_Name> exp;
  Generic name;
  for (bool bb = dom_classes.First(name); bb; bb = dom_classes.Next(name)) {
    // Skip CPP classes if any
//    if(classes[name].Is(TAG_TYPE_CPP_Module)) continue;

    SET<TYPE_AS_Name> supers (theState().GetSupers( name ));
    clhchy.ImpModify(name, supers);
    if (supers.IsEmpty()) {
      exp.Insert( name );
    }
  }

  Map clhchy1 (ExpandHierarchy(clhchy, exp));
  if (OkHierarchy(clhchy1)) {
    theState().SetHchy(clhchy1);
  }
  else {
    RTERR::Error(L"TransHierarchy", RTERR_CIRC_CL_DEPENDENCY, Nil(), Nil(), Sequence());
  }
}

// ExpandHierarchy
// hchy1_p : GLOBAL`Hierarchy // map AS`Name to set of AS`Name
// donel : set of AS`Name
// -> GLOBAL`Hierarchy
Map CLASS::ExpandHierarchy(const Map & hchy1_p, const SET<TYPE_AS_Name> & done1)
{
  Map hchy_p (hchy1_p); // map AS`Name to set of AS`Name
  SET<TYPE_AS_Name> done(done1);

  bool stop(hchy_p.Dom() == done);

  while (!stop) {
    SET<TYPE_AS_Name> not_done (hchy_p.Dom());
    not_done.ImpDiff(done);

    SET<TYPE_AS_Name> thisSet (hchy_p.Dom());
    if (hchy_p.Dom() == done) {
      stop = true;
    }
    else {
      Map hchy_pp;
      Generic nm;
      for (bool bb = not_done.First(nm); bb; bb = not_done.Next(nm)) {
        SET<TYPE_AS_Name> tmp (ExpandNextLevel(hchy_p[nm], hchy_p, SET<TYPE_AS_Name>()));
        hchy_pp.ImpModify(nm, tmp);
      }

      SET<TYPE_AS_Name> newly_done (NewlyDone(hchy_pp, done));

      done.ImpUnion(newly_done);

      // not_done <-: hchy'
      for (bool cc = not_done.First(nm); cc; cc = not_done.Next(nm)) {
        if (hchy_p.DomExists(nm)) {
          hchy_p.RemElem(nm);
        }
      }
      hchy_p.ImpOverride(hchy_pp);
    }
  }
  return hchy_p;
}

// ExpandNextLevel
// to_exp : set of AS`Name
// hchy : GLOBAL`Hierarchy (map AS`Name to set of AS`Name)
// in_hchy : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> CLASS::ExpandNextLevel(const SET<TYPE_AS_Name> & to_exp,
                                         const Map & hchy,
                                         const SET<TYPE_AS_Name> & in_hchy)
{
  SET<TYPE_AS_Name> res;

  SET<TYPE_AS_Name> to_exp_q (to_exp);
  Generic nm;
  for (bool bb = to_exp_q.First(nm); bb; bb = to_exp_q.Next(nm)) {
    res.Insert(nm);
    res.ImpUnion(ExpCl(nm, hchy, in_hchy));
  }
  return res;
}

// ExpCl
// nm : AS`Name
// hchy_p : GLOBAL`Hierarchy (map AS`Name to set of AS`Name)
// in_hchy : set of AS`Name
// ==> set of AS`Name
SET<TYPE_AS_Name> CLASS::ExpCl( const TYPE_AS_Name & nm, const Map & hchy_p, const SET<TYPE_AS_Name> & in_hchy)
{
  if (hchy_p.DomExists(nm)) {
    if ( Set (hchy_p[nm]).IsEmpty()) {
      return in_hchy;
    }
    else {
      if (in_hchy.InSet(nm)) {
        vdm_iplog <<L"  Class Name: '" << ASTAUX::ASName2String (nm) << L"'" << endl << flush;
        RTERR::InitError(L"ExpCl", RTERR_CIRC_CL_DEPENDENCY, Nil(), Nil(), nm.GetInt(pos_AS_Name_cid), Sequence());
      }
      else {
        SET<TYPE_AS_Name> nextlevel(theState().GetSupers(nm));
        SET<TYPE_AS_Name> res;
        res.Insert(nm);
        SET<TYPE_AS_Name> in_hchy1 (in_hchy);
        in_hchy1.Insert(nm);

        res.ImpUnion(ExpandNextLevel(nextlevel, hchy_p, in_hchy1));

        return res;
      }
    }
  }
  else {
//    vdm_iplog << L"  Class Name: '" << ASTAUX::ASName2String (nm) << L"'" << endl << flush;
    RTERR::InitError(L"ExpCl", RTERR_CLNM_NOT_DEFINED, Nil(), Nil(), nm.GetInt(pos_AS_Name_cid),
                     Sequence().ImpAppend(AUX::SingleNameToString(nm)));
  }
  return SET<TYPE_AS_Name>(); // dummy
}

// NewlyDone
// hchy : GLOBAL`Hierarchy (map AS`Name to set of AS`Name)
// done : set of AS`Name
// -> set of AS`Name
SET<TYPE_AS_Name> CLASS::NewlyDone(const Map & hchy, const SET<TYPE_AS_Name> & done)
{
  Set dom_hchy (hchy.Dom());
  SET<TYPE_AS_Name> res;
  Generic nm;
  for (bool bb = dom_hchy.First(nm); bb; bb = dom_hchy.Next(nm)) {
    SET<TYPE_AS_Name> hchy_nm (hchy[nm]);
    bool forall = true;
    Generic cl;
    for (bool cc = hchy_nm.First(cl); cc && forall; cc = hchy_nm.Next(cl)) {
      forall = done.InSet(cl);
    }
    if (forall) {
      res.Insert(nm);
    }
  }
  return res;
}

// EvalInhStrct
// ==> ()
void CLASS::EvalInhStrct()
{
  Set classes (theState().GetClasses().Dom());

  Map local_inhstrct;
  Generic nm;
  for (bool bb = classes.First(nm); bb; bb = classes.Next(nm)) {
    local_inhstrct.ImpModify(nm, OrderOfProcess(nm));
  }
  theState().SetInhStrct(local_inhstrct);
}

// OkHierarchy
// clhchy : GLOBAL`Hierarchy (map AS`Name to set of AS`Name)
// -> bool
bool CLASS::OkHierarchy(const Map & clhchy)
{
  Set dom_clhchy (clhchy.Dom());
  bool forall = true;
  Generic cl;
  for (bool bb = dom_clhchy.First(cl); bb && forall; bb = dom_clhchy.Next(cl)) {
    forall = !Set(clhchy[cl]).InSet(cl);
  }
  return forall;
}

// OrderOfProcess
// nm : AS`Name
// -> seq of set of AS`Name
Sequence CLASS::OrderOfProcess(const TYPE_AS_Name & nm)
{
  SET<TYPE_AS_Name> supers (theState().GetSupers(nm));
  Sequence tmp;
  tmp.ImpAppend(Set());
  return OrderOfProcess_Aux(tmp, supers);
}

// OrderOfProcess_Aux
// order : seq of (set of AS`Name)
// to_process : set of AS`Name
// -> seq of set of AS`Name
Sequence CLASS::OrderOfProcess_Aux(const Sequence & order,
                                   const SET<TYPE_AS_Name> & to_process)
{
  if (to_process.IsEmpty()) {
    return order;
  }

  Sequence new_order;               // seq of (set of AS`Name)
  new_order.ImpAppend(to_process);

  size_t len = order.Length();
  for (size_t i = 1; i <= len; i++) {
    new_order.ImpAppend(Set(order[i]).ImpDiff(to_process));
  }

  SET<TYPE_AS_Name> to_process_q (to_process);
  SET<TYPE_AS_Name> supers;
  Generic sb;
  for (bool cc = to_process_q.First(sb); cc; cc = to_process_q.Next(sb)) {
    supers.ImpUnion(theState().GetSupers(sb));
  }

  return OrderOfProcess_Aux(new_order, supers);
}

// TransLocalHchy
// nm : AS`Name
// -> map AS`Name to set of AS`Name
Map CLASS::TransLocalHchy(const TYPE_AS_Name & nm)
{
  SET<TYPE_AS_Name> supers (theState().GetSupers(nm));

  Map res;
  res.ImpModify(nm, supers);

  Generic cl;
  for (bool bb = supers.First(cl); bb; bb = supers.Next(cl)) {
    res.ImpOverride(TransLocalHchy( cl ));
  }
  return res;
}

// CreateConstructor
// curcls : AS`Name
// instvars : seq of AS`InstAssignDef
// supercls : seq of AS`Name
// opm : map AS`Name to AS`OpDef
// ==> map seq of AS`Type to (STKM`DebugCmd * AS`Access)
Map CLASS::CreateConstructor(const TYPE_AS_Name & curcls,
                             const SEQ<TYPE_AS_InstAssignDef> & instvars,
                             const SEQ<TYPE_AS_Name> & supercls,
                             const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & opm)
{
  TYPE_STKM_SubProgram sp;

  // code for initialize super classes
  size_t len_supercls = supercls.Length();
  for (size_t icls = 1; icls <= len_supercls; icls++) {
    const TYPE_AS_Name & cls (supercls[icls]);
    sp.ImpAppend(TYPE_INSTRTP_PUSHCLNMCUROBJ().Init(cls, cls));
    sp.ImpAppend(TYPE_INSTRTP_INITCLASS().Init(cls, Int(0)));
    sp.ImpAppend(TYPE_INSTRTP_POPCLNMCUROBJ());
  }

  // code for initialize instance variables by default value
  size_t len_instvars = instvars.Length();
  for (size_t i = 1; i <= len_instvars; i++) {
    const TYPE_AS_InstAssignDef & iad (instvars[i]);
    const TYPE_AS_AssignDef & ad (iad.GetRecord(pos_AS_InstAssignDef_ad));
    bool stat (iad.GetBool(pos_AS_InstAssignDef_stat).GetValue());

    const Generic & Iinit (ad.GetField(pos_AS_AssignDef_dclinit));

    if (!Iinit.IsNil() && !stat) {
      const TYPE_AS_Name & nm (ad.GetRecord(pos_AS_AssignDef_var));
      const TYPE_AS_Type & tp (ad.GetRecord(pos_AS_AssignDef_tp));

      sp.ImpConc(theCompiler().E2I(Iinit));
      sp.ImpConc(theCompiler().SetContext(ASTAUX::GetCid(tp), false));
      sp.ImpAppend(TYPE_INSTRTP_DTC().Init(tp));
      sp.ImpAppend(TYPE_INSTRTP_BINDINSTVAR().Init(curcls,nm));
    }
  }

  Map constrmap;

  // code defined in constructor
  // call constructor as operation
  Set dom_opm (opm.Dom());
  Generic nm;
  for (bool dd = dom_opm.First(nm); dd; dd = dom_opm.Next(nm)) {
    const TYPE_AS_OpDef & opdef (opm[nm]);
    if (ASTAUX::GetConstr(opdef)) {
      SEQ<TYPE_AS_Type> tpl (ASTAUX::GetOpParms(opdef));
      TYPE_STKM_SubProgram thisSp (sp);
      thisSp.ImpAppend(TYPE_INSTRTP_LOOKUPOP().Init(ASTAUX::GetOpName(opdef)))
            .ImpAppend(TYPE_INSTRTP_SWAP())
            .ImpAppend(TYPE_INSTRTP_APPLY());
      if (tpl.IsEmpty()) {
        thisSp.ImpAppend(TYPE_INSTRTP_POP().Init(Int(1)));
      }
      thisSp.ImpAppend(TYPE_INSTRTP_EOCL());

      //constrmap.ImpModify(tpl, mk_(thisSp, ASTAUX::GetOpAccess(opdef)));
      constrmap.ImpModify(tpl, mk_(TYPE_STKM_DebugCmd().Init(thisSp), ASTAUX::GetOpAccess(opdef)));
    }
  }
  if (!constrmap.Dom().InSet(SEQ<TYPE_AS_Type>())) {
    // code for default constructor
    TYPE_STKM_SubProgram sp1;                          // 06.06.2001 M.Lebedev
    sp1.ImpAppend(TYPE_INSTRTP_POP().Init(Int(1)));    // 06.06.2001 M.Lebedev
    sp1.ImpConc(sp);                                   // 06.06.2001 M.Lebedev
    sp1.ImpAppend(TYPE_INSTRTP_EOCL());                // 06.06.2001 M.Lebedev sp -> sp1

    constrmap.Insert(SEQ<TYPE_AS_Type>(), mk_(TYPE_STKM_DebugCmd().Init(sp1), Int(PUBLIC_AS))); // default
  }
  return constrmap;
}

// ExtOpDom
// opdef : AS`OpDef
// ==> seq of AS`Type
SEQ<TYPE_AS_Type> CLASS::ExtOpDom(const TYPE_AS_OpDef & opdef)
{
  return ASTAUX::GetOpParms(opdef);
}

// TransSyncs
// cs : AS`Document (seq1 of AS`Class)
// ==> ()
void CLASS::TransSyncs(const TYPE_AS_Document & cs)
{
#ifdef VICE
  bool usesThreads = true;
#else
  bool usesThreads = false;
#endif // VDMPP
  Map class_m;  // map AS`Name to [AS`Definitions]

  size_t len_cs = cs.Length();
  for (size_t i = 1; i <= len_cs; i++) {
    if(!cs[i].Is(TAG_TYPE_CPP_Module)) {
      const TYPE_AS_Class & as_class (cs[i]);
      const Generic & def_g (as_class.GetField(pos_AS_Class_defs));
      class_m.Insert(as_class.GetRecord(pos_AS_Class_nm), def_g);

      if (!usesThreads && !def_g.IsNil()) {
        TYPE_AS_Definitions def (def_g);
        usesThreads = (!def.GetSequence(pos_AS_Definitions_syncs).IsEmpty() ||
                       !def.GetField(pos_AS_Definitions_threaddef).IsNil());
      }
    }
  }

  if (usesThreads) {
    SEQ<TYPE_AS_Name> order (GetPermissionOrder(cs));
    size_t len_order = order.Length();
    for (size_t i = 1; i <= len_order; i++) {
      const TYPE_AS_Name & clnm (order[i]);
      theCompiler().SetClMod(clnm);
      TransSyncsForOneClass(clnm, class_m[clnm]);
    }
    theStackMachine().SetUsesThreads(true);
  }
}

// GetPermissionOrder
// cs : AS`Document
// ==> seq of AS`Name
SEQ<TYPE_AS_Name> CLASS::GetPermissionOrder(const TYPE_AS_Document & cs)
{
  Map dep_m; // map AS`Name to set of AS`Name

  size_t len_cs = cs.Length();
  for (size_t i = 1; i <= len_cs; i++) {
    if(!cs[i].Is(TAG_TYPE_CPP_Module)) {
      const TYPE_AS_Class & as_class (cs[i]);
      dep_m.Insert(as_class.GetRecord(pos_AS_Class_nm), as_class.GetSequence(pos_AS_Class_supercls).Elems());
    }
  }

  SEQ<TYPE_AS_Name> res;
  while (! dep_m.IsEmpty()) {
    SET<TYPE_AS_Name> no_link;
    Map rem_m; // map AS`Name to set of AS`Name

    Set dom_dep_m (dep_m.Dom());
    Generic cl_nm;
    for (bool cc = dom_dep_m.First(cl_nm); cc; cc = dom_dep_m.Next(cl_nm)) {
      SET<TYPE_AS_Name> supercl_s (dep_m[cl_nm]);
      if (supercl_s.IsEmpty()) {
        no_link.Insert(cl_nm);
      }
      else {
        rem_m.ImpModify(cl_nm, supercl_s);
      }
    }

    Map new_m; // map AS`Name to set of AS`Name
    Set dom_rem_m (rem_m.Dom());
    for (bool dd = dom_rem_m.First(cl_nm); dd; dd = dom_rem_m.Next(cl_nm)) {
      SET<TYPE_AS_Name> supercl_s (dep_m[cl_nm]);
      supercl_s.ImpDiff(no_link);
      new_m.ImpModify(cl_nm, supercl_s);
    }

    res.ImpConc(set2seq(no_link));
    dep_m = new_m;
  }
  return res;
}

// set2seq
// elms : set of AS`Name
// -> seq of AS`Name
SEQ<TYPE_AS_Name> CLASS::set2seq(const SET<TYPE_AS_Name> & elms)
{
  return elms.ToSequence();
}

// TransSyncsForOneClass
// clnm : AS`Name
// defs_g : [AS`Definitions]
void CLASS::TransSyncsForOneClass(const TYPE_AS_Name & clnm, const Generic & defs_g)
{
  if (defs_g.IsNil()) {
    return;
  }

  TYPE_AS_Definitions defs (defs_g);
  const SET<TYPE_AS_Name> & dom_opm (defs.GetMap(pos_AS_Definitions_opm).Dom());
  const SEQ<TYPE_AS_SyncDef> & syncs (defs.GetSequence(pos_AS_Definitions_syncs));
  const Generic & thread (defs.GetField(pos_AS_Definitions_threaddef));

//  SET<TYPE_AS_Name> superops (theState().GetAllOpsNmsSupers(clnm));
//  superops.ImpDiff(theState().GetAllOps(clnm).Dom());
  SET<TYPE_AS_Name> superops (theState().GetAllOpsNmsSupers(clnm).ImpDiff(theState().GetAllOps(clnm).Dom()));

  TransThreadDef(clnm, thread);

  size_t len = syncs.Length();
  for (size_t i = 1; i <= len; i++) {
    TransSyncDef( clnm, syncs[i], dom_opm, superops);
  }

  Generic opnm;
  for (bool cc = superops.First(opnm); cc; cc = superops.Next(opnm) ) {
    theState().SetPermission(clnm, opnm, MergePermissionWithSupers(clnm, opnm, TYPE_STKM_SubProgram()));
  }
}

// GetInhThread
// clnm : AS`Name
// for PP
// ==> [STKM`SubProgram]
// for VICE
// ==> [STKM`SubProgram * [nat * nat * nat * nat]]
Generic CLASS::GetInhThread (const TYPE_AS_Name & clnm)
{
  Generic td (theState().GetThreadDef(clnm));

  if (!td.IsNil()) {
    return td;
  }
  SET<TYPE_AS_Name> supers (theState().GetInhCon(clnm));
  Set super_threads;
  Generic g;
  for (bool bb = supers.First(g); bb; bb = supers.Next(g)) {
    Generic s_td (theState().GetThreadDef(g));
    if (!s_td.IsNil()) {
      super_threads.Insert(s_td);
    }
  }

  switch(super_threads.Card()) {
    case 0: {
      return Nil();
    }
    case 1: {
      return super_threads.GetElem();
    }
    default: { // card super_threads > 1
      RTERR::InitError(L"GetInhThread", RTERR_MULT_THREAD_INH, Nil(), Nil(), clnm.get_cid(), Sequence());
      return Nil();
    }
  }
}

// TransThreadDef
// clnm : AS`Name
// Def : [AS`ThreadDef]
void CLASS::TransThreadDef(const TYPE_AS_Name & clnm, const Generic & Def)
{
  if (Def.IsNil()) {
    theState().SetThreadField( clnm, GetInhThread( clnm ));
  }
  else {
    switch(Record(Def).GetTag()) {
#ifdef VICE
      case TAG_TYPE_AS_PerObl: {
        theState().SetPerThreadDef( clnm, Def );
        break;
      }
      case TAG_TYPE_AS_SpoObl: {
        theState().SetSpoThreadDef( clnm, Def );
        break;
      }
#else
      case TAG_TYPE_AS_PerObl: {
        RTERR::InitError(L"TransThreadDef", RTERR_PERIODIC_THREAD_UNSUPPORTED, Nil(), Nil(), ASTAUX::GetCid(Def), Sequence());
        break;
      }
      case TAG_TYPE_AS_SpoObl: {
        RTERR::InitError(L"TransThreadDef", RTERR_SPORADIC_THREAD_UNSUPPORTED, Nil(), Nil(), ASTAUX::GetCid(Def), Sequence());
        break;
      }
#endif //VICE
      default: { // TYPE_AS_Stmt
        //theState().SetThreadDef(clnm, TYPE_STKM_ProcThread().Init(theCompiler().S2I(Def)));
        TYPE_STKM_SubProgram code;
        code.ImpAppend(TYPE_INSTRTP_PUSHCONTEXT().Init(Int(ALL)));
        code.ImpConc(theCompiler().S2I(Def));
        code.ImpAppend(TYPE_INSTRTP_POPCONTEXT());
        theState().SetThreadDef(clnm, TYPE_STKM_ProcThread().Init(code));
        break;
      }
    }
  }
}

// TransSyncDef
// nm : AS`Name
// Def : AS`SyncDef
// opnm_s : set of AS`Name
// supers : set of AS`Name
void CLASS::TransSyncDef(const TYPE_AS_Name & nm,
                         const TYPE_AS_SyncDef & Def,
                         const SET<TYPE_AS_Name> & opnm_s,
                         const SET<TYPE_AS_Name> & supers)
{
  switch(Def.GetTag()) {
    case TAG_TYPE_AS_Permission: { TransPermission(nm, Def, opnm_s, supers); break; }
    case TAG_TYPE_AS_Mutex:      { TransMutex(nm, Def, opnm_s); break; }
    default: break;
  }
}

// TransPermission
// clsnm : AS`Name
// perm : AS`Permission
// opnm_s : set of AS`Name
// super_ops : set of AS`Name
void CLASS::TransPermission(const TYPE_AS_Name & clsnm,
                            const TYPE_AS_Permission & perm,
                            const SET<TYPE_AS_Name> & opnm_s,
                            const SET<TYPE_AS_Name> & super_ops)
{
  const TYPE_AS_Name & spec (perm.GetRecord(pos_AS_Permission_spec));
  const TYPE_AS_Expr & guard (perm.GetRecord(pos_AS_Permission_guard));

#ifdef VICE
  bool old_compiling = theCompiler().GetCompilingTime();
  theCompiler().SetCompilingTime(false);
  SET<TYPE_AS_Name> tmp (opnm_s);
  tmp.ImpUnion(super_ops);
  theCompiler().AddDepIds(spec,tmp);
#endif //VICE
  TYPE_STKM_SubProgram guard_code (theCompiler().E2I(guard));
  InstallPermission(clsnm, spec, guard_code, opnm_s);
#ifdef VICE
  Tuple t (theCompiler().StopDepIds());
  const SET<TYPE_AS_Name> & nms (t.GetSet(1));
  const SET<TYPE_AS_Name> & histnms (t.GetSet(2));
  theState().AddHistDeps(clsnm, AUX::ConstructDoubleName(clsnm,spec), nms, histnms);
  theCompiler().SetCompilingTime(old_compiling);
#endif //VICE
}

// InstallPermission
// clnm : AS`Name
// opnm : AS`Name
// code : STKM`SubProgram
// local_ops : set of AS`Name
void CLASS::InstallPermission(const TYPE_AS_Name & clnm,
                              const TYPE_AS_Name & opnm,
                              const TYPE_STKM_SubProgram & code,
                              const SET<TYPE_AS_Name> & local_ops)
{
  Generic existing_codeg (theState().LookUpPermis(AUX::ConstructDoubleName(clnm, opnm)));

  TYPE_STKM_SubProgram res_code;
  if (! existing_codeg.IsNil()) {
    TYPE_STKM_SubProgram existing_code (existing_codeg);
    TYPE_STKM_SubProgram no_push_code (existing_code.SubSequence(2, existing_code.Length() -1));
    res_code = MergePermission(code, no_push_code);
  }
  else if (local_ops.InSet(opnm) ) {
    res_code = code;
  }
  else {
    res_code = MergePermissionWithSupers(clnm, opnm, code);
  }
  TYPE_STKM_SubProgram push_code;
  push_code.ImpAppend(TYPE_INSTRTP_PUSHCLNMCUROBJ().Init(clnm, clnm));
  push_code.ImpConc(res_code);
  push_code.ImpAppend(TYPE_INSTRTP_POPCLNMCUROBJ());
  theState().SetPermission(clnm, opnm, push_code);
}

// MergePermission
// prog1 : STKM`SubProgram
// prog2 : STKM`SubProgram
// ==> STKM`SubProgram
TYPE_STKM_SubProgram CLASS::MergePermission(const TYPE_STKM_SubProgram & prog1, const TYPE_STKM_SubProgram & prog2)
{
  if (prog1.IsEmpty()) {
    return prog2;
  }
  else if (prog2.IsEmpty()) {
    return prog1;
  }
  return theCompiler().ConcIfThenElse(prog1,
                                      prog2,
                                      TYPE_STKM_SubProgram().ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_false)));
}

// MergePermissionWithSupers
// clnm : AS`Name
// opnm : AS`Name
// prog : STKM`SubProgram
// ==> STKM`SubProgram
TYPE_STKM_SubProgram CLASS::MergePermissionWithSupers(const TYPE_AS_Name & clnm,
                                                      const TYPE_AS_Name & opnm,
                                                      const TYPE_STKM_SubProgram & prog)
{
  SET<TYPE_AS_Name> supers (theState().GetSupers(clnm));
  Set super_codes;
  Generic supernm;
  for (bool bb = supers.First(supernm); bb; bb = supers.Next(supernm)) {
    TYPE_AS_Name q_opnm (AUX::ConstructDoubleName( supernm, opnm ));
    Generic super_code (theState().LookUpPermis(q_opnm));
    if (!super_code.IsNil()) {
      super_codes.Insert(super_code);
    }
  }
  switch (super_codes.Card()) {
    case 0: {
      return prog;
    }
    case 1: {
      return MergePermission(prog, super_codes.GetElem());
    }
    default: {
      RTERR::Error(L"MergePermissionWithSupers", RTERR_OP_DEF_IN_MULTSUPERS, Nil(), Nil(), Sequence());
      return prog; // dummy
    }
  }
}

// TransMutex
// clnm : AS`Name
// mutex : AS`Mutex
// local_ops : set of AS`Name
void CLASS::TransMutex(const TYPE_AS_Name & clnm, const TYPE_AS_Mutex & mutex, const SET<TYPE_AS_Name> & local_ops)
{
#ifdef VICE
  bool old_compiling = theCompiler().GetCompilingTime();
  theCompiler().SetCompilingTime(false);
#endif //VICE

  const Generic & gops (mutex.GetField(pos_AS_Mutex_ops));

  SET<TYPE_AS_Name> all_ops;
  if (gops.IsNil()) {
    all_ops = theState().GetAllOpsNmsSupers(clnm);
  }
  else {
//    all_ops = (Generic)((Sequence) gops).Elems();
    all_ops.ImpUnion(Sequence(gops).Elems());
  }
// TODO: mutex with multiple operation names
/*
  TYPE_AS_BinaryExpr pred;
  pred.Init(TYPE_AS_ActiveExpr().Init(set2seq(all_ops), NilContextId),
            Int(EQ),
            TYPE_AS_RealLit().Init(Int(0), NilContextId),
            NilContextId);

  TYPE_STKM_SubProgram code (theCompiler().E2I(pred));

  Generic opnm;
  for (bool bb = all_ops.First(opnm); bb; bb = all_ops.Next(opnm)) {
    InstallPermission(clnm, opnm, code, local_ops);
  }
*/
  Generic opnm;
  for (bool bb = all_ops.First(opnm); bb; bb = all_ops.Next(opnm)) {
    SEQ<TYPE_AS_Name> ops;
    if (all_ops.Card() == 1 ) {
      ops.ImpAppend(all_ops.GetElem());
    }
    else {
// TODO
        ops.ImpConc(ASTAUX::SetToSequenceR(all_ops));
//        // same as spec
//        ops.ImpConc(ASTAUX::SetToSequenceR(Set(all_ops).RemElem(opnm)));
    }

    TYPE_AS_BinaryExpr pred;
    pred.Init(TYPE_AS_ActiveExpr().Init(ops, NilContextId),
              Int(EQ),
              TYPE_AS_RealLit().Init(Int(0), NilContextId),
              NilContextId);

    TYPE_STKM_SubProgram code (theCompiler().E2I(pred));
 
    InstallPermission(clnm, opnm, code, local_ops);
  }

#ifdef VICE
  theCompiler().SetCompilingTime(old_compiling);
#endif //VICE
}

// TransStaticRef
// classes : map AS`Name to GLOBAL`SIgmaClass
// -> map AS`Name to set of AS`Name
Map CLASS::TransStaticRef(const MAP<TYPE_AS_Name, TYPE_GLOBAL_SigmaClass> & classes)
{
  SET<TYPE_AS_Name> dom_classes (classes.Dom());
 
  MAP< TYPE_AS_Name, SET<TYPE_AS_Name> > sdep;
  SET<TYPE_AS_Name> exp;
  Generic name;
  for (bool bb = dom_classes.First(name); bb; bb = dom_classes.Next(name)) {
    SET<TYPE_AS_Name> clsnms;
    const SEQ<TYPE_AS_ValueDef> & vls (classes[name].GetSequence(pos_GLOBAL_SigmaClass_vls_udef));
    size_t len_vls = vls.Length();
    for (size_t i = 1; i <= len_vls; i++) {
      const TYPE_AS_Expr & val (vls[i].GetRecord(pos_AS_ValueDef_val));
      clsnms.ImpUnion(AUX::ClModNmInExpr(val)); 
    }

    const SEQ<TYPE_AS_InstAssignDef> & instvars (classes[name].GetSequence(pos_GLOBAL_SigmaClass_instvars));
    size_t len_instvars = instvars.Length();
    for (size_t j = 1; j <= len_instvars; j++) {
      const TYPE_AS_InstAssignDef & iad (instvars[j]);
//      if (iad.GetBoolValue(pos_AS_InstAssignDef_stat))
//      {
        const TYPE_AS_AssignDef & ad (iad.GetRecord(pos_AS_InstAssignDef_ad));
        const Generic & dclinit (ad.GetField(pos_AS_AssignDef_dclinit)); // [AS`Expr]
        if (!dclinit.IsNil()) {
          clsnms.ImpUnion(AUX::ClModNmInExpr(dclinit)); 
        }
//      }
    }

    const SEQ<TYPE_AS_InstanceInv> & invs (classes[name].GetTuple(pos_GLOBAL_SigmaClass_inst_uinv).GetSequence(1));
    size_t len_invs = invs.Length();
    for (size_t k = 1; k <= len_invs; k++) {
      const TYPE_AS_Expr & expr (invs[k].GetRecord(pos_AS_InstanceInv_expr));
      clsnms.ImpUnion(AUX::ClModNmInExpr(expr)); 
    }

    clsnms.ImpDiff(mk_set(name));
    clsnms.ImpDiff(classes[name].GetSet(pos_GLOBAL_SigmaClass_inhcon));
 
    sdep.ImpModify( name, clsnms );
    if (clsnms.IsEmpty()) {
      exp.Insert( name );
    }
  }

  return ExpandStaticRef(sdep, exp);
}
#endif // VDMPP

// ExpandStaticRef
// sdep : map AS`Name to set of AS`Name
// done1 : set of AS`Name
// -> map AS`Name to set of AS`Name
Map CLASS::ExpandStaticRef(const Map & sdep, const SET<TYPE_AS_Name> & done1)
{
  Map res (sdep);
  SET<TYPE_AS_Name> done (done1);
  SET<TYPE_AS_Name> not_done (sdep.Dom().ImpDiff(done));
  while(!not_done.IsEmpty()) {
    int64_t size = not_done.Card();
    Generic nm;
    for (bool bb = not_done.First(nm); bb; bb = not_done.Next(nm)) {
      SET<TYPE_AS_Name> s (res[nm]);
      bool all_done = true;
      SET<TYPE_AS_Name> tmp (res[nm]);
      Generic r_g;
      for (bool cc = s.First(r_g); cc && all_done; cc = s.Next(r_g)) {
        if (res.DomExists(r_g)) {
          all_done = done.InSet(r_g);
          tmp.ImpUnion(res[r_g]);
        }
        else {
          all_done = true;
        }
      }
      if (all_done) {
        res.ImpModify(nm, tmp);
        done.Insert(nm);
        not_done.RemElem(nm);
        break;
      }
    }
    if (size == not_done.Card()) {
      RTERR::Error(L"ExpandStaticRef", RTERR_CIRCULAR_STATIC_DEPENDENCY, Nil(), Nil(), Sequence());
    }
  }
  return res;
}

