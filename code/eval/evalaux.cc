#include "metaiv.h"
#include "astaux.h"
#include "stackeval.h"
#include "rt_errmsg.h"
#include "libman.h"
#include "evalaux.h"
#include "asquotes.h"
#include "tbutils.h"
#include "intconvquotes.h"

SET<TYPE_SEM_BlkEnv> AUX::eset;

// CombineBlkEnv
// blk1 : SEM`BlkEnv
// vlk2 : SEM`BlkEnv
// ==> SEM`BlkEnv
TYPE_SEM_BlkEnv AUX::CombineBlkEnv (const TYPE_SEM_BlkEnv & blk1, const TYPE_SEM_BlkEnv & blk2)
{
  const TYPE_SEM_Permission & permission (blk1.GetField(pos_SEM_BlkEnv_perm));
  if (permission != blk2.GetField(pos_SEM_BlkEnv_perm)) {
    RTERR::Error(L"CombineBlkEnv", RTERR_INTERNAL_ERROR, Nil(), Nil(), Sequence());
    return blk1;
  }

  MAP<TYPE_AS_Name,TYPE_SEM_ValTp> id_m (blk1.GetMap(pos_SEM_BlkEnv_id_um));
  id_m.ImpOverride(blk2.GetMap(pos_SEM_BlkEnv_id_um));
  return TYPE_SEM_BlkEnv().Init(id_m, permission);

//  return DistribCombineBlkEnv(mk_set(blk1, blk2)).GetElem();
}

// DistribCombineBlkEnv
// blkenv_s : set of SEM`BlkEnv
// ==> set of SEM`BlkEnv
SET<TYPE_SEM_BlkEnv> AUX::DistribCombineBlkEnv(const SET<TYPE_SEM_BlkEnv> & blkenv_s)
{
  switch (blkenv_s.Card()) {
    case 0: {
      return eset;
    }
    case 1: {
      return blkenv_s;
    }
    default: {
      SET<TYPE_SEM_BlkEnv> env_s (blkenv_s);
      TYPE_SEM_BlkEnv some_blkenv (env_s.GetElem());
      env_s.RemElem(some_blkenv);

      MAP<TYPE_AS_Name,TYPE_SEM_ValTp> result_m (some_blkenv.GetMap(pos_SEM_BlkEnv_id_um));
      const TYPE_SEM_Permission & permission (some_blkenv.GetField(pos_SEM_BlkEnv_perm));

      Generic blkenv;
      for (bool bb = env_s.First(blkenv); bb; bb = env_s.Next(blkenv)) {
        const MAP<TYPE_AS_Name,TYPE_SEM_ValTp> & id_m (Record(blkenv).GetMap(pos_SEM_BlkEnv_id_um));
        if (!result_m.IsCompatible(id_m)) {
          return eset;
        }
        result_m.ImpOverride(id_m);
      }
      return SET<TYPE_SEM_BlkEnv>().Insert(TYPE_SEM_BlkEnv().Init(result_m, permission));
    }
  }
}

// MkBlkEnv
// id : AS`Name
// val_v : SEM`VAL
// tp : [AS`Type]
// perm : SEM`Permission
// ==> SEM`BlkEnv
TYPE_SEM_BlkEnv AUX::MkBlkEnv (const TYPE_AS_Name & id, const TYPE_SEM_VAL & val_v,
                               const Generic & tp,
                               const TYPE_SEM_Permission & perm )
{
  return TYPE_SEM_BlkEnv().Init(Map().Insert(id, TYPE_SEM_ValTp().Init(val_v, tp)), perm);
}

// MkBlkEnvFromIdMap
// id_m : map AS`Name to SEM`VAL
// perm : SEM`Permission
// -> SEM`BlkEnv;
TYPE_SEM_BlkEnv AUX::MkBlkEnvFromIdMap (const MAP<TYPE_AS_Name, TYPE_SEM_VAL> & id_m,
                                        const TYPE_SEM_Permission & perm )
{
  Map m;
  Set dom_id_m (id_m.Dom());
  Generic id;
  for (bool bb = dom_id_m.First (id); bb; bb = dom_id_m.Next (id)) {
    m.ImpModify(id, TYPE_SEM_ValTp().Init(id_m[id], Nil()));
  }
  return TYPE_SEM_BlkEnv().Init(m, sem_read_only);
}

// MkEmptyBlkEnv
// perm : SEM`Permission
// ==> SEM`BlkEnv
TYPE_SEM_BlkEnv AUX::MkEmptyBlkEnv (const TYPE_SEM_Permission & perm)
{
  return TYPE_SEM_BlkEnv().Init(Map(), perm);
}

#ifdef VDMSL
// IsTypeStructExported
// name : AS`Name
// ==> bool
bool AUX::IsTypeStructExported (const TYPE_AS_Name & name)
{
  // We do not check for this condition since the type checker flags
  // this anyway and it slows down everything.
  return true; // This has been ok with PGL.
}

// IsConstructExported
// loc_name : AS`Name
// mod_name : AS`Name
// ==> bool
bool AUX::IsConstructExported (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name)
{
  const TYPE_GLOBAL_SigmaMO & sigmamo (theState().GetModule(mod_name));
  const Generic & exp_sig_g (sigmamo.GetField(pos_GLOBAL_SigmaMO_exp));

  if (exp_sig_g.IsNil()) {
    return true;
  }
  else {
    TYPE_AS_ExportSig exp_sig (exp_sig_g);

    return (exp_sig.GetMap(pos_AS_ExportSig_val).DomExists (loc_name) ||
            exp_sig.GetMap(pos_AS_ExportSig_fns).DomExists (loc_name) ||
            exp_sig.GetMap(pos_AS_ExportSig_ops).DomExists (loc_name));
  }
}

// UniqueTagName
// tname : AS`Name
// ==> AS`Name
TYPE_AS_Name AUX::UniqueTagName (const TYPE_AS_Name & tname)
{
  if (!IsRealName(tname)) {
    const TYPE_AS_Name & curmod (theStackMachine().CurrentModule ());
    
    if (!curmod.GetSequence(pos_AS_Name_ids).IsEmpty()) {
      return ConstructDoubleName(curmod, tname);
    }
  }
  return tname;
}

// ExtractTagName
// name : AS`Name
// ==> [AS`Name] * bool
Tuple AUX::ExtractTagName(const TYPE_AS_Name & name, const SET<TYPE_AS_Name> &)
{
/*
  if (nm_s.InSet(name)) {
    RTERR::Error(L"ExtractTagName", RTERR_TAG_CYCLIC, Nil(), Nil(), Sequence());
    return mk_(Nil(), Bool(false));
  }
  TYPE_AS_Name the_tag (UniqueTagName(name));
  Tuple t (LookUpRename(the_tag));
  if (t.GetBoolValue(1))
    return ExtractTagName(t.GetRecord(2), nm_s.Union(mk_set(the_tag)));
  else {
    if (IsDefinedInModule (the_tag) && IsTypeStructExported(the_tag))
      return mk_(the_tag, Bool(true));
    else
      return mk_(Nil(), Bool(false));
  }
*/
  Tuple t (LookUpRename(UniqueTagName(name)));
  TYPE_AS_Name the_tag (t.GetRecord(2));
  if (IsDefinedInModule (the_tag) && IsTypeStructExported(the_tag))
    return mk_(the_tag, Bool(true));
  else
    return mk_(Nil(), Bool(false));
}
#endif //VDMSL

#ifdef VDMPP
// ExtractTagName
// name : AS`Name
// ==> [AS`Name] * bool
//Tuple AUX::ExtractTagName(const TYPE_AS_Name & name)
Tuple AUX::ExtractTagName(const TYPE_AS_Name & name, const SET<TYPE_AS_Name> & nm_s)
{
  if (nm_s.InSet(name)) {
    RTERR::Error(L"ExtractTagName", RTERR_TAG_CYCLIC, Nil(), Nil(), Sequence());
    return mk_(Nil(), Bool(false));
  }

  // mk_AS`Name(l^[id], -) = name in
  bool l_empty = (name.GetSequence(pos_AS_Name_ids).Length() == 1);

  TYPE_AS_Name thename (ExtractName(name));

  Set allsupers;
  if (l_empty) {
    if (!theStackMachine().HasCurCl()) {
      RTERR::Error(L"ExtractTagName", RTERR_TAG_UNKNOWN, Nil(), Nil(), Sequence());
      return mk_(Nil(), Bool(false));
    }
    else {
      const TYPE_AS_Name & clnm (theStackMachine().GetCurCl()); // HasCurCl() == true

/*
      MAP<TYPE_AS_Name,TYPE_AS_TypeDef> tdm (theState().GetLocalTps(clnm));

      if (tdm.DomExists(thename)) {
        const TYPE_AS_TypeDef & td (tdm[thename]);
        const TYPE_AS_Type & shape (td.GetRecord(pos_AS_TypeDef_shape));
        if (shape.Is(TAG_TYPE_AS_TypeName))
          return ExtractTagName(shape.GetRecord(pos_AS_TypeName_name), Set(nm_s).Insert(name));
      }
*/
      if (theState().GetRecSel(clnm).DomExists(thename))
        return mk_(ConstructDoubleName(clnm, name), Bool (true));

      allsupers = theState().GetAllSupers(clnm); // HasCurCl() == true
    }
  }
  else {
    TYPE_AS_Name classname (ASTAUX::GetFirstName(name));
/*
    if (theState().IsAClass(classname))
    {
      MAP<TYPE_AS_Name,TYPE_AS_TypeDef> tdm (theState().GetLocalTps(classname));
      if (tdm.DomExists(thename))
      {
        const TYPE_AS_TypeDef & td (tdm[thename]);
        const TYPE_AS_Type & shape (td.GetRecord(pos_AS_TypeDef_shape));
        if (shape.Is(TAG_TYPE_AS_TypeName))
        {
          const TYPE_AS_Name & tn (shape.GetRecord(pos_AS_TypeName_name));
          if (tn.GetSequence(pos_AS_Name_ids).Length() == 2)
          {
            TYPE_AS_Name cls (ASTAUX::GetFirstName(tn));
            TYPE_AS_Name tnm (ASTAUX::GetSecondName(tn));
            
            MAP<TYPE_AS_Name,TYPE_AS_TypeDef> ntdm (theState().GetLocalTps(cls));
            if (ntdm.DomExists(tnm))
            {
              const TYPE_AS_TypeDef & ntd (ntdm[tnm]);
              const TYPE_AS_Access & acc (ntd.GetField(pos_AS_TypeDef_access));
              if (theState().AccessOk(acc, classname, cls))
              {
                return ExtractTagName(tn, Set(nm_s).Insert(name));
              }
              else
                return mk_(Nil(), Bool(false));
            }
          }
        }
      }
    }
*/
    allsupers = theState().GetAllSupers(classname);
    allsupers.Insert(classname);
  }

  const TYPE_AS_Name & clnm (theStackMachine().GetCurCl()); // HasCurCl() == true
//  Set allsupers;
//  if (l_empty)
//    allsupers = theState().GetAllSupers(clnm); // HasCurCl() == true
//  else {
//    TYPE_AS_Name classname (ASTAUX::GetFirstName(name));
//    allsupers = theState().GetAllSupers(classname);
//    allsupers.Insert(classname);
//  }

  MAP<TYPE_AS_Name,TYPE_AS_TypeDef> ltp;
  MAP<TYPE_AS_Name, Tuple> rs_tps;
  Generic clname;
  for (bool bb = allsupers.First(clname); bb; bb = allsupers.Next(clname)) {
    Map recsel (theState().GetRecSel(clname)); // map AS`Name to GLOBAL`RecSel
    if (recsel.DomExists(thename))
      rs_tps.ImpModify(clname, recsel[thename]); //

    Map ltps (theState().GetLocalTps(clname));
    if (ltps.DomExists(name))
      ltp.ImpModify(clname, ltps[name]); //
  }

  // in cases dom recsels:
  SET<TYPE_AS_Name> dom_rs_tps (rs_tps.Dom());
  switch(dom_rs_tps.Card()) {
    case 0: {
      if (ltp.Size() == 1)
      {
        TYPE_AS_TypeDef td (ltp.Rng().GetElem());
        TYPE_AS_Type tp (td.GetRecord(pos_AS_TypeDef_shape));
        if (tp.Is(TAG_TYPE_AS_TypeName)) {
          return ExtractTagName(tp.GetRecord(pos_AS_TypeName_name), Set(nm_s).Insert(name));
        }
      }
      return mk_(Nil(), Bool(false));
    }
    case 1: {
      TYPE_AS_Name cl (dom_rs_tps.GetElem());
      Tuple t (rs_tps[cl]);
      //if (theState().AccessOk(t.GetField(2), clnm, cl)) // AS`Access
      if (theState().AccessOk(t.GetField(4), clnm, cl)) // AS`Access
        return mk_(ConstructDoubleName(cl, name), Bool(true));
      else
        return mk_(Nil(), Bool(false));
    }
    default: {
      //TYPE_AS_Name classname (ASTAUX::GetFirstName(name));
      //if (!l_empty && dom_rs_tps.InSet(clnm)) {
      if (!l_empty && dom_rs_tps.InSet(ASTAUX::GetFirstName(name))) {
        return mk_(name, Bool(true));
      }
      else {
        SET<TYPE_AS_Name> rem_priv;
        Generic nm;
        for (bool cc = dom_rs_tps.First(nm); cc; cc = dom_rs_tps.Next(nm)) {
          Tuple t (rs_tps[nm]);
          //const TYPE_AS_Access & acc (t.GetField(2));
          const TYPE_AS_Access & acc (t.GetField(4));
          if ((acc != Int(PRIVATE_AS)) && (acc != Int(NOT_INITIALISED_AS))) {
            rem_priv.Insert(nm);
          }
        }
        Tuple eoc (theState().ExistsOneChild(rem_priv, dom_rs_tps));
        if (eoc.GetBool(1)) {
          const TYPE_AS_Name & child (eoc.GetRecord(2));
          Tuple t (rs_tps[child]);
          //if (theState().AccessOk(t.GetField(2), clnm, child)) 
          if (theState().AccessOk(t.GetField(4), clnm, child)) 
            return mk_(ConstructDoubleName(child, name), Bool(true));
          else
            return mk_(Nil(), Bool(false));
        }
        else
          return mk_(Nil(), Bool(false));
      }
    }
  }
}
#endif // VDMPP

#ifdef VDMSL
// Note: The IsTypeDef replaces to the IsTypeDef and LookUpTypeDef.
// LookUpTypeDef
// name : AS`Name
// ==> AS`TypeDef

// IsTypeDef
// name : AS`Name
// ==> bool * [GLOBAL`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * [AS`Name]
Tuple AUX::IsTypeDef (const TYPE_AS_Name & name)
{
  Tuple name_t (RenameExtractModule(name));
  const TYPE_AS_Name & mod_name (name_t.GetRecord(1));
  const TYPE_AS_Name & rec_name (name_t.GetRecord(2));

  const TYPE_GLOBAL_SigmaMO & sigmamo (theState().GetModule(mod_name));

  const Map & tps (sigmamo.GetMap(pos_GLOBAL_SigmaMO_tps));
  if (tps.DomExists (rec_name)) {
    TYPE_AS_TypeDef td (tps[rec_name]);
    return mk_(Bool(true), td.GetRecord(pos_AS_TypeDef_shape),
                           td.GetField(pos_AS_TypeDef_Inv),
                           td.GetField(pos_AS_TypeDef_Eq),
                           td.GetField(pos_AS_TypeDef_Ord),
                           mod_name);
  }
  else {
    return mk_(Bool(false), Nil(), Nil(), Nil(), Nil(), Nil());
  }
}
#endif //VDMSL

#ifdef VDMPP
// IsTypeDef
// name : AS`Name
// ==> bool * [GLOBAL`Type] * [AS`Invariant] * [AS`Equal] * [AS`Order] * [AS`Name] * [AS`Access]
Tuple AUX::IsTypeDef(const TYPE_AS_Name & name)
{
  if (theState().IsAClass(name)) {
    TYPE_GLOBAL_ObjRefType type;
    type.SetField(pos_GLOBAL_ObjRefType_nm, name);
    
    return mk_(Bool(true), type, Nil(), Nil(), Nil(), name, Nil());
  }

  bool l_empty = (name.GetSequence(pos_AS_Name_ids).Length() == 1);

  if (l_empty && ! theStackMachine().HasCurCl()) {
    RTERR::Error(L"IsTypeDef", RTERR_TYPE_UNKNOWN, Nil(), Nil(), Sequence());
    return mk_(Bool(false), Nil(), Nil(), Nil(), Nil(), Nil(), Nil());
  }

  TYPE_AS_Name thename (ExtractName(name));
  const TYPE_AS_Name & clnm (theStackMachine().GetCurCl()); // HacCurCl() == true
  const TYPE_AS_Name & origcl (theStackMachine().GetOrigCl());

  if (l_empty) { // Current Class
    Map tps (theState().GetLocalTps(clnm));

    if (tps.DomExists(thename)) {
      TYPE_AS_TypeDef typedef_r (ExtComp(tps[thename], clnm));
      const TYPE_AS_Access & access (typedef_r.GetField(pos_AS_TypeDef_access));
      Bool scopeok (theState().AccessOk(access, origcl, clnm));

      return mk_(scopeok,
                 typedef_r.GetRecord(pos_AS_TypeDef_shape),
                 typedef_r.GetField(pos_AS_TypeDef_Inv),
                 typedef_r.GetField(pos_AS_TypeDef_Eq),
                 typedef_r.GetField(pos_AS_TypeDef_Ord),
                 clnm,
                 access);
    }
  }

  TYPE_AS_Name classname;
  SET<TYPE_AS_Name> allsupers;
  if (l_empty) {
    classname = theStackMachine().GetCurObjName();
    allsupers = theState().GetAllSupers(clnm);
  }
  else {
    classname = ASTAUX::GetFirstName(name);
    allsupers = theState().GetAllSupers(classname);
    allsupers.Insert(classname);
  }

  MAP<TYPE_AS_Name, Map> spcl_tps;
  Generic clname;
  for (bool bb = allsupers.First(clname); bb; bb = allsupers.Next(clname)) {
    MAP<TYPE_AS_Name,TYPE_AS_TypeDef> glt (theState().GetLocalTps(clname));
    if (glt.DomExists(thename))
      spcl_tps.ImpModify(clname, glt);
  }

  // in cases dom spcl_tps:
  switch(spcl_tps.Size()) {
    case 0: {
      return mk_(Bool(false), Nil(), Nil(), Nil(), Nil(), Nil(), Nil());
    }
    case 1: {
      TYPE_AS_Name cl (spcl_tps.Dom().GetElem());
      TYPE_AS_TypeDef td (ExtComp(spcl_tps[cl][thename], cl));

      const TYPE_AS_Access & access (td.GetField(pos_AS_TypeDef_access));
      Bool scopeok (theState().AccessOk(access, origcl, cl));

      return mk_(scopeok, td.GetRecord(pos_AS_TypeDef_shape),
                          td.GetField(pos_AS_TypeDef_Inv),
                          td.GetField(pos_AS_TypeDef_Eq),
                          td.GetField(pos_AS_TypeDef_Ord),
                          cl, access);
    }
    default: {
      TYPE_AS_Name cl;
      bool doesthere = false;
      if (spcl_tps.DomExists(classname)) {
        cl = classname;
        doesthere = true;
      }
      else {
        SET<TYPE_AS_Name> dom_spcl_tps (spcl_tps.Dom());
        SET<TYPE_AS_Name> rem_priv;
        Generic clnm;
        for (bool cc = dom_spcl_tps.First(clnm); cc; cc = dom_spcl_tps.Next(clnm)) {
          TYPE_AS_TypeDef td (spcl_tps[clnm][thename]);
          const TYPE_AS_Access & acc (td.GetField(pos_AS_TypeDef_access));
          if ((acc != Int(PRIVATE_AS)) && (acc != Int(NOT_INITIALISED_AS))) {
            rem_priv.Insert(clnm);
          }
        }
        Tuple eoc (theState().ExistsOneChild(rem_priv, dom_spcl_tps));
        if (eoc.GetBool(1)) {
          cl = eoc.GetRecord(2);
          doesthere = true;
        }
      }
      if (doesthere) {
        TYPE_AS_TypeDef td (ExtComp(spcl_tps[cl][thename], cl));

        const TYPE_AS_Access & access (td.GetField(pos_AS_TypeDef_access));
        Bool scopeok (theState().AccessOk(access, origcl, cl));

        return mk_(scopeok, td.GetRecord(pos_AS_TypeDef_shape),
                            td.GetField(pos_AS_TypeDef_Inv),
                            td.GetField(pos_AS_TypeDef_Eq),
                            td.GetField(pos_AS_TypeDef_Ord),
                            cl, access);
      } else {
        vdm_iplog << L"The name was '" << ASTAUX::ASName2String (name) << L"'" << endl << flush;
        RTERR::Error(L"IsTypeDef", RTERR_MULT_DEF, Nil(), Nil(), Sequence());
        return mk_(Bool(false), Nil(), Nil(), Nil(), Nil(), Nil(), Nil());
      }
    }
  }
  return Tuple();               // dummy
}

// ExtComp
// td : AS`TypeDef
// clnm : AS`Name
// ==> AS`TypeDef
TYPE_AS_TypeDef AUX::ExtComp(const TYPE_AS_TypeDef & td, const TYPE_AS_Name & clnm)
{
  const TYPE_AS_Type & shape (td.GetRecord(pos_AS_TypeDef_shape));

  if (shape.Is(TAG_TYPE_AS_CompositeType)) {
    const TYPE_AS_Name & name (shape.GetRecord(pos_AS_CompositeType_name));
    TYPE_AS_Name tag_name (ConstructDoubleName(clnm, name));

    TYPE_AS_CompositeType new_shape (shape);
    new_shape.SetField(pos_AS_CompositeType_name, tag_name);

    TYPE_AS_TypeDef new_td (td);
    new_td.SetField(pos_AS_TypeDef_nm, tag_name);
    new_td.SetField(pos_AS_TypeDef_shape, new_shape);

    return new_td;
  }
  else {
    return td;
  }
}

// IsRecSel
// nm : AS`Name
// ==> bool * [map AS`Name to nat]
#endif //VDMPP

#ifdef VDMSL
// LookUpRenRecSel
// name : AS`Name
// ==> bool * [GLOBAL`RecSel] * [AS`Name] * [AS`Name]
Tuple AUX::LookUpRenRecSel (const TYPE_AS_Name & name)
{
  TYPE_AS_Name rec_name (ExtractName(name));
  const TYPE_GLOBAL_SigmaMO & sigmamo (theState().GetModule(theState().ExtractModule(name)));

  Generic real_name;
  if (sigmamo.GetMap(pos_GLOBAL_SigmaMO_ren).DomExists(rec_name, real_name)) {
    TYPE_AS_Name real_rec_name (ExtractName(real_name));
    TYPE_AS_Name real_mod_name (theState().ExtractModule(real_name));

    const TYPE_GLOBAL_SigmaMO & real_sigmamo (theState().GetModule(real_mod_name));

    Generic recsel;
    if(real_sigmamo.GetMap(pos_GLOBAL_SigmaMO_recsel).DomExists(real_rec_name, recsel)) {
      return mk_(Bool(true), recsel, real_mod_name, real_rec_name);
    }
  }
  return mk_(Bool(false), Nil(), Nil(), Nil());
}

// LookUpRename
// name : AS`Name
// ==> bool * AS`Name
Tuple AUX::LookUpRename (const TYPE_AS_Name & name)
{
  TYPE_AS_Name rec_name (ExtractName(name));
  TYPE_AS_Name mod_name (theState().ExtractModule(name));
  const TYPE_GLOBAL_SigmaMO & sigmamo (theState().GetModule(mod_name));

  Generic real_name;
  if (sigmamo.GetMap(pos_GLOBAL_SigmaMO_ren).DomExists(rec_name, real_name)) {
    return mk_(Bool(true), real_name);
  }
  else {
    return mk_(Bool(false), name);
  }
}

// Note: The LookUpRecSel replaces to the IsRecSel and LookUpRecSel.
// IsRecSel
// name : AS`Name
// ==> bool

// LookUpRecSel
// name : AS`Name
// ==> bool * [GLOBAL`RecSel]
Tuple AUX::LookUpRecSel (const TYPE_AS_Name & name)
{
  Tuple name_t (RenameExtractModule(name));
  const TYPE_GLOBAL_SigmaMO & sigmamo (theState().GetModule(name_t.GetField(1)));
  const TYPE_AS_Name & rec_name (name_t.GetRecord(2));

  Generic recsel;
  if (sigmamo.GetMap(pos_GLOBAL_SigmaMO_recsel).DomExists(rec_name, recsel)) {
    return mk_(Bool(true), recsel);
  }
  else {
    return mk_(Bool(false), Nil());
  }
}
#endif //VDMSL

#ifdef VDMPP
// This function could return void but is seems more consistant to let
// is have the same prototype as the VDMSL function.
// LookUpRecSel
// name : AS`Name
// SPEC: ==> [GLOBAL`RecSel]
// IMPL: ==> bool * [GLOBAL`RecSel]
Tuple AUX::LookUpRecSel(const TYPE_AS_Name & name)
{
  TYPE_AS_Name clnm (ASTAUX::GetFirstName(name));
  TYPE_AS_Name tgnm (ASTAUX::GetSecondName(name));

  Generic recsel;
  if (theState().GetRecSel(clnm).DomExists(tgnm, recsel))
    //return mk_(Bool(true), Tuple(recsel_info).GetField(1));
    return mk_(Bool(true), recsel);
  else
    return mk_(Bool(false), Nil());    // dummy
}
#endif //VDMPP

// Ceiling
// val : real
// ==> int

// IsNat
// val_v : SEM`VAL
// ==> bool
bool AUX::IsNat (const TYPE_SEM_VAL & val_v)
{
  return (val_v.Is(TAG_TYPE_SEM_NUM) ? val_v.GetReal(pos_SEM_NUM_v).IsNat() : false);
}

// IsNatOne
// val_l : SEM`VAL
// ==> bool
bool AUX::IsNatOne (const TYPE_SEM_VAL & val_v)
{
  return (val_v.Is(TAG_TYPE_SEM_NUM) ? val_v.GetReal(pos_SEM_NUM_v).IsNatOne() : false);
}

// IsInt
// val_v : SEM`VAL
// ==> bool
bool AUX::IsInt (const TYPE_SEM_VAL & val_v)
{
  return (val_v.Is(TAG_TYPE_SEM_NUM) ? val_v.GetReal(pos_SEM_NUM_v).IsInt() : false);
}

// IsReal
// val_v : SEM`VAL
// ==> bool
bool AUX::IsReal (const TYPE_SEM_VAL & val_v)
{
  return val_v.Is(TAG_TYPE_SEM_NUM);
}

// IsRat
// val_v : SEM`VAL
// ==> bool
bool AUX::IsRat (const TYPE_SEM_VAL & val_v)
{
  return val_v.Is(TAG_TYPE_SEM_NUM);
}

// IsStmt
// se : AS`Expr | AS`Stmt
// +> bool
bool AUX::IsStmt(const Record & se)
{
  switch (se.GetTag()) {
    case TAG_TYPE_AS_DefStmt:
    case TAG_TYPE_AS_LetStmt:
    case TAG_TYPE_AS_LetBeSTStmt:
    case TAG_TYPE_AS_AssignStmt:
    case TAG_TYPE_AS_SeqForLoopStmt:
    case TAG_TYPE_AS_SetForLoopStmt:
    case TAG_TYPE_AS_IndexForLoopStmt:
    case TAG_TYPE_AS_WhileLoopStmt:
    case TAG_TYPE_AS_CallStmt:
    case TAG_TYPE_AS_ReturnStmt:
    case TAG_TYPE_AS_IfStmt:
    case TAG_TYPE_AS_CasesStmt:
    case TAG_TYPE_AS_ErrorStmt:
    case TAG_TYPE_AS_ExitStmt:
    case TAG_TYPE_AS_AlwaysStmt:
    case TAG_TYPE_AS_TrapStmt:
    case TAG_TYPE_AS_RecTrapStmt:
    case TAG_TYPE_AS_BlockStmt:
    case TAG_TYPE_AS_NonDetStmt:
    case TAG_TYPE_AS_IdentStmt:
    case TAG_TYPE_AS_SpecificationStmt: {
      return true;
    }
    default:
      return false;
  }
}

// RestSeqVal
// l : seq of SEM`VAL
// i : nat
// -> seq of SEM`VAL


// This function is named Premute in the specification (module AUX),
// but is renamed AUX_Permute in order to avoid name
// clash with Permute in libCG.a (used bu code generated
// by the C++ code generator).
// 96/01/16 (HV)
// Permute:
// l : seq of SEM`VAL
// -> set of seq of SEM`VAL
SET<Sequence> AUX::AUX_Permute (const SEQ<TYPE_SEM_VAL> & l)
{
  return l.Permute();
/*
  SET<Sequence> res_s;
  if (l.Length () <= 1)
    res_s.Insert (l);
  else
  {
    size_t maxi = l.Length ();
    for (size_t i = 1; i <= maxi; i++)
    {
      TYPE_SEM_VAL e (l[i]);
      SEQ<TYPE_SEM_VAL> tmp_l;
      for (size_t j = 1; j <= maxi; j++)
        if (i != j)
          tmp_l.ImpAppend (l[j]);

      SET<Sequence> perm_s (AUX_Permute (tmp_l));
      Generic ll;
      for (bool bb = perm_s.First (ll); bb; bb = perm_s.Next (ll))
      {
        SEQ<TYPE_SEM_VAL> tbd_l (ll);
        tbd_l.ImpPrepend (e);
        res_s.Insert (tbd_l);
      }
    }
  }
  return (res_s);
*/
}

// ValSetToSeq
// set_sv : set of SEM`VAL
// ==> seq of SEM`VAL
SEQ<TYPE_SEM_VAL> AUX::ValSetToSeq (const SET<TYPE_SEM_VAL> & set_sv)
{
  return set_sv.ToSequence();
}

// SetToSeq
// val_sv : set of SEM`VAL
// ==> seq of SEM`VAL
SEQ<TYPE_SEM_VAL> AUX::SetToSeq (const SET<TYPE_SEM_VAL> & s)
{
//  return s.ToSequence();
  SEQ<TYPE_SEM_VAL> res;
  SEQ<TYPE_SEM_VAL> already (s.ToSequence()); 
  while(!already.IsEmpty()) {
    size_t index = 1;
    const TYPE_SEM_VAL & val(already[index]);
    Real r (val.GetReal(pos_SEM_NUM_v));
    for (size_t i = 2; i <= already.Length(); i++) {
      if (r.GreaterThan(already[i].GetReal(pos_SEM_NUM_v))) {
        index = i;
        r = already[i].GetReal(pos_SEM_NUM_v);
      }
    }
    res.ImpAppend(already[index]);
    already.RemElem(index);
  }
  return res;
}

// SeqOfSetOf2SetOfSeqOf
// seq_ls : seq of set of (SEM`VAL | SEM`BlkEnv)
// ==> set of seq of (SEM`VAL | SEM`BlkEnv)
SET<Sequence> AUX::SeqOfSetOf2SetOfSeqOf (const SEQ<Set> & seq_ls)
{
  return SeqOfSetOf2SeqOfSeqOf(seq_ls).Elems();
}

// This is an variant of SeqOfSetOf2SetOfSeqOf that does not return
// a Set of cartesian products, but just a sequence of them.

// SeqOfSetOf2SeqOfSeqOf
// seq_ls : seq of set of (SEM`VAL | SEM`BlkEnv)
// ==> seq of seq of (SEM`VAL | SEM`BlkEnv)
SEQ<Sequence> AUX::SeqOfSetOf2SeqOfSeqOf (const SEQ<Set> & l)
{
  return l.DistrSetProduct();
}

// Min
// n_v : SEM`VAL
// set_sv : set of SEM`VAL
// ==> bool


#ifdef VDMSL
bool AUX::FindTagName (const TYPE_AS_Name& name, const TYPE_AS_Type & tp)
{
  switch (tp.GetTag()) {
    case TAG_TYPE_AS_CompositeType: {
      return (tp.GetRecord(pos_AS_CompositeType_name) == name);
    }
    case TAG_TYPE_AS_BracketedType: {
      return FindTagName (name, tp.GetRecord(pos_AS_BracketedType_tp));
    }
    case TAG_TYPE_AS_OptionalType: {
      return FindTagName (name, tp.GetRecord(pos_AS_OptionalType_tp));
    }
    case TAG_TYPE_AS_Set0Type: {
      return FindTagName (name, tp.GetRecord(pos_AS_Set0Type_elemtp));
    }
    case TAG_TYPE_AS_Set1Type: {
      return FindTagName (name, tp.GetRecord(pos_AS_Set1Type_elemtp));
    }
    case TAG_TYPE_AS_Seq0Type: {
      return FindTagName (name, tp.GetRecord(pos_AS_Seq0Type_elemtp));
    }
    case TAG_TYPE_AS_Seq1Type: {
      return FindTagName (name, tp.GetRecord(pos_AS_Seq1Type_elemtp));
    }
    case TAG_TYPE_AS_GeneralMap0Type: {
      return FindTagName (name, tp.GetRecord(pos_AS_GeneralMap0Type_mapdom)) ||
             FindTagName (name, tp.GetRecord(pos_AS_GeneralMap0Type_maprng));
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      return FindTagName (name, tp.GetRecord(pos_AS_GeneralMap1Type_mapdom)) ||
             FindTagName (name, tp.GetRecord(pos_AS_GeneralMap1Type_maprng));
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      return FindTagName (name, tp.GetRecord(pos_AS_InjectiveMap0Type_mapdom)) ||
             FindTagName (name, tp.GetRecord(pos_AS_InjectiveMap0Type_maprng));
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      return FindTagName (name, tp.GetRecord(pos_AS_InjectiveMap1Type_mapdom)) ||
             FindTagName (name, tp.GetRecord(pos_AS_InjectiveMap1Type_maprng));
    }
    case TAG_TYPE_AS_BooleanType:
    case TAG_TYPE_AS_NumericType:
    case TAG_TYPE_AS_TokenType:
    case TAG_TYPE_AS_CharType:
    case TAG_TYPE_AS_QuoteType:
    case TAG_TYPE_AS_TypeName:
    case TAG_TYPE_AS_TypeVar:
    case TAG_TYPE_AS_UnionType:
    case TAG_TYPE_AS_ProductType:
    case TAG_TYPE_AS_PartialFnType:
    case TAG_TYPE_AS_TotalFnType:
    case TAG_TYPE_AS_OpType:
    default: {
      return false;
    }
  }
}

// IsDefinedInModule
// name : AS`Name
// ==> bool
bool AUX::IsDefinedInModule (const TYPE_AS_Name& name)
{
  const TYPE_GLOBAL_SigmaMO & sigmamo (theState().GetModule(theState().ExtractModule(name)));
  TYPE_AS_Name tag_id (ASTAUX::UnqualiName(name));

  Set tps (sigmamo.GetMap(pos_GLOBAL_SigmaMO_tps).Rng());

  bool exists = false;
  Generic tp;
  for(bool bb = tps.First(tp); bb && !exists; bb = tps.Next(tp))
  {
    const TYPE_AS_Type & shape (Record(tp).GetRecord(pos_AS_TypeDef_shape));
    exists = FindTagName(tag_id, shape);
  }
  return exists;
}
#endif // VDMSL

// PreName
// nm : AS`Name
// -> AS`Name
TYPE_AS_Name AUX::PreName(const TYPE_AS_Name & nm)
{
  TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
  TYPE_AS_Id id (ASTAUX::MkId(L"pre_").ImpConc(ids[ids.Length()])); 
  TYPE_AS_Name name (nm);
  name.SetField(pos_AS_Name_ids, ids.SubSequence(1,ids.Length() -1).ImpAppend(id));
  return name;
}

// PostName
// nm : AS`Name
// -> AS`Name
TYPE_AS_Name AUX::PostName(const TYPE_AS_Name & nm)
{
  TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
  TYPE_AS_Id id (ASTAUX::MkId(L"post_").ImpConc(ids[ids.Length()])); 
  TYPE_AS_Name name (nm);
  name.SetField(pos_AS_Name_ids, ids.SubSequence(1,ids.Length() -1).ImpAppend(id));
  return name;
}

// InvName
// nm : AS`Name
// -> AS`Name
TYPE_AS_Name AUX::InvName(const TYPE_AS_Name & nm)
{
  TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
  TYPE_AS_Id id (ASTAUX::MkId(L"inv_").ImpConc(ids[ids.Length()])); 
  TYPE_AS_Name name (nm);
  name.SetField(pos_AS_Name_ids, ids.SubSequence(1,ids.Length() -1).ImpAppend(id));
  return name;
}

// InitName
// nm : AS`Name
// -> AS`Name
TYPE_AS_Name AUX::InitName(const TYPE_AS_Name & nm)
{
  TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
  TYPE_AS_Id id (ASTAUX::MkId(L"init_").ImpConc(ids[ids.Length()])); 
  TYPE_AS_Name name (nm);
  name.SetField(pos_AS_Name_ids, ids.SubSequence(1,ids.Length() -1).ImpAppend(id));
  return name;
}

// OrderName
// nm : AS`Name
// -> AS`Name
TYPE_AS_Name AUX::OrderName(const TYPE_AS_Name & nm)
{
  TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
  TYPE_AS_Id id (ASTAUX::MkId(L"ord_").ImpConc(ids[ids.Length()])); 
  TYPE_AS_Name name (nm);
  name.SetField(pos_AS_Name_ids, ids.SubSequence(1,ids.Length() -1).ImpAppend(id));
  return name;
}

// MaxName
// nm : AS`Name
// -> AS`Name
TYPE_AS_Name AUX::MaxName(const TYPE_AS_Name & nm)
{
  TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
  TYPE_AS_Id id (ASTAUX::MkId(L"max_").ImpConc(ids[ids.Length()])); 
  TYPE_AS_Name name (nm);
  name.SetField(pos_AS_Name_ids, ids.SubSequence(1,ids.Length() -1).ImpAppend(id));
  return name;
}

// MinName
// nm : AS`Name
// -> AS`Name
TYPE_AS_Name AUX::MinName(const TYPE_AS_Name & nm)
{
  TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
  TYPE_AS_Id id (ASTAUX::MkId(L"min_").ImpConc(ids[ids.Length()])); 
  TYPE_AS_Name name (nm);
  name.SetField(pos_AS_Name_ids, ids.SubSequence(1,ids.Length() -1).ImpAppend(id));
  return name;
}

// EqualityName
// nm : AS`Name
// -> AS`Name
TYPE_AS_Name AUX::EqualityName(const TYPE_AS_Name & nm)
{
  TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
  TYPE_AS_Id id (ASTAUX::MkId(L"eq_").ImpConc(ids[ids.Length()])); 
  TYPE_AS_Name name (nm);
  name.SetField(pos_AS_Name_ids, ids.SubSequence(1,ids.Length() -1).ImpAppend(id));
  return name;
}

#ifdef VDMSL
// ExtractId
// nm : AS`Name
// -> AS`Id
TYPE_AS_Id AUX::ExtractId (const TYPE_AS_Name& nm)
{
  return ASTAUX::GetLastId(nm);
}
#endif //VDMSL

// ExtractName
// name : AS`Name
// -> AS`Name
TYPE_AS_Name AUX::ExtractName (const TYPE_AS_Name & name)
{
  return ASTAUX::UnqualiName(name);
}

// SingleNameToString
// nm : AS`Name
// -> seq of char
SEQ<Char> AUX::SingleNameToString(const TYPE_AS_Name & nm)
{
  return SEQ<Char>(ASTAUX::ASName2String(nm));
}

// ConstructDoubleName
// nm1 : AS`Name
// nm2 : AS`Name
// -> AS`Name
TYPE_AS_Name AUX::ConstructDoubleName(const TYPE_AS_Name & nm1, const TYPE_AS_Name & nm2)
{
  return ASTAUX::Combine2Names(nm1, nm2);
}

#ifdef VDMSL
// IsRealName
// name : AS`Name
// -> bool
bool AUX::IsRealName (const TYPE_AS_Name & name)
{
  return (name.GetSequence(pos_AS_Name_ids).Length () > 1);
}
#endif //VDMSL

// EqualNames
// name1 : AS`Name
// name2 : AS`Name
// -> bool
bool AUX::EqualNames (const TYPE_AS_Name & name1, const TYPE_AS_Name & name2)
{
  return (name1.GetSequence(pos_AS_Name_ids) == name2.GetSequence(pos_AS_Name_ids));
}

#ifdef VDMSL
// IsInDLDeclared
// loc_name : AS`Name
// mod_name : AS`Name
// ==> bool
bool AUX::IsInDLDeclared (const TYPE_AS_Name & loc_name, const TYPE_AS_Name & mod_name)
{
  TYPE_GLOBAL_SigmaIMO sigmaimo (theState().GetDLModule(mod_name));
  TYPE_AS_DLExportSig exp_sig (sigmaimo.get_exp());

  return exp_sig.GetMap(pos_AS_DLExportSig_val).DomExists(loc_name) ||
         exp_sig.GetMap(pos_AS_DLExportSig_fns).DomExists(loc_name) ||
         exp_sig.GetMap(pos_AS_DLExportSig_ops).DomExists(loc_name);
}

// RenameExtractModule
// name : AS`Name
// ==> AS`Name * AS`Name
Tuple AUX::RenameExtractModule(const TYPE_AS_Name & name)
{
  TYPE_AS_Name rec_name (ExtractName(name));
  TYPE_AS_Name mod_name (theState().ExtractModule(name));
  const TYPE_GLOBAL_SigmaMO & sigmamo (theState().GetModule(mod_name));

  const Map & ren (sigmamo.GetMap(pos_GLOBAL_SigmaMO_ren));
  if (ren.DomExists(rec_name)) {
    return mk_(theState().ExtractModule(ren[rec_name]), ExtractName(ren[rec_name]));
  }
  else {
    return mk_(mod_name, rec_name);
  }
}
#endif // VDMSL

/*
// PrintName
// name : AS`Name
// -> seq of char
SEQ<Char> AUX::PrintName(const TYPE_AS_Name & name)
{
  const TYPE_AS_Ids & ids (name.GetSequence(pos_AS_Name_ids));
  switch (ids.Length()) {
    case 0:
      return SEQ<Char>(L"");
    case 1:
      return ids[1];
    case 2:
      return ids[1].Conc(SEQ<Char>(L"`")).Conc(ids[2]);
    default: {
      SEQ<Char> res (ids[1]);
      size_t len_ids = ids.Length();
      for (size_t idx = 2; idx <= len_ids; idx++)
      {
        res.ImpAppend(Char(L'`'));
        res.ImpConc(ids[idx]);
      }
      return res;
    }
  }
}

// PrintType
// tp : AS`Type
// -> seq of char
SEQ<Char> AUX::PrintType(const TYPE_AS_Type & tp)
{
  switch (tp.GetTag()) {
    case TAG_TYPE_AS_BooleanType: {
      return SEQ<Char>(L"bool");
    }
    case TAG_TYPE_AS_NumericType: {
      switch (tp.GetIntValue(pos_AS_NumericType_qtp)) {
        case NATONE:     return SEQ<Char>(L"nat1");
        case NAT:        return SEQ<Char>(L"nat");
        case INTEGER:    return SEQ<Char>(L"int");
        case RAT:        return SEQ<Char>(L"rat");
        case REAL:       return SEQ<Char>(L"real");
        default:         return SEQ<Char>(L"???");
      } 
    }
    case TAG_TYPE_AS_TokenType: {
      return SEQ<Char>(L"token");
    }
    case TAG_TYPE_AS_CharType: {
      return SEQ<Char>(L"char");
    }
    case TAG_TYPE_AS_VoidType: {
      return SEQ<Char>(L"()");
    }
    case TAG_TYPE_AS_QuoteType: {
      const TYPE_AS_QuoteLit & ql (tp.GetRecord(pos_AS_QuoteType_lit));
      return SEQ<Char>(L"<").Conc(ql.GetSequence(pos_AS_QuoteLit_val)).Conc(SEQ<Char>(L">"));
    }
    case TAG_TYPE_AS_TypeName: {
      return PrintName(tp.GetRecord(pos_AS_TypeName_name));
    }
    case TAG_TYPE_AS_TypeVar: {
      return SEQ<Char>(L"@").Conc(PrintName(tp.GetRecord(pos_REP_TypeVarRep_nm)));
    }
    case TAG_TYPE_AS_CompositeType: {
      SEQ<Char> res;
      res.ImpConc(SEQ<Char>(L"compose "));
      res.ImpConc(PrintName (tp.GetRecord(pos_AS_CompositeType_name)));
      res.ImpConc(SEQ<Char>(L" of "));

      const SEQ<TYPE_AS_Field> & fields (tp.GetSequence(pos_AS_CompositeType_fields));
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++)
      {
        const TYPE_AS_Field & field (fields[idx]);
        res.ImpConc(PrintType (field.GetRecord(pos_AS_Field_type)));
        res.ImpConc(SEQ<Char>(L" "));
      }
      res.ImpConc(SEQ<Char>(L"end"));
      return res;
    }
    case TAG_TYPE_AS_UnionType: {
      const SEQ<TYPE_AS_Type> & tps (tp.GetSequence(pos_AS_UnionType_tps));
      SEQ<Char> res (L"( ");
      size_t len_tps = tps.Length();
      for (size_t idx = 1; idx <= len_tps; idx++)
      {
        if (idx > 1)
          res.ImpConc(SEQ<Char>(L" | "));
        res.ImpConc(PrintType (tps[idx]));
      }
      res.ImpConc(SEQ<Char>(L" )"));
      return res;
    }
    case TAG_TYPE_AS_ProductType: {
      const SEQ<TYPE_AS_Type> & tps (tp.GetSequence(pos_AS_ProductType_tps));
      SEQ<Char> res (L"( ");
      size_t len_tps = tps.Length();
      for (size_t idx = 1; idx <= len_tps; idx++)
      {
        if (idx > 1)
          res.ImpConc(SEQ<Char>(L" * "));
        res.ImpConc(PrintType (tps[idx]));
      }
      res.ImpConc(SEQ<Char>(L" )"));
      return res;
    }
    case TAG_TYPE_AS_BracketedType: {
      return SEQ<Char>(L"( ").Conc(PrintType (tp.GetRecord(pos_AS_BracketedType_tp))).Conc(SEQ<Char>(L" )"));
    }
    case TAG_TYPE_AS_OptionalType: {
      return SEQ<Char>(L"[").Conc(PrintType (tp.GetRecord(pos_AS_BracketedType_tp))).Conc(SEQ<Char>(L"]"));
    }
    case TAG_TYPE_AS_Set0Type: {
      return SEQ<Char>(L"set of ").Conc(PrintType (tp.GetRecord(pos_AS_Set0Type_elemtp)));
    }
    case TAG_TYPE_AS_Set1Type: {
      return SEQ<Char>(L"set1 of ").Conc(PrintType (tp.GetRecord(pos_AS_Set1Type_elemtp)));
    }
    case TAG_TYPE_AS_Seq0Type: {
      return SEQ<Char>(L"seq of ").Conc(PrintType (tp.GetRecord(pos_AS_Seq0Type_elemtp)));
    }
    case TAG_TYPE_AS_Seq1Type: {
      return SEQ<Char>(L"seq1 of ").Conc(PrintType (tp.GetRecord(pos_AS_Seq1Type_elemtp)));
    }
    case TAG_TYPE_AS_GeneralMap0Type: {
      return SEQ<Char>(L"map ").Conc(PrintType (tp.GetRecord(pos_AS_GeneralMap0Type_mapdom)))
             .Conc(SEQ<Char>(L" to ")).Conc(PrintType (tp.GetRecord(pos_AS_GeneralMap0Type_maprng)));
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      return SEQ<Char>(L"map1 ").Conc(PrintType (tp.GetRecord(pos_AS_GeneralMap1Type_mapdom)))
             .Conc(SEQ<Char>(L" to ")).Conc(PrintType (tp.GetRecord(pos_AS_GeneralMap1Type_maprng)));
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      return SEQ<Char>(L"inmap ").Conc(PrintType (tp.GetRecord(pos_AS_InjectiveMap0Type_mapdom)))
             .Conc(SEQ<Char>(L" to ")).Conc(PrintType (tp.GetRecord(pos_AS_InjectiveMap0Type_maprng)));
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      return SEQ<Char>(L"inmap1 ").Conc(PrintType (tp.GetRecord(pos_AS_InjectiveMap1Type_mapdom)))
             .Conc(SEQ<Char>(L" to ")).Conc(PrintType (tp.GetRecord(pos_AS_InjectiveMap1Type_maprng)));
    }
    case TAG_TYPE_AS_OpType: {
      const SEQ<TYPE_AS_Type> & opdom (tp.GetSequence(pos_AS_OpType_opdom));
      SEQ<Char> res;
      if (opdom.IsEmpty())
        res.ImpConc(SEQ<Char>(L"()"));
      else
      {
        res.ImpConc(SEQ<Char>(L"( "));
        size_t len_opdom = opdom.Length();
        for (size_t idx = 1; idx <= len_opdom; idx++)
        {
          if (idx > 1)
            res.ImpConc(SEQ<Char>(L" * "));
          res.ImpConc(PrintType (opdom[idx]));
        }
        res.ImpConc(SEQ<Char>(L" )"));
      }
      res.ImpConc(SEQ<Char>(L" ==> "));
      res.ImpConc(PrintType (tp.GetRecord(pos_AS_OpType_oprng)));
      return res;
    }
    case TAG_TYPE_AS_PartialFnType: {
      const SEQ<TYPE_AS_Type> & fndom (tp.GetSequence(pos_AS_PartialFnType_fndom));
      SEQ<Char> res;
      if (fndom.IsEmpty())
        res.ImpConc(SEQ<Char>(L"()"));
      else
      {
        res.ImpConc(SEQ<Char>(L"( "));
        size_t len_fndom = fndom.Length();
        for (size_t idx = 1; idx <= len_fndom; idx++)
        {
          if (idx > 1)
            res.ImpConc(SEQ<Char>(L" * "));
          res.ImpConc(PrintType (fndom[idx]));
        }
        res.ImpConc(SEQ<Char>(L" )"));
      }
      res.ImpConc(SEQ<Char>(L" -> "));
      res.ImpConc(PrintType (tp.GetRecord(pos_AS_PartialFnType_fnrng)));
      return res;
    }
    case TAG_TYPE_AS_TotalFnType: {
      const SEQ<TYPE_AS_Type> & fndom (tp.GetSequence(pos_AS_TotalFnType_fndom));
      SEQ<Char> res;
      if (fndom.IsEmpty())
        res.ImpConc(SEQ<Char>(L"()"));
      else
      {
        res.ImpConc(SEQ<Char>(L"( "));
        size_t len_fndom = fndom.Length();
        for (size_t idx = 1; idx <= len_fndom; idx++)
        {
          if (idx > 1)
            res.ImpConc(SEQ<Char>(L" * "));
          res.ImpConc(PrintType (fndom[idx]));
        }
        res.ImpConc(SEQ<Char>(L" )"));
      }
      res.ImpConc(SEQ<Char>(L" -> "));
      res.ImpConc(PrintType (tp.GetRecord(pos_AS_TotalFnType_fnrng)));
      return res;
    }
    case TAG_TYPE_AS_AllType: {
      return SEQ<Char>(L"?");
    }
    default: {
      return SEQ<Char>(L"unknown type !!");
    }
// TODO
  }
  return SEQ<Char>(); // dummy
}
*/


// ClModNmInPatternBindList
// pat_l : seq of AS`PatternBind
// -> set of AS`Name
SET<TYPE_AS_Name> AUX::ClModNmInPatternBindList(const SEQ<TYPE_AS_PatternBind> & pat_l)
{
  SET<TYPE_AS_Name> nms;
  size_t len_pat_l = pat_l.Length();
  for (size_t i = 1; i <= len_pat_l; i++)
    nms.ImpUnion(ClModNmInPatternBind(pat_l[i]));
  return nms;
}

// ClModNmInPatternBind
// patb : AS`PatternBind
// -> set of AS`Name
SET<TYPE_AS_Name> AUX::ClModNmInPatternBind(const TYPE_AS_PatternBind & patb)
{
  switch (patb.GetTag()) {
    case TAG_TYPE_AS_PatternName: {
      return SET<TYPE_AS_Name>();
      break;
    }
    case TAG_TYPE_AS_MatchVal: {
      return ClModNmInExpr(patb.GetRecord(pos_AS_MatchVal_val));
      break;
    }
    case TAG_TYPE_AS_SetEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & Elems (patb.GetSequence(pos_AS_SetEnumPattern_Elems));
      SET<TYPE_AS_Name> nms;
      size_t len_Elems = Elems.Length();
      for (size_t i = 1; i <= len_Elems; i++ )
        nms.ImpUnion(ClModNmInPatternBind(Elems[i]));
      return nms;
      break;
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      SET<TYPE_AS_Name> nms (patb.GetRecord(pos_AS_SetUnionPattern_lp));
      nms.ImpUnion(ClModNmInPatternBind(patb.GetRecord(pos_AS_SetUnionPattern_rp)));
      return nms;
      break;
    }
    case TAG_TYPE_AS_SeqEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & els (patb.GetSequence(pos_AS_SeqEnumPattern_els));
      SET<TYPE_AS_Name> nms;
      size_t len_els = els.Length();
      for (size_t i = 1; i <= len_els; i++ )
        nms.ImpUnion(ClModNmInPatternBind(els[i]));
      return nms;
      break;
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      SET<TYPE_AS_Name> nms (patb.GetRecord(pos_AS_SeqConcPattern_lp));
      nms.ImpUnion(ClModNmInPatternBind(patb.GetRecord(pos_AS_SeqConcPattern_rp)));
      return nms;
      break;
    }
    case TAG_TYPE_AS_MapEnumPattern: {
      const SEQ<TYPE_AS_Maplet> & mls (patb.GetSequence(pos_AS_MapEnumPattern_mls));
      SET<TYPE_AS_Name> nms;
      size_t len_mls = mls.Length();
      for (size_t i = 1; i <= len_mls; i++ ) {
        nms.ImpUnion(ClModNmInPatternBind(mls[i].GetRecord(pos_AS_Maplet_mapdom)));
        nms.ImpUnion(ClModNmInPatternBind(mls[i].GetRecord(pos_AS_Maplet_maprng)));
      }
      return nms;
      break;
    }
    case TAG_TYPE_AS_MapMergePattern: {
      SET<TYPE_AS_Name> nms (patb.GetRecord(pos_AS_MapMergePattern_lp));
      nms.ImpUnion(ClModNmInPatternBind(patb.GetRecord(pos_AS_MapMergePattern_rp)));
      return nms;
      break;
    }
    case TAG_TYPE_AS_RecordPattern: {
      const SEQ<TYPE_AS_Pattern> & fields (patb.GetSequence(pos_AS_RecordPattern_fields));
      SET<TYPE_AS_Name> nms;
      size_t len_fields = fields.Length();
      for (size_t i = 1; i <= len_fields; i++ )
        nms.ImpUnion(ClModNmInPatternBind(fields[i]));
      return nms;
      break;
    }
    case TAG_TYPE_AS_TuplePattern: {
      const SEQ<TYPE_AS_Pattern> & fields (patb.GetSequence(pos_AS_TuplePattern_fields));
      SET<TYPE_AS_Name> nms;
      size_t len_fields = fields.Length();
      for (size_t i = 1; i <= len_fields; i++ )
        nms.ImpUnion(ClModNmInPatternBind(fields[i]));
      return nms;
      break;
    }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern: {
      const SEQ<TYPE_AS_Pattern> & fields (patb.GetSequence(pos_AS_ObjectPattern_fields));
      SET<TYPE_AS_Name> nms;
      size_t len_fields = fields.Length();
      for (size_t i = 1; i <= len_fields; i++ )
        nms.ImpUnion(ClModNmInPatternBind(fields[i].GetRecord(pos_AS_FieldPattern_pat)));
      return nms;
      break;
    }
#endif // VDMPP
    case TAG_TYPE_AS_SetBind: {
      SET<TYPE_AS_Name> nms (ClModNmInPatternBind(patb.GetRecord(pos_AS_SetBind_pat)));
      nms.ImpUnion(ClModNmInExpr(patb.GetRecord(pos_AS_SetBind_Set)));
      return nms;
      break;
    }
    case TAG_TYPE_AS_TypeBind: {
      return ClModNmInPatternBind(patb.GetRecord(pos_AS_TypeBind_pat));
      break;
    }
  }
  return SET<TYPE_AS_Name>(); // dummy
}

// ClModNmInMultBind
// bind : AS`MultBind
// -> set of AS`Name
SET<TYPE_AS_Name> AUX::ClModNmInMultBind(const TYPE_AS_MultBind & bind)
{
  switch (bind.GetTag()) {
    case TAG_TYPE_AS_MultSetBind: {
      const SEQ<TYPE_AS_Pattern> & pat_l (bind.GetSequence(pos_AS_MultSetBind_pat));
      SET<TYPE_AS_Name> nms;
      size_t len_pat_l = pat_l.Length();
      for (size_t i = 1; i <= len_pat_l; i++ )
        nms.ImpUnion(ClModNmInPatternBind(pat_l[i]));
      nms.ImpUnion(ClModNmInExpr(bind.GetRecord(pos_AS_MultSetBind_Set)));
      return nms;
      break;
    }
    case TAG_TYPE_AS_MultTypeBind: {
      const SEQ<TYPE_AS_Pattern> & pat_l (bind.GetSequence(pos_AS_MultTypeBind_pat));
      SET<TYPE_AS_Name> nms;
      size_t len_pat_l = pat_l.Length();
      for (size_t i = 1; i <= len_pat_l; i++ )
        nms.ImpUnion(ClModNmInPatternBind(pat_l[i]));
      return nms;
      break;
    }
  }
  return SET<TYPE_AS_Name>(); // dummy
}

// ClModNmInMultBindList
// bind_l : seq of AS`MultBind
// -> set of AS`Name
SET<TYPE_AS_Name> AUX::ClModNmInMultBindList(const SEQ<TYPE_AS_MultBind> & bind_l)
{
  SET<TYPE_AS_Name> nms;
  size_t len_bind_l = bind_l.Length();
  for (size_t i = 1; i <= len_bind_l; i++)
    nms.ImpUnion(ClModNmInMultBind(bind_l[i]));
  return nms;
}

// ClModNmInLocalDef
// localdef_l : AS`LocalDef
// -> set of AS`Name
SET<TYPE_AS_Name> AUX::ClModNmInLocalDef(const SEQ<TYPE_AS_LocalDef> & localdef_l)
{
  SET<TYPE_AS_Name> nms;
  size_t len_localdef_l = localdef_l.Length();
  for (size_t i = 1; i <= len_localdef_l; i++) {
    const TYPE_AS_LocalDef & localdef(localdef_l[i]);
    switch (localdef.GetTag()) {
      case TAG_TYPE_AS_ValueDef: {
        nms.ImpUnion(ClModNmInPatternBind(localdef.GetRecord(pos_AS_ValueDef_pat)));
        nms.ImpUnion(ClModNmInExpr(localdef.GetRecord(pos_AS_ValueDef_val)));
        break;
      }
      case TAG_TYPE_AS_ExplFnDef: {
        const SEQ<TYPE_AS_Parameters> & parms (localdef.GetSequence(pos_AS_ExplFnDef_parms));
        const TYPE_AS_FnBody & fnbody         (localdef.GetRecord(pos_AS_ExplFnDef_body));
        const Generic & pre_e                 (localdef.GetField(pos_AS_ExplFnDef_fnpre));
        const Generic & post_e                (localdef.GetField(pos_AS_ExplFnDef_fnpost));
        const Generic & body (fnbody.GetField(pos_AS_FnBody_body));

        size_t len_parms = parms.Length();
        for (size_t j = 1; j <= len_parms; j++)
          nms.ImpUnion(ClModNmInPatternBindList(parms[j]));

#ifdef VDMSL
        if (body != Int(NOTYETSPEC))
#endif // VDMSL
#ifdef VDMPP
        if ((body != Int(NOTYETSPEC)) && (body != Int(SUBRESP)))
#endif // VDMPP
          nms.ImpUnion(ClModNmInExpr(body));

        if (!pre_e.IsNil())
          nms.ImpUnion(ClModNmInExpr(pre_e));
        if (!post_e.IsNil())
          nms.ImpUnion(ClModNmInExpr(post_e));
        break;
      }
      case TAG_TYPE_AS_ImplFnDef: {
        const SEQ<TYPE_AS_PatTypePair> & partps (localdef.GetSequence(pos_AS_ImplFnDef_partps));
        const Generic & pre_e           (localdef.GetField(pos_AS_ImplFnDef_fnpre));
        const TYPE_AS_Expr & post_e     (localdef.GetRecord(pos_AS_ImplFnDef_fnpost));

        size_t len_partps = partps.Length();
        for (size_t j = 1; j <= len_partps; j++)
          nms.ImpUnion(ClModNmInPatternBindList(partps[j].GetSequence(pos_AS_PatTypePair_pats)));

        if (!pre_e.IsNil())
          nms.ImpUnion(ClModNmInExpr(pre_e));
        nms.ImpUnion(ClModNmInExpr(post_e));
        break;
      }
      case TAG_TYPE_AS_ExtExplFnDef: {
        const SEQ<TYPE_AS_PatTypePair> & partps (localdef.GetSequence(pos_AS_ExtExplFnDef_partps));
        const TYPE_AS_FnBody & fnbody           (localdef.GetRecord(pos_AS_ExtExplFnDef_body));
        const Generic & pre_e                   (localdef.GetField(pos_AS_ExtExplFnDef_fnpre));
        const Generic & post_e                  (localdef.GetField(pos_AS_ExtExplFnDef_fnpost));
        const Generic & body (fnbody.GetField(pos_AS_FnBody_body));

        size_t len_partps = partps.Length();
        for (size_t j = 1; j <= len_partps; j++)
          nms.ImpUnion(ClModNmInPatternBindList(partps[j].GetSequence(pos_AS_PatTypePair_pats)));
#ifdef VDMSL
        if (body != Int(NOTYETSPEC))
#endif // VDMSL
#ifdef VDMPP
        if ((body != Int(NOTYETSPEC)) && (body != Int(SUBRESP)))
#endif // VDMPP
          nms.ImpUnion(ClModNmInExpr(body));

        if (!pre_e.IsNil())
          nms.ImpUnion(ClModNmInExpr(pre_e));
        if (!post_e.IsNil())
          nms.ImpUnion(ClModNmInExpr(post_e));
        break;
      }
    }
  }
  return nms;
}

// ClModNmInElseifExprList
// elsif_l : seq of AS`ElseifExpr
// -> set of AS`Name
SET<TYPE_AS_Name> AUX::ClModNmInElseifExprList(const SEQ<TYPE_AS_ElseifExpr> & elsif_l)
{
  SET<TYPE_AS_Name> nms;
  size_t len_elsif_l = elsif_l.Length();
  for (size_t i = 1; i <= len_elsif_l; i++) {
    nms.ImpUnion(ClModNmInExpr(elsif_l[i].GetRecord(pos_AS_ElseifExpr_test)));
    nms.ImpUnion(ClModNmInExpr(elsif_l[i].GetRecord(pos_AS_ElseifExpr_cons)));
  }
  return nms;
}

// ClModNmInCaseAltnList
// altns : seq of AS`CaseAltn
// -> set of AS`Name
SET<TYPE_AS_Name> AUX::ClModNmInCaseAltnList(const SEQ<TYPE_AS_CaseAltn> & altns)
{
  SET<TYPE_AS_Name> nms;
  size_t len_altns = altns.Length();
  for (size_t i = 1; i <= len_altns; i++) {
    nms.ImpUnion(ClModNmInPatternBindList(altns[i].GetSequence(pos_AS_CaseAltn_match)));
    nms.ImpUnion(ClModNmInExpr(altns[i].GetRecord(pos_AS_CaseAltn_body)));
  }
  return nms;
}

// ClModNmInExprList
// expr_l : seq of AS`Expr
// -> set of AS`Name
SET<TYPE_AS_Name> AUX::ClModNmInExprList(const SEQ<TYPE_AS_Expr> & expr_l)
{
  SET<TYPE_AS_Name> nms;
  size_t len_expr_l = expr_l.Length();
  for (size_t i = 1; i <= len_expr_l; i++)
    nms.ImpUnion(ClModNmInExpr(expr_l[i]));
  return nms;
}

// ClModNmInExpr
// e : AS`Expr
// -> set of AS`Name
SET<TYPE_AS_Name> AUX::ClModNmInExpr(const TYPE_AS_Expr & e)
{
  switch (e.GetTag()) {
    case TAG_TYPE_AS_Name: {
      SET<TYPE_AS_Name> nms;
      if (e.GetSequence(pos_AS_Name_ids).Length() == 2)
        nms.Insert(ASTAUX::GetFirstName(e)); 
      return nms;
      break;
    }
    case TAG_TYPE_AS_DefExpr: {
      const type_dd2PL & Defs (e.GetSequence(pos_AS_DefExpr_Def));
      SET<TYPE_AS_Name> nms;
      size_t len_Defs = Defs.Length();
      for (size_t i = 1; i <= len_Defs; i++) {
        nms.ImpUnion(ClModNmInPatternBind(Defs[i].GetField(1)));
        nms.ImpUnion(ClModNmInExpr(Defs[i].GetField(2)));
      }
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_DefExpr_In)));
      return nms;
      break;
    }
    case TAG_TYPE_AS_LetExpr: {
      SET<TYPE_AS_Name> nms;
      nms.ImpUnion(ClModNmInLocalDef(e.GetSequence(pos_AS_LetStmt_localdef)));
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_LetExpr_body)));
      return nms;
      break;
    }
    case TAG_TYPE_AS_LetBeSTExpr: {
      const Generic & St (e.GetField(pos_AS_LetBeSTExpr_St));
      SET<TYPE_AS_Name> nms;
      nms.ImpUnion(ClModNmInMultBindList(e.GetSequence(pos_AS_LetBeSTExpr_lhs)));
      if (!St.IsNil())
        nms.ImpUnion(ClModNmInExpr(St));
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_LetBeSTExpr_In)));
      return nms;
      break;
    }
    case TAG_TYPE_AS_IfExpr : {
      SET<TYPE_AS_Name> nms;
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_IfExpr_test)));
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_IfExpr_cons)));
      nms.ImpUnion(ClModNmInElseifExprList(e.GetSequence(pos_AS_IfExpr_elsif)));
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_IfExpr_altn)));
      return nms;
      break;
    }
    case TAG_TYPE_AS_CasesExpr : {
      const Generic & Others (e.GetField(pos_AS_CasesExpr_Others));
      SET<TYPE_AS_Name> nms;
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_CasesExpr_sel)));
      nms.ImpUnion(ClModNmInCaseAltnList(e.GetSequence(pos_AS_CasesExpr_altns)));
      if (!Others.IsNil())
        nms.ImpUnion(ClModNmInExpr(Others));
      return nms;
      break;
    }
    case TAG_TYPE_AS_PrefixExpr : {
      return ClModNmInExpr(e.GetRecord(pos_AS_PrefixExpr_arg));
    }
    case TAG_TYPE_AS_BinaryExpr : {
      SET<TYPE_AS_Name> nms;
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_BinaryExpr_left)));
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_BinaryExpr_right)));
      return nms;
      break;
    }
    case TAG_TYPE_AS_AllOrExistsExpr : {
      SET<TYPE_AS_Name> nms;
      nms.ImpUnion(ClModNmInMultBindList(e.GetSequence(pos_AS_AllOrExistsExpr_bind)));
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_AllOrExistsExpr_pred)));
      return nms;
      break;
    }
    case TAG_TYPE_AS_ExistsUniqueExpr : {
      SET<TYPE_AS_Name> nms;
      nms.ImpUnion(ClModNmInPatternBind(e.GetRecord(pos_AS_ExistsUniqueExpr_bind)));
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_ExistsUniqueExpr_pred)));
      return nms;
      break;
    }
    case TAG_TYPE_AS_IotaExpr : {
      SET<TYPE_AS_Name> nms;
      nms.ImpUnion(ClModNmInPatternBind(e.GetRecord(pos_AS_IotaExpr_bind)));
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_IotaExpr_pred)));
      return nms;
      break;
    }
    case TAG_TYPE_AS_SetEnumerationExpr : {
      return ClModNmInExprList(e.GetSequence(pos_AS_SetEnumerationExpr_els));
      break;
    }
    case TAG_TYPE_AS_SetComprehensionExpr : {
      const Generic & pred (e.GetField(pos_AS_SetComprehensionExpr_pred));
      SET<TYPE_AS_Name> nms;
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_SetComprehensionExpr_elem)));
      nms.ImpUnion(ClModNmInMultBindList(e.GetSequence(pos_AS_SetComprehensionExpr_bind)));
      if (!pred.IsNil())
        nms.ImpUnion(ClModNmInExpr(pred));
      return nms;
      break;
    }
    case TAG_TYPE_AS_SetRangeExpr : {
      SET<TYPE_AS_Name> nms;
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_SetRangeExpr_lb)));
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_SetRangeExpr_ub)));
      return nms;
      break;
    }
    case TAG_TYPE_AS_SeqEnumerationExpr : {
      return ClModNmInExprList(e.GetSequence(pos_AS_SeqEnumerationExpr_els));
      break;
    }
    case TAG_TYPE_AS_SeqComprehensionExpr : {
      const Generic & pred (e.GetField(pos_AS_SeqComprehensionExpr_pred));
      SET<TYPE_AS_Name> nms;
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_SeqComprehensionExpr_elem)));
      nms.ImpUnion(ClModNmInPatternBind(e.GetRecord(pos_AS_SeqComprehensionExpr_bind)));
      if (!pred.IsNil())
        nms.ImpUnion(ClModNmInExpr(pred));
      return nms;
      break;
    }
    case TAG_TYPE_AS_SubSequenceExpr : {
      SET<TYPE_AS_Name> nms;
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_SubSequenceExpr_sequence)));
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_SubSequenceExpr_frompos)));
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_SubSequenceExpr_topos)));
      return nms;
      break;
    }
    case TAG_TYPE_AS_SeqModifyMapOverrideExpr : {
      SET<TYPE_AS_Name> nms;
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_SeqModifyMapOverrideExpr_seqmap)));
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_SeqModifyMapOverrideExpr_mapexp)));
      return nms;
      break;
    }
    case TAG_TYPE_AS_MapEnumerationExpr : {
      const SEQ<TYPE_AS_Maplet> & els (e.GetSequence(pos_AS_MapEnumerationExpr_els));
      SET<TYPE_AS_Name> nms;
      size_t len_els = els.Length();
      for (size_t i = 1; i <= len_els; i++) {
        const TYPE_AS_Maplet & maplet (els[i]); 
        nms.ImpUnion(ClModNmInExpr(maplet.GetRecord(pos_AS_Maplet_mapdom)));
        nms.ImpUnion(ClModNmInExpr(maplet.GetRecord(pos_AS_Maplet_maprng)));
      }
      return nms;
      break;
    }
    case TAG_TYPE_AS_MapComprehensionExpr : {
      const Generic & pred (e.GetField(pos_AS_MapComprehensionExpr_pred));
      SET<TYPE_AS_Name> nms;
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_MapComprehensionExpr_elem)));
      nms.ImpUnion(ClModNmInMultBindList(e.GetSequence(pos_AS_MapComprehensionExpr_bind)));
      if (!pred.IsNil())
        nms.ImpUnion(ClModNmInExpr(pred));
      return nms;
      break;
    }
    case TAG_TYPE_AS_TupleConstructorExpr : {
      return ClModNmInExprList(e.GetSequence(pos_AS_TupleConstructorExpr_fields));
      break;
    }
    case TAG_TYPE_AS_RecordConstructorExpr : {
      return ClModNmInExprList(e.GetSequence(pos_AS_RecordConstructorExpr_fields));
      break;
    }
    case TAG_TYPE_AS_RecordModifierExpr : {
      const SEQ<TYPE_AS_RecordModification> & modifiers (e.GetSequence(pos_AS_RecordModifierExpr_modifiers));
      SET<TYPE_AS_Name> nms;
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_RecordModifierExpr_rec)));
      size_t len_modifiers = modifiers.Length();
      for (size_t i = 1; i <= len_modifiers; i++) {
        nms.ImpUnion(ClModNmInExpr(modifiers[i].GetRecord(pos_AS_RecordModification_newexpr)));
      }
      return nms;
      break;
    }
    case TAG_TYPE_AS_TokenConstructorExpr : {
      return ClModNmInExpr(e.GetSequence(pos_AS_TokenConstructorExpr_field));
      break;
    }
    case TAG_TYPE_AS_TupleSelectExpr : {
      return ClModNmInExpr(e.GetSequence(pos_AS_TupleSelectExpr_tuple));
      break;
    }
    case TAG_TYPE_AS_TypeJudgementExpr : {
      return ClModNmInExpr(e.GetSequence(pos_AS_TypeJudgementExpr_expr));
      break;
    }
    case TAG_TYPE_AS_PreConditionApplyExpr : {
      SET<TYPE_AS_Name> nms;
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_PreConditionApplyExpr_fct)));
      nms.ImpUnion(ClModNmInExprList(e.GetSequence(pos_AS_PreConditionApplyExpr_arg)));
      return nms;
      break;
    }
    case TAG_TYPE_AS_ApplyExpr : {
      SET<TYPE_AS_Name> nms;
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_ApplyExpr_fct)));
      nms.ImpUnion(ClModNmInExprList(e.GetSequence(pos_AS_ApplyExpr_arg)));
      return nms;
      break;
    }
    case TAG_TYPE_AS_LambdaExpr : {
      SET<TYPE_AS_Name> nms;
      nms.ImpUnion(ClModNmInMultBindList(e.GetSequence(pos_AS_LambdaExpr_parm)));
      nms.ImpUnion(ClModNmInExpr(e.GetRecord(pos_AS_LambdaExpr_body)));
      return nms;
      break;
    }
    case TAG_TYPE_AS_FieldSelectExpr : {
      return ClModNmInExpr(e.GetSequence(pos_AS_FieldSelectExpr_rec));
      break;
    }
    case TAG_TYPE_AS_FctTypeInstExpr : {
      return SET<TYPE_AS_Name>();
      break;
    }
    case TAG_TYPE_AS_IsExpr : {
      return ClModNmInExpr(e.GetSequence(pos_AS_IsExpr_arg));
      break;
    }
    case TAG_TYPE_AS_NarrowExpr : {
      return ClModNmInExpr(e.GetSequence(pos_AS_NarrowExpr_expr));
      break;
    }
    case TAG_TYPE_AS_UndefinedExpr : {
      return SET<TYPE_AS_Name>();
      break;
    }
    case TAG_TYPE_AS_BracketedExpr : {
      return ClModNmInExpr(e.GetSequence(pos_AS_BracketedExpr_expr));
      break;
    }
#ifdef VDMPP
    case TAG_TYPE_AS_IsOfClassExpr : {
      return ClModNmInExpr(e.GetSequence(pos_AS_IsOfClassExpr_arg));
      break;
    }
    case TAG_TYPE_AS_IsOfBaseClassExpr : {
      return ClModNmInExpr(e.GetSequence(pos_AS_IsOfBaseClassExpr_arg));
      break;
    }
    case TAG_TYPE_AS_SameBaseClassExpr : {
      SET<TYPE_AS_Name> nms;
      return ClModNmInExpr(e.GetSequence(pos_AS_SameBaseClassExpr_expr1));
      return ClModNmInExpr(e.GetSequence(pos_AS_SameBaseClassExpr_expr2));
      return nms;
      break;
    }
    case TAG_TYPE_AS_SameClassExpr : {
      SET<TYPE_AS_Name> nms;
      return ClModNmInExpr(e.GetSequence(pos_AS_SameClassExpr_expr1));
      return ClModNmInExpr(e.GetSequence(pos_AS_SameClassExpr_expr2));
      return nms;
      break;
    }
#endif // VDMPP
    default : {
      return SET<TYPE_AS_Name>();
      break;
    }
  }
  return SET<TYPE_AS_Name>(); // dummy
}
