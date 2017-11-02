#include "evalmod.h"
#include "astaux.h"
#include "libman.h"
#include "evalaux.h"
#include "evaldef.h"
#include "evalclass.h"
#include "rt_errmsg.h"
#include "position.h"
#include "settings.h"

#ifdef VDMSL
// TranslateModule
// mkd_sig : AS`Module
// ==> GLOBAL`SigmaMO
TYPE_GLOBAL_SigmaMO MOD::TranslateModule(const TYPE_AS_Module & mod_sig)
{
  const TYPE_AS_Name & mod_id (mod_sig.GetRecord(pos_AS_Module_nm));
  const TYPE_AS_Interface & intf (mod_sig.GetRecord(pos_AS_Module_intf));
  const Generic & body (mod_sig.GetField(pos_AS_Module_defs));

  theCompiler().SetClMod( mod_id );
  theCompiler().ResetProgramTable( mod_id );

  const Generic & exp (intf.GetField (pos_AS_Interface_exp));
  MAP<TYPE_AS_Name,TYPE_AS_Name> ren (BuildRenameMap (intf.GetMap (pos_AS_Interface_imp)));

  if (! body.IsNil ())
  {
    // Set the current module name in the SemRecTable
    SemRec::SetCurrentModClass(mod_id);
    TYPE_GLOBAL_SigmaDEFS sigmadefs (DEF::TranslateDefinitions ( mod_id, body ));
    TYPE_GLOBAL_SigmaMO sigmamo;
    sigmamo.Init(sigmadefs.GetMap(pos_GLOBAL_SigmaDEFS_explfns),                  // explfns
                 sigmadefs.GetMap(pos_GLOBAL_SigmaDEFS_explops),                  // explops
                 sigmadefs.GetMap(pos_GLOBAL_SigmaDEFS_explpolys),                // explpolys
                 sigmadefs.GetMap(pos_GLOBAL_SigmaDEFS_tps),                      // tps
                 sigmadefs.GetMap(pos_GLOBAL_SigmaDEFS_recsel),                   // recsel
                 Nil(),                                                           // gv
                 sigmadefs.GetSequence(pos_GLOBAL_SigmaDEFS_gv_uinit),            // gv_init
                 SEQ<TYPE_AS_ValueDef>(),                                         // gv_inst
                 TYPE_GLOBAL_StateMap(),                                          // gs
                 sigmadefs.GetField(pos_GLOBAL_SigmaDEFS_gs_uinit),               // gs_init
                 sigmadefs.GetField(pos_GLOBAL_SigmaDEFS_stp_uid),                // stp_id
                 sigmadefs.GetField(pos_GLOBAL_SigmaDEFS_stateinv),               // stateinv
                 BuildExportSig(exp,
                                sigmadefs.GetMap(pos_GLOBAL_SigmaDEFS_explfns),
                                sigmadefs.GetMap(pos_GLOBAL_SigmaDEFS_explpolys),
                                ren),                                             // exp
                 Map(),                                                           // imp
                 ren);                                                            // ren
    return sigmamo;
  }
  else
  {
    TYPE_GLOBAL_SigmaMO sigmamo;
    sigmamo.Init(Map(),                     // explfns
                 Map(),                     // explops
                 Map(),                     // explpolys
                 Map(),                     // tps
                 Map(),                     // recsel
                 Nil(),                     // gv
                 Sequence(),                // gv_init
                 SEQ<TYPE_AS_ValueDef>(),   // gv_inst
                 TYPE_GLOBAL_StateMap(),    // gs
                 Nil(),                     // gs_init
                 Nil(),                     // stp_id
                 Nil(),                     // stateinv
                 exp,                       // exp
                 Map(),                     // imp
                 ren);                      // ren
    return sigmamo;
  }
}

// BuildRenameMap
// imp_m : map AS`Name to [AS`ImportSig]
// ==> map AS`Name to AS`Name
MAP<TYPE_AS_Name,TYPE_AS_Name> MOD::BuildRenameMap (const Map & imp_m)
{
  MAP<TYPE_AS_Name,TYPE_AS_Name> rename_m;
  Set dom_imp_m (imp_m.Dom());

  Generic id;
  for (bool bb = dom_imp_m.First (id); bb; bb = dom_imp_m.Next (id))
  {
    Generic imp_sig (imp_m[id]);
    if (! imp_sig.IsNil ()) {
      TYPE_AS_ImportSig isig (imp_sig);
      const Map & ren (isig.GetMap (pos_AS_ImportSig_ren));
      Set dom_ren (ren.Dom ());
      if (dom_ren.ImpIntersect (rename_m.Dom ()).IsEmpty ())
        rename_m.ImpOverride (MakeRenamings (id, ren));
      else {
        const TYPE_CI_ContextId & cid (isig.GetInt(pos_AS_ImportSig_cid));
        RTERR::InitError (L"BuildRenameMap", RTERR_EQUAL_RENAMINGS, Nil(), Nil(), cid, Sequence());
      }
    }
  }
  return rename_m;
}

// MakeRenamings
// mod_nm : AS`Name
// ren_m : map AS`Name to AS`Name
// ==> map AS`Name to AS`Name
MAP<TYPE_AS_Name,TYPE_AS_Name> MOD::MakeRenamings (const TYPE_AS_Name & mod_nm,
                                                   const MAP<TYPE_AS_Name,TYPE_AS_Name> & ren_m)
{
  MAP<TYPE_AS_Name, TYPE_AS_Name> res_m;
  TYPE_AS_Id mod_id (ASTAUX::GetFirstId(mod_nm));
  Set dom_ren_m (ren_m.Dom());
  Generic nm_key;
  for (bool bb = dom_ren_m.First (nm_key); bb; bb = dom_ren_m.Next (nm_key))
  {
    TYPE_AS_Name thisNm (ren_m[nm_key]);
    TYPE_AS_Ids ids (thisNm.GetSequence(pos_AS_Name_ids));
    ids.ImpPrepend(mod_id);
    thisNm.SetField(pos_AS_Name_ids, ids);
    res_m.Insert(nm_key, thisNm);
  }
  return (res_m);
}

// BuildExportSig
// exp_sig : [AS`ExportSig]
// defs : GLOBAL`SigmaDEFS
// ren : map AS`Name to AS`Name
// ==> [AS`ExportSig]
Generic MOD::BuildExportSig (const Generic & exp_sig, const Map & explfns, const Map & explpolys,
                             const MAP<TYPE_AS_Name,TYPE_AS_Name> & ren)
{
  if (exp_sig.IsNil ())
    return (exp_sig);

  const Map & fns (Record(exp_sig).GetMap(pos_AS_ExportSig_fns));
  const Map & ops (Record(exp_sig).GetMap(pos_AS_ExportSig_ops));
  const Map & tps (Record(exp_sig).GetMap(pos_AS_ExportSig_tps));

  Map res_fns (fns);

  SET<TYPE_AS_Name> dom_fns (fns.Dom());
  Generic name;
  for (bool bb = dom_fns.First (name); bb; bb = dom_fns.Next (name)) {
    TYPE_AS_Name new_name (ren.DomExists(name) ? ren[name] : (TYPE_AS_Name)name);
    if (HasFnPrePostCond (new_name, explfns, explpolys))
      res_fns.ImpOverride (GetPrePostFns (new_name, explfns, explpolys));
  }

  SET<TYPE_AS_Name> dom_ops (ops.Dom());
  for (bool cc = dom_ops.First (name); cc; cc = dom_ops.Next (name)) {
    TYPE_AS_Name new_name (ren.DomExists(name) ? ren[name] : (TYPE_AS_Name)name);
    if (HasOpPrePostCond (new_name, explfns))
      res_fns.ImpOverride (GetPrePostOps (new_name, explfns));
  }

  SET<TYPE_AS_Name> dom_tps (tps.Dom());
  for (bool dd = dom_tps.First (name); dd; dd = dom_tps.Next (name)) {
    TYPE_AS_Name new_name (ren.DomExists(name) ? ren[name] : (TYPE_AS_Name)name);
    if (HasTpInvariant (new_name, explfns))
      res_fns.ImpOverride (GetTpInvariant (new_name, explfns));
  }

  TYPE_AS_ExportSig new_sig (exp_sig);
  new_sig.SetField(pos_AS_ExportSig_fns, res_fns);
  return (new_sig);
}

// HasFnPrePostCond
// name : AS`Name
// efns : (map AS`Name to (SEM`CompExplFN * AS`Access))
// epfns : (map AS`Name to (SEM`ExplPOLY * AS`Access))
// ==> bool
bool MOD::HasFnPrePostCond (const TYPE_AS_Name & name, const Map & efns, const Map & epfns)
{
  if (AUX::IsRealName (name))
    return false;

  TYPE_AS_Name pre_name (AUX::PreName(name));
  TYPE_AS_Name post_name (AUX::PostName(name));

  return ((efns.DomExists (pre_name)) || (efns.DomExists (post_name)) ||
          (epfns.DomExists (pre_name)) || (epfns.DomExists (post_name)));
}

// GetPrePostFns
// name : AS`Name
// efns : (map AS`Name to SEM`CompExplFN
// epfns : (map AS`Name to SEM`ExplPOLY
// ==> map AS`Name to (AS`TypeVarList * AS`FnType)
Map MOD::GetPrePostFns (const TYPE_AS_Name & name, const Map & efns, const Map & epfns)
{
  TYPE_AS_Name pre_name (AUX::PreName(name));
  TYPE_AS_Name post_name (AUX::PostName(name));

  Map res_m;

  if (efns.DomExists (pre_name)) {
    const TYPE_SEM_CompExplFN & fnval (efns[pre_name]);
    const SEQ<TYPE_SEM_ExplFN> & fl (fnval.GetSequence(pos_SEM_CompExplFN_fl));
    res_m.Insert (pre_name, mk_(Sequence(), fl[1].GetRecord(pos_SEM_ExplFN_tp)));
  }
  else if (epfns.DomExists (pre_name)) {
    const TYPE_SEM_ExplPOLY & fnval (epfns[pre_name]);
    res_m.Insert (pre_name, mk_(fnval.GetSequence(pos_SEM_ExplPOLY_tpparms),
                                fnval.GetRecord(pos_SEM_ExplPOLY_tp)));
  }

  if (efns.DomExists (post_name)) {
    const TYPE_SEM_CompExplFN & fnval (efns[post_name]);
    const SEQ<TYPE_SEM_ExplFN> & fl (fnval.GetSequence(pos_SEM_CompExplFN_fl));
    res_m.Insert (post_name, mk_(Sequence(), fl[1].GetRecord(pos_SEM_ExplFN_tp)));
  }
  else if (epfns.DomExists (post_name)) {
    const TYPE_SEM_ExplPOLY & fnval (epfns[post_name]);
    res_m.Insert (post_name, mk_(fnval.GetSequence(pos_SEM_ExplPOLY_tpparms),
                                 fnval.GetRecord(pos_SEM_ExplPOLY_tp)));
  }
  return (res_m);
}

// HasOpPrePostCond
// name : AS`Name
// efns : map AS`Name to SEM`CompExplFN
// ==> bool
bool MOD::HasOpPrePostCond (const TYPE_AS_Name & name, const Map & efns)
{
  if (AUX::IsRealName (name))
    return false;

  TYPE_AS_Name pre_nm (AUX::PreName(name));
  TYPE_AS_Name post_nm (AUX::PostName(name));

  return (efns.DomExists (pre_nm) || efns.DomExists (post_nm));
}

// GetPrePostOps
// name : AS`Name
// efn : map AS`Name to SEM`CompExplFN
// ==> map AS`Name to (AS`TypeVarList * AS`FnType)
Map MOD::GetPrePostOps (const TYPE_AS_Name & name, const Map & efn)
{
  TYPE_AS_Name pre_name (AUX::PreName(name));
  TYPE_AS_Name post_name (AUX::PostName(name));

  Map res_m;

  if (efn.DomExists (pre_name))
  {
    const TYPE_SEM_CompExplFN & fnval (efn[pre_name]);
    const SEQ<TYPE_SEM_ExplFN> & fl (fnval.GetSequence(pos_SEM_CompExplFN_fl));
    res_m.Insert (pre_name, mk_(Sequence(), fl[1].GetRecord(pos_SEM_ExplFN_tp)));
  }

  if (efn.DomExists (post_name))
  {
    const TYPE_SEM_CompExplFN & fnval (efn[post_name]);
    const SEQ<TYPE_SEM_ExplFN> & fl (fnval.GetSequence(pos_SEM_CompExplFN_fl));
    res_m.Insert (post_name, mk_(Sequence(), fl[1].GetRecord(pos_SEM_ExplFN_tp)));
  }
  return res_m;
}

// HasTpInvariant
// name : AS`Name
// efn : map AS`Name to SEM`CompExplFN
// ==> bool
bool MOD::HasTpInvariant (const TYPE_AS_Name & name, const Map & efns)
{
  if (AUX::IsRealName (name))
    return false;

  return efns.DomExists (AUX::InvName (name));
}

// GetTpInvariant
// name : AS`Name
// efn : map AS`Name to SEM`CompExplFN
// ==> map AS`Name to (AS`TypeVarList * AS`FnType)
Map MOD::GetTpInvariant (const TYPE_AS_Name & name, const Map & efn)
{
  TYPE_AS_Name inv_name (AUX::InvName(name));

  Map res_m;

  const TYPE_SEM_CompExplFN & fnval (efn[inv_name]);
  const SEQ<TYPE_SEM_ExplFN> & fl (fnval.GetSequence(pos_SEM_CompExplFN_fl));
  res_m.Insert (inv_name, mk_(Sequence(), fl[1].GetRecord(pos_SEM_ExplFN_tp)));

  return (res_m);
}

// Note that this operation is identical to InitGV in the CLASS module!!!
// InitGV
// val_l : seq of AS`ValueDef
// ==> GLOBAL`ValueMap
TYPE_GLOBAL_ValueMap MOD::InitGV(const SEQ<TYPE_AS_ValueDef> & val_l, const TYPE_AS_Name & nm)
{
  return CLASS::InitGV(val_l, nm);
}

// InitGS
// sd_ : [AS`StateDef]
// ==> GLOBAL`StateMap
TYPE_GLOBAL_StateMap MOD::InitGS (const Generic & sd_)
{
  if (sd_.IsNil ())
    return TYPE_GLOBAL_StateMap ();
  else
  {
    TYPE_AS_StateDef sd (sd_);
    const TYPE_AS_CompositeType & comp_t (sd.GetRecord(pos_AS_StateDef_tp));
    const TYPE_AS_Name & name (comp_t.GetRecord(pos_AS_CompositeType_name));
    const SEQ<TYPE_AS_Field> & fields_lv (comp_t.GetSequence(pos_AS_CompositeType_fields));

    if (sd.GetField(pos_AS_StateDef_Init).IsNil ())
    {
      TYPE_GLOBAL_StateMap sm;
      size_t maxi = fields_lv.Length ();
      for (size_t i = 1; i <= maxi; i++)
      {
        const TYPE_AS_Field & fld (fields_lv[i]);
        sm.Insert (fld.GetField(pos_AS_Field_sel),
                   TYPE_GLOBAL_State().Init(sem_undef, fld.GetRecord(pos_AS_Field_type)));
      }
      return sm;
    }
    else
    {
      const TYPE_AS_StateInit & inits (sd.GetRecord(pos_AS_StateDef_Init));
      const TYPE_AS_Pattern & initp (inits.GetRecord(pos_AS_StateInit_pat));
      const TYPE_AS_Expr & inite    (inits.GetRecord(pos_AS_StateInit_expr));
      const TYPE_CI_ContextId & cid (inits.GetInt   (pos_AS_StateInit_cid));

      // state initialization can be done in two ways:
      //
      //    1.   init s == s = mk_State(...)
      //         where s is a simple pattern id,
      //
      //    2.   init mk_State(...) == mk_State(...)
      //         where the patterns in the first mk_State are only simple
      //         pattern id's
      //
      // Both ways will be supported.

      switch(inite.GetTag()) {
        case TAG_TYPE_AS_BinaryExpr: {
          // init s == s = mk_sigma(...)
          if (inite.GetInt(pos_AS_BinaryExpr_opr) != Int(EQ))
          {
            // no equal expression
            RTERR::InitError (L"InitGS", RTERR_ILL_STATE_INIT_PAT, Nil(), Nil(), cid, Sequence());
            return TYPE_GLOBAL_StateMap ();
          }
  
          const TYPE_AS_Expr & left (inite.GetRecord(pos_AS_BinaryExpr_left));
  
          if(!left.Is(TAG_TYPE_AS_Name))
          {
            RTERR::InitError (L"InitGS", RTERR_ILL_STATE_INIT_PAT, Nil(), Nil(), ASTAUX::GetCid(left), Sequence());
            return TYPE_GLOBAL_StateMap ();
          }
  
          if (!(initp.Is(TAG_TYPE_AS_PatternName) && !initp.GetField(pos_AS_PatternName_nm).IsNil()))
          {
            RTERR::InitError (L"InitGS", RTERR_ILL_STATE_INIT_PAT, Nil(), Nil(), ASTAUX::GetCid(initp), Sequence());
            return TYPE_GLOBAL_StateMap ();
          }

          if (!AUX::EqualNames(initp.GetRecord(pos_AS_PatternName_nm), left))
          {
            RTERR::InitError (L"InitGS", RTERR_ILL_STATE_INIT_PAT, Nil(), Nil(), ASTAUX::GetCid(initp), Sequence());
            return TYPE_GLOBAL_StateMap ();
          }
          break;
        } // end of TAG_TYPE_AS_BinaryExpr
/*
        case TAG_TYPE_AS_RecordConstructorExpr: {
          // init mk_sigma(...) == mk_sigma(...)
          if (! initp.Is (TAG_TYPE_AS_RecordPattern))
          {
            RTERR::InitError (L"InitGS", RTERR_REC_PAT_EXPECTED, Nil(), Nil(), ASTAUX::GetCid(initp), Sequence());
            return TYPE_GLOBAL_StateMap ();
          }

          if (! IsAllPatternName (initp))
          {
            RTERR::InitError (L"InitGS", RTERR_ALL_PATID_EXPECTED, Nil(), Nil(), ASTAUX::GetCid(initp), Sequence());
            return TYPE_GLOBAL_StateMap ();
          }
          break;
        } // end of TAG_TYPE_AS_RecordConstructorExpr
*/
        default: {
          RTERR::InitError (L"InitGS", RTERR_ILLEGAL_STATE_INIT, Nil(), Nil(), ASTAUX::GetCid(inite), Sequence());
          return TYPE_GLOBAL_StateMap ();
        }
      }

//
      TYPE_STKM_SubProgram pre_sp;
      TYPE_AS_Expr expr;
      switch(inite.GetTag()) {
        case TAG_TYPE_AS_BinaryExpr: {
          const TYPE_AS_Expr & left (inite.GetRecord(pos_AS_BinaryExpr_left));
          const TYPE_AS_Expr & right (inite.GetRecord(pos_AS_BinaryExpr_right));
          pre_sp.ImpConc(theCompiler().SetContext(cid, false));
          pre_sp.ImpConc(theCompiler().SetContext(ASTAUX::GetCid(left), false));
          expr = right;
          break;
        }
        case TAG_TYPE_AS_RecordConstructorExpr: {
          expr = inite;
          break;
        }
      }

      Tuple eac (theStackMachine().EvalAuxCmd(expr, pre_sp, TYPE_STKM_SubProgram(),
                                              SEQ<Char>(L"Init of Global State")));

      if (!eac.GetRecord(1).Is(TAG_TYPE_STKM_Success))
      {
        RTERR::InitError(L"InitGS", RTERR_INTERNAL_ERROR, Nil(), Nil(), ASTAUX::GetCid(inite), Sequence());
        return TYPE_GLOBAL_StateMap ();
      }

      const TYPE_SEM_VAL & initv (eac.GetField(2));

      if (initv.Is(TAG_TYPE_DYNSEM_SEM_REC))
      {
        const TYPE_AS_Name & initname (initv.GetRecord(pos_DYNSEM_SEM_SemRecord_tag));
        SEQ<TYPE_SEM_VAL> initfields_lv (initv.GetRecord(pos_DYNSEM_SEM_SemRecord_value).GetFields());
  
        TYPE_AS_Name name_q (AUX::UniqueTagName (name));
        if (AUX::EqualNames (name_q, initname) && (fields_lv.Length () == initfields_lv.Length ()))
        {
          TYPE_GLOBAL_StateMap res;
          size_t len = fields_lv.Length ();
          for (size_t i = 1; i <= len; i++)
          {
            const TYPE_AS_Field & fld (fields_lv[i]);
            res.Insert (fld.GetField(pos_AS_Field_sel),
                          TYPE_GLOBAL_State().Init(initfields_lv[i], fld.GetRecord(pos_AS_Field_type)));
          }

          if (Settings.DTC())
          {
            SET<TYPE_AS_Name> dom_res (res.Dom ());
            Generic id;
            for (bool bb = dom_res.First (id); bb; bb = dom_res.Next (id))
            {
              const TYPE_GLOBAL_State & st (res[id]);
              if( !theState().SubType(st.GetRecord(pos_GLOBAL_State_val), st.GetRecord(pos_GLOBAL_State_tp)) )
              {
                RTERR::InitError (L"InitGS", RTERR_STATE_INVARIANT_ERROR,
                                  inite, st.get_val(), ASTAUX::GetCid(st.get_tp()), Sequence());
                return TYPE_GLOBAL_StateMap();
              }
            }
          }
          return res;
        }
        else
          RTERR::InitError (L"InitGS", RTERR_ILLEGAL_STATE_INIT, Nil(), Nil(), ASTAUX::GetCid(name_q), Sequence());
      }
      else
        RTERR::InitError (L"InitGS", RTERR_REC_EXPECTED, Nil(), Nil(), ASTAUX::GetCid(inite), Sequence());
//
    }
  }
  return TYPE_GLOBAL_StateMap(); // Dummy return.
}

// IsAllPatternName
// pat_p : AS`PatternBind
// ==> bool
bool MOD::IsAllPatternName (const TYPE_AS_PatternBind & pat_p)
{
  switch(pat_p.GetTag()) {
    case TAG_TYPE_AS_PatternName: {
      return !pat_p.GetField(pos_AS_PatternName_nm).IsNil ();
    }
    case TAG_TYPE_AS_MatchVal:
      return false;
    case TAG_TYPE_AS_SetEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & pat_lp (pat_p.GetSequence(pos_AS_SetEnumPattern_Elems));
      bool forall = true;
      size_t len_pat_lp = pat_lp.Length();
      for (size_t idx = 1; (idx <= len_pat_lp) && forall; idx++)
        forall = IsAllPatternName (pat_lp[idx]);
      return forall;
    }
    case TAG_TYPE_AS_SeqEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & pat_lp (pat_p.GetSequence(pos_AS_SeqEnumPattern_els));
      bool forall = true;
      size_t len_pat_lp = pat_lp.Length();
      for (size_t idx = 1; (idx <= len_pat_lp) && forall; idx++)
        forall = IsAllPatternName (pat_lp[idx]);
      return forall;
    }
    case TAG_TYPE_AS_MapEnumPattern: {
      const SEQ<TYPE_AS_MapletPattern> & mls (pat_p.GetSequence(pos_AS_MapEnumPattern_mls));
      bool forall = true;
      size_t len_mls = mls.Length();
      for (size_t idx = 1; (idx <= len_mls) && forall; idx++) {
        const TYPE_AS_MapletPattern & mp(mls[idx]);
        forall = (IsAllPatternName (mp.GetRecord(pos_AS_MapletPattern_dp)) &&
                  IsAllPatternName (mp.GetRecord(pos_AS_MapletPattern_rp)));
      }
      return forall;
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      return (IsAllPatternName (pat_p.GetRecord(pos_AS_SetUnionPattern_lp)) &&
              IsAllPatternName (pat_p.GetRecord(pos_AS_SetUnionPattern_rp)));
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      return (IsAllPatternName (pat_p.GetRecord(pos_AS_SeqConcPattern_lp)) &&
              IsAllPatternName (pat_p.GetRecord(pos_AS_SeqConcPattern_rp)));
    }
    case TAG_TYPE_AS_MapMergePattern: {
      return (IsAllPatternName (pat_p.GetRecord(pos_AS_MapMergePattern_lp)) &&
              IsAllPatternName (pat_p.GetRecord(pos_AS_MapMergePattern_rp)));
    }
    case TAG_TYPE_AS_TuplePattern: {
      const SEQ<TYPE_AS_Pattern> & pat_lp (pat_p.GetSequence(pos_AS_TuplePattern_fields));
      bool forall = true;
      size_t len_pat_lp = pat_lp.Length();
      for (size_t idx = 1; (idx <= len_pat_lp) && forall; idx++) {
        forall = IsAllPatternName (pat_lp[idx]);
      }
      return forall;
    }
    case TAG_TYPE_AS_RecordPattern: {
      const SEQ<TYPE_AS_Pattern> & pat_lp (pat_p.GetSequence(pos_AS_RecordPattern_fields));
      bool forall = true;
      size_t len_pat_lp = pat_lp.Length();
      for (size_t idx = 1; (idx <= len_pat_lp) && forall; idx++) {
        forall = IsAllPatternName (pat_lp[idx]);
      }
      return forall;
    }
    case TAG_TYPE_AS_SetBind:
    case TAG_TYPE_AS_TypeBind:
      return false;
    default: {
      RTERR::Error (L"IsAllPatternName", RTERR_PATTERN_UNKNOWN, Nil(), Nil(), Sequence());
    }
  }
  return false; //to avoid warnings from gcc
}

// TranslateDLModule
// dlmod : AS`DLModule
// ==> GLOBAL`SigmaIMO
TYPE_GLOBAL_SigmaIMO MOD::TranslateDLModule(const TYPE_AS_DLModule & dlmod)
{
  const TYPE_AS_Name & mod_id (dlmod.GetRecord(pos_AS_DLModule_nm));
  const TYPE_AS_DLInterface & intf (dlmod.GetRecord(pos_AS_DLModule_intf));
  const TYPE_AS_TextLit & useslib (dlmod.GetRecord(pos_AS_DLModule_useslib));

  const TYPE_AS_DLExportSig & exp (intf.GetRecord(pos_AS_DLInterface_exp));

  POSITION::SetPosition (useslib);

  TYPE_GLOBAL_SigmaEXP sigmaexp (DEF::TranslateDLExportSig(mod_id, exp, useslib));

  TYPE_GLOBAL_SigmaIMO sigmaimo;
  sigmaimo.Init(sigmaexp.GetMap(pos_GLOBAL_SigmaEXP_extfns),
                sigmaexp.GetMap(pos_GLOBAL_SigmaEXP_extops),
                sigmaexp.GetField(pos_GLOBAL_SigmaEXP_extv),
                exp,
                Map(),
                Map(),
                useslib);
  return sigmaimo;
}

// TransStaticRef
// mods : map AS`name to AS`SigmaMO
// -> map AS`Name to set of AS`Name
Map MOD::TransStaticRef(const Map & mods )
{
  Map sdep;
  SET<TYPE_AS_Name> exp;
  Set dom_mods (mods.Dom());
  Generic name;
  for (bool bb = dom_mods.First(name); bb; bb = dom_mods.Next(name))
  {
    TYPE_GLOBAL_SigmaMO sigmamo (mods[name]);

    SET<TYPE_AS_Name> modnms;
    // search module names in initialization of values.
    SEQ<TYPE_AS_ValueDef> vls (sigmamo.GetSequence(pos_GLOBAL_SigmaMO_gv_uinit));
    size_t len_vls = vls.Length();
    for (size_t i = 1; i <= len_vls; i++)
    {
      modnms.ImpUnion(AUX::ClModNmInExpr(vls[i].GetRecord(pos_AS_ValueDef_val)));
    }

    // search module names in initialization of state and
    // state invaliants.
    const Generic & gs_init (sigmamo.GetField(pos_GLOBAL_SigmaMO_gs_uinit));
    if (!gs_init.IsNil())
    {
      TYPE_AS_StateDef sd (gs_init);
      const Generic & Inv (sd.GetField(pos_AS_StateDef_Inv));
      const Generic & Init (sd.GetField(pos_AS_StateDef_Init));
      if (!Inv.IsNil())
        modnms.ImpUnion(AUX::ClModNmInExpr(Inv));
      if (!Init.IsNil())
        modnms.ImpUnion(AUX::ClModNmInExpr(Init));
    }

    // check rename
    const MAP<TYPE_AS_Name, TYPE_AS_Name> & ren (sigmamo.GetMap(pos_GLOBAL_SigmaMO_ren));
    SET<TYPE_AS_Name> rmodnms;
    Generic nm;
    for (bool dd = modnms.First(nm); dd; dd = modnms.Next(nm))
    {
      if (ren.DomExists(nm))
        rmodnms.Insert(ren[nm]);
      else
        rmodnms.Insert(nm);
    }

    // current module name should be removed.
    rmodnms.ImpDiff(mk_set(name));

    // static 
    sdep.ImpModify( name, rmodnms );

    // 
    if (rmodnms.IsEmpty())
      exp.Insert( name );
  }

  return CLASS::ExpandStaticRef(sdep, exp);
}
#endif //VDMSL

