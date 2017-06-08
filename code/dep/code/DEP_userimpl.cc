/***
*  * WHAT
*  *    Implementation of implicit function LookUpName in module DEP
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/dep/code/DEP_userimpl.cc,v $
*  * VERSION
*  *    $Revision: 1.8 $
*  * DATE
*  *    $Date: 2006/06/06 04:10:28 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    IDERS-
*  * PROJECT
*  *    IDERS Project EP8593
*  * AUTHOR
*  *    $Author: vdmtools $
*  * LOG
*  *    $Log: DEP_userimpl.cc,v $
*  *    Revision 1.8  2006/06/06 04:10:28  vdmtools
*  *    api
*  *
*  *    Revision 1.7  2001/06/12 15:04:04  paulm
*  *    Updates for wchar_t/wstring/string literals (i18n)
*  *
*  *    Revision 1.6  2001/01/22 12:49:20  paulm
*  *    Updates for use of seq instead of tok in AS_Name
*  *
*  *    Revision 1.5  1999/01/25 13:00:45  jeppe
*  *    UML Mapper updated wrt. changes in dependecy calculation.
*  *
*  *    Revision 1.4  1998/12/04 10:11:52  jeppe
*  *    Optimize depedency: Added vdm_DEP_GetAST
*  *
*  *    Revision 1.3  1998/10/05 16:18:56  erik
*  *    Added missing return is case branch.
*  *
*  *    Revision 1.2  1998/09/30 12:59:50  hanne
*  *    UnfoldType and Rep2As is now implemented by hand. ExpandClass has been introduced in the specification as an implicit operation. It is also implemented by hand
*  *
*  *    Revision 1.1  1998/09/18 15:31:37  jeppe
*  *    Update wrt. cg naming revision: 'external' changed to 'userdef' or 'userimpl',
*  *    'global' changed to 'anonym'.
*  *
*  *    Revision 1.2  1998/03/30 13:53:18  erik
*  *    Join of ast_change_971112_premerge_980328 with branch ast_change_971112_bt.
*  *    Code generated files has been regenerated.
*  *
*  *    Revision 1.1.2.2  1998/02/17 13:48:42  hanne
*  *    MkName changed to use cid
*  *
*  *    Revision 1.1.2.1  1998/01/27 08:07:23  hanne
*  *    Hand coded functions for DEP module is now defined in external_DEP.cc external_DEP.h, because the new cg is used
*  *
*  *    Revision 1.2.2.1  1997/11/14 09:40:39  lto
*  *    Use h2gAS to transform to generated AS format
*  *
*  *    Revision 1.2  1997/11/07 14:02:52  storm
*  *    Modified to reflect changes in dep.vdm. That is, LookUpName has been replaced
*  *    with RepTypeFromName.
*  *
*  *    Revision 1.1  1997/06/05 13:18:32  jeppe
*  *    Code udated acc. to specification
*  *
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "tools.h"
#include "statsem.h"
#include "projectval.h"
#include "tbutils.h"
#include "intconvquotes.h"
#include "Repository.h"

// GetAST
// nm : AS`Name
// ==> AS`Class
TYPE_AS_Class vdm_DEP_GetAST( const TYPE_AS_Name& nm) {
  // Extract AST from Specification Manager.
  TYPE_ProjectTypes_ModuleName asnm (PTAUX::ASName2ModuleName (nm));
  TYPE_ProjectTypes_AstVal ast (ToolMediator::Repos()->vdm_GetAST (asnm));
  return INT2Q::h2gAS (PTAUX::ExtractAstOrFlat(ast));
}

// RepTypeFromName
// nm : AS`Name
// clnm : AS`Name
// ==> [REP`TypeRep]
Generic vdm_DEP_RepTypeFromName(const TYPE_AS_Name& nm, const TYPE_AS_Name& clnm)
{
  return INT2Q::h2gAS(TOOLS::DependencyFindType(nm, clnm));
}

// ExpandClass
// c : AS`Class
void vdm_DEP_ExpandClass(const TYPE_AS_Class &c)
{
  TOOLS::ExpandClassTC(INT2Q::g2hAS(c));
}

#ifdef DEF_DEP_UnfoldType
// UnfoldType
// tp : AS`Type
// clnm : AS`Name
// tpnm_s : set of AS`Name
// vc_s : set of AS`Name
// level : int
// +> AS`Type
TYPE_AS_Type vdm_DEP_UnfoldType(const TYPE_AS_Type &vdm_DEP_tp,
                                const TYPE_AS_Name &vdm_DEP_clnm,
                                const type_7AS_NameCS &vdm_DEP_tpnm_us,
                                const type_7AS_NameCS &vdm_DEP_vc_us)
{
  switch (vdm_DEP_tp.GetTag()) {
    case TAG_TYPE_AS_BooleanType:
    case TAG_TYPE_AS_NumericType:
    case TAG_TYPE_AS_TokenType:
    case TAG_TYPE_AS_CharType:
    case TAG_TYPE_AS_QuoteType:
    case TAG_TYPE_AS_PartialFnType:
    case TAG_TYPE_AS_TotalFnType:
    case TAG_TYPE_AS_TypeVar: {
      return vdm_DEP_tp;
    }
    case TAG_TYPE_AS_CompositeType: {
      TYPE_AS_CompositeType res_tp (vdm_DEP_tp);
      SEQ<TYPE_AS_Field> fields (res_tp.get_fields());
      SEQ<TYPE_AS_Field> r_fields;
      Generic g;
      for (bool bb = fields.First(g); bb; bb = fields.Next(g)) {
        TYPE_AS_Field field(g);
        TYPE_AS_Type fltp (field.get_type());
        field.set_type(vdm_DEP_UnfoldType(fltp, vdm_DEP_clnm, vdm_DEP_tpnm_us, vdm_DEP_vc_us));
        r_fields.ImpAppend(field);
      }
      res_tp.set_fields(r_fields);
      return res_tp;
    }
    case TAG_TYPE_AS_UnionType: {
      TYPE_AS_UnionType res_tp(vdm_DEP_tp);
      SEQ<TYPE_AS_Type> types(res_tp.GetSequence(pos_AS_UnionType_tps));
      SEQ<TYPE_AS_Type> r_types;
      Generic g;
      for (bool bb = types.First(g); bb; bb = types.Next(g)) {
        r_types.ImpAppend(vdm_DEP_UnfoldType(g, vdm_DEP_clnm, vdm_DEP_tpnm_us, vdm_DEP_vc_us));
      }
      res_tp.set_1(r_types);
      return res_tp;
    }
    case TAG_TYPE_AS_ProductType: {
      TYPE_AS_ProductType res_tp (vdm_DEP_tp);
      SEQ<TYPE_AS_Type> types(res_tp.GetSequence(pos_AS_ProductType_tps));
      SEQ<TYPE_AS_Type> r_types;
      Generic g;
      for (bool bb = types.First(g); bb; bb = types.Next(g)) {
        r_types.ImpAppend(vdm_DEP_UnfoldType(g, vdm_DEP_clnm, vdm_DEP_tpnm_us, vdm_DEP_vc_us));
      }
      res_tp.set_1(r_types);
      return res_tp;
    }
    case TAG_TYPE_AS_OptionalType: {
      TYPE_AS_OptionalType res_tp(vdm_DEP_tp);
      res_tp.set_tp(vdm_DEP_UnfoldType(res_tp.get_tp(), vdm_DEP_clnm, vdm_DEP_tpnm_us, vdm_DEP_vc_us));
      return res_tp;
    }
    case TAG_TYPE_AS_Set0Type: {
      TYPE_AS_Set0Type res_tp(vdm_DEP_tp);
      res_tp.set_elemtp(vdm_DEP_UnfoldType(res_tp.get_elemtp(), vdm_DEP_clnm, vdm_DEP_tpnm_us, vdm_DEP_vc_us));
      return res_tp;
    }
    case TAG_TYPE_AS_Set1Type: {
      TYPE_AS_Set1Type res_tp(vdm_DEP_tp);
      res_tp.set_elemtp(vdm_DEP_UnfoldType(res_tp.get_elemtp(), vdm_DEP_clnm, vdm_DEP_tpnm_us, vdm_DEP_vc_us));
      return res_tp;
    }
    case TAG_TYPE_AS_Seq0Type: {
      TYPE_AS_Seq0Type res_tp(vdm_DEP_tp);
      res_tp.set_elemtp(vdm_DEP_UnfoldType(res_tp.get_elemtp(), vdm_DEP_clnm, vdm_DEP_tpnm_us, vdm_DEP_vc_us));
      return res_tp;
    }
    case TAG_TYPE_AS_Seq1Type: {
      TYPE_AS_Seq1Type res_tp(vdm_DEP_tp);
      res_tp.set_elemtp(vdm_DEP_UnfoldType(res_tp.get_elemtp(), vdm_DEP_clnm, vdm_DEP_tpnm_us, vdm_DEP_vc_us));
      return res_tp;
    }
    case TAG_TYPE_AS_GeneralMap0Type: {
      TYPE_AS_GeneralMap0Type res_tp(vdm_DEP_tp);
      res_tp.set_mapdom(vdm_DEP_UnfoldType(res_tp.get_mapdom(), vdm_DEP_clnm, vdm_DEP_tpnm_us, vdm_DEP_vc_us));
      res_tp.set_maprng(vdm_DEP_UnfoldType(res_tp.get_maprng(), vdm_DEP_clnm, vdm_DEP_tpnm_us, vdm_DEP_vc_us));
      return res_tp;
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      TYPE_AS_GeneralMap1Type res_tp(vdm_DEP_tp);
      res_tp.set_mapdom(vdm_DEP_UnfoldType(res_tp.get_mapdom(), vdm_DEP_clnm, vdm_DEP_tpnm_us, vdm_DEP_vc_us));
      res_tp.set_maprng(vdm_DEP_UnfoldType(res_tp.get_maprng(), vdm_DEP_clnm, vdm_DEP_tpnm_us, vdm_DEP_vc_us));
      return res_tp;
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      TYPE_AS_InjectiveMap0Type res_tp(vdm_DEP_tp);
      res_tp.set_mapdom(vdm_DEP_UnfoldType(res_tp.get_mapdom(), vdm_DEP_clnm, vdm_DEP_tpnm_us, vdm_DEP_vc_us));
      res_tp.set_maprng(vdm_DEP_UnfoldType(res_tp.get_maprng(), vdm_DEP_clnm, vdm_DEP_tpnm_us, vdm_DEP_vc_us));
      return res_tp;
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      TYPE_AS_InjectiveMap1Type res_tp(vdm_DEP_tp);
      res_tp.set_mapdom(vdm_DEP_UnfoldType(res_tp.get_mapdom(), vdm_DEP_clnm, vdm_DEP_tpnm_us, vdm_DEP_vc_us));
      res_tp.set_maprng(vdm_DEP_UnfoldType(res_tp.get_maprng(), vdm_DEP_clnm, vdm_DEP_tpnm_us, vdm_DEP_vc_us));
      return res_tp;
    }
    case TAG_TYPE_AS_BracketedType: {
      TYPE_AS_BracketedType res_tp(vdm_DEP_tp);
      res_tp.set_tp(vdm_DEP_UnfoldType(res_tp.get_tp(), vdm_DEP_clnm, vdm_DEP_tpnm_us, vdm_DEP_vc_us));
      return res_tp;
    }
    case TAG_TYPE_AS_TypeName: {
      TYPE_AS_TypeName tpnm (vdm_DEP_tp);
      TYPE_AS_Name nm(tpnm.get_name());
      if (vdm_DEP_tpnm_us.InSet(nm) ||
          vdm_DEP_vc_us.InSet(nm) ) {
        return tpnm;
      }
      else {
        Set new_tpnm_s(vdm_DEP_tpnm_us);
        new_tpnm_s.Insert(nm);
        return vdm_DEP_UnfoldType(vdm_DEP_Rep2As(vdm_DEP_LookUpType(nm, vdm_DEP_clnm)),
                                  vdm_DEP_clnm, (Generic) new_tpnm_s, vdm_DEP_vc_us);
      }
    }
    default: {
      return TYPE_AS_Type();
    }
  }
}

#endif

#ifdef DEF_DEP_Rep2As
// Rep2As
// rep : REP`TypeRep
// +> AS`Type
TYPE_AS_Type vdm_DEP_Rep2As(const TYPE_REP_TypeRep &vdm_DEP_rep)
{
  int tag (vdm_DEP_rep.GetTag());
  switch (tag) {
    case TAG_TYPE_REP_BooleanTypeRep: {
      return TYPE_AS_BooleanType().Init(NilContextId);
    }
    case TAG_TYPE_REP_NumericTypeRep: {
      return TYPE_AS_NumericType().Init(vdm_DEP_rep.GetField(pos_REP_NumericTypeRep_qtp),NilContextId);
    }
    case TAG_TYPE_REP_TokenTypeRep: {
      return TYPE_AS_TokenType().Init(NilContextId);
    }
    case TAG_TYPE_REP_CharTypeRep: {
      return TYPE_AS_CharType().Init(NilContextId);
    }
    case TAG_TYPE_REP_QuoteTypeRep : {
      TYPE_REP_QuoteTypeRep rep(vdm_DEP_rep);
      TYPE_AS_Name lit(rep.get_lit());
      TYPE_AS_Ids ids (lit.get_ids());

      TYPE_AS_QuoteLit qlit;
      qlit.set_val(ids.Hd());
      qlit.set_cid(NilContextId);

      TYPE_AS_QuoteType res;
      res.set_lit(qlit);
      res.set_cid(NilContextId);
      return res;
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
      TYPE_REP_CompositeTypeRep rep(vdm_DEP_rep);
      SEQ<TYPE_REP_FieldRep> fl_l (rep.get_fields());
      SEQ<TYPE_AS_Field> res_fl_l;
      Generic g;
      for (bool bb = fl_l.First(g); bb; bb = fl_l.Next(g)) {
        TYPE_REP_FieldRep frep(g);
        TYPE_AS_Field fas;
        fas.set_sel(frep.get_sel());
        fas.set_type(vdm_DEP_Rep2As(frep.get_tp()));
        fas.set_dc(frep.get_dc());
        res_fl_l.ImpAppend(fas);
      }

      TYPE_AS_CompositeType res;
      res.set_name(rep.get_nm());
      res.set_fields(res_fl_l);
      res.set_cid(NilContextId);
      return res;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      TYPE_REP_UnionTypeRep rep(vdm_DEP_rep);
      SET<TYPE_REP_TypeRep> tps (rep.get_tps());
      type_FLbb3P rur (vdm_DEP_ResolveUnionRep(tps, (Bool)false, (Bool)false));
      type_FL astp (rur.get_1());
      Bool isNil (rur.get_2());
      Bool isUnion (rur.get_3());
      if (isNil.GetValue() && isUnion.GetValue() && astp.Length() > 1) {
        TYPE_AS_OptionalType res;
        TYPE_AS_UnionType ut;
        ut.set_tps(astp);
        ut.set_cid(NilContextId);
        res.set_tp(ut);
        res.set_cid(NilContextId);
        return res;
      }
      else if (isNil.GetValue()) {
        TYPE_AS_OptionalType res;
        res.set_tp(astp.Hd());
        res.set_cid(NilContextId);
        return res;
      }
      else if (isUnion.GetValue()) {
        TYPE_AS_UnionType res;
        res.set_tps(astp);
        res.set_cid(NilContextId);
        return res;
      }
      else
        return astp.Hd();
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      TYPE_REP_ProductTypeRep rep (vdm_DEP_rep);
      SEQ<TYPE_REP_TypeRep> tp_l (rep.GetSequence(pos_REP_ProductTypeRep_tps));
      SEQ<TYPE_AS_Type> res_tp_l;
      Generic g;
      for (bool bb = tp_l.First(g); bb; bb = tp_l.Next(g)) {
        res_tp_l.ImpAppend(vdm_DEP_Rep2As(g));
      }
      TYPE_AS_ProductType res;
      res.set_1(res_tp_l);
      res.set_cid(NilContextId);
      return res;
    }
    case TAG_TYPE_REP_SetTypeRep: {
      TYPE_REP_SetTypeRep rep(vdm_DEP_rep);
      TYPE_AS_Set1Type res;
      res.set_elemtp(vdm_DEP_Rep2As(rep.get_elemtp()));
      res.set_cid(NilContextId);
      return res;
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      TYPE_REP_SeqTypeRep rep(vdm_DEP_rep);
      TYPE_AS_Seq1Type res;
      res.set_elemtp(vdm_DEP_Rep2As(rep.get_elemtp()));
      res.set_cid(NilContextId);
      return res;
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      TYPE_REP_GeneralMapTypeRep rep (vdm_DEP_rep);
      TYPE_AS_GeneralMap1Type res;
      res.set_mapdom(vdm_DEP_Rep2As(rep.get_mapdom()));
      res.set_maprng(vdm_DEP_Rep2As(rep.get_maprng()));
      res.set_cid(NilContextId);
      return res;
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      TYPE_REP_InjectiveMapTypeRep rep (vdm_DEP_rep);
      TYPE_AS_InjectiveMap1Type res;
      res.set_mapdom(vdm_DEP_Rep2As(rep.get_mapdom()));
      res.set_maprng(vdm_DEP_Rep2As(rep.get_maprng()));
      res.set_cid(NilContextId);
      return res;
    }
    case TAG_TYPE_REP_TypeNameRep: {
      TYPE_REP_TypeNameRep rep(vdm_DEP_rep);
      TYPE_AS_TypeName res;
      res.set_name(rep.get_nm());
      res.set_cid(NilContextId);
      return res;
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      TYPE_REP_PartialFnTypeRep rep(vdm_DEP_rep);
      TYPE_AS_PartialFnType res;
      Generic tp_l (rep.get_fndom());
      SEQ<TYPE_AS_Type> res_tp_l;
      if (tp_l.IsSequence()) {
        SEQ<TYPE_REP_TypeRep> fndom(tp_l);
        Generic g;
        for (bool bb = fndom.First(g); bb; bb = fndom.Next(g)) {
          res_tp_l.ImpAppend(vdm_DEP_Rep2As(g));
        }
      }
      res.set_fndom(res_tp_l);
      res.set_fnrng(vdm_DEP_Rep2As(rep.get_fnrng()));
      res.set_cid(NilContextId);
      return res;
    }
    case TAG_TYPE_REP_TotalFnTypeRep: {
      TYPE_REP_TotalFnTypeRep rep(vdm_DEP_rep);
      TYPE_AS_TotalFnType res;
      Generic tp_l (rep.get_fndom());
      SEQ<TYPE_AS_Type> res_tp_l;
      if (tp_l.IsSequence()) {
        SEQ<TYPE_REP_TypeRep> fndom(tp_l);
        Generic g;
        for (bool bb = fndom.First(g); bb; bb = fndom.Next(g)) {
          res_tp_l.ImpAppend(vdm_DEP_Rep2As(g));
        }
      }
      res.set_fndom(res_tp_l);
      res.set_fnrng(vdm_DEP_Rep2As(rep.get_fnrng()));
      res.set_cid(NilContextId);
      return res;
    }
    case TAG_TYPE_REP_ObjRefTypeRep: {
      TYPE_REP_ObjRefTypeRep rep (vdm_DEP_rep);
      TYPE_AS_TypeName tpnm;
      tpnm.set_name(rep.get_nm());
      tpnm.set_cid(NilContextId);
      return tpnm;
    }
    case TAG_TYPE_REP_TypeVarRep: {
      TYPE_REP_TypeVarRep rep (vdm_DEP_rep);
      TYPE_AS_TypeVar res;
      res.set_1(rep.get_nm());
      res.set_cid(NilContextId);
      return res;
    }
    case TAG_TYPE_REP_InvTypeRep: {
      TYPE_REP_InvTypeRep rep (vdm_DEP_rep);
      return (vdm_DEP_Rep2As(rep.get_shape()));
    }
  }
  return TYPE_AS_Type(); // To keep VC++ silent.
}
#endif
