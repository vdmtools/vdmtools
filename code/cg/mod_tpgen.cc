/***
*  * WHAT
*  *    VDM Code Generator.
*  *    mod_tpgen.cc: Implementation of mod_tpgen.vdm 1.54
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_tpgen.cc,v $
*  * VERSION
*  *    $Revision: 1.40 $
*  * DATE
*  *    $Date: 2006/03/17 08:29:12 $
*  * STATUS
*  *    Under development
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "vdmcg.h"
#include "astaux.h"
#include "statsem.h"
#include "BC.h"

// InitState_TPGEN (InitState)
// nm : [AS`Name]
// ==> ()
void vdmcg::InitState_TPGEN(const Generic & nm)
{
  this->cppenv = Map();

  if (nm.IsNil()) {
    this->known_types = Map();
    //this->invariant_types.Clear();
  }

  this->known_union_types = SET<TYPE_REP_TypeNameRep>();
  this->union_types = Map();
  this->record_types = Map();

#ifdef VDMSL
  if (!nm.IsNil()) {
    this->imported_types = SET<TYPE_AS_Name>();
  }
#endif //VDMSL

#ifdef VDMPP
  if (nm.IsNil()) {
    this->inh_tree = Map();
  }

  this->obj_refs = SET<TYPE_AS_Name>();

  if (nm.IsNil()) {
    this->classTypes = SET<TYPE_AS_Name>();
  }
#endif //VDMPP

  this->anonym_decl = SEQ<TYPE_CPP_Identifier>();
  this->anonym_comp_decl = SEQ<TYPE_CPP_Identifier>();
  this->mod_decl = SEQ<TYPE_CPP_Identifier>();
  this->mod_fctdef = TYPE_CPP_CPPAS();

  if (vdm_CPP_isCPP()) {
    this->anonym_tpdefs = TYPE_CPP_CPPAS();
    this->anonym_tpdecl = TYPE_CPP_CPPAS();
    this->anonym_fctdef = TYPE_CPP_CPPAS();
  }
  if (nm.IsNil() ) {
    this->tag = 1;
  }
  this->quotes_l = SEQ<TYPE_CPP_Identifier>();
//  this->ifdefs = TYPE_CPP_CPPAS(); // not used
  this->modnm = nm;
  this->inclgh = SET<TYPE_AS_Name>();
  this->inclh = SET<TYPE_AS_Name>();
}

Map vdmcg::get_known_types ()
{
  return this->known_types;
}

Set vdmcg::get_known_union_types ()
{
  return this->known_union_types;
}

#ifdef VDMSL
Set vdmcg::get_imported_types ()
{
  return this->imported_types;
}
#endif // VDMSL

// new_tag
// ==> CPP`IntegerLit
TYPE_CPP_IntegerLit vdmcg::new_tag()
{
  int t = this->tag;
  this->tag ++;
  return vdm_BC_GenIntegerLit(t);
}

// AddInclGH
// nm : AS`Name
// ==> ()
void vdmcg::AddInclGH(const TYPE_AS_Name & nm)
{
  this->inclgh.Insert(nm);
}

// AddInclH
// nm : AS`Name
// ==> ()
void vdmcg::AddInclH(const TYPE_AS_Name & nm)
{
  this->inclh.Insert(nm);
}

// DeclModCppTp
// id : Identifier
// cpptp : CPP`CPPAS
// ==> ()
void vdmcg::DeclModCppTp(const TYPE_AS_Id & id, const TYPE_CPP_CPPAS & cpptp)
{
  AddNewCppTp(id, cpptp);
  this->mod_decl.ImpAppend(Id2CppGTpId(id));
}

// DeclAnonymCppTp
// id : Identifier
// cpptp : CPP`CPPAS
// ==> ()
void vdmcg::DeclAnonymCppTp(const TYPE_AS_Id & id, const TYPE_CPP_CPPAS & cpptp)
{
  AddNewCppTp(id, Sequence());
  this->anonym_decl.ImpAppend(Id2CppGTpId(id));
  if (vdm_CPP_isCPP()) {
    this->anonym_tpdecl.ImpConc( GenTypeDeclIfDef (Id2CppGTpId(id), cpptp));
  }
}

// AddNewCppTp
// id : Identifier
// for VDMSL
// cpptp : CPP`CPPAS
// for VDMPP
// cpptp : (CPP`CPPAS | seq of CPP`MemberDeclaration)
// ==> ()
void vdmcg::AddNewCppTp(const TYPE_AS_Id & id, const Sequence &cpptp)
{
  this->cppenv.ImpModify(id, cpptp);
}

#ifdef VDMPP
// AddClassTypes
// cs : AS`Class
// ==> ()
void vdmcg::AddClassTypes(const TYPE_AS_Class & cs)
{
  const TYPE_AS_Name & nm (cs.GetRecord(pos_AS_Class_nm));
  const Generic & defs (cs.GetField(pos_AS_Class_defs));

  if (!defs.IsNil()) {
    const SEQ<TYPE_AS_Name> & inh (cs.GetSequence(pos_AS_Class_supercls));
//    const Bool & sys (cs.GetBool(pos_AS_Class_sys)); // 20070227

    MAP<TYPE_AS_Name, TYPE_AS_TypeDef> tps (Record(defs).GetMap(pos_AS_Definitions_typem));
    this->typeDefs.ImpModify(nm, tps);

    SET<TYPE_AS_Name> dom_tps (tps.Dom());
    Generic tpd;
    for (bool bb = dom_tps.First(tpd); bb; bb = dom_tps.Next(tpd)) {
      const TYPE_AS_TypeDef & tps_tpd (tps[tpd]);
      this->known_types.ImpModify(GenQName2(nm, tps_tpd.GetRecord(pos_AS_TypeDef_nm)),
                                  GenQComp( nm, tps_tpd.GetRecord(pos_AS_TypeDef_shape)));
      const TYPE_AS_Type & shape (tps_tpd.GetRecord(pos_AS_TypeDef_shape));
      if (shape.Is(TAG_TYPE_AS_CompositeType)) {
        const TYPE_AS_Name & tag (shape.GetRecord(pos_AS_CompositeType_name));
        if (!this->known_types.DomExists(GenQName2(nm, tag))) {
          this->known_types.ImpModify(GenQName2(nm, tag), GenQComp(nm, shape));
        }
      }
    }
    this->inh_tree.ImpModify(nm, inh.Elems());
  }
  this->classTypes.Insert(nm);
}
#endif //VDMPP

#ifdef VDMSL
// AddModuleTypes
// md : AS`Module
void vdmcg::AddModuleTypes(const TYPE_AS_Module & md)
{
  const TYPE_AS_Name & mnm (md.GetRecord(pos_AS_Module_nm));
  const TYPE_AS_Interface & intf (md.GetRecord(pos_AS_Module_intf));

  if (! md.GetField(pos_AS_Module_defs).IsNil()) {
    const TYPE_AS_Definitions & defs (md.GetRecord(pos_AS_Module_defs));
    const Map & tpm (defs.GetMap(pos_AS_Definitions_typem));
    const Generic & state (defs.GetField(pos_AS_Definitions_State));

    this->typeDefs.ImpModify(mnm, tpm);

    MAP<TYPE_AS_Name,TYPE_AS_Type> env (ModuleTypes2TpEnv(mnm, tpm));
    SET<TYPE_AS_Name> dom_env (env.Dom()); 
    Generic n;
    for (bool bb = dom_env.First(n); bb; bb = dom_env.Next(n)) {
      this->known_types.ImpModify(GenQName2(mnm, n), env[n]);
    }

    if (! state.IsNil()) {
      TYPE_AS_CompositeType tp (Record(state).GetRecord(pos_AS_StateDef_tp));
      const TYPE_AS_Name & n (tp.GetRecord(pos_AS_CompositeType_name));
      tp.SetField(pos_AS_CompositeType_name, GenQName2(mnm, n));
      this->known_types.ImpModify(GenQName2(mnm, n), tp);
    }
  }

  const Map & imp (intf.GetMap(pos_AS_Interface_imp));
  Set dom_imp (imp.Dom());
  Generic n;
  for (bool bb = dom_imp.First(n); bb; bb = dom_imp.Next(n)) {
    if (!imp[n].IsNil()) {
      const TYPE_AS_ImportSig & is (imp[n]);
      const Map & tpm (is.GetMap(pos_AS_ImportSig_tps));
      const Map & ren (is.GetMap(pos_AS_ImportSig_ren));
      this->known_types.ImpOverride(GetImportetTypes(mnm, n, tpm, ren));
    }
  }
}

// GetImportetTypes
// mnm : AS`Name
// impmnm : AS`Name
// tpm : map AS`Name to [AS`TypeDef]
// renm : map AS`Name to AS`Name
// ==> TpEnv (map AS`Name to AS`Type)
Map vdmcg::GetImportetTypes(const TYPE_AS_Name & mnm,
                            const TYPE_AS_Name & impmnm,
                            const Map & tpm,
                            const MAP<TYPE_AS_Name,TYPE_AS_Name> & renm)
{
  Map rename (renm.Inverse()); // map AS`Name to AS`Name

  MAP<TYPE_AS_Name, TYPE_AS_Type> env;
  SET<TYPE_AS_Name> dom_tpm (tpm.Dom());
  Generic n;
  for (bool bb = dom_tpm.First(n); bb; bb = dom_tpm.Next(n)) {
    TYPE_AS_Name qnm (rename.DomExists(n) ? GenQReName2(mnm, rename[n]) : GenQReName2(mnm, n));

    if (!tpm[n].IsNil()) {
      const TYPE_AS_TypeDef & td (tpm[n]);
      TYPE_AS_Type qtp (GenQType(mnm, td.GetRecord(pos_AS_TypeDef_shape)));
      if (qtp.Is(TAG_TYPE_AS_CompositeType)) {
        env.ImpModify(qnm, TYPE_AS_CompositeType().Init(GenQReName2(impmnm, n),
                                                        qtp.GetSequence(pos_AS_CompositeType_fields),
                                                        qtp.GetInt(pos_AS_CompositeType_cid)));
        this->imported_types.Insert(qnm);
      }
      else {
        env.ImpModify(qnm, qtp);
      }
    }
    else {
      env.ImpModify(qnm, TYPE_AS_TypeName().Init(GenQReName2(impmnm, n), Record(n).GetInt(pos_AS_Name_cid)));
    }
  }
  return env;
}
#endif //VMDSL


//Following functions are not implemented:
//LOT
//LOTrec
//ReportErrorExpr
//ReportErrorStmt

#ifdef VDMPP
// LOTrec
// qnm : AS`Name
// left : set of AS`Name
// tried : set of AS`Name
// ==> [AS`Name]
Generic vdmcg::LOTrec(const TYPE_AS_Name & qnm, const SET<TYPE_AS_Name> & left, const SET<TYPE_AS_Name> & tried)
{
  if (this->known_types.DomExists(qnm)) return qnm;

  if (left.Diff(tried).IsEmpty()) return Nil();

  TYPE_AS_Name nm (ASTAUX::MkNameFromId(qnm.get_ids()[1], NilContextId));
  SET<TYPE_AS_Name> left_q (left);
  Generic n;
  for(bool bb = left_q.First(n); bb; bb = left_q.Next(n)) {
    Generic t (LOTrec(GenQReName2(n, qnm), Set(this->inh_tree[n]).Diff(tried), tried.Union(mk_set(nm))));

    if(!t.IsNil()) return t;
  }
  return Nil();
}

// isclassType
// nm : AS`Name
// ==> bool
bool vdmcg::isclassType(const TYPE_AS_Name & nm)
{
  if (this->known_types.DomExists(nm)) {
    return false;
  }
  TYPE_AS_Name uqnm ((nm.GetSequence(pos_AS_Name_ids).Length() == 2) ? ASTAUX::GetSecondName(nm) : nm);
  
  if (uqnm != this->modnm && !this->obj_refs.InSet(uqnm)) {
    this->obj_refs.Insert(uqnm);
  }
  return this->classTypes.InSet(uqnm);
}

// GenQualifiedTypeName
// tpnm : AS`Name
// ==> [AS`Name]
Generic vdmcg::GenQualifiedTypeName(const TYPE_AS_Name & qnm)
{
  Tuple t (GetStatSem().PublicLookUpTypeName (qnm, GiveCurCASName()));
  const Generic & DefiningClass (t.GetField(2));

  if (DefiningClass.IsNil())
    return qnm;   // some kind of error
  else {
    IncludeClass(DefiningClass);
    return ASTAUX::Combine2Names(DefiningClass, qnm);
  }
}

// GenRootType
// tpnm : AS`Name
// ==> [AS`Name] | REP`TypeRep
Generic vdmcg::GenRootType(const TYPE_AS_Name & qnm)
{
  Tuple t (GetStatSem().PublicLookUpTypeName (qnm, GiveCurCASName()));
  const Generic & DefiningClass (t.GetField(2));

  if (DefiningClass.IsNil()) {
    return qnm;   // some kind of error
  }
  else {
    TYPE_REP_TypeNameRep rtp;
    rtp.Init(qnm);
    TYPE_REP_TypeRep tp (LOT(rtp));

    if (vdm_CPP_isJAVA().GetValue() && !tp.Is(TAG_TYPE_REP_TypeNameRep)) {
      if(!tp.Is(TAG_TYPE_REP_UnionTypeRep)) {
        return tp;
      }
      else {
        SET<TYPE_REP_TypeRep> s (tp.GetSet(pos_REP_UnionTypeRep_tps));
        if (s.Card() == 2) {
          TYPE_REP_TypeRep e1 (s.GetElem());
          s.RemElem(e1);
          TYPE_REP_TypeRep e2 (s.GetElem());
          if ((e1.Is(TAG_TYPE_REP_SeqTypeRep) && e2.Is(TAG_TYPE_REP_EmptySeqTypeRep)) ||
              (e2.Is(TAG_TYPE_REP_SeqTypeRep) && e1.Is(TAG_TYPE_REP_EmptySeqTypeRep))) {
            return tp;
          }
          if ((e1.Is(TAG_TYPE_REP_SetTypeRep) && e2.Is(TAG_TYPE_REP_EmptySetTypeRep)) ||
              (e2.Is(TAG_TYPE_REP_SetTypeRep) && e1.Is(TAG_TYPE_REP_EmptySetTypeRep))) {
            return tp;
          }
          if ((e1.Is(TAG_TYPE_REP_GeneralMapTypeRep) && e2.Is(TAG_TYPE_REP_EmptyMapTypeRep)) ||
              (e2.Is(TAG_TYPE_REP_GeneralMapTypeRep) && e1.Is(TAG_TYPE_REP_EmptyMapTypeRep))) {
            return tp;
          }
          if ((e1.Is(TAG_TYPE_REP_InjectiveMapTypeRep) && e2.Is(TAG_TYPE_REP_EmptyMapTypeRep)) ||
              (e2.Is(TAG_TYPE_REP_InjectiveMapTypeRep) && e1.Is(TAG_TYPE_REP_EmptyMapTypeRep))) {
            return tp;
          }
        }
      }
    }
    return ASTAUX::Combine2Names(DefiningClass, qnm);
  }
}
#endif // VDMPP

// GenQType
// mnm : AS`Name
// tp : AS`Type
// -> AS`Type
TYPE_AS_Type vdmcg::GenQType(const TYPE_AS_Name & mnm, const TYPE_AS_Type & tp)
{
  switch (tp.GetTag()) {
    case TAG_TYPE_AS_BracketedType: {
      TYPE_AS_BracketedType res (tp);
      res.SetField(pos_AS_BracketedType_tp, GenQType(mnm, tp.GetRecord(pos_AS_BracketedType_tp)));
      return res;
    }
    case TAG_TYPE_AS_OptionalType: {
      TYPE_AS_OptionalType res (tp);
      res.SetField(pos_AS_OptionalType_tp, GenQType(mnm, tp.GetRecord(pos_AS_OptionalType_tp)));
      return res;
    }
    case TAG_TYPE_AS_Set0Type: {
      TYPE_AS_Set0Type res (tp);
      res.SetField(pos_AS_Set0Type_elemtp, GenQType(mnm, tp.GetRecord(pos_AS_Set0Type_elemtp)));
      return res;
    }
    case TAG_TYPE_AS_Set1Type: {
      TYPE_AS_Set1Type res (tp);
      res.SetField(pos_AS_Set1Type_elemtp, GenQType(mnm, tp.GetRecord(pos_AS_Set1Type_elemtp)));
      return res;
    }
    case TAG_TYPE_AS_Seq1Type: {
      TYPE_AS_Seq1Type res (tp);
      res.SetField(pos_AS_Seq1Type_elemtp, GenQType(mnm, tp.GetRecord(pos_AS_Seq1Type_elemtp)));
      return res;
    }
    case TAG_TYPE_AS_Seq0Type: {
      TYPE_AS_Seq0Type res (tp);
      res.SetField(pos_AS_Seq0Type_elemtp, GenQType(mnm, tp.GetRecord(pos_AS_Seq0Type_elemtp)));
      return res;
    }
    case TAG_TYPE_AS_CompositeType: {
      TYPE_AS_CompositeType res (tp);
      res.SetField(pos_AS_CompositeType_name, GenQName2(mnm, tp.GetRecord(pos_AS_CompositeType_name)));
      res.SetField(pos_AS_CompositeType_fields, GenQFieldSeq(mnm, tp.GetSequence(pos_AS_CompositeType_fields)));
      return res;
    }
    case TAG_TYPE_AS_UnionType: {
      TYPE_AS_UnionType res (tp);
      res.SetField(pos_AS_UnionType_tps, GenQTypeSeq(mnm, tp.GetSequence(pos_AS_UnionType_tps)));
      return res;
    }
    case TAG_TYPE_AS_ProductType: {
      TYPE_AS_ProductType res (tp);
      res.SetField(pos_AS_ProductType_tps, GenQTypeSeq(mnm, tp.GetSequence(pos_AS_ProductType_tps)));
      return res;
    }
    case TAG_TYPE_AS_GeneralMap0Type: {
      TYPE_AS_GeneralMap0Type res (tp);
      res.SetField(pos_AS_GeneralMap0Type_mapdom, GenQType(mnm, tp.GetRecord(pos_AS_GeneralMap0Type_mapdom)));
      res.SetField(pos_AS_GeneralMap0Type_maprng, GenQType(mnm, tp.GetRecord(pos_AS_GeneralMap0Type_maprng)));
      return res;
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      TYPE_AS_GeneralMap1Type res (tp);
      res.SetField(pos_AS_GeneralMap1Type_mapdom, GenQType(mnm, tp.GetRecord(pos_AS_GeneralMap1Type_mapdom)));
      res.SetField(pos_AS_GeneralMap1Type_maprng, GenQType(mnm, tp.GetRecord(pos_AS_GeneralMap1Type_maprng)));
      return res;
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      TYPE_AS_InjectiveMap0Type res (tp);
      res.SetField(pos_AS_InjectiveMap0Type_mapdom, GenQType(mnm, tp.GetRecord(pos_AS_InjectiveMap0Type_mapdom)));
      res.SetField(pos_AS_InjectiveMap0Type_maprng, GenQType(mnm, tp.GetRecord(pos_AS_InjectiveMap0Type_maprng)));
      return res;
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      TYPE_AS_InjectiveMap1Type res (tp);
      res.SetField(pos_AS_InjectiveMap1Type_mapdom, GenQType(mnm, tp.GetRecord(pos_AS_InjectiveMap1Type_mapdom)));
      res.SetField(pos_AS_InjectiveMap1Type_maprng, GenQType(mnm, tp.GetRecord(pos_AS_InjectiveMap1Type_maprng)));
      return res;
    }
    case TAG_TYPE_AS_TypeName: {
      TYPE_AS_TypeName res (tp);
      res.SetField(pos_AS_TypeName_name, GenQName2(mnm, tp.GetRecord(pos_AS_TypeName_name)));
      return res;
    }
    case TAG_TYPE_AS_BooleanType:
    case TAG_TYPE_AS_NumericType:
    case TAG_TYPE_AS_TokenType:
    case TAG_TYPE_AS_CharType:
    case TAG_TYPE_AS_VoidType:
    case TAG_TYPE_AS_QuoteType: {
      return tp;
    }
    case TAG_TYPE_AS_PartialFnType: {
      TYPE_AS_PartialFnType res (tp);
      res.SetField(pos_AS_PartialFnType_fndom, GenQTypeSeq(mnm, tp.GetSequence(pos_AS_PartialFnType_fndom)));
      res.SetField(pos_AS_PartialFnType_fnrng, GenQType(mnm, tp.GetRecord(pos_AS_PartialFnType_fnrng)));
      return res;
    }
    case TAG_TYPE_AS_TotalFnType: {
      TYPE_AS_TotalFnType res (tp);
      res.SetField(pos_AS_TotalFnType_fndom, GenQTypeSeq(mnm, tp.GetSequence(pos_AS_TotalFnType_fndom)));
      res.SetField(pos_AS_TotalFnType_fnrng, GenQType(mnm, tp.GetRecord(pos_AS_TotalFnType_fnrng)));
      return res;
    }
    case TAG_TYPE_AS_OpType: {
      TYPE_AS_OpType res (tp);
      res.SetField(pos_AS_OpType_opdom, GenQTypeSeq(mnm, tp.GetSequence(pos_AS_OpType_opdom)));
      res.SetField(pos_AS_OpType_oprng, GenQType(mnm, tp.GetRecord(pos_AS_OpType_oprng)));
      return res;
    }
    case TAG_TYPE_AS_TypeVar:
      return tp;
    default:
      ReportError(L"GenQType");
  }
  return tp; // To keep VC++ happy.
}

// GenQTypeSeq
// mnm : AS`Name
// tpl : seq of AS`Type
// -> seq of AS`Type
SEQ<TYPE_AS_Type> vdmcg::GenQTypeSeq(const TYPE_AS_Name & mnm, const SEQ<TYPE_AS_Type> & tpl)
{
  SEQ<TYPE_AS_Type> tp_l;
  size_t len_tpl = tpl.Length();
  for (size_t i = 1; i <= len_tpl; i++) {
    tp_l.ImpAppend(GenQType(mnm, tpl[i]));
  }
  return tp_l;
}

// GenQFieldSeq
// mnm : AS`Name
// fl : seq of AS`Field
// -> seq of AS`Field
//Sequence
SEQ<TYPE_AS_Field> vdmcg::GenQFieldSeq(const TYPE_AS_Name & mnm, const SEQ<TYPE_AS_Field> & fl)
{
  SEQ<TYPE_AS_Field> f_l;
  size_t len_fl = fl.Length();
  for (size_t i = 1; i <= len_fl; i++)
  {
    const TYPE_AS_Field & f (fl[i]);
    f_l.ImpAppend(TYPE_AS_Field().Init(f.GetField(pos_AS_Field_sel),
                                       GenQType(mnm, f.GetRecord(pos_AS_Field_type)),
                                       f.GetBool(pos_AS_Field_dc),
                                       f.GetInt(pos_AS_Field_cid)));
  }
  return f_l;
}

// IsTpRecursive
// nm : AS`Name
// tp : REP`TypeRep
// names : set of AS`Name
// +> bool
bool vdmcg::IsTpRecursive(const TYPE_AS_Name & nm, const TYPE_REP_TypeRep & tp, const SET<TYPE_AS_Name> & names)
{
  switch (tp.GetTag()) {

    case TAG_TYPE_REP_SetTypeRep: {
      return IsTpRecursive(nm, tp.GetRecord(pos_REP_SetTypeRep_elemtp), names);
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return IsTpRecursive(nm, tp.GetRecord(pos_REP_SeqTypeRep_elemtp), names);
    }

    case TAG_TYPE_REP_EmptySetTypeRep :
    case TAG_TYPE_REP_EmptySeqTypeRep :
    case TAG_TYPE_REP_EmptyMapTypeRep :  {
      return false;
    }

    case TAG_TYPE_REP_CompositeTypeRep : {
      return false;
    }

    case TAG_TYPE_REP_UnionTypeRep : {
      SET<TYPE_REP_TypeRep> ts (tp.GetSet(pos_REP_UnionTypeRep_tps));
      bool exists = false;
      Generic t;
      for (bool bb = ts.First(t); bb && !exists; bb = ts.Next(t)) {
        exists = IsTpRecursive(nm, t, names);
      };
      return exists;
    }

    case TAG_TYPE_REP_ProductTypeRep : {
      const SEQ<TYPE_REP_TypeRep> & ts (tp.GetSequence(pos_REP_ProductTypeRep_tps));
      bool exists = false;
      size_t len_ts = ts.Length();
      for (size_t idx = 1; (idx <= len_ts) && !exists; idx++) {
        exists = IsTpRecursive(nm, ts[idx], names);
      };
      return exists;
    }

    case TAG_TYPE_REP_GeneralMapTypeRep : {
      return (IsTpRecursive(nm, tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom), names) ||
              IsTpRecursive(nm, tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng), names) );
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep : {
      return (IsTpRecursive(nm, tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom), names) ||
              IsTpRecursive(nm, tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng), names) );
    }

    case TAG_TYPE_REP_TypeNameRep : {
      TYPE_AS_Name n (tp.GetRecord(pos_REP_TypeNameRep_nm));

      if ( n == nm || names.InSet(n) )
        return true;

      Generic lot_tp (LOT(tp));
      if (lot_tp.IsNil())
        return false;

      SET<TYPE_AS_Name> res_names (names);
      return IsTpRecursive(nm, lot_tp, res_names.Insert(n));
    }

    case TAG_TYPE_REP_PartialFnTypeRep : {
      const SEQ<TYPE_REP_TypeRep> & fndom (tp.GetSequence(pos_REP_PartialFnTypeRep_fndom));
      const TYPE_REP_TypeRep & fnrng (tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng));
      bool exists = false;
      size_t len_fndom = fndom.Length();
      for (size_t idx = 1; (idx <= len_fndom) && !exists; idx++) {
        exists = IsTpRecursive(nm, fndom[idx], names);
      };
      return exists || IsTpRecursive(nm, fnrng, names);
    }
    case TAG_TYPE_REP_TotalFnTypeRep : {
      const SEQ<TYPE_REP_TypeRep> & fndom (tp.GetSequence(pos_REP_TotalFnTypeRep_fndom));
      const TYPE_REP_TypeRep & fnrng (tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng));
      bool exists = false;
      size_t len_fndom = fndom.Length();
      for (size_t idx = 1; (idx <= len_fndom) && !exists; idx++) {
        exists = IsTpRecursive(nm, fndom[idx], names);
      };
      return exists || IsTpRecursive(nm, fnrng, names);
    }
    case TAG_TYPE_REP_OpTypeRep : {
      const SEQ<TYPE_REP_TypeRep> & opdom (tp.GetSequence(pos_REP_OpTypeRep_Dom));
      const TYPE_REP_TypeRep & oprng (tp.GetRecord(pos_REP_OpTypeRep_Rng));
      bool exists = false;
      size_t len_opdom = opdom.Length();
      for (size_t idx = 1; (idx <= len_opdom) && !exists; idx++) {
        exists = IsTpRecursive(nm, opdom[idx], names);
      };
      return exists || IsTpRecursive(nm, oprng, names);
    }
#ifdef VDMPP
    case TAG_TYPE_REP_ObjRefTypeRep :
#endif // VDMPP
    case TAG_TYPE_REP_TypeParRep:
    case TAG_TYPE_REP_TypeVarRep :
    case TAG_TYPE_REP_BooleanTypeRep :
    case TAG_TYPE_REP_NumericTypeRep :
    case TAG_TYPE_REP_TokenTypeRep :
    case TAG_TYPE_REP_CharTypeRep :
    case TAG_TYPE_REP_QuoteTypeRep :
    case TAG_TYPE_REP_NilTypeRep :
    case TAG_TYPE_REP_UnitTypeRep : {
        return false;
    }

    case TAG_TYPE_REP_InvTypeRep: {
      return IsTpRecursive(nm, tp.GetRecord(pos_REP_InvTypeRep_shape), names);
    }

    default:
      vdmcg::ReportUndefined(L"IsTpRecursive");
  }
  return false; // Should never be reached
}

//////////////////////////////////
/// CONVERSION FUNCTIONS
//////////////////////////////////

// ConcatIds
// il : seq of AS`Id
// -> Identifier
TYPE_AS_Id vdmcg::ConcatIds(const TYPE_AS_Ids & il)
{
  switch(il.Length()) {
    case 0:
      return TYPE_AS_Id();
    case 1:
      return CleanIdentifier(il.Hd());
    default: { // il.Length() > 1
      TYPE_AS_Id res (CleanIdentifier(il.Hd()));
      res.ImpConc(ASTAUX::MkId(L"_"));
      res.ImpConc(ConcatIds(il.Tl()));
      return res;
    }
  }
}

// Name2Id
// nm : AS`Name
// -> Identifier
TYPE_AS_Id vdmcg::Name2Id(const TYPE_AS_Name & nm)
{
  return ConcatIds(nm.get_ids());
}

// ModuleTypes2TpEnv
// mdnm : AS`Name
// tps : map AS`Name to AS`TypeDef
// +> TpEnv (map AS`Name to AS`Type)
Map vdmcg::ModuleTypes2TpEnv(const TYPE_AS_Name & mdnm, const MAP<TYPE_AS_Name,TYPE_AS_TypeDef> & tps)
{
  MAP<TYPE_AS_Name,TYPE_AS_Type> res;
  Set dom_tps (tps.Dom());
  Generic id;
  for (bool bb = dom_tps.First(id); bb; bb = dom_tps.Next(id))
  {
    TYPE_AS_TypeDef td (tps[id]);
// /ModuleTypes2TpEnv

    res.ImpModify(id, GenQComp(mdnm, td.get_shape()));

// 20110607 -->
    const TYPE_AS_Type & shape (td.GetRecord(pos_AS_TypeDef_shape));
    if (shape.Is(TAG_TYPE_AS_CompositeType))
    {
      const TYPE_AS_Name & tag (shape.GetRecord(pos_AS_CompositeType_name));
      if (!res.DomExists(tag)) 
      {
        res.ImpModify(tag, GenQComp(mdnm, shape));
      }
    }
// <-- 20110607
  }
  return res;
}

// Id2CppGTpId
// i : Identifier
// -> CPP`Identifier
TYPE_CPP_Identifier vdmcg::Id2CppGTpId(const TYPE_AS_Id & i)
{
  wstring str (i.GetString());
  if (str.compare(L"i") == 0 )
    return GenIntType().get_tp();
  else if (str.compare(L"b") == 0)
    return GenBoolType().get_tp();
  else if (str.compare(L"r") == 0)
    return GenRealType().get_tp();
  else if (str.compare(L"c") == 0)
    return GenCharType().get_tp();
  else if (str.compare(L"t") == 0)
    return GenTokenType().get_tp();
  else if (str.compare(L"s") == 0)
    return GenSetType().get_tp();
  else if (str.compare(L"l") == 0)
    return GenSeq0Type().get_tp();
  else if (str.compare(L"m") == 0)
    return GenMapType().get_tp();
  else if (str.compare(L"p") == 0)
    return GenProductType().get_tp();
  else if (str.compare(L"d") == 0)
    return GenRecordType(Nil()).get_tp();
  else if (str.compare(L"v") == 0)
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"void"));
  else if (str.compare(L"n") == 0)
    return GenRealType().get_tp();
  else if (str.compare(L"F") == 0 )
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"Generic"));
  else if (str.compare(L"Q") == 0)
    return GenQuoteType(Nil()).get_tp();
  else if (str.compare(L"U") == 0)
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"Generic"));
#ifdef VDMPP
  else if (str.compare(L"o") == 0)
    return GenObjRefType_DS(Nil()).get_tp();
#endif // VDMPP
  //  else if (str.compare(L"US") == 0)
  //    return GenSetType().get_tp();
  //  else if (str.compare(L"UL") == 0)
  //    return GenSeq0Type().get_tp();
  //  else if (str.compare(L"UUM") == 0)
  //    return GenMapType().get_tp();
  else if (str.compare(L"void") == 0)
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"void"));
  else if (str.compare(L"Set") == 0)
    return GenSetType().get_tp();
  else if (str.compare(L"Sequence") == 0)
    return GenSeq0Type().get_tp();
  else if (str.compare(L"Map") == 0)
    return GenMapType().get_tp();
  else if (str.compare(L"Record") == 0)
    return GenRecordType(Nil()).get_tp();
  else if (str.compare(L"Tuple") == 0)
    return GenProductType().get_tp();
  else if (str.compare(L"Generic") == 0)
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"Generic"));
#ifdef VDMPP
  else if (str.compare(L"ObjectRef") == 0)
    return GenObjRefType_DS(Nil()).get_tp();
#endif // VDMPP
  else if (str.compare(L"operation") == 0)
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"Generic"));
  else if (str.compare(L"typevariable") == 0)
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"Generic"));
  else if (str.compare(L"function") == 0)
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"Generic"));
  else if (str[str.size()-1] == L'C' || str[str.size()-1] == L'N') {
    return vdm_BC_GenIdentifier(PrefixModuleType(RemPrefixNum(ASTAUX::MkId(str.substr(0,str.size()-1)))));
  }
  else if (str[str.size()-1] == L'R') {
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"type_ref_").ImpConc(RemPrefixNum(ASTAUX::MkId(str.substr(0,str.size()-1)))));
  }
  else
    return vdm_BC_GenIdentifier(PrefixAnonymType(ASTAUX::MkId(str)));
}

#ifdef VDMPP
// Id2JavaGTpId
// i : Identifier
// ==> CPP`Identifier
TYPE_CPP_Identifier vdmcg::Id2JavaGTpId(const TYPE_AS_Id & i)
{
  wstring str (i.GetString());
  if (str.compare(L"i") == 0 ) {
    if (get_smalltypes_option()) {
      return GenSmallIntType().get_tp();
    }
    else {
      return GenIntType().get_tp();
    }
  }
  else if (str.compare(L"b") == 0)
    if (get_smalltypes_option())
      return GenSmallBoolType().get_tp();
    else
      return GenBoolType().get_tp();
  else if (str.compare(L"r") == 0)
    if (get_smalltypes_option())
      return GenSmallRealType().get_tp();
    else
      return GenRealType().get_tp();
  else if (str.compare(L"c") == 0)
    return GenCharType().get_tp();
  else if (str.compare(L"t") == 0)
    return GenTokenType().get_tp();
  else if (str.compare(L"s") == 0)
    return GenSetType().get_tp();
  else if (str.compare(L"l") == 0)
    return GenSeq0Type().get_tp();
  else if (str.compare(L"m") == 0)
    return GenMapType().get_tp();
  else if (str.compare(L"p") == 0)
    return GenProductType().get_tp();
  else if (str.compare(L"d") == 0)
    return GenRecordType(nil).get_tp();
  else if (str.compare(L"v") == 0)
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"void"));
  else if (str.compare(L"n") == 0)
    return GenNumType().get_tp();
  else if (str.compare(L"F") == 0 )
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"Object"));
  else if (str.compare(L"Q") == 0)
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"Quote"));
  else if (str.compare(L"U") == 0)
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"Object"));
  else if (str.compare(L"o") == 0)
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"Object"));
  //  else if (str.compare(L"US") == 0)
  //    return GenSetType().get_tp();
  //  else if (str.compare(L"UL") == 0)
  //    return GenSeq0Type().get_tp();
  //  else if (str.compare(L"UUM") == 0)
  //    return GenMapType().get_tp();
  else if (str.compare(L"void") == 0)
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"void"));
  else if (str.compare(L"Set") == 0)
    return GenSetType().get_tp();
  else if (str.compare(L"Sequence") == 0)
    return GenSeq0Type().get_tp();
  else if (str.compare(L"Map") == 0)
    return GenMapType().get_tp();
  else if (str.compare(L"Record") == 0)
    return GenRecordType(nil).get_tp();
  else if (str.compare(L"Tuple") == 0)
    return GenProductType().get_tp();
  else if (str.compare(L"Generic") == 0)
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"Object"));
  else if (str.compare(L"ObjectRef") == 0)
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"Object"));
  else if (str.compare(L"operation") == 0)
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"Object"));
  else if (str.compare(L"typevariable") == 0)
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"Object"));
  else if (str.compare(L"function") == 0)
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"Object"));
  else if (str[str.size()-1] == L'C') {
    return vdm_BC_GenIdentifier(ReplaceUnderscore(RemPrefixNum(ASTAUX::MkId(str.substr(0,str.size()-1)))));
  }
  else if (str[str.size()-1] == L'R') {
    return vdm_BC_GenIdentifier(RemPrefixNum(ASTAUX::MkId(str.substr(0,str.size()-1))));
  }
  else
    return vdm_BC_GenIdentifier(PrefixAnonymType(ASTAUX::MkId(str)));
}
#endif // VDMPP

// PrefixAnonymType
// id : Identifier
// -> Identifier
TYPE_AS_Id vdmcg::PrefixAnonymType(const TYPE_AS_Id & id)
{
  return ASTAUX::MkId(L"type_").ImpConc(id);
}

// PrefixModuleType
// id : Identifier
// -> Identifier
TYPE_AS_Id vdmcg::PrefixModuleType(const TYPE_AS_Id & id)
{
  return ASTAUX::MkId(L"TYPE_").ImpConc(id);
}

// RemPrefixNum
// i : Identifier
// +> Identifier
TYPE_AS_Id vdmcg::RemPrefixNum(const TYPE_AS_Id & i)
{
  wstring str (i.GetString());
  unsigned int pos = 0;
  while (pos < str.size() && str[pos] >= '0' && str[pos] <= '9')
    pos++;
  return ASTAUX::MkId(str.substr(pos,str.size()-pos));
}

// ReplaceUnderscore
// i : Identifier
// +> Identifier
TYPE_AS_Id vdmcg::ReplaceUnderscore(const TYPE_AS_Id & i)
{
  wstring res (i.GetString());
  unsigned int pos=0;
  while (pos<res.size() && res[pos] == L'_')
    pos++;
  pos--;
  res[pos] = L'.';
  return ASTAUX::MkId(res);
}

// Name2CppTpId
// tpnm : AS`Name
// -> CPP`Identifier
TYPE_CPP_Identifier vdmcg::Name2CppTpId(const TYPE_AS_Name& tpnm)
{
#ifdef VDMSL
  TYPE_AS_Name qtpnm(GenQName(tpnm));
#endif  // VDMSL
#ifdef VDMPP
  TYPE_AS_Name qtpnm(GenQualifiedTypeName(GenQName(tpnm)));
  if (vdm_CPP_isJAVA()) {
    return vdm_BC_GenIdentifier(Name2Id(qtpnm));
  }
  else
#endif // VDMPP
    return vdm_BC_GenIdentifier(ASTAUX::MkId(L"TYPE_").ImpConc(Name2Id(qtpnm)));
}

// GenQuoteId
// q : Identifier
// -> CPP`Identifier
TYPE_CPP_Identifier vdmcg::GenQuoteId(const TYPE_AS_Id & q)
{
  return vdm_BC_GenIdentifier(ASTAUX::MkId(L"quote_").ImpConc(q));
}

// GetQuoteId
// q : CPP`Identifier
// -> Identifier
TYPE_AS_Id vdmcg::GetQuoteId(const TYPE_CPP_Identifier& q)
{
  TYPE_AS_Id id (q.get_id());
  wstring str (id.GetString());
  return ASTAUX::MkId(str.substr(6, str.size()));
}

// GenTypeTagCppId
// nm : CPP`Identifier
// -> CPP`Identifier
TYPE_CPP_Identifier vdmcg::GenTypeTagCppId(const TYPE_CPP_Identifier &nm)
{
  return vdm_BC_GenIdentifier(ASTAUX::MkId(L"TAG_").ImpConc(nm.get_id()));
}

// GenAnonymTypeTag
// ==> CPP`Identifier
TYPE_CPP_Identifier vdmcg::GenAnonymTypeTag()
{
  return vdm_BC_GenIdentifier(ASTAUX::MkId(L"TAG_").ImpConc(Name2Id(this->modnm)));
}

// GenModuleTypeTag
// ==> CPP`Identifier
TYPE_CPP_Identifier vdmcg::GenModuleTypeTag()
{
  return vdm_BC_GenIdentifier(ASTAUX::MkId(L"TAG_").ImpConc(Name2Id(this->modnm)));
}

// GenQName
// n : AS`Name
// ==> AS`Name
TYPE_AS_Name vdmcg::GenQName(const TYPE_AS_Name& n)
{
  if (this->modnm.IsNil())
    ReportError(L"GenQName");
  else {
#ifdef VDMSL
    return GenQName2(this->modnm, n);
#endif //VDMSL
#ifdef VDMPP
    if (n.get_ids().Length() == 1)
    {
      if (this->known_types.DomExists(ASTAUX::Combine2Names(this->modnm, n)))
        return GenQName2(this->modnm, n);
      SEQ<TYPE_AS_Name> cs_l (GetOrderedSupers(GiveCurCASName()));
      if (!cs_l.IsEmpty())
      {
        size_t len_cs_l = cs_l.Length();
        for (size_t idx = 1; idx <= len_cs_l; idx++)
          if (this->known_types.DomExists(ASTAUX::Combine2Names(cs_l[idx], n)))
            return GenQName2(cs_l[idx], n);
      }
    }
    return n;
#endif //VDMPP
  }
  return n; // To keep VC++ happy.
}

// GenQName2
// m : AS`Name
// n : AS`Name
// -> AS`Name
TYPE_AS_Name vdmcg::GenQName2(const TYPE_AS_Name & m, const TYPE_AS_Name & n)
{
  if (n.GetSequence(pos_AS_Name_ids).Length() == 2)
    return n;
  else {
    TYPE_AS_Ids ids (m.GetSequence(pos_AS_Name_ids));
    ids.ImpConc(n.GetSequence(pos_AS_Name_ids));

    return TYPE_AS_Name().Init(ids, n.GetInt(pos_AS_Name_cid));
  }
}

// GenQReName2
// m : AS`Name
// n : AS`Name
// -> AS`Name
TYPE_AS_Name vdmcg::GenQReName2(const TYPE_AS_Name & m, const TYPE_AS_Name & n )
{
  TYPE_AS_Ids ids (m.GetSequence(pos_AS_Name_ids));
  const TYPE_AS_Ids & n_ids (n.GetSequence(pos_AS_Name_ids));
  ids.ImpAppend(n_ids[n_ids.Length()]);

  return TYPE_AS_Name().Init(ids, n.GetInt(pos_AS_Name_cid));
}

// GenQComp
// clnm : AS`Name
// type : [AS`Type]
// -> [AS`Type]
Generic vdmcg::GenQComp(const TYPE_AS_Name & clnm, const Generic & type)
{
  if (type.IsNil())
    return type;

  TYPE_AS_Type tp (type);
  switch (tp.GetTag()) {
    case TAG_TYPE_AS_BooleanType :
    case TAG_TYPE_AS_NumericType :
    case TAG_TYPE_AS_TokenType :
    case TAG_TYPE_AS_CharType :
    case TAG_TYPE_AS_VoidType :
    case TAG_TYPE_AS_QuoteType : {
      return type;
    }
    case TAG_TYPE_AS_CompositeType : {
      const SEQ<TYPE_AS_Field> & fields (tp.GetSequence(pos_AS_CompositeType_fields));
      SEQ<TYPE_AS_Field> new_fields;
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++) {
        TYPE_AS_Field field (fields[idx]);
        field.set_type(GenQComp(clnm, fields[idx].GetRecord(pos_AS_Field_type)));
        new_fields.ImpAppend(field);
      }
      TYPE_AS_CompositeType res (tp);
      res.SetField(pos_AS_CompositeType_name, GenQName2(clnm, tp.GetRecord(pos_AS_CompositeType_name)));
      res.SetField(pos_AS_CompositeType_fields, new_fields);
      return res;
    }
    case TAG_TYPE_AS_UnionType : {
      const SEQ<TYPE_AS_Type> & type_l (tp.GetSequence(pos_AS_UnionType_tps));
      SEQ<TYPE_AS_Type> res_tp_l (GenQCompList(clnm, type_l));
      TYPE_AS_UnionType res (tp);
      res.SetField(pos_AS_UnionType_tps, res_tp_l);
      return res;
    }
    case TAG_TYPE_AS_ProductType : {
      const SEQ<TYPE_AS_Type> & type_l (tp.GetSequence(pos_AS_ProductType_tps));
      SEQ<TYPE_AS_Type> res_tp_l (GenQCompList(clnm, type_l));
      TYPE_AS_ProductType res (tp);
      res.SetField(pos_AS_ProductType_tps, res_tp_l);
      return res;
    }
    case TAG_TYPE_AS_OptionalType : {
      TYPE_AS_OptionalType res (tp);
      res.SetField(pos_AS_OptionalType_tp, GenQComp(clnm, tp.GetRecord(pos_AS_OptionalType_tp)));
      return res;
    }
    case TAG_TYPE_AS_Set0Type : {
      TYPE_AS_Set0Type res (tp);
      res.SetField(pos_AS_Set0Type_elemtp, GenQComp(clnm, tp.GetRecord(pos_AS_Set0Type_elemtp)));
      return res;
    }
    case TAG_TYPE_AS_Set1Type : {
      TYPE_AS_Set1Type res (tp);
      res.SetField(pos_AS_Set1Type_elemtp, GenQComp(clnm, tp.GetRecord(pos_AS_Set1Type_elemtp)));
      return res;
    }
    case TAG_TYPE_AS_Seq0Type : {
      TYPE_AS_Seq0Type res (tp);
      res.SetField(pos_AS_Seq0Type_elemtp, GenQComp(clnm, tp.GetRecord(pos_AS_Seq0Type_elemtp)));
      return res;
    }
    case TAG_TYPE_AS_Seq1Type : {
      TYPE_AS_Seq1Type res (tp);
      res.SetField(pos_AS_Seq1Type_elemtp, GenQComp(clnm, tp.GetRecord(pos_AS_Seq1Type_elemtp)));
      return res;
    }
    case TAG_TYPE_AS_GeneralMap0Type : {
      TYPE_AS_GeneralMap0Type res (tp);
      res.SetField(pos_AS_GeneralMap0Type_mapdom, GenQComp(clnm, tp.GetRecord(pos_AS_GeneralMap0Type_mapdom)));
      res.SetField(pos_AS_GeneralMap0Type_maprng, GenQComp(clnm, tp.GetRecord(pos_AS_GeneralMap0Type_maprng)));
      return res;
    }
    case TAG_TYPE_AS_GeneralMap1Type : {
      TYPE_AS_GeneralMap1Type res (tp);
      res.SetField(pos_AS_GeneralMap1Type_mapdom, GenQComp(clnm, tp.GetRecord(pos_AS_GeneralMap1Type_mapdom)));
      res.SetField(pos_AS_GeneralMap1Type_maprng, GenQComp(clnm, tp.GetRecord(pos_AS_GeneralMap1Type_maprng)));
      return res;
    }
    case TAG_TYPE_AS_InjectiveMap0Type : {
      TYPE_AS_InjectiveMap0Type res (tp);
      res.SetField(pos_AS_InjectiveMap0Type_mapdom, GenQComp(clnm, tp.GetRecord(pos_AS_InjectiveMap0Type_mapdom)));
      res.SetField(pos_AS_InjectiveMap0Type_maprng, GenQComp(clnm, tp.GetRecord(pos_AS_InjectiveMap0Type_maprng)));
      return res;
    }
    case TAG_TYPE_AS_InjectiveMap1Type : {
      TYPE_AS_InjectiveMap1Type res (tp);
      res.SetField(pos_AS_InjectiveMap1Type_mapdom, GenQComp(clnm, tp.GetRecord(pos_AS_InjectiveMap1Type_mapdom)));
      res.SetField(pos_AS_InjectiveMap1Type_maprng, GenQComp(clnm, tp.GetRecord(pos_AS_InjectiveMap1Type_maprng)));
      return res;
    }
    case TAG_TYPE_AS_OpType : {
      TYPE_AS_OpType res (tp);
      res.SetField(pos_AS_OpType_opdom, GenQCompList(clnm, tp.GetSequence(pos_AS_OpType_opdom)));
      res.SetField(pos_AS_OpType_oprng, GenQComp(clnm, tp.GetRecord(pos_AS_OpType_oprng)));
      return res;
    }
    case TAG_TYPE_AS_PartialFnType : {
      TYPE_AS_PartialFnType res (tp);
      res.SetField(pos_AS_PartialFnType_fndom, GenQCompList(clnm, tp.GetSequence(pos_AS_PartialFnType_fndom)));
      res.SetField(pos_AS_PartialFnType_fnrng, GenQComp(clnm, tp.GetRecord(pos_AS_PartialFnType_fnrng)));
      return res;
    }
    case TAG_TYPE_AS_TotalFnType : {
      TYPE_AS_TotalFnType res (tp);
      res.SetField(pos_AS_TotalFnType_fndom, GenQCompList(clnm, tp.GetSequence(pos_AS_TotalFnType_fndom)));
      res.SetField(pos_AS_TotalFnType_fnrng, GenQComp(clnm, tp.GetRecord(pos_AS_TotalFnType_fnrng)));
      return res;
    }
    case TAG_TYPE_AS_BracketedType : {
      TYPE_AS_BracketedType res (tp);
      res.SetField(pos_AS_BracketedType_tp, GenQComp(clnm, tp.GetRecord(pos_AS_BracketedType_tp)));
      return res;
    }
    case TAG_TYPE_AS_TypeName : {
#ifdef VDMPP
      const TYPE_AS_Name & nm (tp.GetRecord(pos_AS_TypeName_name));
      if (IsClass(nm)) {
        return tp;
      }
#endif // VDMPP
      TYPE_AS_TypeName res (tp);
      res.SetField(pos_AS_TypeName_name, GenQName2(clnm, tp.GetRecord(pos_AS_TypeName_name)));
      return res;
    }
    default : {
      ReportError(L"GenQComp");
    }
  }
  return tp; // To keep VC++ happy.
}

// GenQCompList
// clnm : AS`Name
// type_l : seq of AS`Type
// -> seq of AS`Type
SEQ<TYPE_AS_Type> vdmcg::GenQCompList(const TYPE_AS_Name & clnm, const SEQ<TYPE_AS_Type> & type_l)
{
  SEQ<TYPE_AS_Type> res;
  size_t len_type_l = type_l.Length();
  for (size_t idx = 1; idx <= len_type_l; idx++) {
    res.ImpAppend(GenQComp(clnm, type_l[idx]));
  }
  return res;
}

// GenRQComp
// clnm : AS`Name
// type : [REP`TypeRep]
// -> [REP`TypeRep]
Generic vdmcg::GenRQComp(const TYPE_AS_Name & clnm, const Generic & type)
{
  if (type.IsNil()) {
    return Nil();
  }
  TYPE_REP_TypeRep tp (type);
  switch (tp.GetTag()) {
    case TAG_TYPE_REP_SetTypeRep : {
      return mk_REP_SetTypeRep(GenRQComp(clnm, tp.GetRecord(pos_REP_SetTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_SeqTypeRep : {
      return mk_REP_SeqTypeRep(GenRQComp(clnm, tp.GetRecord(pos_REP_SeqTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_EmptySetTypeRep :
    case TAG_TYPE_REP_EmptySeqTypeRep :
    case TAG_TYPE_REP_EmptyMapTypeRep :
    case TAG_TYPE_REP_UnitTypeRep:
    case TAG_TYPE_REP_NilTypeRep :
    case TAG_TYPE_REP_AllTypeRep : {
      return tp;
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
      const SEQ<TYPE_REP_FieldRep> & fields (tp.GetSequence(pos_REP_CompositeTypeRep_fields));
      SEQ<TYPE_REP_FieldRep> new_fields;
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++) {
        TYPE_REP_FieldRep f (fields[idx]);
        f.set_tp(GenRQComp(clnm, f.GetRecord(pos_REP_FieldRep_tp)));
        new_fields.ImpAppend(f);
      }
      return mk_REP_CompositeTypeRep(GenQName2(clnm, tp.GetRecord(pos_REP_CompositeTypeRep_nm)), new_fields);
    }
    case TAG_TYPE_REP_UnionTypeRep : {
      return mk_REP_UnionTypeRep(GenRQCompSet(clnm, tp.GetSet(pos_REP_UnionTypeRep_tps)));
    }
    case TAG_TYPE_REP_ProductTypeRep : {
      return mk_REP_ProductTypeRep(GenRQCompList(clnm, tp.GetSequence(pos_REP_ProductTypeRep_tps)));
    }
    case TAG_TYPE_REP_GeneralMapTypeRep : {
      return mk_REP_GeneralMapTypeRep(GenRQComp(clnm, tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom)),
                                      GenRQComp(clnm, tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng)));
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep : {
      return mk_REP_InjectiveMapTypeRep(GenRQComp(clnm, tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom)),
                                        GenRQComp(clnm, tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng)));
    }
    case TAG_TYPE_REP_TypeNameRep : {
      return mk_REP_TypeNameRep(GenQName2(clnm, tp.GetRecord(pos_REP_TypeNameRep_nm)));
    }
#ifdef VDMPP
    case TAG_TYPE_REP_ObjRefTypeRep : {
      return tp;
    }
#endif //VDMPP
    case TAG_TYPE_REP_TypeVarRep : {
      return tp;
    }
    case TAG_TYPE_REP_OpTypeRep : {
      const SEQ<TYPE_REP_TypeRep> & opdom (tp.GetSequence(pos_REP_OpTypeRep_Dom));
      return mk_REP_OpTypeRep(GenRQCompList(clnm, opdom), GenRQComp(clnm, tp.GetRecord(pos_REP_OpTypeRep_Rng)));
    }
    case TAG_TYPE_REP_BooleanTypeRep :
    case TAG_TYPE_REP_NumericTypeRep :
    case TAG_TYPE_REP_TokenTypeRep :
    case TAG_TYPE_REP_CharTypeRep :
    case TAG_TYPE_REP_QuoteTypeRep : {
      return tp;
    }
    case TAG_TYPE_REP_PartialFnTypeRep : {
      const Generic & fndom (tp.GetField(pos_REP_PartialFnTypeRep_fndom));
      Generic new_fndom = Nil();
      if (fndom.IsSequence()) {
        new_fndom = GenRQCompList (clnm, fndom);
      }
      return mk_REP_PartialFnTypeRep(new_fndom, GenRQComp(clnm, tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng)));
    }
    case TAG_TYPE_REP_TotalFnTypeRep : {
      const Generic & fndom (tp.GetField(pos_REP_TotalFnTypeRep_fndom));
      Generic new_fndom = Nil();
      if (fndom.IsSequence()) {
        new_fndom = GenRQCompList (clnm, fndom);
      }
      return mk_REP_TotalFnTypeRep(new_fndom, GenRQComp(clnm, tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng)));
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return mk_REP_InvTypeRep(tp.GetRecord(pos_REP_InvTypeRep_name),
                               GenRQComp(clnm, tp.GetRecord(pos_REP_InvTypeRep_shape)),
                               tp.GetField(pos_REP_InvTypeRep_invariant));
    }
#ifdef VDMPP
    case TAG_TYPE_REP_OverTypeRep: {
      return mk_REP_OverTypeRep(GenRQCompSet(clnm, tp.GetSet(pos_REP_OverTypeRep_tps)));
    }
#endif // VDMPP
    default : {
      ReportError(wstring(L"GenRQComp: ") + GetStatSem().Type2Ascii(tp));
      return  tp;
    }
  }
}

// GenRQCompSet
// clnm : AS`Name
// type_s : set of REP`TypeRep
// -> set of REP`TypeRep
SET<TYPE_REP_TypeRep> vdmcg::GenRQCompSet(const TYPE_AS_Name & clnm, const SET<TYPE_REP_TypeRep> & type_s)
{
  SET<TYPE_REP_TypeRep> tps (type_s); // for safe
  SET<TYPE_REP_TypeRep> res;
  Generic tp;
  for (bool bb = tps.First(tp); bb; bb = tps.Next(tp)) {
    res.Insert(GenRQComp(clnm, tp));
  }
  return res;
}

// GenRQCompList
// clnm : AS`Name
// type_l : seq of REP`TypeRep
// -> seq of REP`TypeRep
SEQ<TYPE_REP_TypeRep> vdmcg::GenRQCompList(const TYPE_AS_Name & clnm, const SEQ<TYPE_REP_TypeRep> & type_l)
{
  SEQ<TYPE_REP_TypeRep> res;
  size_t len_type_l = type_l.Length();
  for (size_t idx = 1; idx <= len_type_l; idx++) {
    res.ImpAppend(GenRQComp(clnm, type_l[idx]));
  }
  return res;
}

// GenIfNotDef
#ifdef VDMSL
// id : CPP`Identifier
// defval : CPP`Expr
// -> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenIfNotDef(const TYPE_CPP_Identifier & id, const TYPE_CPP_Expr & defval)
#endif // VDMSP
#ifdef VDMPP
// id : CPP`Identifier
// defval : CPP`Expr
// -> seq of CPP`MemberDeclaration | CPP`CPPAS
Sequence vdmcg::GenIfNotDef(const TYPE_CPP_Identifier & id, const TYPE_CPP_Expr & defval)
#endif // VDMPP
{
#ifdef VDMSL
  TYPE_CPP_CPPAS res;
#endif // VDMSP
#ifdef VDMPP
  Sequence res;
#endif // VDMPP
  res.ImpAppend(vdm_BC_GenPreIfNotDef(id));
  res.ImpAppend(vdm_BC_GenPreDefine(id, defval));
  res.ImpAppend(vdm_BC_GenPreEndIf(id));
  return res;
}

// GenTypeDeclIfDef
#ifdef VDMSL
// id : CPP`Identifier
// cpp : CPP`CPPAS
// -> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenTypeDeclIfDef(const TYPE_CPP_Identifier & id, const TYPE_CPP_CPPAS & cpp)
#endif // VDMSL
#ifdef VDMPP
// id : CPP`Identifier
// cpp : (CPP`CPPAS| seq of CPP`MemberDeclaration)
// -> seq of CPP`MemberDeclaration | CPP`CPPAS
Sequence vdmcg::GenTypeDeclIfDef(const TYPE_CPP_Identifier & id, const Sequence & cpp)
#endif // VDMPP
{
#ifdef VDMSL
  TYPE_CPP_CPPAS res;
#endif // VDMSL
#ifdef VDMPP
  Sequence res;
#endif // VDMPP
  res.ImpAppend(vdm_BC_GenPreIfNotDef(PrefixANONYMDecl(id)));
  res.ImpAppend(vdm_BC_GenPreDefine(PrefixANONYMDecl(id), vdm_BC_GenIntegerLit(1)));
  res.ImpConc(cpp);
  res.ImpAppend(vdm_BC_GenPreEndIf(PrefixANONYMDecl(id)));
  return res;
}

// GenFctOpDefIfDef
#ifdef VDMSL
// nm : AS`Name
// cpp : CPP`CPPAS
// ==> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenFctOpDefIfDef(const TYPE_AS_Name & nm, const TYPE_CPP_CPPAS & cpp)
#endif // VDMSL
#ifdef VDMPP
// nm : AS`Name
// cpp : (CPP`CPPAS| seq of CPP`MemberDeclaration)
// ==> seq of CPP`MemberDeclaration | CPP`CPPAS
Sequence vdmcg::GenFctOpDefIfDef(const TYPE_AS_Name & nm, const Sequence & cpp)
#endif // VDMPP
{
#ifdef VDMSL
  TYPE_CPP_Identifier id (vdm_BC_GenIdentifier(Name2Id(GenQName(nm))));
  TYPE_CPP_CPPAS res;
#endif//VDMSL
#ifdef VDMPP
  TYPE_CPP_Identifier id (vdm_BC_GenIdentifier(Name2Id(GenQName2(this->modnm, nm))));
  Sequence res;
#endif //VDMPP
  res.ImpAppend(vdm_BC_GenPreIfNotDef(PrefixANONYMDef(id)));
  res.ImpConc(cpp);
  res.ImpAppend(vdm_BC_GenPreEndIf(PrefixANONYMDef(id)));
  return res;
}

// PrefixANONYMDecl
// id : CPP`Identifier
// +> CPP`Identifier
TYPE_CPP_Identifier vdmcg::PrefixANONYMDecl(const TYPE_CPP_Identifier & id)
{
  return vdm_BC_GenIdentifier(ASTAUX::MkId(L"DECL_").ImpConc(id.get_id()));
}

// PrefixANONYMDef
// id : CPP`Identifier
// +> CPP`Identifier
TYPE_CPP_Identifier vdmcg::PrefixANONYMDef(const TYPE_CPP_Identifier &id)
{
  return vdm_BC_GenIdentifier(ASTAUX::MkId(L"DEF_").ImpConc(id.get_id()));
}

// GenTypeDefIfDef
// id : CPP`Identifier
#ifdef VDMSL
// cpp : CPP`CPPAS
// -> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenTypeDefIfDef(const TYPE_CPP_Identifier & id, const TYPE_CPP_CPPAS & cpp)
#endif // VDMSL
#ifdef VDMPP
// cpp : seq of CPP`MemberDeclaration
// -> seq of CPP`MemberDeclaration
SEQ<TYPE_CPP_MemberDeclaration> vdmcg::GenTypeDefIfDef(const TYPE_CPP_Identifier & id,
                                                       const SEQ<TYPE_CPP_MemberDeclaration> & cpp)
#endif // VDMPP
{
#ifdef VDMSL
  TYPE_CPP_CPPAS res;
#endif // VDMSL
#ifdef VDMPP
  SEQ<TYPE_CPP_MemberDeclaration> res;
#endif // VDMPP
  res.ImpAppend(vdm_BC_GenPreIf(vdm_BC_GenLogAnd(vdm_BC_GenNot(PrefixANONYMDef(id)), PrefixANONYMDecl(id))));
  res.ImpAppend(vdm_BC_GenPreDefine(PrefixANONYMDef(id), vdm_BC_GenIntegerLit(1)));
  res.ImpConc(cpp);
  res.ImpAppend(vdm_BC_GenPreEndIf(PrefixANONYMDef(id)));
  return res;
}

// GenForwardDecl
// d : CPP`Identifier
// +> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenForwardDecl(const TYPE_CPP_Identifier & d)
{
  TYPE_CPP_CPPAS res;
  res.ImpAppend(vdm_BC_GenClassTypeDecl(d));
  return res;
}

// GenMemFctMacros
// - : <SET> | <SEQ> | <MAP> | <PRODUCT> | <COMPOSITE> | <OBJREF>
// -> seq of CPP`MemberDeclaration
SEQ<TYPE_CPP_MemberDeclaration> vdmcg::GenMemFctMacros(const Int &)
{
  SEQ<TYPE_CPP_MemberDeclaration> res;
  //  res.ImpAppend(GenPreMacro(vdm_BC_GenIdentifier(ASTAUX::MkId(L"EXT_MEMFCT"))));
  return res;
}

// GenGetFunctionDef
// - : (<PRODUCT> | <COMPOSITE>)
// s : CPP`Identifier -- field name
// t : CPP`Identifier -- field type name
// i : FieldTag
// cnm : CPP`QualifiedClassName
// tp : REP`TypeRep
// +> CPP`FunctionDefinition
TYPE_CPP_FunctionDefinition vdmcg::GenGetFunctionDef(const Int &,
                                                     const TYPE_CPP_Identifier & s,
                                                     const TYPE_CPP_Identifier & t,
                                                     int i,
                                                     const TYPE_CPP_Identifier & cnm,
                                                     const TYPE_REP_TypeRep & tp)
{
  SEQ<Char> meth (GenGetMethod(tp));

  // for special care
  SEQ<Char> id (t.GetSequence(pos_CPP_Identifier_id));
  if (id == ASTAUX::MkId(L"TYPE_STKM_EvaluatorStatus")) {
    meth = ASTAUX::MkId(L"GetField");
  }
  TYPE_CPP_TypeSpecifier ts (vdm_BC_GenTypeSpecifier(t));
  SEQ<TYPE_CPP_DeclSpecifier> dsl (mk_sequence(ts));

  TYPE_CPP_FctDecl decl (vdm_BC_GenConstFctDecl(
                            //vdm_BC_GenQualifiedName2(
                            vdm_BC_GenQualifiedName(
                                cnm,
                                vdm_BC_GenIdentifier(ASTAUX::MkId(L"get_").ImpConc(s.get_id()))),
                            SEQ<TYPE_CPP_ArgumentDeclaration>()));

  //SEQ<TYPE_CPP_TypeSpecifier> ds_l (mk_sequence(vdm_BC_GenTypeSpecifier(quote_CONST),
  //                                              ts,
  //                                              vdm_BC_GenTypeSpecifier(quote_REFERENCE)));

  TYPE_CPP_Expr fcall (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(meth), mk_sequence(vdm_BC_GenIntegerLit(i))));
  TYPE_CPP_Stmt body (vdm_BC_GenBlock(mk_sequence(vdm_BC_GenReturnStmt(fcall))));
  return vdm_BC_GenFctDef(dsl, decl, nil, body);
}

// GenGetFunctionDecl
// s : CPP`Identifier
// t : CPP`Identifier
// +> CPP`MemberSpecifier
TYPE_CPP_MemberSpecifier vdmcg::GenGetFunctionDecl(const TYPE_CPP_Identifier& s, const TYPE_CPP_Identifier& t)
{
  SEQ<TYPE_CPP_DeclSpecifier> cl (mk_sequence(vdm_BC_GenTypeSpecifier(t)));

  TYPE_CPP_FctDecl fctdecl (vdm_BC_GenConstFctDecl(
                             vdm_BC_GenIdentifier(ASTAUX::MkId(L"get_").ImpConc(s.get_id())),
                                                   SEQ<TYPE_CPP_ArgumentDeclaration>()));
  return vdm_BC_GenMemberSpec(cl, fctdecl);
}

// GenSetFunctionDecl
// s : CPP`Identifier
// t : CPP`Identifier
// +> CPP`MemberSpecifier
TYPE_CPP_MemberSpecifier vdmcg::GenSetFunctionDecl(const TYPE_CPP_Identifier &s,
                                                   const TYPE_CPP_Identifier &t)
{
  SEQ<TYPE_CPP_DeclSpecifier> decl_l (mk_sequence(vdm_BC_GenTypeSpecifier(quote_CONST),
                                             vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(t.get_id()))));

  TYPE_CPP_ArgDecl arg (vdm_BC_GenArgDecl(decl_l, vdm_BC_GenRef(vdm_BC_GenIdentifier(ASTAUX::MkId(L"p")))));
  TYPE_CPP_FctDecl decl (vdm_BC_GenFctDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"set_").ImpConc(s.get_id())),
                                           mk_sequence(arg)));
  SEQ<TYPE_CPP_DeclSpecifier> cl (mk_sequence(vdm_BC_GenVoid()));

  return vdm_BC_GenMemberSpec(cl, decl);
}

// GenSetFunctionDef
// s : CPP`Identifier
// t : CPP`Identifier
// i : FieldTag
// cnm : CPP`QualifiedClassName
// +> CPP`FunctionDefinition
TYPE_CPP_FunctionDefinition vdmcg::GenSetFunctionDef(const TYPE_CPP_Identifier & s,
                                                     const TYPE_CPP_Identifier & t,
                                                     int i,
                                                     const TYPE_CPP_Identifier & cnm)
{
  SEQ<TYPE_CPP_DeclSpecifier> dsl (mk_sequence(vdm_BC_GenTypeSpecifier(vdm_BC_GenVoid())));

  SEQ<TYPE_CPP_DeclSpecifier> decl_l (mk_sequence(vdm_BC_GenTypeSpecifier(quote_CONST),
                                                  vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(t.get_id()))));

  TYPE_CPP_ArgDecl arg (vdm_BC_GenArgDecl(decl_l, vdm_BC_GenRef(vdm_BC_GenIdentifier(ASTAUX::MkId(L"p")))));

  TYPE_CPP_FctDecl decl (vdm_BC_GenFctDecl(
                           //vdm_BC_GenQualifiedName2(cnm,
                           vdm_BC_GenQualifiedName(cnm,
                                                    vdm_BC_GenIdentifier(ASTAUX::MkId(L"set_").ImpConc(s.get_id()))),
                           mk_sequence(arg)));

  SEQ<TYPE_CPP_Expr> setfield_arg (mk_sequence(vdm_BC_GenIntegerLit(i),
                                               vdm_BC_GenIdentifier(ASTAUX::MkId(L"p"))));

  TYPE_CPP_Expr e (vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"SetField")), setfield_arg));

  TYPE_CPP_Stmt body (vdm_BC_GenBlock(mk_sequence(vdm_BC_GenExpressionStmt(e))));

  return vdm_BC_GenFctDef(dsl, decl, nil, body);
}

// GenGetTypeNameDecl
// id : CPP`Identifier
// +> CPP`FunctionDefinition
TYPE_CPP_FunctionDefinition vdmcg::GenGetTypeNameDecl(const TYPE_CPP_Identifier & id)
{
  SEQ<TYPE_CPP_DeclSpecifier> dsl (mk_sequence(vdm_BC_GenTypeSpecifier(quote_CONST),
                                               vdm_BC_GenIdentifier(GenCppCharType())));

  TYPE_CPP_FctDecl decl(vdm_BC_GenConstFctDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"GetTypeName")), type_dL()));

  TYPE_CPP_Expr e (vdm_BC_GenReturnStmt(vdm_BC_GenStringLit(id.get_id())));
  TYPE_CPP_Stmt body (vdm_BC_GenBlock(mk_sequence(e)));

  return vdm_BC_GenFctDef(dsl, decl, nil, body);
}

// GenCommonConstrDecl
// cnm : CPP`Identifier
// supid : (CPP`Identifier | CPP`CompleteClassName | CPP`TemplateClassName)
// +> CPP`FunctionDefinition
TYPE_CPP_FunctionDefinition vdmcg::GenCommonConstrDecl(const TYPE_CPP_Identifier & cnm, const Record & supid)
{
  SEQ<TYPE_CPP_DeclSpecifier> arg_l (mk_sequence(vdm_BC_GenTypeSpecifier(quote_CONST),
                                                 vdm_BC_GenTypeSpecifier(
                                                   vdm_BC_GenIdentifier(ASTAUX::MkId(L"Common")))));

  TYPE_CPP_Identifier id (vdm_BC_GenIdentifier(ASTAUX::MkId(L"c")));

  TYPE_CPP_Declarator idref;
  if (vdm_CPP_isCPP()) {
    idref = vdm_BC_GenRef(id);
  }
  else {
    idref = id;
  }
  TYPE_CPP_ArgDecl arg (vdm_BC_GenArgDecl(arg_l, idref));
  TYPE_CPP_FctDecl decl (vdm_BC_GenFctDecl(cnm, mk_sequence(arg)));
  TYPE_CPP_Stmt body (vdm_BC_GenBlock(SEQ<TYPE_CPP_Stmt>()));
  SEQ<TYPE_CPP_MemInitializer> inh (vdm_BC_GenInitBaseN(supid, mk_sequence(vdm_BC_GenIdentifier(ASTAUX::MkId(L"c")))));

  return vdm_BC_GenConstructorDef(SEQ<TYPE_CPP_Annotation>(), SEQ<TYPE_CPP_DeclSpecifier>(), decl, inh, body);
}

// GenVDMBasePConstrDecl
// cnm : CPP`Identifier
// supid : (CPP`Identifier | CPP`CompleteClassName | CPP`TemplateClassName)
// +> CPP`FunctionDefinition
TYPE_CPP_FunctionDefinition vdmcg::GenVDMBasePConstrDecl(const TYPE_CPP_Identifier & cnm, const Record & supid)
{
  SEQ<TYPE_CPP_DeclSpecifier> arg_ds (mk_sequence(vdm_BC_GenTypeSpecifier(
                                                    vdm_BC_GenIdentifier(ASTAUX::MkId(L"vdmBase")))));
  TYPE_CPP_ArgDecl arg (vdm_BC_GenArgDecl(arg_ds, vdm_BC_GenIndirection(vdm_BC_GenIdentifier(ASTAUX::MkId(L"p")))));
  TYPE_CPP_FctDecl decl (vdm_BC_GenFctDecl(cnm, mk_sequence(arg)));
  TYPE_CPP_Stmt body (vdm_BC_GenBlock(SEQ<TYPE_CPP_Stmt>()));
  SEQ<TYPE_CPP_MemInitializer> inh (vdm_BC_GenInitBaseN(supid, mk_sequence(vdm_BC_GenIdentifier(ASTAUX::MkId(L"p")))));

  return vdm_BC_GenConstructorDef(SEQ<TYPE_CPP_Annotation>(), SEQ<TYPE_CPP_DeclSpecifier>(), decl, inh, body);
}

// GenTemplateConstrDecl
// cnm : CPP`Identifier
// supid : CPP`TemplateClassName
// +> CPP`FunctionDefinition
TYPE_CPP_FunctionDefinition vdmcg::GenTemplateConstrDecl(const TYPE_CPP_Identifier& cnm,
                                                         const TYPE_CPP_TemplateClassName & supid )
{
  SEQ<TYPE_CPP_DeclSpecifier> arg_ds (mk_sequence(vdm_BC_GenTypeSpecifier(quote_CONST),
                                                  vdm_BC_GenTypeSpecifier(supid)));
  TYPE_CPP_ArgDecl arg (vdm_BC_GenArgDecl(arg_ds, vdm_BC_GenRef(vdm_BC_GenIdentifier(ASTAUX::MkId(L"c")))));
  TYPE_CPP_FctDecl decl (vdm_BC_GenFctDecl(cnm, mk_sequence(arg)));
  TYPE_CPP_Stmt body (vdm_BC_GenBlock(type_dL()));
  SEQ<TYPE_CPP_MemInitializer> inh (vdm_BC_GenInitBaseN(supid, mk_sequence(vdm_BC_GenIdentifier(ASTAUX::MkId(L"c")))));

  return vdm_BC_GenConstructorDef(SEQ<TYPE_CPP_Annotation>(), SEQ<TYPE_CPP_DeclSpecifier>(), decl, inh, body);
}

// GenDefaultConstrDecl
// cnm : CPP`Identifier
// supid : (CPP`Identifier | CPP`CompleteClassName | CPP`TemplateClassName)
// +> CPP`FunctionDefinition
TYPE_CPP_FunctionDefinition vdmcg::GenDefaultConstrDecl(const TYPE_CPP_Identifier & cnm, const Record & supid)
{
  TYPE_CPP_FctDecl decl (vdm_BC_GenFctDecl(cnm, SEQ<TYPE_CPP_ArgumentDeclaration>()));
  TYPE_CPP_Stmt body (vdm_BC_GenBlock(SEQ<TYPE_CPP_Stmt>()));

  if (vdm_CPP_isCPP()) {
    return vdm_BC_GenConstructorDef(SEQ<TYPE_CPP_Annotation>(),
                                    SEQ<TYPE_CPP_DeclSpecifier>(),
                                    decl, vdm_BC_GenInitBaseN(supid, type_dL()), body);
  }
  else { // java
    SEQ<TYPE_CPP_DeclSpecifier> dsl (mk_sequence(vdm_BC_GenModifier(quote_PUBLIC)));
    return vdm_BC_GenConstructorDef(SEQ<TYPE_CPP_Annotation>(), dsl, decl, nil, body);
  }
}

// GenClassDefaultConstructorDecl
// whattp : (<COMPOSITE>|<PRODUCT>)
// cnm : CPP`Identifier
// sl : TagList
// ==> CPP`MemberDeclaration
TYPE_CPP_MemberDeclaration vdmcg::GenClassDefaultConstructorDecl(const Int & whattp,
                                                                 const TYPE_CPP_Identifier cnm,
                                                                 const SEQ<TYPE_CPP_Identifier> &sl)
{
  TYPE_CPP_FctDecl decl (vdm_BC_GenFctDecl(cnm, SEQ<TYPE_CPP_ArgumentDeclaration>()));
  TYPE_CPP_Stmt body (vdm_BC_GenBlock(SEQ<TYPE_CPP_Stmt>()));

  if (vdm_CPP_isCPP()) {
    SEQ<TYPE_CPP_MemInitializer> inh;
    if (whattp == (Int) TAG_quote_COMPOSITE ) {
      inh = vdm_BC_GenInitBaseN(GenRecordType(Nil()).get_tp(),
                                mk_sequence(GenTypeTagCppId(cnm), vdm_BC_GenIntegerLit(sl.Length())));
    }
    else {
      inh = vdm_BC_GenInitBaseN(GenProductType().get_tp(), mk_sequence(vdm_BC_GenIntegerLit(sl.Length())));
    }
    return vdm_BC_GenConstructorDef(SEQ<TYPE_CPP_Annotation>(), SEQ<TYPE_CPP_DeclSpecifier>(), decl, inh, body);
  }
  else { // java
    SEQ<TYPE_CPP_DeclSpecifier> dsl (mk_sequence(vdm_BC_GenModifier(quote_PUBLIC)));
    return vdm_BC_GenConstructorDef(SEQ<TYPE_CPP_Annotation>(), dsl, decl, nil, body);
  }
}

// GenIdentDecls
// sl : TagList      (seq of CPP`Identifier)
// tenv : TagEnv     (map CPP`Identifier to CPP`Name)
// ==> seq of CPP`MemberDeclaration
SEQ<TYPE_CPP_MemberDeclaration> vdmcg::GenIdentDecls(const SEQ<TYPE_CPP_Identifier> & sl,
                                                     const MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> & tenv)
{
  SEQ<TYPE_CPP_DeclarationStmt> rb;
  size_t len_sl = sl.Length();
  for (size_t idx = 1; idx <= len_sl; idx++) {
    const TYPE_CPP_Identifier & j (sl[idx]);
    if (vdm_CPP_isCPP()) {
      rb.ImpAppend(vdm_BC_GenDecl(vdm_BC_GenTypeSpecifier(tenv[j]), j, nil));
    }
    else {
      rb.ImpAppend(vdm_BC_GenDecl(vdm_BC_GenTypeSpecifier(tenv[j]), j, vdm_BC_GenAsgnInit(GenNilLit())));
    }
  }

  SEQ<TYPE_CPP_MemberDeclaration> memdecls;
  size_t len_rb = rb.Length();
  for (size_t i = 1; i <= len_rb; i++) {
    const TYPE_CPP_DeclarationStmt & dstmt (rb[i]);
    const TYPE_CPP_IdentDeclaration & idecl (dstmt.GetRecord(pos_CPP_DeclarationStmt_decl));
    SEQ<TYPE_CPP_DeclSpecifier> ds (idecl.GetSequence(pos_CPP_IdentDeclaration_ds));
    const SEQ<TYPE_CPP_InitDeclarator> & initd (idecl.GetSequence(pos_CPP_IdentDeclaration_dl));
    TYPE_CPP_Modifier pub (vdm_BC_GenModifier(quote_PUBLIC));
    memdecls.ImpAppend(vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(), ds.ImpPrepend(pub), initd));
  }
  return memdecls;
}

#ifdef VDMPP
// GenClassConstrDecl
// cn : PP`Identifier
// sl : TagList        (seq of CPP`Identifier)
// tenv : TagEnv       (map CPP`Identifier to CPP`Name)
// ==> CPP`FunctionDefinition
TYPE_CPP_FunctionDefinition vdmcg::GenClassConstrDecl(const TYPE_CPP_Identifier & cn,
                                                      const SEQ<TYPE_CPP_Identifier> & sl,
                                                      const MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> & tenv)
{
  SEQ<TYPE_CPP_ArgumentDeclaration> arglist;
  size_t len_sl = sl.Length();
  SEQ<TYPE_CPP_Stmt> stmtl;
  for (size_t j = 1; j <= len_sl; j++) {
    arglist.ImpAppend(vdm_BC_GenArgDecl(type_dL().ImpAppend(vdm_BC_GenTypeSpecifier(tenv[sl[j]])),
                                        vdm_BC_GenIdentifier(ASTAUX::MkId(L"p").ImpConc(StringNumber(j)))));
    stmtl.ImpAppend(vdm_BC_GenAsgnStmt(vdm_BC_GenObjectMemberAccess(GenThis(), sl[j]),
                                       vdm_BC_GenIdentifier(ASTAUX::MkId(L"p").ImpConc(StringNumber(j)))));
  }

  TYPE_CPP_FctDecl decl (vdm_BC_GenFctDecl(cn, arglist));
  SEQ<TYPE_CPP_Modifier> ml (mk_sequence(vdm_BC_GenModifier(quote_PUBLIC)));

  return vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
                          ml, SEQ<TYPE_CPP_DeclSpecifier>(), decl, nil, vdm_BC_GenBlock(stmtl));
}

// GetBasicTypes
// ==> set of CPP`Expr
SET<TYPE_CPP_Expr> vdmcg::GetBasicTypes()
{
  SET<TYPE_CPP_Expr> expr_set;
  if (get_smalltypes_option()) {
    expr_set.Insert(GenSmallIntType().get_tp());
  }
  else {
    expr_set.Insert(GenIntType().get_tp());
  }

  expr_set.Insert(GenRealType().get_tp());
  expr_set.Insert(GenBoolType().get_tp());
  expr_set.Insert(GenCharType().get_tp());
  expr_set.Insert(GenStringType().get_tp());
  expr_set.Insert(GenTokenType().get_tp());

  return expr_set;
}

// GenCloneMethod
// cn : CPP`Identifier
// sl : TagList         (seq of CPP`Identifier)
// tenv : TagEnv        (map CPP`Identifier to CPP`Name)
// ==> CPP`FunctionDefinition
TYPE_CPP_FunctionDefinition vdmcg::GenCloneMethod(const TYPE_CPP_Identifier & cn,
                                                  const SEQ<TYPE_CPP_Identifier> &sl,
                                                  const MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> & tenv)
{
  SEQ<TYPE_CPP_Expr> arglist;
  SET<TYPE_CPP_Expr> basic (GetBasicTypes());
  size_t len_sl = sl.Length();
  for (size_t j = 1; j <= len_sl; j++) {
    TYPE_CPP_Identifier tag (sl[j]);
    TYPE_CPP_Name nm (tenv[tag]);

    if (basic.InSet(nm) ||
        (nm.Is(TAG_TYPE_CPP_Identifier) &&
         TYPE_CPP_Identifier(nm).get_id().SubSequence(1,7) == Sequence(L"quotes."))) {
      arglist.ImpAppend(tag);
    }
    else {
      arglist.ImpAppend(vdm_BC_GenCastExpr(vdm_BC_GenTypeSpecifier(nm),
                                           vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.clone")),
                                                             SEQ<TYPE_CPP_Expr>().ImpAppend(tag))));
    }
  }

  SEQ<TYPE_CPP_Stmt> stmtl;
  stmtl.ImpAppend(vdm_BC_GenReturnStmt(vdm_BC_GenClassInstanceCreationExpr(cn, arglist)));

  TYPE_CPP_FctDecl decl (vdm_BC_GenFctDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"clone")), type_dL()));
  SEQ<TYPE_CPP_Modifier> ml (mk_sequence(vdm_BC_GenModifier(quote_PUBLIC)));
  SEQ<TYPE_CPP_DeclSpecifier> dsl (mk_sequence(vdm_BC_GenTypeSpecifier(
                                                 vdm_BC_GenIdentifier(ASTAUX::MkId(L"Object")))));

  return vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(), ml, dsl, decl, nil, vdm_BC_GenBlock(stmtl));
}

// GenAsciiMethod
// cn : CPP`Identifier
// sl : TagList       (seq of CPP`Identifier)
// tenv : TagEnv      (map CPP`Identifier to CPP`Name)
// ==> CPP`FunctionDefinition
TYPE_CPP_FunctionDefinition vdmcg::GenAsciiMethod(const TYPE_CPP_Identifier& cn,
                                                  const SEQ<TYPE_CPP_Identifier> &sl,
                                                  const MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> & tenv)
{
  TYPE_AS_Id id (cn.get_id());
  TYPE_AS_Id classnm (GiveCurCName());
  SET<TYPE_CPP_Expr> basic (GetBasicTypes());

  TYPE_CPP_Expr expr;

  if (!sl.IsEmpty()) {
    expr = vdm_BC_GenStringLit(ASTAUX::MkId(L"mk_").ImpConc(classnm).ImpConc(ASTAUX::MkId(L"`")).ImpConc(id).ImpConc(ASTAUX::MkId(L"(")));
    size_t len_sl = sl.Length();
    for (size_t i = 1; i <= len_sl; i++) {
      const TYPE_CPP_Identifier & tag (sl[i]);
      const TYPE_CPP_Name & id (tenv[tag]);
      if (get_smalltypes_option()) {
        if (id == vdm_BC_GenIdentifier(ASTAUX::MkId(L"char")))
          expr = vdm_BC_GenPlus(expr, vdm_BC_GenPlus(vdm_BC_GenPlus(vdm_BC_GenStringLit(ASTAUX::MkId(L"'")), tag),
                                                     vdm_BC_GenStringLit(ASTAUX::MkId(L"'"))));
        else if (basic.InSet(id))
          expr = vdm_BC_GenPlus(expr, tag);
        else
          expr = vdm_BC_GenPlus(expr, vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.toString")),
                                                        SEQ<TYPE_CPP_Expr>().ImpAppend(tag)));
      }
      else {
        expr = vdm_BC_GenPlus(expr, vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.toString")),
                                                      SEQ<TYPE_CPP_Expr>().ImpAppend(tag)));
      }

      if (i != (size_t)(sl.Length())) {
        expr = vdm_BC_GenPlus(expr, vdm_BC_GenStringLit(ASTAUX::MkId(L", ")));
      }
    }
    expr = vdm_BC_GenPlus(expr, vdm_BC_GenStringLit(ASTAUX::MkId(L")")));
  }
  else {
    expr = vdm_BC_GenStringLit(ASTAUX::MkId(L"mk_").ImpConc(classnm).ImpConc(ASTAUX::MkId(L"`")).ImpConc(id).ImpConc(ASTAUX::MkId(L"()")));
  }

  TYPE_CPP_Stmt stmt (vdm_BC_GenReturnStmt(expr));
  TYPE_CPP_FctDecl decl (vdm_BC_GenFctDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"toString")), type_dL()));

  SEQ<TYPE_CPP_Modifier> ml (mk_sequence(vdm_BC_GenModifier(quote_PUBLIC)));
  SEQ<TYPE_CPP_DeclSpecifier> dsl (mk_sequence(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"String")))));

  return vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(), ml, dsl, decl, nil, stmt);
}

// GenEqualsMethod
// cn : CPP`Identifier
// sl : TagList     (seq of CPP`Identifier)
// tenv : TagEnv    (map CPP`Identifier to CPP`Name)
// dcenv : DCEnv
// ==> CPP`FunctionDefinition
TYPE_CPP_FunctionDefinition vdmcg::GenEqualsMethod(const TYPE_CPP_Identifier & cn,
                                                   const SEQ<TYPE_CPP_Identifier> & sl,
                                                   const MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> & tenv,
                                                   const MAP<TYPE_CPP_Identifier,Bool> & dcenv)
{
  TYPE_CPP_Identifier argNm (vdm_BC_GenIdentifier(ASTAUX::MkId(L"obj")));
  TYPE_CPP_TypeSpecifier clsTp (vdm_BC_GenTypeSpecifier(cn));
  TYPE_CPP_Stmt test = vdm_BC_GenBracketedExpr(vdm_BC_GenTypeComp(clsTp,argNm));

  bool forall = true;
  size_t len_sl = sl.Length();
  for (size_t i = 1; (i <= len_sl) && forall; i ++) {
    TYPE_CPP_Identifier s (sl[i]);
    forall = dcenv[s].GetValue();
  }

  TYPE_CPP_Stmt stmt;
  if (forall) {
    stmt = vdm_BC_GenReturnStmt(test);
  }
  else {
    TYPE_CPP_Identifier temp = vdm_BC_GenIdentifier(ASTAUX::MkId(L"temp"));
    SET<TYPE_CPP_Expr> basic (GetBasicTypes());
    Generic expr = nil;
    for (size_t i = 1; i <= len_sl; i ++) {
      TYPE_CPP_Identifier s (sl[i]);
      if (!dcenv[s].GetValue()) {
        TYPE_CPP_Expr new_expr;
        if ((get_smalltypes_option()) && (basic.InSet(tenv[s]))) {
          new_expr = vdm_BC_GenEq(s, vdm_BC_GenQualifiedName(temp, s));
        }
        else {
          new_expr = vdm_BC_GenFctCall(vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.equals")),
                                type_dL().ImpAppend(s).ImpAppend(vdm_BC_GenQualifiedName(temp, s)));
        }
        if (expr.IsNil()) {
          expr = new_expr;
        }
        else {
          expr = vdm_BC_GenLogAnd(expr, new_expr);
        }
      }
    }
    TYPE_CPP_AsgnInit ai (vdm_BC_GenAsgnInit(vdm_BC_GenCastExpr(clsTp, argNm)));
    SEQ<TYPE_CPP_Stmt> stmts;
    stmts.ImpAppend(vdm_BC_GenDecl(clsTp, temp, ai));
    stmts.ImpAppend(vdm_BC_GenReturnStmt(expr));

    stmt = vdm_BC_GenIfStmt(test,
                            vdm_BC_GenBlock(stmts),
                            vdm_BC_GenReturnStmt(vdm_BC_GenIdentifier(ASTAUX::MkId(L"false"))));
  }
  
  TYPE_CPP_ArgDecl arg (vdm_BC_GenArgDecl(SEQ<TYPE_CPP_DeclSpecifier>().ImpAppend(vdm_BC_GenGeneric()),
                                          argNm));
  TYPE_CPP_FctDecl decl (vdm_BC_GenFctDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"equals")),
                                           SEQ<TYPE_CPP_Expr>().ImpAppend(arg)));
  SEQ<TYPE_CPP_Modifier> ml (mk_sequence(vdm_BC_GenModifier(quote_PUBLIC)));
  SEQ<TYPE_CPP_DeclSpecifier> dsl (mk_sequence(GenSmallBoolType()));

  return vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(), ml, dsl, decl, nil, stmt);
}

// GenHashMethod
// cn : CPP`Identifier
// sl : TagList      (seq of CPP`Identifier)
// tenv : TagEnv     (map CPP`Identifier to CPP`Name)
// dcenv : DCEnv
// ==> CPP`FunctionDefinition
TYPE_CPP_FunctionDefinition vdmcg::GenHashMethod(const TYPE_CPP_Identifier & cn,
                                                 const SEQ<TYPE_CPP_Identifier> & sl,
                                                 const MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> & tenv,
                                                 const MAP<TYPE_CPP_Identifier,Bool> & dcenv)
{
  bool forall = true;
  size_t len_sl = sl.Length();
  for (size_t i = 1; (i <= len_sl) && forall; i ++) {
    TYPE_CPP_Identifier s (sl[i]);
    forall = dcenv[s].GetValue();
  }

  Generic expr = nil;
  if (forall) {
    expr = vdm_BC_GenIntegerLit(0);
  }
  else {
    for (size_t i = 1; i <= len_sl; i++) {
      const TYPE_CPP_Identifier & s (sl[i]);
      TYPE_CPP_Expr rhs (vdm_BC_GenBracketedExpr(
                           vdm_BC_GenCondExpr(vdm_BC_GenEq(vdm_BC_GenIdentifier(ASTAUX::MkId(L"null")),s),
                                            vdm_BC_GenIntegerLit(0),
                                            vdm_BC_GenFctCallObjMemAcc(s, ASTAUX::MkId(L"hashCode"),
                                                                       SEQ<TYPE_CPP_Expr>()))));
      if (!(dcenv[s].GetValue())) {
        if (expr.IsNil()) {
          expr = rhs;
        }
        else {
          expr = vdm_BC_GenPlus(expr,rhs);
        }
      }
    }
  }

  TYPE_CPP_Stmt stmt (vdm_BC_GenReturnStmt(expr));
  TYPE_CPP_FctDecl decl(vdm_BC_GenFctDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"hashCode")), type_dL()));
  SEQ<TYPE_CPP_Modifier> ml (mk_sequence(vdm_BC_GenModifier(quote_PUBLIC)));
  SEQ<TYPE_CPP_DeclSpecifier> dsl (mk_sequence(GenSmallIntType()));

  return vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(), ml, dsl, decl, nil, stmt);
}
#endif // VDMPP

// GenInitFunctionDecl
// cnm : CPP`Identifier
// sl : TagList    (seq of CPP`Identifier)
// tenv : TagEnv   (map CPP`Identifier to CPP`Name)
// ==> CPP`MemberSpecifier
TYPE_CPP_MemberSpecifier vdmcg::GenInitFunctionDecl(const TYPE_CPP_Identifier & cnm,
                                                    const SEQ<TYPE_CPP_Identifier> & sl,
                                                    const MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> & tenv)
{
  SEQ<TYPE_CPP_ArgumentDeclaration> arglist;
  size_t len_sl = sl.Length();
  for (size_t i = 1; i <= len_sl; i++) {
    TYPE_CPP_Identifier id (sl[i]);
    TYPE_CPP_Name clnm (tenv[id]);
    SEQ<TYPE_CPP_DeclSpecifier> decl_l;
    decl_l.ImpAppend(vdm_BC_GenTypeSpecifier(quote_CONST));
    decl_l.ImpAppend(vdm_BC_GenTypeSpecifier(clnm));
    TYPE_CPP_ArgDecl arg (vdm_BC_GenArgDecl(decl_l,
                                            vdm_BC_GenRef (
                                              vdm_BC_GenIdentifier(ASTAUX::MkId(L"p").ImpConc(StringNumber(i))))));
    arglist.ImpAppend(arg);
  }
  return vdm_BC_GenMemberSpec(type_dL().ImpAppend(vdm_BC_GenTypeSpecifier (cnm)),
                              vdm_BC_GenRef(vdm_BC_GenFctDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Init")), arglist)));
}

// GenInitFunctionDef
// cnm : CPP`QualifiedClassName
// sl : TagList   (seq of CPP`Identifier)
// tenv : TagEnv  (map CPP`Identifier to CPP`Name);
// ==> CPP`FunctionDefinition
TYPE_CPP_FunctionDefinition vdmcg::GenInitFunctionDef(const TYPE_CPP_Identifier & cnm,
                                                      const SEQ<TYPE_CPP_Identifier> & sl,
                                                      const MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> & tenv)
{
  SEQ<TYPE_CPP_ArgumentDeclaration> arglist;
  SEQ<TYPE_CPP_Stmt> stmtl;
  size_t len_sl = sl.Length();
  for (size_t i = 1; i <= len_sl; i++) {
    const TYPE_CPP_Identifier & id (sl[i]);
    const TYPE_CPP_Name & clnm (tenv[id]);
    SEQ<TYPE_CPP_DeclSpecifier> decl_l;
    decl_l.ImpAppend(vdm_BC_GenTypeSpecifier(quote_CONST));
    decl_l.ImpAppend(vdm_BC_GenTypeSpecifier(clnm));
    TYPE_CPP_ArgDecl arg (vdm_BC_GenArgDecl(decl_l,
                                            vdm_BC_GenRef (
                                              vdm_BC_GenIdentifier(ASTAUX::MkId(L"p").ImpConc(StringNumber(i))))));
    arglist.ImpAppend(arg);

    stmtl.ImpAppend(GenRecSetFieldonThis(vdm_BC_GenIntegerLit(i),
                                         vdm_BC_GenIdentifier(ASTAUX::MkId(L"p").ImpConc(StringNumber(i)))));
  }
  stmtl.ImpAppend (vdm_BC_GenReturnStmt (vdm_BC_GenIndirection (GenThis())));

  //TYPE_CPP_FctDecl decl(vdm_BC_GenFctDecl(vdm_BC_GenQualifiedName2(cnm, vdm_BC_GenIdentifier(ASTAUX::MkId(L"Init"))),
  TYPE_CPP_FctDecl decl(vdm_BC_GenFctDecl(vdm_BC_GenQualifiedName(cnm, vdm_BC_GenIdentifier(ASTAUX::MkId(L"Init"))),
                                          arglist));

  return vdm_BC_GenFctDef(type_dL().ImpAppend(vdm_BC_GenTypeSpecifier (cnm)),
                          vdm_BC_GenRef (decl), nil,
                          vdm_BC_GenBlock(stmtl));
}

// GenCompositeTypeTagId
// nm : AS`Name
// -> CPP`Identifier
TYPE_CPP_Identifier vdmcg::GenCompositeTypeTagId(const TYPE_AS_Name & nm)
{
  return GenTypeTagCppId(vdm_BC_GenIdentifier(ASTAUX::MkId(L"TYPE_").ImpConc(Name2Id(GenQName(nm)))));
}

// GenCompositeClassDecl
// nm : AS`Name
// acc : [AS`Access]
// cpptpid : CPP`Identifier
// taglist : TagList   (seq of CPP`Identifier)
// tagenv : TagEnv     (map CPP`Identifier to CPP`Name)
// dcenv : DCEnv       (map CPP`Identifier to bool)
// tps : seq of REP`TypeRep
// ==> seq of CPP`IdentDeclaration
SEQ<TYPE_CPP_IdentDeclaration> vdmcg::GenCompositeClassDecl(const TYPE_AS_Name & nm,
                                                            const Generic & acc,
                                                            const TYPE_CPP_Identifier & cpptpid,
                                                            const SEQ<TYPE_CPP_Identifier> & taglist,
                                                            const MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> & tagenv,
                                                            const MAP<TYPE_CPP_Identifier,Bool> & dcenv,
                                                            const SEQ<TYPE_REP_TypeRep> & tps)
{
  SEQ<TYPE_CPP_MemberDeclaration> pub_ml, priv_ml, fctdef;

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    SEQ<TYPE_CPP_PackageName> inter_l;
    if (this->record_types.Dom().InSet(nm)) {
      SEQ<TYPE_AS_Name> rn (this->record_types[nm]);
      size_t len_rn = rn.Length();
      for (size_t idx = 1; idx <= len_rn; idx++) {
        inter_l.ImpAppend(vdm_BC_GenSimplePackageName(Name2Id(rn[idx])));
      }
    }

    TYPE_CPP_Identifier idnm (vdm_BC_GenIdentifier(Name2Id(nm)));
    TYPE_CPP_Identifier cnm (idnm);
    TYPE_CPP_Identifier cn (GenRecordType(Nil()).get_tp());

    SEQ<TYPE_CPP_BaseSpecifier> bs;
    bs.ImpAppend(vdm_BC_GenClass2Package(cn));
    SEQ<TYPE_CPP_Modifier> mods;
    mods.ImpAppend(vdm_BC_GenModifier(quote_STATIC));

    if (acc != nil) {
      mods.ImpPrepend(JavaAccess(acc));
    }

    SEQ<TYPE_CPP_Annotation> annos;
    TYPE_CPP_ClassHead ch (vdm_BC_GenJavaClassHead(annos, mods, idnm, SEQ<TYPE_CPP_BaseSpecifier>(),
                                                   inter_l.ImpConc(bs), nil));
    pub_ml.ImpConc(GenIdentDecls(taglist, tagenv));
    pub_ml.ImpAppend(GenClassDefaultConstructorDecl((Int)TAG_quote_COMPOSITE, idnm, taglist));

    if (!taglist.IsEmpty()) {
      pub_ml.ImpAppend(GenClassConstrDecl(idnm, taglist, tagenv));
    }

    pub_ml.ImpAppend(GenCloneMethod(idnm, taglist, tagenv));
    pub_ml.ImpAppend(GenAsciiMethod(idnm, taglist, tagenv));
    pub_ml.ImpAppend(GenEqualsMethod(idnm, taglist, tagenv, dcenv));
    pub_ml.ImpAppend(GenHashMethod(idnm, taglist, tagenv, dcenv));

    SEQ<TYPE_CPP_DeclSpecifier> sp;
    sp.ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenClassSpecifier(ch, pub_ml)));

    SEQ<TYPE_CPP_IdentDeclaration> cl;
    cl.ImpAppend(vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(), sp, SEQ<TYPE_CPP_InitDeclarator>()));
    return cl;
  }
  else
#endif // VDMPP
  {
    TYPE_CPP_Identifier cnm (cpptpid);
    TYPE_CPP_Identifier cn (GenRecordType(Nil()).get_tp());

    SEQ<TYPE_CPP_BaseSpecifier> bs;
    bs.ImpAppend(vdm_BC_GenAccBaseSpec(cn));
    TYPE_CPP_ClassHead ch (vdm_BC_GenClassHead(cpptpid, bs));

    if (!taglist.IsEmpty()) {
      fctdef.ImpAppend(GenInitFunctionDef(cnm, taglist, tagenv));
    }

    pub_ml.ImpAppend(GenClassDefaultConstructorDecl((Int)TAG_quote_COMPOSITE, cpptpid, taglist));

    if (!taglist.IsEmpty()) {
      pub_ml.ImpAppend(GenInitFunctionDecl(cpptpid, taglist, tagenv));
    }
    pub_ml.ImpAppend(GenCommonConstrDecl(cpptpid, cn));

    pub_ml.ImpAppend(GenGetTypeNameDecl(cpptpid));
    pub_ml.ImpConc(GenMemFctMacros((Int) TAG_quote_COMPOSITE));

    size_t len_taglist = taglist.Length();
    for (size_t i = 1; i <= len_taglist; i++) {
      TYPE_CPP_Identifier id (taglist[i]);

      pub_ml.ImpAppend(GenGetFunctionDecl(id, tagenv[id]));
      pub_ml.ImpAppend(GenSetFunctionDecl(id, tagenv[id]));

      fctdef.ImpAppend(GenGetFunctionDef((Int) TAG_quote_COMPOSITE, id, tagenv[id], i, cnm, tps[i]));
      fctdef.ImpAppend(GenSetFunctionDef(id, tagenv[id], i, cnm));
    }

#ifdef VDMSL
    TYPE_AS_Name qnm (nm.get_ids().Length() == 1 ? GenQName(nm) : nm);
    if (!this->imported_types.InSet(qnm)) {
      this->mod_fctdef.ImpConc(fctdef);
    }
#endif // VDMSL
#ifdef VDMPP
    this->mod_fctdef.ImpConc(fctdef);
#endif // VDMPP

    SEQ<TYPE_CPP_DeclSpecifier> sp;
    type_dL mem;
    mem.ImpConc(vdm_BC_GenPrivate(priv_ml));
    mem.ImpConc(vdm_BC_GenPublic(pub_ml));
    sp.ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenClassSpecifier(ch, mem)));

    SEQ<TYPE_CPP_IdentDeclaration> cl;
    cl.ImpAppend(vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(), sp, SEQ<TYPE_CPP_InitDeclarator>()));
    return cl;
  }
}

// GenProductClassDecl
// cpptpid : CPP`Identifier
// taglist : TagList
// tagenv : TagEnv
// tps : seq of REP`TypeRep
// ==> seq of CPP`IdentDeclaration
SEQ<TYPE_CPP_IdentDeclaration> vdmcg::GenProductClassDecl(const TYPE_CPP_Identifier & cpptpid,
                                                          const SEQ<TYPE_CPP_Identifier> & taglist,
                                                          const MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> & tagenv,
                                                          const SEQ<TYPE_REP_TypeRep> & tps)
{
  SEQ<TYPE_CPP_MemberDeclaration> pub_ml, priv_ml, fctdef;

  TYPE_CPP_Identifier cnm (cpptpid);
  TYPE_CPP_Identifier cn (GenProductType().get_tp());
  SEQ<TYPE_CPP_BaseSpecifier> bs;
  bs.ImpAppend(vdm_BC_GenAccBaseSpec(cn));
  TYPE_CPP_ClassHead ch (vdm_BC_GenClassHead(cpptpid, bs));

  fctdef.ImpAppend(GenInitFunctionDef(cnm, taglist, tagenv));
  pub_ml.ImpAppend(GenClassDefaultConstructorDecl( (Int) TAG_quote_PRODUCT, cpptpid, taglist));
  pub_ml.ImpAppend(GenInitFunctionDecl(cpptpid, taglist, tagenv));
  pub_ml.ImpAppend(GenCommonConstrDecl(cpptpid, cn));
  pub_ml.ImpAppend(GenGetTypeNameDecl(cpptpid));
  pub_ml.ImpConc(GenMemFctMacros((Int) TAG_quote_PRODUCT));

  size_t len_taglist = taglist.Length();
  for (size_t i = 1; i <= len_taglist; i++) {
    TYPE_CPP_Identifier id (taglist[i]);

    pub_ml.ImpAppend(GenGetFunctionDecl(id, (Generic)tagenv[id]));
    pub_ml.ImpAppend(GenSetFunctionDecl(id, (Generic)tagenv[id]));

    fctdef.ImpAppend(GenGetFunctionDef((Int) TAG_quote_PRODUCT, id, (Generic)tagenv[id], i, cnm, tps[i]));
    fctdef.ImpAppend(GenSetFunctionDef(id, (Generic)tagenv[id], i, cnm));
  }

  this->anonym_fctdef.ImpConc(GenTypeDefIfDef(cpptpid, fctdef));

  type_dL mem_l;
  mem_l.ImpConc(vdm_BC_GenPrivate(priv_ml));
  mem_l.ImpConc(vdm_BC_GenPublic(pub_ml));

  SEQ<TYPE_CPP_IdentDeclaration> cl;
  cl.ImpAppend(vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(),
                                          type_dL().ImpAppend(
                                            vdm_BC_GenTypeSpecifier(vdm_BC_GenClassSpecifier(ch, mem_l))),
                                          Sequence()));
  return cl;
}

// GenProductDecl
// cpptpid : CPP`Identifier
// ptp : REP`TypeRep
// ==> seq of CPP`IdentDeclaration
SEQ<TYPE_CPP_IdentDeclaration> vdmcg::GenProductDecl(const TYPE_CPP_Identifier& cpptpid,
                                                     const TYPE_REP_ProductTypeRep & ptp)
{
  SEQ<TYPE_REP_TypeRep> fl (ptp.GetSequence(pos_REP_ProductTypeRep_tps));

  MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> tagenv;
  SEQ<TYPE_CPP_Identifier> tagnames;
  SEQ<TYPE_REP_TypeRep> tps;
  size_t len_fl = fl.Length();
  for (size_t i = 1; i <= len_fl; i++) {
    TYPE_REP_TypeRep tp (fl[i]);
    TYPE_REP_TypeRep c_type (CleanFlatType(tp));
    TYPE_CPP_Name cpptp;
#ifdef VDMPP
    if (c_type.Is(TAG_TYPE_REP_ObjRefTypeRep)) {
      cpptp = GenVDMTpName(c_type, (Int) TAG_quote_H, Set());
    }
    else
#endif //VDMPP
    {
      cpptp = GenVDMTpName(tp, (Int) TAG_quote_H, Set());
    }

    TYPE_CPP_Identifier cppid (vdm_BC_GenIdentifier(StringNumber(i)));
    tagnames.ImpAppend(cppid);
    tagenv.ImpModify(cppid, cpptp); // map CPP`Identifier to CPP`Name
    tps.ImpAppend(c_type);
  }
  return GenProductClassDecl(cpptpid, tagnames, tagenv, tps);
}

// GenProductType
// tp : REP`TypeRep
// names : set of AS`Name
// ==> Identifier
TYPE_AS_Id vdmcg::GenProductType(const TYPE_REP_ProductTypeRep & tp, const SET<TYPE_AS_Name> & names)
{
  TYPE_AS_Id id;
  const SEQ<TYPE_REP_TypeRep> & tpl (tp.GetSequence(pos_REP_ProductTypeRep_tps));
  size_t len_tpl = tpl.Length();
  for (size_t idx = 1; idx <= len_tpl; idx++) {
    id.ImpConc(GenCppTpDecl(tpl[idx], names));
  }
  id.ImpConc(StringNumber(len_tpl));
  id.ImpConc(ASTAUX::MkId(L"P"));

  if (! this->cppenv.DomExists(id)) {
    DeclAnonymCppTp(id, GenProductDecl(Id2CppGTpId(id), tp));
  }

  return id;
}

// GenCompositeDecl
// nm : AS`Name
// acc : [AS`Access]
// apptpid : CPP`Identifier
// str : REP`TypeRep
// ==> seq of CPP`IdentDeclaration
SEQ<TYPE_CPP_IdentDeclaration> vdmcg::GenCompositeDecl(const TYPE_AS_Name& nm,
                                                       const Generic & acc,
                                                       const TYPE_CPP_Identifier & cpptpid,
                                                       const TYPE_REP_CompositeTypeRep & ctr)
{
  const SEQ<TYPE_REP_FieldRep> & fl (ctr.GetSequence(pos_REP_CompositeTypeRep_fields));

  SET<TYPE_AS_Id> selnames;
  size_t len_fl = fl.Length();
  for (size_t idx = 1; idx <= len_fl; idx++) {
    const TYPE_REP_FieldRep & fls (fl[idx]);
    if (!fls.GetField(pos_REP_FieldRep_sel).IsNil()) {
      const TYPE_AS_Name & selnm (fls.GetRecord(pos_REP_FieldRep_sel));
      const TYPE_AS_Ids & ids (selnm.GetSequence(pos_AS_Name_ids));
      selnames.Insert(ids[1]);
    }
  }

  MAP<TYPE_CPP_Identifier,TYPE_CPP_Name> tagenv;
  MAP<TYPE_CPP_Identifier,Bool> dcenv;
  SEQ<TYPE_CPP_Identifier> tagnames;
  SEQ<TYPE_REP_TypeRep> tps;
  for (size_t i = 1; i <= len_fl; i++) {
    const TYPE_REP_FieldRep & fr (fl[i]);

    const Generic & sel (fr.GetField(pos_REP_FieldRep_sel));
    const TYPE_REP_TypeRep & tp (fr.GetRecord(pos_REP_FieldRep_tp));
    const Bool & dc (fr.GetBool(pos_REP_FieldRep_dc));
    TYPE_REP_TypeRep c_type (RemoveNil(CleanFlatType(tp)));

    TYPE_CPP_Name cpptp;
#ifdef VDMPP
    if (c_type.Is(TAG_TYPE_REP_ObjRefTypeRep)) {
      cpptp = GenVDMTpName(c_type, (Int) TAG_quote_H, Set());
    }
    else
#endif //VDMPP
    {
      if (vdm_CPP_isCPP()) {
        cpptp = GenVDMTpName(tp, (Int) TAG_quote_H, Set());
      }
      else {
        if (c_type.Is(TAG_TYPE_REP_UnionTypeRep) && IsCompositeType(c_type)) {
          cpptp = GenVDMTpName(tp, (Int) TAG_quote_H, Set());
        }
        else {
          cpptp = GenVDMTpName(c_type, (Int) TAG_quote_H, Set());
        }
      }
    }

    TYPE_CPP_Identifier cppid;
    if (vdm_CPP_isCPP()) {
      cppid = vdm_BC_GenIdentifier(sel.IsNil() ? StringNumber(i) : Name2Id(sel));
    }
    else {
      cppid = vdm_BC_GenIdentifier(sel.IsNil() ? GetUniqueName(ASTAUX::MkId(L"f").ImpConc(StringNumber(i)), selnames)
                                               : Name2Id(sel));
    }

    tagnames.ImpAppend(cppid);
    tagenv.ImpModify(cppid, cpptp);
    dcenv.ImpModify(cppid, dc);
    tps.ImpAppend(c_type);
  }
  return GenCompositeClassDecl(nm, acc, cpptpid, tagnames, tagenv, dcenv, tps);
}

// GetUniqueName
// stem : seq of char
// existingNames : set of (seq of char)
// -> seq of char
TYPE_AS_Id vdmcg::GetUniqueName(const TYPE_AS_Id & stem, const SET<TYPE_AS_Id> & existingNames)
{
  TYPE_AS_Id newstem (stem);
  while(existingNames.InSet(newstem)) {
    TYPE_AS_Id tmp (ASTAUX::MkId(L"f").ImpConc(newstem));
    newstem = tmp;
  }
  return newstem;
}

// GenCompositeType
// tp : REP`CompositeTypeRep
// acc : [AS`Access]
// names : set of AS`Name
// ==> Identifier
TYPE_AS_Id vdmcg::GenCompositeType(const TYPE_REP_CompositeTypeRep & tp,
                                   const Generic & acc,
                                   const SET<TYPE_AS_Name> & names)
{
  TYPE_AS_Name nm (tp.get_nm());
  TYPE_AS_Name qnm (GenQName(nm));
  TYPE_AS_Id nid (Name2Id(qnm));
  TYPE_AS_Id id (StringNumber(nid.Length()).ImpConc(nid).ImpConc(ASTAUX::MkId(L"C")));

  TYPE_AS_Ids modnm_l;
  TYPE_AS_Id modnm_t;
  if (!this->modnm.IsNil()) {
    modnm_l = TYPE_AS_Name(this->modnm).get_ids();
    modnm_t = modnm_l.Hd();
  }

  TYPE_AS_Ids qnm_l (qnm.get_ids());
  TYPE_AS_Id qnm_t (qnm_l.Hd());
  if (!this->modnm.IsNil() && (modnm_t == qnm_t )) {
    if ( !this->cppenv.DomExists(id) ) {
      AddNewCppTp(id, Sequence());
      DeclModCppTp(id, GenCompositeDecl(nm, acc, Id2CppGTpId(id), tp));
    }
  }
  else {
    AddNewCppTp(id, Sequence());
  }

  return id;
}

// GenTemplateClass
// what : (<SET>|<SEQ>|<MAP>)
// cpptpid : CPP`Identifier
// tcn : CPP`TemplateClassName
// +> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenTemplateClass(const Int & what,
                                       const TYPE_CPP_Identifier &cpptpid,
                                       const TYPE_CPP_TemplateClassName& tcn)
{
  SEQ<TYPE_CPP_BaseSpecifier> bs;
  bs.ImpAppend(vdm_BC_GenAccTemplateBaseSpec(tcn));
  TYPE_CPP_ClassHead ch (vdm_BC_GenClassHead(cpptpid, bs));

  SEQ<TYPE_CPP_MemberDeclaration> pub_l;
  pub_l.ImpAppend(GenDefaultConstrDecl(cpptpid, tcn));
  pub_l.ImpAppend(GenTemplateConstrDecl(cpptpid, tcn));
  pub_l.ImpAppend(GenCommonConstrDecl(cpptpid, tcn));
  pub_l.ImpAppend(GenGetTypeNameDecl(cpptpid));
  pub_l.ImpConc(GenMemFctMacros(what));

  type_dL arg_l;
  arg_l.ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenClassSpecifier(ch, (Generic)vdm_BC_GenPublic(pub_l))));

  TYPE_CPP_CPPAS res;
  res.ImpAppend(vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(), arg_l, Sequence()));

  return res;
}

// GenMapDecl
// spptpid : CPP`Identifier
// d : CPP`Identifier
// r : CPP`Identifier
// +>  CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenMapDecl(const TYPE_CPP_Identifier& cpptpid,
                                 const TYPE_CPP_Identifier& d,
                                 const TYPE_CPP_Identifier& r)
{
  TYPE_CPP_TemplateClassName tcn(vdm_BC_GenTemplateClassName(
                                   vdm_BC_GenIdentifier(ASTAUX::MkId(L"MAP")),
                                   SEQ<TYPE_CPP_Identifier>().ImpAppend(d).ImpAppend(r)));

  return GenTemplateClass((Int) TAG_quote_MAP, cpptpid, tcn);
}

// GenMapType
// dt : REP`TypeRep
// rt : REP`TypeRep
// names : set of AS`Name
// ==> Identifier
TYPE_AS_Id vdmcg::GenMapType(const TYPE_REP_TypeRep& dt, const TYPE_REP_TypeRep& rt, const SET<TYPE_AS_Name> & names)
{
  TYPE_AS_Id di (GenCppTpDecl(dt, names));
  TYPE_AS_Id ri (GenCppTpDecl(rt, names));

  TYPE_AS_Id id (di);
  id.ImpConc(ri).ImpConc(ASTAUX::MkId(L"M"));

  if (!this->cppenv.DomExists(id)) {
    DeclAnonymCppTp(id, GenMapDecl(Id2CppGTpId(id), Id2CppGTpId(di), Id2CppGTpId(ri)));
  }
  return id;
}

// GenSetDecl
// cpptpid : CPP`Identifier
// i : CPP`Identifier
// +>  CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenSetDecl(const TYPE_CPP_Identifier & cpptpid, const TYPE_CPP_Identifier & i)
{
  TYPE_CPP_TemplateClassName tcn (vdm_BC_GenTemplateClassName(vdm_BC_GenIdentifier(ASTAUX::MkId(L"SET")),
                                                              SEQ<TYPE_CPP_Identifier>().ImpAppend(i)));

  return GenTemplateClass((Int) TAG_quote_SET, cpptpid, tcn);
}

// GenSetType
// tp : REP`SetTypeRep
// names : set of AS`Name
// ==> Identifier
TYPE_AS_Id vdmcg::GenSetType(const TYPE_REP_SetTypeRep & tp, const SET<TYPE_AS_Name> & names)
{
  TYPE_REP_TypeRep t (tp.get_elemtp());
  TYPE_AS_Id i (GenCppTpDecl(t, names));
  TYPE_AS_Id id (i);
  id.ImpConc(ASTAUX::MkId(L"S"));

  if (!this->cppenv.DomExists(id)) {
    DeclAnonymCppTp(id, GenSetDecl(Id2CppGTpId(id), Id2CppGTpId(i)));
#ifdef VDMPP
    if (t.Is(TAG_TYPE_REP_CompositeTypeRep)) {
      const TYPE_AS_Name & name (t.GetRecord(pos_REP_CompositeTypeRep_nm));
      if (name.GetSequence (pos_AS_Name_ids).Length() == 2) {
         TYPE_CPP_Identifier cpid (Id2CppGTpId(i));
         if  ((ASTAUX::GetFirstName(name) != this->modnm) && !this->anonym_comp_decl.Elems().InSet(cpid)) {
           this->anonym_comp_decl.ImpAppend(cpid);
         }
      }
    }
#endif // VDMPP
  }
  return id;
}

// GenSeqDecl
// cpptpid : CPP`Identifier
// i : CPP`Identifier
// +>  CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenSeqDecl(const TYPE_CPP_Identifier & cpptpid, const TYPE_CPP_Identifier & i)
{
  TYPE_CPP_TemplateClassName tcn (vdm_BC_GenTemplateClassName(vdm_BC_GenIdentifier(ASTAUX::MkId(L"SEQ")),
                                                              SEQ<TYPE_CPP_Identifier>().ImpAppend(i)));

  return GenTemplateClass((Int) TAG_quote_SEQ, cpptpid, tcn);
}

// GenSeqType
// tp : REP`SeqTypeRep
// names : set of AS`Name
// ==> Identifier
TYPE_AS_Id vdmcg::GenSeqType(const TYPE_REP_SeqTypeRep & tp, const SET<TYPE_AS_Name> & names)
{
  const TYPE_REP_TypeRep & t (tp.GetRecord(pos_REP_SeqTypeRep_elemtp));
  TYPE_AS_Id i (GenCppTpDecl(t, names));
  TYPE_AS_Id id (i);
  id.ImpConc(ASTAUX::MkId(L"L"));

  if (!this->cppenv.DomExists(id)) {
    DeclAnonymCppTp(id, GenSeqDecl(Id2CppGTpId(id), Id2CppGTpId(i)));
#ifdef VDMPP
    if (t.Is(TAG_TYPE_REP_CompositeTypeRep)) {
      const TYPE_AS_Name & name (t.GetRecord(pos_REP_CompositeTypeRep_nm));
      if (name.GetSequence (pos_AS_Name_ids).Length() == 2) {
         TYPE_CPP_Identifier cpid (Id2CppGTpId(i));
         if  ((ASTAUX::GetFirstName(name) != this->modnm) && !this->anonym_comp_decl.Elems().InSet(cpid)) {
           this->anonym_comp_decl.ImpAppend(cpid);
         }
      }
    }
#endif // VDMPP
  }
  return id;
}

// StringAsBefore
bool vdmcg::StringAsBefore(wstring & id, const wstring & s)
{
  bool cont (true);
  if (id.compare(L"") == 0) {
    id = s;
  }
  if (id.compare(s) != 0) {
    cont = false;
  }
  return cont;
}

// GenUnionType
// tp : REP`TypeRep
// utp : REP`TypeRep
// names : set of AS`Name
// ==> Identifier
TYPE_AS_Id vdmcg::GenUnionType(const TYPE_REP_UnionTypeRep & tp,
                               const TYPE_REP_UnionTypeRep & utp,
                               const SET<TYPE_AS_Name> & names)
{
  SET<TYPE_REP_TypeRep> ts (tp.GetSet(pos_REP_UnionTypeRep_tps));
  SET<TYPE_REP_TypeRep> uts (utp.GetSet(pos_REP_UnionTypeRep_tps));

  Generic t_g;
  for (bool bb = uts.First(t_g); bb; bb = uts.Next(t_g)) {
    TYPE_REP_TypeRep tp (t_g);
    if (tp.Is(TAG_TYPE_REP_InvTypeRep)) {
      tp = tp.GetRecord(pos_REP_InvTypeRep_shape);
    }

    if (tp.Is(TAG_TYPE_REP_TypeNameRep)) {
      const TYPE_AS_Name & nm (tp.GetRecord(pos_REP_TypeNameRep_nm));

      Generic lot_tp (LOT(tp));
      if (!lot_tp.IsNil()) {
        if (IsTpRecursive(GenQName(nm), lot_tp, names)) {
          return ASTAUX::MkId(L"F");
        }
      }
    }
  }

  TYPE_AS_Id tmp_id;
  Generic gg;
  for (bool cc = ts.First(gg); cc; cc = ts.Next(gg)) {
    tmp_id = GenCppTpDecl(gg, names);
  }

  wstring id;
  id = L"";
  bool cont = true;
  Generic g;
  for (bool dd = ts.First(g); dd && cont; dd = ts.Next(g) ) {
    TYPE_REP_TypeRep tp (g);
    if (tp.Is(TAG_TYPE_REP_InvTypeRep))
      tp = tp.GetRecord(pos_REP_InvTypeRep_shape);

    switch (tp.GetTag()) {
      case TAG_TYPE_REP_SetTypeRep: {
        cont = StringAsBefore(id, wstring(L"s")); // id = L"s";
        break;
      }
      case TAG_TYPE_REP_SeqTypeRep: {
        cont = StringAsBefore(id, wstring(L"l")); // id = L"l";
        break;
      }
      case TAG_TYPE_REP_GeneralMapTypeRep:
      case TAG_TYPE_REP_InjectiveMapTypeRep: {
        cont = StringAsBefore(id, wstring(L"m")); // id = L"m";
        break;
      }
      case TAG_TYPE_REP_CompositeTypeRep: {
        cont = StringAsBefore(id, wstring(L"d")); // id = L"d";
        break;
      }
      case TAG_TYPE_REP_ProductTypeRep: {
        cont = StringAsBefore(id, wstring(L"p")); // id = "p";
        break;
      }
      case TAG_TYPE_REP_NumericTypeRep: {
        cont = StringAsBefore(id, wstring(L"n")); // id = "n";
        break;
      }
// 20150717 --> Generic
//      case TAG_TYPE_REP_QuoteTypeRep: {
//        cont = StringAsBefore(id, wstring(L"Q")); // id = "Q";
//        break;
//      }
// <-- 20150717
#ifdef VDMPP
      case TAG_TYPE_REP_ObjRefTypeRep: {
        cont = StringAsBefore (id, wstring(L"o")); // id = "o"
        break;
      }
#endif //VDMPP
      default : {
        id = L"U";
        cont = false;
      }
    }
  }
  if ( (!cont) || id.empty() ) {
    id = L"U";
  }
  return ASTAUX::MkId(id);
}

// 20110627 -->
// RemoveInvType
// tp : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep vdmcg::RemoveInvType(const TYPE_REP_TypeRep & tp)
{
  switch (tp.GetTag()) {
    case TAG_TYPE_REP_InvTypeRep: {
      return RemoveInvType(tp.GetRecord(pos_REP_InvTypeRep_shape));
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tps (tp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> new_tps;
      Generic t;
      for (bool bb = tps.First(t); bb; bb = tps.Next(t)) {
        new_tps.Insert(RemoveInvType(t));
      }
      if (new_tps.Card() == 1) {
        return new_tps.GetElem();
      }
      else {
        return mk_REP_UnionTypeRep(new_tps);  
      }
    }
    case TAG_TYPE_REP_SetTypeRep: {
      return mk_REP_SetTypeRep(RemoveInvType(tp.GetRecord(pos_REP_SetTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_EmptySetTypeRep: {
      return mk_REP_EmptySetTypeRep(RemoveInvType(tp.GetRecord(pos_REP_EmptySetTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return mk_REP_SeqTypeRep(RemoveInvType(tp.GetRecord(pos_REP_SeqTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_EmptySeqTypeRep: {
      return mk_REP_EmptySeqTypeRep(RemoveInvType(tp.GetRecord(pos_REP_EmptySeqTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return mk_REP_GeneralMapTypeRep(RemoveInvType(tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom)),
                                      RemoveInvType(tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng)));
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return mk_REP_InjectiveMapTypeRep(RemoveInvType(tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom)),
                                        RemoveInvType(tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng)));
    }
    case TAG_TYPE_REP_EmptyMapTypeRep: {
      return mk_REP_EmptyMapTypeRep(RemoveInvType(tp.GetRecord(pos_REP_EmptyMapTypeRep_mapdom)),
                                    RemoveInvType(tp.GetRecord(pos_REP_EmptyMapTypeRep_maprng)));
    }
    case TAG_TYPE_REP_ProductTypeRep : {
      const SEQ<TYPE_REP_TypeRep> & fl (tp.GetSequence(pos_REP_ProductTypeRep_tps));
      SEQ<TYPE_REP_TypeRep> fl_new;
      size_t len_fl = fl.Length();
      for (size_t idx = 1; idx <= len_fl; idx++) {
        fl_new.ImpAppend(RemoveInvType(fl[idx]));
      }
      return mk_REP_ProductTypeRep(fl_new);
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & fndom (tp.GetSequence(pos_REP_PartialFnTypeRep_fndom));
      SEQ<TYPE_REP_TypeRep> new_fndom;
      size_t len_fndom = fndom.Length();
      for (size_t idx = 1; idx <= len_fndom; idx++)
        new_fndom.ImpAppend(RemoveInvType(fndom[idx]));
      return mk_REP_PartialFnTypeRep(new_fndom, RemoveInvType(tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng)));
    }
    case TAG_TYPE_REP_TotalFnTypeRep : {
      const SEQ<TYPE_REP_TypeRep> & fndom (tp.GetSequence(pos_REP_TotalFnTypeRep_fndom));
      SEQ<TYPE_REP_TypeRep> new_fndom;
      size_t len_fndom = fndom.Length();
      for (size_t idx = 1; idx <= len_fndom; idx++)
        new_fndom.ImpAppend(RemoveInvType(fndom[idx]));
      return mk_REP_TotalFnTypeRep(new_fndom, RemoveInvType(tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng)));
    }
    case TAG_TYPE_REP_OpTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & opdom (tp.GetSequence(pos_REP_OpTypeRep_Dom));
      SEQ<TYPE_REP_TypeRep> new_opdom;
      size_t len_opdom = opdom.Length();
      for (size_t idx = 1; idx <= len_opdom; idx++)
        new_opdom.ImpAppend(RemoveInvType(opdom[idx]));
      return mk_REP_OpTypeRep(new_opdom, RemoveInvType(tp.GetRecord(pos_REP_OpTypeRep_Rng)));
    }
    default: {
      return tp;
    }
  }
}
// <-- 20110627

// AddUnionType
// enm : AS`Name
// nm : AS`Name
// ==> ()
void vdmcg::AddUnionType(const TYPE_AS_Name & enm, const TYPE_AS_Name & nm)
{
  SEQ<TYPE_AS_Name> rn;
  if (this->union_types.Dom().InSet(enm)) {
    rn = this->union_types[enm];
    if (! rn.Elems().InSet(nm)) {
      rn.ImpAppend(nm);
    }
  }
  else {
    rn.ImpAppend(nm);
  }
  this->union_types.ImpModify(enm, rn);
}

// AddRecordType
// enm : AS`Name
// nm : AS`Name
// ==> ()
void vdmcg::AddRecordType(const TYPE_AS_Name & enm, const TYPE_AS_Name & nm)
{
  SEQ<TYPE_AS_Name> rn;
  if (this->record_types.Dom().InSet(enm)) {
    rn = this->record_types[enm];
    if (! rn.Elems().InSet(nm)) {
      rn.ImpAppend(nm);
    }
  }
  else {
    rn.ImpAppend(nm);
  }

  this->record_types.ImpModify(enm,rn);
}

#ifdef VDMPP
// GenUnionInterfaceDecl
// nm : AS`Name
// ==> seq of CPP`IdentDeclaration
SEQ<TYPE_CPP_IdentDeclaration> vdmcg::GenUnionInterfaceDecl(const TYPE_AS_Name & nm)
{
  TYPE_REP_TypeNameRep tnr;
  tnr.set_nm(nm);
  this->known_union_types.Insert(tnr);

  SEQ<TYPE_CPP_PackageName> inter_l;
  if (this->union_types.Dom().InSet(nm)) {
    SEQ<TYPE_AS_Name> rn (this->union_types[nm]);
    size_t len_rn = rn.Length();
    for (size_t idx = 1; idx <= len_rn; idx++) {
     inter_l.ImpAppend(vdm_BC_GenSimplePackageName(Name2Id(rn[idx])));
    }
  }

  TYPE_CPP_Identifier idnm (vdm_BC_GenIdentifier(Name2Id(nm)));
  TYPE_CPP_Modifier pub (vdm_BC_GenModifier(quote_PUBLIC));
  TYPE_CPP_Modifier stat (vdm_BC_GenModifier(quote_STATIC));
  TYPE_CPP_InterfaceHead ch (vdm_BC_GenJavaInterfaceHead(
                               SEQ<TYPE_CPP_Annotation>(),
                               SEQ<TYPE_CPP_Modifier>().ImpAppend(pub).ImpAppend(stat),
                               idnm, inter_l, nil));
  TYPE_CPP_IdentDeclaration cl (vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(),
                                  type_dL().ImpAppend(
                                    vdm_BC_GenTypeInterfaceSpecifier(
                                      ch, type_dL())),
                                  nil));
  return SEQ<TYPE_CPP_IdentDeclaration>().ImpAppend(cl);
}

// GenObjRefType
// otp : REP`ObjRefTypeRep
// - : set of AS`Name
// ==> Identifier
TYPE_AS_Id vdmcg::GenObjRefType(const TYPE_REP_ObjRefTypeRep& otp, const SET<TYPE_AS_Name> & )
{
  const TYPE_AS_Name & nm (otp.GetRecord(pos_REP_ObjRefTypeRep_nm));
  // Difference from spec here as we have helper functions for AS`Name
  TYPE_AS_Id nid (Name2Id(GiveFirstName(nm)));
  TYPE_AS_Id id (StringNumber(nid.Length()).ImpConc(nid).ImpConc(ASTAUX::MkId(L"R")));
  return id;
}

// GenObjRefClass
// classnm : AS`Name
// inh : set of AS`Name
// ==> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenObjRefClass(const TYPE_AS_Name& classnm, const SET<TYPE_AS_Name> & inh)
{
  SEQ<TYPE_CPP_BaseSpecifier> bs;
  if (inh.IsEmpty()) {
    bs.ImpAppend(vdm_BC_GenAccVirBaseSpec(vdm_BC_GenIdentifier(ASTAUX::MkId(L"ObjectRef"))));
  }
  else {
    SET<TYPE_AS_Name> inh_q (inh);
    Generic g;
    for (bool bb = inh_q.First(g); bb; bb= inh_q.Next(g) ) {
      TYPE_AS_Name c (g);
      TYPE_AS_Name nm (GiveFirstName(c));
      bs.ImpAppend(vdm_BC_GenAccVirBaseSpec(vdm_BC_GenIdentifier(
                                              PrefixAnonymType(ASTAUX::MkId(L"ref_").ImpConc(Name2Id(nm))))));
    }
  }

  TYPE_AS_Name nm (GiveFirstName(classnm));
  TYPE_CPP_Identifier cpptpid(vdm_BC_GenIdentifier( PrefixAnonymType(ASTAUX::MkId(L"ref_").ImpConc(Name2Id(nm)))));
  TYPE_CPP_Identifier cn (vdm_BC_GenIdentifier(ASTAUX::MkId(L"ObjectRef")));
  TYPE_CPP_ClassHead ch (vdm_BC_GenClassHead(cpptpid, bs));

  this->mod_decl.ImpAppend(cpptpid);
  type_dL pub_l;
  pub_l.ImpAppend(GenDefaultConstrDecl(cpptpid, cn));
  pub_l.ImpAppend(GenCommonConstrDecl(cpptpid, cn));
  pub_l.ImpAppend(GenVDMBasePConstrDecl(cpptpid, cn));
  pub_l.ImpAppend(GenGetTypeNameDecl(cpptpid));
  pub_l.ImpConc(GenMemFctMacros((Int) TAG_quote_OBJREF));

  type_dL arg_decl;
  arg_decl.ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenClassSpecifier(ch, (Generic)vdm_BC_GenPublic(pub_l))));

  TYPE_CPP_CPPAS res;
  res.ImpAppend(vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(), arg_decl, type_dL()));
  return res;
}
#endif // VDMPP

// GenQuoteDecl
// q : CPP`Identifier
// +> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenQuoteDecl(const TYPE_CPP_Identifier& q)
{
  TYPE_CPP_Identifier cn (vdm_BC_GenIdentifier(ASTAUX::MkId(L"Quote")));
  SEQ<TYPE_CPP_DeclSpecifier> arg1;
  arg1.ImpAppend(vdm_BC_GenStorageClassSpecifier(quote_EXTERN)).ImpAppend(
                 vdm_BC_GenTypeSpecifier(quote_CONST)).ImpAppend(
                 vdm_BC_GenTypeSpecifier(cn));
  SEQ<TYPE_CPP_InitDeclarator> arg2;
  arg2.ImpAppend(vdm_BC_GenInitDecl(q, Nil()));
  return TYPE_CPP_CPPAS().ImpAppend(vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(), arg1, arg2));
}

// GenQuoteDef
// q : CPP`Identifier
// +> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenQuoteDef(const TYPE_CPP_Identifier& q)
{
  TYPE_CPP_Identifier cn(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Quote")));
  type_dL arg1;
  arg1.ImpAppend(vdm_BC_GenTypeSpecifier(quote_CONST));
  arg1.ImpAppend(vdm_BC_GenTypeSpecifier(cn));

  TYPE_CPP_ObjectInit objinit (vdm_BC_GenObjectInit(mk_sequence(vdm_BC_GenStringLit(GetQuoteId(q)))));

  SEQ<TYPE_CPP_InitDeclarator> arg2;
  arg2.ImpAppend(vdm_BC_GenInitDecl(q, objinit));
  return TYPE_CPP_CPPAS().ImpAppend(vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(), arg1, arg2));
}

// GenCppTpDecl
// tp : REP`TypeRep
// names : set of AS`Name
// ==> Identifier
TYPE_AS_Id vdmcg::GenCppTpDecl(const TYPE_REP_TypeRep & tp, const SET<TYPE_AS_Name> & names)
{
  TYPE_AS_Id id;

  switch(tp.GetTag()) {
    case TAG_TYPE_REP_EmptySetTypeRep:
    case TAG_TYPE_REP_EmptySeqTypeRep:
    case TAG_TYPE_REP_EmptyMapTypeRep: {
      return ASTAUX::MkId(L"");
      break;
    }

    case TAG_TYPE_REP_NilTypeRep: {
      return ASTAUX::MkId(L"Generic");
      break;
    }
    case TAG_TYPE_REP_UnitTypeRep: {
      return ASTAUX::MkId(L"void");
      break;
    }
    case TAG_TYPE_REP_AllTypeRep: {
      return ASTAUX::MkId(L"Generic");
      break;
    }
    case TAG_TYPE_REP_SetTypeRep: {
      return GenSetType(tp, names);
      break;
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return GenSeqType(tp, names);
      break;
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return GenCppTpDecl(tp.GetRecord(pos_REP_InvTypeRep_shape), names);
      break;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      // TODO: remove inv
      TYPE_REP_TypeRep ctp (CleanFlatType(tp)); // Optimize Clean ???
      if (ctp.Is(TAG_TYPE_REP_UnionTypeRep))
        return GenUnionType(ctp, tp, names);
      else
        return GenCppTpDecl(ctp, names);
      break;
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      return GenProductType(tp, names);
      break;
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
      return GenCompositeType(tp, (TYPE_AS_Access)nil, names);
      break;
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return GenMapType(tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom),
                        tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng),
                        names);
      break;
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return GenMapType(tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom),
                        tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng),
                        names);
      break;
    }
    case TAG_TYPE_REP_TypeParRep: {
      const TYPE_AS_Name & nm (tp.GetRecord(pos_REP_TypeParRep_nm));
      TYPE_AS_Id nid (Name2Id(GenQName(nm)));
      return StringNumber(nid.Length()).ImpConc(nid).ImpConc(ASTAUX::MkId(L"N"));
      break;
    }
    case TAG_TYPE_REP_TypeNameRep: {
      Generic t (LOT(tp));
#ifdef VDMPP
      if (t.Is (TAG_TYPE_REP_ObjRefTypeRep))
        return GenCppTpDecl (t, names);
#endif // VDMPP
      const TYPE_AS_Name & nm (tp.GetRecord(pos_REP_TypeNameRep_nm));
      if (! t.IsNil() ) {
        if (IsTpRecursive(GenQName(nm), t, names))
          return ASTAUX::MkId(L"F");
        else
          return GenCppTpDecl(t, names);
      }
      else
        ReportError(L"GenCppDecl");
      return ASTAUX::MkId(L"");
      break;
    }
    case TAG_TYPE_REP_BooleanTypeRep: {
      return ASTAUX::MkId(L"b");
      break;
    }
    case TAG_TYPE_REP_NumericTypeRep: {
      switch (tp.GetIntValue(pos_REP_NumericTypeRep_qtp)) {
        case TAG_quote_NATONE:
        case TAG_quote_NAT :
        case TAG_quote_INTEGER :{return ASTAUX::MkId(L"i");}
        case TAG_quote_RAT:
        case TAG_quote_REAL:  {return ASTAUX::MkId(L"r"); break;}
        default: {ReportError(L"GenCppDecl"); break;}
      }
      return ASTAUX::MkId(L"");
      break;
    }
    case TAG_TYPE_REP_TokenTypeRep: {
      return ASTAUX::MkId(L"t");
      break;
    }
    case TAG_TYPE_REP_CharTypeRep: {
      return ASTAUX::MkId(L"c");
      break;
    }

    case TAG_TYPE_REP_QuoteTypeRep: {
      TYPE_AS_Name nm (tp.GetRecord(pos_REP_QuoteTypeRep_lit));
      TYPE_AS_Id q_name (nm.get_ids().Hd());

      if (!this->cppenv.DomExists(q_name) ) {
        this->quotes_l.ImpAppend(GenQuoteId(q_name));
        AddNewCppTp(q_name, Sequence());
      }
      return ASTAUX::MkId(L"Q");
      break;
    }

    case TAG_TYPE_REP_PartialFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & tpl (tp.GetSequence(pos_REP_PartialFnTypeRep_fndom));
      size_t len_tpl = tpl.Length();
      TYPE_AS_Id id;
      for (size_t idx = 1; idx <= len_tpl; idx++)
        GenCppTpDecl(tpl[idx], names);
      GenCppTpDecl(tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng), names);
//  -- whats going on ? id is now overwritten?
      return  ASTAUX::MkId(L"function");
      break;
    }
    case TAG_TYPE_REP_TotalFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & tpl (tp.GetSequence(pos_REP_TotalFnTypeRep_fndom));
      size_t len_tpl = tpl.Length();
      TYPE_AS_Id id;
      for (size_t idx = 1; idx <= len_tpl; idx++)
        GenCppTpDecl(tpl[idx], names);
      GenCppTpDecl(tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng), names);
//  -- whats going on ? id is now overwritten?
      return ASTAUX::MkId(L"function");
      break;
    }

#ifdef VDMPP
    case TAG_TYPE_REP_ObjRefTypeRep: {
      this->obj_refs.Insert(tp.GetRecord(pos_REP_ObjRefTypeRep_nm));
      return GenObjRefType(tp, names);
      break;
    }
#endif // VDMPP

    case TAG_TYPE_REP_OpTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & dtpl (tp.GetSequence(pos_REP_OpTypeRep_Dom));
      size_t len_dtpl = dtpl.Length();
      for (size_t idx = 1; idx <= len_dtpl; idx++)
        GenCppTpDecl(dtpl[idx], names);

      GenCppTpDecl(tp.GetRecord(pos_REP_OpTypeRep_Rng), names);
//  -- whats going on ? id is now overwritten?
      return ASTAUX::MkId(L"operation");
      break;
    }
    case TAG_TYPE_REP_TypeVarRep: {
      return ASTAUX::MkId(L"typevariable");
      break;
    }

    default : {
      ReportError(L"GenCppTpDecl");
      return ASTAUX::MkId(L"");
    }
  }
  //return id;
}

// GenCppTypeDef
// nm : AS`Name
// tp : AS`Type
// acc : [AS`Access]
#ifdef VDMSL
// ==> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenCppTypeDef(const TYPE_AS_Name & nm, const TYPE_AS_Type & tp, const Generic & acc)
#endif // VDMSL
#ifdef VDMPP
// ==> (seq of CPP`MemberDeclaration | CPP`CPPAS)
Sequence vdmcg::GenCppTypeDef(const TYPE_AS_Name & nm, const TYPE_AS_Type & tp, const Generic & acc)
#endif // VDMPP
{
  TYPE_REP_TypeRep rtp (RemoveInvType(FromAS2RepType(tp)));

  if (tp.Is(TAG_TYPE_AS_CompositeType)) {
    TYPE_AS_Id id (GenCompositeType(rtp, acc, SET<TYPE_AS_Name>().Insert(GenQName(nm))));
    return (Sequence) this->cppenv[id];
  }
  else {
#ifdef VDMPP
    if (vdm_CPP_isJAVA()) {
      switch(tp.GetTag()) {
        case TAG_TYPE_AS_UnionType: {
          if (IsCompositeType(rtp)) {
            return GenUnionInterfaceDecl(nm);
          }
          else {
            return Sequence();
          }
        }
        case TAG_TYPE_AS_TypeName: {
          return Sequence();
        }
        default: {
          return Sequence();
        }
      }
    }
    else
#endif // VDMPP
    {  // C++
      TYPE_CPP_Expr val;
      if(IsTpRecursive(nm, rtp, Set())) {
        val = vdm_BC_GenIdentifier(ASTAUX::MkId(L"Generic"));
      }
      else {
        val = GenVDMTpName( rtp, (Int) TAG_quote_ANONYM, Set().Insert(GenQName(nm)));
      }
 
      TYPE_CPP_PreDefine tpdef (vdm_BC_GenPreDefine(Name2CppTpId(nm), val));
      this->anonym_tpdefs.ImpPrepend(tpdef);
      return Sequence();
    }
  }
}

// GenVDMTpName
// tp : REP`TypeRep
// incl : (<ANONYM>|<CC>|<CCANONYM>|<H>)
// names : set of AS`Name
// ==> CPP`Name
TYPE_CPP_Name vdmcg::GenVDMTpName(const TYPE_REP_TypeRep & tp, const Int & incl,
                                  const SET<TYPE_AS_Name> & names)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    switch(tp.GetTag()) {
      case TAG_TYPE_REP_InvTypeRep: {
        return GenVDMTpName(tp.GetRecord(pos_REP_InvTypeRep_shape), incl, names);
        break;
      }
      case TAG_TYPE_REP_TypeNameRep: {
        TYPE_AS_Name nm (tp.GetRecord(pos_REP_TypeNameRep_nm));
        TYPE_AS_Ids ids (nm.get_ids());
        if ( ids.Length() == 2 ) {
          if (ids.Hd() == GiveCurCName())
            return vdm_BC_GenIdentifier(GiveLastName(nm)) ;
          else
            return (TYPE_CPP_Identifier) vdm_BC_Rename(nm) ;
        }
        else
          return vdm_BC_GenIdentifier(GiveLastName(nm)) ;
      }
      case TAG_TYPE_REP_ProductTypeRep: {
        return GenProductType().get_tp();
      }
      case TAG_TYPE_REP_SetTypeRep: {
        return GenSetType().get_tp();
      }
      case TAG_TYPE_REP_SeqTypeRep: {
        if (IsStringType(tp))
          return GenStringType().get_tp();
        else
          return GenSeq0Type().get_tp();
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        if (IsMapType(tp))
        {
          return GenMapType().get_tp();
        }
        else if (IsSetType(tp))
        {
          return GenSetType().get_tp();
        }
        else if (IsSeqType(tp))
        {
          if (IsPossibleStringType(tp))
            return GenStringType().get_tp();
          else
            return GenSeq0Type().get_tp();
        }
        else if (IsNumType(tp))
        {
          return GenNumType().get_tp();
        }
        else if (IsCompositeType(tp))
        {
          return GenRecordType(Nil()).get_tp();
        }
        return vdm_BC_GenIdentifier(ASTAUX::MkId(L"Object"));
      }
      case TAG_TYPE_REP_GeneralMapTypeRep: {
        return GenMapType().get_tp();
      }
      case TAG_TYPE_REP_InjectiveMapTypeRep: {
        return GenMapType().get_tp();
      }
      case TAG_TYPE_REP_QuoteTypeRep: {
        const TYPE_AS_Name & n (tp.GetRecord(pos_REP_QuoteTypeRep_lit));
        const TYPE_AS_Ids & ids (n.GetSequence(pos_AS_Name_ids));
        TYPE_AS_Id id (ids.Index(ids.Length()));
        return vdm_BC_GenIdentifier(ASTAUX::MkId(L"quotes.").ImpConc(id));
      }
      case TAG_TYPE_REP_CompositeTypeRep: {
        TYPE_AS_Name nm (tp.GetRecord(pos_REP_CompositeTypeRep_nm));
        TYPE_AS_Ids ids (nm.get_ids());
        if (ids.Length() > 1)
          if (ids.Hd() == GiveCurCName())
            return vdm_BC_GenIdentifier(GiveLastName(nm));
          else
            return (TYPE_CPP_Identifier)vdm_BC_Rename(nm);
        else
          return vdm_BC_GenIdentifier(GiveLastName(nm));
      }
      default: {
        return Id2JavaGTpId(GenCppTpDecl(tp, names));
      }
    }
  }
  else
#endif //VDMPP
  {
    switch(tp.GetTag()) {
      case TAG_TYPE_REP_InvTypeRep: {
        return GenVDMTpName(tp.GetRecord(pos_REP_InvTypeRep_shape), incl, names);
        break;
      }
      case TAG_TYPE_REP_TypeNameRep: {
        TYPE_AS_Name nm (tp.GetRecord(pos_REP_TypeNameRep_nm));
#ifdef VDMPP
        TYPE_AS_Ids l (nm.get_ids());
        if (l.Length() == 2) {
          switch (incl.GetValue()) {
            case TAG_quote_ANONYM : AddInclGH(GiveFirstName(nm)); break;
            case TAG_quote_CC : IncludeClass(GiveFirstName(nm)); break;
            case TAG_quote_CCANONYM : {
              AddInclGH(GiveFirstName(nm));
              IncludeClass(GiveFirstName(nm));
              break;
            }
            case TAG_quote_H : AddInclH(GiveFirstName(nm)); break;
          }
        }
#endif //VDMPP
        return Name2CppTpId(nm);
      }
      default:{
        return Id2CppGTpId(GenCppTpDecl(tp,names));
      }
    }
  }
}

// Rep2CppTypeSpecifier
// rtp : REP`TypeRep
// atp : [AS`Type]
// ==> CPP`TypeSpecifier
TYPE_CPP_TypeSpecifier vdmcg::Rep2CppTypeSpecifier(const TYPE_REP_TypeRep & rtp_, const Generic & atp)
{
  TYPE_REP_TypeRep rtp (RemoveNil(RemoveInvType(rtp_)));
  TYPE_CPP_Name cpp_name(GenVDMTpName(rtp, (Int) TAG_quote_H, Set()));

#ifdef VDMSL
  if (vdm_CPP_isCPP()) {
    if (atp.Is(TAG_TYPE_AS_TypeName)) {
      TYPE_AS_TypeName tp (atp);
      const TYPE_AS_Name & nm (tp.GetRecord(pos_AS_TypeName_name));
      return vdm_BC_GenTypeSpecifier(Name2CppTpId(nm));
    }
    else {
      return vdm_BC_GenTypeSpecifier(cpp_name);
    }
  }
  else {
    return vdm_BC_GenTypeSpecifier(cpp_name);
  }
#endif //VDMSL

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (atp.Is(TAG_TYPE_AS_TypeName) && rtp.Is(TAG_TYPE_REP_UnionTypeRep) && IsCompositeType(rtp)) {
      TYPE_AS_TypeName tp (atp);
      const TYPE_AS_Name & nm (tp.GetRecord(pos_AS_TypeName_name));
      const TYPE_AS_Ids & ids (nm.GetSequence(pos_AS_Name_ids));
      if (ids.Length() > 1) {
        if (ids.Hd() == GiveCurCName()) {
          return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(GiveLastName(nm)));
        }
        else {
          return vdm_BC_GenTypeSpecifier((TYPE_CPP_Identifier)vdm_BC_Rename(nm));
        }
      }
      else {
        return vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(GiveLastName(nm)));
      }
    }
    if (rtp.Is(TAG_TYPE_REP_UnionTypeRep) && IsProductType(rtp)) {
      return GenProductType();
    }
    else {
      return vdm_BC_GenTypeSpecifier(cpp_name);
    }
  }
  else { // C++
    if (atp.Is(TAG_TYPE_AS_TypeName) && !rtp.Is(TAG_TYPE_REP_ObjRefTypeRep) ) {
      TYPE_AS_TypeName tp (atp);
      const TYPE_AS_Name & nm (tp.GetRecord(pos_AS_TypeName_name));
      TYPE_AS_Name qnm (GenQualifiedTypeName (GenQName(nm)));
      TYPE_AS_Name firstname (GiveFirstName(qnm));
      if (firstname != this->modnm) {
        AddInclH(GiveFirstName(qnm));
      }
      return vdm_BC_GenTypeSpecifier(Name2CppTpId(qnm));
    }
    else {
      return vdm_BC_GenTypeSpecifier(cpp_name);
    }
  }
  return vdm_BC_GenTypeSpecifier(cpp_name);
#endif //VDMPP
}

#ifdef VDMSL
// GetImpModuleNames
// md : AS`Module
// +> set of AS`Name
SET<TYPE_AS_Name> vdmcg::GetImpModuleNames(const TYPE_AS_Module & md)
{
  return md.get_intf().get_imp().Dom();
}
#endif //VDMSL

#ifdef VDMPP
// GetImpModuleNames
// md : AS`Class
// ==> set of AS`Name
SET<TYPE_AS_Name> vdmcg::GetImpModuleNames(const TYPE_AS_Class & md)
{
  SET<TYPE_AS_Name> res;
  res.ImpUnion(this->inclgh);
  res.ImpUnion(this->obj_refs);
  return res;
}
#endif //VDMPP

// GenModuleCCIncludes
// ==> CPP`Preprocessors
SEQ<TYPE_CPP_Preprocessor> vdmcg::GenModuleCCIncludes()
{
  SEQ<TYPE_CPP_Preprocessor> code;

#ifdef VDMPP
   Set i_set (this->obj_refs);
   i_set.ImpDiff(mk_set(this->modnm));

   Generic g;
   for (bool bb = i_set.First(g); bb; bb = i_set.Next(g)) {
     code.ImpAppend(vdm_BC_GenInclusion(GiveLastName(g).ImpConc(ASTAUX::MkId(L".h"))));
   }
#endif //VDMPP
  return code;
}

// GenModuleHIncludes
// ==> CPP`Preprocessors
SEQ<TYPE_CPP_Preprocessor> vdmcg::GenModuleHIncludes()
{
  SEQ<TYPE_CPP_Preprocessor> code;
  code.ImpAppend(vdm_BC_GenInclusion(GiveLastName(this->modnm).ImpConc(Anonym_Suffix).ImpConc(ASTAUX::MkId(L".h"))));
  return code;
}

// GenHIncludes
// IsIncluded : set of AS`Name
// ==> CPP`Preprocessors
SEQ<TYPE_CPP_Preprocessor> vdmcg::GenHIncludes(const SET<TYPE_AS_Name> & IsIncluded)
{
  SEQ<TYPE_CPP_Preprocessor> code;
  SET<TYPE_AS_Name> i_set(this->inclh);
  i_set.ImpDiff(IsIncluded);

  Generic g;
  for (bool bb = i_set.First(g); bb; bb = i_set.Next(g)) {
    code.ImpAppend(vdm_BC_GenInclusion(GiveLastName(g).ImpConc(ASTAUX::MkId(L".h"))));
  }

// 20121113 -->
/*
#ifdef VDMPP
  SET<TYPE_AS_Name> orefs (this->obj_refs);
  Generic nm;
  for (bool bb = orefs.First(nm); bb; bb = orefs.Next(nm))
  {
    SET<TYPE_AS_Name> supers1 (GetOrderedSupers(nm).Elems());
    SET<TYPE_AS_Name> supers2 (GetOrderedSupers(this->modnm).Elems().Insert(this->modnm));
    if (supers1.Intersect(supers2).IsEmpty())
      code.ImpAppend(vdm_BC_GenInclusion(GiveLastName(nm).ImpConc(ASTAUX::MkId(L".h"))));
  }
#endif // VDMPP
*/
// <-- 20121113

  return code;
}

// GenModuleCCPart
// ==> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenModuleCCPart()
{
  return this->mod_fctdef;
}

// GenModuleHPart
// ==> CPP`CPPAS
TYPE_CPP_CPPAS vdmcg::GenModuleHPart()
{
  TYPE_CPP_CPPAS code;
// 20120912 -->
/*
// <-- 20120912
#ifdef VDMSL
  SET<TYPE_AS_Name> itypes (this->imported_types);
  Generic nm;
  for (bool bb = itypes.First(nm); bb; bb = itypes.Next(nm) )
  {
    TYPE_REP_TypeNameRep rep_nm;
    rep_nm.set_nm(nm);
    TYPE_AS_Id key (GenCppTpDecl(LOT(rep_nm), Set()));
    code.ImpAppend(this->cppenv[key]);
  }
#endif //VDMSL
// 20120912 -->
*/
// <-- 20120912

  return code;
}

#ifdef VDMSL
// GenAnonymFiles
// md : AS`Module
// ==> CPP`Files
TYPE_CPP_Files vdmcg::GenAnonymFiles(const TYPE_AS_Module& md)
#endif //VDMSL
#ifdef VDMPP
// GenAnonymFiles
// md : AS`Class
// ==> CPP`Files
TYPE_CPP_Files vdmcg::GenAnonymFiles(const TYPE_AS_Class& md)
#endif //VDMPP
{
  SET<TYPE_CPP_File> files;
  TYPE_CPP_CPPAS code;
  SET<TYPE_AS_Name> impl (GetImpModuleNames(md));

  //----------------------------
  //-- Module anonym include  file
  //----------------------------

  //-- includes
  TYPE_AS_Id userdefIncl (GiveLastName(md.get_nm()).ImpConc(Userdef_Suffix).ImpConc(ASTAUX::MkId(L".h")));
  code.ImpAppend(vdm_BC_GenInclusion(userdefIncl));
  code.ImpAppend(vdm_BC_GenInclusion(ASTAUX::MkId(L"metaiv.h")));

  //-- declare quotes
  size_t len_quotes_l = this->quotes_l.Length();
  for (size_t idx = 1; idx <= len_quotes_l; idx++) {
    code.ImpConc(GenQuoteDecl(this->quotes_l[idx]));
  }

  //-- forward declartion of generated vdm based types
  SEQ<TYPE_CPP_Identifier> decl;
  decl.ImpConc(this->mod_decl)
      .ImpConc(this->anonym_decl)
      .ImpConc(this->anonym_comp_decl);

  Generic i;
  for (bool cc = decl.First(i); cc; cc = decl.Next(i)) {
    code.ImpConc(GenForwardDecl(i));
  }

  //-- declaration of defined module types using #define - can`t be forward declared
  if (vdm_CPP_isCPP())
    code.ImpConc(this->anonym_tpdefs);

  //-- anonym type tag if defs for anonym and module/class types
// 20150508 -->
  //SEQ<TYPE_CPP_Identifier> glttags (this->anonym_decl);
  //glttags.ImpConc(this->quotes_l);
  SEQ<TYPE_CPP_Identifier> glttags (this->quotes_l);
// <-- 20150508

  for (bool dd = glttags.First(i); dd; dd = glttags.Next(i)) {
    TYPE_CPP_Identifier deftag (GenTypeTagCppId(i));
    TYPE_CPP_Expr expr (vdm_BC_GenBracketedExpr(vdm_BC_GenPlus(GenAnonymTypeTag(), new_tag())));
    code.ImpConc(GenIfNotDef(deftag, expr));
  }

  //-- anonym includes
  Set i_set (impl);
#ifdef VDMPP
//  i_set.ImpUnion(this->obj_refs);
//// 20121113 -->
  i_set.ImpDiff(mk_set(this->modnm));
//// <-- 20121113
#endif //VDMPP
  Generic nm;
  for (bool ee = i_set.First(nm); ee; ee = i_set.Next(nm) )
  {
    TYPE_AS_Id inclSeq (GiveLastName(nm).ImpConc(Anonym_Suffix).ImpConc(ASTAUX::MkId(L".h")));
    code.ImpAppend(vdm_BC_GenInclusion(inclSeq));
  }

  //  code.ImpConc(GenIfNotDef(vdm_BC_GenIdentifier(ASTAUX::MkId(L"EXT_MEMFCT")),
  //                           vdm_BC_GenIdentifier(ASTAUX::MkId(L" "))));

  size_t len_mod_decl = this->mod_decl.Length();
  for (size_t idx = 1; idx <= len_mod_decl; idx++)
  {
    TYPE_CPP_Identifier deftag (GenTypeTagCppId(this->mod_decl[idx]));
    TYPE_CPP_Expr expr (vdm_BC_GenBracketedExpr(vdm_BC_GenPlus(GenModuleTypeTag(), new_tag())));
    code.ImpConc(GenIfNotDef(deftag, expr));
  }

  //-- anonym declaration
  if (vdm_CPP_isCPP())
    code.ImpConc(this->anonym_tpdecl);

  TYPE_AS_Id mdSeq (GiveLastName(md.get_nm()));
  mdSeq.ImpConc(Anonym_Suffix);
  mdSeq.ImpConc(ASTAUX::MkId(L".h"));
  files.Insert(vdm_BC_GenFile(nil, mdSeq, Sequence(), code));

  //Module Anonym code file

  code = TYPE_CPP_CPPAS();

  mdSeq = GiveLastName(md.get_nm());
  mdSeq.ImpConc(Userdef_Suffix);
  mdSeq.ImpConc(ASTAUX::MkId(L".h"));
  code.ImpAppend(vdm_BC_GenInclusion(mdSeq));
  code.ImpAppend(vdm_BC_GenInclusion(ASTAUX::MkId(L"metaiv.h")));

#ifdef _MSC_VER
  TYPE_AS_Id clnm (GiveLastName(md.get_nm()));
  TYPE_AS_Id id ((clnm == ASTAUX::MkId(L"MATH")) ? ASTAUX::MkId(L"vdm_MATH.h")
                                                 : clnm.ImpConc(ASTAUX::MkId(L".h")));
  code.ImpAppend(vdm_BC_GenInclusion(id));
#else
  code.ImpAppend(vdm_BC_GenInclusion(GiveLastName(md.get_nm()).ImpConc(ASTAUX::MkId(L".h"))));
#endif // _MSC_VER

  for (bool gg = impl.First(nm); gg; gg = impl.Next(nm)) {
    code.ImpAppend(vdm_BC_GenInclusion(GiveLastName(nm).ImpConc(ASTAUX::MkId(L".h"))));
  }

#ifdef VDMPP
  //-- object referencer
  i_set = this->obj_refs;
//  if (i_set.InSet(this->modnm))
//    i_set.RemElem(this->modnm);
  i_set.ImpDiff(mk_set(this->modnm));

  Generic n;
  for (bool hh = i_set.First(n); hh; hh = i_set.Next(n)) {
    code.ImpAppend(vdm_BC_GenInclusion(GiveLastName(n).ImpConc(ASTAUX::MkId(L".h"))));
  }
#endif //VDMPP

  //-- generate quote declarations
//  Generic q;
//  for (bool ii = this->quotes_l.First(q); ii; ii = this->quotes_l.Next(q)) {
//    code.ImpConc(GenTypeDefIfDef(q, GenQuoteDef(q)));
//  }
  for (size_t idx2 = 1; idx2 <= len_quotes_l; idx2++)
  {
    code.ImpConc(GenTypeDefIfDef(this->quotes_l[idx2], GenQuoteDef(this->quotes_l[idx2])));
  }

  //-- functions
  code.ImpConc(this->anonym_fctdef);

  TYPE_AS_Id lastname (GiveLastName(md.get_nm()));
  TYPE_AS_Id filename (GenFileExt(lastname.ImpConc(Anonym_Suffix)));
  files.Insert(vdm_BC_GenFile(nil, filename, Sequence(), code));

  return files;
}

// GenDlMember
// ==> CPP`Identifier
TYPE_CPP_Identifier vdmcg::GenDlMember()
{
  return vdm_BC_GenIdentifier(GiveLastName(this->modnm).ImpConc(DlClassPtr_Suffix));
}

// GetTypeDef
// nm : AS`Name
// ==> [AS`TypeDef]
Generic vdmcg::GetTypeDef(const TYPE_AS_Name & nm)
{
  TYPE_AS_Name clmodnm;
  TYPE_AS_Name tpnm;
  if (2 == nm.GetSequence(pos_AS_Name_ids).Length()) {
    clmodnm = GiveFirstName(nm);
    tpnm = GiveSecondName(nm);
  } else {
    clmodnm = GiveCurCASName();
    tpnm = nm;
  }
#ifdef VDMPP
  Generic clsnm (GetStatSem().LookUpDefClassForTypeName(ASTAUX::Combine2Names(clmodnm, nm)));
  if (clsnm.IsNil()) {
    return Nil();
  }
  clmodnm = clsnm;
#endif // VDMPP
  if (this->typeDefs.DomExists(clmodnm)) {
    const Map & typem (this->typeDefs[clmodnm]);
    if (typem.DomExists(tpnm)) {
      TYPE_AS_TypeDef tdef(typem[tpnm]);
/*
      if (tdef.GetRecord(pos_AS_TypeDef_shape).Is(TAG_TYPE_AS_CompositeType)) {
        TYPE_AS_CompositeType ctp(tdef.GetRecord(pos_AS_TypeDef_shape));
        TYPE_AS_Name nm (ctp.GetRecord(pos_AS_CompositeType_name));
        ctp.SetField(pos_AS_CompositeType_name, ASTAUX::Combine2Names(clmodnm, nm));
        
        tdef.SetField(pos_AS_TypeDef_shape, ctp);
      }
*/
      const TYPE_AS_Type & shape (tdef.GetRecord(pos_AS_TypeDef_shape));
#ifdef VDMSL
      tdef.SetField(pos_AS_TypeDef_shape, AddClMod(shape, clmodnm, Set()));
#endif // VDMSL
#ifdef VDMPP
      tdef.SetField(pos_AS_TypeDef_shape, AddClMod(shape, clmodnm, GetAllClasses()));
#endif // VDMPP
      return tdef;
    }
  }
  return Nil();
}

