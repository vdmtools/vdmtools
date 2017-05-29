/***
*  * WHAT
*  *    Functions for generating ParseTypeInfo. This must be generated
*  *    for all classes when they have been parsed.
*  * Specification;
*  *    vcm.cc: Implementation of vcm.vdm 1.60
*  * Id
*  *   $Id: vcm.cc,v 1.85 2006/06/13 07:02:12 vdmtools Exp $
***/

#include "statsem.h"
#include "astaux.h"
#ifdef VDMPP
#include "MANGLE.h"
#endif // VDMPP
#include "intconvquotes.h"

//wcout << L"intertp: " << INT2Q::h2gAS(intertp) << endl;

#ifdef VDMPP
#define OpDefAccess      Int(PRIVATE_AS)
#define FnDefAccess      Int(PRIVATE_AS)
#define InstvarDefAccess Int(PRIVATE_AS)
#define ValDefAccess     Int(PRIVATE_AS)
#define TypesDefAccess   Int(PRIVATE_AS)
#endif //VDMPP

#ifndef NOSS

// InitParseEnv
void StatSem::InitParseEnv()
{
#ifdef VDMSL
  this->ModuleEnv.Clear();
#endif // VDMSL
#ifdef VDMPP
  this->ParseEnv.Clear();
  this->classNames.Clear();
  this->clstypes.Clear();
#ifdef VICE
  TYPE_AS_Name cpu (ASTAUX::MkNameFromId(ASTAUX::MkId(L"CPU"), NilContextId));
  TYPE_AS_Name bus (ASTAUX::MkNameFromId(ASTAUX::MkId(L"BUS"), NilContextId));
  this->ParseEnv.ImpModify(cpu, CPUInfo());
  this->ParseEnv.ImpModify(bus, BUSInfo());
  this->classNames.Insert(cpu);
  this->classNames.Insert(bus);
#endif // VICE
#endif // VDMPP

// 20080612
  setRecMap(Map());
}

#ifdef VDMPP
// SetDefAccess
// acs : AS`Access
// df : AS`Access
// return : AS`Access
TYPE_AS_Access StatSem::SetDefAccess (const TYPE_AS_Access & acs, const TYPE_AS_Access & df) const
{
  if (acs == Int(DEFAULT_AS))
    return df;
  else
    return acs;
}
#endif // VDMPP

#ifdef VDMSL
// GetModuleRep
// nm : AS`Name
// ==> bool * ExpSigRep
Tuple StatSem::GetModuleRep (const TYPE_AS_Name & nm) const
{
  if (this->ModuleEnv.DomExists (nm))
    return mk_( Bool(true), this->ModuleEnv[nm] );
  else
    return mk_( Bool(false), mk_SSENV_ExpSigRep( Map(), Map(), Map(), Map(), Map(), Map(), Map() ) );
}

// GetModuleEnv
// ==> ModuleInfoEnv
MAP<TYPE_AS_Name, TYPE_SSENV_ExpSigRep> StatSem::GetModuleEnv () const
{
  return this->ModuleEnv;
}

// SetModuleEnv
void StatSem::SetModuleEnv (const MAP<TYPE_AS_Name, TYPE_SSENV_ExpSigRep> & _ModuleEnv)
{
  this->ModuleEnv = _ModuleEnv;
}

// UpdateModuleEnv
// mod : AS`Module | AS`DLModule
void StatSem::UpdateModuleEnv (const Record & mod)
{
  if (mod.Is (TAG_TYPE_AS_Module))
    UpdateStdModuleEnv(mod);
  else
    UpdateDLModuleEnv(mod);
}

// UpdateStdModuleEnv
// mod : AS`Module
void StatSem::UpdateStdModuleEnv (const TYPE_AS_Module & mod)
{
  const TYPE_AS_Name & modid     (mod.GetRecord(pos_AS_Module_nm));
  const TYPE_AS_Interface & intf (mod.GetRecord(pos_AS_Module_intf));
  const Generic & defs           (mod.GetField(pos_AS_Module_defs));

  const Generic & exp (intf.GetField(pos_AS_Interface_exp));
  const Map & imp     (intf.GetMap(pos_AS_Interface_imp)); // map Name to [AS`ImportSig]

  Map bind (FindRenamedTypes (modid, imp));

  if (!exp.IsNil ())
    ExportSome (modid, exp, defs, bind);
  else
    ExportAll (modid, defs, bind);
}

// ExportSome
// modid : AS`Name
// expsig : AS`ExportSig
// defs : AS`Definitions
// bind : map AS`Name to AS`Name
void StatSem::ExportSome (const TYPE_AS_Name & modid,
                          const TYPE_AS_ExportSig & expsig,
                          const TYPE_AS_Definitions & defs,
                          const MAP<TYPE_AS_Name, TYPE_AS_Name> & bind)
{
  const Map & tps (expsig.GetMap(pos_AS_ExportSig_tps));  // map AS`Name to [<STRUCT>]
  const Map & val (expsig.GetMap(pos_AS_ExportSig_val));  // map AS`Name to AS`Type
  const Map & fns (expsig.GetMap(pos_AS_ExportSig_fns));  // map AS`Name to (AS`TypeVarList * AS`FnType)
  const Map & ops (expsig.GetMap(pos_AS_ExportSig_ops));  // map AS`Name to AS`OpType

  const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & typem (defs.GetMap(pos_AS_Definitions_typem));
  const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & fnm     (defs.GetMap(pos_AS_Definitions_fnm));
  const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & opm     (defs.GetMap(pos_AS_Definitions_opm));

  const Generic & state (defs.GetField(pos_AS_Definitions_State)); // [ AS`StateDef ]

//  Map ntm, nfm, pfm, nop, monofns;
  MAP<TYPE_AS_Name, TYPE_AS_TypeDef> ntm;
  MAP<TYPE_AS_Name, TYPE_AS_FnDef> nfm;
  MAP<TYPE_AS_Name, TYPE_AS_FnDef> pfm;
  MAP<TYPE_AS_Name, TYPE_AS_OpDef> nop;
  MAP<TYPE_AS_Name, TYPE_AS_FnType> monofns;
  // tps : map AS`Name to [<STRUCT>]
  Set dom_typem (typem.Dom().ImpIntersect(tps.Dom()));
  Generic nm;
  for (bool bb = dom_typem.First(nm); bb; bb = dom_typem.Next(nm))
  {
    if (tps[nm] == (Int)STRUCT)
      ntm.Insert (nm, typem[nm]);
  }

  // fns : map AS`Name to (AS`TypeVarList * AS`FnType)
  Set dom_fnm_fns (fnm.Dom().ImpIntersect(fns.Dom()));
  for (bool cc = dom_fnm_fns.First(nm); cc; cc = dom_fnm_fns.Next(nm))
  {
    if (FindTypeVars (fnm[nm]).IsEmpty())
      nfm.Insert (nm, fnm[nm]);
    else
      pfm.Insert (nm, fnm[nm]);
  }

  // ops : map AS`Name to AS`OpType
  Set dom_opm (opm.Dom().ImpIntersect(ops.Dom()));
  for (bool dd = dom_opm.First(nm); dd; dd = dom_opm.Next(nm))
  {
    nop.Insert (nm, opm[nm]);
  }

  // fns : map AS`Name to (AS`TypeVarList * AS`FnType)
  // monofns : map AS`Name to AS`FnType
  Set dom_fns (fns.Dom());
  Set dom_fnm (fnm.Dom());
  for (bool ee = dom_fns.First(nm); ee; ee = dom_fns.Next(nm))
  {
    // (nm in set dom fnm) => FindTypeVars(fnm(nm)) = []
    if (!dom_fnm.InSet(nm) || FindTypeVars(fnm[nm]).IsEmpty())
    {
      monofns.Insert (nm, Tuple (fns[nm]).GetRecord (2)); // AS`FnType
    }
  }

  //
  {
    Map allfns (ExportInvs (modid, ntm));
    allfns.ImpOverride (ExportSimple (modid, monofns));
    allfns.ImpOverride (ExportPrePostFns (modid, nfm));
    allfns.ImpOverride (ExportPrePostOps (modid, nop, state));

    Tuple infer (ExportSomeTypes (modid, tps, typem, state));
    const Map & exptps (infer.GetMap (1)); // map AS`Name to REP`TypeRep
    const Map & exptag (infer.GetMap (2)); // map AS`Name to seq of REP`FieldRep

    Map expval (ExportSimple (modid, val));
    Map polyfns (ExportPolyFns(modid, pfm));
    Map expops (ExportSimple (modid, ops));

    Set dom_fnm (fnm.Dom());
    Map fnmq;
    Generic x;
    for (bool ff = dom_fnm.First(x); ff; ff = dom_fnm.Next(x))
    {
      fnmq.ImpModify(ASTAUX::Combine2Names(modid, x), fnm[x]);
    }

    TYPE_SSENV_ExpSigRep newexp ( mk_SSENV_ExpSigRep( exptps, expval, allfns, polyfns, expops, exptag, fnmq ) );
    TYPE_SSENV_ExpSigRep finalexp ( UseRenamedTypes ( newexp, bind ) );

    this->ModuleEnv.ImpModify (modid, finalexp);
  }
}

// UpdateDLModuleEnv
// mod : AS`DLModule
void StatSem::UpdateDLModuleEnv (const TYPE_AS_DLModule & mod)
{
  const TYPE_AS_Name & modid (mod.GetRecord(pos_AS_DLModule_nm));
  const TYPE_AS_DLInterface & intf (mod.GetRecord(pos_AS_DLModule_intf));
  const TYPE_AS_DLExportSig & exp (intf.GetRecord(pos_AS_DLInterface_exp));
  this->ModuleEnv.ImpModify (modid, ExportDLModule(modid,exp));
}

// ExportAll
// modid : AS`Name
// defs_0 : AS`Definitions
// bind : map AS`Name to AS`Name
void StatSem::ExportAll (const TYPE_AS_Name & modid,
                         const TYPE_AS_Definitions & defs,
                         const MAP<TYPE_AS_Name, TYPE_AS_Name> & bind)
{
  const Map & typem       (defs.GetMap(pos_AS_Definitions_typem));
  const Map & fnm         (defs.GetMap(pos_AS_Definitions_fnm));
  const Map & opm         (defs.GetMap(pos_AS_Definitions_opm));
  const Generic & state   (defs.GetField(pos_AS_Definitions_State));
  const Sequence & valuem (defs.GetSequence(pos_AS_Definitions_valuem));

  Map nfm, pfm;
  Set dom_fnm (fnm.Dom());
  Generic nm;
  for (bool bb = dom_fnm.First(nm); bb; bb = dom_fnm.Next(nm))
  {
    if (FindTypeVars(fnm[nm]).IsEmpty())
      nfm.ImpModify (nm, fnm[nm]);
    else
      pfm.ImpModify (nm, fnm[nm]);
  }

  {
    Map allfns (ExportInvs (modid, typem));
    allfns.ImpOverride (ExportFunctions (modid, nfm));
    allfns.ImpOverride (ExportPrePostFns (modid, nfm));
    allfns.ImpOverride (ExportPrePostOps (modid, opm, state));

    Tuple infer (ExportTypes (modid, typem, state));
    const Map & tps (infer.GetMap (1)); // map AS`Name to REP`TypeRep
    const Map & tag (infer.GetMap (2)); // map AS`Name to seq of REP`FieldRep

    Map val (ExportValues (modid, valuem));
    Map polyfns (ExportPolyFns (modid, pfm));
    Map ops (ExportOperations (modid, opm));

    Map fnmq;
    Generic x;
    for (bool cc = dom_fnm.First(x); cc; cc = dom_fnm.Next(x))
    {
      fnmq.ImpModify(ASTAUX::Combine2Names(modid, x), fnm[x]);
    }

    TYPE_SSENV_ExpSigRep newexp ( mk_SSENV_ExpSigRep( tps, val, allfns, polyfns, ops, tag, fnmq ) );
    TYPE_SSENV_ExpSigRep finalexp ( UseRenamedTypes ( newexp, bind ) );

    this->ModuleEnv.ImpModify (modid, finalexp);
  }
}

// ExportTypes
// modid : [AS`Name]
// typem : map AS`Name to AS`TypeDef
// state : [AS`StateDef]
// return : map AS`Name to REP`TypeRep,
//          map AS`Name to seq of REP`FieldRep
Tuple StatSem::ExportTypes (const Generic & modid,
                            const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & typem,
                            const Generic & state)
{
  Set dom_typem (typem.Dom());
  Map tp_m;
  Generic nm;
  for (bool bb = dom_typem.First(nm); bb; bb = dom_typem.Next(nm))
  {
    TYPE_AS_TypeDef td (typem[nm]);
    const TYPE_AS_Type & shape (td.GetRecord(pos_AS_TypeDef_shape));
    const Generic & inv        (td.GetField(pos_AS_TypeDef_Inv));

    if (modid.IsNil ())
      tp_m.ImpModify (nm, TransTypeDef (modid, nm, shape, inv));
    else
      tp_m.ImpModify (ExtName (modid, nm), TransTypeDef (modid, nm, shape, inv));
  }

  Map st_m;
  if (!state.IsNil ())
  {
    TYPE_AS_StateDef st (state);
    const TYPE_AS_CompositeType & tp (st.GetRecord(pos_AS_StateDef_tp));
    const Generic & inv (st.GetField(pos_AS_StateDef_Inv));
    const TYPE_AS_Name & name (tp.GetRecord(pos_AS_CompositeType_name));

    if (modid.IsNil ())
      st_m.Insert (tp.GetRecord(pos_AS_CompositeType_name), TransTypeDef (modid, name, tp, inv));
    else
      st_m.Insert (ExtName (modid, tp.GetRecord(pos_AS_CompositeType_name)), TransTypeDef (modid, name, tp, inv));
  }

  if (!state.IsNil ())
  {
    TYPE_AS_StateDef st (state);
    const TYPE_AS_CompositeType & tp (st.GetRecord(pos_AS_StateDef_tp));
    const TYPE_AS_Name & name (tp.GetRecord(pos_AS_CompositeType_name));
    if (tp_m.DomExists (name) && !(tp_m[name] == st_m[name]))
      //----------------------------------------------------------
      // Error message #341
      // Type of state not compatible with similar type definition
      //----------------------------------------------------------
      GenErr (name, WRN1,341, Sequence());
  }

  Map tagenv;
  if (!state.IsNil ())
  {
    TYPE_AS_StateDef st (state);
    const TYPE_AS_CompositeType & tp (st.GetRecord(pos_AS_StateDef_tp));

    if (modid.IsNil ())
      tagenv.Insert (tp.GetRecord(pos_AS_CompositeType_name),
                     TransFields (modid, tp.GetSequence(pos_AS_CompositeType_fields)));
    else
      tagenv.Insert (ExtName (modid, tp.GetRecord(pos_AS_CompositeType_name)),
                     TransFields (modid, tp.GetSequence(pos_AS_CompositeType_fields)));
  }

  for (bool cc = dom_typem.First(nm); cc; cc = dom_typem.Next(nm))
  {
    TYPE_AS_TypeDef td (typem[nm]);
    const TYPE_AS_Type & shape (td.GetRecord(pos_AS_TypeDef_shape));
    tagenv.ImpOverride (ColTags (modid, shape));
  }

  st_m.ImpOverride (tp_m);
  return mk_(st_m, tagenv);
}
#endif // VDMSL

#ifdef VDMPP
// ExportTypes
// cls : AS`Name
// typem : map AS`Name to AS`TypeDef
// return : map AS`Name to ENV`AccessTypeRep,
//          map AS`Name to ENV`AccessFieldRep
Tuple StatSem::ExportTypes (const TYPE_AS_Name & cls, const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & typem)
{
//wcout << INT2Q::h2gAS(typem) << endl;
  Set dom_typem (typem.Dom());
  Map tp_m;

  Generic nm;
  for (bool bb = dom_typem.First(nm); bb; bb = dom_typem.Next(nm))
  {
    TYPE_AS_TypeDef td (typem[nm]);
    const TYPE_AS_Type & shape (td.GetRecord(pos_AS_TypeDef_shape));
    const Generic & inv        (td.GetField(pos_AS_TypeDef_Inv));

    tp_m.ImpModify (ExtName(cls, nm),
                    mk_SSENV_AccessTypeRep(
                       TransTypeDef (cls, nm, shape, inv),
                       SetDefAccess (td.GetInt(pos_AS_TypeDef_access), TypesDefAccess),
                       Bool(true)));

// TODO: search inner compose type more
// 20091128 -->
    if (shape.Is(TAG_TYPE_AS_CompositeType))
    {
// 20141202 -->
      TYPE_AS_Name tnm (ExtName(cls, nm));
      //TYPE_REP_CompositeTypeRep ctr (TransTypeDef (cls, nm, shape, inv));
      //const TYPE_AS_Name & cnm (ctr.GetRecord(pos_REP_CompositeTypeRep_nm));
      const TYPE_AS_Name & lnm (shape.GetRecord(pos_AS_CompositeType_name));
      TYPE_AS_Name cnm (ExtName(cls, lnm));
      TYPE_REP_CompositeTypeRep ctr (TransTypeDef (cls, lnm, shape, inv));
      if (tnm != cnm)
      {
        if (!tp_m.DomExists(cnm))
          tp_m.ImpModify (cnm, mk_SSENV_AccessTypeRep(ctr, Int(PRIVATE_AS), Bool(true)));
        //else
        //  wcout << ctr << endl; 
      }
// <-- 20141204
    }
// <-- 20091128
  }

  Map tagenv;
  for (bool cc = dom_typem.First(nm); cc; cc = dom_typem.Next(nm))
  {
    TYPE_AS_TypeDef td (typem[nm]);
    Map ct_m (ColTags (cls, td.GetRecord(pos_AS_TypeDef_shape)));
    Int acs_q (SetDefAccess (td.GetInt(pos_AS_TypeDef_access), TypesDefAccess));

    Set dom_ct_m (ct_m.Dom());
    Generic n;
    Map tagenvtmp;
    for (bool dd = dom_ct_m.First (n); dd; dd = dom_ct_m.Next (n))
    {
      tagenvtmp.Insert (n, mk_SSENV_AccessFieldRep(ct_m[n], acs_q));
    }
    tagenv.ImpOverride (tagenvtmp);
  }
//wcout << INT2Q::h2gAS(mk_(tp_m, tagenv)) << endl;
  return mk_(tp_m, tagenv);
}
// ExportInstVars
// cls : AS`Name
// instvars : seq of AS`InstanceVarDef
// return : map AS`Name to ENV`AccessTypeRep * bool
Tuple StatSem::ExportInstVars (const TYPE_AS_Name & cls, const SEQ<TYPE_AS_InstanceVarDef> & instvars)
{
  Map vars;
  bool instinv = false;
  size_t len_instvars = instvars.Length();
  for (size_t idx = 1; idx <= len_instvars; idx++)
  {
    const TYPE_AS_InstanceVarDef & ivd (instvars[idx]);
    switch (ivd.GetTag()) {
      case TAG_TYPE_AS_InstAssignDef: {
        const TYPE_AS_AssignDef & asgdf (ivd.GetRecord(pos_AS_InstAssignDef_ad));
        const TYPE_AS_Access & acs      (ivd.GetField(pos_AS_InstAssignDef_access));
        const Bool & stat               (ivd.GetBool(pos_AS_InstAssignDef_stat));
        const TYPE_AS_Name & var        (asgdf.GetRecord(pos_AS_AssignDef_var));
        const TYPE_AS_Type & tp         (asgdf.GetRecord(pos_AS_AssignDef_tp));

        TYPE_AS_Access acs_q (SetDefAccess (acs, InstvarDefAccess));
        vars.Insert (ExtName (cls,var), mk_SSENV_AccessTypeRep( TransType (cls, tp), acs_q, stat ) );
        break;
      }
      case TAG_TYPE_AS_InstanceInv: {
        instinv = true;
        break;
      }
    }
  }
  return mk_(vars, Bool(instinv));
}

// ExportThread
// thread : [AS`ThreadDef]
// return : <NONE> | <PER> | <DECL>
int StatSem::ExportThread(const Generic & thread)
{
  if (thread.IsNil())
    return NONE;
  else if (TYPE_AS_ThreadDef(thread).Is(TAG_TYPE_AS_PerObl))
    return PER;
  else
    return DECL;
}
#endif // VDMPP

// SetName
// modid : [AS`Name]
// nm : AS`Name
// return : AS`Name
TYPE_AS_Name StatSem::SetName (const Generic & modid, const TYPE_AS_Name & nm)
{
  if (modid.IsNil ())
    return nm;
  else
    return ExtName (modid, nm);
}

#ifdef VDMSL
// MakeTpRng
// t : REP`TypeRep
// - : AS`Access
// - : bool
// +> REP`TypeRep
Record StatSem::MakeTpRng (const TYPE_REP_TypeRep & t, const TYPE_AS_Access &, const Bool &)
{
  return t;
}

// MakeFnRng
// f : REP`FnTypeRep
// - : AS`Access
// - : bool
// +> REP`FnTypeRep
Record StatSem::MakeFnRng (const TYPE_REP_FnTypeRep & f, const TYPE_AS_Access &, const Bool &)
{
  return f;
}

// MakeOpRng
// f : REP`OpTypeRep
// - : AS`Access
// - : bool
// +> REP`OpTypeRep
Record StatSem::MakeOpRng (const TYPE_REP_OpTypeRep & o, const TYPE_AS_Access &, const Bool &, const Bool &)
{
  return o;
}

// MakePolyRng
// f : REP`PolyTypeRep
// - : AS`Access
// - : bool
// +> REP`PolyTypeRep
Record StatSem::MakePolyRng (const TYPE_REP_PolyTypeRep & p, const TYPE_AS_Access &, const Bool &)
{
  return p;
}
#endif // VDMSL

#ifdef VDMPP
// MakeTpRng
// t : REP`TypeRep
// accs : AS`Access
// stat : bool
// +> ENV`AccessTypeRep
Record StatSem::MakeTpRng (const TYPE_REP_TypeRep & t, const TYPE_AS_Access & accs, const Bool & stat)
{
  return mk_SSENV_AccessTypeRep( t, SetDefAccess (accs, ValDefAccess), stat );
}

// MakeFnRng
// t : REP`FnTypeRep
// accs : AS`Access
// stat : bool
// +> ENV`AccessFnTypeRep
Record StatSem::MakeFnRng (const TYPE_REP_FnTypeRep & f, const TYPE_AS_Access & accs, const Bool & stat)
{
  return mk_SSENV_AccessFnTypeRep( f, SetDefAccess (accs, FnDefAccess), stat );
}

// MakeOpRng
// t : REP`OpTypeRep
// accs : AS`Access
// stat : bool
// +> ENV`AccessOpTypeRep
Record StatSem::MakeOpRng (const TYPE_REP_OpTypeRep & o, const TYPE_AS_Access & accs, const Bool & stat,
                           const Bool & oppure)
{
  return mk_SSENV_AccessOpTypeRep( o, SetDefAccess (accs, OpDefAccess), stat, oppure );
}

// MakePolyRng
// t : REP`PolyTypeRep
// accs : AS`Access
// stat : bool
// +> ENV`AccessPolyTypeRep
Record StatSem::MakePolyRng (const TYPE_REP_PolyTypeRep & p, const TYPE_AS_Access & accs, const Bool & stat)
{
  return mk_SSENV_AccessPolyTypeRep( p, SetDefAccess (accs, FnDefAccess), stat );
}
#endif // VDMPP

// ExportValues
// modid : [AS`Name]
// valuem : seq of AS`ValueDef
// ==> map AS`Name to REP`TypeRep
Map StatSem::ExportValues (const Generic & modid, const SEQ<TYPE_AS_ValueDef> & valuem)
{
  Map val;
  size_t len_valuem = valuem.Length();
  for (size_t idx = 1; idx <= len_valuem; idx++)
  {
    const TYPE_AS_ValueDef & vd (valuem[idx]);
    const TYPE_AS_Pattern & pat (vd.GetRecord(pos_AS_ValueDef_pat));
    const Generic & tp          (vd.GetField(pos_AS_ValueDef_tp));
    const TYPE_AS_Access & acs  (vd.GetField(pos_AS_ValueDef_access));

    if (tp.IsNil ())
    {
#ifdef VDMSL
      //---------------------------------------------
      // Error message #342
      // All values in export section should be typed
      //---------------------------------------------
      GenErr (pat, ERR, 342, Sequence());
#endif // VDMSL

      MAP<TYPE_AS_Name,Tuple> newbd (ExtractBindings (pat));
      Set dom_newbd (newbd.Dom());
      Generic nm;
      for (bool bb = dom_newbd.First (nm); bb; bb = dom_newbd.Next (nm))
        val.ImpModify (SetName(modid, nm), MakeTpRng (newbd[nm].GetRecord(1), acs, Bool(true)));
    }
    else
    {
      Tuple infer (wf_Pattern (POS, pat, TransType (modid, tp)));
      const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap (2));
      Set dom_bd (bd.Dom());
      Generic nm;
      for (bool bb = dom_bd.First(nm); bb; bb = dom_bd.Next(nm))
        val.ImpModify (SetName (modid, nm), MakeTpRng (bd[nm].GetRecord(1), acs, Bool(true)));
    }
  }
  return val;
}

// ExportFunctions
// modid : [AS`Name]
// fnm : map AS`Name to AS`FnDef
// ==> map AS`Name to REP`FnTypeRep
Map StatSem::ExportFunctions (const Generic & modid, const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & fnm)
{
  Set dom_fnm (fnm.Dom());
  MAP<TYPE_AS_Name, TYPE_REP_FnTypeRep> fns;
  Generic nm;
  for (bool bb = dom_fnm.First(nm); bb; bb = dom_fnm.Next(nm))
  {
#ifdef VDMPP
    if (!MANGLE::IsMangled(nm))
#endif //VDMPP
    {
      TYPE_AS_FnDef fndef (fnm[nm]);
      switch(fndef.GetTag()) {
        case TAG_TYPE_AS_ExplFnDef: {
          fns.ImpModify (SetName (modid, nm),
                         MakeFnRng (ExportExplicitFunction (modid, fndef),
                                    fndef.GetField(pos_AS_ExplFnDef_access),
                                    fndef.GetBool(pos_AS_ExplFnDef_stat)));
          break;
        }
        case TAG_TYPE_AS_ImplFnDef: {
          fns.ImpModify (SetName (modid, nm),
                         MakeFnRng (ExportImplicitFunction (modid, fndef),
                                    fndef.GetField(pos_AS_ImplFnDef_access),
                                    fndef.GetBool(pos_AS_ImplFnDef_stat)));
          break;
        }
        case TAG_TYPE_AS_ExtExplFnDef: {
          fns.ImpModify (SetName (modid, nm),
                         MakeFnRng (ExportExtExplicitFunction (modid, fndef),
                                    fndef.GetField(pos_AS_ExtExplFnDef_access),
                                    fndef.GetBool(pos_AS_ExtExplFnDef_stat)));
          break;
        }
        default:
          break;
      }
    }
  }
  return fns;
}

#ifdef VDMPP
// ExportOverloads
// p_nm : AS`Name
// p_opm : map AS`Name to AS`OpDef
// p_fnm : map AS`Name to AS`FnDef
// -> map AS`Name to set of (ENV`AccessOpTypeRep | ENV`AccessFnTypeRep | ENV`AccessPolyTypeRep)
Map StatSem::ExportOverloads(const TYPE_AS_Name& p_nm,
                             const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & p_opm,
                             const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & p_fnm)
{
  Map l_ops (MakeOverloadedMap(p_nm, p_opm, true)); // map AS`Name to set of (ENV`AccessOpTypeRep | ENV`AccessFnTypeRep)
  Map l_fns (MakeOverloadedMap(p_nm, p_fnm, false)); // map AS`Name to set of (ENV`AccessOpTypeRep | ENV`AccessFnTypeRep)
  Set wholeDom (l_ops.Dom());      // set of AS`Name
  wholeDom.ImpUnion(l_fns.Dom());

  Set opDom (l_ops.Dom());         // set of AS`Name
  opDom.ImpDiff(l_fns.Dom());

  Set fnDom (l_fns.Dom());         // set of AS`Name
  fnDom.ImpDiff(l_ops.Dom());

  Map result;
  Generic b_nm;
  for (bool bb = wholeDom.First(b_nm); bb; bb = wholeDom.Next(b_nm))
  {
    if (opDom.InSet(b_nm))
      result.Insert(SetName(p_nm, b_nm), l_ops[b_nm]);
    else if (fnDom.InSet(b_nm))
      result.Insert(SetName(p_nm, b_nm), l_fns[b_nm]);
    else
      result.Insert(SetName(p_nm, b_nm), Set(l_ops[b_nm]).ImpUnion(Set(l_fns[b_nm])));
  }
  return result;
}

// MakeOverloadedMap
// p_name : AS`Name
// p_methmp : ((map AS`Name to AS`OpDef) | (map AS`Name to AS`FnDef))
// p_isop :  bool
// ==> map AS`Name to set of (ENV`AccessOpTypeRep | ENV`AccessFnTypeRep | ENV`AccessPolyTypeRep)
Map StatSem::MakeOverloadedMap(const TYPE_AS_Name & p_nm, const MAP<TYPE_AS_Name, Record> & p_methmp, bool p_isop)
{
  MAP<TYPE_AS_Name, Set> l_resMap; // map AS`Name to set of (ENV`AccessOpTypeRep | ENV`AccessFnTypeRep | ENV`AccessPolyTypeRep)

  // It is not necessary to explicitly build l_mangledNames as we do it on the fly

  Set dom_p_methmp (p_methmp.Dom());
  Generic b_nm;
  for (bool bb = dom_p_methmp.First(b_nm); bb; bb = dom_p_methmp.Next(b_nm))
  {
    if (MANGLE::IsMangled(b_nm))
    {
      Record l_meth (p_methmp[b_nm]); // AS`OpDef | AS`FnDef

 // (seq of REP`TypeRep) * REP`TypeRep
      Tuple l_methp (p_isop ? GetOperationSignature(l_meth) : GetFunctionSignature(l_meth));

      TYPE_AS_Name l_nm (MANGLE::GetUnmangledName(b_nm));

      Set l_resMap_l_nm (l_resMap.DomExists(l_nm) ? l_resMap[l_nm] : Set());

      if (p_isop)
      {
        switch(l_meth.GetTag()) {
          case TAG_TYPE_AS_ExplOpDef: {
            l_resMap_l_nm.Insert(MakeOpRng(mk_REP_OpTypeRep(l_methp.GetField(1), l_methp.GetField(2)),
                                           l_meth.GetField(pos_AS_ExplOpDef_access),
                                           l_meth.GetBool(pos_AS_ExplOpDef_stat),
                                           l_meth.GetBool(pos_AS_ExplOpDef_oppure)));
            break;
          }
          case TAG_TYPE_AS_ImplOpDef: {
            l_resMap_l_nm.Insert(MakeOpRng(mk_REP_OpTypeRep(l_methp.GetField(1), l_methp.GetField(2)),
                                           l_meth.GetField(pos_AS_ImplOpDef_access),
                                           l_meth.GetBool(pos_AS_ImplOpDef_stat),
                                           l_meth.GetBool(pos_AS_ImplOpDef_oppure)));
            break;
          }
          case TAG_TYPE_AS_ExtExplOpDef: {
            l_resMap_l_nm.Insert(MakeOpRng(mk_REP_OpTypeRep(l_methp.GetField(1), l_methp.GetField(2)),
                                           l_meth.GetField(pos_AS_ExtExplOpDef_access),
                                           l_meth.GetBool(pos_AS_ExtExplOpDef_stat),
                                           l_meth.GetBool(pos_AS_ExtExplOpDef_oppure)));
            break;
          }
        }
      }
      else
      {
        Generic fntype;
        if (l_meth.Is(TAG_TYPE_AS_ExplFnDef) && l_meth.GetRecord(pos_AS_ExplFnDef_tp).Is(TAG_TYPE_AS_PartialFnType))
          fntype = mk_REP_PartialFnTypeRep(l_methp.GetField(1), l_methp.GetField(2));
        else
          fntype = mk_REP_TotalFnTypeRep(l_methp.GetField(1), l_methp.GetField(2));
        SEQ<TYPE_AS_TypeVar> tv_l (FindTypeVars(l_meth));
        if (tv_l.IsEmpty())
        {
          switch (l_meth.GetTag()) {
            case TAG_TYPE_AS_ExplFnDef: {
                l_resMap_l_nm.Insert(MakeFnRng(fntype,
                                               l_meth.GetField(pos_AS_ExplFnDef_access),
                                               l_meth.GetBool(pos_AS_ExplFnDef_stat)));
              break;
            }
            case TAG_TYPE_AS_ImplFnDef: {
                l_resMap_l_nm.Insert(MakeFnRng(fntype,
                                               l_meth.GetField(pos_AS_ImplFnDef_access),
                                               l_meth.GetBool(pos_AS_ImplFnDef_stat)));
              break;
            }
            case TAG_TYPE_AS_ExtExplFnDef: {
                l_resMap_l_nm.Insert(MakeFnRng(fntype,
                                               l_meth.GetField(pos_AS_ExtExplFnDef_access),
                                               l_meth.GetBool(pos_AS_ExtExplFnDef_stat)));
              break;
            }
          }
        }
        else
        {
          SEQ<TYPE_REP_TypeRep> r_l (TransTypeList (p_nm, tv_l));
          switch (l_meth.GetTag()) {
            case TAG_TYPE_AS_ExplFnDef: {
                l_resMap_l_nm.Insert(MakePolyRng(mk_REP_PolyTypeRep(r_l, fntype),
                                                 l_meth.GetField(pos_AS_ExplFnDef_access),
                                                 l_meth.GetBool(pos_AS_ExplFnDef_stat)));
              break;
            }
            case TAG_TYPE_AS_ImplFnDef: {
                l_resMap_l_nm.Insert(MakePolyRng(mk_REP_PolyTypeRep(r_l, fntype),
                                                 l_meth.GetField(pos_AS_ImplFnDef_access),
                                                 l_meth.GetBool(pos_AS_ImplFnDef_stat)));
              break;
            }
            case TAG_TYPE_AS_ExtExplFnDef: {
                l_resMap_l_nm.Insert(MakePolyRng(mk_REP_PolyTypeRep(r_l, fntype),
                                                 l_meth.GetField(pos_AS_ExtExplFnDef_access),
                                                 l_meth.GetBool(pos_AS_ExtExplFnDef_stat)));
              break;
            }
          }
        }
      }
      l_resMap.ImpModify(l_nm, l_resMap_l_nm);
    }
  }
  return l_resMap;
}

// ExportSubresps
// p_nm : AS`Name
// p_fnm : map AS`Name to AS`FnDef
// p_pfm : map AS`Name to AS`FnDef
// p_opm : map AS`Name to AS`OpDef
// ==> map AS`Name to ENV`AccessFnTypeRep | ENV`AccessPolyTypeRep | ENV`AccessOpTypeRep |
//                      set of (ENV`AccessOpTypeRep | ENV`AccessPolyTypeRep | ENV`AccessFnTypeRep)
Map StatSem::ExportSubresps(const TYPE_AS_Name & p_nm,
                            const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & p_fnm,
                            const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & p_pfm,
                            const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & p_opm)
{
  Map l_resMap;
  Set dom_p_fnm (p_fnm.Dom());
  Generic fnnm;
  for (bool bb = dom_p_fnm.First(fnnm); bb; bb = dom_p_fnm.Next(fnnm))
  {
    const TYPE_AS_FnDef & fndef (p_fnm[fnnm]);
    switch (fndef.GetTag()) {
      case TAG_TYPE_AS_ExplFnDef: {
        if (fndef.GetRecord(pos_AS_ExplFnDef_body).GetField(pos_AS_FnBody_body) == Int(SUBRESP))
        {
          TYPE_SSENV_AccessType atp (MakeFnRng (ExportExplicitFunction(p_nm, fndef),
                                                fndef.GetField(pos_AS_ExplFnDef_access),
                                                fndef.GetBool(pos_AS_ExplFnDef_stat)));
          TYPE_AS_Name nm (SetName (p_nm, (fndef.GetRecord(pos_AS_ExplFnDef_nm))));
          if (l_resMap.DomExists(nm))
          {
            Generic atpg (l_resMap[nm]);
            Set atp_s (atpg.IsSet() ? Set(atpg) : mk_set(atpg));
            atp_s.Insert(atp);
            l_resMap.ImpModify(nm, atp_s);
          }
          else
            l_resMap.ImpModify(nm, atp);
        }
        break;
      }
      case TAG_TYPE_AS_ExtExplFnDef: {
        if (fndef.GetRecord(pos_AS_ExtExplFnDef_body).GetField(pos_AS_FnBody_body) == Int(SUBRESP))
        {
          TYPE_SSENV_AccessType atp (MakeFnRng (ExportExtExplicitFunction(p_nm, fndef),
                                                fndef.GetField(pos_AS_ExtExplFnDef_access),
                                                fndef.GetBool(pos_AS_ExtExplFnDef_stat)));
          TYPE_AS_Name nm (SetName (p_nm, (fndef.GetRecord(pos_AS_ExtExplFnDef_nm))));
          if (l_resMap.DomExists(nm))
          {
            Generic atpg (l_resMap[nm]);
            Set atp_s (atpg.IsSet() ? Set(atpg) : mk_set(atpg));
            atp_s.Insert(atp);
            l_resMap.ImpModify(nm, atp_s);
          }
          else
            l_resMap.ImpModify(nm, atp);
        }
        break;
      }
      case TAG_TYPE_AS_ImplFnDef:
      default: {
        break;
      }
    }
  }

  Set dom_p_pfm (p_pfm.Dom());
  Generic pfnm;
  for (bool bb = dom_p_pfm.First(pfnm); bb; bb = dom_p_pfm.Next(pfnm))
  {
    const TYPE_AS_FnDef & fndef (p_pfm[pfnm]);
    switch (fndef.GetTag()) {
      case TAG_TYPE_AS_ExplFnDef: {
        if (fndef.GetRecord(pos_AS_ExplFnDef_body).GetField(pos_AS_FnBody_body) == Int(SUBRESP))
        {
          const SEQ<TYPE_AS_TypeVar> & tv_l (fndef.GetSequence(pos_AS_ExplFnDef_tpparms));
          TYPE_REP_TypeRep fntp (ExportExplicitFunction(p_nm, fndef));
          SEQ<TYPE_REP_TypeRep> r_l (TransTypeList (p_nm, tv_l));
          TYPE_SSENV_AccessType atp (MakePolyRng (mk_REP_PolyTypeRep(r_l, fntp),
                                                  fndef.GetField(pos_AS_ExplFnDef_access),
                                                  fndef.GetBool(pos_AS_ExplFnDef_stat)));

          TYPE_AS_Name nm (SetName(p_nm, fndef.GetRecord(pos_AS_ExplFnDef_nm)));
          if (l_resMap.DomExists(nm))
          {
            Generic atpg (l_resMap[nm]);
            Set atp_s (atpg.IsSet() ? Set(atpg) : mk_set(atpg));
            atp_s.Insert(atp);
            l_resMap.ImpModify(nm, atp_s);
          }
          else
            l_resMap.ImpModify(nm, atp);
        }
        break;
      }
      case TAG_TYPE_AS_ExtExplFnDef: {
        if (fndef.GetRecord(pos_AS_ExtExplFnDef_body).GetField(pos_AS_FnBody_body) == Int(SUBRESP))
        {
          const SEQ<TYPE_AS_TypeVar> & tv_l (fndef.GetSequence(pos_AS_ExtExplFnDef_params));
          TYPE_REP_TypeRep fntp (ExportExtExplicitFunction(p_nm, fndef));
          SEQ<TYPE_REP_TypeRep> r_l (TransTypeList (p_nm, tv_l));
          TYPE_SSENV_AccessType atp (MakePolyRng(mk_REP_PolyTypeRep(r_l, fntp),
                                                 fndef.GetField(pos_AS_ExtExplFnDef_access),
                                                 fndef.GetBool(pos_AS_ExtExplFnDef_stat)));

          TYPE_AS_Name nm (SetName (p_nm, (fndef.GetRecord(pos_AS_ExtExplFnDef_nm))));
          if (l_resMap.DomExists(nm))
          {
            Generic atpg (l_resMap[nm]);
            Set atp_s (atpg.IsSet() ? Set(atpg) : mk_set(atpg));
            atp_s.Insert(atp);
            l_resMap.ImpModify(nm, atp_s);
          }
          else
            l_resMap.ImpModify(nm, atp);
        }
        break;
      }
      case TAG_TYPE_AS_ImplFnDef:
      default: {
        break;
      }
    }
  }

  Set dom_p_opm (p_opm.Dom());
  Generic opnm;
  for (bool cc = dom_p_opm.First(opnm); cc; cc = dom_p_opm.Next(opnm))
  {
    const TYPE_AS_FnDef & opdef (p_opm[opnm]);
    switch(opdef.GetTag()) {
      case TAG_TYPE_AS_ExplOpDef: {
        if (opdef.GetRecord(pos_AS_ExplOpDef_body).GetField(pos_AS_OpBody_body) == Int(SUBRESP))
        {
          TYPE_SSENV_AccessType atp (MakeOpRng (ExportExplicitOperation(p_nm, opdef),
                                                opdef.GetField(pos_AS_ExplOpDef_access),
                                                opdef.GetBool(pos_AS_ExplOpDef_stat),
                                                opdef.GetBool(pos_AS_ExplOpDef_oppure)));
          TYPE_AS_Name nm (SetName (p_nm, (opdef.GetRecord(pos_AS_ExplOpDef_nm))));
          if (l_resMap.DomExists(nm))
          {
            Generic atpg (l_resMap[nm]);
            Set atp_s (atpg.IsSet() ? Set(atpg) : mk_set(atpg));
            atp_s.Insert(atp);
            l_resMap.ImpModify(nm, atp_s);
          }
          else
            l_resMap.ImpModify(nm, atp);
        }
        break;
      }
      case TAG_TYPE_AS_ExtExplOpDef: {
        if (opdef.GetRecord(pos_AS_ExtExplOpDef_body).GetField(pos_AS_OpBody_body) == Int(SUBRESP))
        {
          TYPE_SSENV_AccessType atp (MakeOpRng (ExportExtExplicitOperation(p_nm, opdef),
                                                opdef.GetField(pos_AS_ExtExplOpDef_access),
                                                opdef.GetBool(pos_AS_ExtExplOpDef_stat),
                                                opdef.GetBool(pos_AS_ExtExplOpDef_oppure)));
          TYPE_AS_Name nm (SetName (p_nm, (opdef.GetRecord(pos_AS_ExtExplOpDef_nm))));
          if (l_resMap.DomExists(nm))
          {
            Generic atpg (l_resMap[nm]);
            Set atp_s (atpg.IsSet() ? Set(atpg) : mk_set(atpg));
            atp_s.Insert(atp);
            l_resMap.ImpModify(nm, atp_s);
          }
          else
            l_resMap.ImpModify(nm, atp);
        }
        break;
      }
      case TAG_TYPE_AS_ImplOpDef:
      default: {
        break;
      }
    }
  }

  return l_resMap;
}

#endif //VDMPP

// ExportOperations
// modid : [AS`Name]
// opm :  map AS`Name to AS`OpDef
// ==> map AS`Name to REP`OpTypeRep
Map StatSem::ExportOperations (const Generic & modid, const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & opm)
{
  Set dom_opm (opm.Dom());
  Map ops;
  Generic nm;
  for (bool bb = dom_opm.First(nm); bb; bb = dom_opm.Next(nm))
  {
#ifdef VDMPP
    if (!MANGLE::IsMangled(nm))
#endif //VDMPP
    {
      TYPE_AS_OpDef opdef (opm[nm]);
      switch(opdef.GetTag()) {
        case TAG_TYPE_AS_ExplOpDef: {
          ops.ImpModify (SetName (modid, nm),
                         MakeOpRng (ExportExplicitOperation(modid, opdef),
                                    opdef.GetField(pos_AS_ExplOpDef_access),
                                    opdef.GetBool(pos_AS_ExplOpDef_stat),
                                    opdef.GetBool(pos_AS_ExplOpDef_oppure)));
          break;
        }
        case TAG_TYPE_AS_ImplOpDef: {
          ops.ImpModify (SetName (modid, nm),
                         MakeOpRng (ExportImplicitOperation (modid, opdef),
                                    opdef.GetField(pos_AS_ImplOpDef_access),
                                    opdef.GetBool(pos_AS_ImplOpDef_stat),
                                    opdef.GetBool(pos_AS_ImplOpDef_oppure)));
          break;
        }
        case TAG_TYPE_AS_ExtExplOpDef: {
          ops.ImpModify (SetName (modid, nm),
                         MakeOpRng (ExportExtExplicitOperation (modid, opdef),
                                    opdef.GetField(pos_AS_ExtExplOpDef_access),
                                    opdef.GetBool(pos_AS_ExtExplOpDef_stat),
                                    opdef.GetBool(pos_AS_ExtExplOpDef_oppure)));
          break;
        }
        default:
          break;
      }
    }
  }
  return ops;
}

#ifdef VDMPP

// ExportConstructors
// modid : [AS`Name]
// opm :  map AS`Name to AS`OpDef
// ==> set of ENV`AccessOpTypeRep
SET<TYPE_SSENV_AccessOpTypeRep> StatSem::ExportConstructors(const Generic & modid,
                                                            const MAP<TYPE_AS_Name, TYPE_AS_OpDef>& opm)
{
  Set dom_opm (opm.Dom());
  Set ops;
  Generic nm;
  for (bool bb = dom_opm.First(nm); bb; bb = dom_opm.Next(nm))
  {
    TYPE_AS_OpDef opdef (opm[nm]);
    switch(opdef.GetTag()) {
      case TAG_TYPE_AS_ExplOpDef: {
        if (opdef.GetBoolValue(pos_AS_ExplOpDef_constr)) {
          ops.Insert (MakeOpRng (ExportExplicitOperation (modid, opdef),
                                 opdef.GetField(pos_AS_ExplOpDef_access),
                                 opdef.GetBool(pos_AS_ExplOpDef_stat),
                                 opdef.GetBool(pos_AS_ExplOpDef_oppure)));
        }
        break;
      }
      case TAG_TYPE_AS_ImplOpDef: {
        if (opdef.GetBoolValue(pos_AS_ImplOpDef_constr)) {
          ops.Insert (MakeOpRng (ExportImplicitOperation (modid, opdef),
                                 opdef.GetField(pos_AS_ImplOpDef_access),
                                 opdef.GetBool(pos_AS_ImplOpDef_stat),
                                 opdef.GetBool(pos_AS_ImplOpDef_oppure)));
        }
        break;
      }
      case TAG_TYPE_AS_ExtExplOpDef: {
        if (opdef.GetBoolValue(pos_AS_ExtExplOpDef_constr)) {
          ops.Insert (MakeOpRng (ExportExtExplicitOperation (modid, opdef),
                                 opdef.GetField(pos_AS_ExtExplOpDef_access),
                                 opdef.GetBool(pos_AS_ExtExplOpDef_stat),
                                 opdef.GetBool(pos_AS_ExtExplOpDef_oppure)));
        }
        break;
      }
      default:
        break;
    }
  }
  return ops;
}

#endif //VDMPP

// ExportInvs
// modid : [AS`Name]
// typem : map AS`Name to AS`TypeDef
// ==> map AS`Name to REP`FnTypeRep
Map StatSem::ExportInvs (const Generic & modid, const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & typem)
{
  Set dom_typem (typem.Dom());
  Map invs;
  Generic tpnm;
  // typem : map AS`Name to AS`TypeDef
  for (bool bb = dom_typem.First(tpnm); bb; bb = dom_typem.Next(tpnm)) {
    TYPE_AS_TypeDef tpdef (typem[tpnm]);
    const TYPE_AS_Name & nm     (tpdef.GetRecord(pos_AS_TypeDef_nm));
    const TYPE_AS_Type & tp     (tpdef.GetRecord(pos_AS_TypeDef_shape));
    const Generic & Invar       (tpdef.GetField(pos_AS_TypeDef_Inv));
    const Generic & Eq          (tpdef.GetField(pos_AS_TypeDef_Eq));
    const Generic & Ord         (tpdef.GetField(pos_AS_TypeDef_Ord));
    const TYPE_AS_Access & accs (tpdef.GetField(pos_AS_TypeDef_access));

    if (!Invar.IsNil ()) {
      SEQ<TYPE_REP_TypeRep> sq;
      sq.ImpAppend (TransType(modid, tp));
      TYPE_REP_TotalFnTypeRep fntp (mk_REP_TotalFnTypeRep(sq, btp_bool));
      invs.ImpModify(SetName(modid, Inv (nm)), MakeFnRng(fntp, accs, Bool(true)));
    }
    if (!Eq.IsNil ()) {
      SEQ<TYPE_REP_TypeRep> sq;
      sq.ImpAppend (TransType(modid, tp));
      sq.ImpAppend (TransType(modid, tp));
      TYPE_REP_TotalFnTypeRep fntp (mk_REP_TotalFnTypeRep(sq, btp_bool));
      invs.ImpModify(SetName(modid, Equality (nm)), MakeFnRng(fntp, accs, Bool(true)));
    }
    if (!Ord.IsNil ()) {
      SEQ<TYPE_REP_TypeRep> sq;
      sq.ImpAppend (TransType(modid, tp));
      sq.ImpAppend (TransType(modid, tp));
      TYPE_REP_TotalFnTypeRep fntp (mk_REP_TotalFnTypeRep(sq, btp_bool));
      invs.ImpModify(SetName(modid, Order (nm)), MakeFnRng(fntp, accs, Bool(true)));
    }
  }
  return invs;
}

// ExportPrePostFns
// modid : [AS`Name]
// fnm : map AS`Name to AS`FnDef
// ==> map AS`Name to REP`FnTypeRep
Map StatSem::ExportPrePostFns (const Generic & modid, const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & fnm)
{
  Set dom_fnm (fnm.Dom());
  Map fns;
  Generic fnnm;
  // fnm : map AS`Name to AS`FnDef
  for (bool bb = dom_fnm.First(fnnm); bb; bb = dom_fnm.Next(fnnm)) {
    TYPE_AS_FnDef fndef (fnm[fnnm]);

    switch(fndef.GetTag()) {
      case TAG_TYPE_AS_ExplFnDef: {
        const TYPE_AS_Name & nm     (fndef.GetRecord(pos_AS_ExplFnDef_nm));
        const Generic & prefn       (fndef.GetField(pos_AS_ExplFnDef_fnpre));
        const Generic & postfn      (fndef.GetField(pos_AS_ExplFnDef_fnpost));
        const TYPE_AS_Access & accs (fndef.GetField(pos_AS_ExplFnDef_access));
        const Bool & stat           (fndef.GetBool(pos_AS_ExplFnDef_stat));

        TYPE_REP_TypeRep fntp (ExportExplicitFunction(modid, fndef));

        if (!prefn.IsNil())
          fns.ImpModify(SetName(modid, Pre(nm)), MakeFnRng(MakePreType(fntp), accs, stat));

        if (!postfn.IsNil()) 
          fns.ImpModify(SetName(modid, Post(nm)), MakeFnRng(MakePostType(fntp), accs, stat));

        break;
      }
      case TAG_TYPE_AS_ImplFnDef: {
        const TYPE_AS_Name & nm     (fndef.GetRecord(pos_AS_ImplFnDef_nm));
        const Generic & prefn       (fndef.GetField(pos_AS_ImplFnDef_fnpre));
        const TYPE_AS_Access & accs (fndef.GetField(pos_AS_ImplFnDef_access));
        const Bool & stat           (fndef.GetBool(pos_AS_ImplFnDef_stat));

        TYPE_REP_TypeRep fntp (ExportImplicitFunction(modid, fndef));

        if (!prefn.IsNil())
          fns. ImpModify(SetName(modid, Pre(nm)), MakeFnRng(MakePreType(fntp), accs, stat));

        fns.ImpModify(SetName(modid, Post(nm)), MakeFnRng(MakePostType(fntp), accs, stat));
        break;
      }
      case TAG_TYPE_AS_ExtExplFnDef: {
        const TYPE_AS_Name & nm     (fndef.GetRecord(pos_AS_ExtExplFnDef_nm));
        const Generic & prefn       (fndef.GetField(pos_AS_ExtExplFnDef_fnpre));
        const Generic & postfn      (fndef.GetField(pos_AS_ExtExplFnDef_fnpost));
        const TYPE_AS_Access & accs (fndef.GetField(pos_AS_ExtExplFnDef_access));
        const Bool & stat           (fndef.GetBool(pos_AS_ExtExplFnDef_stat));

        TYPE_REP_TypeRep fntp (ExportExtExplicitFunction(modid, fndef));

        if (!prefn.IsNil ())
          fns.ImpModify (SetName(modid, Pre(nm)), MakeFnRng(MakePreType(fntp), accs, stat));

        if (!postfn.IsNil ())
          fns.ImpModify (SetName(modid, Post(nm)), MakeFnRng(MakePostType(fntp), accs, stat));
        break;
      }
      default:
        break;
    }
  }
  return fns;
}

// ExportPolyFns
// modid : [AS`Name]
// fnm : map AS`Name to AS`FnDef
// ==> map AS`Name to REP`PolyTypeRep
Map StatSem::ExportPolyFns (const Generic & modid, const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & fnm)
{
  Set dom_fnm (fnm.Dom());
  Map fns;
  Generic fnnm;
  for (bool bb = dom_fnm.First(fnnm); bb; bb = dom_fnm.Next(fnnm))
  {
// 20111019 -->
#ifdef VDMPP
    if (!MANGLE::IsMangled(fnnm))
    {
#endif // VDMPP
// <-- 20111019
    TYPE_AS_FnDef fndef (fnm[fnnm]);

    switch(fndef.GetTag()) {
      case TAG_TYPE_AS_ExplFnDef: {
        const TYPE_AS_Name & nm           (fndef.GetRecord(pos_AS_ExplFnDef_nm));
        const SEQ<TYPE_AS_TypeVar> & tv_l (fndef.GetSequence(pos_AS_ExplFnDef_tpparms));
        const Generic & prefn             (fndef.GetField(pos_AS_ExplFnDef_fnpre));
        const Generic & postfn            (fndef.GetField(pos_AS_ExplFnDef_fnpost));
        const TYPE_AS_Access & accs       (fndef.GetField(pos_AS_ExplFnDef_access));
        const Bool & stat                 (fndef.GetBool(pos_AS_ExplFnDef_stat));

        TYPE_REP_TypeRep fntp (ExportExplicitFunction(modid, fndef));
        SEQ<TYPE_REP_TypeRep> r_l (TransTypeList (modid, tv_l));

        fns.Insert (SetName (modid, nm), MakePolyRng (mk_REP_PolyTypeRep(r_l, fntp), accs, stat));

        if (!prefn.IsNil ())
        {
          Tuple infer (MakePolyPreType(r_l, fntp));
          TYPE_REP_PolyTypeRep ptr (mk_REP_PolyTypeRep(infer.GetSequence(1), infer.GetRecord(2)));
          fns.Insert (SetName (modid, Pre (nm)), MakePolyRng (ptr, accs, stat));
        }

        if (!postfn.IsNil ())
        {
          Tuple infer (MakePolyPostType(r_l, fntp));
          TYPE_REP_PolyTypeRep ptr (mk_REP_PolyTypeRep(infer.GetSequence(1), infer.GetRecord(2)));
          fns.Insert (SetName(modid, Post(nm)), MakePolyRng(ptr, accs, stat));
        }
        break;
      }
      case TAG_TYPE_AS_ImplFnDef: {
        const TYPE_AS_Name & nm                 (fndef.GetRecord(pos_AS_ImplFnDef_nm));
        const SEQ<TYPE_AS_TypeVar> & tv_l       (fndef.GetSequence(pos_AS_ImplFnDef_params));
        const Generic & prefn                   (fndef.GetField(pos_AS_ImplFnDef_fnpre));
        const TYPE_AS_Access & accs             (fndef.GetField(pos_AS_ImplFnDef_access));
        const Bool & stat                       (fndef.GetBool(pos_AS_ImplFnDef_stat));

        TYPE_REP_TypeRep fntp (ExportImplicitFunction(modid, fndef));
        SEQ<TYPE_REP_TypeRep> r_l (TransTypeList(modid, tv_l));

        fns.Insert (SetName(modid, nm), MakePolyRng(mk_REP_PolyTypeRep(r_l, fntp), accs, stat));

        if (!prefn.IsNil())
        {
          Tuple infer (MakePolyPreType(r_l, fntp));
          TYPE_REP_PolyTypeRep ptr (mk_REP_PolyTypeRep(infer.GetSequence(1), infer.GetRecord(2)));
          fns.Insert(SetName(modid, Pre(nm)), MakePolyRng(ptr, accs, stat));
        }

        Tuple infer (MakePolyPostType(r_l, fntp));
        TYPE_REP_PolyTypeRep ptr (mk_REP_PolyTypeRep(infer.GetSequence(1), infer.GetRecord(2)));
        fns.Insert(SetName(modid, Post(nm)), MakePolyRng(ptr, accs, stat));
        break;
      }
      case TAG_TYPE_AS_ExtExplFnDef: {
        const TYPE_AS_Name & nm                 (fndef.GetRecord(pos_AS_ExtExplFnDef_nm));
        const SEQ<TYPE_AS_TypeVar> & tv_l       (fndef.GetSequence(pos_AS_ExtExplFnDef_params));
        const Generic & prefn                   (fndef.GetField(pos_AS_ExtExplFnDef_fnpre));
        const Generic & postfn                  (fndef.GetField(pos_AS_ExtExplFnDef_fnpost));
        const TYPE_AS_Access & accs             (fndef.GetField(pos_AS_ExtExplFnDef_access));
        const Bool & stat                       (fndef.GetBool(pos_AS_ExtExplFnDef_stat));

        TYPE_REP_TypeRep fntp (ExportExtExplicitFunction(modid, fndef));
        SEQ<TYPE_REP_TypeRep> r_l (TransTypeList (modid, tv_l));

        fns.Insert(SetName(modid, nm), MakePolyRng(mk_REP_PolyTypeRep(r_l, fntp), accs, stat));

        if (!prefn.IsNil())
        {
          Tuple infer (MakePolyPreType(r_l, fntp));
          TYPE_REP_PolyTypeRep ptr (mk_REP_PolyTypeRep(infer.GetSequence(1), infer.GetRecord(2)));
          fns.Insert(SetName(modid, Pre (nm)), MakePolyRng(ptr, accs, stat));
        }

        if (!postfn.IsNil())
        {
          Tuple infer (MakePolyPostType(r_l, fntp));
          TYPE_REP_PolyTypeRep ptr (mk_REP_PolyTypeRep(infer.GetSequence(1), infer.GetRecord(2)));
          fns.Insert(SetName(modid, Post (nm)), MakePolyRng(ptr, accs, stat));
        }
        break;
      }
      default:
        break;
    }
// 20111019 -->
#ifdef VDMPP
    }
#endif // VDMPP
// <--20111019
  }
  return fns;
}

#ifdef VDMSL
// ExportPrePostOps
// modid : [AS`Name]
// opm : map AS`Name to AS`OpDef
// state : [AS`StateDef]
// ==> map AS`Name to REP`FnTypeRep
Map StatSem::ExportPrePostOps (const Generic & modid,
                               const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & opm,
                               const Generic & state)
{
  Set dom_opm (opm.Dom());
  Map fns;
  Generic opnm;
  // opm : map AS`Name to AS`OpDef
  for (bool bb = dom_opm.First(opnm); bb; bb = dom_opm.Next(opnm))
  {
    TYPE_AS_OpDef opdef (opm[opnm]);
    switch(opdef.GetTag()) {
      case TAG_TYPE_AS_ExplOpDef: {
        const TYPE_AS_Name & nm   (opdef.GetRecord(pos_AS_ExplOpDef_nm));
        const Generic & preop     (opdef.GetField(pos_AS_ExplOpDef_oppre));
        const Generic & postop    (opdef.GetField(pos_AS_ExplOpDef_oppost));

        TYPE_REP_OpTypeRep optp (ExportExplicitOperation(modid, opdef));

        if (!preop.IsNil ())
        {
          TYPE_AS_Name prenm (modid.IsNil() ? Pre (nm) : ExtName(modid, Pre(nm)));
          TYPE_REP_TypeRep pretp (MakeOpPreType(modid, optp, state));
          fns.ImpModify(prenm, pretp);
        }
        if (!postop.IsNil())
        {
          TYPE_AS_Name postnm (modid.IsNil() ? Post (nm) : ExtName(modid, Post(nm)));
          TYPE_REP_TypeRep posttp (MakeOpPostType(modid, optp, state));
          fns.ImpModify(postnm, posttp);
        }
        break;
      }
      case TAG_TYPE_AS_ImplOpDef: {
        const TYPE_AS_Name & nm                 (opdef.GetRecord(pos_AS_ImplOpDef_nm));
        const Generic & preop                   (opdef.GetField(pos_AS_ImplOpDef_oppre));

        TYPE_REP_OpTypeRep optp (ExportImplicitOperation(modid, opdef));
        if (!preop.IsNil())
        {
          TYPE_AS_Name prenm (modid.IsNil() ? Pre (nm) : ExtName(modid, Pre(nm)));
          TYPE_REP_TypeRep pretp (MakeOpPreType (modid, optp, state));
          fns.ImpModify(prenm, pretp);
        }

        TYPE_AS_Name postnm (modid.IsNil() ? Post (nm) : ExtName(modid, Post(nm)));
        TYPE_REP_TypeRep posttp (MakeOpPostType (modid, optp, state));
        fns.ImpModify(postnm, posttp);
        break;
      }
      case TAG_TYPE_AS_ExtExplOpDef: {
        const TYPE_AS_Name & nm                 (opdef.GetRecord(pos_AS_ExtExplOpDef_nm));
        const Generic & preop                   (opdef.GetField(pos_AS_ExtExplOpDef_oppre));
        const Generic & postop                  (opdef.GetField(pos_AS_ExtExplOpDef_oppost));

        TYPE_REP_OpTypeRep optp (ExportExtExplicitOperation(modid, opdef));

        if (!preop.IsNil ())
        {
          TYPE_AS_Name prenm (modid.IsNil() ? Pre (nm) : ExtName(modid, Pre(nm)));
          TYPE_REP_TypeRep pretp (MakeOpPreType(modid, optp, state));
          fns.ImpModify(prenm, pretp);
        }

        if (!postop.IsNil ())
        {
          TYPE_AS_Name postnm (modid.IsNil() ? Post (nm) : ExtName(modid, Post(nm)));
          TYPE_REP_TypeRep posttp (MakeOpPostType(modid, optp, state));
          fns.ImpModify(postnm, posttp);
        }
        break;
      }
      default:
        break;
    }
  }
  return fns;
}

// ExportSomeTypes
// modid : [AS`Name]
// tps : map AS`Name to [<STRUCT>]
// typem : map AS`Name to AS`TypeDef
// state : [AS`StateDef]
// ==> map AS`Name to REP`TypeRep * map AS`Name to seq of REP`FieldRep
Tuple StatSem::ExportSomeTypes (const Generic & modid,
                                const MAP<TYPE_AS_Name, Generic>  & tps,
                                const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & typem,
                                const Generic & state)
{
  Generic stid;
  if (!state.IsNil())
  {
    stid = Record(state).GetRecord(pos_AS_StateDef_tp).GetRecord(pos_AS_CompositeType_name);
  }
  else
    stid = Nil ();

  Set dom_tps (tps.Dom());
  Map exptps, exptag;
  Generic nm;
  // tps : map AS`Name to [<STRUCT>]
  for (bool bb = dom_tps.First(nm); bb; bb = dom_tps.Next(nm))
  {
    if (nm == stid)
    {
      TYPE_AS_StateDef sd (state);
      const TYPE_AS_CompositeType & tp (sd.GetRecord(pos_AS_StateDef_tp));
      const Generic & invar (sd.GetField(pos_AS_StateDef_Inv));

      if (tps[nm].IsNil())
      {
// 20140326 -->
        //if (modid.IsNil())
        //  exptps.ImpModify(nm, mk_REP_TypeParRep(nm));
        //else
        //  exptps.ImpModify(ExtName(modid, nm), mk_REP_TypeParRep(nm));
        if (modid.IsNil())
          exptps.ImpModify(stid, TransTypeDef(modid, nm, tp, invar));
        else
          exptps.ImpModify(ExtName(modid, tp.get_name()), TransTypeDef(modid, nm, tp, invar));
// <-- 20140326
      }
      else
      {
        if (modid.IsNil()) {
          exptps.ImpModify(stid, TransTypeDef(modid, nm, tp, invar));
          exptag.ImpModify(stid, TransFields(modid, tp.get_fields()));
        }
        else
        {
          exptps.ImpModify(ExtName(modid, tp.get_name()), TransTypeDef(modid, nm, tp, invar));
          exptag.ImpModify(ExtName(modid, tp.get_name()), TransFields(modid, tp.get_fields()));
        }
      }
    } // nm == stid
    else if (tps[nm] == (Int)STRUCT)
    {
      if (modid.IsNil ())
      {
        if (typem.DomExists (nm))
        {
           TYPE_AS_TypeDef tpdef (typem[nm]);
           exptps.ImpModify (nm, TransTypeDef (modid, nm, tpdef.get_shape (), tpdef.get_Inv ()));
           exptag.ImpOverride (ColTags (modid, tpdef.get_shape ()));
        }
        else
        {
          //--------------------------------------
          // Error message #343
          // The type L"%1" cannot be exported here
          //--------------------------------------
          GenErr (nm, WRN1, 343, mk_sequence(PrintName(nm)));
          exptps.ImpModify (nm, rep_alltp);
        }
      }
      else
      {
        if (typem.DomExists (nm))
        {
          TYPE_AS_TypeDef tpdef (typem[nm]);
          exptps.ImpModify (ExtName (modid, nm), TransTypeDef (modid, nm, tpdef.get_shape (), tpdef.get_Inv ()));
          exptag.ImpOverride (ColTags (modid, tpdef.get_shape ()));
        }
        else
        {
          //--------------------------------------
          // Error message #343
          // The type L"%1" cannot be exported here
          //--------------------------------------
          GenErr (nm, WRN1, 343, mk_sequence(PrintName(nm)));
          exptps.ImpModify (ExtName (modid, nm), rep_alltp);
        }
      }
    }
    else
    {
// 20140321 -->      
      //if (modid.IsNil ())
      //  exptps.ImpModify (nm, mk_REP_TypeParRep( nm ));
      //else
      //  exptps.ImpModify (ExtName (modid, nm), mk_REP_TypeParRep( nm ));
      TYPE_AS_TypeDef td (typem[nm]);
      TYPE_AS_Type tp (td.GetRecord(pos_AS_TypeDef_shape));
      if (modid.IsNil ())
        exptps.ImpModify (nm, TransType (modid, tp));
      else
        exptps.ImpModify (ExtName (modid, nm), TransType (modid, tp));
// <-- 20140321
    }
  }
  return mk_(exptps, exptag);
}

// ExportSimple
// modid : [AS`Name]
// simple : map AS`Name to AS`Type
// ==> map AS`Name to REP`TypeRep
Map StatSem::ExportSimple (const Generic & modid, const MAP<TYPE_AS_Name, TYPE_AS_Type> & simple)
{
  Set dom_simple (simple.Dom());
  MAP<TYPE_AS_Name,TYPE_REP_TypeRep> res;
  Generic nm;
  // simple : map AS`Name to AS`Type
  for (bool bb = dom_simple.First(nm); bb; bb = dom_simple.Next(nm))
  {
    TYPE_AS_Type tp (simple[nm]);
    if (modid.IsNil ())
      res.Insert (nm, TransType (modid, tp));
    else
      res.Insert (ExtName (modid, nm), TransType (modid, tp));
  }

  return res;
}

// FindRenamedTypes
// modid : AS`Name
// imp_m : map AS`Name to [AS`ImportSig]
// ==> map AS`Name to AS`Name
Map StatSem::FindRenamedTypes (const TYPE_AS_Name & modid, const MAP<TYPE_AS_Name, TYPE_AS_Name> & imp_m)
{
  SET<TYPE_AS_Name> dom_imp_m (imp_m.Dom());
  MAP<TYPE_AS_Name, TYPE_AS_Name> bind;
  Generic modnm;
  // imp_m : map AS`Name to [AS`ImportSig]
  for (bool bb = dom_imp_m.First(modnm); bb; bb = dom_imp_m.Next(modnm))
  {
    if (!imp_m[modnm].IsNil ())
    {
      const TYPE_AS_ImportSig & imp (imp_m[modnm]);
// 20110510 -->
      const Map & tps (imp.GetMap(pos_AS_ImportSig_tps)); // map Name to [TypeDef]
      const Map & val (imp.GetMap(pos_AS_ImportSig_val)); // map Name to [Type]
      const Map & fns (imp.GetMap(pos_AS_ImportSig_fns)); // map Name to [(TypeVarList * FnType)]
      const Map & ops (imp.GetMap(pos_AS_ImportSig_ops)); // map Name to [OpType]
// <--20110510
      const MAP<TYPE_AS_Name, TYPE_AS_Name>  & ren (imp.GetMap(pos_AS_ImportSig_ren)); // map Name to Name

      SET<TYPE_AS_Name> dom_ren (ren.Dom());

      Generic nm;
      for (bb = dom_ren.First(nm); bb; bb = dom_ren.Next(nm))
      {
        bind.ImpModify (ExtName (modid, nm), ExtName (modnm, ren[nm]));

// 20110510 -->
        TYPE_AS_Name onm (ren[nm]);
        if (tps.DomExists(onm) && !tps[onm].IsNil())
        {
          TYPE_AS_TypeDef td (tps[onm]);
          GetCI().SetTypeInfo(ASTAUX::GetCid(nm),
                              TransTypeDef(modid, nm, td.GetRecord(pos_AS_TypeDef_shape),
                                                      td.GetField(pos_AS_TypeDef_Inv)));  
        }
        else if (val.DomExists(onm) && !val[onm].IsNil())
        {
          GetCI().SetTypeInfo(ASTAUX::GetCid(nm), TransType(modid, val[onm]));
        }
        else if (fns.DomExists(onm) && !fns[onm].IsNil())
        {
          Tuple t (fns[onm]);
          GetCI().SetTypeInfo(ASTAUX::GetCid(nm), TransType(modid, t.GetField(2)));
        }
        else if (ops.DomExists(onm) && !ops[onm].IsNil())
        {
          GetCI().SetTypeInfo(ASTAUX::GetCid(nm), TransType(modid, ops[onm]));
        }
// <--20110510
      }
    }
  }
  return bind;
}

// UseRenamedTypes
// exp_org : ExpSigRep
// bind : map AS`Name to AS`Name
// ==> ExpSigRep
TYPE_SSENV_ExpSigRep StatSem::UseRenamedTypes (const TYPE_SSENV_ExpSigRep & exp_org,
                                               const MAP<TYPE_AS_Name, TYPE_AS_Name> & bind)
{
  TYPE_SSENV_ExpSigRep exp (exp_org);

  Map bind_;
  // bind : map AS`Name to AS`Name
  SET<TYPE_AS_Name> dom_bind (bind.Dom());
  Generic nm;
  for (bool bb = dom_bind.First (nm); bb; bb = dom_bind.Next (nm))
  {
    bind_.Insert ( nm, mk_REP_TypeNameRep( bind[nm] ) );
  }

  Map tps (exp.get_tps ()), new_tps; // map AS`Name to REP`TypeRep
  Set dom_tps (tps.Dom());
  for (bool cc = dom_tps.First(nm); cc; cc = dom_tps.Next(nm))
    new_tps.Insert (nm, InstFn (REN, tps[nm], bind_));
  exp.set_tps (new_tps);

  Map val (exp.get_val ()), new_val; // map AS`Name to REP`TypeRep
  Set dom_val (val.Dom());
  for (bool dd = dom_val.First(nm); dd; dd = dom_val.Next(nm))
    new_val.Insert (nm, InstFn (REN, val[nm], bind_));
  exp.set_val (new_val);

  Map fns (exp.get_fns ()), new_fns; // map AS`Name to REP`FnTypeRep
  Set dom_fns (fns.Dom());
  for (bool ee = dom_fns.First(nm); ee; ee = dom_fns.Next(nm))
    new_fns.Insert (nm, InstFn (REN, fns[nm], bind_));
  exp.set_fns (new_fns);

  Map polys (exp.get_polys ()), new_polys; // map AS`Name to REP`PolyTypeRep
  Set dom_polys (polys.Dom());
  for (bool ff = dom_polys.First(nm); ff; ff = dom_polys.Next(nm))
    new_polys.Insert (nm, InstFn (REN, polys[nm], bind_));
  exp.set_polys (new_polys);

  Map ops (exp.get_ops ()), new_ops; // map AS`Name to REP`OpTypeRep
  Set dom_ops (ops.Dom());
  for (bool gg = dom_ops.First(nm); gg; gg = dom_ops.Next(nm))
    new_ops.Insert (nm, InstFn (REN, ops[nm], bind_));
  exp.set_ops (new_ops);

  return exp;
}
#endif // VDMSL


// ColTags
// modid : [AS`Name]
// tp : AS`Type
// ==> map AS`Name to seq of REP`FieldRep
Map StatSem::ColTags (const Generic & modid, const TYPE_AS_Type & tp) const
{
  switch (tp.GetTag()) {
    case TAG_TYPE_AS_BooleanType:
    case TAG_TYPE_AS_NumericType:
    case TAG_TYPE_AS_TokenType:
    case TAG_TYPE_AS_CharType:
    case TAG_TYPE_AS_QuoteType:
    case TAG_TYPE_AS_TypeName:
    case TAG_TYPE_AS_TypeVar: {
      return Map();
    }
    case TAG_TYPE_AS_CompositeType: {
      Map tagenv;
      const SEQ<TYPE_AS_Field> & fields (tp.GetSequence(pos_AS_CompositeType_fields));
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++)
      {
        const TYPE_AS_Field & f (fields[idx]);
        tagenv.ImpOverride (ColTags (modid, f.GetRecord(pos_AS_Field_type)));
      }

      if (modid.IsNil ())
        tagenv.ImpModify (tp.GetRecord(pos_AS_CompositeType_name), TransFields (modid, fields));
      else
        tagenv.ImpModify (ExtName (modid, tp.GetRecord(pos_AS_CompositeType_name)), TransFields (modid, fields));
      return tagenv;
    }
    case TAG_TYPE_AS_UnionType: {
      Map tagenv;
      const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence(pos_AS_UnionType_tps));
      size_t len_tp_l = tp_l.Length();
      for (size_t idx = 1; idx <= len_tp_l; idx++)
        tagenv.ImpOverride (ColTags (modid, tp_l[idx]));
      return tagenv;
    }
    case TAG_TYPE_AS_ProductType: {
      Map tagenv;
      const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence(pos_AS_ProductType_tps));
      size_t len_tp_l = tp_l.Length();
      for (size_t idx = 1; idx <= len_tp_l; idx++)
        tagenv.ImpOverride (ColTags (modid, tp_l[idx]));
      return tagenv;
    }
    case TAG_TYPE_AS_BracketedType: {
      return ColTags (modid, tp.GetRecord(pos_AS_BracketedType_tp));
    }
    case TAG_TYPE_AS_OptionalType: {
      return ColTags (modid, tp.GetRecord(pos_AS_OptionalType_tp));
    }
    case TAG_TYPE_AS_Set0Type: {
      return ColTags (modid, tp.GetRecord(pos_AS_Set0Type_elemtp));
    }
    case TAG_TYPE_AS_Set1Type: {
      return ColTags (modid, tp.GetRecord(pos_AS_Set1Type_elemtp));
    }
    case TAG_TYPE_AS_Seq0Type: {
      return ColTags (modid, tp.GetRecord(pos_AS_Seq0Type_elemtp));
    }
    case TAG_TYPE_AS_Seq1Type: {
      return ColTags (modid, tp.GetRecord(pos_AS_Seq1Type_elemtp));
    }
    case TAG_TYPE_AS_GeneralMap0Type: {
      Map tagenv (ColTags (modid, tp.GetRecord(pos_AS_GeneralMap0Type_mapdom)));
      tagenv.ImpOverride (ColTags (modid, tp.GetRecord(pos_AS_GeneralMap0Type_maprng)));
      return tagenv;
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      Map tagenv (ColTags (modid, tp.GetRecord(pos_AS_GeneralMap1Type_mapdom)));
      tagenv.ImpOverride (ColTags (modid, tp.GetRecord(pos_AS_GeneralMap1Type_maprng)));
      return tagenv;
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      Map tagenv (ColTags (modid, tp.GetRecord(pos_AS_InjectiveMap0Type_mapdom)));
      tagenv.ImpOverride (ColTags (modid, tp.GetRecord(pos_AS_InjectiveMap0Type_maprng)));
      return tagenv;
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      Map tagenv (ColTags (modid, tp.GetRecord(pos_AS_InjectiveMap1Type_mapdom)));
      tagenv.ImpOverride (ColTags (modid, tp.GetRecord(pos_AS_InjectiveMap1Type_maprng)));
      return tagenv;
    }
    case TAG_TYPE_AS_PartialFnType: {
      Map tagenv;
      const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence(pos_AS_PartialFnType_fndom));
      size_t len_tp_l = tp_l.Length();
      for (size_t idx = 1; idx <= len_tp_l; idx++)
        tagenv.ImpOverride (ColTags (modid, tp_l[idx]));
      tagenv.ImpOverride (ColTags (modid, tp.GetRecord(pos_AS_PartialFnType_fnrng)));
      return tagenv;
    }
    case TAG_TYPE_AS_TotalFnType: {
      Map tagenv;
      const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence(pos_AS_TotalFnType_fndom));
      size_t len_tp_l = tp_l.Length();
      for (size_t idx = 1; idx <= len_tp_l; idx++)
        tagenv.ImpOverride (ColTags (modid, tp_l[idx]));
      tagenv.ImpOverride (ColTags (modid, tp.GetRecord(pos_AS_TotalFnType_fnrng)));
      return tagenv;
    }
    default: {
      InternalError(L"ColTags");
      return Map ();
    }
  }
}

#ifdef VDMSL
// ExportDLModule
// modid : AS`Name
// iexp : AS`DLExportSig
// +> ExpSigRep
TYPE_SSENV_ExpSigRep StatSem::ExportDLModule (const TYPE_AS_Name & modid, const TYPE_AS_DLExportSig & iexp) const
{
  Map val (iexp.get_val ()); // map AS`Name to AS`Type
  Map fns (iexp.get_fns ()); // map AS`Name to AS`FnType
  Map ops (iexp.get_ops ()); // map AS`Name to AS`OpType

  Generic nm;
  Map vm; // map AS`Name to REP`TypeRep
  for (bool bb = val.First (nm); bb; bb = val.Next (nm))
    vm.Insert (ExtName(modid,nm), TransType(modid, val[nm]));
  Map fm; // map AS`Name to REP`FnTypeRep
  for (bool cc = fns.First (nm); cc; cc = fns.Next (nm))
    fm.Insert (ExtName(modid,nm), TransType(modid, fns[nm]));
  Map om; // map AS`Name to REP`OpTypeRep
  for (bool dd = ops.First (nm); dd; dd = ops.Next (nm))
    om.Insert (ExtName(modid,nm), TransType(modid, ops[nm]));

  return mk_SSENV_ExpSigRep( Map(), vm, fm, Map(), om, Map(), Map() );
}
#endif // VDMSL

// ExportExplicitFunction
// modid : [AS`Name]
// efd : AS`ExplFnDef
// -> REP`FnTypeRep
TYPE_REP_FnTypeRep StatSem::ExportExplicitFunction (const Generic & modid, const TYPE_AS_ExplFnDef & efd) const
{
  return TransType (modid, efd.GetRecord(pos_AS_ExplFnDef_tp));
}

// ExportImplicitFunction
// modid : [AS`Name]
// ifd : AS`ImplFnDef
// -> REP`FnTypeRep
TYPE_REP_FnTypeRep StatSem::ExportImplicitFunction (const Generic & modid, const TYPE_AS_ImplFnDef & ifd) const
{
  SEQ<TYPE_AS_Type> domtps (ConstructImplDomType(ifd.GetSequence(pos_AS_ImplFnDef_partps)));
  TYPE_AS_Type rngtp (ConstructImplRngType(ifd.GetSequence(pos_AS_ImplFnDef_resnmtps)));

  return mk_REP_TotalFnTypeRep(TransTypeList (modid, domtps), TransType (modid, rngtp));
}

// ExportExtExplicitFunction
// modid : [AS`Name]
// ifd : AS`ExtExplFnDef
// -> REP`FnTypeRep
TYPE_REP_FnTypeRep StatSem::ExportExtExplicitFunction (const Generic & modid,
                                                       const TYPE_AS_ExtExplFnDef & eefd) const
{
  SEQ<TYPE_AS_Type> domtps (ConstructImplDomType(eefd.GetSequence(pos_AS_ExtExplFnDef_partps)));
  TYPE_AS_Type rngtp (ConstructImplRngType(eefd.GetSequence(pos_AS_ExtExplFnDef_resnmtps)));

  return mk_REP_TotalFnTypeRep(TransTypeList (modid, domtps), TransType (modid, rngtp));
}

// ExportExplicitOperation
// modid : [AS`Name]
// ifd : AS`ExplOpDef
// -> REP`OpTypeRep
TYPE_REP_OpTypeRep StatSem::ExportExplicitOperation (const Generic & modid, const TYPE_AS_ExplOpDef & efd) const
{
  return TransType (modid, efd.GetRecord(pos_AS_ExplOpDef_tp));
}

// ExportImplicitOperation
// modid : [AS`Name]
// ifd : AS`ImplOpDef
// -> REP`OpTypeRep
TYPE_REP_OpTypeRep StatSem::ExportImplicitOperation (const Generic & modid, const TYPE_AS_ImplOpDef & ifd) const
{
  SEQ<TYPE_AS_NameType> resnmtps (ifd.GetSequence(pos_AS_ImplOpDef_resnmtps));
  SEQ<TYPE_REP_TypeRep> domtps (TransTypeList (modid, ConstructImplDomType (ifd.GetSequence(pos_AS_ImplOpDef_partps))));

  TYPE_REP_TypeRep rngtp (TransType (modid, ConstructImplRngType(resnmtps)));

  return mk_REP_OpTypeRep(domtps, rngtp);
}

// ExportExtExplicitOperation
// modid : [AS`Name]
// ifd : AS`ExtExplOpDef
// -> REP`OpTypeRep
TYPE_REP_OpTypeRep StatSem::ExportExtExplicitOperation (const Generic & modid,
                                                        const TYPE_AS_ExtExplOpDef & eefd) const
{
  const SEQ<TYPE_AS_NameType> & resnmtps (eefd.GetSequence(pos_AS_ExtExplOpDef_resnmtps));
  const SEQ<TYPE_AS_PatTypePair> & partps (eefd.GetSequence(pos_AS_ExtExplOpDef_partps));
  SEQ<TYPE_REP_TypeRep> domtps (TransTypeList (modid, ConstructImplDomType (partps)));

  TYPE_REP_TypeRep rngtp (TransType (modid, ConstructImplRngType(resnmtps)));

  return mk_REP_OpTypeRep( domtps, rngtp );
}

// FindTypeVars
// fndef : AS`FnDef
// return : AS`TypeVarList
SEQ<TYPE_AS_TypeVar> StatSem::FindTypeVars (const TYPE_AS_FnDef & fndef) const
{
  switch(fndef.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:
      return fndef.GetSequence(pos_AS_ExplFnDef_tpparms);
    case TAG_TYPE_AS_ImplFnDef:
      return fndef.GetSequence(pos_AS_ImplFnDef_params);
    case TAG_TYPE_AS_ExtExplFnDef:
      return fndef.GetSequence(pos_AS_ExtExplFnDef_params);
    default:
      return SEQ<TYPE_AS_TypeVar>();
  }
}

#ifdef VDMSL
// GetFuncDefWOCtxt
// nm : AS`Name
// ==> [AS`FnDef]
Generic StatSem::GetFuncDefWOCtxt(const TYPE_AS_Name & nm)
{
  TYPE_AS_Name clnm (ASTAUX::GetFirstName(nm));
  Generic classInfo_g (GetModuleTypeRepWOCtxt(clnm)); // [ExpSigRep]
  if (classInfo_g.IsNil()) return Nil();

  TYPE_SSENV_ExpSigRep classInfo (classInfo_g);
  const Map & fcts (classInfo.GetMap(pos_SSENV_ExpSigRep_fcts));
  if (fcts.DomExists(nm))
    return fcts[nm];
  else
    return Nil();
}

// GetModuleTypeRepWOCtxt
// nm : AS`Name
// ==> [ExpSigRep]
Generic StatSem::GetModuleTypeRepWOCtxt(const TYPE_AS_Name & nm)
{
  if (this->ModuleEnv.DomExists(nm))
    return this->ModuleEnv[nm];
  else
    return Nil();
}
#endif // VDMSL

#ifdef VDMPP
#ifdef VICE
// IsSysClass
//  : AS`Name
// ==> bool
bool StatSem::IsSysClass(const TYPE_AS_Name & nm) const
{
  if ( this->ParseEnv.DomExists(nm) )
  {
    return this->ParseEnv[nm].GetBoolValue(pos_SSENV_ParseTypeInfo_sys);
  }
  return false;
}

TYPE_SSENV_ParseTypeInfo StatSem::CPUInfo()
{
  TYPE_SSENV_ParseTypeInfo pti;
  pti.Init(ASTAUX::MkNameFromId(ASTAUX::MkId(L"CPU"), NilContextId),
           Bool(false),
           SET<TYPE_AS_Name>(),
           Map (),
           Map (),
           Map (),
           Map (),
           Map (),
           CPUPriorityOperation(),
           Map (),
           CPUDeployOperation(),
           Int(NONE),
           CPUConstructor(),
           Map (),
           Map (),
           Bool(false));
  return pti;
}

TYPE_SSENV_ParseTypeInfo StatSem::BUSInfo()
{
  TYPE_SSENV_ParseTypeInfo pti;
  pti.Init(ASTAUX::MkNameFromId(ASTAUX::MkId(L"BUS"), NilContextId),
           Bool(false),
           SET<TYPE_AS_Name>(),
           Map (),
           Map (),
           Map (),
           Map (),
           Map (),
           Map (),
           Map (),
           Map (),
           Int(NONE),
           BUSConstructor(),
           Map (),
           Map (),
           Bool(false));
  return pti;
}

// CPUPriorityOperation
// +> map AS`Name to ENV`AccessOpTypeRep
Map StatSem::CPUPriorityOperation()
{
  TYPE_AS_Ids ids;
  ids.ImpAppend(ASTAUX::MkId(L"CPU"));
  ids.ImpAppend(ASTAUX::MkId(L"setPriority"));
  TYPE_AS_Name prionm (ASTAUX::MkNameFromIds(ids, NilContextId));

  SEQ<TYPE_REP_TypeRep> argptps;
  argptps.ImpAppend(TYPE_REP_AnyOpTypeRep());
  argptps.ImpAppend(btp_nat);
  TYPE_REP_TypeRep priotp (mk_REP_OpTypeRep(argptps, rep_unittp));
  //TYPE_SSENV_AccessOpTypeRep aotr (mk_SSENV_AccessOpTypeRep(priotp, Int(PUBLIC_AS), Bool(false)));
  TYPE_SSENV_AccessOpTypeRep aotr (MakeOpRng(priotp, Int(PUBLIC_AS), Bool(false), Bool(false)));
  
  Map res;
  res.ImpModify(prionm, aotr);
  return res;
}

// CPUDeployOperation
// +> map AS`Name to set of ENV`AccessOpTypeRep
Map StatSem::CPUDeployOperation()
{
  TYPE_AS_Ids ids;
  ids.ImpAppend(ASTAUX::MkId(L"CPU"));
  ids.ImpAppend(ASTAUX::MkId(L"deploy"));
  TYPE_AS_Name dplnm (ASTAUX::MkNameFromIds(ids, NilContextId));

  SEQ<TYPE_REP_TypeRep> argdtps;
  argdtps.ImpAppend(mk_REP_ObjRefTypeRep(ASTAUX::MkNameFromVoid()));
  TYPE_REP_TypeRep dpltp1 (mk_REP_OpTypeRep(argdtps, rep_unittp));
  argdtps.ImpAppend(mk_REP_SeqTypeRep(btp_char));
  TYPE_REP_TypeRep dpltp2 (mk_REP_OpTypeRep(argdtps, rep_unittp));

  //TYPE_SSENV_AccessOpTypeRep aotr1 (mk_SSENV_AccessOpTypeRep(dpltp1, Int(PUBLIC_AS), Bool(false)));
  //TYPE_SSENV_AccessOpTypeRep aotr2 (mk_SSENV_AccessOpTypeRep(dpltp2, Int(PUBLIC_AS), Bool(false)));
  TYPE_SSENV_AccessOpTypeRep aotr1 (MakeOpRng(dpltp1, Int(PUBLIC_AS), Bool(false), Bool(false)));
  TYPE_SSENV_AccessOpTypeRep aotr2 (MakeOpRng(dpltp2, Int(PUBLIC_AS), Bool(false), Bool(false)));
  
  Map res;
  res.ImpModify(dplnm, mk_set(aotr1, aotr2));
  return res;
}

// CPUConstructor
// +> set of ENV`AccessOpTypeRep
Set StatSem::CPUConstructor()
{
  TYPE_REP_QuoteTypeRep fcfs (mk_REP_QuoteTypeRep(ASTAUX::MkNameFromId(ASTAUX::MkId(L"FCFS"), NilContextId)));
  TYPE_REP_QuoteTypeRep fp (mk_REP_QuoteTypeRep(ASTAUX::MkNameFromId(ASTAUX::MkId(L"FP"), NilContextId)));
  SEQ<TYPE_REP_TypeRep> argtps;
  argtps.ImpAppend(mk_REP_UnionTypeRep(mk_set(fcfs, fp)));
  argtps.ImpAppend(btp_natone);
// 20091216 -->
//  TYPE_REP_TypeRep cputp (mk_REP_ObjRefTypeRep(ASTAUX::MkNameFromId(ASTAUX::MkId(L"CPU"), NilContextId)));
  TYPE_REP_TypeRep cputp (mk_REP_TypeNameRep(ASTAUX::MkNameFromId(ASTAUX::MkId(L"CPU"), NilContextId)));
// <-- 20091216
  TYPE_REP_TypeRep optp (mk_REP_OpTypeRep(argtps, cputp));
  //TYPE_SSENV_AccessOpTypeRep aotr (mk_SSENV_AccessOpTypeRep(optp, Int(PUBLIC_AS), Bool(false)));
  TYPE_SSENV_AccessOpTypeRep aotr (MakeOpRng(optp, Int(PUBLIC_AS), Bool(false), Bool(false)));
  
  return mk_set(aotr); 
}

// BUSConstructor
// +> set of ENV`AccessOpTypeRep
Set StatSem::BUSConstructor()
{
  TYPE_REP_QuoteTypeRep fcfs (mk_REP_QuoteTypeRep(ASTAUX::MkNameFromId(ASTAUX::MkId(L"FCFS"), NilContextId)));
  TYPE_REP_QuoteTypeRep tdma (mk_REP_QuoteTypeRep(ASTAUX::MkNameFromId(ASTAUX::MkId(L"TDMA"), NilContextId)));
  TYPE_REP_QuoteTypeRep csmacd (mk_REP_QuoteTypeRep(ASTAUX::MkNameFromId(ASTAUX::MkId(L"CSMACD"), NilContextId)));
  TYPE_REP_TypeRep cputp (mk_REP_ObjRefTypeRep(ASTAUX::MkNameFromId(ASTAUX::MkId(L"CPU"), NilContextId)));
  SEQ<TYPE_REP_TypeRep> argtps;
  argtps.ImpAppend(mk_REP_UnionTypeRep(mk_set(fcfs, tdma, csmacd)));
  argtps.ImpAppend(btp_natone);
  argtps.ImpAppend(mk_REP_SetTypeRep(cputp));
// 20091216 -->
//  TYPE_REP_TypeRep bustp (mk_REP_ObjRefTypeRep(ASTAUX::MkNameFromId(ASTAUX::MkId(L"BUS"), NilContextId)));
  TYPE_REP_TypeRep bustp (mk_REP_TypeNameRep(ASTAUX::MkNameFromId(ASTAUX::MkId(L"BUS"), NilContextId)));
// <-- 20091216
  TYPE_REP_TypeRep optp (mk_REP_OpTypeRep(argtps, bustp));
  //TYPE_SSENV_AccessOpTypeRep aotr (mk_SSENV_AccessOpTypeRep(optp, Int(PUBLIC_AS), Bool(false)));
  TYPE_SSENV_AccessOpTypeRep aotr (MakeOpRng(optp, Int(PUBLIC_AS), Bool(false), Bool(false)));
  
  Set res;
  res.Insert(aotr);
  return res; 
}

#endif // VICE

// GetFuncDefWOCtxt
// nm : AS`Name
// ==> [AS`FnDef]
Generic StatSem::GetFuncDefWOCtxt(const TYPE_AS_Name & nm)
{
  TYPE_AS_Name clnm (ASTAUX::GetFirstName(nm));
  Generic classInfo_g (GetClassTypeRepWOCtxt(clnm));
  if (classInfo_g.IsNil()) return Nil();
  TYPE_SSENV_ParseTypeInfo classInfo (classInfo_g);
  Map fndefs (classInfo.get_fndefs());
  TYPE_AS_Name fnm (ASTAUX::GetSecondName(nm));
  if (fndefs.DomExists(fnm))
    return fndefs[fnm];

  // Do search in superclasses!
  SET<TYPE_AS_Name> supers (classInfo.GetSet(pos_SSENV_ParseTypeInfo_super));
  if (supers.IsEmpty()) return Nil(); 
  return SearchFuncInSuper(nm, supers);
}

// SearchFuncInSuper
// nm : AS`Name
// sn : set of AS`Name
// ==> [AS`FnDef]
Generic StatSem::SearchFuncInSuper(const TYPE_AS_Name & nm, const SET<TYPE_AS_Name> & sn)
{
  SET<TYPE_AS_Name> supers (sn);
  Generic clnm;
  for (bool bb = supers.First(clnm); bb; bb = supers.Next(clnm))
  {
    Generic f (GetFuncDefWOCtxt(ASTAUX::Combine2Names(clnm, nm)));
    if (!f.IsNil()) return f;
  }
  return Nil();
}

// GetParseEnv : ()
// ==> ParseTypeEnv
Map StatSem::GetParseEnv()
{
  return this->ParseEnv;
}

// GetClassTypeRepWOCtxt
// nm : AS`Name
// ==> [ENV`ParseTypeInfo]
Generic StatSem::GetClassTypeRepWOCtxt(const TYPE_AS_Name & nm)
{
  return GetClassTypeRep(nm);
}

// GetClassTypeRep
// nm : AS`Name
// ==> [ENV`ParseTypeInfo]
Generic StatSem::GetClassTypeRep (const TYPE_AS_Name & nm) const
/* This method is called by the type checker  */
{
  if (this->ParseEnv.DomExists(nm))
    return this->ParseEnv[nm];
  else
    return Nil();  //dummy
}

// UpdateParseEnv
// nm : AS`Name
// cls : ENV`ParseTypeInfo
void StatSem::UpdateParseEnv (const TYPE_AS_Name & nm, const TYPE_SSENV_ParseTypeInfo & cls)
{
  this->ParseEnv.ImpModify (nm, cls);
}

// UpdateValuesInParseEnv
// nm : AS`Name
// vals : map AS`Name to REP`TypeRep
void StatSem::UpdateValuesInParseEnv(const TYPE_AS_Name & nm, const MAP<TYPE_AS_Name, TYPE_REP_TypeRep> & vals)
{
  if (this->ParseEnv.DomExists(nm))
  {
    TYPE_SSENV_ParseTypeInfo penv (this->ParseEnv[nm]);
    Map stval (penv.GetMap(pos_SSENV_ParseTypeInfo_vals)); // vals : map AS`Name to ENV`AccessTypeRep
    Set dom_stval (stval.Dom ());
    Map newstval;
    Set dom_vals (vals.Dom());
    Generic tnm;
    for (bool bb = dom_vals.First (tnm); bb; bb = dom_vals.Next (tnm))
    {
      if (dom_stval.InSet (tnm))
      {
        TYPE_SSENV_AccessTypeRep atr (stval[tnm]);
        newstval.Insert (tnm, mk_SSENV_AccessTypeRep(vals[tnm], atr.get_a (), atr.get_stat ()));
      }
    }
    penv.SetField(pos_SSENV_ParseTypeInfo_vals, newstval); // vals : map AS`Name to ENV`AccessTypeRep
    this->ParseEnv.ImpModify(nm, penv);
  }
}

// IsClassName
// p_nm : AS`Name
// return : bool
bool StatSem::IsClassName(const TYPE_AS_Name& p_nm) const
{
  return classNames.InSet(p_nm);
}

// GetTypes
// ==> set of AS`Name
SET<TYPE_AS_Name> StatSem::GetTypes() const
{
  return clstypes;
}

// UpdateParseEnvWithAST
// ast_l : AS`Document
// ==> bool
bool StatSem::UpdateParseEnvWithAST(const TYPE_AS_Document & ast_l)
{
  size_t len_ast_l = ast_l.Length(); 
  for (size_t idx = 1; idx <= len_ast_l; idx++)
  {
    this->classNames.Insert(ast_l[idx].GetRecord(pos_AS_Class_nm));
  }

  for (size_t i = 1; i <= len_ast_l; i++)
  {
    const TYPE_AS_Class & ast (ast_l[i]);

    const TYPE_AS_Name & nm (ast.GetRecord(pos_AS_Class_nm));
    const Bool & sys        (ast.GetBool(pos_AS_Class_sys));
    const Sequence & super  (ast.GetSequence(pos_AS_Class_supercls));
    const Generic & defs    (ast.GetField(pos_AS_Class_defs));

#ifdef VICE
    if ( nm == ASTAUX::MkNameFromId(ASTAUX::MkId(L"CPU"), NilContextId) )
    {
      GenErr (nm, ERR, 403, Sequence());
      return false;
    }
    else if ( nm == ASTAUX::MkNameFromId(ASTAUX::MkId(L"BUS"), NilContextId) )
    {
      GenErr (nm, ERR, 404, Sequence());
      return false;
    }
#endif // VICE

    SetCurClass(nm);

    if (defs.IsNil ())
    {
      TYPE_SSENV_ParseTypeInfo pti;
      pti.SetField(pos_SSENV_ParseTypeInfo_nm,           nm);
      pti.SetField(pos_SSENV_ParseTypeInfo_sys,          sys);
      pti.SetField(pos_SSENV_ParseTypeInfo_super,        super.Elems ());
      pti.SetField(pos_SSENV_ParseTypeInfo_tps,          Map ());
      pti.SetField(pos_SSENV_ParseTypeInfo_vals,         Map ());
      pti.SetField(pos_SSENV_ParseTypeInfo_insts,        Map ());
      pti.SetField(pos_SSENV_ParseTypeInfo_fcts,         Map ());
      pti.SetField(pos_SSENV_ParseTypeInfo_polys,        Map ());
      pti.SetField(pos_SSENV_ParseTypeInfo_ops,          Map ());
      pti.SetField(pos_SSENV_ParseTypeInfo_tags,         Map ());
      pti.SetField(pos_SSENV_ParseTypeInfo_overloaded,   Map ());
      pti.SetField(pos_SSENV_ParseTypeInfo_thread,       Int(NONE));
      pti.SetField(pos_SSENV_ParseTypeInfo_constructors, Set());
      pti.SetField(pos_SSENV_ParseTypeInfo_fndefs,       Map());
      pti.SetField(pos_SSENV_ParseTypeInfo_subresps,     Map());
      pti.SetField(pos_SSENV_ParseTypeInfo_instinv,      Bool(false));
      UpdateParseEnv (nm, pti);
    }
    else
    {
      TYPE_AS_Definitions asdef (defs);
      const MAP<TYPE_AS_Name,TYPE_AS_TypeDef> & typem (asdef.GetMap(pos_AS_Definitions_typem));
      const SEQ<TYPE_AS_ValueDef> & valuem (asdef.GetSequence(pos_AS_Definitions_valuem));
      const MAP<TYPE_AS_Name,TYPE_AS_FnDef> & fnm (asdef.GetMap(pos_AS_Definitions_fnm));
      const MAP<TYPE_AS_Name,TYPE_AS_OpDef> & opm (asdef.GetMap(pos_AS_Definitions_opm));
      const SEQ<TYPE_AS_InstanceVarDef> & instvars (asdef.GetSequence(pos_AS_Definitions_instvars));
      const Generic & thread (asdef.GetField(pos_AS_Definitions_threaddef));

      this->clstypes = typem.Dom();

      Set dom_fnm (fnm.Dom());
      Map nfm, pfm;
      Generic fnnm;
      for (bool cc = dom_fnm.First (fnnm); cc; cc = dom_fnm.Next (fnnm))
      {
        TYPE_AS_FnDef fd (fnm[fnnm]);
        if (FindTypeVars (fnm[fnnm]).IsEmpty())
          nfm.Insert (fnnm, fd);
        else
          pfm.Insert (fnnm, fd);
      }

      Tuple infer (ExportTypes (nm, typem));
      const Map & tps (infer.GetMap (1)); // TODO
      const Map & tags (infer.GetMap (2));
      Map invs (ExportInvs (nm, typem));
      Map vals (ExportValues (nm, valuem));
      Tuple instm (ExportInstVars (nm, instvars));
      Int td (ExportThread(thread));
      Map fcts (ExportFunctions (nm, nfm));
      Map implfns (ExportPrePostFns (nm, nfm));
      Map polys (ExportPolyFns (nm, pfm));
      Map ops (ExportOperations (nm, opm));
      Map overloads (ExportOverloads(nm, opm, fnm));
      Set constructors (ExportConstructors(nm, opm));
      Map subresps (ExportSubresps(nm, nfm, pfm, opm));

      TYPE_SSENV_ParseTypeInfo pti;
      pti.SetField(pos_SSENV_ParseTypeInfo_nm,           nm);
      pti.SetField(pos_SSENV_ParseTypeInfo_sys,          sys);
      pti.SetField(pos_SSENV_ParseTypeInfo_super,        super.Elems ());
      pti.SetField(pos_SSENV_ParseTypeInfo_tps,          tps);
      pti.SetField(pos_SSENV_ParseTypeInfo_vals,         vals);
      pti.SetField(pos_SSENV_ParseTypeInfo_insts,        instm.GetMap(1));
      pti.SetField(pos_SSENV_ParseTypeInfo_fcts,         invs.ImpOverride (fcts).ImpOverride (implfns));
      pti.SetField(pos_SSENV_ParseTypeInfo_polys,        polys);
      pti.SetField(pos_SSENV_ParseTypeInfo_ops,          ops);
      pti.SetField(pos_SSENV_ParseTypeInfo_tags,         tags);
      pti.SetField(pos_SSENV_ParseTypeInfo_overloaded,   overloads);
      pti.SetField(pos_SSENV_ParseTypeInfo_thread,       td);
      pti.SetField(pos_SSENV_ParseTypeInfo_constructors, constructors);
      pti.SetField(pos_SSENV_ParseTypeInfo_fndefs,       fnm);
      pti.SetField(pos_SSENV_ParseTypeInfo_subresps,     subresps);
      pti.SetField(pos_SSENV_ParseTypeInfo_instinv,      instm.GetBool(2));

      this->clstypes.Clear();
      UpdateParseEnv (nm, pti);
    }
  }
  return true;
}

// RemoveClassFromParseEnv
// cls_s : set of ProjectTypes`ModuleName
void StatSem::RemoveClassFromParseEnv(const SET<TYPE_AS_Name> & cls_s)
{
  SET<TYPE_AS_Name> clsnms (this->classNames);
  Generic clsnm;
  for( bool bb = clsnms.First(clsnm); bb; bb = clsnms.Next(clsnm) )
  {
#ifdef VICE
    if( (clsnm == ASTAUX::MkNameFromId(ASTAUX::MkId(L"CPU"), NilContextId)) ||
        (clsnm == ASTAUX::MkNameFromId(ASTAUX::MkId(L"BUS"), NilContextId)) )
      continue;
#endif // VICE 
    if( !cls_s.InSet(clsnm) )
    {
      this->classNames.RemElem( clsnm );
      if( this->ParseEnv.DomExists( clsnm ) )
        this->ParseEnv.RemElem( clsnm );
    }
  }
}
#endif //VDMPP
#endif //! NOSS

