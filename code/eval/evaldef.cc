#include "evaldef.h"
#include "asquotes.h"
#include "astaux.h"
#include "stackeval.h"
#include "libman.h"
#include "oldname.h"
#ifdef VDMPP
#include "MANGLE.h"
#endif // VDMPP
#include "rt_errmsg.h"
#include "evalclass.h"
#include "evalaux.h"
#include "settings.h"
#include "intconvquotes.h"

//wcout << L"intertp: " << INT2Q::h2gAS(intertp) << endl;

//////////////////////////////////////////////////
//             Default Access
//////////////////////////////////////////////////
#define Default_Op Int(PRIVATE_AS)
#define Default_Fn Int(PRIVATE_AS)
#define Default_Val Int(PRIVATE_AS)
#define Default_Tp Int(PRIVATE_AS)
#define Default_Inst Int(PRIVATE_AS)

#ifdef VDMSL
// TranslateDefinitions
// mod_id : AS`Name
// AS`Definitions
// ==> GLOBAL`SigmaDEFS
TYPE_GLOBAL_SigmaDEFS DEF::TranslateDefinitions (const TYPE_AS_Name & mod_id, const TYPE_AS_Definitions & defs)
{
  const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & tm (defs.GetMap     (pos_AS_Definitions_typem));
  const SEQ<TYPE_AS_ValueDef> & vm              (defs.GetSequence(pos_AS_Definitions_valuem));
  const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & fnm  (defs.GetMap     (pos_AS_Definitions_fnm));
  const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & opm  (defs.GetMap     (pos_AS_Definitions_opm));
  const Generic & gst                           (defs.GetField   (pos_AS_Definitions_State));

  // types
  MAP<TYPE_AS_Name, TYPE_AS_TypeDef> tps (tm);

  // functions
  Tuple fns (TransFnMap (mod_id, fnm));
  Map efn (fns.GetMap (1)); // map AS`Name to (SEM`CompExplFN * AS`Access)

  // operations
  //Tuple ops (TransOpMap (mod_id, opm));
  Map eop (TransOpMap (mod_id, opm));

  // recsel
  Map recsel (TransTpDefMap (tm));

  // stp_id
  Generic stp_id = Nil();

  // gs
  if (! gst.IsNil ()) {
    const TYPE_AS_Type & tp (Record(gst).GetRecord(pos_AS_StateDef_tp));

    stp_id = tp.GetRecord(pos_AS_CompositeType_name);

    if (recsel.DomExists (stp_id)) {
      RTERR::InitError (L"TranslateDefinitions", RTERR_TYPE_ALREADY_DEF, Nil(), Nil(), ASTAUX::GetCid(tp), Sequence());
    }

    recsel.ImpOverride (TransTP(Set().Insert(mk_(tp, Int(PUBLIC_AS)))));
    tps.ImpOverride (GenStateTypeDef (gst));
    efn.ImpOverride (CreateStateInvAndInit(mod_id, gst));
  }

  efn.ImpOverride (CreateInvs (mod_id, tm));
  efn.ImpOverride (CreateOperationPrePostFns (mod_id, opm, gst));

  TYPE_GLOBAL_SigmaDEFS sigmadefs;
  sigmadefs.SetField(pos_GLOBAL_SigmaDEFS_explfns,   efn);
  sigmadefs.SetField(pos_GLOBAL_SigmaDEFS_explops,   eop); // eop
  sigmadefs.SetField(pos_GLOBAL_SigmaDEFS_explpolys, fns.GetMap (2)); // epf
  sigmadefs.SetField(pos_GLOBAL_SigmaDEFS_tps,       tps);
  sigmadefs.SetField(pos_GLOBAL_SigmaDEFS_recsel,    recsel);
  sigmadefs.SetField(pos_GLOBAL_SigmaDEFS_gv_uinit,  vm);
  sigmadefs.SetField(pos_GLOBAL_SigmaDEFS_gs_uinit,  gst);
  sigmadefs.SetField(pos_GLOBAL_SigmaDEFS_stp_uid,   stp_id);
  sigmadefs.SetField(pos_GLOBAL_SigmaDEFS_stateinv,  StateInvariant (gst));

  return sigmadefs;
}

TYPE_GLOBAL_SigmaMO DEF::EmptySigmaMO()
{
  TYPE_GLOBAL_SigmaMO sigmamo;
  sigmamo.SetField(pos_GLOBAL_SigmaMO_explfns,   Map());
  sigmamo.SetField(pos_GLOBAL_SigmaMO_explops,   Map());
  sigmamo.SetField(pos_GLOBAL_SigmaMO_explpolys, Map());
  sigmamo.SetField(pos_GLOBAL_SigmaMO_tps,       Map());
  sigmamo.SetField(pos_GLOBAL_SigmaMO_recsel,    Map());
  sigmamo.SetField(pos_GLOBAL_SigmaMO_gv,        Nil());
  sigmamo.SetField(pos_GLOBAL_SigmaMO_gv_uinit,  Sequence());
  sigmamo.SetField(pos_GLOBAL_SigmaMO_gv_uinst,  Sequence());
  sigmamo.SetField(pos_GLOBAL_SigmaMO_gs,        Map());
  sigmamo.SetField(pos_GLOBAL_SigmaMO_gs_uinit,  Nil());
  sigmamo.SetField(pos_GLOBAL_SigmaMO_stp_uid,   Nil());
  sigmamo.SetField(pos_GLOBAL_SigmaMO_stateinv,  Nil());
  sigmamo.SetField(pos_GLOBAL_SigmaMO_exp,       Nil());
  sigmamo.SetField(pos_GLOBAL_SigmaMO_imp,       Map());
  sigmamo.SetField(pos_GLOBAL_SigmaMO_ren,       Map());
  return sigmamo;
}
#endif //VDMSL

#ifdef VDMPP
// ReadClasses
// cs : AS`Class
// ==> map AS`Name to GLOBAL`SigmaClass
MAP<TYPE_AS_Name,TYPE_GLOBAL_SigmaClass> DEF::ReadClasses(const TYPE_AS_Class & cs)
{
  const TYPE_AS_Name & nm (cs.GetRecord(pos_AS_Class_nm));
  const SEQ<TYPE_AS_Name> & supercls (cs.GetSequence(pos_AS_Class_supercls));
  const Generic & defs (cs.GetField(pos_AS_Class_defs));

  SemRec::SetCurrentModClass(nm);

  TYPE_GLOBAL_SigmaClass sigmacl(EmptySigmaClass());

  theCompiler().SetClMod(nm);
  theCompiler().ResetProgramTable(nm);

  SET<TYPE_AS_Name> inh (TransInheritance(supercls));
  sigmacl.SetField(pos_GLOBAL_SigmaClass_inhcon, inh);
  if (defs.IsNil()) {
    MAP<TYPE_AS_Name,TYPE_GLOBAL_SigmaClass> res;
    res.Insert(nm, sigmacl);
    return res;
  }

  TYPE_AS_Definitions td (defs);
  const MAP<TYPE_AS_Name,TYPE_AS_TypeDef> & typem (td.GetMap(pos_AS_Definitions_typem));
  const SEQ<TYPE_AS_ValueDef> & valuem (td.GetSequence(pos_AS_Definitions_valuem));
  const MAP<TYPE_AS_Name,TYPE_AS_FnDef> & fnm (td.GetMap(pos_AS_Definitions_fnm));
  const MAP<TYPE_AS_Name,TYPE_AS_OpDef> & opm (td.GetMap(pos_AS_Definitions_opm));
  const SEQ<TYPE_AS_InstanceVarDef> & as_instvars (td.GetSequence(pos_AS_Definitions_instvars));

  Tuple tiv( TransInstVars(as_instvars));

  sigmacl.SetField(pos_GLOBAL_SigmaClass_instvars,        tiv.GetSequence(1));
  sigmacl.SetField(pos_GLOBAL_SigmaClass_instvars_utp,    tiv.GetMap(3));

  TYPE_STKM_SubProgram code;
  if (!tiv.GetSequence(2).IsEmpty()) {
    SEQ<TYPE_AS_InstanceInv> instinv (tiv.GetSequence(2));
    size_t len_instinv = instinv.Length();
    for (size_t i = len_instinv; i >= 1; i--) {
      TYPE_STKM_SubProgram cond (theCompiler().E2I(instinv[i].GetRecord(pos_AS_InstanceInv_expr)));
      //code = (i == 1 ? cond :
      code = (i == len_instinv ? cond :
                       theCompiler().ConcIfThenElse(cond, code,
                               TYPE_STKM_SubProgram().ImpAppend(TYPE_INSTRTP_PUSH().Init(sem_false)))); 
    }
  }
  sigmacl.SetField(pos_GLOBAL_SigmaClass_inst_uinv,       mk_(tiv.GetSequence(2), code));
  sigmacl.SetField(pos_GLOBAL_SigmaClass_inst_uinit_uval, TYPE_SEM_InsStrct());
  sigmacl.SetField(pos_GLOBAL_SigmaClass_vls_udef,        valuem);
  sigmacl.SetField(pos_GLOBAL_SigmaClass_vls_uinit,       TYPE_GLOBAL_ValueMap());

  Tuple tfm (TransFnMap(nm, fnm));
  sigmacl.SetField(pos_GLOBAL_SigmaClass_explfns,   tfm.GetMap(1));
  sigmacl.SetField(pos_GLOBAL_SigmaClass_explpolys, tfm.GetMap(2));

  Map eop (TransOpMap(nm, opm));
  sigmacl.SetField(pos_GLOBAL_SigmaClass_explops,   eop);

  sigmacl.SetField(pos_GLOBAL_SigmaClass_overloaded, TransOverloaded(opm, fnm));

  Map prepostfns (CreateOperationPrePostFns(nm, opm, Nil()));
  Map res_explfns (sigmacl.GetMap(pos_GLOBAL_SigmaClass_explfns));
  res_explfns.ImpOverride(CreateInvs(nm, typem));
  res_explfns.ImpOverride(prepostfns);
  sigmacl.SetField(pos_GLOBAL_SigmaClass_explfns, res_explfns);

  sigmacl.SetField(pos_GLOBAL_SigmaClass_all_ufns_uops_upolys, MergeFnsOpsPolys(sigmacl));

  sigmacl.SetField(pos_GLOBAL_SigmaClass_localtps, UpdateTypeDefs(typem, theCompiler().GetClMod()));
  sigmacl.SetField(pos_GLOBAL_SigmaClass_recsel, TransLocalRecSel(typem));
  sigmacl.SetField(pos_GLOBAL_SigmaClass_localhchy, Map());

  Map m (ExtractStaticMembers(fnm, opm, sigmacl));
  Set nm_s (m.Dom());
  Map statics;
  Generic name;
  for (bool bb = nm_s.First(name); bb; bb = nm_s.Next(name)) {
    TYPE_SEM_VAL val (m[name]);
    switch (val.GetTag()) {
      case TAG_TYPE_SEM_CompExplFN: {
        const SEQ<TYPE_SEM_ExplFN> & fl (val.GetSequence(pos_SEM_CompExplFN_fl));
        statics.Insert(name, mk_(val, Nil(), fl[1].GetField(pos_SEM_ExplFN_access)));
        break;
      }
      case TAG_TYPE_SEM_ExplPOLY: {
        statics.Insert(name, mk_(val, Nil(), val.GetField(pos_SEM_ExplPOLY_access)));
        break;
      }
      case TAG_TYPE_SEM_ExplOP: {
        statics.Insert(name, mk_(val, Nil(), val.GetField(pos_SEM_ExplOP_access)));
        break;
      }
    } 
  }
  sigmacl.SetField(pos_GLOBAL_SigmaClass_statics, statics);
  sigmacl.SetField(pos_GLOBAL_SigmaClass_defaultcons, theState().GetDefaultCons(nm));

  MAP<TYPE_AS_Name,TYPE_GLOBAL_SigmaClass> res;
  res.Insert(nm, sigmacl);
  return res;
}

// EmptySigmaClass
TYPE_GLOBAL_SigmaClass DEF::EmptySigmaClass()
{
  TYPE_GLOBAL_SigmaClass sigmacl;
  sigmacl.Init( SET<TYPE_AS_Name>(),                           // inhcon
                SEQ<TYPE_AS_InstAssignDef>(),                  // instvars
                MAP<TYPE_AS_Name, TYPE_AS_Type>(),             // instvars_tp
                Tuple(),                                       // inst_inv
                TYPE_SEM_InsStrct(),                           // inst_init_val
                SEQ<TYPE_AS_ValueDef>(),                       // vls_def
                TYPE_GLOBAL_ValueMap(),                        // vls_init
                MAP<TYPE_AS_Name, type_7AS_NameC14SEM_CompExplFNCM>(),// explfns
                MAP<TYPE_AS_Name, type_7AS_NameC10SEM_ExplOPCM>(),    // explops
                MAP<TYPE_AS_Name, TYPE_GLOBAL_Overloaded>(),   // overloaded
                MAP<TYPE_AS_Name, type_7AS_NameC12SEM_ExplPOLYCM>(),  // explpolys
                MAP<TYPE_AS_Name, TYPE_AS_TypeDef>(),          // localtps
                MAP<TYPE_AS_Name, type_i7AS_NameCiMdLU4P>(),   // recsel
                MAP<TYPE_AS_Name, type_7AS_NameCS>(),          // localhchy
                Bool(false),                                   // isinit
                (Generic)Map(),                                // constructor
                Bool(false),                                   // defaultcons
                TYPE_GLOBAL_ValueMap(),                        // statics
                MAP<TYPE_AS_Name, TYPE_SEM_History>(),         // statichist
                MAP<TYPE_AS_Name, type_dL>(),                  // perm_pred
                Nil(),                                         // thread
#ifdef VICE
                (Generic)Map(),                                // depnms
                (Generic)Map(),                                // depops
                Bool(false),                                   // sys
#endif //VICE
                (Generic)Map() );                          // all_fns_ops_polys
  return sigmacl;
}

// UpdateConstructors
// cls : AS`Name
// supercls : (seq of AS`Name)
// opm : (map AS`Name to AS`OpDef)
// instvars : seq of AS`InstAssignDef
// ==> map seq of AS`Type to (STKM`DebugCmd * AS`Access)
Map DEF::UpdateConstructors(const TYPE_AS_Name & cls,
                            const SEQ<TYPE_AS_Name> & supercls,
                            const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & opm,
                            const SEQ<TYPE_AS_InstAssignDef> & instvars)
{
//  SEQ<TYPE_AS_Name> superlist;
//  if (!supercls.IsEmpty())
//    superlist = theState().AllSuperList(1, supercls);
//  return CLASS::CreateConstructor(cls, instvars, superlist, opm);

  return CLASS::CreateConstructor(cls, instvars, supercls, opm);
}

// MergeFnsOpsPolys
// sigmacl : GLOBAL`SigmaClass
// -> GLOBAL`All_Fns_Ops_Polys
Map DEF::MergeFnsOpsPolys(const TYPE_GLOBAL_SigmaClass & sigmacl)
{
  Map m;
//  m.ImpOverride(sigmacl.GetMap(pos_GLOBAL_SigmaClass_explfns));
//  m.ImpOverride(sigmacl.GetMap(pos_GLOBAL_SigmaClass_explops));
//  m.ImpOverride(sigmacl.GetMap(pos_GLOBAL_SigmaClass_explpolys));
  Map explfns (sigmacl.GetMap(pos_GLOBAL_SigmaClass_explfns));
  Set dom_explfns (explfns.Dom());
  Generic nm;
  for (bool bb = dom_explfns.First(nm); bb; bb = dom_explfns.Next(nm)) {
    TYPE_SEM_CompExplFN fn(explfns[nm]);
    const SEQ<TYPE_SEM_ExplFN> & fl (fn.GetSequence(pos_SEM_CompExplFN_fl));
    m.Insert(nm, mk_(fn, fl[1].GetField(pos_SEM_ExplFN_access)));
  }
  Map explops (sigmacl.GetMap(pos_GLOBAL_SigmaClass_explops));
  Set dom_explops (explops.Dom());
  for (bool cc = dom_explops.First(nm); cc; cc = dom_explops.Next(nm)) {
    TYPE_SEM_ExplOP op(explops[nm]);
    m.Insert(nm, mk_(op, op.GetField(pos_SEM_ExplOP_access)));
  }
  Map explpolys (sigmacl.GetMap(pos_GLOBAL_SigmaClass_explpolys));
  Set dom_explpolys (explpolys.Dom());
  for (bool dd = dom_explpolys.First(nm); dd; dd = dom_explpolys.Next(nm)) {
    TYPE_SEM_ExplPOLY fn(explpolys[nm]);
    m.Insert(nm, mk_(fn, fn.GetField(pos_SEM_ExplPOLY_access)));
  }
  return m;
}

// UpdateTypeDefs
// typem : (map AS`Name to AS`TypeDef)
// curcls : AS`Name
// +> map AS`Name to AS`TypeDef
MAP<TYPE_AS_Name,TYPE_AS_TypeDef> DEF::UpdateTypeDefs(const MAP<TYPE_AS_Name,TYPE_AS_TypeDef> & typem,
                                                      const TYPE_AS_Name & curcls)
{
  Set dom_typem (typem.Dom());
  MAP<TYPE_AS_Name,TYPE_AS_TypeDef> res;
  Generic nm;
  for (bool bb = dom_typem.First(nm); bb; bb = dom_typem.Next(nm)) {
    TYPE_AS_TypeDef td (typem[nm]);
    td.set_shape(UpdateType(td.GetRecord(pos_AS_TypeDef_shape), curcls));
    td.set_access(RealAccess(td.GetField(pos_AS_TypeDef_access), ACC_OP));
    res.Insert(nm, td);
  }
  return res;
}

// UpdateType
// type : AS`Type
// curcls : AS`Name
// +> AS`Type
TYPE_AS_Type DEF::UpdateType(const TYPE_AS_Type & type, const TYPE_AS_Name & curcls)
{
  switch (type.GetTag()) {
    case TAG_TYPE_AS_TypeName: {
      const TYPE_AS_Name & nm (type.GetRecord(pos_AS_TypeName_name));
      if (theState().AClass(nm)) {
        return type;
      }
      else if (nm.GetSequence(pos_AS_Name_ids).Length() == 1) {
        TYPE_AS_Ids ids (curcls.GetSequence(pos_AS_Name_ids));
        ids.ImpAppend(nm.GetSequence(pos_AS_Name_ids).Hd());
        return TYPE_AS_TypeName().Init(TYPE_AS_Name().Init(ids, type.GetInt(pos_AS_TypeName_cid)),
                                       type.GetInt(pos_AS_TypeName_cid));
      }
      else {
        return type;
      }
    }
  
    case TAG_TYPE_AS_CompositeType: {
      const SEQ<TYPE_AS_Field> & fields (type.GetSequence(pos_AS_CompositeType_fields));
      SEQ<TYPE_AS_Field> new_fls;
      size_t len = fields.Length();
      for (size_t i = 1; i <= len; i++) {
        TYPE_AS_Field f (fields[i]);
        f.SetField(pos_AS_Field_type, UpdateType(f.GetRecord(pos_AS_Field_type), curcls));
        new_fls.ImpAppend(f);
      }
      return TYPE_AS_CompositeType().Init(type.GetRecord(pos_AS_CompositeType_name),
                                          new_fls,
                                          type.GetInt(pos_AS_CompositeType_cid));
    }
  
    case TAG_TYPE_AS_UnionType: {
      const SEQ<TYPE_AS_Type> & typeset (type.GetSequence(pos_AS_UnionType_tps));
      SEQ<TYPE_AS_Type> new_ts;
      size_t len = typeset.Length();
      for (size_t i = 1; i <= len; i++ ) {
        new_ts.ImpAppend(UpdateType(typeset[i], curcls));
      }
      return TYPE_AS_UnionType().Init(new_ts, type.GetInt(pos_AS_UnionType_cid));
    }
  
    case TAG_TYPE_AS_ProductType: {
      const SEQ<TYPE_AS_Type> & typeseq (type.GetSequence(pos_AS_ProductType_tps));
      SEQ<TYPE_AS_Type> new_ts;
      size_t len = typeseq.Length();
      for (size_t i = 1; i <= len; i++) {
        new_ts.ImpAppend(UpdateType(typeseq[i], curcls));
      }
      return TYPE_AS_ProductType().Init(new_ts, type.GetInt(pos_AS_ProductType_cid));
    }
  
    case TAG_TYPE_AS_OptionalType: {
      return TYPE_AS_OptionalType().Init(UpdateType(type.GetRecord(pos_AS_OptionalType_tp), curcls),
                                         type.GetInt(pos_AS_OptionalType_cid));
    }
  
    case TAG_TYPE_AS_Set0Type: {
      return TYPE_AS_Set0Type().Init(UpdateType(type.GetRecord(pos_AS_Set0Type_elemtp), curcls),
                                    type.GetInt(pos_AS_Set0Type_cid));
    }
  
    case TAG_TYPE_AS_Set1Type: {
      return TYPE_AS_Set1Type().Init(UpdateType(type.GetRecord(pos_AS_Set1Type_elemtp), curcls),
                                    type.GetInt(pos_AS_Set1Type_cid));
    }
  
    case TAG_TYPE_AS_BracketedType: {
      return TYPE_AS_BracketedType().Init(UpdateType(type.GetRecord(pos_AS_BracketedType_tp), curcls),
                                          type.GetInt(pos_AS_BracketedType_cid));
    }
  
    case TAG_TYPE_AS_Seq0Type: {
      return TYPE_AS_Seq0Type().Init(UpdateType(type.GetRecord(pos_AS_Seq0Type_elemtp), curcls),
                                     type.GetInt(pos_AS_Seq0Type_cid));
    }
  
    case TAG_TYPE_AS_Seq1Type: {
      return TYPE_AS_Seq1Type().Init(UpdateType(type.GetRecord(pos_AS_Seq1Type_elemtp), curcls),
                                     type.GetInt(pos_AS_Seq1Type_cid));
    }
  
    case TAG_TYPE_AS_GeneralMap0Type: {
      return TYPE_AS_GeneralMap0Type().Init(UpdateType(type.GetRecord(pos_AS_GeneralMap0Type_mapdom), curcls),
                                           UpdateType(type.GetRecord(pos_AS_GeneralMap0Type_maprng), curcls),
                                           type.GetInt(pos_AS_GeneralMap0Type_cid));
    }
  
    case TAG_TYPE_AS_GeneralMap1Type: {
      return TYPE_AS_GeneralMap1Type().Init(UpdateType(type.GetRecord(pos_AS_GeneralMap1Type_mapdom), curcls),
                                           UpdateType(type.GetRecord(pos_AS_GeneralMap1Type_maprng), curcls),
                                           type.GetInt(pos_AS_GeneralMap1Type_cid));
    }
  
    case TAG_TYPE_AS_InjectiveMap0Type: {
      return TYPE_AS_InjectiveMap0Type().Init(UpdateType(type.GetRecord(pos_AS_InjectiveMap0Type_mapdom), curcls),
                                             UpdateType(type.GetRecord(pos_AS_InjectiveMap0Type_maprng), curcls),
                                             type.GetInt(pos_AS_InjectiveMap0Type_cid));
    }
  
    case TAG_TYPE_AS_InjectiveMap1Type: {
      return TYPE_AS_InjectiveMap1Type().Init(UpdateType(type.GetRecord(pos_AS_InjectiveMap1Type_mapdom), curcls),
                                             UpdateType(type.GetRecord(pos_AS_InjectiveMap1Type_maprng), curcls),
                                             type.GetInt(pos_AS_InjectiveMap1Type_cid));
    }
  
    case TAG_TYPE_AS_PartialFnType: {
      const SEQ<TYPE_AS_Type> & dt (type.GetSequence(pos_AS_PartialFnType_fndom));
      SEQ<TYPE_AS_Type> new_dt;
      size_t len = dt.Length();
      for (size_t i = 1; i <= len; i++) {
        new_dt.ImpAppend(UpdateType(dt[i], curcls));
      }
      return TYPE_AS_PartialFnType().Init(new_dt,
                                          UpdateType(type.GetRecord(pos_AS_PartialFnType_fnrng), curcls),
                                          type.GetInt(pos_AS_PartialFnType_cid));
    }
  
    case TAG_TYPE_AS_TotalFnType: {
      const SEQ<TYPE_AS_Type> & dt (type.GetSequence(pos_AS_TotalFnType_fndom));
      SEQ<TYPE_AS_Type> new_dt;
      size_t len = dt.Length();
      for (size_t i = 1; i <= len; i++) {
        new_dt.ImpAppend(UpdateType(dt[i], curcls));
      }
      return TYPE_AS_TotalFnType().Init(new_dt,
                                        UpdateType(type.GetRecord(pos_AS_TotalFnType_fnrng), curcls),
                                        type.GetInt(pos_AS_TotalFnType_cid));
    }
    case TAG_TYPE_AS_OpType: {
      const SEQ<TYPE_AS_Type> & dt (type.GetSequence(pos_AS_OpType_opdom));
      SEQ<TYPE_AS_Type> new_dt;
      size_t len = dt.Length();
      for (size_t i = 1; i <= len; i++) {
        new_dt.ImpAppend(UpdateType(dt[i], curcls));
      }
      return TYPE_AS_OpType().Init(new_dt,
                                   UpdateType(type.GetRecord(pos_AS_OpType_oprng), curcls),
                                   type.GetInt(pos_AS_OpType_cid));
    }
//
    default: {
      return type;
    }
  }
}
#endif // VDMPP

#ifdef VDMSL
// GenStateTypeDef
// sd : AS`StateDef
// ==> map AS`Name to AS`TypeDef
MAP<TYPE_AS_Name,TYPE_AS_TypeDef> DEF::GenStateTypeDef (const TYPE_AS_StateDef & sd)
{
  const TYPE_AS_Type & comp_t (sd.GetRecord(pos_AS_StateDef_tp));
  const TYPE_AS_Name & id (comp_t.GetRecord(pos_AS_CompositeType_name));
  const Generic & Inv (sd.GetField(pos_AS_StateDef_Inv));

  TYPE_AS_TypeDef td;
  td.Init(id, comp_t, Inv, Nil(), Nil(), Default_Tp, id.GetInt(pos_AS_Name_cid));

  Map res_m;
  res_m.Insert (id, td);

  return (res_m);
}

// StateInvariant
// e : [AS`StateDef]
// ==> [GLOBAL`StateInv(AS`Name * seq of AS`Name * AS`Pattern * AS`Expr)]
Generic DEF::StateInvariant (const Generic & e)
{
  if (e.IsNil ()) {
    return Nil ();
  }
  TYPE_AS_StateDef sd (e);
  if (sd.GetField(pos_AS_StateDef_Inv).IsNil ()) {
    return Nil ();
  }
  const TYPE_AS_Type & comp_t (sd.GetRecord(pos_AS_StateDef_tp)); // CompositeType
  const TYPE_AS_Name & name (comp_t.GetRecord(pos_AS_CompositeType_name)); // AS`Name
  const SEQ<TYPE_AS_Field> & fields_lv (comp_t.GetSequence(pos_AS_CompositeType_fields));

  const TYPE_AS_Invariant & invariant (sd.GetRecord(pos_AS_StateDef_Inv)); // AS`Invariant
  const TYPE_AS_Pattern & pat_p (invariant.GetRecord(pos_AS_Invariant_pat));
  const TYPE_AS_Expr & expr_e (invariant.GetRecord(pos_AS_Invariant_expr));

  size_t maxi = fields_lv.Length ();
  SEQ<TYPE_AS_Name> id_l;
  for (size_t i = 1; i <= maxi; i++) {
    id_l.ImpAppend (fields_lv[i].GetField(pos_AS_Field_sel));
  };
  return mk_(name, id_l, pat_p, expr_e, theCompiler().P2P(pat_p), theCompiler().E2I(expr_e));
}
#endif // VDMSL

#ifdef VDMPP
// TransInheritance
// inh_l : seq of AS`Name
// -> GLOBAL`InhCon(set of AS`Name)
SET<TYPE_AS_Name> DEF::TransInheritance(const SEQ<TYPE_AS_Name> & inh_l)
{
  return inh_l.Elems();
}

// TransInstVars
// instvardef_l : seq of AS`InstanceVarDef
// -> seq of AS`InstAssignDef * seq of AS`InstanceInv * map AS`Name to AS`Type
Tuple DEF::TransInstVars(const SEQ<TYPE_AS_InstanceVarDef> & instvardef_l)
{
  SEQ<TYPE_AS_InstAssignDef> instvars;
  SEQ<TYPE_AS_InstanceInv> instinv;
  Map instvars_tp; // map AS`Name to AS`Type

  size_t len_instvardef_l = instvardef_l.Length();
  for (size_t idx = 1; idx <= len_instvardef_l; idx++) {
    const TYPE_AS_InstanceVarDef & ivd (instvardef_l[idx]);
    switch(ivd.GetTag()) {
      case TAG_TYPE_AS_InstAssignDef: {
        TYPE_AS_AssignDef ad (ivd.GetRecord(pos_AS_InstAssignDef_ad)); // AssignDef
        instvars.ImpAppend(ivd);
        instvars_tp.Insert(ad.GetRecord(pos_AS_AssignDef_var), ad.GetRecord(pos_AS_AssignDef_tp));
        break;
      }
      case TAG_TYPE_AS_InstanceInv: {
        instinv.ImpAppend(ivd);
        break;
      }
      default:
        break;
    }
  }
  return mk_(instvars, instinv, instvars_tp);
}

// ExtractStaticMembers
// fnm : map AS`Name to AS`FnDef
// opm : map AS`Name to AS`OpDef
// sigma : GLOBAL`SigmaClass
// ==> map AS`Name to (SEM`CompExplFN | SEM`ExplPOLY | SEM`ExplOP)
MAP<TYPE_AS_Name, TYPE_SEM_VAL> DEF::ExtractStaticMembers(const MAP<TYPE_AS_Name,TYPE_AS_FnDef> & fnm,
                                                          const MAP<TYPE_AS_Name,TYPE_AS_OpDef> & opm,
                                                          const TYPE_GLOBAL_SigmaClass & sigma)
{
  SET<TYPE_AS_Name> dom_fnm (fnm.Dom());
  MAP<TYPE_AS_Name, TYPE_SEM_VAL> statmap;
  Generic fnnm;
  for (bool bb = dom_fnm.First(fnnm); bb; bb = dom_fnm.Next(fnnm)) {
    const TYPE_AS_FnDef & fn (fnm[fnnm]);
    if (ASTAUX::GetFnStatic(fn)) {
      statmap.Insert(fnnm, LookUpLocalFn(fnnm, sigma));
      if (!ASTAUX::GetFnPrecond(fn).IsNil()) {
        TYPE_AS_Name prenm (AUX::PreName(fnnm));
        statmap.Insert(prenm, LookUpLocalFn(prenm, sigma));
      }
      if (!ASTAUX::GetFnPostcond(fn).IsNil()) {
        TYPE_AS_Name postnm (AUX::PostName(fnnm));
        statmap.Insert(postnm, LookUpLocalFn(postnm, sigma));
      }
    }
  }

  SET<TYPE_AS_Name> dom_opm (opm.Dom());
  const Map & explops (sigma.GetMap(pos_GLOBAL_SigmaClass_explops));
  Generic opnm;
  for (bool cc = dom_opm.First(opnm); cc; cc = dom_opm.Next(opnm)) {
    const TYPE_AS_OpDef & op (opm[opnm]);
    if (ASTAUX::GetOpStatic(op)) {
      if (explops.DomExists(opnm)) {
        statmap.Insert(opnm, explops[opnm]);
      }
    }
  }
  return statmap;
}

// LookUpLocalFn
// fnnm : AS`Name
// sigma : GLOBAL`SigmaClass
// ==> (SEM`CompExplFN | SEM`ExplPOLY)
TYPE_SEM_VAL DEF::LookUpLocalFn(const TYPE_AS_Name & fnnm, const TYPE_GLOBAL_SigmaClass & sigma)
{
  Generic g;
  if (sigma.GetMap(pos_GLOBAL_SigmaClass_explfns).DomExists(fnnm, g)) {
    return g;
  }
  else {
    return sigma.GetMap(pos_GLOBAL_SigmaClass_explpolys)[fnnm];
  }
}

// TransLocalRecSel
// tpdefs : map AS`Name to AS`TypeDef
// -> map AS`Name to (GLOBAL`RecSel * AS`Access)
Map DEF::TransLocalRecSel(const MAP<TYPE_AS_Name,TYPE_AS_TypeDef> & tpdefs)
{
  return TransTpDefMap(tpdefs);
  //Not TransLocalRecSel is not implemented as in the specification.
}

#endif // VDMPP

// TransFnMap
// mod_id : AS`Name
// FnMap : map AS`Name to AS`FnDef
// ==> FnTuple = (map AS`Name to SEM`CompExplFN) * (map AS`Name to SEM`ExplPOLY)
Tuple DEF::TransFnMap (const TYPE_AS_Name & mod_id, const MAP<TYPE_AS_Name,TYPE_AS_FnDef> & fnm)
{
  SET<TYPE_AS_Name> dom_fnm (fnm.Dom());
  Map efn, epf;
  Generic id;
  for (bool bb = dom_fnm.First (id); bb; bb = dom_fnm.Next (id)) {
    const TYPE_AS_FnDef & fd (fnm[id]);
    TYPE_SEM_VAL FNval (TransFN (mod_id, fd));

    switch (FNval.GetTag ()) {
      case TAG_TYPE_SEM_CompExplFN: {
        efn.Insert (id, FNval);
        efn.ImpOverride (CreatePrePostFns (mod_id, fd, id));
        efn.ImpOverride (CreateMeasureFns (mod_id, fd, id));
        break;
      }
      case TAG_TYPE_SEM_ExplPOLY: {
        epf.Insert (id, FNval);
        epf.ImpOverride (CreatePolyPrePostFns (mod_id, fd, id));
        epf.ImpOverride (CreatePolyMeasureFns (mod_id, fd, id));
        break;
      }
    }
  }
  return mk_(efn, epf);
}

// TransFN
// mod_id : AS`Name
// fn_def : AS`FnDef
// ==> SEM`FN | SEM`POLY
TYPE_SEM_VAL DEF::TransFN (const TYPE_AS_Name & mod_id, const TYPE_AS_FnDef & fn_def)
{
  switch(fn_def.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:    { return TransExplFN(mod_id, fn_def);    break; }
    case TAG_TYPE_AS_ExtExplFnDef: { return TransExtExplFN(mod_id, fn_def); break; }
    case TAG_TYPE_AS_ImplFnDef:    { return TransImplFN(mod_id, fn_def);    break; }
    default: { return TYPE_SEM_VAL(); } // dummy
  }
}

// TransExplFN
// mod_id : AS`Name
// fn_def : AS`ExplFnDef
// ==> SEM`FN | SEM`POLY
TYPE_SEM_VAL DEF::TransExplFN(const TYPE_AS_Name & mod_id, const TYPE_AS_ExplFnDef& fn_def)
{
  const TYPE_AS_Name & nm                 (fn_def.GetRecord(pos_AS_ExplFnDef_nm));
  const SEQ<TYPE_AS_TypeVar> & tpp        (fn_def.GetSequence(pos_AS_ExplFnDef_tpparms));
  const TYPE_AS_FnType & tp               (fn_def.GetRecord(pos_AS_ExplFnDef_tp));
  const SEQ<TYPE_AS_Parameters> & parms_l (fn_def.GetSequence(pos_AS_ExplFnDef_parms));
  const TYPE_AS_Access & access           (fn_def.GetField(pos_AS_ExplFnDef_access));

  TYPE_STKM_SubProgramId instr (theCompiler().CompileFnOpDef(fn_def));

  SEQ<type_dL> pi_l; // seq of seq of STKM`Pattern
  size_t len = parms_l.Length();
  for (size_t i = 1; i <= len; i++) {
    pi_l.ImpAppend(theCompiler().PL2PL(parms_l[i]));
  }
  if (tpp.IsEmpty()) { // non-polymorphic function
    TYPE_SEM_ExplFN FNval;
    FNval.Init(tp, // type
               pi_l, // parameters
               instr, // body
               AUX::MkEmptyBlkEnv(sem_read_only), // closure environment
               MAP<TYPE_AS_TypeVar,TYPE_AS_Type>(), // type instantiation map
               nm,
               mod_id,
               Bool(false),
               Nil (),
               RealAccess(access, ACC_FN));
    return SemRec::CompFN(FNval);
  }
  else { // polymorphic function
    TYPE_AS_Name clmodName (theCompiler().GetClMod());
    TYPE_SEM_ExplPOLY FNval;
    FNval.Init(tpp,       // polymorphic type parameters
               tp, // type
               pi_l,   // parameters
               instr, // body
               AUX::MkEmptyBlkEnv(sem_read_only), // closure environment
               nm,
               mod_id,
               Nil (),
               RealAccess(access, ACC_FN));
    return (FNval);
  }
}

// TransImplFN
// mod_id : AS`Name
// fn_def : AS`ExplFnDef
// ==> SEM`FN | SEM`POLY
TYPE_SEM_VAL DEF::TransImplFN(const TYPE_AS_Name & mod_id, const TYPE_AS_ImplFnDef& fn_def)
{
  const TYPE_AS_Name & nm                (fn_def.GetRecord(pos_AS_ImplFnDef_nm));
  const SEQ<TYPE_AS_TypeVar> & tpp       (fn_def.GetSequence(pos_AS_ImplFnDef_params));
  const SEQ<TYPE_AS_PatTypePair> & parml (fn_def.GetSequence(pos_AS_ImplFnDef_partps));
  const SEQ<TYPE_AS_NameType> & resnmtps (fn_def.GetSequence(pos_AS_ImplFnDef_resnmtps));
  const TYPE_AS_Access & access          (fn_def.GetField(pos_AS_ImplFnDef_access));

  TYPE_STKM_SubProgramId instr (theCompiler().CompileFnOpDef(fn_def));

  Tuple tmp_ITP (ImplicitTypeParams(parml));
  const SEQ<TYPE_AS_Type> & fndom    (tmp_ITP.GetSequence(1));
  const SEQ<TYPE_AS_Pattern> & parms (tmp_ITP.GetSequence(2));

  TYPE_AS_Type fnrng (ImplicitResType(resnmtps));

  TYPE_AS_TotalFnType tp;
  tp.Init(fndom, fnrng, NilContextId);

  SEQ<type_dL> parms_s;  // seq of seq of STKM`Pattern
  parms_s.ImpAppend(theCompiler().PL2PL(parms));

  if (tpp.IsEmpty()) { // non-polymorphic function
    TYPE_SEM_ExplFN FNval;
    FNval.Init(tp,              // type
               parms_s,         // parameters
               instr,           // body
               AUX::MkEmptyBlkEnv(sem_read_only), // closure environment
               MAP<TYPE_AS_TypeVar,TYPE_AS_Type>(), // type instantiation map
               nm,
               mod_id,
               Bool(true),
               Nil(),
               RealAccess(access, ACC_FN));
    return SemRec::CompFN(FNval);
  }
  else { // polymorphic function
    TYPE_AS_Name clmodName (theCompiler().GetClMod());

    TYPE_SEM_ExplPOLY FNval;
    FNval.Init(tpp,             // polymorphic type parameters
               tp,              // type
               parms_s,         // parameters
               instr,           // body
               AUX::MkEmptyBlkEnv(sem_read_only), // closure environment
               nm,
               mod_id,
               Nil (),
               RealAccess(access, ACC_FN));
    return (FNval);
  }
}

// TransExtExplFN
// mod_id : AS`Name
// fn_def : AS`ExplFnDef
// ==> SEM`FN | SEM`POLY
TYPE_SEM_VAL DEF::TransExtExplFN(const TYPE_AS_Name & mod_id, const TYPE_AS_ExtExplFnDef& fn_def)
{
  const TYPE_AS_Name & nm                (fn_def.GetRecord(pos_AS_ExtExplFnDef_nm));
  const SEQ<TYPE_AS_TypeVar> & tpp       (fn_def.GetSequence(pos_AS_ExtExplFnDef_params));
  const SEQ<TYPE_AS_PatTypePair> & parml (fn_def.GetSequence(pos_AS_ExtExplFnDef_partps));
  const SEQ<TYPE_AS_NameType> & resnmtps (fn_def.GetSequence(pos_AS_ExtExplFnDef_resnmtps));
  const TYPE_AS_Access & access          (fn_def.GetField(pos_AS_ExtExplFnDef_access));

  TYPE_STKM_SubProgramId instr (theCompiler().CompileFnOpDef(fn_def));

  Tuple tmp_ITP (ImplicitTypeParams(parml));
  const SEQ<TYPE_AS_Type> & fndom    (tmp_ITP.GetSequence(1));
  const SEQ<TYPE_AS_Pattern> & parms (tmp_ITP.GetSequence(2));

  TYPE_AS_Type fnrng (ImplicitResType(resnmtps));

  TYPE_AS_TotalFnType tp;
  tp.Init(fndom, fnrng, NilContextId);

  SEQ<type_dL> parms_s;  // seq of seq of STKM`Pattern
  parms_s.ImpAppend(theCompiler().PL2PL(parms));

  if (tpp.IsEmpty()) { // non-polymorphic function
    TYPE_SEM_ExplFN FNval;
    FNval.Init(tp,              // type
               parms_s,         // parameters
               instr,           // body
               AUX::MkEmptyBlkEnv(sem_read_only), // closure environment
               MAP<TYPE_AS_TypeVar,TYPE_AS_Type>(), // type instantiation map
               nm,
               mod_id,
               Bool(false),
               Nil(),
               RealAccess(access, ACC_FN));
    return SemRec::CompFN(FNval);
  }
  else { // polymorphic function
    TYPE_AS_Name clmodName (theCompiler().GetClMod());

    TYPE_SEM_ExplPOLY FNval;
    FNval.Init(tpp,             // polymorphic type parameters
               tp,              // type
               parms_s,         // parameters
               instr,           // body
               AUX::MkEmptyBlkEnv(sem_read_only), // closure environment
               nm,
               mod_id,
               Nil (),
               RealAccess(access, ACC_FN));
    return (FNval);
  }
}

// CreatePrePostFns
// mod_id : AS`Name
// fn_def : AS`FnDef
// overloadnm : AS`Name
// ==> map AS`Name to SEM`CompExplFN
Map DEF::CreatePrePostFns (const TYPE_AS_Name & mod_id,
                           const TYPE_AS_FnDef & fn_def,
                           const TYPE_AS_Name & overloadnm)
{
  switch(fn_def.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:    { return CreateExplPrePostFns(mod_id, fn_def, overloadnm); }
    case TAG_TYPE_AS_ExtExplFnDef: { return CreateExtExplPrePostFns(mod_id, fn_def, overloadnm); }
    case TAG_TYPE_AS_ImplFnDef:    { return CreateImplPrePostFns(mod_id, fn_def, overloadnm); }
    default:                       { return Map(); }
  }
}

// CreateExplPrePostFns
// mod_id : AS`Name
// fn_def : AS`ExplFnDef
// overloadnm : AS`Name
// ==> map AS`Name to SEM`CompExplFN
Map DEF::CreateExplPrePostFns (const TYPE_AS_Name & mod_id,
                               const TYPE_AS_ExplFnDef & fn_def,
                               const TYPE_AS_Name & overloadnm)
{
//  const TYPE_AS_Name & nm (fn_def.GetRecord(pos_AS_ExplFnDef_nm));
  const TYPE_AS_FnType & tp             (fn_def.GetRecord(pos_AS_ExplFnDef_tp));
  const SEQ<TYPE_AS_Parameters> & parms (fn_def.GetSequence(pos_AS_ExplFnDef_parms));
  const Generic & pre_e                 (fn_def.GetField(pos_AS_ExplFnDef_fnpre));
  const Generic & post_e                (fn_def.GetField(pos_AS_ExplFnDef_fnpost));
  const TYPE_AS_Access & access         (fn_def.GetField(pos_AS_ExplFnDef_access));

  Map res_m;

  if (! pre_e.IsNil ()) {
    SEQ<type_dL> parm_pre; // seq of seq of STKM`Pattern
    size_t len_parms = parms.Length();
    for (size_t idx = 1; idx <= len_parms; idx++) {
      parm_pre.ImpAppend (theCompiler().PL2PL(parms[idx]));
    }

    TYPE_AS_TotalFnType fn_tp (CreateFunctionPreType(tp.GetSequence (1), tp.GetRecord(2), parms));

    TYPE_AS_Name nm_pre (AUX::PreName(overloadnm));

    TYPE_SEM_ExplFN fn_pre;
    fn_pre.Init(fn_tp,
                parm_pre,
                theCompiler().CompilePrePostExpr(pre_e),
                AUX::MkEmptyBlkEnv(sem_read_only),
                MAP<TYPE_AS_TypeVar,TYPE_AS_Type>(),
                nm_pre,
                mod_id,
                Bool(false),
                Nil(),
                RealAccess(access, ACC_FN));

    res_m.Insert (nm_pre, SemRec::CompFN(fn_pre));
  }

  if (! post_e.IsNil ()) {
    SEQ<TYPE_AS_Type> tp_fnrng_s;
    tp_fnrng_s.ImpAppend(tp.GetField (2));
    TYPE_AS_Type tp_post (CreateFunctionPostType (tp.GetSequence (1), tp_fnrng_s, parms));

    TYPE_CI_ContextId cid (ASTAUX::GetCid(post_e));
    TYPE_STKM_PatternName pat_id;
    pat_id.Init(ASTAUX::MkNameFromId (ASTAUX::MkId(L"RESULT"), cid), Nil());

    SEQ<type_dL> parm_post; // seq of seq of STKM`Pattern
    size_t len_parms = parms.Length();
    for (size_t idx = 1; idx <= len_parms; idx++) {
      SEQ<TYPE_STKM_Pattern> tmp (theCompiler().PL2PL(parms[idx]));
      if (idx == len_parms) {
        tmp.ImpAppend (pat_id);
      }
      parm_post.ImpAppend (tmp);
    }

    TYPE_AS_Name nm_post (AUX::PostName(overloadnm));

    TYPE_SEM_ExplFN fn_post;
    fn_post.Init(tp_post,
                 parm_post,
                 theCompiler().CompilePrePostExpr(post_e),
                 AUX::MkEmptyBlkEnv(sem_read_only),
                 MAP<TYPE_AS_TypeVar,TYPE_AS_Type>(),
                 nm_post,
                 mod_id,
                 Bool(false),
                 Nil(),
                 RealAccess(access, ACC_FN));

    res_m.Insert (nm_post, SemRec::CompFN(fn_post));
  }
  return (res_m);
}

// CreateExtExplPrePostFns
// mod_id : AS`Name
// fn_def : AS`ExtExplFnDef
// overloadnm : AS`Name
// ==> map AS`Name to SEM`CompExplFN
Map DEF::CreateExtExplPrePostFns (const TYPE_AS_Name & mod_id,
                                  const TYPE_AS_ExtExplFnDef & fn_def,
                                  const TYPE_AS_Name & overloadnm)
{
//  const TYPE_AS_Name & nm                 (fn_def.GetRecord(pos_AS_ExtExplFnDef_nm));
  const SEQ<TYPE_AS_PatTypePair> & partps (fn_def.GetSequence(pos_AS_ExtExplFnDef_partps));
  const SEQ<TYPE_AS_NameType> & resnmtps  (fn_def.GetSequence(pos_AS_ExtExplFnDef_resnmtps));
  const Generic & pre_e                   (fn_def.GetField(pos_AS_ExtExplFnDef_fnpre));
  const Generic & post_e                  (fn_def.GetField(pos_AS_ExtExplFnDef_fnpost));
  const TYPE_AS_Access & access           (fn_def.GetField(pos_AS_ExtExplFnDef_access));

  Tuple tmp_parml (ImplicitTypeParams(partps));
  const SEQ<TYPE_AS_Type> & fndom    (tmp_parml.GetSequence(1));
  const SEQ<TYPE_AS_Pattern> & parms (tmp_parml.GetSequence(2));

  Tuple tmp_resnmtps (ImplicitResNameTypes(resnmtps));
  const SEQ<TYPE_AS_Name> & resnms   (tmp_resnmtps.GetSequence(1));
  const SEQ<TYPE_AS_Type> & restps   (tmp_resnmtps.GetSequence(2));

  Map res_m;

  if (! pre_e.IsNil ()) {
    SEQ<type_dL> parm_pre; // seq of seq of STKM`Pattern
    parm_pre.ImpAppend (theCompiler().PL2PL(parms));

    TYPE_AS_TotalFnType fn_tp;
    fn_tp.Init(fndom, TYPE_AS_BooleanType().Init(NilContextId), NilContextId);

    TYPE_AS_Name nm_pre (AUX::PreName(overloadnm));

    TYPE_SEM_ExplFN fn_pre;
    fn_pre.Init(fn_tp,
                parm_pre,
                theCompiler().CompilePrePostExpr(pre_e),
                AUX::MkEmptyBlkEnv (sem_read_only),
                MAP<TYPE_AS_TypeVar,TYPE_AS_Type>(),
                nm_pre,
                mod_id,
                Bool(false),
                Nil(),
                RealAccess(access, ACC_FN));

    res_m.Insert (nm_pre, SemRec::CompFN(fn_pre));
  }

  if (! post_e.IsNil ()) {
    SEQ<TYPE_AS_Pattern> p_res (CreatePostParms(resnms));

    TYPE_AS_Type tp_post (CreateFunctionPostType (fndom, restps, Sequence()));

    SEQ<type_dL> parm_post_s; // seq of seq of STKM`Pattern
    parm_post_s.ImpAppend((theCompiler().PL2PL(parms)).ImpConc(theCompiler().PL2PL(p_res)));

    TYPE_AS_Name nm_post (AUX::PostName(overloadnm));

    TYPE_SEM_ExplFN fn_post;
    fn_post.Init(tp_post,
                 parm_post_s,
                 theCompiler().CompilePrePostExpr(post_e),
                 AUX::MkEmptyBlkEnv (sem_read_only),
                 MAP<TYPE_AS_TypeVar,TYPE_AS_Type>(),
                 nm_post,
                 mod_id,
                 Bool(false),
                 Nil(),
                 RealAccess(access, ACC_FN));

    res_m.Insert (nm_post, SemRec::CompFN(fn_post));
  }

  return (res_m);
}
// CreateImplPrePostFns
// mod_id : AS`Name
// fn_def : AS`ImplFnDef
// overloadnm : AS`Name
// ==> map AS`Name to SEM`CompExplFN
Map DEF::CreateImplPrePostFns (const TYPE_AS_Name & mod_id,
                               const TYPE_AS_ImplFnDef & fn_def,
                               const TYPE_AS_Name & overloadnm)
{
//  const TYPE_AS_Name & nm                 (fn_def.GetRecord(pos_AS_ExtExplFnDef_nm));
  const SEQ<TYPE_AS_PatTypePair> & partps (fn_def.GetSequence(pos_AS_ImplFnDef_partps));
  const SEQ<TYPE_AS_NameType> & resnmtps  (fn_def.GetSequence(pos_AS_ImplFnDef_resnmtps));
  const Generic & pre_e                   (fn_def.GetField(pos_AS_ImplFnDef_fnpre));
  const Generic & post_e                  (fn_def.GetField(pos_AS_ImplFnDef_fnpost));
  const TYPE_AS_Access & access           (fn_def.GetField(pos_AS_ImplFnDef_access));

  Tuple tmp_parml (ImplicitTypeParams(partps));
  const SEQ<TYPE_AS_Type> & fndom    (tmp_parml.GetSequence(1));
  const SEQ<TYPE_AS_Pattern> & parms (tmp_parml.GetSequence(2));

  Tuple tmp_resnmtps (ImplicitResNameTypes(resnmtps));
  const SEQ<TYPE_AS_Name> & resnms   (tmp_resnmtps.GetSequence(1));
  const SEQ<TYPE_AS_Type> & restps   (tmp_resnmtps.GetSequence(2));

  Map res_m;

  if (! pre_e.IsNil ()) {
    SEQ<type_dL> parm_pre; // seq of seq of STKM`Pattern
    parm_pre.ImpAppend (theCompiler().PL2PL(parms));

    TYPE_AS_TotalFnType fn_tp;
    fn_tp.Init(fndom, TYPE_AS_BooleanType().Init(NilContextId), NilContextId);

    TYPE_AS_Name nm_pre (AUX::PreName(overloadnm));

    TYPE_SEM_ExplFN fn_pre;
    fn_pre.Init(fn_tp,
                parm_pre,
                theCompiler().CompilePrePostExpr(pre_e),
                AUX::MkEmptyBlkEnv (sem_read_only),
                MAP<TYPE_AS_TypeVar,TYPE_AS_Type>(),
                nm_pre,
                mod_id,
                Bool(false),
                Nil(),
                RealAccess(access, ACC_FN));

    res_m.Insert (nm_pre, SemRec::CompFN(fn_pre));
  }

  SEQ<TYPE_AS_Pattern> p_res (CreatePostParms(resnms));

  TYPE_AS_Type tp_post (CreateFunctionPostType (fndom, restps, Sequence()));

  SEQ<type_dL> parm_post_s; // seq of seq of STKM`Pattern
  parm_post_s.ImpAppend((theCompiler().PL2PL(parms)).ImpConc(theCompiler().PL2PL(p_res)));

  TYPE_AS_Name nm_post (AUX::PostName(overloadnm));

  TYPE_SEM_ExplFN fn_post;
  fn_post.Init(tp_post,
               parm_post_s,
               theCompiler().CompilePrePostExpr(post_e),
               AUX::MkEmptyBlkEnv (sem_read_only),
               MAP<TYPE_AS_TypeVar,TYPE_AS_Type>(),
               nm_post,
               mod_id,
               Bool(false),
               Nil(),
               RealAccess(access, ACC_FN));

  res_m.Insert (nm_post, SemRec::CompFN(fn_post));

  return (res_m);
}

// CreatePostParms
// resnms : seq of AS`Name
// +> seq of AS`Pattern
SEQ<TYPE_AS_Pattern> DEF::CreatePostParms(const SEQ<TYPE_AS_Name> & resnms)
{
  switch(resnms.Length()) {
    case 0: {
      return SEQ<TYPE_AS_Pattern>();
    }
    case 1: {
      const TYPE_AS_Name & nm (resnms.Hd());
      SEQ<TYPE_AS_Pattern> res;
      res.ImpAppend(TYPE_AS_PatternName().Init(nm, Nil(), nm.GetInt(pos_AS_Name_cid)));
      return res;
    }
    default: {
      SEQ<TYPE_AS_Pattern> l;
      size_t len_resnms = resnms.Length();
      for (size_t idx = 1; idx <= len_resnms; idx++) {
        const TYPE_AS_Name & nm (resnms[idx]);
        l.ImpAppend(TYPE_AS_PatternName().Init(nm, Nil(), nm.GetInt(pos_AS_Name_cid)));
      }
      SEQ<TYPE_AS_Pattern> res;
      res.ImpAppend(TYPE_AS_TuplePattern().Init(l, ASTAUX::GetCid(resnms.Hd())));
      return res;
    }
  }
}

// CreatePolyPrePostFns
// mod_id : AS`Name
// fn_def : AS`FnDef
// overloadnm : AS`Name
// ==> map AS`Name to SEM`ExplPOLY
Map DEF::CreatePolyPrePostFns (const TYPE_AS_Name & mod_id,
                               const TYPE_AS_FnDef & fn_def,
                               const TYPE_AS_Name & overloadnm)
{
  switch(fn_def.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:    { return CreateExplPolyPrePostFns(mod_id, fn_def, overloadnm); }
    case TAG_TYPE_AS_ExtExplFnDef: { return CreateExtExplPolyPrePostFns(mod_id, fn_def, overloadnm); }
    case TAG_TYPE_AS_ImplFnDef:    { return CreateImplPolyPrePostFns(mod_id, fn_def, overloadnm); }
    default:                       { return Map(); }
  }
}

// CreateExplPolyPrePostFns
// mod_id : AS`Name
// fn_def : AS`ExplFnDef
// overloadnm : AS`Name
// ==> map AS`Name to SEM`ExplPOLY
Map DEF::CreateExplPolyPrePostFns (const TYPE_AS_Name & mod_id,
                                   const TYPE_AS_ExplFnDef & fn_def,
                                   const TYPE_AS_Name & overloadnm)
{
//  const TYPE_AS_Name & nm          (fn_def.GetRecord(pos_AS_ExplFnDef_nm));
  const SEQ<TYPE_AS_TypeVar> & tpp      (fn_def.GetSequence(pos_AS_ExplFnDef_tpparms));
  const TYPE_AS_FnType & tp             (fn_def.GetRecord(pos_AS_ExplFnDef_tp));
  const SEQ<TYPE_AS_Parameters> & parms (fn_def.GetSequence(pos_AS_ExplFnDef_parms));
  const Generic & pre_e                 (fn_def.GetField(pos_AS_ExplFnDef_fnpre));
  const Generic & post_e                (fn_def.GetField(pos_AS_ExplFnDef_fnpost));
  const TYPE_AS_Access & access         (fn_def.GetField(pos_AS_ExplFnDef_access));

  Map res_m;

  if (!pre_e.IsNil ()) {
    SEQ<type_dL> pi_l; // seq of seq of STKM`Pattern
    size_t len_parms = parms.Length();
    for (size_t idx = 1; idx <= len_parms; idx++) {
      pi_l.ImpAppend(theCompiler().PL2PL(parms[idx]));
    }

    TYPE_AS_TotalFnType fn_tp (CreateFunctionPreType(tp.GetSequence (1), tp.GetRecord(2), parms));

    TYPE_AS_Name nm_pre (AUX::PreName(overloadnm));

    TYPE_SEM_ExplPOLY fn_pre;
    fn_pre.Init(tpp,
                fn_tp,
                pi_l,
                theCompiler().CompilePrePostExpr(pre_e),
                AUX::MkEmptyBlkEnv (sem_read_only),
                nm_pre,
                mod_id,
                Nil(),
                RealAccess(access, ACC_FN));

    res_m.Insert (nm_pre, fn_pre);
  }

  if (! post_e.IsNil ()) {
    SEQ<TYPE_AS_Type> tp_fnrng_s;
    tp_fnrng_s.ImpAppend(tp.GetRecord (2));

    TYPE_AS_Type tp_post (CreateFunctionPostType (tp.GetSequence (1), tp_fnrng_s, parms));

    TYPE_CI_ContextId cid (ASTAUX::GetCid(post_e));
    TYPE_STKM_PatternName pat_id;
    pat_id.Init(ASTAUX::MkNameFromId (ASTAUX::MkId(L"RESULT"), cid), Nil());

    SEQ<type_dL> parm_post; // seq of seq of STKM`Pattern
    size_t len_parms = parms.Length();
    for (size_t idx = 1; idx <= len_parms; idx++) {
      SEQ<TYPE_STKM_Pattern> tmp (theCompiler().PL2PL(parms[idx]));
      if (idx == len_parms) {
        tmp.ImpAppend (pat_id);
      }
      parm_post.ImpAppend (tmp);
    }

    TYPE_AS_Name nm_post (AUX::PostName(overloadnm));

    TYPE_SEM_ExplPOLY fn_post;
    fn_post.Init(tpp,
                 tp_post,
                 parm_post,
                 theCompiler().CompilePrePostExpr(post_e),
                 AUX::MkEmptyBlkEnv (sem_read_only),
                 nm_post,
                 mod_id,
                 Nil(),
                 RealAccess(access, ACC_FN));

    res_m.Insert (nm_post, fn_post);
  }

  return (res_m);
}

// CreateExtExplPolyPrePostFns
// mod_id : AS`Name
// fn_def : AS`ExtExplFnDef
// overloadnm : AS`Name
// ==> map AS`Name to SEM`ExplPOLY
Map DEF::CreateExtExplPolyPrePostFns (const TYPE_AS_Name & mod_id,
                                      const TYPE_AS_ExtExplFnDef & fn_def,
                                      const TYPE_AS_Name & overloadnm)
{
//  const TYPE_AS_Name & nm                 (fn_def.GetRecord(pos_AS_ExtExplFnDef_nm));
  const SEQ<TYPE_AS_TypeVar> & tpp        (fn_def.GetSequence(pos_AS_ExtExplFnDef_params));
  const SEQ<TYPE_AS_PatTypePair> & partps (fn_def.GetSequence(pos_AS_ExtExplFnDef_partps));
  const SEQ<TYPE_AS_NameType> & resnmtps  (fn_def.GetSequence(pos_AS_ExtExplFnDef_resnmtps));
  const Generic & pre_e                   (fn_def.GetField(pos_AS_ExtExplFnDef_fnpre));
  const Generic & post_e                  (fn_def.GetField(pos_AS_ExtExplFnDef_fnpost));
  const TYPE_AS_Access & access           (fn_def.GetField(pos_AS_ExtExplFnDef_access));

  Tuple tmp_parml (ImplicitTypeParams(partps));
  const SEQ<TYPE_AS_Type> & fndom    (tmp_parml.GetSequence(1));
  const SEQ<TYPE_AS_Pattern> & parms (tmp_parml.GetSequence(2));

  Tuple tmp_resnmtps (ImplicitResNameTypes(resnmtps));
  const SEQ<TYPE_AS_Name> & resnms   (tmp_resnmtps.GetSequence(1));
  const SEQ<TYPE_AS_Type> & restps   (tmp_resnmtps.GetSequence(2));

  Map res_m;

  if (! pre_e.IsNil ()) {
    SEQ<type_dL> parm_pre; // seq of seq of STKM`Pattern
    parm_pre.ImpAppend (theCompiler().PL2PL(parms));

    TYPE_AS_TotalFnType fn_tp;
    fn_tp.Init(fndom, TYPE_AS_BooleanType().Init(NilContextId), NilContextId);

    TYPE_AS_Name nm_pre (AUX::PreName(overloadnm));

    TYPE_SEM_ExplPOLY fn_pre;
    fn_pre.Init(tpp,
                fn_tp,
                parm_pre,
                theCompiler().CompilePrePostExpr(pre_e),
                AUX::MkEmptyBlkEnv (sem_read_only),
                nm_pre,
                mod_id,
                Nil(),
                RealAccess(access, ACC_FN));

    res_m.Insert (nm_pre, fn_pre);
  }

  if (! post_e.IsNil ()) {
    SEQ<TYPE_AS_Pattern> res_p (CreatePostParms(resnms));

    TYPE_AS_Type tp_post (CreateFunctionPostType (fndom, restps, Sequence()));

    SEQ<type_dL> parm_post_s; // seq of seq of STKM`Pattern
    parm_post_s.ImpAppend((theCompiler().PL2PL(parms)).ImpConc(theCompiler().PL2PL(res_p)));

    TYPE_AS_Name nm_post (AUX::PostName(overloadnm));

    TYPE_SEM_ExplPOLY fn_post;
    fn_post.Init(tpp,
                 tp_post,
                 parm_post_s,
                 theCompiler().CompilePrePostExpr(post_e),
                 AUX::MkEmptyBlkEnv (sem_read_only),
                 nm_post,
                 mod_id,
                 Nil(),
                 RealAccess(access, ACC_FN));

    res_m.Insert (nm_post, fn_post);
  }
  return (res_m);
}

// CreateImplPolyPrePostFns
// mod_id : AS`Name
// fn_def : AS`ImplFnDef
// overloadnm : AS`Name
// ==> map AS`Name to SEM`ExplPOLY
Map DEF::CreateImplPolyPrePostFns (const TYPE_AS_Name & mod_id,
                                   const TYPE_AS_ImplFnDef & fn_def,
                                   const TYPE_AS_Name & overloadnm)
{
//  const TYPE_AS_Name & nm                 (fn_def.GetRecord(pos_AS_ImplFnDef_nm));
  const SEQ<TYPE_AS_TypeVar> & tpp        (fn_def.GetSequence(pos_AS_ImplFnDef_params));
  const SEQ<TYPE_AS_PatTypePair> & partps (fn_def.GetSequence(pos_AS_ImplFnDef_partps));
  const SEQ<TYPE_AS_NameType> & resnmtps  (fn_def.GetSequence(pos_AS_ImplFnDef_resnmtps));
  const Generic & pre_e                   (fn_def.GetField(pos_AS_ImplFnDef_fnpre));
  const Generic & post_e                  (fn_def.GetField(pos_AS_ImplFnDef_fnpost));
  const TYPE_AS_Access & access           (fn_def.GetField(pos_AS_ImplFnDef_access));

  Tuple tmp_parml (ImplicitTypeParams(partps));
  const SEQ<TYPE_AS_Type> & fndom    (tmp_parml.GetSequence(1));
  const SEQ<TYPE_AS_Pattern> & parms (tmp_parml.GetSequence(2));

  Tuple tmp_resnmtps (ImplicitResNameTypes(resnmtps));
  const SEQ<TYPE_AS_Name> & resnms   (tmp_resnmtps.GetSequence(1));
  const SEQ<TYPE_AS_Type> & restps   (tmp_resnmtps.GetSequence(2));

  Map res_m;

  if (! pre_e.IsNil ()) {
    SEQ<type_dL> parm_pre; // seq of seq of STKM`Pattern
    parm_pre.ImpAppend (theCompiler().PL2PL(parms));

    TYPE_AS_TotalFnType fn_tp;
    fn_tp.Init(fndom, TYPE_AS_BooleanType().Init(NilContextId), NilContextId);

    TYPE_AS_Name nm_pre (AUX::PreName(overloadnm));

    TYPE_SEM_ExplPOLY fn_pre;
    fn_pre.Init(tpp,
                fn_tp,
                parm_pre,
                theCompiler().CompilePrePostExpr(pre_e),
                AUX::MkEmptyBlkEnv (sem_read_only),
                nm_pre,
                mod_id,
                Nil(),
                RealAccess(access, ACC_FN));

    res_m.Insert (nm_pre, fn_pre);
  }

  SEQ<TYPE_AS_Pattern> res_p (CreatePostParms(resnms));

  TYPE_AS_Type tp_post (CreateFunctionPostType (fndom, restps, Sequence()));

  SEQ<type_dL> parm_post_s; // seq of seq of STKM`Pattern
  parm_post_s.ImpAppend((theCompiler().PL2PL(parms)).ImpConc(theCompiler().PL2PL(res_p)));

  TYPE_AS_Name nm_post (AUX::PostName(overloadnm));

  TYPE_SEM_ExplPOLY fn_post;
  fn_post.Init(tpp,
               tp_post,
               parm_post_s,
               theCompiler().CompilePrePostExpr(post_e),
               AUX::MkEmptyBlkEnv (sem_read_only),
               nm_post,
               mod_id,
               Nil(),
               RealAccess(access, ACC_FN));

  res_m.Insert (nm_post, fn_post);

  return (res_m);
}

// CreateMeasureFns
// mod_id : AS`Name
// fn_def : AS`FnDef
// overloadnm : AS`Name
// ==> map AS`Name to SEM`CompExplFN
Map DEF::CreateMeasureFns (const TYPE_AS_Name & mod_id,
                           const TYPE_AS_FnDef & fn_def,
                           const TYPE_AS_Name & overloadnm)
{
  switch(fn_def.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:    { return CreateExplMeasureFns(mod_id, fn_def, overloadnm); }
    case TAG_TYPE_AS_ExtExplFnDef: { return CreateExtExplMeasureFns(mod_id, fn_def, overloadnm); }
    case TAG_TYPE_AS_ImplFnDef:    { return Map(); }
    default:                       { return Map(); }
  }
}

// CreateExplMeasureFns
// mod_id : AS`Name
// fn_def : AS`FnDef
// overloadnm : AS`Name
// ==> map AS`Name to SEM`CompExplFN
Map DEF::CreateExplMeasureFns (const TYPE_AS_Name & mod_id,
                               const TYPE_AS_FnDef & fn_def,
                               const TYPE_AS_Name & overloadnm)
{
  const TYPE_AS_FnType & tp             (fn_def.GetRecord(pos_AS_ExplFnDef_tp));
  const SEQ<TYPE_AS_Parameters> & parms (fn_def.GetSequence(pos_AS_ExplFnDef_parms));
  const Generic & measu                 (fn_def.GetField(pos_AS_ExplFnDef_measu));
  const TYPE_AS_Access & access         (fn_def.GetField(pos_AS_ExplFnDef_access));

  Map res_m;

  if (!measu.IsNil () && (measu != Int(NOTYETSPEC))) {
    if (!theCompiler().MeasureIsId(measu, fn_def)) {
      SEQ<type_dL> parm_measu; // seq of seq of STKM`Pattern
      size_t len_parms = parms.Length();
      for (size_t idx = 1; idx <= len_parms; idx++) {
        parm_measu.ImpAppend (theCompiler().PL2PL(parms[idx]));
      }

      TYPE_AS_TotalFnType fn_tp (CreateFunctionMeasureType(tp.GetSequence (1), tp.GetRecord(2), parms));

      TYPE_AS_Name nm_measu (AUX::MeasureName(overloadnm));

      TYPE_SEM_ExplFN fn_measu;
      fn_measu.Init(fn_tp,
                    parm_measu,
                    theCompiler().CompileMeasureExpr(measu),
                    AUX::MkEmptyBlkEnv(sem_read_only),
                    MAP<TYPE_AS_TypeVar,TYPE_AS_Type>(),
                    nm_measu,
                    mod_id,
                    Bool(false),
                    Nil(),
                    RealAccess(access, ACC_FN));

      res_m.Insert (nm_measu, SemRec::CompFN(fn_measu));
    }
  }
  return (res_m);
}

// CreateExtExplMeasureFns
// mod_id : AS`Name
// fn_def : AS`FnDef
// overloadnm : AS`Name
// ==> map AS`Name to SEM`CompExplFN
Map DEF::CreateExtExplMeasureFns (const TYPE_AS_Name & mod_id,
                                  const TYPE_AS_FnDef & fn_def,
                                  const TYPE_AS_Name & overloadnm)
{
  const SEQ<TYPE_AS_PatTypePair> & partps (fn_def.GetSequence(pos_AS_ExtExplFnDef_partps));
  const Generic & measu                   (fn_def.GetField(pos_AS_ExtExplFnDef_measu));
  const TYPE_AS_Access & access           (fn_def.GetField(pos_AS_ExtExplFnDef_access));

  Tuple tmp_parml (ImplicitTypeParams(partps));
  const SEQ<TYPE_AS_Type> & fndom    (tmp_parml.GetSequence(1));
  const SEQ<TYPE_AS_Pattern> & parms (tmp_parml.GetSequence(2));

  Map res_m;

  if (!measu.IsNil () && (measu != Int(NOTYETSPEC))) {
    if (!theCompiler().MeasureIsId(measu, fn_def)) {
      SEQ<type_dL> parm_measu; // seq of seq of STKM`Pattern
      parm_measu.ImpAppend (theCompiler().PL2PL(parms));

      TYPE_AS_TotalFnType fn_tp;
      fn_tp.Init(fndom, TYPE_AS_AllType().Init(NilContextId), NilContextId);
  
      TYPE_AS_Name nm_measu (AUX::MeasureName(overloadnm));
  
      TYPE_SEM_ExplFN fn_measu;
      fn_measu.Init(fn_tp,
                    parm_measu,
                    theCompiler().CompileMeasureExpr(measu),
                    AUX::MkEmptyBlkEnv (sem_read_only),
                    MAP<TYPE_AS_TypeVar,TYPE_AS_Type>(),
                    nm_measu,
                    mod_id,
                    Bool(false),
                    Nil(),
                    RealAccess(access, ACC_FN));
  
      res_m.Insert (nm_measu, SemRec::CompFN(fn_measu));
    }
  }
  return (res_m);
}

// CreatePolyMeasureFns
// mod_id : AS`Name
// fn_def : AS`FnDef
// overloadnm : AS`Name
// ==> map AS`Name to SEM`ExplPOLY
Map DEF::CreatePolyMeasureFns (const TYPE_AS_Name & mod_id,
                               const TYPE_AS_FnDef & fn_def,
                               const TYPE_AS_Name & overloadnm)
{
  switch(fn_def.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:    { return CreateExplPolyMeasureFns(mod_id, fn_def, overloadnm); }
    case TAG_TYPE_AS_ExtExplFnDef: { return CreateExtExplPolyMeasureFns(mod_id, fn_def, overloadnm); }
    case TAG_TYPE_AS_ImplFnDef:    { return Map(); }
    default:                       { return Map(); }
  }
}

// CreateExplPolyMeasureFns
// mod_id : AS`Name
// fn_def : AS`FnDef
// overloadnm : AS`Name
// ==> map AS`Name to SEM`ExplPOLY
Map DEF::CreateExplPolyMeasureFns (const TYPE_AS_Name & mod_id,
                                   const TYPE_AS_FnDef & fn_def,
                                   const TYPE_AS_Name & overloadnm)
{
  const SEQ<TYPE_AS_TypeVar> & tpp      (fn_def.GetSequence(pos_AS_ExplFnDef_tpparms));
  const TYPE_AS_FnType & tp             (fn_def.GetRecord(pos_AS_ExplFnDef_tp));
  const SEQ<TYPE_AS_Parameters> & parms (fn_def.GetSequence(pos_AS_ExplFnDef_parms));
  const Generic & measu                 (fn_def.GetField(pos_AS_ExplFnDef_measu));
  const TYPE_AS_Access & access         (fn_def.GetField(pos_AS_ExplFnDef_access));

  Map res_m;

  if (!measu.IsNil () && (measu != Int(NOTYETSPEC))) {
    if (!theCompiler().MeasureIsId(measu, fn_def)) {
      SEQ<type_dL> pi_l; // seq of seq of STKM`Pattern
      size_t len_parms = parms.Length();
      for (size_t idx = 1; idx <= len_parms; idx++) {
        pi_l.ImpAppend(theCompiler().PL2PL(parms[idx]));
      }

      TYPE_AS_TotalFnType fn_tp (CreateFunctionMeasureType(tp.GetSequence (1), tp.GetRecord(2), parms));

      TYPE_AS_Name nm_measu (AUX::MeasureName(overloadnm));

      TYPE_SEM_ExplPOLY fn_measu;
      fn_measu.Init(tpp,
                    fn_tp,
                    pi_l,
                    theCompiler().CompileMeasureExpr(measu),
                    AUX::MkEmptyBlkEnv (sem_read_only),
                    nm_measu,
                    mod_id,
                    Nil(),
                    RealAccess(access, ACC_FN));
  
      res_m.Insert (nm_measu, fn_measu);
    }
  }
  return (res_m);
}

// CreateExtExplPolyMeasureFns
// mod_id : AS`Name
// fn_def : AS`FnDef
// overloadnm : AS`Name
// ==> map AS`Name to SEM`ExplPOLY
Map DEF::CreateExtExplPolyMeasureFns (const TYPE_AS_Name & mod_id,
                                      const TYPE_AS_FnDef & fn_def,
                                      const TYPE_AS_Name & overloadnm)
{
  const SEQ<TYPE_AS_TypeVar> & tpp        (fn_def.GetSequence(pos_AS_ExtExplFnDef_params));
  const SEQ<TYPE_AS_PatTypePair> & partps (fn_def.GetSequence(pos_AS_ExtExplFnDef_partps));
  const Generic & measu                   (fn_def.GetField(pos_AS_ExtExplFnDef_measu));
  const TYPE_AS_Access & access           (fn_def.GetField(pos_AS_ExtExplFnDef_access));

  Tuple tmp_parml (ImplicitTypeParams(partps));
  const SEQ<TYPE_AS_Type> & fndom    (tmp_parml.GetSequence(1));
  const SEQ<TYPE_AS_Pattern> & parms (tmp_parml.GetSequence(2));

  Map res_m;

  if (!measu.IsNil () && (measu != Int(NOTYETSPEC))) {
    if (!theCompiler().MeasureIsId(measu, fn_def)) {
      SEQ<type_dL> parm_measu; // seq of seq of STKM`Pattern
      parm_measu.ImpAppend (theCompiler().PL2PL(parms));
  
      TYPE_AS_TotalFnType fn_tp;
      fn_tp.Init(fndom, TYPE_AS_AllType().Init(NilContextId), NilContextId);

      TYPE_AS_Name nm_measu (AUX::MeasureName(overloadnm));

      TYPE_SEM_ExplPOLY fn_measu;
      fn_measu.Init(tpp,
                    fn_tp,
                    parm_measu,
                    theCompiler().CompileMeasureExpr(measu),
                    AUX::MkEmptyBlkEnv (sem_read_only),
                    nm_measu,
                    mod_id,
                    Nil(),
                    RealAccess(access, ACC_FN));
  
      res_m.Insert (nm_measu, fn_measu);
    }
  }
  return (res_m);
}

// ImplicitTypeParams
// partps : AS`ParameterTypes
// ==> AS`DiscretionaryType * AS`Parameters
Tuple DEF::ImplicitTypeParams (const SEQ<TYPE_AS_PatTypePair> & partps)
{
  SEQ<TYPE_AS_Type> tp_l;
  SEQ<TYPE_AS_Pattern> parm_l;
  size_t len_partps = partps.Length();
  for (size_t i = 1; i <= len_partps; i++) {
    const TYPE_AS_PatTypePair & ptp (partps[i]);
    const SEQ<TYPE_AS_Pattern> & pat_l (ptp.GetSequence(pos_AS_PatTypePair_pats));
    const TYPE_AS_Type & tp    (ptp.GetRecord(pos_AS_PatTypePair_tp));

    size_t len_pat_l = pat_l.Length();
    for (size_t j = 1; j <= len_pat_l; j++) {
      tp_l.ImpAppend(tp);
      parm_l.ImpAppend (pat_l[j]);
    }
  }
  return mk_(tp_l, parm_l);
}

// ImplicitResNameTypes
// resnmtps : seq of AS`NameType
// ==> seq of AS`Name * seq of AS`Type
Tuple DEF::ImplicitResNameTypes (const SEQ<TYPE_AS_NameType> & resnmtps)
{
  SEQ<TYPE_AS_Name> nm_l;
  SEQ<TYPE_AS_Type> tp_l;
  size_t len_resnmtps = resnmtps.Length();
  for (size_t idx = 1; idx <= len_resnmtps; idx++) {
    const TYPE_AS_NameType & nmtp (resnmtps[idx]);
    nm_l.ImpAppend(nmtp.GetRecord(pos_AS_NameType_nm));
    tp_l.ImpAppend(nmtp.GetRecord(pos_AS_NameType_tp));
  }
  return mk_(nm_l, tp_l);
}

// ImplicitResType
// resnmtps : seq of AS`NameType
// ==> AS`Type
TYPE_AS_Type DEF::ImplicitResType (const SEQ<TYPE_AS_NameType> & resnmtps)
{
  switch(resnmtps.Length()) {
    case 0: {
      return TYPE_AS_VoidType().Init(NilContextId);
    }
    case 1: {
      return resnmtps[1].GetRecord(pos_AS_NameType_tp);
    }
    default: { // resnmtps.Length() > 1
      size_t len_resnmtps = resnmtps.Length();
      SEQ<TYPE_AS_Type> tp_l;
      for (size_t idx = 1; idx <= len_resnmtps; idx++) {
        const TYPE_AS_NameType & nmtp (resnmtps[idx]);
        tp_l.ImpAppend(nmtp.GetRecord(pos_AS_NameType_tp));
      }
      return TYPE_AS_ProductType().Init(tp_l, NilContextId);
    }
  }
}

// CreateFunctionPreType
// tpdom : seq of AS`Type
// tprng : AS`Type
// parms : seq of AS`Parameters (= seq of seq of AS`Pattern)
// ==> AS`Type
TYPE_AS_Type DEF::CreateFunctionPreType (const SEQ<TYPE_AS_Type> & tpdom,
                                         const TYPE_AS_Type & tprng,
                                         const SEQ<TYPE_AS_Parameters> & parms)
{
  switch (parms.Length()) {
    case 0:
    case 1: {
      TYPE_AS_TotalFnType fn_tp;
      fn_tp.Init(tpdom, TYPE_AS_BooleanType().Init(NilContextId), NilContextId);
      return fn_tp;
    }
    default: {
      // curry function
      switch (tprng.GetTag()) {
        case TAG_TYPE_AS_TotalFnType: {
          const SEQ<TYPE_AS_Type>  & dom (tprng.GetSequence(pos_AS_TotalFnType_fndom));
          const TYPE_AS_Type & rng (tprng.GetRecord(pos_AS_TotalFnType_fnrng));
          TYPE_AS_Type ptp (CreateFunctionPreType(dom, rng, parms.Tl()));  
          return TYPE_AS_TotalFnType().Init(tpdom, ptp, NilContextId);
        }
        case TAG_TYPE_AS_PartialFnType: {
          const SEQ<TYPE_AS_Type> & dom (tprng.GetSequence(pos_AS_PartialFnType_fndom));
          const TYPE_AS_Type & rng      (tprng.GetRecord(pos_AS_PartialFnType_fnrng));
          TYPE_AS_Type ptp (CreateFunctionPreType(dom, rng, parms.Tl()));  
          return TYPE_AS_TotalFnType().Init(tpdom, ptp, NilContextId);
        }
        default: {
          RTERR::Error(L"CreateFunctionPreType", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
          return TYPE_AS_Type();
        }
      }
    }
  }
}

// CreateFunctionPostType
// tpdom_o : AS`DiscretionaryType (seq of Type)
// tprng : AS`DiscretionaryType
// parms : AS`Parameters (not in spec) // for curry
// ==> AS`Type
TYPE_AS_Type DEF::CreateFunctionPostType (const SEQ<TYPE_AS_Type> & tpdom,
                                          const SEQ<TYPE_AS_Type> & tprng,
                                          const SEQ<TYPE_AS_Parameters> & parms)
{
  switch (parms.Length()) {
    case 0:
    case 1: {
      SEQ<TYPE_AS_Type> ntpdom (tpdom);
      if (tprng.Length() <= 1) {
        ntpdom.ImpConc (tprng);
      }
      else {
        ntpdom.ImpAppend(TYPE_AS_ProductType().Init(tprng, ASTAUX::GetCid(tprng.Hd())));
      }
      TYPE_AS_TotalFnType fn_tp;
      fn_tp.Init(ntpdom, TYPE_AS_BooleanType().Init(NilContextId), NilContextId);
      return fn_tp;
    }
    default: {
      // curry function
      // tp must be AS`FnType
      TYPE_AS_Type tp (tprng.Hd());
      switch (tp.GetTag()) {
        case TAG_TYPE_AS_TotalFnType: {
          const SEQ<TYPE_AS_Type> dom (tp.GetSequence(pos_AS_TotalFnType_fndom));
          SEQ<TYPE_AS_Type> rng;
          rng.ImpAppend(tp.GetRecord(pos_AS_TotalFnType_fnrng));
          TYPE_AS_Type ptp (CreateFunctionPostType(dom, rng, parms.Tl()));  
          return TYPE_AS_TotalFnType().Init(tpdom, ptp, NilContextId);
        }
        case TAG_TYPE_AS_PartialFnType: {
          const SEQ<TYPE_AS_Type> dom (tp.GetSequence(pos_AS_PartialFnType_fndom));
          SEQ<TYPE_AS_Type> rng;
          rng.ImpAppend(tp.GetRecord(pos_AS_PartialFnType_fnrng));
          TYPE_AS_Type ptp (CreateFunctionPostType(dom, rng, parms.Tl()));  
          return TYPE_AS_TotalFnType().Init(tpdom, ptp, NilContextId);
        }
        default: {
          RTERR::Error(L"CreateFunctionPostType", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
          return TYPE_AS_Type();
        }
      }
    }
  }
}

// CreateFunctionMeasureType
// tpdom : seq of AS`Type
// tprng : AS`Type
// parms : seq of AS`Parameters (= seq of seq of AS`Pattern)
// ==> AS`Type
TYPE_AS_Type DEF::CreateFunctionMeasureType (const SEQ<TYPE_AS_Type> & tpdom,
                                             const TYPE_AS_Type & tprng,
                                             const SEQ<TYPE_AS_Parameters> & parms)
{
  switch (parms.Length()) {
    case 0:
    case 1: {
      TYPE_AS_TotalFnType fn_tp;
      fn_tp.Init(tpdom, TYPE_AS_AllType().Init(NilContextId), NilContextId);
      return fn_tp;
    }
    default: {
      // curry function
      switch (tprng.GetTag()) {
        case TAG_TYPE_AS_TotalFnType: {
          const SEQ<TYPE_AS_Type> dom (tprng.GetSequence(pos_AS_TotalFnType_fndom));
          const TYPE_AS_Type & rng    (tprng.GetRecord(pos_AS_TotalFnType_fnrng));
          TYPE_AS_Type ptp (CreateFunctionMeasureType(dom, rng, parms.Tl()));  
          return TYPE_AS_TotalFnType().Init(tpdom, ptp, NilContextId);
        }
        case TAG_TYPE_AS_PartialFnType: {
          const SEQ<TYPE_AS_Type> dom (tprng.GetSequence(pos_AS_PartialFnType_fndom));
          const TYPE_AS_Type & rng    (tprng.GetRecord(pos_AS_PartialFnType_fnrng));
          TYPE_AS_Type ptp (CreateFunctionMeasureType(dom, rng, parms.Tl()));  
          return TYPE_AS_TotalFnType().Init(tpdom, ptp, NilContextId);
        }
        default: {
          RTERR::Error(L"CreateFunctionMeasureType", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
          return TYPE_AS_Type();
        }
      }
    }
  }
}

// CreateOperationPreType
// tpdom_o : AS`DiscretionaryType (SEQ<TYPE_AS_Type>)
// st_id : [AS`Name]
// ==> AS`Type
TYPE_AS_Type DEF::CreateOperationPreType (const SEQ<TYPE_AS_Type> & tpdom, const Generic & st_id)
{
  SEQ<TYPE_AS_Type> new_tpdom;
  new_tpdom.ImpConc(tpdom);

#ifdef VDMSL
  if (!st_id.IsNil ()) {
    new_tpdom.ImpAppend(TYPE_AS_TypeName().Init(st_id, ASTAUX::GetCid(st_id)));
  }
#endif // VDMSL

  return TYPE_AS_TotalFnType().Init(new_tpdom, TYPE_AS_BooleanType().Init(NilContextId), NilContextId);
}

// CreateOperationPostType
// tpdom : AS`DiscretionaryType (SEQ<TYPE_AS_Type>)
// tprng : AS`DiscretionaryType (SEQ<TYPE_AS_Type>)
// st_id : [AS`Name]
// ==> AS`Type
TYPE_AS_Type DEF::CreateOperationPostType (const SEQ<TYPE_AS_Type> & tpdom,
                                           const SEQ<TYPE_AS_Type> & tprng,
                                           const Generic & st_id)
{
  SEQ<TYPE_AS_Type> realtprng;
  // This block implemented from PGL spec (based on eval_def
  // v1.35.2.22) 990820
  switch (tprng.Length()) {
    case 0: {
      break;
    }
    case 1: {
      realtprng.ImpConc(tprng);
      break;
    }
    default: {
      realtprng.ImpAppend(TYPE_AS_ProductType().Init(tprng, ASTAUX::GetCid(tprng[1])));
      break; 
    }
  }

  SEQ<TYPE_AS_Type> new_tpdom;
  new_tpdom.ImpConc(tpdom);
  new_tpdom.ImpConc(realtprng);

#ifdef VDMSL
  if (!st_id.IsNil ()) {
    TYPE_AS_TypeName st_tp;
    st_tp.Init(st_id, ASTAUX::GetCid(st_id));
    new_tpdom.ImpAppend(st_tp);
    new_tpdom.ImpAppend(st_tp);
  }
#endif // VDMSL

  return TYPE_AS_TotalFnType().Init(new_tpdom, TYPE_AS_BooleanType().Init(NilContextId), NilContextId);
}

#ifdef VDMPP

// TransOverloaded
// opm : (map AS`Name to AS`OpDef)
// fnm : (map AS`Name to AS`FnDef)
// ==> map AS`Name to GLOBAL`Overloaded
//                          (map nat to map AS`Name to seq of AS`Type)
MAP<TYPE_AS_Name,TYPE_GLOBAL_Overloaded> DEF::TransOverloaded(const MAP<TYPE_AS_Name,TYPE_AS_OpDef> & opm,
                                                              const MAP<TYPE_AS_Name,TYPE_AS_FnDef> & fnm)
{
  SET<TYPE_AS_Name> ids (opm.Dom().ImpUnion(fnm.Dom()));

  MAP<TYPE_AS_Name, TYPE_GLOBAL_Overloaded> over;
  Generic id; // AS`Name
  for (bool bb = ids.First(id); bb; bb = ids.Next(id)) {
    if (MANGLE::IsMangled(id)) {
      Tuple unmangledTup (MANGLE::UnMangle(id));
      const Generic & realid         (unmangledTup.GetField(1));    // AS`Name
      const Int & arit               (unmangledTup.GetInt(2));      // number of arguments
      const SEQ<TYPE_AS_Type> & tp_l (unmangledTup.GetSequence(3)); // seq of AS`Type

      TYPE_AS_Access acc (opm.Dom().InSet(id) ? ASTAUX::GetOpAccess(opm[id]) : ASTAUX::GetFnAccess(fnm[id]));

      if (over.DomExists(realid)) {
        // add overload function
        TYPE_GLOBAL_Overloaded overload (over[realid]); // map nat to map AS`Name to ((seq of AS`Type) * AS`Access)
        Map thisAritVal (overload.DomExists(arit) ? Map(overload[arit]) : Map());
        thisAritVal.ImpModify(id, mk_(tp_l, acc, Nil()));

        overload.ImpModify(arit, thisAritVal);
        over.ImpModify(realid, overload);
      }
      else {
        // new overload function
        TYPE_GLOBAL_Overloaded overload; // map nat to map AS`Name to ((seq of AS`Type) * AS`Access)
        Map thisAritVal; // map AS`Name to ((seq of AS`Type) * AS`Access)
        thisAritVal.ImpModify(id, mk_(tp_l, acc, Nil()));

        overload.ImpModify(arit, thisAritVal);
        over.ImpModify(realid, overload);
      }
//
      // create overload map for pre function
      if (fnm.DomExists(id) && !ASTAUX::GetFnPrecond(fnm[id]).IsNil()) {
        TYPE_AS_Name realpreid (AUX::PreName(realid));
        TYPE_GLOBAL_Overloaded m (over.DomExists(realpreid) ? Map(over[realpreid]) : Map());

        TYPE_AS_Name preid (AUX::PreName(id));
        TYPE_GLOBAL_Overloaded overload (over[realid]); // map nat to map AS`Name to ((seq of AS`Type) * AS`Access)
        SET<Int> dom_overload (overload.Dom());
        Generic arit; // nat
        for (bool cc = dom_overload.First(arit); cc; cc = dom_overload.Next(arit)) {
          Map thisAritVal (overload[arit]); // map AS`Name to ((seq of AS`Type) * AS`Access)
          SET<TYPE_AS_Name> dom_thisAritVal (thisAritVal.Dom());
          Map newover (m.DomExists(arit) ? Map(m[arit]) : Map());
  
          Generic gid;
          for (bool dd = dom_thisAritVal.First(gid); dd; dd = dom_thisAritVal.Next(gid)) {
            newover.ImpModify(preid, thisAritVal[gid]);
          }
          m.ImpModify(arit, newover);
        }
        over.ImpModify(realpreid, m);
      }
      // create overload map for post function
      if (fnm.DomExists(id) && !ASTAUX::GetFnPostcond(fnm[id]).IsNil()) {
        TYPE_AS_Name realpostid (AUX::PostName(realid));
        TYPE_GLOBAL_Overloaded m (over.DomExists(realpostid) ? Map(over[realpostid]) : Map());

        TYPE_AS_Name postid (AUX::PostName(id));
        TYPE_GLOBAL_Overloaded overload (over[realid]); // map nat to map AS`Name to ((seq of AS`Type) * AS`Access)
        SET<Int> dom_overload (overload.Dom());
        Generic arit; // nat
        for (bool cc = dom_overload.First(arit); cc; cc = dom_overload.Next(arit)) {
          Map thisAritVal (overload[arit]); // map AS`Name to ((seq of AS`Type) * AS`Access)
          SET<TYPE_AS_Name> dom_thisAritVal (thisAritVal.Dom());
          Map newover (m.DomExists(arit) ? Map(m[arit]) : Map());
          Generic gid;
          for (bool dd = dom_thisAritVal.First(gid); dd; dd = dom_thisAritVal.Next(gid)) {
            Tuple tacc (thisAritVal[gid]); // ((seq of AS`Type) * AS`Access)
            const SEQ<TYPE_AS_Type> & t (tacc.GetSequence(1)); // seq of AS`Type
            const TYPE_AS_Access & acc (tacc.GetField(2));     // AS`Access
            const TYPE_AS_FnDef & fd (fnm[id]);
            SEQ<TYPE_AS_Type> restp;
            switch(fd.GetTag()) {
              case TAG_TYPE_AS_ExplFnDef: {
                restp.ImpAppend(fd.GetRecord(pos_AS_ExplFnDef_tp).GetRecord(2));
                break;
              }
              case TAG_TYPE_AS_ImplFnDef: {
                const SEQ<TYPE_AS_NameType> & restps (fd.GetSequence(pos_AS_ImplFnDef_resnmtps));
                size_t len_restps = restps.Length();
                for (size_t i = 1; i <= len_restps; i++)
                {
                  restp.ImpAppend(restps[i].GetRecord(pos_AS_NameType_tp));
                }
                break;
              }
              case TAG_TYPE_AS_ExtExplFnDef: {
                const SEQ<TYPE_AS_NameType> & restps (fd.GetSequence(pos_AS_ExtExplFnDef_resnmtps));
                size_t len_restps = restps.Length();
                for (size_t i = 1; i <= len_restps; i++)
                {
                  restp.ImpAppend(restps[i].GetRecord(pos_AS_NameType_tp));
                }
                break;
              }
            }
            TYPE_AS_TotalFnType tft (CreateFunctionPostType(t, restp, Sequence()));            
            newover.ImpModify(postid, mk_(tft.GetSequence(pos_AS_TotalFnType_fndom), acc));
          }
          m.ImpModify(arit, newover);
        }
        over.ImpModify(realpostid, m);
      }
    }
  }
  return over;
}
#endif // VDMPP

// TransOpMap
// mod_id : AS`Name
// OpMap : map AS`Name to AS`OpDef
//Tuple DEF::TransOpMap (const TYPE_AS_Name & mod_id, const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & opm)
// ==> OpMap = map AS`Name to SEM`ExplOP
Map DEF::TransOpMap (const TYPE_AS_Name & mod_id, const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & opm)
{
  Set dom_opm (opm.Dom());
  Map eps;
  Generic id;
  for (bool bb = dom_opm.First (id); bb; bb = dom_opm.Next (id)) {
    const TYPE_AS_OpDef & od (opm[id]);
    TYPE_SEM_VAL OPval (TransOP (mod_id, od));
    switch (OPval.GetTag ()) {
      case TAG_TYPE_SEM_ExplOP: {
        eps.Insert (id, OPval);
        break;
      }
    }
  }
  return eps;
}

// TransOP
// mod_id : AS`Name
// Op : AS`OpDef
// ==> SEM`OP
TYPE_SEM_OP DEF::TransOP (const TYPE_AS_Name & mod_id, const TYPE_AS_OpDef & Op)
{
  switch(Op.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef:    { return TransExplOP(mod_id, Op); }
    case TAG_TYPE_AS_ExtExplOpDef: { return TransExtExplOP(mod_id, Op); }
    case TAG_TYPE_AS_ImplOpDef:    { return TransImplOP(mod_id, Op); }
    default:                       { return TYPE_SEM_VAL(); } // dummy
  }
}

// TransExplOP
// mod_id : AS`Name
// Op : AS`ExplOpDef
// ==> SEM`OP
TYPE_SEM_OP DEF::TransExplOP (const TYPE_AS_Name & mod_id, const TYPE_AS_ExplOpDef & Op)
{
  const TYPE_AS_Name & nm            (Op.GetRecord  (pos_AS_ExplOpDef_nm));
  const TYPE_AS_OpType & tp          (Op.GetRecord  (pos_AS_ExplOpDef_tp));
  const SEQ<TYPE_AS_Pattern> & parms (Op.GetSequence(pos_AS_ExplOpDef_parms));
  const Bool & oppure                (Op.GetBool    (pos_AS_ExplOpDef_oppure));
  const Bool & sync                  (Op.GetBool    (pos_AS_ExplOpDef_opsync));
  const TYPE_AS_Access & access      (Op.GetField   (pos_AS_ExplOpDef_access));

  SEQ<TYPE_STKM_Pattern> pi_l (theCompiler().PL2PL(parms));

  TYPE_SEM_ExplOP OPval;
  OPval.Init(tp,
             oppure,
             sync,
             Nil(),
             pi_l,
             theCompiler().CompileFnOpDef(Op),
             nm,
             mod_id,
             Bool(false),
             Nil(),
             RealAccess(access, ACC_OP));
  return (OPval);
}

// TransImplOP
// mod_id : AS`Name
// Op : AS`ImplOpDef
// ==> SEM`OP
TYPE_SEM_OP DEF::TransImplOP (const TYPE_AS_Name & mod_id, const TYPE_AS_ImplOpDef & Op)
{
  const TYPE_AS_Name & nm                (Op.GetRecord(pos_AS_ImplOpDef_nm));
  const SEQ<TYPE_AS_PatTypePair> & parml (Op.GetSequence(pos_AS_ImplOpDef_partps));
  const SEQ<TYPE_AS_NameType> & resnmtps (Op.GetSequence(pos_AS_ImplOpDef_resnmtps));
  const Bool & oppure                    (Op.GetBool    (pos_AS_ImplOpDef_oppure));
  const Bool & sync                      (Op.GetBool    (pos_AS_ImplOpDef_opsync));
  const TYPE_AS_Access & access          (Op.GetField   (pos_AS_ImplOpDef_access));

  Tuple tmp_parml (ImplicitTypeParams(parml));
  const SEQ<TYPE_AS_Type> & opdom    (tmp_parml.GetSequence(1));
  const SEQ<TYPE_AS_Pattern> & parms (tmp_parml.GetSequence(2));

  TYPE_AS_Type oprng (ImplicitResType(resnmtps));

  TYPE_AS_OpType tp;
  tp.Init(opdom, oprng, NilContextId);

  SEQ<TYPE_STKM_Pattern> pi_l (theCompiler().PL2PL(parms));

  TYPE_SEM_ExplOP OPval;
  OPval.Init(tp,
             oppure,
             sync,
             Nil(),
             pi_l,
             theCompiler().CompileFnOpDef(Op),
             nm,
             mod_id,
             Bool(true),
             Nil(),
             RealAccess(access, ACC_OP));
  return (OPval);
}

// TransExtExplOP
// mod_id : AS`Name
// Op : AS`ExtExplOpDef
// ==> SEM`OP
TYPE_SEM_OP DEF::TransExtExplOP (const TYPE_AS_Name & mod_id, const TYPE_AS_ExtExplOpDef & Op)
{
  const TYPE_AS_Name & nm                (Op.GetRecord(pos_AS_ExtExplOpDef_nm));
  const SEQ<TYPE_AS_PatTypePair> & parml (Op.GetSequence(pos_AS_ExtExplOpDef_partps));
  const SEQ<TYPE_AS_NameType> & resnmtps (Op.GetSequence(pos_AS_ExtExplOpDef_resnmtps));
  const Bool & oppure                    (Op.GetBool(pos_AS_ExtExplOpDef_oppure));
  const Bool & sync                      (Op.GetBool(pos_AS_ExtExplOpDef_opsync));
  const TYPE_AS_Access & access          (Op.GetField   (pos_AS_ExtExplOpDef_access));

  Tuple tmp_parml (ImplicitTypeParams(parml));
  const SEQ<TYPE_AS_Type> & opdom    (tmp_parml.GetSequence(1));
  const SEQ<TYPE_AS_Pattern> & parms (tmp_parml.GetSequence(2));

  TYPE_AS_Type oprng (ImplicitResType(resnmtps));

  TYPE_AS_OpType tp;
  tp.Init(opdom, oprng, NilContextId);

  SEQ<TYPE_STKM_Pattern> pi_l (theCompiler().PL2PL(parms));

  TYPE_SEM_ExplOP OPval;
  OPval.Init(tp,
             oppure,
             sync,
             Nil(),
             pi_l,
             theCompiler().CompileFnOpDef(Op),
             nm,
             mod_id,
             Bool(false),
             Nil(),
             RealAccess(access, ACC_OP));
  return (OPval);
}

// CreateOperationPrePostFns
// mod_id : AS`Name
// opm : (map AS`Name to AS`OpDef)
// gst : [AS`StateDef]
// ==> map AS`Name to SEM`CompExplFN
Map DEF::CreateOperationPrePostFns (const TYPE_AS_Name & mod_id,
                                    const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & opm,
                                    const Generic & gst)
{
  Map res_m;
  Set dom_opm (opm.Dom());
  Generic id;
  for (bool bb = dom_opm.First (id); bb; bb = dom_opm.Next (id)) {
    TYPE_AS_OpDef opd (opm[id]);
    switch(opd.GetTag()) {
      case TAG_TYPE_AS_ExplOpDef: {
        res_m.ImpOverride (CreateExplOpFns (mod_id, opd, gst));
        break;
      }
      case TAG_TYPE_AS_ExtExplOpDef: {
        res_m.ImpOverride (CreateExtExplOpFns (mod_id, opd, gst));
        break;
      }
      case TAG_TYPE_AS_ImplOpDef: {
        res_m.ImpOverride (CreateImplOpFns (mod_id, opd, gst));
        break;
      }
      default:
        break;
    }
  }
  return (res_m);
}

// CreateOperationPreParms
// parm : AS`Parameters (seq of AS`Pattern)
// gst : [AS`StateDef]
// ==> AS`ParametersList
SEQ<TYPE_AS_Parameters> DEF::CreateOperationPreParms (const SEQ<TYPE_AS_Pattern> & parm, const Generic & gst)
{
  SEQ<TYPE_AS_Pattern> tmp_l (parm);

  if (! gst.IsNil ()) {
    tmp_l.ImpAppend (CreateSigmaPattern (Record(gst).GetRecord(pos_AS_StateDef_tp), false));
  }

  SEQ<TYPE_AS_Parameters> res_l;
  res_l.ImpAppend (tmp_l);
  return (res_l);
}

// CreateExplOperationPostParms
// parm : AS`Parameters
// resnmtps : AS`DiscretionaryType (seq of AS`Type)
// gst : [AS`StateDef]
// ==> AS`ParametersList
SEQ<TYPE_AS_Parameters> DEF::CreateExplOperationPostParms (const SEQ<TYPE_AS_Pattern> & parm,
                                                           const SEQ<TYPE_AS_Type> & resnmtps,
                                                           const Generic & gst)
{
  SEQ<TYPE_AS_Pattern> tmp_l (parm);

  if (! resnmtps.IsEmpty ()) {
    TYPE_AS_PatternName pat_id;
    pat_id.Init(ASTAUX::MkNameFromId (ASTAUX::MkId(L"RESULT"), NilContextId), Nil(), NilContextId);
    tmp_l.ImpAppend (pat_id);
  }
  if (! gst.IsNil ()) {
    TYPE_AS_StateDef sd (gst);
    tmp_l.ImpAppend (CreateSigmaPattern (sd.get_tp (), true));
    tmp_l.ImpAppend (CreateSigmaPattern (sd.get_tp (), false));
  }
  SEQ<TYPE_AS_Parameters> res_l;
  res_l.ImpAppend (tmp_l);
  return (res_l);
}

// CreateExtExplOperationPostParms
// parm : AS`Parameters
// resnms : seq of AS`Name
// gst : [AS`StateDef]
// ==> AS`ParametersList
SEQ<TYPE_AS_Parameters> DEF::CreateExtExplOperationPostParms (const SEQ<TYPE_AS_Pattern> & parm,
                                                              const SEQ<TYPE_AS_Name> & resnms,
                                                              const Generic & gst)
{
  SEQ<TYPE_AS_Parameters> res_l;
  SEQ<TYPE_AS_Pattern> res_parm (parm);
  res_parm.ImpConc(CreatePostParms(resnms));

  if (! gst.IsNil ()) {
    TYPE_AS_StateDef sd (gst);
    res_parm.ImpAppend (CreateSigmaPattern (sd.get_tp (), true));
    res_parm.ImpAppend (CreateSigmaPattern (sd.get_tp (), false));
  }
  res_l.ImpAppend (res_parm);
  return (res_l);
}

// CreateImplOperationPostParms
// parm : AS`Parameters
// resnms : seq of AS`Name
// gst : [AS`StateDef]
// ==> AS`ParametersList
SEQ<TYPE_AS_Parameters> DEF::CreateImplOperationPostParms (const SEQ<TYPE_AS_Pattern> & parm,
                                                           const SEQ<TYPE_AS_Name> & resnms,
                                                           const Generic & gst)
{
  SEQ<TYPE_AS_Parameters> res_l;
  SEQ<TYPE_AS_Pattern> res_parm (parm);
  res_parm.ImpConc(CreatePostParms(resnms));

  if (! gst.IsNil ()) {
    TYPE_AS_StateDef sd (gst);
    res_parm.ImpAppend (CreateSigmaPattern (sd.get_tp (), true)); 
    res_parm.ImpAppend (CreateSigmaPattern (sd.get_tp (), false));
  }

  res_l.ImpAppend (res_parm);
  return (res_l);
}

// CreateSigmaPattern
// comp_t : AS`CompositeType
// old_names : bool
// ==> AS`RecordPattern
TYPE_AS_RecordPattern DEF::CreateSigmaPattern (const TYPE_AS_CompositeType & comp_t, bool old_names)
{
  const SEQ<TYPE_AS_Field> & fields (comp_t.GetSequence(pos_AS_CompositeType_fields));

  SEQ<TYPE_AS_Pattern> rec_l;
  size_t len_fields = fields.Length();
  for (size_t idx = 1; idx <= len_fields; idx++) {
    const TYPE_AS_Field & field (fields[idx]);
    const Generic & gsel (field.GetField(pos_AS_Field_sel));

    if (gsel.IsNil ()) {
      rec_l.ImpAppend (TYPE_AS_PatternName().Init(Nil(), Nil(), field.GetInt(pos_AS_Field_cid)));
    }
    else {
      TYPE_AS_Name sel (gsel);
      Sequence id (sel.GetSequence(pos_AS_Name_ids).Last());
      TYPE_AS_Name new_sel (ASTAUX::MkNameFromId(((old_names) ? id.ImpAppend(Char('~')) : id), sel.GetInt(pos_AS_Name_cid)));

      rec_l.ImpAppend (TYPE_AS_PatternName().Init(new_sel, Nil(), field.GetInt(pos_AS_Field_cid)));
    }
  }
  return TYPE_AS_RecordPattern().Init(comp_t.GetRecord(pos_AS_CompositeType_name),
                                      rec_l,
                                      comp_t.GetInt(pos_AS_CompositeType_cid));
}

// CreateExplOpFns
// mod_id : AS`Name
// op_def : AS`ExplOpDef
// gst : [AS`StateDef]
// ==> map AS`Name to SEM`CompExplFN
Map DEF::CreateExplOpFns (const TYPE_AS_Name & mod_id, const TYPE_AS_ExplOpDef & op_def, const Generic & gst)
{
  const TYPE_AS_Name & nm            (op_def.GetRecord(pos_AS_ExplOpDef_nm));
  const TYPE_AS_OpType & tp          (op_def.GetRecord(pos_AS_ExplOpDef_tp));
  const SEQ<TYPE_AS_Pattern> & parms (op_def.GetSequence(pos_AS_ExplOpDef_parms));
  const Generic & pre_e              (op_def.GetField(pos_AS_ExplOpDef_oppre));
#ifdef VDMSL
  const Generic & post_e             (op_def.GetField(pos_AS_ExplOpDef_oppost));
#endif // VDMSL
  const TYPE_AS_Access & access      (op_def.GetField(pos_AS_ExplOpDef_access));

  Generic st_id = Nil();
#ifdef VDMSL
  if (!gst.IsNil ()) {
    st_id = TYPE_AS_StateDef(gst).GetRecord(pos_AS_StateDef_tp).GetRecord(pos_AS_CompositeType_name);
  }
#endif // VDMSL

  Map res_m;

  if (! pre_e.IsNil ()) {
    SEQ<TYPE_AS_Parameters> pl (CreateOperationPreParms(parms, gst));

    SEQ<type_dL> pi_l; // seq of seq of STKM`Pattern
    size_t len_pl = pl.Length();
    for (size_t idx = 1; idx <= len_pl; idx++) {
      pi_l.ImpAppend(theCompiler().PL2PL(pl[idx]));
    }

    TYPE_AS_Name nm_pre (AUX::PreName(nm));

    TYPE_SEM_ExplFN fn_pre;
    fn_pre.Init(CreateOperationPreType (tp.get_opdom (), st_id),
                pi_l,
                theCompiler().CompilePrePostExpr(pre_e),
                AUX::MkEmptyBlkEnv (sem_read_only),
                MAP<TYPE_AS_TypeVar,TYPE_AS_Type>(),
                nm_pre,
                mod_id,
                Bool(false),
                Nil(),
                RealAccess(access, ACC_OP));

    res_m.Insert (nm_pre, SemRec::CompFN(fn_pre));
  }

#ifdef VDMSL
  if (! post_e.IsNil ()) {
    TYPE_AS_Expr expr_post (gst.IsNil() ? TYPE_AS_Expr (post_e) : OLD::OldNameInExpr (post_e));

    // This block and parameters to CreateExplOperationPostParms and
    // CreateOperationPostType implemented from PGL spec (based on
    // eval_def v1.35.2.22) 990820
    //
    SEQ<TYPE_AS_Type> oprng_l;
    const TYPE_AS_Type & oprng (tp.GetRecord(pos_AS_OpType_oprng));
    if (!oprng.Is(TAG_TYPE_AS_VoidType)) {
      oprng_l.ImpAppend(oprng);
    }

    SEQ<TYPE_AS_Parameters> parm_post (CreateExplOperationPostParms (parms, oprng_l, gst));
    SEQ<type_dL> pi_l; // seq of seq of STKM`Pattern
    size_t len = parm_post.Length();
    for (size_t i = 1; i <= len; i++) {
      pi_l.ImpAppend(theCompiler().PL2PL(parm_post[i]));
    }

    TYPE_AS_Name nm_post (AUX::PostName(nm));

    TYPE_SEM_ExplFN fn_post;
    fn_post.Init(CreateOperationPostType (tp.get_opdom (), oprng_l, st_id),
                 pi_l,
                 theCompiler().CompilePrePostExpr(expr_post),
                 AUX::MkEmptyBlkEnv (sem_read_only),
                 MAP<TYPE_AS_TypeVar,TYPE_AS_Type>(),
                 nm_post,
                 mod_id,
                 Bool(false),
                 Nil(),
                 RealAccess(access, ACC_OP));

    res_m.Insert (nm_post, SemRec::CompFN(fn_post));
  }
#endif //VDMSL
  return (res_m);
}

// CreateExtExplOpFns
// mod_id : AS`Name
// op_def : AS`ExtExplOpDef
// gst : [AS`StateDef]
// ==> map AS`Name to SEM`CompExplFN
Map DEF::CreateExtExplOpFns (const TYPE_AS_Name & mod_id, const TYPE_AS_ExtExplOpDef & op_def, const Generic & gst)
{
  const TYPE_AS_Name & nm                 (op_def.GetRecord(pos_AS_ExtExplOpDef_nm));
  const SEQ<TYPE_AS_PatTypePair> & partps (op_def.GetSequence(pos_AS_ExtExplOpDef_partps));
#ifdef VDMSL
  const SEQ<TYPE_AS_NameType> & resnmtps  (op_def.GetSequence(pos_AS_ExtExplOpDef_resnmtps));
#endif // VDMSL
  const Generic & pre_e                   (op_def.GetField(pos_AS_ExtExplOpDef_oppre));
#ifdef VDMSL
  const Generic & post_e                  (op_def.GetField(pos_AS_ExtExplOpDef_oppost));
#endif // VDMSL
  const TYPE_AS_Access & access           (op_def.GetField(pos_AS_ExtExplOpDef_access));

  Tuple tmp_partps (ImplicitTypeParams(partps));
  const SEQ<TYPE_AS_Type> & opdom    (tmp_partps.GetSequence(1));
  const SEQ<TYPE_AS_Pattern> & parms (tmp_partps.GetSequence(2));

#ifdef VDMSL
  Tuple tmp_resnmtps (ImplicitResNameTypes(resnmtps));
  const SEQ<TYPE_AS_Name> & resnms (tmp_resnmtps.GetSequence(1));
  const SEQ<TYPE_AS_Type> & restps (tmp_resnmtps.GetSequence(2));
#endif // VDMSL

  Generic st_id = Nil();
#ifdef VDMSL
  if (!gst.IsNil ()) {
    st_id = TYPE_AS_StateDef(gst).GetRecord(pos_AS_StateDef_tp).GetRecord(pos_AS_CompositeType_name);
  }
#endif // VDMSL

  Map res_m;

  if (! pre_e.IsNil ()) {
    SEQ<TYPE_AS_Parameters> pl (CreateOperationPreParms(parms, gst));
    SEQ<type_dL> pi_l; // seq of seq of STKM`Pattern
    size_t len_pl = pl.Length();
    for (size_t idx = 1; idx <= len_pl; idx++) {
      pi_l.ImpAppend(theCompiler().PL2PL(pl[idx]));
    }

    TYPE_AS_Name nm_pre (AUX::PreName(nm));

    TYPE_SEM_ExplFN fn_pre;
    fn_pre.Init(CreateOperationPreType (opdom, st_id),
                pi_l,
                theCompiler().CompilePrePostExpr(pre_e),
                AUX::MkEmptyBlkEnv (sem_read_only),
                MAP<TYPE_AS_TypeVar,TYPE_AS_Type>(),
                nm_pre,
                mod_id,
                Bool(false),
                Nil(),
                RealAccess(access, ACC_OP));

    res_m.Insert (nm_pre, SemRec::CompFN(fn_pre));
  }

#ifdef VDMSL
  if (! post_e.IsNil ()) {
    TYPE_AS_Expr expr_post (gst.IsNil() ? TYPE_AS_Expr (post_e) : OLD::OldNameInExpr (post_e));

    SEQ<TYPE_AS_Parameters> parm_post (CreateExtExplOperationPostParms (parms, resnms, gst));
    SEQ<type_dL> pi_l; // seq of seq of STKM`Pattern
    size_t len_parm_post = parm_post.Length();
    for (size_t idx = 1; idx <= len_parm_post; idx++) {
      pi_l.ImpAppend(theCompiler().PL2PL(parm_post[idx]));
    }

    TYPE_AS_Name nm_post (AUX::PostName(nm));

    TYPE_SEM_ExplFN fn_post;
    fn_post.Init(CreateOperationPostType (opdom, restps, st_id),
                 pi_l,
                 theCompiler().CompilePrePostExpr(expr_post),
                 AUX::MkEmptyBlkEnv (sem_read_only),
                 MAP<TYPE_AS_TypeVar,TYPE_AS_Type>(),
                 nm_post,
                 mod_id,
                 Bool(false),
                 Nil(),
                 RealAccess(access, ACC_OP));

    res_m.Insert (nm_post, SemRec::CompFN(fn_post));
  }
#endif //VDMSL
  return (res_m);
}

// CreateImplOpFns
// mod_id : AS`Name
// op_def : AS`ImplOpDef
// gst : [AS`StateDef]
// ==> map AS`Name to SEM`CompExplFN
Map DEF::CreateImplOpFns (const TYPE_AS_Name & mod_id, const TYPE_AS_ImplOpDef & op_def, const Generic & gst)
{
  const TYPE_AS_Name & nm                 (op_def.GetRecord(pos_AS_ImplOpDef_nm));
  const SEQ<TYPE_AS_PatTypePair> & partps (op_def.GetSequence(pos_AS_ImplOpDef_partps));
#ifdef VDMSL
  const SEQ<TYPE_AS_NameType> & resnmtps  (op_def.GetSequence(pos_AS_ImplOpDef_resnmtps));
#endif // VDMSL
  const Generic & pre_e                   (op_def.GetField(pos_AS_ImplOpDef_oppre));
#ifdef VDMSL
  const Generic & post_e                  (op_def.GetField(pos_AS_ImplOpDef_oppost));
#endif // VDMSL
  const TYPE_AS_Access & access           (op_def.GetField(pos_AS_ImplOpDef_access));

  Tuple tmp_partps (ImplicitTypeParams(partps));
  const SEQ<TYPE_AS_Type> & op_tp      (tmp_partps.GetSequence(1));
  const SEQ<TYPE_AS_Pattern> & op_parm (tmp_partps.GetSequence(2));

#ifdef VDMSL
  Tuple tmp_resnmtps (ImplicitResNameTypes(resnmtps));
  const SEQ<TYPE_AS_Name> & res_nms (tmp_resnmtps.GetSequence(1));
  const SEQ<TYPE_AS_Type> & res_tps (tmp_resnmtps.GetSequence(2));
#endif // VDMSL

  Generic st_id = Nil();
#ifdef VDMSL
  if (!gst.IsNil ()) {
    st_id = TYPE_AS_StateDef(gst).GetRecord(pos_AS_StateDef_tp).GetRecord(pos_AS_CompositeType_name);
  }
#endif // VDMSL

  Map res_m;

  if (! pre_e.IsNil ()) {
    SEQ<TYPE_AS_Parameters> pi_l (CreateOperationPreParms(op_parm, gst));
    SEQ<type_dL> pi_ll; // seq of seq of STKM`Pattern
    size_t len_pi_l = pi_l.Length();
    for (size_t idx = 1; idx <= len_pi_l; idx++) {
      pi_ll.ImpAppend(theCompiler().PL2PL(pi_l[idx]));
    }

    TYPE_AS_Name nm_pre (AUX::PreName(nm));

    TYPE_SEM_ExplFN fn_pre;
    fn_pre.Init(CreateOperationPreType (op_tp, st_id),
                pi_ll,
                theCompiler().CompilePrePostExpr(pre_e),
                AUX::MkEmptyBlkEnv (sem_read_only),
                MAP<TYPE_AS_TypeVar,TYPE_AS_Type>(),
                nm_pre,
                mod_id,
                Bool(false),
                Nil(),
                RealAccess(access, ACC_OP));

    res_m.Insert (nm_pre, SemRec::CompFN(fn_pre));
  }

#ifdef VDMSL
  TYPE_AS_Expr expr_post (gst.IsNil() ? TYPE_AS_Expr (post_e) : OLD::OldNameInExpr (post_e));

  SEQ<TYPE_AS_Parameters> parm_post (CreateImplOperationPostParms (op_parm, res_nms, gst));
  SEQ<type_dL> pi_l; // seq of seq of STKM`Pattern
  size_t len_parm_post = parm_post.Length();
  for (size_t idx = 1; idx <= len_parm_post; idx++) {
    pi_l.ImpAppend(theCompiler().PL2PL(parm_post[idx]));
  }

  TYPE_AS_Name nm_post (AUX::PostName(nm));

  TYPE_SEM_ExplFN fn_post;
  fn_post.Init(CreateOperationPostType (op_tp, res_tps, st_id),
               pi_l,
               theCompiler().CompilePrePostExpr(expr_post),
               AUX::MkEmptyBlkEnv (sem_read_only),
               MAP<TYPE_AS_TypeVar,TYPE_AS_Type>(),
               nm_post,
               mod_id,
               Bool(false),
               Nil(),
               RealAccess(access, ACC_OP));

  res_m.Insert (nm_post, SemRec::CompFN(fn_post));

#endif //VDMSL
  return (res_m);
}

// TransTpDefMap
// TpMap : map AS`Name to AS`TypeDef
// ==> map AS`Name to GLOBAL`RecSel
Map DEF::TransTpDefMap (const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & tm)
{
  Set dom_tm (tm.Dom());
  Set s;
  Generic nm;
  for (bool bb = dom_tm.First (nm); bb; bb = dom_tm.Next (nm)) {
    const TYPE_AS_TypeDef & td (tm[nm]);
    s.Insert(mk_(td.GetRecord(pos_AS_TypeDef_shape), td.GetField(pos_AS_TypeDef_access)));
  }
  return TransTP(s);
}

// TransTP
// typeset : set of (AS`Type * AS`Access)
// ==> map AS`Name to GLOBAL`RecSel
Map DEF::TransTP(const Set & typeset_)
{
  if (typeset_.IsEmpty()) {
    return Map();
  }

  Set typeset (typeset_);
  Map res;
  // Note the specification is tail-recursive. This is changed to a loop
  // here in the implementation.
  Generic g;
  for (bool bb = typeset.First(g); bb; bb = typeset.Next(g)) {
    Tuple t (g);                                    // (AS`Type * AS`Access)
    const TYPE_AS_Type & type     (t.GetRecord(1)); // AS`Type
    const TYPE_AS_Access & access (t.GetField(2));  // AS`Access

    switch (type.GetTag()) {
      case TAG_TYPE_AS_CompositeType: {
        const TYPE_AS_Name & nm           (type.GetRecord(pos_AS_CompositeType_name));
        const SEQ<TYPE_AS_Field> & fields (type.GetSequence(pos_AS_CompositeType_fields));

        //res.Insert(nm, mk_(TCompT(nm, fields), access));
        //Tuple t (mk_(TCompT(nm, fields, access), access));
        Tuple t (TCompT(nm, fields, access));
        if (res.DomExists(nm)) {
          if (t != res[nm]) {
            RTERR::InitError (L"TransTP", RTERR_TYPE_ALREADY_DEF, Nil(), Nil(), ASTAUX::GetCid(nm), Sequence());
          }
        }
        res.Insert(nm, t);
  
        Set s;
        size_t len_fields = fields.Length();
        for (size_t idx = 1; idx <= len_fields; idx++) {
          s.Insert(mk_(fields[idx].GetRecord(pos_AS_Field_type), access));
        }
        res.ImpOverride(TransTP(s));
        break;
      }

      case TAG_TYPE_AS_UnionType: {
        const SEQ<TYPE_AS_Type> & tpset (type.GetSequence(pos_AS_UnionType_tps));
        Set s;
        size_t len_tpset = tpset.Length();
        for (size_t idx = 1; idx <= len_tpset; idx++) {
          s.Insert(mk_(tpset[idx], access)); // 20060915
        }
        res.ImpOverride(TransTP(s));
        break;
      }
      case TAG_TYPE_AS_ProductType: {
        const SEQ<TYPE_AS_Type> & tpseq (type.GetSequence(pos_AS_ProductType_tps));
        Set s;
        size_t len_tpset = tpseq.Length();
        for (size_t idx = 1; idx <= len_tpset; idx++) {
          s.Insert(mk_(tpseq[idx], access)); // 20060915
        }
        res.ImpOverride(TransTP(s));
        break;
      }
  
      case TAG_TYPE_AS_OptionalType: {
        Set s;
        s.Insert(mk_(type.GetRecord(pos_AS_OptionalType_tp), access));
        res.ImpOverride(TransTP(s));
        break;
      }
      case TAG_TYPE_AS_Set0Type: {
        Set s;
        s.Insert(mk_(type.GetRecord(pos_AS_Set0Type_elemtp), access));
        res.ImpOverride(TransTP(s));
        break;
      }
      case TAG_TYPE_AS_Set1Type: {
        Set s;
        s.Insert(mk_(type.GetRecord(pos_AS_Set1Type_elemtp), access));
        res.ImpOverride(TransTP(s));
        break;
      }
      case TAG_TYPE_AS_BracketedType: {
        Set s;
        s.Insert(mk_(type.GetRecord(pos_AS_BracketedType_tp), access));
        res.ImpOverride(TransTP(s));
        break;
      }
      case TAG_TYPE_AS_Seq0Type: {
        Set s;
        s.Insert(mk_(type.GetRecord(pos_AS_Seq0Type_elemtp), access));
        res.ImpOverride(TransTP(s));
        break;
      }
      case TAG_TYPE_AS_Seq1Type: {
        Set s;
        s.Insert(mk_(type.GetRecord(pos_AS_Seq1Type_elemtp), access));
        res.ImpOverride(TransTP(s));
        break;
      }
      case TAG_TYPE_AS_GeneralMap0Type: {
        Set s;
        s.Insert(mk_(type.GetRecord(pos_AS_GeneralMap0Type_mapdom), access));
        s.Insert(mk_(type.GetRecord(pos_AS_GeneralMap0Type_maprng), access));
        res.ImpOverride(TransTP(s));
        break;
      }
      case TAG_TYPE_AS_GeneralMap1Type: {
        Set s;
        s.Insert(mk_(type.GetRecord(pos_AS_GeneralMap1Type_mapdom), access));
        s.Insert(mk_(type.GetRecord(pos_AS_GeneralMap1Type_maprng), access));
        res.ImpOverride(TransTP(s));
        break;
      }
      case TAG_TYPE_AS_InjectiveMap0Type: {
        Set s;
        s.Insert(mk_(type.GetRecord(pos_AS_InjectiveMap0Type_mapdom), access));
        s.Insert(mk_(type.GetRecord(pos_AS_InjectiveMap0Type_maprng), access));
        res.ImpOverride(TransTP(s));
        break;
      }
      case TAG_TYPE_AS_InjectiveMap1Type: {
        Set s;
        s.Insert(mk_(type.GetRecord(pos_AS_InjectiveMap1Type_mapdom), access));
        s.Insert(mk_(type.GetRecord(pos_AS_InjectiveMap1Type_maprng), access));
        res.ImpOverride(TransTP(s));
        break;
      }
      case TAG_TYPE_AS_PartialFnType: {
        const SEQ<TYPE_AS_Type> & dt (type.GetSequence(pos_AS_PartialFnType_fndom));
        Set s;
        s.Insert(mk_(type.GetRecord(pos_AS_PartialFnType_fnrng), access));
        size_t len = dt.Length();
        for (size_t i = 1; i <= len; i++ ) {
          s.Insert(mk_(dt[i], access));
        }
        res.ImpOverride(TransTP(s));
        break;
      }
      case TAG_TYPE_AS_TotalFnType: {
        const SEQ<TYPE_AS_Type> & dt (type.GetSequence(pos_AS_TotalFnType_fndom));
        Set s;
        s.Insert(mk_(type.GetRecord(pos_AS_TotalFnType_fnrng), access));
        size_t len = dt.Length();
        for (size_t i = 1; i <= len; i++ ) {
          s.Insert(mk_(dt[i], access));
        }
        res.ImpOverride(TransTP(s));
        break;
      }
      case TAG_TYPE_AS_OpType: {
        const SEQ<TYPE_AS_Type> & dt (type.GetSequence(pos_AS_OpType_opdom));
        const TYPE_AS_Type & oprng   (type.GetRecord(pos_AS_OpType_oprng));
        Set s;
        s.Insert(mk_(oprng, access));
        size_t len = dt.Length();
        for (size_t i = 1; i <= len; i++ ) {
          s.Insert(mk_(dt[i], access));
        }
        res.ImpOverride(TransTP(s));
        break;
      }
    }
  }
  return res;
}

// This function needs the id argument in the implementation, because of
// the implementation specific representation of Records.

// TCompT
// id : AS`Name (for Decl_SEM_REC, not in spec)
// fields : seq of AS`Field
// ==> GLOBAL`RecSel
Tuple DEF::TCompT(const TYPE_AS_Name & id, const SEQ<TYPE_AS_Field> & fields, const TYPE_AS_Access & access)
{
  MAP<TYPE_AS_Name, Int> pos;
  SEQ<TYPE_AS_Type> type_l;
  SEQ<Bool> dc_s; // for Decl_SEM_REC, not in spec

  size_t size = fields.Length ();
  for (size_t i = 1; i <= size; i++) {
    const TYPE_AS_Field & field (fields[i]);

    const Generic & sel (field.GetField(pos_AS_Field_sel));
    type_l.ImpAppend (field.GetRecord(pos_AS_Field_type));

    if (!sel.IsNil ()) {
      if (pos.DomExists (sel)) {
        RTERR::InitError (L"TCompT", RTERR_IDENTICAL_FIELDS, Nil(), Nil(), ASTAUX::GetCid(field), Sequence());
      }
      else {
        pos.Insert (sel, Int (i));
      }
    }
    dc_s.ImpAppend(field.GetBool(pos_AS_Field_dc)); // Don't care
  }

  // Implementation specific:
  // Declare the semantic record value.
  SemRec::Decl_SEM_REC(id, size, dc_s);

  return mk_(Int (size), pos, type_l, RealAccess(access, ACC_TP));
}

#ifdef VDMSL
// CreateStateInvAndInit
// mod_id : AS`Name
// statedef : AS`StateDef
// ==> map AS`Name to SEM`CompExplFN
Map DEF::CreateStateInvAndInit(const TYPE_AS_Name & mod_id, const TYPE_AS_StateDef & statedef)
{
  const TYPE_AS_Type & tp (statedef.GetRecord(pos_AS_StateDef_tp));
  const Generic & Invg    (statedef.GetField (pos_AS_StateDef_Inv));
  const Generic & Initg   (statedef.GetField (pos_AS_StateDef_Init));

  const TYPE_AS_Name & stateName (tp.GetRecord (pos_AS_CompositeType_name));

  TYPE_AS_TotalFnType tfp;
  tfp.Init(SEQ<TYPE_AS_Type>().ImpAppend(tp), TYPE_AS_BooleanType().Init(NilContextId), NilContextId);

  Map res; // map AS`Name to (SEM`CompExplFN * AS`Access)
  if (!Invg.IsNil()) {
    TYPE_AS_Invariant Inv (Invg);
    const TYPE_AS_Pattern & pattern (Inv.GetRecord(pos_AS_Invariant_pat));
    const TYPE_AS_Expr & expr       (Inv.GetRecord(pos_AS_Invariant_expr));

    SEQ<TYPE_AS_Parameters> parms;
    parms.ImpAppend(SEQ<TYPE_AS_Pattern>().ImpAppend(pattern));

    TYPE_AS_Name stateInvName (AUX::InvName(stateName));

    TYPE_AS_ExplFnDef func;
    func.Init(stateInvName,
              SEQ<TYPE_AS_TypeVar>(),
              tfp,
              parms,
              TYPE_AS_FnBody().Init(expr, NilContextId),
              Nil(),
              Nil(),
              Default_Tp,
              Bool(false),
              Nil(),
              NilContextId);

    res.Insert (stateInvName, TransFN (mod_id, func));
  }

  if (!Initg.IsNil()) {
    // init s == s = mk_sigma(...)
    TYPE_AS_StateInit Init (Initg);
    const TYPE_AS_Pattern & pattern (Init.GetRecord(pos_AS_StateInit_pat));
    const TYPE_AS_Expr & expr (Init.GetRecord(pos_AS_StateInit_expr));

    SEQ<TYPE_AS_Parameters> parms;
    parms.ImpAppend(SEQ<TYPE_AS_Pattern>().ImpAppend(pattern));

    TYPE_AS_Name stateInitName (AUX::InitName(stateName));
    TYPE_AS_ExplFnDef func;
    func.Init(stateInitName,
              SEQ<TYPE_AS_TypeVar>(),
              tfp,
              parms,
              TYPE_AS_FnBody().Init(expr, NilContextId),
              Nil(),
              Nil(),
              Default_Inst,
              Bool(false),
              Nil(),
              NilContextId);

    res.Insert (stateInitName, TransFN (mod_id, func));
  }
  return res;
}
#endif //VDMSL

// CreateInvs
// mod_id : AS`Name
// tm : map AS`Name to AS`TypeDef
// ==> map AS`Name to SEM`CompExplFN
Map DEF::CreateInvs (const TYPE_AS_Name & mod_id, const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & tm)
{
  TYPE_AS_Type btp (TYPE_AS_BooleanType().Init(NilContextId));
  Map tmp; // map AS`Name to (SEM`CompExplFN * AS`Access)
  Set dom_tm (tm.Dom());
  Generic name;
  for (bool bb = dom_tm.First (name); bb; bb = dom_tm.Next (name)) {
    TYPE_AS_TypeDef tdef (tm[name]);
    const Generic & inv_g (tdef.GetField(pos_AS_TypeDef_Inv));
    const Generic & order_g (tdef.GetField(pos_AS_TypeDef_Ord));
    const Generic & equal_g (tdef.GetField(pos_AS_TypeDef_Eq));

    if (!inv_g.IsNil ()) {
      TYPE_AS_Invariant inv (inv_g);
      const TYPE_AS_Pattern & pattern (inv.GetRecord(pos_AS_Invariant_pat));
      const TYPE_AS_Expr & expr       (inv.GetRecord(pos_AS_Invariant_expr));
      const TYPE_AS_Access & access   (tdef.GetField(pos_AS_TypeDef_access));

      SEQ<TYPE_AS_Parameters> parms;
      parms.ImpAppend (SEQ<TYPE_AS_Pattern>().ImpAppend (pattern));

      TYPE_AS_Type shape (tdef.GetRecord(pos_AS_TypeDef_shape));
      if (shape.Is(TAG_TYPE_AS_CompositeType)) {
        shape = TYPE_AS_TypeName().Init(shape.GetRecord(pos_AS_CompositeType_name), NilContextId);
      }

      TYPE_AS_TotalFnType tp;
      tp.Init(SEQ<TYPE_AS_Type>().ImpAppend (shape), btp, NilContextId);

      TYPE_AS_Name typeInvName (AUX::InvName(AUX::ExtractName(name)));

      TYPE_AS_ExplFnDef func;
      func.Init(typeInvName,
                SEQ<TYPE_AS_TypeVar>(),
                tp,
                parms,
                TYPE_AS_FnBody().Init(expr, NilContextId),
                Nil (),
                Nil (),
                access,
                Bool(false),
                Nil(),
                NilContextId);

      tmp.Insert (typeInvName, TransFN (mod_id, func));
    }

    if (!order_g.IsNil ()) {
      TYPE_AS_Order order (order_g);
      const TYPE_AS_Pattern & lhs   (order.GetRecord(pos_AS_Order_lhs));
      const TYPE_AS_Pattern & rhs   (order.GetRecord(pos_AS_Order_rhs));
      const TYPE_AS_Expr & expr     (order.GetRecord(pos_AS_Order_expr));
      const TYPE_AS_Access & access (tdef.GetField(pos_AS_TypeDef_access));

      TYPE_AS_Name param1 (ASTAUX::MkNameFromId (ASTAUX::MkId(L"p1"), NilContextId));
      TYPE_AS_Name param2 (ASTAUX::MkNameFromId (ASTAUX::MkId(L"p2"), NilContextId));
      TYPE_AS_PatternName pn1 (TYPE_AS_PatternName().Init(param1,Nil(),NilContextId));
      TYPE_AS_PatternName pn2 (TYPE_AS_PatternName().Init(param2,Nil(),NilContextId));
      // if !(param1 < param2) ...
      TYPE_AS_Expr cond (TYPE_AS_LetExpr().Init(
         mk_sequence(TYPE_AS_ValueDef().Init(lhs, Nil(), param1, Int (NOT_INITIALISED_AS),
                                             Bool(true), NilContextId),
                     TYPE_AS_ValueDef().Init(rhs, Nil(), param2, Int (NOT_INITIALISED_AS),
                                             Bool(true), NilContextId)),
         TYPE_AS_PrefixExpr().Init(Int(NOT),expr,NilContextId), NilContextId));
      TYPE_AS_Expr maxExpr (TYPE_AS_IfExpr().Init(cond,param1,Sequence(),param2,NilContextId));
      TYPE_AS_Expr minExpr (TYPE_AS_IfExpr().Init(cond,param2,Sequence(),param1,NilContextId));
      
      SEQ<TYPE_AS_Parameters> parms (mk_sequence(mk_sequence(lhs, rhs)));
      SEQ<TYPE_AS_Parameters> maxMinParms (mk_sequence(mk_sequence(pn2, pn1)));

      TYPE_AS_Type shape (tdef.GetRecord(pos_AS_TypeDef_shape));
      if (shape.Is(TAG_TYPE_AS_CompositeType)) {
        shape = TYPE_AS_TypeName().Init(shape.GetRecord(pos_AS_CompositeType_name), NilContextId);
      }

      TYPE_AS_TotalFnType fntp;
      fntp.Init(SEQ<TYPE_AS_Type>().ImpAppend (shape).ImpAppend(shape), btp, NilContextId);

      TYPE_AS_TotalFnType maxMinFntp;
      maxMinFntp.Init(SEQ<TYPE_AS_Type>().ImpAppend (shape).ImpAppend(shape), shape, NilContextId);

      TYPE_AS_Name orderName (AUX::OrderName(AUX::ExtractName(name)));
      TYPE_AS_Name maxName (AUX::MaxName(AUX::ExtractName(name)));
      TYPE_AS_Name minName (AUX::MinName(AUX::ExtractName(name)));

      TYPE_AS_ExplFnDef orderFunc;
      orderFunc.Init(orderName,
                SEQ<TYPE_AS_TypeVar>(),
                fntp,
                parms,
                TYPE_AS_FnBody().Init(expr, NilContextId),
                Nil (),
                Nil (),
                access,
                Bool(false),
                Nil(),
                NilContextId);

      tmp.Insert (orderName, TransFN (mod_id, orderFunc));

      TYPE_AS_ExplFnDef maxFunc;
      maxFunc.Init(maxName,
                SEQ<TYPE_AS_TypeVar>(),
                maxMinFntp,
                maxMinParms,
                TYPE_AS_FnBody().Init(maxExpr, NilContextId),
                Nil (),
                Nil (),
                access,
                Bool(false),
                Nil(),
                NilContextId);

      tmp.Insert (maxName, TransFN (mod_id, maxFunc));

      TYPE_AS_ExplFnDef minFunc;
      minFunc.Init(minName,
                SEQ<TYPE_AS_TypeVar>(),
                maxMinFntp,
                maxMinParms,
                TYPE_AS_FnBody().Init(minExpr, NilContextId),
                Nil (),
                Nil (),
                access,
                Bool(false),
                Nil(),
                NilContextId);

      tmp.Insert (minName, TransFN (mod_id, minFunc));
    }

    if (!equal_g.IsNil ()) {
      TYPE_AS_Equal equal (equal_g);
      const TYPE_AS_Pattern & lhs   (equal.GetRecord(pos_AS_Equal_lhs));
      const TYPE_AS_Pattern & rhs   (equal.GetRecord(pos_AS_Equal_rhs));
      const TYPE_AS_Expr & expr     (equal.GetRecord(pos_AS_Equal_expr));
      const TYPE_AS_Access & access (tdef.GetField(pos_AS_TypeDef_access));

      SEQ<TYPE_AS_Parameters> parms;
      parms.ImpAppend (SEQ<TYPE_AS_Pattern>().ImpAppend (lhs).ImpAppend(rhs));

      TYPE_AS_Type shape (tdef.GetRecord(pos_AS_TypeDef_shape));
      if (shape.Is(TAG_TYPE_AS_CompositeType)) {
        shape = TYPE_AS_TypeName().Init(shape.GetRecord(pos_AS_CompositeType_name), NilContextId);
      }

      TYPE_AS_TotalFnType tp;
      tp.Init(SEQ<TYPE_AS_Type>().ImpAppend (shape).ImpAppend(shape), btp, NilContextId);

      TYPE_AS_Name equalityName (AUX::EqualityName(AUX::ExtractName(name)));

      TYPE_AS_ExplFnDef func;
      func.Init(equalityName,
                SEQ<TYPE_AS_TypeVar>(),
                tp,
                parms,
                TYPE_AS_FnBody().Init(expr, NilContextId),
                Nil (),
                Nil (),
                access,
                Bool(false),
                Nil(),
                NilContextId);

      tmp.Insert (equalityName, TransFN (mod_id, func));
    }
  }
  return tmp;
}

// RealAccess
// access : AS`Access
// kind : (<OP>|<FN>|<VAL>|<TP>|<INST>)
// +> AS`Access
TYPE_AS_Access DEF::RealAccess(const TYPE_AS_Access & access, accessType kind)
{
  if (access == Int(DEFAULT_AS)) {
    switch (kind) {
      case ACC_OP:   return Default_Op;
      case ACC_FN:   return Default_Fn;
      case ACC_VAL:  return Default_Val;
      case ACC_TP:   return Default_Tp;
      case ACC_INST: return Default_Inst;
    }
  }

  if (access == Int(NOT_INITIALISED_AS)) {
    return Int(PRIVATE_AS);
  }
  return access;
}

#ifdef VDMSL
// TranslateDLExportSig
// mod_id : AS`Name
// dlexportsig : AS`DLExportSig
// useslib : [AS`TextLit]
// ==> GLOBAL`SigmaEXP
TYPE_GLOBAL_SigmaEXP DEF::TranslateDLExportSig(const TYPE_AS_Name & mod_id,
                                               const TYPE_AS_DLExportSig & dlexportsig,
                                               const Generic & useslib)
{
  Map val (dlexportsig.get_val());
  Map fns (dlexportsig.get_fns());
  Map ops (dlexportsig.get_ops());

  MAP<TYPE_AS_Name,TYPE_SEM_DLFN> efn;
  MAP<TYPE_AS_Name,TYPE_SEM_DLOP> eop;
  MAP<TYPE_AS_Name,TYPE_SEM_VAL> gv;

  Set dom_fns (fns.Dom());
  Generic nm;
  for (bool bb = dom_fns.First(nm); bb; bb = dom_fns.Next(nm)) {
    theState().DLInsertClassLib(mod_id, useslib, nm);
    efn.Insert(nm, TYPE_SEM_DLFN().Init(fns[nm], TYPE_SEM_LOC().Init(nm), nm, mod_id));
  }

  Set dom_ops (ops.Dom());
  for (bool cc = dom_ops.First(nm); cc; cc = dom_ops.Next(nm)) {
    theState().DLInsertClassLib(mod_id, useslib, nm);
    eop.Insert(nm, TYPE_SEM_DLOP().Init(ops[nm], TYPE_SEM_LOC().Init(nm), nm, mod_id));
  }

  for (bool dd = val.First(nm); dd; dd = val.Next(nm)) {
    theState().DLInsertClassLib(mod_id, useslib, nm);
    gv.Insert(nm, mk_SEM_NUM(Real(0)));
  }
  return TYPE_GLOBAL_SigmaEXP().Init(efn, eop, gv);
}
#endif //VDMSL

