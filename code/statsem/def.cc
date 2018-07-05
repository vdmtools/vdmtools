/***
*  * ID
*  *    $Id: def.cc,v 1.101 2006/04/03 23:50:30 vdmtools Exp $
*  * SPECIFICATION VERSION
*  *    def.cc: Implementation of def.vdm 1.99
***/

#include "statsem.h"
#include "contextinfo.h"
#include "astaux.h"
#include "rec.h"
#include "settings.h"
#include "intconvquotes.h"

//wcout << L"intertp: " << INT2Q::h2gAS(intertp) << endl;

#ifndef NOSS

#ifdef VDMPP
// wf_Class
// i : TYPE`Ind
// as_class : AS`Class
// ==> bool
bool StatSem::wf_Class(const Int & i, const TYPE_AS_Class & as_class)
{
  bool reswf = ExpandClass(i, as_class);

  const SEQ<TYPE_AS_Name> & super (as_class.GetSequence(pos_AS_Class_supercls));
  size_t len_super = super.Length();
  for (size_t idx = 1; idx <= len_super; idx++) {
    const TYPE_AS_Name & cls (super[idx]);
    if (!CheckClassName(cls)) {
      //----------------------------------------
      // Error message #1
      // The class L"%1" has not been pre-checked
      //----------------------------------------
      GenErr(cls, ERR, 1, mk_sequence(PrintName(cls)));
      reswf = false;
    }
#ifdef VICE
    if (cls == ASTAUX::MkNameFromId(ASTAUX::MkId(L"CPU"), NilContextId)) {
      //------------------------------------------
      //-- Error message #405
      //-- No inheritance from CPU is allowed
      //------------------------------------------
      GenErr(cls, ERR, 405, Sequence());
      reswf = false;
    }
    else if (cls == ASTAUX::MkNameFromId(ASTAUX::MkId(L"BUS"), NilContextId)) {
      //------------------------------------------
      //-- Error message #406
      //-- No inheritance from BUS is allowed
      //------------------------------------------
      GenErr(cls, ERR, 406, Sequence());
      reswf = false;
    }
    else if (IsSysClass(cls)) {
      //------------------------------------------
      //-- Error message #407
      //-- No inheritance from system "%1" is allowed
      //------------------------------------------
      GenErr(cls, ERR, 407, mk_sequence(PrintName(cls)));
      reswf = false;
    }
#endif // VICE
  }

  if (IsLocalSuperSuper(as_class.GetRecord(pos_AS_Class_nm))) {
    //----------------------------------------
    // Error message #2
    // Circular inheritance dependecy detected
    //----------------------------------------
    GenErr(as_class.GetRecord(pos_AS_Class_nm), ERR, 2, Sequence());
    reswf = false;
  }

  if (! as_class.GetField(pos_AS_Class_defs).IsNil ()) {
    const TYPE_AS_Name & clsnm (as_class.GetRecord(pos_AS_Class_nm));

    SetDefClass(clsnm);                 // 20070305

    const TYPE_AS_Definitions & defs (as_class.GetRecord(pos_AS_Class_defs));

    // map AS`Name to ENV`FunctionInfo
    Map recMap (SSREC::BuildRecMap(clsnm, defs.GetMap(pos_AS_Definitions_fnm)));
    setRecMap(getRecMap().ImpOverride(recMap));

    reswf = wf_TypeDefs     (i, defs.GetMap(pos_AS_Definitions_typem)) && reswf;
    reswf = wf_InstanceVars (i, defs.GetSequence(pos_AS_Definitions_instvars)) && reswf;

    reswf = wf_Functions    (i, defs.GetMap(pos_AS_Definitions_fnm)) && reswf;
    reswf = wf_Operations   (i, defs.GetMap(pos_AS_Definitions_opm)) && reswf;

    reswf = wf_Sync         (i, clsnm, defs.GetSequence(pos_AS_Definitions_syncs)) && reswf;
    reswf = wf_Thread       (i, defs.GetField(pos_AS_Definitions_threaddef), clsnm) && reswf;
    reswf = wf_Traces       (i, defs.GetMap(pos_AS_Definitions_tracem)) && reswf;
    reswf = CheckThreadInherit(clsnm) && reswf;
  }
  InitEnv();
  return reswf;
}
#endif //VDMPP

#ifdef VDMSL
// wf_Module
// i : TYPE`Ind
// mod (AS`Module | AS`DLModule)
// ==> bool
bool StatSem::wf_Module (const Int & i, const Record & mod)
{
  switch (mod.GetTag()) {
    case TAG_TYPE_AS_Module: {
      const TYPE_AS_Name & modid       (mod.GetRecord(pos_AS_Module_nm));
      const TYPE_AS_Interface & intf   (mod.GetRecord(pos_AS_Module_intf));
      const TYPE_AS_Definitions & defs (mod.GetField (pos_AS_Module_defs));
      const Generic & exp              (intf.GetField(pos_AS_Interface_exp));
  
      SetCurMod(modid);
  
      bool reswf = ExpandModule (i, mod);

      if (Settings.VDMSLmode() && !(modid == ASTAUX::GetDefaultModASName())) {
        //---------------------------------------------
        // Error message #351
        // Modules are not supported by standard VDM-SL
        //---------------------------------------------
        GenErr (modid, ERR, 351, Sequence());
        reswf = false;
      }
  
      Map recMap = SSREC::BuildRecMap(modid, defs.GetMap(pos_AS_Definitions_fnm));
      setRecMap(getRecMap().ImpOverride(recMap));
  
      reswf = wf_Operations (i, defs.GetMap(pos_AS_Definitions_opm)) && reswf;
      reswf = wf_Functions (i, defs.GetMap(pos_AS_Definitions_fnm)) && reswf;
      reswf = wf_TypeDefs (i, defs.GetMap(pos_AS_Definitions_typem)) && reswf;
  
      if ( !defs.GetField(pos_AS_Definitions_State).IsNil() ) {
        reswf = wf_State (i, defs.GetRecord(pos_AS_Definitions_State)) && reswf;
      } 
      reswf = wf_Traces (i, defs.GetMap(pos_AS_Definitions_tracem)) && reswf;
  
      if (!exp.IsNil() ) {
        LeaveCurModule (modid, exp);
      } 
      InitEnv();
      return reswf;
      break;
    }
    case TAG_TYPE_AS_DLModule: {
      return ExpandDLModule (i, mod);
    }
    default: { return false; } //not occur
  }
}
#endif //VDMSL

// wf_TypeDefs
// i : TYPE`Ind
// tp_m : map AS`Name to AS`TypeDef
// ==> bool
bool StatSem::wf_TypeDefs (const Int & i, const Map & tp_m)
{
  bool reswf = true;

  PushContext (Int(AS_PURE));

  Set dom_tp_m (tp_m.Dom());
  Generic tp;
  for (bool bb = dom_tp_m.First (tp) ; bb ; bb = dom_tp_m.Next(tp)) {
    TYPE_AS_TypeDef rc (tp_m[tp]);
    const TYPE_AS_Name & nm    (rc.GetRecord(pos_AS_TypeDef_nm));
    const TYPE_AS_Type & Tp    (rc.GetRecord(pos_AS_TypeDef_shape));
    const Generic & Inv        (rc.GetField (pos_AS_TypeDef_Inv));
    const Generic & Eq         (rc.GetField (pos_AS_TypeDef_Eq));
    const Generic & Ord        (rc.GetField (pos_AS_TypeDef_Ord));
#ifdef VDMPP
    const TYPE_AS_Access & acc (rc.GetField (pos_AS_TypeDef_access));
#endif //VDMPP

    TYPE_REP_TypeRep tprep (TransType(Nil (), Tp));

    reswf =
#ifdef VDMPP
      CheckTypeDefAccess (acc, nm) &&
      CheckTypeAccess (nm, Tp) &&
#endif //VDMPP
      wf_Type (i, Tp) && reswf;

    if (IsUnionRecursive(tprep)) {
      //---------------------------
      // Error message #3
      // Type name L"%1" is infinite
      //---------------------------
      GenErr (nm, ERR, 3, mk_sequence(PrintName(nm)));
      reswf = false;
    }
    reswf = wf_TypeInv (i, tprep, Inv, nm) && reswf;
    reswf = wf_TypeEq (i, tprep, Eq, nm) && reswf;
    reswf = wf_TypeOrd (i, tprep, Ord, nm) && reswf;
  }
  PopContext ();
  return reswf;
}

// wf_Type
// i : TYPE`Ind
// tp : AS`Type
// ==> bool
bool StatSem::wf_Type (const Int & i, const TYPE_AS_Type & type)
{
  switch(type.GetTag()) {
    case TAG_TYPE_AS_BooleanType:
    case TAG_TYPE_AS_NumericType:
    case TAG_TYPE_AS_TokenType:
    case TAG_TYPE_AS_CharType:
    case TAG_TYPE_AS_VoidType:
    case TAG_TYPE_AS_QuoteType: {
      return true;
    }
    case TAG_TYPE_AS_ProductType: {
      const SEQ<TYPE_AS_Type> & tp_l (type.GetSequence(pos_AS_ProductType_tps));
      size_t len_tp_l = tp_l.Length();
      bool reswf = true;
      for (size_t idx = 1; idx <= len_tp_l; idx++) {
        reswf = wf_Type(i, tp_l[idx]) && reswf;
      }
      return reswf;
    }
    case TAG_TYPE_AS_UnionType: {
      const SEQ<TYPE_AS_Type> & tp_l (type.GetSequence(pos_AS_UnionType_tps));
      size_t len_tp_l = tp_l.Length();
      bool reswf = true;
      for (size_t idx = 1; idx <= len_tp_l; idx++) {
        reswf = wf_Type(i, tp_l[idx]) && reswf;
      }
      return reswf;
    }
    case TAG_TYPE_AS_CompositeType: {
      const TYPE_AS_Name & tag (type.GetRecord(pos_AS_CompositeType_name));
      const SEQ<TYPE_AS_Field> & flds (type.GetSequence(pos_AS_CompositeType_fields));
      Set UsedSelectors;
      bool reswf = true;
      size_t len_flds = flds.Length();
      for(size_t idx = 1; idx <= len_flds; idx++) {
        const TYPE_AS_Field & fld (flds[idx]);
        const Generic & sel (fld.GetField(pos_AS_Field_sel));
        const TYPE_AS_Type & tp (fld.GetRecord(pos_AS_Field_type));
        reswf = wf_Type(i, tp) && reswf;
        if (sel.IsRecord () && UsedSelectors.InSet (sel)) {
          //----------------------------------
          // Error message #4
          // Selector L"%1" is multiple defined
          //----------------------------------
          GenErr (sel, ERR, 4, mk_sequence(PrintName (sel)));
          reswf = false;
        }
        else if (!sel.IsNil()) {
          UsedSelectors.Insert (sel);
        }
      }
      bool wf (CheckTag(tag));
      if (!wf) {
        //--------------------
        // Error message #5
        // Unknown tag L"%1"
        //--------------------
        GenErr (tag, ERR, 5, mk_sequence(PrintName (tag)));
      }
      return reswf && wf;
    }
    case TAG_TYPE_AS_Set0Type: {
      bool reswf = wf_Type (i, type.GetRecord(pos_AS_Set0Type_elemtp))
                && IsFlat(TransType(Nil (), type.GetRecord(pos_AS_Set0Type_elemtp)), Set());
      return reswf;
    }
    case TAG_TYPE_AS_Set1Type: {
      bool reswf = wf_Type (i, type.GetRecord(pos_AS_Set1Type_elemtp))
                && IsFlat(TransType(Nil (), type.GetRecord(pos_AS_Set1Type_elemtp)), Set());
      return reswf;
    }
    case TAG_TYPE_AS_OptionalType: {
      return wf_Type(i, type.GetRecord(pos_AS_OptionalType_tp));
    }
    case TAG_TYPE_AS_Seq0Type: {
      return wf_Type(i, type.GetRecord(pos_AS_Seq0Type_elemtp));
    }
    case TAG_TYPE_AS_Seq1Type: {
      return wf_Type(i, type.GetRecord(pos_AS_Seq1Type_elemtp));
    }
    case TAG_TYPE_AS_BracketedType: {
      return wf_Type(i, type.GetRecord(pos_AS_BracketedType_tp));
    }
    case TAG_TYPE_AS_GeneralMap0Type: {
      bool reswf = wf_Type (i, type.GetRecord(pos_AS_GeneralMap0Type_mapdom));
      reswf = IsFlat (TransType(Nil (), type.GetRecord(pos_AS_GeneralMap0Type_mapdom)), Set()) && reswf;
      reswf = wf_Type (i, type.GetRecord(pos_AS_GeneralMap0Type_maprng)) && reswf;
      return reswf;
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      bool reswf = wf_Type (i, type.GetRecord(pos_AS_GeneralMap1Type_mapdom));
      reswf = IsFlat (TransType(Nil (), type.GetRecord(pos_AS_GeneralMap1Type_mapdom)), Set()) && reswf;
      reswf = wf_Type (i, type.GetRecord(pos_AS_GeneralMap1Type_maprng)) && reswf;
      return reswf;
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      bool reswf = wf_Type (i, type.GetRecord(pos_AS_InjectiveMap0Type_mapdom));
      reswf = IsFlat (TransType(Nil (), type.GetRecord(pos_AS_InjectiveMap0Type_mapdom)), Set()) && reswf;
      reswf = wf_Type (i, type.GetRecord(pos_AS_InjectiveMap0Type_maprng)) && reswf;
      return reswf;
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      bool reswf = wf_Type (i, type.GetRecord(pos_AS_InjectiveMap1Type_mapdom));
      reswf = IsFlat (TransType(Nil (), type.GetRecord(pos_AS_InjectiveMap1Type_mapdom)), Set()) && reswf;
      reswf = wf_Type (i, type.GetRecord(pos_AS_InjectiveMap1Type_maprng)) && reswf;
      return reswf;
    }
    case TAG_TYPE_AS_OpType: {
      const SEQ<TYPE_AS_Type> & dtp_l (type.GetSequence(pos_AS_OpType_opdom));
      bool reswf = true;
      size_t len_dtp_l = dtp_l.Length();
      for (size_t idx = 1; idx <= len_dtp_l; idx++) {
        reswf = reswf && wf_Type (i, dtp_l[idx]);
      }
      reswf = wf_Type (i, type.GetRecord(pos_AS_OpType_oprng)) && reswf;
      return reswf;
    }
    case TAG_TYPE_AS_PartialFnType: {
      const SEQ<TYPE_AS_Type> & dtp_l (type.GetSequence(pos_AS_PartialFnType_fndom));
      bool reswf = true;
      size_t len_dtp_l = dtp_l.Length();
      for (size_t idx = 1; idx <= len_dtp_l; idx++) {
        reswf = reswf && wf_Type (i, dtp_l[idx]);
      }
      reswf = wf_Type (i, type.GetRecord(pos_AS_PartialFnType_fnrng)) && reswf;
      return reswf;
    }
    case TAG_TYPE_AS_TotalFnType: {
      const SEQ<TYPE_AS_Type> & dtp_l (type.GetSequence(pos_AS_TotalFnType_fndom));
      bool reswf = true;
      size_t len_dtp_l = dtp_l.Length();
      for (size_t idx = 1; idx <= len_dtp_l; idx++) {
        reswf = reswf && wf_Type (i, dtp_l[idx]);
      }
      reswf = wf_Type (i, type.GetRecord(pos_AS_TotalFnType_fnrng)) && reswf;
      return reswf;
    }
    case TAG_TYPE_AS_TypeName: {
      const TYPE_AS_Name & nm (type.GetRecord(pos_AS_TypeName_name));
      if (!CheckTypeName(nm)) {
        //------------------------------
        // Error message #6
        // Type name L"%1" is not defined
        //------------------------------
        GenErr (nm, ERR, 6, mk_sequence(PrintName(nm)));
        return false;
      }
#ifdef VICE
      else if (CpuOrBusOutSideSys(nm)) {
        if (nm == ASTAUX::MkNameFromId(ASTAUX::MkId(L"CPU"), NilContextId)) {
          //------------------------------------------
          //-- Error message #408
          //-- Instances of CPU can only be made in a system class
          //------------------------------------------
          GenErr (nm, ERR, 408, Sequence());
          return false;
        }
        else if (nm == ASTAUX::MkNameFromId(ASTAUX::MkId(L"BUS"), NilContextId)) {
          //------------------------------------------
          //-- Error message #409
          //-- Instances of BUS can only be made in a system class
          //------------------------------------------
          GenErr (nm, ERR, 409, Sequence());
          return false;
        }
        else if (IsSysClass(nm)) {
          //--------------------------------------------
          //-- Error message #410
          //-- Instances of system classes cannot be made: "%1"
          //--------------------------------------------
          GenErr (nm, ERR, 410, mk_sequence(PrintName(nm)));
          return false;
        }
        else {
          return true;
        }
      }
#endif // VICE
      else {
        return true;
      }
    }
    case TAG_TYPE_AS_TypeVar: {
      const TYPE_AS_Name & nm (type.GetRecord(1));
      if (!CheckTypeVar (TransType(Nil (), type))) {
        //---------------------------------------
        // Error message #7
        // Type variable L"%1" is not allowed here
        //---------------------------------------
        GenErr (nm, ERR, 7, mk_sequence(PrintName(nm)));
        return false;
      }
      else {
        return true;
      }
    }
    case TAG_TYPE_AS_AllType: {
      return true;
    }
    default: {
      InternalError(L"wf_Type");
      return false;
    }
  }
}

// wf_TypeInv
// i : TYPE`Ind
// Type : REP`TypeRep
// Inv : [AS`Invariant]
// nm : AS`Name
// ==> bool
bool StatSem::wf_TypeInv (const Int & i,
                               const TYPE_REP_TypeRep & Type,
                               const Generic & Inv,
                               const TYPE_AS_Name & nm)
{
  if (Inv.IsNil()) {
    return true;
  }
  else {
    TYPE_AS_Invariant inv (Inv);
    const TYPE_AS_Pattern & pat (inv.GetRecord(pos_AS_Invariant_pat));
    const TYPE_AS_Expr & expr (inv.GetRecord(pos_AS_Invariant_expr));

    bool reswf = true;
    if (ExtractPatternName (pat).Dom().InSet (nm)) {
      //----------------------------------------------------
      // Error message #8
      // Pattern in invariant must not be the type name L"%1"
      //----------------------------------------------------
      GenErr (pat, ERR, 8, mk_sequence(PrintName (nm)));
      reswf = false;
    }

    Tuple infer1 (wf_Pattern (i, pat, Type));
    const Generic & wf_pat (infer1.GetField(1)); // [bool]

    if (wf_pat == Bool(false)) {
      //----------------------------------------------------
      // Error message #352
      // Invariant pattern is different from type definition
      //----------------------------------------------------
      GenErr (nm, ERR,352, Sequence());
    }

    EnterScope(infer1.GetMap(2));

    Tuple infer2 (wf_Pred(i, expr, INV));
    const Bool & wf_body (infer2.GetBool(1));
    LeaveScope();

    reswf = reswf && (wf_pat == Bool(true)) && wf_body;

    return reswf;
  }
}

// wf_TypeEq
// i : TYPE`Ind
// Type : REP`TypeRep
// Inv : [AS`Equal]
// nm : AS`Name
// ==> bool
bool StatSem::wf_TypeEq (const Int & i,
                         const TYPE_REP_TypeRep & Type,
                         const Generic & Eq,
                         const TYPE_AS_Name & nm)
{
  if (Eq.IsNil()) {
    return true;
  }
  else {
    TYPE_AS_Equal eq (Eq);
    const TYPE_AS_Pattern & lhs (eq.GetRecord(pos_AS_Equal_lhs));
    const TYPE_AS_Pattern & rhs (eq.GetRecord(pos_AS_Equal_rhs));
    const TYPE_AS_Expr & expr (eq.GetRecord(pos_AS_Equal_expr));

    bool reswf = true;
    if (ExtractPatternName (lhs).Dom().InSet (nm)) {
      //----------------------------------------------------
      // Error message #463
      // Pattern in equality must not be the type name L"%1"
      //----------------------------------------------------
      GenErr (lhs, ERR, 463, mk_sequence(PrintName (nm)));
      reswf = false;
    }
    if (ExtractPatternName (rhs).Dom().InSet (nm)) {
      //----------------------------------------------------
      // Error message #463
      // Pattern in equality must not be the type name L"%1"
      //----------------------------------------------------
      GenErr (rhs, ERR, 463, mk_sequence(PrintName (nm)));
      reswf = false;
    }

    Tuple infer1 (wf_Pattern (i, lhs, Type));
    const Generic & wf_lhs (infer1.GetField(1)); // [bool]
    Tuple infer2 (wf_Pattern (i, rhs, Type));
    const Generic & wf_rhs (infer2.GetField(1)); // [bool]

    if ((wf_lhs == Bool(false)) || (wf_rhs == Bool(false))) {
      //----------------------------------------------------
      // Error message #464
      // Equality pattern is different from type definition
      //----------------------------------------------------
      GenErr (nm, ERR,464, Sequence());
    }

    EnterScope(infer1.GetMap(2).Override(infer2.GetMap(2)));

    Tuple infer3 (wf_Pred(i, expr, EQUALITY));
    const Bool & wf_body (infer3.GetBool(1));
    LeaveScope();

    reswf = reswf && (wf_lhs == Bool(true)) && (wf_rhs == Bool(true)) && wf_body;

    return reswf;
  }
}

// wf_TypeOrd
// i : TYPE`Ind
// Type : REP`TypeRep
// Inv : [AS`Equal]
// nm : AS`Name
// ==> bool
bool StatSem::wf_TypeOrd (const Int & i,
                          const TYPE_REP_TypeRep & Type,
                          const Generic & Ord,
                          const TYPE_AS_Name & nm)
{
  if (Ord.IsNil()) {
    return true;
  }
  else {
    TYPE_AS_Equal ord (Ord);
    const TYPE_AS_Pattern & lhs (ord.GetRecord(pos_AS_Equal_lhs));
    const TYPE_AS_Pattern & rhs (ord.GetRecord(pos_AS_Equal_rhs));
    const TYPE_AS_Expr & expr (ord.GetRecord(pos_AS_Equal_expr));

    bool reswf = true;
    if (ExtractPatternName (lhs).Dom().InSet (nm)) {
      //----------------------------------------------------
      // Error message #466
      // Pattern in equality must not be the type name L"%1"
      //----------------------------------------------------
      GenErr (lhs, ERR, 466, mk_sequence(PrintName (nm)));
      reswf = false;
    }
    if (ExtractPatternName (rhs).Dom().InSet (nm)) {
      //----------------------------------------------------
      // Error message #466
      // Pattern in equality must not be the type name L"%1"
      //----------------------------------------------------
      GenErr (rhs, ERR, 466, mk_sequence(PrintName (nm)));
      reswf = false;
    }

    Tuple infer1 (wf_Pattern (i, lhs, Type));
    const Generic & wf_lhs (infer1.GetField(1)); // [bool]
    Tuple infer2 (wf_Pattern (i, rhs, Type));
    const Generic & wf_rhs (infer2.GetField(1)); // [bool]

    if ((wf_lhs == Bool(false)) || (wf_rhs == Bool(false))) {
      //----------------------------------------------------
      // Error message #467
      // Equality pattern is different from type definition
      //----------------------------------------------------
      GenErr (nm, ERR,467, Sequence());
    }

    EnterScope(infer1.GetMap(2).Override(infer2.GetMap(2)));

    Tuple infer3 (wf_Pred(i, expr, EQUALITY));
    const Bool & wf_body (infer3.GetBool(1));
    LeaveScope();

    reswf = reswf && (wf_lhs == Bool(true)) && (wf_rhs == Bool(true)) && wf_body;

    return reswf;
  }
}

// wf_TypeList
// i : TYPE`Ind
// tp_l : seq of AS`Type
// ==> bool
bool StatSem::wf_TypeList (const Int & i, const SEQ<TYPE_AS_Type> & tp_l)
{
  bool reswf = true;

  size_t len_tp_l = tp_l.Length();
  for (size_t idx = 1; idx <= len_tp_l; idx++) {
    reswf = reswf && wf_Type (i, tp_l[idx]);
  }

  return reswf;
}

// IsFlat
// opttp: [REP`TypeRep]
// checked : set of AS`Name
// ==> bool
bool StatSem::IsFlat (const Generic & , const SET<TYPE_AS_Name> &) {
  return true;
}
//  bool reswf (true);
//  if (opttp.IsRecord()) {
//    Record tp (opttp);
//    switch (tp.GetTag ()) {
//    case ProductTypeRep:
//      {
//        Generic g; Sequence sq (tp.GetField (1));
//        for (bool bb = sq.First(g); bb; bb = sq.Next(g))
//          reswf = (IsFlat (pi, g, checked)) && reswf;
//        return reswf;
//      }
//    case UnionTypeRep:
//      {
//        Generic g; Set sq (tp.GetField (1));
//        for (bool bb = sq.First(g); bb; bb = sq.Next(g))
//          reswf = (IsFlat (pi, g, checked)) && reswf;
//        return resw;
//      }
//    case CompositeTypeRep:
//      {
//        Generic g; Sequence flds (tp.GetField (2));
//        for (bool bb = flds.First(g); bb; bb = flds.Next(g))
//          reswf = (IsFlat (pi, Record (g).GetField (2),checked)) && reswf;
//        return reswf;
//      }
//    case InvTypeRep:
//      {
//        return IsFlat (pi, tp.GetField (1),checked);
//      }
//    case GeneralMapTypeRep:
//    case InjectiveMapTypeRep:
//      {
//        return IsFlat (pi, tp.GetField (2),checked);
//      }
//    case OpTypeRep:
//    case PolyTypeRep:
//    case PartialFnTypeRep:
//    case TotalFnTypeRep:
//      {
//        GenErr (pi, ERR,L"A non-flat type cannot be used here");
//        return false;
//      }
//    case TypeNameRep:  {
//      Record nm (tp.GetField (1));
//      if (!checked.InSet(nm) && CheckTypeName(nm) && !IsUnionRecursive(tp))
//        return IsFlat (pi, LookUpTypeName (nm), checked.Insert(nm));
//      else
//        return true;
//    }
//    default:
//      {
//        return true;
//      }
//    }
//  }
//  else
//    return true;
//}
//

// wf_Functions
// i : TYPE`Ind
// fn_m : map AS`Name to AS`FnDef
// ==> bool
bool StatSem::wf_Functions (const Int & i, const Map & fn_m)
{
  bool reswf = true;

  PushContext (Int(AS_PURE));

  SET<TYPE_AS_Name> dom_fn_m (fn_m.Dom());
  Generic nm;
  for (bool bb = dom_fn_m.First(nm) ; bb ; bb = dom_fn_m.Next(nm)) {
    const TYPE_AS_FnDef & fn (fn_m[nm]);
    switch(fn.GetTag()) {
      case TAG_TYPE_AS_ImplFnDef: {
        reswf = wf_ImplFunction (i, fn) && reswf;
#ifdef VDMPP
        const TYPE_AS_Name & fnm            (fn.GetRecord  (pos_AS_ImplFnDef_nm));
        const SEQ<TYPE_AS_TypeVar> & params (fn.GetSequence(pos_AS_ImplFnDef_params));
        const TYPE_AS_Access & access       (fn.GetField   (pos_AS_ImplFnDef_access));
        const Bool & stat                   (fn.GetBool    (pos_AS_ImplFnDef_stat));

        TYPE_REP_TypeRep fntp (ExportImplicitFunction(GetCurClass(), fn));
        if (params.IsEmpty()) {
          reswf = CheckDefAccess (MakeFnRng (fntp, access, stat), fnm) && reswf;
        }
        else {
          SEQ<TYPE_REP_TypeRep> r_l (TransTypeList (GetCurClass(), params));
          reswf = CheckDefAccess (MakePolyRng (mk_REP_PolyTypeRep(r_l, fntp), access, stat), fnm) && reswf;
        }
#endif // VDMPP
        break;
      }
      case TAG_TYPE_AS_ExplFnDef: {
        reswf = wf_ExplFunction (i, fn) && reswf;
#ifdef VDMPP
        const TYPE_AS_Name & fnm             (fn.GetRecord(pos_AS_ExplFnDef_nm));
        const SEQ<TYPE_AS_TypeVar> & tpparms (fn.GetSequence(pos_AS_ExplFnDef_tpparms));
        const TYPE_AS_Access & accs          (fn.GetField(pos_AS_ExplFnDef_access));
        const Bool & stat                    (fn.GetBool(pos_AS_ExplFnDef_stat));

        TYPE_REP_TypeRep fntp (ExportExplicitFunction(GetCurClass(), fn));
        if (tpparms.IsEmpty()) {
          reswf = CheckDefAccess (MakeFnRng (fntp, accs, stat), fnm) && reswf;
        }
        else {
          SEQ<TYPE_REP_TypeRep> r_l (TransTypeList (GetCurClass(), tpparms));
          reswf = CheckDefAccess (MakePolyRng (mk_REP_PolyTypeRep(r_l, fntp), accs, stat), nm) && reswf;
        }
#endif // VDMPP
        break;
      }
      case TAG_TYPE_AS_ExtExplFnDef: {
        reswf = wf_ExtExplFunction (i, fn) && reswf;
#ifdef VDMPP
        const TYPE_AS_Name & fnm            (fn.GetRecord(pos_AS_ExtExplFnDef_nm));
        const SEQ<TYPE_AS_TypeVar> & params (fn.GetSequence(pos_AS_ExtExplFnDef_params));
        const TYPE_AS_Access & accs         (fn.GetField(pos_AS_ExtExplFnDef_access));
        const Bool & stat                   (fn.GetBool(pos_AS_ExtExplFnDef_stat));

        TYPE_REP_TypeRep fntp (ExportExtExplicitFunction(GetCurClass(), fn));
        if (params.IsEmpty()) {
          reswf = CheckDefAccess (MakeFnRng (fntp, accs, stat), fnm) && reswf;
        }
        else {
          SEQ<TYPE_REP_TypeRep> r_l (TransTypeList (GetCurClass(), params));
          reswf = CheckDefAccess (MakePolyRng (mk_REP_PolyTypeRep(r_l, fntp), accs, stat), fnm) && reswf;
        }
#endif // VDMPP
        break;
      }
      default: {
        break;
      }
    }
  }
  PopContext ();
  return reswf;
}

// wf_ImplFunction
// i : TYPE`Ind
// vFnDef AS`ImplFnDef
// ==> bool
bool StatSem::wf_ImplFunction (const Int & i, const TYPE_AS_ImplFnDef & vFnDef)
{
  const TYPE_AS_Name & nm                 (vFnDef.GetRecord  (pos_AS_ImplFnDef_nm));
  const SEQ<TYPE_AS_TypeVar> & tv_l       (vFnDef.GetSequence(pos_AS_ImplFnDef_params));
  const SEQ<TYPE_AS_PatTypePair> & partps (vFnDef.GetSequence(pos_AS_ImplFnDef_partps));
  const SEQ<TYPE_AS_NameType> & resnmtps  (vFnDef.GetSequence(pos_AS_ImplFnDef_resnmtps));
  const Generic & fnpre                   (vFnDef.GetField   (pos_AS_ImplFnDef_fnpre));
  const TYPE_AS_Expr & fnpost             (vFnDef.GetRecord  (pos_AS_ImplFnDef_fnpost));
#ifdef VDMPP
  const TYPE_AS_Access & access           (vFnDef.GetField   (pos_AS_ImplFnDef_access));
  Bool stat                               (Settings.VDM10() ? Bool(true) : vFnDef.GetBool (pos_AS_ImplFnDef_stat));
#endif // VDMPP

  this->WhileChecking = L"while checking function " + PrintName(nm).GetString();
  SEQ<TYPE_AS_Type> fndom (ConstructImplDomType (partps));

  SEQ<TYPE_AS_Pattern> parms (ConstructImplParms (partps));
  TYPE_AS_Type fnrng (ConstructImplRngType(resnmtps));
  MAP<TYPE_AS_Name,Tuple> valtp_bd (ExtractValueBindings (resnmtps));

  PushTypeVarContext (TransTypeList(Nil (), tv_l));

  SEQ<TYPE_REP_TypeRep> dom_l (TransTypeList(Nil(), fndom));
#ifdef VDMPP
  bool ok_fntps = CheckParmsInHierarchy(nm, dom_l, access, false);
#endif //VDMPP

  Tuple infer (wf_PatternList (i, parms, TransTypeList (Nil (), fndom), nm));
  const Bool & ok_in (infer.GetBool(1)); // bool
  const MAP<TYPE_AS_Name,Tuple> & bind (infer.GetMap(2));

  bool wf_dom (true);

  wf_dom =
#ifdef VDMPP
    CheckTypeListAccess (nm,fndom) &&
#endif //VDMPP
    wf_TypeList (i, fndom) && wf_dom;

  bool wf_rng (true);
  if (!resnmtps.IsEmpty()) {
    wf_rng =
#ifdef VDMPP
      CheckTypeAccess (nm,fnrng) &&
#endif //VDMPP
      wf_Type (i, fnrng) && wf_rng;
  }

  if (CheckAllType(fnrng)) {
    //--------------------------------------------------------------
    // Error message #447
    // '?' must not be in function/operation return type
    //--------------------------------------------------------------
    GenErr (nm, WRN1, 447, Sequence());
  }

  //SET<TYPE_AS_Name> dombind (bind.Dom());
  //dombind.ImpIntersect(valtp_bd.Dom());
  SET<TYPE_AS_Name> dombind (bind.Dom().Intersect(valtp_bd.Dom()));
  bool ok_bd (dombind.IsEmpty());

  if (!ok_bd) {
    //--------------------------------------------------------------
    // Error message #9
    // Parameter identifiers overlap the result identifiers for L"%1"
    //--------------------------------------------------------------
    GenErr (nm, ERR, 9, mk_sequence(PrintName(nm)));
  }

  EnterScope(bind);
#ifdef VDMPP
  SetStatic(stat);
#endif //VDMPP

  Tuple infer2 (wf_Pred (i, fnpre, PRE));
  const Bool & wf_pre (infer2.GetBool(1));

  EnterScope(valtp_bd);

  Tuple infer3 (wf_Pred (i, fnpost, POST));
  const Bool & wf_post (infer3.GetBool(1));

#ifdef VDMPP
  UnsetStatic();
#endif //VDMPP

  LeaveScope();
  LeaveScope();
  PopTypeVarContext ();

  return ok_in &&
#ifdef VDMPP
         ok_fntps &&
#endif //VDMPP
         ok_bd && wf_pre && wf_post && wf_rng && wf_dom && wf_rng;
}

// IsPrelim
// bdy : AS`FnBody | AS`OpBody
// -> bool
bool StatSem::IsPrelim (const Record & fnbody)
{
  switch(fnbody.GetTag()) {
    case TAG_TYPE_AS_FnBody: {
      const Generic & body (fnbody.GetField(pos_AS_FnBody_body));
#ifdef VDMSL
      return (body == Int(NOTYETSPEC));
#endif // VDMSL
#ifdef VDMPP
      return ((body == Int(SUBRESP)) || (body == Int(NOTYETSPEC)));
#endif // VDMPP
    }
    case TAG_TYPE_AS_OpBody: {
      const Generic & body (fnbody.GetField(pos_AS_OpBody_body));
#ifdef VDMSL
      return (body == Int(NOTYETSPEC));
#endif // VDMSL
#ifdef VDMPP
      return ((body == Int(SUBRESP)) || (body == Int(NOTYETSPEC)));
#endif // VDMPP
    }
    default: {
      return false; // dummy
    }
  }
}

// wf_Body
// i : TYPE`Ind
// body : (AS`FnBody | AS`OpBody)
// rng_tp : REP`TypeRep
// names : set of AS`Name
// ==> bool * (REP`TypeRep | set of REP`TypeRep)
Tuple StatSem::wf_Body(const Int & i,
                       const Record & body,
                       const TYPE_REP_TypeRep & rng_tp,
                       const SET<TYPE_AS_Name> & names_)
{
  if (IsPrelim(body)) {
    SET<TYPE_AS_Name> names (names_);
    Generic nm;
    for (bool bb = names.First(nm); bb; bb = names.Next(nm)) {
      (void) LookUp(nm, true);
    }
    return mk_(Bool(true), rng_tp);
  }
  else {
    switch(body.GetTag()) {
      case TAG_TYPE_AS_FnBody: {
        const TYPE_AS_Expr & bd (body.GetRecord(pos_AS_FnBody_body));
        return wf_Expr(i, bd, rng_tp); // bool * (REP`TypeRep | set of REP`TypeRep)
      }
      case TAG_TYPE_AS_OpBody: {
        const TYPE_AS_Stmt & bd (body.GetRecord(pos_AS_OpBody_body));
//        return wf_Stmt(i, bd, rng_tp); // bool * REP`TypeRep
        Tuple infer (wf_Stmt(i, bd, rng_tp)); // bool * REP`TypeRep
        const TYPE_CI_ContextId & cid (body.GetInt(pos_AS_OpBody_cid)); 
        if (cid != NilContextId) {
          GetCI().SetTypeInfo(cid, infer.GetRecord(2));
        }
        return infer;
      }
      default: {
        return Tuple(2); // dummy
      }
    }
  }
}

// wf_ExplFunction
// i : TYPE`Ind
// vFnDef : AS`ExplFnDef
// ==> bool
bool StatSem::wf_ExplFunction (const Int & i, const TYPE_AS_ExplFnDef & vFnDef)
{
  const TYPE_AS_Name & nm           (vFnDef.GetRecord(pos_AS_ExplFnDef_nm));
  const SEQ<TYPE_AS_TypeVar> & tv_l (vFnDef.GetSequence(pos_AS_ExplFnDef_tpparms));
  const TYPE_AS_FnType & tp         (vFnDef.GetRecord(pos_AS_ExplFnDef_tp));
  const SEQ<TYPE_AS_Parameters> & parms (vFnDef.GetSequence(pos_AS_ExplFnDef_parms));
  const TYPE_AS_FnBody & fnbody     (vFnDef.GetRecord(pos_AS_ExplFnDef_body));
  const Generic & fnpre             (vFnDef.GetField(pos_AS_ExplFnDef_fnpre));
  const Generic & fnpost            (vFnDef.GetField(pos_AS_ExplFnDef_fnpost));
#ifdef VDMPP
  const TYPE_AS_Access & access     (vFnDef.GetField(pos_AS_ExplFnDef_access));
  Bool stat                         (Settings.VDM10() ? Bool(true) : vFnDef.GetBool(pos_AS_ExplFnDef_stat));
#endif // VDMPP
  const Generic & measu             (vFnDef.GetField(pos_AS_ExplFnDef_measu));

  this->WhileChecking = L"while checking function " + PrintName(nm).GetString();

  MAP<TYPE_AS_Name,Tuple> bindings;
  TYPE_REP_TypeRep fnrng (TransType(Nil (), tp));

  PushTypeVarContext (TransTypeList(Nil (), tv_l));
  size_t len_parms = parms.Length ();

  Sequence fndom;
  while ((fnrng.Is(TAG_TYPE_REP_TotalFnTypeRep) ||
          fnrng.Is(TAG_TYPE_REP_PartialFnTypeRep)) &&
         (size_t)(fndom.Length ()) < len_parms) {
    fndom.ImpAppend (fnrng.GetField (1));
    fnrng = fnrng.GetField (2);
  }

  TYPE_REP_TypeRep tprep;
  Generic tprep2;
  while (len_parms > (size_t)(fndom.Length ()) &&
         ((fnrng.Is(TAG_TYPE_REP_TypeNameRep) &&
           (
#ifdef VDMSL
            !(tprep2 = LookUpTypeName(fnrng.GetField (1))).IsNil ()
#endif // VDMSL
#ifdef VDMPP
            !(tprep2 = CheckAccessCurClass (LookUpTypeName(fnrng.GetField (1), true))).IsNil ()
#endif //VDMPP
            && ((tprep = tprep2).Is (TAG_TYPE_REP_PartialFnTypeRep)
            || tprep.Is (TAG_TYPE_REP_TotalFnTypeRep)))) ||
          fnrng.Is (TAG_TYPE_REP_PartialFnTypeRep))) {
    if (fnrng.Is (TAG_TYPE_REP_TypeNameRep)) {
      fndom.ImpAppend (tprep.GetField (1));
      fnrng = tprep.GetField (2);
    }
    else {
      fndom.ImpAppend (fnrng.GetField (1));
      fnrng = fnrng.GetField (2);
    }
  }

  bool ok_in = true;
  if ((size_t)(fndom.Length ()) != len_parms) {
    //----------------------------------------------------------
    // Error message #10
    // There is too many Curried parameters in the function L"%1"
    //----------------------------------------------------------
    GenErr (nm, ERR, 10, mk_sequence(PrintName (nm)));

    ok_in = false;
    //size_t len_parms = parms.Length();
    for (size_t idx = 1; idx <= len_parms; idx++) {
      const SEQ<TYPE_AS_Pattern> & parm (parms[idx]);
      size_t len_parm = parm.Length();
      for (size_t idp = 1; idp <= len_parm; idp++) {
        bindings.ImpOverride (ExtractBindings (parm[idp]));
      }
    }
  }
  else {
    // for curry
    size_t ccount = 0;
    TYPE_AS_Type tp_q (tp);
    while(tp_q.Is(TAG_TYPE_AS_PartialFnType) || tp_q.Is(TAG_TYPE_AS_TotalFnType)) {
      ccount++;
      switch (tp_q.GetTag()) {
        case TAG_TYPE_AS_PartialFnType: {
          tp_q = tp_q.GetRecord(pos_AS_PartialFnType_fnrng);
          break;
        }
        case TAG_TYPE_AS_TotalFnType: {
          tp_q = tp_q.GetRecord(pos_AS_TotalFnType_fnrng);
          break;
        }
      }
    }
    if (ccount < len_parms) {
      //----------------------------------------------------------
      // Error message #10
      // There is too many Curried parameters in the function L"%1"
      //----------------------------------------------------------
      GenErr (nm, ERR, 10, mk_sequence(PrintName (nm)));
      ok_in = false;
    }

    for (size_t index = 1; index <= len_parms && index; index++) {
      Tuple infer (wf_PatternList (i, parms[index], fndom[index],nm));
      const Bool & ok (infer.GetBool(1)); // bool
      const MAP<TYPE_AS_Name,Tuple> & bind (infer.GetMap(2));

      SET<TYPE_AS_Name> overlap (bind.Dom ().Intersect (bindings.Dom ()));
      if (!overlap.IsEmpty()) {
        Generic id;
        for (bool bb = overlap.First(id); bb; bb = overlap.Next(id)) {
          //---------------------------------------------------------------------------------
          // Error message #11
          // L"%1" is used multiple times in the Curried argument list for the function L"%2"
          //---------------------------------------------------------------------------------
          GenErr (id, WRN1, 11, mk_sequence(PrintName (id), PrintName (nm)));
        }
      }

      bindings.ImpOverride (bind);
      ok_in = ok_in && ok;
    }
  }

// 20100913 -->
#ifdef VDMPP
  bool ok_subresp = true;
  if (fnbody.GetField(pos_AS_FnBody_body) == Int(SUBRESP)) {
    if ((access != Int(PUBLIC_AS)) && (access != Int(PROTECTED_AS))) {
      // ------------------------------------------------------------
      // -- Error message #438
      // -- Abstract function/operation "%1" must be defined as public or protected
      // ------------------------------------------------------------
      GenErr(nm, ERR, 438, mk_sequence(PrintName(nm)));
      ok_subresp = false;
    }
  }
#endif // VDMPP
// <-- 20100913

  SEQ<TYPE_REP_TypeRep> dom_l (TransTypeList(Nil(), tp.GetField(1)));
#ifdef VDMPP
  bool ok_fntps = CheckParmsInHierarchy(nm, dom_l, access, false);
#endif //VDMPP
  bool wf_tp (true);
  wf_tp =
#ifdef VDMPP
    CheckTypeAccess (nm,tp) &&
#endif //VDMPP
    wf_Type (i, tp) && wf_tp;

  EnterScope(bindings);
#ifdef VDMPP
  SetStatic(stat);
#endif //VDMPP

  Tuple infer (wf_Body(i,fnbody, fnrng, bindings.Dom()));
  const Bool & wf_body (infer.GetBool(1));
  const TYPE_REP_TypeRep & tpe (infer.GetRecord(2));

  bool ok_measu = wf_Measure (i, nm, measu, fndom[1], tp);

  bool ok_out = (IsCompatible (i, tpe, fnrng));

  if (!ok_out) {
    //-------------------------------------------------------------
    // Error message #12
    // Return type of function for L"%1" is different from signature
    //-------------------------------------------------------------
    GenErrTp (nm, ERR, 12, tpe, fnrng, mk_sequence(PrintName(nm)));
  }
 
  if (CheckAllType(fnrng)) {
    //--------------------------------------------------------------
    // Error message #447
    // '?' must not be in function/operation return type
    //--------------------------------------------------------------
    GenErr (nm, WRN1, 447, Sequence());
  }

  Tuple infer2 (wf_Pred (i, fnpre, PRE));
  const Bool & wf_pre (infer2.GetBool(1));

  if (!fnpost.IsNil () && (fndom.Length () > 1)) {
    //------------------------------------------------
    // Error message #13
    // A Curried function cannot have a post-condition
    //------------------------------------------------
    GenErr (nm, ERR, 13, Sequence());
    LeaveScope ();
#ifdef VDMPP
    UnsetStatic();
#endif //VDMPP
    return false;
  }

  if (!fnpost.IsNil ()) {
    MAP<TYPE_AS_Name,Tuple> mp;
    mp.Insert (ASTAUX::MkNameFromId (ASTAUX::MkId(L"RESULT"), NilContextId),
               mk_(TransType(Nil (), tp.GetField (2)), Int(1)));
    EnterScope (mp);
  }

  Tuple infer3 (wf_Pred (i, fnpost, POST));
  const Bool & wf_post (infer3.GetBool(1));

  if (!fnpost.IsNil ()) {
    LeaveScope();
  }
  LeaveScope ();

  PopTypeVarContext ();
#ifdef VDMPP
  UnsetStatic();
#endif //VDMPP

#ifdef VDMSL
  if (Settings.VDMSLmode() && !fnpost.IsNil ()) {
    //------------------------------------------------------------------------
    // Error message #353
    // Post conditions in explicit functions are not supported in standard
    // VDM-SL
    //------------------------------------------------------------------------
    GenErr (fnpost, ERR, 353, Sequence());
    return false;
  }
#endif // VDMSL

  return ok_in && wf_body &&
#ifdef VDMPP
         ok_fntps && ok_subresp &&
#endif //VDMPP
         ok_measu && ok_out && wf_pre && wf_tp && wf_post;
}

// wf_ExtExplFunction
// i : TYPE`Ind
// vFnDef : AS`ExtExplFnDef
// ==> bool
bool StatSem::wf_ExtExplFunction (const Int & i, const TYPE_AS_ExtExplFnDef & vFnDef)
{
  const TYPE_AS_Name & nm                 (vFnDef.GetRecord(pos_AS_ExtExplFnDef_nm));
  const SEQ<TYPE_AS_TypeVar> & tv_l       (vFnDef.GetSequence(pos_AS_ExtExplFnDef_params));
  const SEQ<TYPE_AS_PatTypePair> & partps (vFnDef.GetSequence(pos_AS_ExtExplFnDef_partps));
  const SEQ<TYPE_AS_NameType> & resnmtps  (vFnDef.GetSequence(pos_AS_ExtExplFnDef_resnmtps));
  const TYPE_AS_FnBody & fnbody           (vFnDef.GetRecord(pos_AS_ExtExplFnDef_body));
  const Generic & fnpre                   (vFnDef.GetField(pos_AS_ExtExplFnDef_fnpre));
  const Generic & fnpost                  (vFnDef.GetField(pos_AS_ExtExplFnDef_fnpost));
#ifdef VDMPP
  const TYPE_AS_Access & access           (vFnDef.GetField(pos_AS_ExtExplFnDef_access));
  Bool stat                               (Settings.VDM10() ? Bool(true) : vFnDef.GetBool(pos_AS_ExtExplFnDef_stat));
#endif // VDMPP

  this->WhileChecking = L"while checking function " + PrintName(nm).GetString();
  SEQ<TYPE_AS_Type> fndom (ConstructImplDomType (partps));

  SEQ<TYPE_AS_Pattern> parms (ConstructImplParms (partps));
  TYPE_AS_Type tprng (ConstructImplRngType(resnmtps));

  MAP<TYPE_AS_Name,Tuple> valtp_bd (ExtractValueBindings (resnmtps));

  TYPE_REP_TypeRep fnrng (TransType(Nil (), tprng));

  PushTypeVarContext (TransTypeList(Nil (), tv_l));
  SEQ<TYPE_REP_TypeRep> dom_l (TransTypeList(Nil(), fndom));
#ifdef VDMPP
  bool ok_fntps = CheckParmsInHierarchy(nm, dom_l, access, false);
#endif //VDMPP

  Tuple infer (wf_PatternList (i, parms, TransTypeList (Nil (), fndom), nm));
  const Bool & ok_in (infer.GetBool(1)); // bool
  const MAP<TYPE_AS_Name,Tuple> & bind (infer.GetMap(2));

  bool wf_dom =
#ifdef VDMPP
    CheckTypeListAccess( nm,fndom) &&
#endif //VDMPP
    wf_TypeList (i, fndom);

  bool wf_rng = false;
  if (!resnmtps.IsEmpty()) {
    wf_rng =
#ifdef VDMPP
      CheckTypeAccess (nm, tprng) &&
#endif //VDMPP
      wf_Type (i, tprng);
  }

  Set dombind (bind.Dom().Intersect(valtp_bd.Dom()));

  bool ok_bd = (dombind.IsEmpty());

  if (!ok_bd) {
    //--------------------------------------------------------------
    // Error message #9
    // Parameter identifiers overlap the result identifiers for L"%1"
    //--------------------------------------------------------------
    GenErr (nm, ERR, 9, mk_sequence(PrintName(nm)));
  }

// 20100913 -->
#ifdef VDMPP
  bool ok_subresp = true;
  if (fnbody.GetField(pos_AS_FnBody_body) == Int(SUBRESP)) {
    if ((access != Int(PUBLIC_AS)) && (access != Int(PROTECTED_AS))) {
      // ------------------------------------------------------------
      // -- Error message #438
      // -- Abstract function/operation "%1" must be defined as public or protected
      // ------------------------------------------------------------
      GenErr(nm, ERR, 438, mk_sequence(PrintName(nm)));
      ok_subresp = false;
    }
  }
#endif // VDMPP
// <-- 20100913

  EnterScope(bind);
#ifdef VDMPP
  SetStatic(stat);
#endif //VDMPP

  Tuple infer2 (wf_Pred (i, fnpre, PRE));
  const Bool & wf_pre (infer2.GetBool(1));

  EnterScope(valtp_bd);

  SET<TYPE_AS_Name> parmsNames;
  size_t len_parms = parms.Length();
  for (size_t idx = 1; idx <= len_parms; idx++) {
    parmsNames.ImpUnion(ExtractPatternName(parms[idx]).Dom());
  }
  SET<TYPE_AS_Name> resNames (valtp_bd.Dom().Union(parmsNames));

  Tuple infer3 (wf_Body(i, fnbody, fnrng, resNames));
  const Bool & wf_body (infer3.GetBool(1));
  const TYPE_REP_TypeRep & tpe (infer3.GetRecord(2));

  bool ok_out = (IsCompatible (i, tpe, fnrng));

  if (!ok_out) {
    //-------------------------------------------------------------
    // Error message #12
    // Return type of function for L"%1" is different from signature
    //-------------------------------------------------------------
    GenErrTp (nm, ERR, 12, tpe, fnrng, mk_sequence(PrintName(nm)));
  }

  if (CheckAllType(fnrng)) {
    //--------------------------------------------------------------
    // Error message #447
    // '?' must not be in function/operation return type
    //--------------------------------------------------------------
    GenErr (nm, WRN1, 447, Sequence());
  }

  Tuple infer4 (wf_Pred (i, fnpost, POST));
  const Bool & wf_post (infer.GetBool(1));

  LeaveScope();
  LeaveScope();
#ifdef VDMPP
  UnsetStatic();
#endif //VDMPP
  PopTypeVarContext ();

  return ok_in && wf_dom && wf_rng && wf_body && ok_out &&
#ifdef VDMPP
         ok_fntps && ok_subresp &&
#endif //VDMPP
         ok_bd && wf_pre && wf_post;
}

// wf_Pred
// i : TYPE`Ind
// Pred : [AS`Expr]
// kind : (<PRE>|<POST>|<INV>|<INIT>|<EXCEP>|<EQ>|<ORD>)
// ==> bool * REP`TypeRep
Tuple StatSem::wf_Pred (const Int & i, const Generic & Pred, const Int & kind)
{
  if (Pred.IsNil()) {
    return mk_(Bool(true), btp_bool);
  }
  else {
    Tuple infer (wf_Expr (i, Pred, btp_bool));
    const Bool & wf_pred (infer.GetBool(1));
    const TYPE_REP_TypeRep & tp (infer.GetRecord(2));

    Bool com (IsCompatible (i, tp, btp_bool));

    if (!com) {
      switch(kind) {
        case PRE: {
          //-----------------------------------------
          // Error message #22
          // Pre condition must be boolean expression
          //-----------------------------------------
          GenErrTp (Pred, ERR, 22, tp, btp_bool, Sequence());
          break;
        }
        case POST: {
          //------------------------------------------
          // Error message #14
          // Post condition must be boolean expression
          //------------------------------------------
          GenErrTp (Pred, ERR, 14, tp, btp_bool, Sequence());
          break;
        }
        case INV: {
          //-----------------------------------------------
          // Error message #15
          // Invariant condition must be boolean expression
          //-----------------------------------------------
          GenErrTp (Pred, ERR, 15, tp, btp_bool, Sequence());
          break;
        }
        case INIT: {
          //----------------------------------------------------
          // Error message #16
          // Initialisation condition must be boolean expression
          //----------------------------------------------------
          GenErrTp (Pred, ERR, 16, tp, btp_bool, Sequence());
          break;
        }
        case EXCEP: {
          //-----------------------------------------------
          // Error message #17
          // Exception condition must be boolean expression
          //-----------------------------------------------
          GenErrTp (Pred, ERR, 17, tp, btp_bool, Sequence());
          break;
        }
        case EQUALITY: {
          //-----------------------------------------------
          // Error message #465
          // Equality condition must be boolean expression
          //-----------------------------------------------
          GenErrTp (Pred, ERR, 465, tp, btp_bool, Sequence());
          break;
        }
        case ORDER: {
          //-----------------------------------------------
          // Error message #468
          // Order condition must be boolean expression
          //-----------------------------------------------
          GenErrTp (Pred, ERR, 468, tp, btp_bool, Sequence());
          break;
        }
      }
    }
    return mk_(wf_pred && com, tp);
  }
}

// wf_Operations
// i : TYPE`Ind
// opmap : map AS`Name to AS`OpDef
// return : bool
bool StatSem::wf_Operations (const Int & i, const Map & opmap)
{
  bool reswf = true;

  //PushContext(Int(ALL));

  FullOpTest(false);
  this->trapops = SET<TYPE_AS_OpDef>();

  Set dom_opmap (opmap.Dom());
  Generic nm;
  for (bool bb = dom_opmap.First(nm) ; bb ; bb = dom_opmap.Next(nm)) {
    TYPE_AS_OpDef op (opmap[nm]);
    switch(op.GetTag()) {
      case TAG_TYPE_AS_ImplOpDef: {
        const Bool & oppure           (op.GetBool(pos_AS_ImplOpDef_oppure));
        if (oppure) {
          PushContext(Int(PUREOP));
        }
        else {
          PushContext(Int(ALL));
        }
        reswf = wf_ImplOperation (i, op) && reswf;
#ifdef VDMPP
        const TYPE_AS_Name & onm      (op.GetRecord(pos_AS_ImplOpDef_nm));
        const TYPE_AS_Access & access (op.GetField(pos_AS_ImplOpDef_access));
        const Bool & stat             (op.GetBool(pos_AS_ImplOpDef_stat));

        TYPE_REP_TypeRep optp (ExportImplicitOperation(GetCurClass(), op));
        reswf = CheckDefAccess(MakeOpRng(optp, access, stat, oppure), onm) && reswf;
#endif // VDMPP
        PopContext();
        break;
      }
      case TAG_TYPE_AS_ExplOpDef: {
        const Bool & oppure        (op.GetBool  (pos_AS_ExplOpDef_oppure));
        if (oppure) {
          PushContext(Int(PUREOP));
        }
        else {
          PushContext(Int(ALL));
        }
        reswf = wf_ExplOperation (i, op) && reswf;
#ifdef VDMPP
        const TYPE_AS_Name & onm   (op.GetRecord(pos_AS_ExplOpDef_nm));
        const TYPE_AS_Access & acc (op.GetField (pos_AS_ExplOpDef_access));
        const Bool & stat          (op.GetBool  (pos_AS_ExplOpDef_stat));

        TYPE_REP_TypeRep optp (ExportExplicitOperation(GetCurClass(), op));
        reswf = CheckDefAccess(MakeOpRng(optp, acc, stat, oppure), onm) && reswf;
#endif // VDMPP
        PopContext();
        break;
      }
      case TAG_TYPE_AS_ExtExplOpDef: {
        const Bool & oppure           (op.GetBool  (pos_AS_ExtExplOpDef_oppure));
        if (oppure) {
          PushContext(Int(PUREOP));
        }
        else {
          PushContext(Int(ALL));
        }
        reswf = wf_ExtExplOperation (i, op) && reswf;
#ifdef VDMPP
        const TYPE_AS_Name & onm      (op.GetRecord(pos_AS_ExtExplOpDef_nm));
        const TYPE_AS_Access & access (op.GetField (pos_AS_ExtExplOpDef_access));
        const Bool & stat             (op.GetBool  (pos_AS_ExtExplOpDef_stat));

        TYPE_REP_TypeRep optp (ExportExtExplicitOperation(GetCurClass(), op));
        reswf = CheckDefAccess(MakeOpRng(optp, access, stat, oppure), onm) && reswf;
#endif // VDMPP
        PopContext();
        break;
      }
      default: {
        break;
      }
    }
  }

  PushContext(Int(ALL));
  FullOpTest(true);
  SET<TYPE_AS_OpDef> trops (this->trapops);
  Generic defi;
  for (bool cc = trops.First (defi); cc; cc = trops.Next (defi)) {
    switch(Record(defi).GetTag()) {
      case TAG_TYPE_AS_ExtExplOpDef: {
        reswf = wf_ExtExplOperation (i, defi) && reswf;
        break;
      }
      case TAG_TYPE_AS_ExplOpDef: {
        reswf = wf_ExplOperation (i, defi) && reswf;
        break;
      }
      default: {
        break;
      }
    }
  }
  PopContext();
  return reswf;
}

// wf_ExplOperation
// i : TYPE`Ind
// vOpDef : AS`ExplOpDef
// retrun : bool
bool StatSem::wf_ExplOperation (const Int & i, const TYPE_AS_ExplOpDef & vOpDef)
{
  const TYPE_AS_Name & opnm     (vOpDef.GetRecord(pos_AS_ExplOpDef_nm));
  const TYPE_AS_OpType & optp   (vOpDef.GetRecord(pos_AS_ExplOpDef_tp));
  const SEQ<TYPE_AS_Pattern> & parms (vOpDef.GetSequence(pos_AS_ExplOpDef_parms));
  const TYPE_AS_OpBody & opbody (vOpDef.GetRecord(pos_AS_ExplOpDef_body));
  const Generic & oppre         (vOpDef.GetField(pos_AS_ExplOpDef_oppre));
  const Generic & oppost        (vOpDef.GetField(pos_AS_ExplOpDef_oppost));
  const Bool & oppure           (vOpDef.GetBool(pos_AS_ExplOpDef_oppure));
#ifdef VDMPP
  const Bool & sync             (vOpDef.GetBool(pos_AS_ExplOpDef_opsync));
  const TYPE_AS_Access & acc    (vOpDef.GetField(pos_AS_ExplOpDef_access));
  const Bool & stat             (vOpDef.GetBool(pos_AS_ExplOpDef_stat));
  const Bool & constr           (vOpDef.GetBool(pos_AS_ExplOpDef_constr));
#endif // VDMPP

  bool reswf = true;

  this->WhileChecking = L"while checking operation " + PrintName(opnm).GetString();

  bool wf_tp (true);
  wf_tp =
#ifdef VDMPP
    CheckTypeAccess (opnm,optp) &&
#endif //VDMPP
    wf_Type (i, optp) && wf_tp;

  SEQ<TYPE_REP_TypeRep> dom_l (TransTypeList(Nil(), optp.GetSequence(pos_AS_OpType_opdom)));

#ifdef VDMPP
  bool ok_optps = CheckParmsInHierarchy(opnm, dom_l, acc, true);
#endif //VDMPP

  Tuple infer (wf_PatternList (i, parms, dom_l, opnm));
  const Bool & ok_in (infer.GetBool(1)); // bool
  const MAP<TYPE_AS_Name,Tuple> & bindings (infer.GetMap(2));

  EnterScope(bindings);
  SetOpContext (opnm);

  Set usi (UsedStateIds (bindings.Dom ()));
  if (!usi.IsEmpty()) {
    Generic stid;
    for (bool bb = usi.First (stid); bb; bb = usi.Next (stid)) {
      //----------------------------------------------------
      // Error message #18
      // The scope of the state component L"%1" is now hidden
      //----------------------------------------------------
      //GenErr (g, WRN2, 18, mk_sequence(PrintName (g)));
      GenErr (stid, ERR, 18, mk_sequence(PrintName (stid)));
    }
  }

#ifdef VDMPP
  SetStatic(stat);
#endif //VDMPP
  SET<TYPE_AS_Name> pnms;
  size_t len_parms = parms.Length();
  for (size_t idx = 1; idx <= len_parms; idx++) {
    pnms.ImpUnion(ExtractPatternName(parms[idx]).Dom());
  }
  const TYPE_AS_Type & oprng (optp.GetRecord(pos_AS_OpType_oprng));

  if (CheckAllType(oprng)) {
    //--------------------------------------------------------------
    // Error message #447
    // '?' must not be in function/operation return type
    //--------------------------------------------------------------
    GenErr (opnm, WRN1, 447, Sequence());
  }

#ifdef VDMSL
  TYPE_REP_TypeRep rng_tp;
  if (oprng.Is(TAG_TYPE_AS_VoidType)) {
    rng_tp = mk_REP_UnionTypeRep(mk_set(rep_unittp, rep_rettp));
  }
  else {
    rng_tp = TransType(Nil (), oprng);
  }
#endif //VDMSL

#ifdef VDMPP
  TYPE_REP_TypeRep rng_tp;
  if (constr) {
    rng_tp = mk_REP_UnionTypeRep(mk_set(rep_unittp, mk_REP_ObjRefTypeRep(GetCurClass()), rep_rettp));
  }
  else if (oprng.Is(TAG_TYPE_AS_VoidType)) {
    rng_tp = mk_REP_UnionTypeRep(mk_set(rep_unittp, rep_rettp));
  }
  else {
    rng_tp = TransType(Nil (), oprng);
  }
#endif //VDMPP

  Tuple infer2 (wf_Body(i, opbody, rng_tp, pnms));
  const Bool & wf_body (infer2.GetBool(1));
  const TYPE_REP_TypeRep & tp (infer2.GetRecord(2));

  if (AnyTraps()) {
    this->trapops.Insert (vOpDef);
  }
  if (oppure && oprng.Is(TAG_TYPE_AS_VoidType)) {
    // ------------------------------------------------------------
    // -- Error message #454
    // -- Pure operation %1 must return a value
    // ------------------------------------------------------------
    GenErr(opnm, ERR, 454, mk_sequence(PrintName(opnm)));
    reswf = false;
  }
#ifdef VDMPP
  if (!sync && !oprng.Is(TAG_TYPE_AS_VoidType)) {
    // ------------------------------------------------------------
    // -- Error message #436
    // -- Asynchronous operation %1 cannot return a value
    // ------------------------------------------------------------
    GenErr(opnm, ERR, 436, mk_sequence(PrintName(opnm)));
    reswf = false;
  }
  if (constr) {
    if (!sync) {
      // ------------------------------------------------------------
      // -- Error message #432
      // -- Constructors like "%1" cannot be defined async
      // ------------------------------------------------------------
      GenErr(opnm, ERR, 432, mk_sequence(PrintName(GetCurClass())));
      reswf = false;
    }
    if (oppure) {
      // ------------------------------------------------------------
      // -- Error message #455
      // -- Constructors like "%1" cannot be defined pure
      // ------------------------------------------------------------
      GenErr(opnm, ERR, 455, mk_sequence(PrintName(opnm)));
      reswf = false;
    }
    if (stat) {
      // ------------------------------------------------------------
      // -- Error message #445
      // -- Constructor cannot be defined static
      // ------------------------------------------------------------
      GenErr(opnm, ERR, 445, Sequence());
      reswf = false;
    }
    if(oprng.Is(TAG_TYPE_AS_VoidType) ||
       !(oprng.Is(TAG_TYPE_AS_TypeName) && (Record(oprng).GetRecord(pos_AS_TypeName_name) == GetCurClass()) ) ) {
      // ------------------------------------------------------------
      // -- Error message #393
      // -- Constructor does not have correct return type: "%1"
      // ------------------------------------------------------------
      GenErr(opnm, ERR, 393, mk_sequence(PrintName(GetCurClass())));
      reswf = false;
    }
  }
  if ((opbody.GetField(pos_AS_OpBody_body) == Int(SUBRESP)) && (acc != Int(PUBLIC_AS)) && (acc != Int(PROTECTED_AS))) {
    // ------------------------------------------------------------
    // -- Error message #438
    // -- Abstract function/operation "%1" must be defined as public or protected
    // ------------------------------------------------------------
    GenErr(opnm, ERR, 438, mk_sequence(PrintName(opnm)));
    reswf = false;
  }
#endif //VDMPP

  bool ok_out = (IsCompatible (i, tp, rng_tp));

  if ((!ok_out)
#ifdef VDMPP
      && !(constr && (tp.Is(TAG_TYPE_REP_UnitTypeRep)))
#endif //VDMPP
     ) {
#ifdef VDMPP
    if (constr) {
      // ------------------------------------------------------------
      // -- Error message #390
      // -- Constructor does not return a reference to class L"%1"
      // ------------------------------------------------------------
      GenErr(opnm, ERR, 390, mk_sequence(PrintName(GetCurClass())));
      reswf = false; // 
    }
    else
#endif //VDMPP
    {
      //--------------------------------------------------------------
      // Error message #19
      // Return type of operation for L"%1" is different from signature
      //--------------------------------------------------------------
      GenErrTp (opnm, ERR, 19, tp, rng_tp, mk_sequence(PrintName(opnm)));
      reswf = false; //
    }
  }

  // ExplOperation dos't have ext definition
  reswf = SetExtAll(pnms) && reswf;

  PushContext(Int(PRE)); // need ext

  Tuple infer3 (wf_Pred (i, oppre, PRE));
  const Bool & wf_pre (infer3.GetBool(1));

  PopContext();

  if (!oppost.IsNil ()) {
    PushContext(Int(POST)); // need ext

    if (!tp.Is (TAG_TYPE_REP_UnitTypeRep)) {
      MAP<TYPE_AS_Name,Tuple> mp;
      mp.Insert (ASTAUX::MkNameFromId (ASTAUX::MkId(L"RESULT"), NilContextId), mk_(tp, Int(1)));
      EnterScope (mp); //     UpdateScope (mp);
    }

    Tuple infer4 (wf_Pred (i, oppost, POST));
    const Bool & wf_post (infer4.GetBool(1));

    PopContext();

    if (!tp.Is (TAG_TYPE_REP_UnitTypeRep)) {
      LeaveScope ();
    }
    reswf = reswf && wf_post;
  }

  LeaveScope ();
#ifdef VDMPP
  UnsetStatic();
#endif //VDMPP

#ifdef VDMSL
  if (Settings.VDMSLmode() && !oppost.IsNil ()) {
    //--------------------------------------------------------------------------
    // Error message #354
    // Post conditions in explicit operations are not supported in standard
    // VDM-SL
    //--------------------------------------------------------------------------
    GenErr (oppost, ERR,354, Sequence());
    return false;
  }
#endif // VDMSL

  return ok_in && wf_tp && wf_body && ok_out &&
#ifdef VDMPP
         ok_optps &&
#endif //VDMPP
         wf_pre && reswf;
}

// wf_ImplOperation
// i : TYPE`Ind
// vOpDef : AS`ImplOpDef
// return : bool
bool StatSem::wf_ImplOperation (const Int & i, const TYPE_AS_ImplOpDef & vOpDef)
{
  const TYPE_AS_Name & opnm        (vOpDef.GetRecord(pos_AS_ImplOpDef_nm));
  const SEQ<TYPE_AS_PatTypePair> & partps (vOpDef.GetSequence(pos_AS_ImplOpDef_partps));
  const SEQ<TYPE_AS_NameType> & resnmtps (vOpDef.GetSequence(pos_AS_ImplOpDef_resnmtps));
  const SEQ<TYPE_AS_ExtInf> & opext (vOpDef.GetSequence(pos_AS_ImplOpDef_opext));
  const Generic & oppre            (vOpDef.GetField(pos_AS_ImplOpDef_oppre));
  const TYPE_AS_Expr & oppost      (vOpDef.GetRecord(pos_AS_ImplOpDef_oppost));
  const SEQ<TYPE_AS_Error> & excps (vOpDef.GetSequence(pos_AS_ImplOpDef_excps));
  const Bool & oppure              (vOpDef.GetBool(pos_AS_ImplOpDef_oppure));
#ifdef VDMPP
  const Bool & sync                (vOpDef.GetBool(pos_AS_ImplOpDef_opsync));
  const TYPE_AS_Access & access    (vOpDef.GetField(pos_AS_ImplOpDef_access));
  const Bool & stat                (vOpDef.GetBool(pos_AS_ImplOpDef_stat));
  const Bool & constr              (vOpDef.GetBool(pos_AS_ImplOpDef_constr));
#endif // VDMPP

  bool reswf = true;
  this->WhileChecking = L"while checking operation " + PrintName(opnm).GetString();

  SEQ<TYPE_AS_Type> opdom (ConstructImplDomType (partps));
  SEQ<TYPE_AS_Pattern> parms (ConstructImplParms (partps));
  TYPE_AS_Type oprng (ConstructImplRngType(resnmtps));
  MAP<TYPE_AS_Name, Tuple> valtp_bd (ExtractValueBindings (resnmtps));

  SEQ<TYPE_REP_TypeRep> dom_l (TransTypeList(Nil(), opdom));
#ifdef VDMPP
  bool ok_optps = CheckParmsInHierarchy(opnm, dom_l, access, true);
#endif //VDMPP

  Tuple infer (wf_PatternList (i, parms, dom_l, opnm));
  const Bool & ok_in (infer.GetBool(1)); // bool
  const MAP<TYPE_AS_Name,Tuple> & bind (infer.GetMap(2));

  Set pnms;
  Generic g;
  for (bool bb = parms.First(g); bb; bb = parms.Next(g)) {
    pnms.ImpUnion(ExtractPatternName(g).Dom());
  }
  bool wf_dom (true);
  wf_dom =
#ifdef VDMPP
    CheckTypeListAccess (opnm,opdom) &&
#endif //VDMPP
    wf_TypeList (i, opdom) && wf_dom;
  bool wf_rng = true;
  bool wf_ext;
  if (resnmtps.IsEmpty()) {
    wf_ext = SetExt(opext, pnms);
  }
  else {
    wf_rng =
#ifdef VDMPP
      CheckTypeAccess (opnm,oprng) &&
#endif //VDMPP
      wf_Type (i, oprng) && wf_rng;
    wf_ext = SetExt(opext, pnms.ImpUnion(valtp_bd.Dom()));
  }

  if (CheckAllType(oprng)) {
    //--------------------------------------------------------------
    // Error message #447
    // '?' must not be in function/operation return type
    //--------------------------------------------------------------
    GenErr (opnm, WRN1, 447, Sequence());
  }

  SET<TYPE_AS_Name> dombind (bind.Dom());
  dombind.ImpIntersect(valtp_bd.Dom());
  bool ok_bd = dombind.IsEmpty();

  if (!ok_bd) {
    //--------------------------------------------------------------
    // Error message #9
    // Parameter identifiers overlap the result identifiers for L"%1"
    //--------------------------------------------------------------
    GenErr (opnm, ERR, 9, mk_sequence(PrintName(opnm)));
    reswf = false;
  }

  if (oppure && oprng.Is(TAG_TYPE_AS_VoidType)) {
    // ------------------------------------------------------------
    // -- Error message #454
    // -- Pure operation %1 must return a value
    // ------------------------------------------------------------
    GenErr(opnm, ERR, 454, mk_sequence(PrintName(opnm)));
    reswf = false;
  }
#ifdef VDMPP
  if (!sync && !resnmtps.IsEmpty()) {
    // ------------------------------------------------------------
    // -- Error message #436
    // -- Asynchronous operation %1 cannot return a value
    // ------------------------------------------------------------
    GenErr(opnm, ERR, 436, mk_sequence(PrintName(opnm)));
    reswf = false;
  }

  if (constr) {
    if (oprng.Is(TAG_TYPE_AS_VoidType) ||
        (oprng.Is(TAG_TYPE_AS_TypeName) && (oprng.GetRecord(pos_AS_TypeName_name) != GetCurClass()))) {
      // ------------------------------------------------------------
      // -- Error message #390
      // -- Constructor does not return a reference to class L"%1"
      // ------------------------------------------------------------
      GenErr(opnm, ERR, 390, mk_sequence(PrintName(opnm)));
      reswf = false;
    }
    if (!sync) {
      // ------------------------------------------------------------
      // -- Error message #432
      // -- Constructors like "%1" cannot be defined async
      // ------------------------------------------------------------
      GenErr(opnm, ERR, 432, mk_sequence(PrintName(GetCurClass())));
      reswf = false;
    }
    if (oppure) {
      // ------------------------------------------------------------
      // -- Error message #455
      // -- Constructors like "%1" cannot be defined pure
      // ------------------------------------------------------------
      GenErr(opnm, ERR, 455, mk_sequence(PrintName(opnm)));
      reswf = false;
    }
    if (stat) {
      // ------------------------------------------------------------
      // -- Error message #445
      // -- Constructor cannot be defined static
      // ------------------------------------------------------------
      GenErr(opnm, ERR, 445, Sequence());
      reswf = false;
    }
  }
#endif //VDMPP

  EnterScope(bind);

#ifdef VDMPP
  // Set the static flag so that non-static expressions can't be used in a
  // static env. etc
  SetStatic(stat);
#endif //VDMPP

  bool wf_pred = wf_ImplOpBody (i, opext, oppre, oppost, excps, valtp_bd);
#ifdef VDMPP
  UnsetStatic();
#endif //VDMPP
  LeaveScope();

  return ok_in && wf_pred &&
#ifdef VDMPP
         ok_optps &&
#endif //VDMPP
         ok_bd && wf_ext && wf_dom && wf_rng && reswf;
}

// wf_ExtExplOperation
// i : TYPE`Ind
// vOpDef : AS`ExtExplOpDef
// return : bool
bool StatSem::wf_ExtExplOperation (const Int & i, const TYPE_AS_ExtExplOpDef & vOpDef)
{
  const TYPE_AS_Name & opnm               (vOpDef.GetRecord  (pos_AS_ExtExplOpDef_nm));
  const SEQ<TYPE_AS_PatTypePair> & partps (vOpDef.GetSequence(pos_AS_ExtExplOpDef_partps));
  const SEQ<TYPE_AS_NameType> & resnmtps  (vOpDef.GetSequence(pos_AS_ExtExplOpDef_resnmtps));
  const TYPE_AS_OpBody & opbody           (vOpDef.GetRecord  (pos_AS_ExtExplOpDef_body));
  const SEQ<TYPE_AS_ExtInf> & opext       (vOpDef.GetSequence(pos_AS_ExtExplOpDef_opext));
  const Generic & oppre                   (vOpDef.GetField   (pos_AS_ExtExplOpDef_oppre));
  const Generic & oppost                  (vOpDef.GetField   (pos_AS_ExtExplOpDef_oppost));
  const SEQ<TYPE_AS_Error> & excps        (vOpDef.GetSequence(pos_AS_ExtExplOpDef_excps));
  const Bool & oppure                     (vOpDef.GetBool    (pos_AS_ExtExplOpDef_oppure));
#ifdef VDMPP
  const Bool & sync                       (vOpDef.GetBool    (pos_AS_ExtExplOpDef_opsync));
  const TYPE_AS_Access & access           (vOpDef.GetField   (pos_AS_ExtExplOpDef_access));
  const Bool & stat                       (vOpDef.GetBool    (pos_AS_ExtExplOpDef_stat));
  const Bool & constr                     (vOpDef.GetBool    (pos_AS_ExtExplOpDef_constr));
#endif // VDMPP

  bool reswf = true;

  this->WhileChecking = L"while checking operation " + PrintName(opnm).GetString();

  SEQ<TYPE_AS_Type> opdom (ConstructImplDomType (partps));
  SEQ<TYPE_AS_Pattern> parms (ConstructImplParms (partps));
  TYPE_AS_Type oprng (ConstructImplRngType(resnmtps));
  MAP<TYPE_AS_Name,Tuple> valtp_bd (ExtractValueBindings (resnmtps));
  SEQ<TYPE_REP_TypeRep> dom_l (TransTypeList(Nil(), opdom));

  TYPE_REP_TypeRep rng_tp;
  if (oprng.Is(TAG_TYPE_AS_VoidType)) {
    rng_tp = mk_REP_UnionTypeRep(mk_set(rep_unittp, rep_rettp));
  }
  else {
    rng_tp = TransType(Nil (), oprng);
  }
#ifdef VDMPP
  bool ok_optps = CheckParmsInHierarchy(opnm, dom_l, access, true);
#endif //VDMPP

  Tuple infer (wf_PatternList (i, parms, dom_l, opnm));
  const Bool & ok_in (infer.GetBool(1)); // bool
  const MAP<TYPE_AS_Name,Tuple> & bind (infer.GetMap(2));

  Set pnms;
  Generic g;
  for (bool bb = parms.First(g); bb; bb = parms.Next(g)) {
    pnms.ImpUnion(ExtractPatternName(g).Dom());
  }
  bool wf_dom =
#ifdef VDMPP
    CheckTypeListAccess (opnm, opdom) &&
#endif //VDMPP
    wf_TypeList (i, opdom);
  bool wf_rng = true;
  bool wf_ext;
  if (resnmtps.IsEmpty()) {
    wf_ext = SetExt(opext, pnms);
  }
  else {
    wf_rng =
#ifdef VDMPP
      CheckTypeAccess (opnm, oprng) &&
#endif //VDMPP
      wf_Type (i, oprng) && wf_rng;

    wf_ext = SetExt(opext, pnms.ImpUnion(valtp_bd.Dom()));

    if (CheckAllType(oprng)) {
      //--------------------------------------------------------------
      // Error message #447
      // '?' must not be in function/operation return type
      //--------------------------------------------------------------
      GenErr (opnm, WRN1, 447, Sequence());
    }
  }

  Set dombind (bind.Dom());
  dombind.ImpIntersect(valtp_bd.Dom());
  bool ok_bd = Bool(dombind.IsEmpty ());

  if (!ok_bd) {
    //--------------------------------------------------------------
    // Error message #9
    // Parameter identifiers overlap the result identifiers for L"%1"
    //--------------------------------------------------------------
    GenErr (opnm, ERR, 9, mk_sequence(PrintName(opnm)));
  }

  if (oppure && oprng.Is(TAG_TYPE_AS_VoidType)) {
    // ------------------------------------------------------------
    // -- Error message #454
    // -- Pure operation %1 must return a value
    // ------------------------------------------------------------
    GenErr(opnm, ERR, 454, mk_sequence(PrintName(opnm)));
    reswf = false;
  }

#ifdef VDMPP
  if (!sync && !resnmtps.IsEmpty()) {
    // ------------------------------------------------------------
    // -- Error message #436
    // -- Asynchronous operation %1 cannot return a value
    // ------------------------------------------------------------
    GenErr(opnm, ERR, 436, mk_sequence(PrintName(opnm)));
    reswf = false;
  }
  if (constr) {
    if (oprng.Is(TAG_TYPE_AS_VoidType) ||
        (oprng.Is(TAG_TYPE_AS_TypeName) && oprng.GetRecord(pos_AS_TypeName_name) != GetCurClass())) {
      // ------------------------------------------------------------
      // -- Error message #390
      // -- Constructor does not return a reference to class L"%1"
      // ------------------------------------------------------------
      GenErr(opnm, ERR, 390, mk_sequence(PrintName(opnm)));
      reswf = false;
    }
    if(!sync) {
      // ------------------------------------------------------------
      // -- Error message #432
      // -- Constructors like "%1" cannot be defined async
      // ------------------------------------------------------------
      GenErr(opnm, ERR, 432, mk_sequence(PrintName(GetCurClass())));
      reswf = false;
    }
    if (oppure) {
      // ------------------------------------------------------------
      // -- Error message #455
      // -- Constructors like "%1" cannot be defined pure
      // ------------------------------------------------------------
      GenErr(opnm, ERR, 455, mk_sequence(PrintName(opnm)));
      reswf = false;
    }
    if (stat) {
      // ------------------------------------------------------------
      // -- Error message #445
      // -- Constructor cannot be defined static
      // ------------------------------------------------------------
      GenErr(opnm, ERR, 445, Sequence());
      reswf = false;
    }
  } 
  if (opbody.GetField(pos_AS_OpBody_body) == Int(SUBRESP)) {
    if ((access != Int(PUBLIC_AS)) && (access != Int(PROTECTED_AS))) {
      // ------------------------------------------------------------
      // -- Error message #438
      // -- Abstract function/operation "%1" must be defined as public or protected
      // ------------------------------------------------------------
      GenErr(opnm, ERR, 438, mk_sequence(PrintName(opnm)));
      reswf = false;
    }
  }
#endif // VDMPP

  EnterScope(bind);

  PushContext(Int(PRE));

  bool wf = true;
  size_t len_excps = excps.Length();
  for (size_t idx = 1; idx <= len_excps; idx++) {
    const TYPE_AS_Error & err (excps[idx]);
    Tuple infer2 (wf_Pred (i, err.get_cond (), EXCEP));
    wf = wf && infer2.GetBool (1);
  }

  Tuple infer3 (wf_Pred (i, oppre, PRE));
  wf = wf && infer3.GetBool (1);

  EnterScope (valtp_bd);

  SetOpContext (opnm);

  Set usi (UsedStateIds (bind.Dom ()));
  if (!usi.IsEmpty()) {
    for (bool dd = usi.First (g); dd; dd = usi.Next (g)) {
      //----------------------------------------------------
      // Error message #18
      // The scope of the state component L"%1" is now hidden
      //----------------------------------------------------
      //GenErr (g, WRN2, 18, mk_sequence(PrintName (g)));
      GenErr (g, ERR, 18, mk_sequence(PrintName (g)));
    }
  }

#ifdef VDMPP
  SetStatic(stat);
#endif //VDMPP

  PopContext();

  Set resnms (valtp_bd.Dom());
  resnms.ImpUnion(pnms);

  Tuple infer4 (wf_Body(i, opbody, rng_tp, resnms));
  const Bool & wf_body (infer4.GetBool(1));
  const TYPE_REP_TypeRep & tp (infer4.GetRecord(2));

  if (AnyTraps()) {
    this->trapops.Insert (vOpDef);
  }
  bool ok_out = (IsCompatible (i, tp, rng_tp));

  if (!ok_out) {
    //--------------------------------------------------------------
    // Error message #19
    // Return type of operation for L"%1" is different from signature
    //--------------------------------------------------------------
    GenErrTp (opnm, ERR, 19, tp, rng_tp, mk_sequence(PrintName(opnm)));
    reswf = false;
  }

  if (!oppost.IsNil ()) {
    PushContext(Int(POST));

//     Generic p;
//     Set pnms;
//     for (bool ii=parms.First (p); ii; ii=parms.Next (p))
//       pnms.ImpUnion(ExtractPatternName (p).Dom());
//     reswf = SetExtAll(pnms) && reswf;

    bool wf = true;
    size_t len_excps = excps.Length(); 
    for (size_t idx = 1; idx <= len_excps; idx++) {
      const TYPE_AS_Error & err (excps[idx]);
      Tuple infer5 (wf_Pred (i, err.get_action (), EXCEP));
      wf = wf && infer5.GetBool (1);
    }
    Tuple infer6 (wf_Pred (i, oppost, POST));
    const Bool & wf_post (wf && infer6.GetBool(1));

    reswf = reswf && wf_post;

    PopContext();
  }

#ifdef VDMPP
  UnsetStatic();
#endif //VDMPP

  LeaveScope ();

  LeaveScope();

  return ok_in &&
#ifdef VDMPP
         ok_optps &&
#endif //VDMPP
         wf_dom && wf_rng && wf_body && wf && ok_bd && wf_ext && reswf;
}

#ifdef VDMPP
// wf_InstanceVars
// i : TYPE`Ind
// instdef_l : seq of AS`InstanceVarDef
// ==> bool
bool StatSem::wf_InstanceVars (const Int & i, const SEQ<TYPE_AS_InstanceVarDef> & instdef_l)
{
  bool reswf = true;

  PushContext(Int(ALL));

  size_t len_instdef_l = instdef_l.Length();
  for(size_t idx = 1; idx <= len_instdef_l; idx++) {
    const TYPE_AS_InstanceVarDef & ivd (instdef_l[idx]);
    switch(ivd.GetTag()) {
      case TAG_TYPE_AS_InstAssignDef: {
        const TYPE_AS_AssignDef & ad  (ivd.GetRecord(pos_AS_InstAssignDef_ad));
        const TYPE_AS_Access & access (ivd.GetField(pos_AS_InstAssignDef_access));
        const TYPE_AS_Name & var      (ad.GetRecord(pos_AS_AssignDef_var));
        const TYPE_AS_Type & tp       (ad.GetRecord(pos_AS_AssignDef_tp));
        const Generic & ini           (ad.GetField(pos_AS_AssignDef_dclinit));

        reswf = CheckTypeAccess (var, tp) && wf_Type (i, tp) && reswf;
        reswf = CheckInstAccess(access, var) && reswf;

        if (!ini.IsNil()) {
          TYPE_REP_TypeRep rtp (TransType(Nil(), tp));
          Tuple t (wf_Expr (i, ini, rtp));
          const TYPE_REP_TypeRep & var_tp (t.GetRecord(2));

//wcout << L"var_tp: " << INT2Q::h2gAS(var_tp) << endl;
//wcout << L"rtp: " << INT2Q::h2gAS(rtp) << endl;
          bool cmp = IsCompatible(i, var_tp, rtp);
          //bool cmp = IsCompatible(DEF, var_tp, rtp); // 20090827
          reswf = t.GetBool (1) && cmp && reswf;
          if (!cmp) {
            //------------------------------------------------------------------------
            // Error message #307
            // Initialization expression is not compatible with defining type of L"%1"
            //------------------------------------------------------------------------
            GenErrTp (var, ERR, 307, var_tp, rtp, mk_sequence(PrintName (var)));
          }
          else {
            GetCI().SetTypeInfo(var.get_cid(), var_tp);
          }
        }
        else {
          //--------------------------------------
          // Error message #25
          // The instance L"%1" is not initialized
          //--------------------------------------
          GenErr(var, WRN2, 25, mk_sequence(PrintName (var)));
        }
        break;
      }
      case TAG_TYPE_AS_InstanceInv: {
        const TYPE_AS_Expr & expr (ivd.GetRecord(pos_AS_InstanceInv_expr));
        Tuple t (wf_Pred (i, expr, INV));
        reswf = t.GetBool (1) && reswf;
      }
      default: {
        break;
      }
    }
  }

  if (!instdef_l.IsEmpty()) {
    this->WhileChecking = L"while checking instance variables";
  }
  PopContext();
  return reswf;
}

// wf_Sync
// i : TYPE`Ind
// clsnm : AS`Name
// syncs : seq of AS`SyncDef
// ==> bool
bool StatSem::wf_Sync (const Int & i, const TYPE_AS_Name & clsnm, const SEQ<TYPE_AS_SyncDef> & syncs)
{
  if (syncs.IsEmpty ()) {
    return true;
  }
  else {
    return wf_DeclarativeSync (i, clsnm, syncs);
  }
}

// wf_DeclarativeSync
// i : TYPE`Ind
// clsnm : AS`Name
// syncs : seq1 of AS`DeclarativeSync
// ==> bool
bool StatSem::wf_DeclarativeSync (const Int & i,
                                  const TYPE_AS_Name & clsnm,
                                  const SEQ<TYPE_AS_DeclarativeSync> & syncs)
{
  bool reswf = true;
  SET<TYPE_AS_Name> permis_nms;

  PushContext(Int(PERM));

  size_t len_syncs = syncs.Length();
  for (size_t idx = 1; idx <= len_syncs; idx++) {
    const TYPE_AS_SyncDef & sd (syncs[idx]);
    switch(sd.GetTag()) {
      case TAG_TYPE_AS_Permission: {
        const TYPE_AS_Name & mthd (sd.GetRecord(pos_AS_Permission_spec));
        const TYPE_AS_Expr & guard (sd.GetRecord(pos_AS_Permission_guard));
        TYPE_REP_TypeRep ExpectedGuardType (btp_bool);
  
        Tuple infer (wf_Expr (i, guard, ExpectedGuardType));
        const Bool & wf_guard (infer.GetBool (1));
        const TYPE_REP_TypeRep & guardtp (infer.GetRecord (2));
        Set mthtp (LookUpOperationName (Nil (), mthd, Nil ()));
  
        if (mthd == clsnm) {
          //-----------------------------------
          // Error message #430
          // annot put permission guard on constructor "%1"
          //-----------------------------------
          GenErr (mthd, ERR, 430, mk_sequence(PrintName(mthd)));
          reswf = false;
        }
        else if (permis_nms.InSet(mthd)) {
          //-----------------------------------
          // Error message #426
          // Multiple permission predicates are defined for "%1"
          //-----------------------------------
          GenErr (mthd, ERR, 426, mk_sequence(PrintName(mthd)));
          reswf = false;
        }
        else {
          permis_nms.Insert(mthd);
        }

        if (!IsCompatible (i, guardtp, ExpectedGuardType)) {
          //-----------------------------------
          // Error message #27
          // Guard must be a boolean expression
          //-----------------------------------
          GenErrTp (guard, ERR, 27, guardtp, ExpectedGuardType, Sequence());
          reswf = false;
        }
  
        if (mthtp.IsEmpty ()) {
          //------------------------------
          // Error message #28
          // Operation L"%1" is not defined
          //------------------------------
          GenErr (mthd, ERR, 28, mk_sequence(PrintName (mthd)));
          reswf = false;
        }
        else {
          bool forallFnOp = true;
          bool forallFn = true;
          Generic tp;
          for (bool cc = mthtp.First(tp); cc && forallFnOp; cc = mthtp.Next(tp)) {
            switch(Record(tp).GetTag()) {
              case TAG_TYPE_SSENV_AccessFnTypeRep: {
                break;
              } 
              case TAG_TYPE_SSENV_AccessOpTypeRep: {
                TYPE_SSENV_AccessOpTypeRep aor (tp);
                if (!aor.GetBoolValue(pos_SSENV_AccessOpTypeRep_oppure))
                  forallFn = false;
                break;
              }
              default: {
                forallFnOp = false;
                forallFn = false;
                break;
              }
            }
          }
          if (forallFnOp && (mthtp.Card() > 1)) {
            //------------------------------
            // Error message #424
            // Permission guard for overloaded operation "%1"
            //------------------------------
            GenErr (mthd, WRN1, 424, mk_sequence(PrintName (mthd)));
          }
          if (forallFn) {
            //------------------------------
            // Error message #428
            // Permission predicates can only be defined for operations
            //------------------------------
            GenErr (mthd, ERR, 428, Sequence());
            reswf = false;
          }
          Tuple wf_t (wf_Expr (i, mthd, rep_alltp));
          const Bool & acc (wf_t.GetBool (1));
          reswf = acc && (reswf);
        }
  
        reswf = reswf && wf_guard;
        break;
      }
      case TAG_TYPE_AS_Mutex: {
        const Generic & nm_l (sd.GetField(pos_AS_Mutex_ops));
        if (!nm_l.IsNil ()) {
          SEQ<TYPE_AS_Name> nmlist (nm_l);
          SET<TYPE_AS_Name> prev_nm;
          Generic nm;
          for (bool cc = nmlist.First (nm); cc; cc = nmlist.Next (nm)) {
            Set look (LookUpOperationName (Nil (), nm, Nil ()));
            if (look.IsEmpty ()) {
              //------------------------------
              // Error message #28
              // Operation L"%1" is not defined
              //------------------------------
              GenErr (nm, ERR, 28, mk_sequence(PrintName (nm)));
              reswf = false;
            }
            bool forall = true;
            Generic tp;
            for (bool dd = look.First(tp); dd && forall; dd = look.Next(tp)) {
              switch(Record(tp).GetTag()) {
                case TAG_TYPE_SSENV_AccessFnTypeRep:
                case TAG_TYPE_SSENV_AccessOpTypeRep: {
                  break;
                }
                default: {
                  forall = false;
                  break;
                }
              }
            }
            if (forall && (look.Card() > 1)) {
              //------------------------------
              // Error message #425
              // Mutex includes overloaded operation "%1"
              //------------------------------
              GenErr (nm, WRN1, 425, mk_sequence(PrintName (nm)));
            }
            if (prev_nm.InSet (nm)) {
              //------------------------------
              // Error message #73
              // L"%1" multiple defined
              //------------------------------
              GenErr (nm, WRN1, 73, mk_sequence(PrintName (nm)));
            }
            else {
              prev_nm.Insert (nm);
            }
          }
        }
        break;
      }
      default: {
        break;
      }
    }
  }
  PopContext();
  return reswf;
}

// wf_Thread
// i : TYPE`Ind
// thread : [AS`ThreadDef]
// clsnm : AS`Name
// ==> bool
bool StatSem::wf_Thread (const Int & i, const Generic & thread, const TYPE_AS_Name & clsnm)
{
  if (thread.IsNil ()) {
    return true;
  }
  else {
    TYPE_AS_ThreadDef threadRec (thread);
    switch(threadRec.GetTag()) {
      case TAG_TYPE_AS_PerObl: { return wf_PeriodicThread (i, thread, clsnm); }
      case TAG_TYPE_AS_SpoObl: { return wf_SporadicThread (i, thread, clsnm); }
      default:                 { return wf_ProceduralThread (i, thread); }
    }
  }
}

// wf_PeriodicThread
// i : TYPE`Ind
// thread : AS`PerObl
// clsnm : AS`Name
// ==> bool
bool StatSem::wf_PeriodicThread (const Int & i, const TYPE_AS_PerObl& thread, const TYPE_AS_Name & clsnm)
{
  const TYPE_AS_Expr & period (thread.GetRecord(pos_AS_PerObl_period));
  const TYPE_AS_Expr & jitter (thread.GetRecord(pos_AS_PerObl_jitter));
  const TYPE_AS_Expr & delay  (thread.GetRecord(pos_AS_PerObl_delay));
  const TYPE_AS_Expr & offset (thread.GetRecord(pos_AS_PerObl_offset));
  const TYPE_AS_Name & mthd   (thread.GetRecord(pos_AS_PerObl_mtd));

  bool reswf = true;
  PushContext(Int(ALL));

  Tuple infer (wf_Expr (i, period, btp_natone));
  const Bool & wf_period (infer.GetBool (1));
  const TYPE_REP_TypeRep & periodtp (infer.GetRecord (2));

  Tuple infer2 (wf_Expr (i, jitter, btp_nat));
  const Bool & wf_jitter (infer2.GetBool (1));

  Tuple infer3 (wf_Expr (i, delay, btp_nat));
  const Bool & wf_delay (infer3.GetBool (1));

  Tuple infer4 (wf_Expr (i, offset, btp_nat));
  const Bool & wf_offset (infer4.GetBool (1));

  Set mthdtp (LookUpOperationName (Nil (), mthd, Nil()));

  if ( (period.Is(TAG_TYPE_AS_RealLit) && period.GetReal(pos_AS_RealLit_val) == Real(0)) ||
       (period.Is(TAG_TYPE_AS_NumLit) && period.GetReal(pos_AS_NumLit_val) == Real(0)) ||
       !IsCompatible (i, periodtp, btp_natone) ) {
    //----------------------------------------------------
    // Error message #32
    // Duration must evaluate to a positive natural number
    //----------------------------------------------------
    GenErr (period, ERR, 32, Sequence());
    reswf = false;
  }
/*
  -- guranteed by the parser
  --if not IsCompatible (i, jittp, ntp0) then
  --  ------------------------------------------------------
  --  -- Error message #399
  --  -- Jitter must evaluate to a non-negative natural number
  --  ------------------------------------------------------
  --( ERR`GenErr (jit, <ERROR>,399,[]);
  --  reswf := false
  --);
  -- guranteed by the parser
  --if not IsCompatible (i, disttp, ntp0) then
  --  ------------------------------------------------------
  --  -- Error message #400
  --  -- Distance must evaluate to a non-negative natural number
  --  ------------------------------------------------------
  --( ERR`GenErr (dist, <ERROR>,400,[]);
  --  reswf := false
  --);
  -- guranteed by the parser
  --if not IsCompatible (i, offsettp, ntp0) then
  --  ------------------------------------------------------
  --  -- Error message #399
  --  -- Distance must evaluate to a non-negative natural number
  --  ------------------------------------------------------
  --( ERR`GenErr (offset, <ERROR>,401,[]);
  --  reswf := false
  --);
*/
  if (delay.GetReal(pos_AS_RealLit_val).GetValue() >= period.GetReal(pos_AS_RealLit_val).GetValue()) {
    //------------------------------
    // Error message #434
    // Delay argument must be less than the period in %1
    //------------------------------
    GenErr (period, ERR, 434, mk_sequence(PrintName (clsnm)));
    reswf = false;
  }
  if (mthdtp.IsEmpty ()) {
    //------------------------------
    // Error message #28
    // Operation L"%1" is not defined
    //------------------------------
    GenErr (mthd, ERR, 28, mk_sequence(PrintName (mthd)));
    reswf = false;
  }
  else {
    Generic tp;
    for (bool bb = mthdtp.First(tp); bb; bb = mthdtp.Next(tp)) {
      switch (Record(tp).GetTag()) {
        case TAG_TYPE_SSENV_AccessFnTypeRep: {
          //------------------------------
          // Error message #458
          // Function or pure operation "%1" used in thread
          //------------------------------
          GenErr (mthd, ERR, 458, mk_sequence(PrintName (mthd)));
          reswf = false;
          break;
        } 
        case TAG_TYPE_SSENV_AccessOpTypeRep: {
          TYPE_SSENV_AccessOpTypeRep aor (tp);
          if (aor.GetBoolValue(pos_SSENV_AccessOpTypeRep_oppure)) {
            //------------------------------
            // Error message #458
            // Function or pure operation "%1" used in thread
            //------------------------------
            GenErr (mthd, ERR, 458, mk_sequence(PrintName (mthd)));
            reswf = false;
          } else {
            // TODO: check signature
          }
          break;
        } 
      }
    }
  }

  PopContext();

  return reswf && wf_period && wf_jitter && wf_delay && wf_offset;
}

// wf_SporadicThread
// i : TYPE`Ind
// thread : AS`SpoObl
// clsnm : AS`Name
// ==> bool
bool StatSem::wf_SporadicThread (const Int & i, const TYPE_AS_SpoObl& thread, const TYPE_AS_Name & clsnm)
{
  const TYPE_AS_Expr & delay  (thread.GetRecord(pos_AS_SpoObl_delay));
  const TYPE_AS_Expr & bound  (thread.GetRecord(pos_AS_SpoObl_bound));
  const TYPE_AS_Expr & offset (thread.GetRecord(pos_AS_SpoObl_offset));
  const TYPE_AS_Name & mthd   (thread.GetRecord(pos_AS_SpoObl_mtd));

  Tuple infer (wf_Expr (i, delay, btp_natone));
  const Bool & wf_delay (infer.GetBool (1));
  const TYPE_REP_TypeRep & delaytp (infer.GetRecord (2));

  Tuple infer2 (wf_Expr (i, bound, btp_natone));
  const Bool & wf_bound (infer2.GetBool (1));

  Tuple infer3 (wf_Expr (i, offset, btp_nat));
  const Bool & wf_offset (infer3.GetBool (1));

  Set mthdtp (LookUpOperationName (Nil (), mthd, Nil()));

  bool reswf = true;

  PushContext(Int(ALL));
  // TODO:
  if (mthdtp.IsEmpty ()) {
    //------------------------------
    // Error message #28
    // Operation L"%1" is not defined
    //------------------------------
    GenErr (mthd, ERR, 28, mk_sequence(PrintName (mthd)));
    reswf = false;
  }
  else {
    Generic tp;
    for (bool bb = mthdtp.First(tp); bb; bb = mthdtp.First(tp)) {
      switch (Record(tp).GetTag()) {
        case TAG_TYPE_SSENV_AccessFnTypeRep: {
          //------------------------------
          // Error message #458
          // Function or pure operation "%1" used in thread
          //------------------------------
          GenErr (mthd, ERR, 458, mk_sequence(PrintName (mthd)));
          reswf = false;
          break;
        } 
        case TAG_TYPE_SSENV_AccessOpTypeRep: {
          TYPE_SSENV_AccessOpTypeRep aor (tp);
          if (aor.GetBoolValue(pos_SSENV_AccessOpTypeRep_oppure)) {
            //------------------------------
            // Error message #458
            // Function or pure operation "%1" used in thread
            //------------------------------
            GenErr (mthd, ERR, 458, mk_sequence(PrintName (mthd)));
            reswf = false;
          } else {
            // TODO: check signature
          }
          break;
        } 
      }
    }
  }

  PopContext();
  return reswf;
}

// wf_ProceduralThread
// i : TYPE`Ind
// thread : AS`Stmt
// ==> bool
bool StatSem::wf_ProceduralThread (const Int & i, const TYPE_AS_Stmt & thread)
{
  PushContext(Int(PTHREAD));

  Tuple tmp (wf_Stmt (i,thread, rep_alltp));
  bool reswf (tmp.GetBool (1));
  const TYPE_REP_TypeRep & stmttp (tmp.GetRecord (2));

  if (!IsCompatible (i, stmttp, rep_unittp)) {
    //---------------------------------------
    // Error message #337
    // Statement must not have a return value
    //---------------------------------------
    GenErrTp (thread, ERR, 337, stmttp, rep_unittp, Sequence());
    reswf = false;
  }
  PopContext();
  return reswf;
}
#endif //VDMPP

// wf_Traces
// i : TYPE`Ind
// trace_m : map AS`NameList to AS`TraceDefList
// ==> bool
bool StatSem::wf_Traces(const Int & i, const Map & trace_m)
{
  PushContext(Int(ALL));

  bool reswf = true;
  Set nml_s (trace_m.Dom());
  Generic nml;
  for (bool bb = nml_s.First(nml); bb; bb = nml_s.Next(nml)) {
    reswf = wf_TraceDefList(i, trace_m[nml]) & reswf;
  }

  PopContext();
  return reswf;
}

// wf_TraceDefList
// i : TYPE`Ind
// tdl : AS`TraceDefList
// ==> bool
bool StatSem::wf_TraceDefList(const Int & i, const TYPE_AS_TraceDefList & tdl)
{
  bool reswf = true;
  size_t len_tdl = tdl.Length();
  for (size_t index = 1; index <= len_tdl; index++) {
    reswf = wf_TraceDefTerm(i, tdl[index]) & reswf;
  }
  return reswf;
}

// wf_TraceDefTerm
// i : TYPE`Ind
// tdt : AS`TraceDefTerm
// ==> bool
bool StatSem::wf_TraceDefTerm(const Int & i, const TYPE_AS_TraceDefTerm & tdt)
{
  switch(tdt.GetTag()) {
    case TAG_TYPE_AS_TraceApplyExpr: {
#ifdef VDMSL
      return wf_TraceApplyExpr(i,
                               tdt.GetRecord(pos_AS_TraceApplyExpr_op),
                               tdt.GetSequence(pos_AS_TraceApplyExpr_args));
#endif // VDMSL
#ifdef VDMPP
      return wf_TraceApplyExpr(i,
                               tdt.GetField(pos_AS_TraceApplyExpr_obj),
                               tdt.GetRecord(pos_AS_TraceApplyExpr_op),
                               tdt.GetSequence(pos_AS_TraceApplyExpr_args));
#endif // VDMPP
    }
    case TAG_TYPE_AS_TraceBracketedExpr: {
      return wf_TraceDefList(i, tdt.GetSequence(pos_AS_TraceBracketedExpr_list));
    }
    case TAG_TYPE_AS_TracePermuteExpr: {
      return wf_TraceDefList(i, tdt.GetSequence(pos_AS_TracePermuteExpr_list));
    }
    case TAG_TYPE_AS_QualifiedTrace: {
      return wf_QualifiedTrace(i,
                               tdt.GetSequence(pos_AS_QualifiedTrace_binds),
                               tdt.GetRecord(pos_AS_QualifiedTrace_tdef));
    }
    case TAG_TYPE_AS_RepeatTrace: {
      return wf_RepeatTrace(i,
                            tdt.GetRecord(pos_AS_RepeatTrace_tdef),
                            tdt.GetField(pos_AS_RepeatTrace_pat));
    }
    case TAG_TYPE_AS_QualifiedRepeatTrace: {
      return wf_QualifiedRepeatTrace(i,
                                     tdt.GetSequence(pos_AS_QualifiedRepeatTrace_binds),
                                     tdt.GetRecord(pos_AS_QualifiedRepeatTrace_tdef),
                                     tdt.GetField(pos_AS_QualifiedRepeatTrace_pat));
    }
    case TAG_TYPE_AS_TraceDefAltn: {
      return wf_TraceDefAltn(i,
                             tdt.GetRecord(pos_AS_TraceDefAltn_term),
                             tdt.GetRecord(pos_AS_TraceDefAltn_tdef));
    }
  }
  return true;
}

#ifdef VDMSL
// wf_TraceApplyExpr
// i : TYPE`Ind
// op : AS`Name
// args : seq of AS`Expr
// ==> bool
bool StatSem::wf_TraceApplyExpr(const Int & i,
                                const TYPE_AS_Name & opnm,
                                const SEQ<TYPE_AS_Expr> & args)
{
  bool reswf = true;

  Generic oprep (LookUp(opnm, true));

  if (oprep.Is(TAG_TYPE_REP_OpTypeRep)) {
    TYPE_REP_OpTypeRep otr (oprep);
    SEQ<TYPE_REP_TypeRep> opdomtpl (otr.GetSequence(pos_REP_OpTypeRep_Dom));
    if (opdomtpl.Length() == args.Length()) {
      size_t len = args.Length();
      for (size_t i = 1; i <= len; i++) {
        Tuple infer (wf_Expr(POS, args[i], opdomtpl[i]));
//        const Bool & arg_ok (infer.GetBool(1)); // unused
        const TYPE_REP_TypeRep & arg_tp (infer.GetRecord(2));
        if (!IsCompatible(POS, arg_tp, opdomtpl[i])) {
          //-------------------------------------------------------------
          // Error message #274
          // Operation is not applied with parameters of the correct type
          //-------------------------------------------------------------
          GenErrTp (args[i], ERR, 274, arg_tp, opdomtpl[i], Sequence()),
          reswf = false;
        }
      }
    }
    else {
      //-----------------------------------------------------------
      // Error message #275
      // Operation applied with wrong number of arguments
      //-----------------------------------------------------------
      GenErr (opnm, ERR, 275, Sequence());
      reswf = false;
    }
  }
  else if (oprep.Is(TAG_TYPE_REP_TotalFnTypeRep) || oprep.Is(TAG_TYPE_REP_PartialFnTypeRep)) {
    TYPE_REP_FnTypeRep ftr (oprep);
    SEQ<TYPE_REP_TypeRep> fndomtpl;
    switch(ftr.GetTag()) {
      case TAG_TYPE_REP_PartialFnTypeRep: {
        fndomtpl.ImpConc(ftr.GetSequence(pos_REP_PartialFnTypeRep_fndom));
        break;
      }
      case TAG_TYPE_REP_TotalFnTypeRep: {
        fndomtpl.ImpConc(ftr.GetSequence(pos_REP_TotalFnTypeRep_fndom));
        break;
      }
    }
    if (fndomtpl.Length() == args.Length()) {
      size_t len = args.Length();
      for (size_t i = 1; i <= len; i++) {
        Tuple infer (wf_Expr(POS, args[i], fndomtpl[i]));
//        const Bool & arg_ok (infer.GetBool(1)); // unused
        const TYPE_REP_TypeRep & arg_tp (infer.GetRecord(2));
        if (!IsCompatible(POS, arg_tp, fndomtpl[i])) {
          //-----------------------------------------------------------
          // Error message #272
          // Function is not applied with parameters
          // of the correct type
          //-----------------------------------------------------------
          GenErrTp (args[i], ERR, 272, arg_tp, fndomtpl[i], Sequence());
          reswf = false;
        }
      }
    }
    else {
      //-----------------------------------------------------------
      // Error message #273
      // Function applied with wrong number of arguments
      //-----------------------------------------------------------
      GenErr (opnm, ERR, 273, Sequence());
      reswf = false;
    }
  }
  else {
    //-----------------------------------------------------------
    // Error message #28
    // Operation \"%1\" is not defined
    //-----------------------------------------------------------
    GenErr (opnm, ERR, 28, mk_sequence(PrintName(opnm)));
    reswf = false;
  }
  return reswf;
}
#endif // VDMSL
#ifdef VDMPP
// wf_TraceApplyExpr
// i : TYPE`Ind
// obj : [AS`Name]
// op : AS`Name
// args : seq of AS`Expr
// ==> bool
bool StatSem::wf_TraceApplyExpr(const Int & i,
                                     const Generic & objnm,
                                     const TYPE_AS_Name & opnm, 
                                     const SEQ<TYPE_AS_Expr> & args)
{
  bool reswf = true;

  Generic oprep = Nil();
  if (objnm.IsNil()) {
    oprep = LookUp(opnm, true);
    if (!oprep.IsRecord()) {
      return false;
    }
  }
  else {
    Generic objtp_ (LookUp(objnm, true));

    if (objtp_.IsRecord()) {
      Record objtp (objtp_);
      if (objtp.Is(TAG_TYPE_REP_ObjRefTypeRep) ||
          (objtp.Is(TAG_TYPE_REP_TypeNameRep) && IsClassName(objtp.GetRecord(pos_REP_TypeNameRep_nm)))) {
        TYPE_AS_Name nm (objtp.Is(TAG_TYPE_REP_ObjRefTypeRep)
                          ? objtp.GetRecord(pos_REP_ObjRefTypeRep_nm)
                          : objtp.GetRecord(pos_REP_TypeNameRep_nm));
        //Generic oprep (LookUpInObject(nm, opnm, false));
        oprep = (LookUpInObject(nm, opnm, false, true));
      }
      else {
        return false;
      }
    }
    else {
      return false;
    }
  }

  if (oprep.Is(TAG_TYPE_SSENV_AccessOpTypeRep)) {
    TYPE_SSENV_AccessOpTypeRep aotr (oprep);
    SEQ<TYPE_REP_TypeRep> opdomtpl (aotr.GetRecord(pos_SSENV_AccessOpTypeRep_tp)
                                        .GetSequence(pos_REP_OpTypeRep_Dom));
    if (opdomtpl.Length() == args.Length()) {
      size_t len_args = args.Length();
      for (size_t i = 1; i <= len_args; i++) {
        Tuple infer (wf_Expr(POS, args[i], opdomtpl[i]));
        //const Bool & arg_ok (infer.GetBool(1)); // unused
        const TYPE_REP_TypeRep & arg_tp (infer.GetRecord(2));
        if (!IsCompatible(POS, arg_tp, opdomtpl[i])) {
          //-------------------------------------------------------------
          // Error message #274
          // Operation is not applied with parameters of the correct type
          //-------------------------------------------------------------
          GenErrTp (args[i], ERR, 274, arg_tp, opdomtpl[i], Sequence());
          reswf = false;
        }
      }
    }
    else {
      //-----------------------------------------------------------
      // Error message #275
      // Operation applied with wrong number of arguments
      //-----------------------------------------------------------
      GenErr (opnm, ERR, 275, Sequence());
      reswf = false;
    }
  }
  else if (oprep.Is(TAG_TYPE_SSENV_AccessFnTypeRep)) {
    TYPE_SSENV_AccessFnTypeRep aftr (oprep);
    const TYPE_REP_FnTypeRep & ftr (aftr.GetRecord(pos_SSENV_AccessFnTypeRep_tp));
    SEQ<TYPE_REP_TypeRep> fndomtpl;
    switch(ftr.GetTag()) {
      case TAG_TYPE_REP_PartialFnTypeRep: {
        fndomtpl.ImpConc(ftr.GetSequence(pos_REP_PartialFnTypeRep_fndom));
        break;
      }
      case TAG_TYPE_REP_TotalFnTypeRep: {
        fndomtpl.ImpConc(ftr.GetSequence(pos_REP_TotalFnTypeRep_fndom));
        break;
      }
    }
    if (fndomtpl.Length() == args.Length()) {
      size_t len_args = args.Length();
      for (size_t i = 1; i <= len_args; i++) {
        Tuple infer (wf_Expr(POS, args[i], fndomtpl[i]));
        //const Bool & arg_ok (infer.GetBool(1)); // unused
        const TYPE_REP_TypeRep & arg_tp (infer.GetRecord(2));
        if (!IsCompatible(POS, arg_tp, fndomtpl[i])) {
          //-----------------------------------------------------------
          // Error message #272
          // Function is not applied with parameters
          // of the correct type
          //-----------------------------------------------------------
          GenErrTp (args[i], ERR, 272, arg_tp, fndomtpl[i], Sequence());
          reswf = false;
        }
      }
    }
    else {
      //-----------------------------------------------------------
      // Error message #273
      // Function applied with wrong number of arguments
      //-----------------------------------------------------------
      GenErr (opnm, ERR, 273, Sequence());
      reswf = false;
    }
  }
  else {
    //-----------------------------------------------------------
    // Error message #28
    // Operation \"%1\" is not defined
    //-----------------------------------------------------------
    GenErr (opnm, ERR, 28, mk_sequence(PrintName(opnm)));
    reswf = false;
  }
  return reswf;
}
#endif // VDMPP

// wf_QualifiedTrace
// i : TYPE`Ind
// binds : seq of AS`TraceBind
// tdef : AS`SimpleTrace
// ==> bool
bool StatSem::wf_QualifiedTrace(const Int & i,
                                const SEQ<TYPE_AS_TraceBind> & binds,
                                const TYPE_AS_SimpleTrace & tdef)
{
  bool reswf (true);

  size_t len_binds = binds.Length();
  for (size_t index = 1; index <= len_binds; index++) {
    TYPE_AS_TraceBind bind (binds[index]);
    Tuple infer (wf_TraceBind(i, bind));
    const Generic & wf_b (infer.GetField(1));
    const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap(2));

    if (wf_b != Bool(true)) {
      //-----------------------------------------------------------
      // Error message #420
      // Pattern in qualified repeat trace cannot match
      //-----------------------------------------------------------
      //GenErr (bind, ERR, 420, Sequence());
      reswf = false;
    }
    EnterScope(bd);
  }

  reswf = wf_TraceDefTerm(i, tdef) && reswf;

  for (size_t i = 1; i <= len_binds; i++) {
    LeaveScope();
  }

  return reswf;
}

// wf_RepeatTrace
// i : TYPE`Ind
// tdef : AS`SimpleTrace
// pat : AS`TraceRepeatPattern ==> bool
bool StatSem::wf_RepeatTrace(const Int & i,
                             const TYPE_AS_SimpleTrace & tdef, 
                             const TYPE_AS_TraceRepeatPattern & pat)
{
  bool reswf = wf_TraceDefTerm(i, tdef);
  reswf = wf_TraceRepeatPattern(i, pat) && reswf;
  return reswf;
}

// wf_QualifiedRepeatTrace
// i : TYPE`Ind
// binds : seq of AS`TraceBind
// tdef : AS`SimpleTrace 
// pat : AS`TraceRepeatPattern
// ==> bool
bool StatSem::wf_QualifiedRepeatTrace(const Int & i,
                                      const SEQ<TYPE_AS_TraceBind> & binds,
                                      const TYPE_AS_SimpleTrace & tdef,
                                      const TYPE_AS_TraceRepeatPattern & pat)
{
  bool reswf (true);
  
  size_t len_binds = binds.Length();
  for (size_t index = 1; index <= len_binds; index++) {
    TYPE_AS_TraceBind bind (binds[index]);
    Tuple infer (wf_TraceBind(i, bind));
    const Generic & wf_b (infer.GetField(1));
    const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap(2));

    if (wf_b != Bool(true)) {
      //-----------------------------------------------------------
      // Error message #420
      // Pattern in qualified repeat trace cannot match
      //-----------------------------------------------------------
      //GenErr (bind, ERR, 420, Sequence());
      reswf = false;
    }
    EnterScope(bd);
  }

  reswf = wf_TraceDefTerm(i, tdef) && reswf;
  reswf = wf_TraceRepeatPattern(i, pat) && reswf;

  for (size_t i = 1; i <= len_binds; i++) {
    LeaveScope();
  }

  return reswf;
}

// wf_TraceDefAltn
// i : TYPE`Ind
// term : AS`TraceDefTerm
// tdef : AS`TraceDef
// ==> bool
bool StatSem::wf_TraceDefAltn(const Int & i,
                                   const TYPE_AS_TraceDefTerm & term,
                                   const TYPE_AS_TraceDef & tdef)
{
  bool reswf = wf_TraceDefTerm(i, term);
  reswf = wf_TraceDefTerm(i, tdef) && reswf;
  return reswf;
}

// wf_TraceRepeatPattern
// i : TYPE`Ind
// trp : AS`TraceRepeatPattern
// ==> bool
bool StatSem::wf_TraceRepeatPattern(const Int & i, const TYPE_AS_TraceRepeatPattern & trp)
{
  if (trp.Is(TAG_TYPE_AS_RepeatInterval)) {
    int low = (int)(Record(trp).GetRecord(pos_AS_RepeatInterval_low).GetRealValue(pos_AS_RealLit_val));
    int high = (int)(Record(trp).GetRecord(pos_AS_RepeatInterval_high).GetRealValue(pos_AS_RealLit_val));
    if (low >= high) {
      //-----------------------------------------------------------
      // Error message #419
      // Higher repeat pattern should be larger than lower repeat pattern
      //-----------------------------------------------------------
      GenErr (trp, ERR, 419, Sequence());
      return false;
    }
  }
  return true;
}

// wf_TraceBind
// i : TYPE`Ind
// bind : AS`TraceBind
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_TraceBind(const Int & i, const TYPE_AS_TraceBind & bind)
{
  switch(bind.GetTag()) {
    case TAG_TYPE_AS_LocalTraceBind: { return wf_LocalTraceBind(i, bind); }
    case TAG_TYPE_AS_LetTraceBind:   { return wf_LetTraceBind(i, bind); }
    case TAG_TYPE_AS_LetBeTraceBind: { return wf_LetBeTraceBind(i, bind); }
    default:                         { return mk_(Bool(true), Map()); // dummy
    }
  }
}

// wf_LocalTraceBind
// bind : AS`LocalTraceBind
// ==> bool * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_LocalTraceBind(const Int & i, const TYPE_AS_LocalTraceBind & bind)
{
  const SEQ<TYPE_AS_LocalDef> & ldef_l (bind.GetSequence(pos_AS_LocalTraceBind_localdefs));
  Tuple infer (ExtractPatterns (i,ldef_l));
  const Bool & wf (infer.GetBool (1)); // bool // not used
  const MAP<TYPE_AS_Name, Tuple> & bd (infer.GetMap (2)); // map AS`Name to (REP`TypeRep * bool)

  //Bool reswf (true);
  Bool reswf (wf);

  MAP<TYPE_AS_Name,Tuple> nbd;
  Set dom_bd (bd.Dom());
  Generic nm;
  for (bool bb = dom_bd.First(nm); bb; bb = dom_bd.Next(nm)) {
    nbd.Insert(nm, mk_(bd[nm].GetRecord(1), Int(1)));
  }
  size_t len_ldef_l = ldef_l.Length();
  for (size_t idx = 1; idx <= len_ldef_l; idx++) {
    const TYPE_AS_LocalDef & val (ldef_l[idx]);
    Bool wf_def;
    MAP<TYPE_AS_Name,Tuple> realbd;
    switch(val.GetTag()) {
      case TAG_TYPE_AS_ValueDef: {
        Tuple infer2 (wf_ValueDef (i, val));
        wf_def = infer2.GetBool (1);
        realbd = infer2.GetField (2);
        break;
      }
      default: {
        break;
      }
    }
    reswf = reswf && wf_def;
    nbd.ImpOverride(realbd);
  }
  return mk_(reswf, nbd);
}

// wf_LetTraceBind
// i : TYPE`Ind
// bind : AS`Bind
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_LetTraceBind(const Int & i, const TYPE_AS_LetTraceBind & bind)
{
  const SEQ<TYPE_AS_MultBind> & p (bind.GetSequence(pos_AS_LetTraceBind_bind));
  Tuple infer (wf_MultiBindList(i, p, true));
  const Generic & wf_b (infer.GetField(1));
  const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap(2));

  bool exists = false;
  size_t len_p = p.Length();
  for (size_t idx = 1; (idx <= len_p) && !exists; idx++) {
    exists = p[idx].Is(TAG_TYPE_AS_MultTypeBind);
  }
  if (exists) {
    //-----------------------------------------------------------
    // Error message #421
    // A type binding cannot be used in traces
    //-----------------------------------------------------------
    GenErr (bind, ERR, 421, Sequence());
    return mk_(Bool(false), bd); 
  }
  else {
    return mk_(wf_b, bd); 
  }
}

// wf_LetBeTraceBind
// i : TYPE`Ind
// bind : AS`LetBeTraceBind
// ==> [bool] * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_LetBeTraceBind(const Int & i, const TYPE_AS_LetBeTraceBind & bind)
{
  const SEQ<TYPE_AS_MultBind> & p (bind.GetSequence(pos_AS_LetBeTraceBind_bind));
  const Generic & e (bind.GetField(pos_AS_LetBeTraceBind_stexpr));
  
  Tuple infer (wf_MultiBindList(i, p, true));
  const Generic & wf_b (infer.GetField(1));
  const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap(2));

  bool exists = false;
  size_t len_p = p.Length();
  for (size_t idx = 1; (idx <= len_p) && !exists; idx++) {
    exists = p[idx].Is(TAG_TYPE_AS_MultTypeBind);
  }
  if (exists) {
    //-----------------------------------------------------------
    // Error message #421
    // A type binding cannot be used in traces
    //-----------------------------------------------------------
    GenErr (bind, ERR, 421, Sequence());
    return mk_(Bool(false), bd); 
  }
  else {
    if (wf_b != Bool(true)) {
      //-----------------------------------------------------------
      // Error message #113
      // Pattern in Let-Be-Expr cannot match
      //-----------------------------------------------------------
      GenErr (p, ERR, 113, Sequence());
    }

    if (!e.IsNil()) {
      EnterScope(bd);
      Tuple infer2 (wf_Expr(i, e, btp_bool));
      const Bool & wf_st (infer2.GetBool(1));
      const TYPE_REP_TypeRep & sttp (infer2.GetRecord(2));
      bool stcomp = IsCompatible(i, sttp, btp_bool);
      LeaveScope();
      return mk_(Bool((wf_b == Bool(true)) && wf_st && stcomp), bd);
    }
    else {
      return mk_(wf_b, bd);
    }
  }
}

#ifdef VDMSL
// wf_State
// i : TYPE`Ind
// state : AS`StateDef
// ==> bool
bool StatSem::wf_State (const Int & i, const TYPE_AS_StateDef & state)
{
  const TYPE_AS_CompositeType & tp (state.GetRecord(pos_AS_StateDef_tp));
  const Generic & Invar            (state.GetField (pos_AS_StateDef_Inv));
  const Generic & Init             (state.GetField (pos_AS_StateDef_Init));

  bool reswf = wf_Type (i, tp);

  if (!Invar.IsNil ()) {
    TYPE_AS_Invariant Inv (Invar);
    const TYPE_AS_Pattern & pat (Inv.GetRecord(pos_AS_Invariant_pat));
    const TYPE_AS_Expr & expr   (Inv.GetRecord(pos_AS_Invariant_expr));

    const TYPE_AS_Name & nm (tp.GetRecord(pos_AS_CompositeType_name));
    if (ExtractPatternName(pat).Dom().InSet (nm)) {
      //-----------------------------------------------------------
      // Error message #345
      // Pattern in state invariant must not be the state name L"%1"
      //-----------------------------------------------------------
      GenErr (pat, ERR, 345, mk_sequence(PrintName (nm)));
      reswf = false;
    }

    Tuple infer (wf_Pattern (i, pat, TransType(Nil (), tp)));
    const Bool & wf_pat (infer.GetField (1) == Bool(true)); // [bool]
    const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap (2));

    if (!wf_pat) {
      //----------------------------------------
      // Error message #346
      // Pattern in state invariant cannot match
      //----------------------------------------
      GenErr (pat, ERR,346, Sequence());
    }

    EnterScope (bd);

    Tuple infer2 (wf_Pred (i, expr, INV));
    const Bool & wf_body (infer2.GetBool (1));

    LeaveScope();

    reswf = reswf && wf_body && wf_pat;
  }

  if (!Init.IsNil ()) {
    TYPE_AS_StateInit Ini (Init);
    const TYPE_AS_Pattern & pat (Ini.GetRecord(pos_AS_StateInit_pat));
    const TYPE_AS_Expr & expr   (Ini.GetRecord(pos_AS_StateInit_expr));

    const TYPE_AS_Name & nm (tp.GetRecord(pos_AS_CompositeType_name));
    if (ExtractPatternName(pat).Dom().InSet (nm)) {
      //----------------------------------------------------------------
      // Error message #347
      // Pattern in state initialisation must not be the state name L"%1"
      //----------------------------------------------------------------
      GenErr (pat, ERR, 347, mk_sequence(PrintName(nm)));
      reswf = false;
    }

    Tuple infer (wf_Pattern (i, pat, TransType(Nil (), tp)));
    Bool wf_pat (infer.GetField (1) == Bool(true)); // [bool]
    const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap (2));

    if (!wf_pat) {
      //----------------------------------------
      // Error message #346
      // Pattern in state invariant cannot match
      //----------------------------------------
      GenErr (pat, ERR, 346, Sequence());
    }

    EnterScope (bd);

    Tuple infer2 (wf_Pred (i, expr, INIT));
   const Bool & wf_body (infer2.GetBool (1));

    LeaveScope();

    reswf = reswf && wf_body && wf_pat;
  }
  return reswf;
}
#endif //VDMSL

// extractDomRng
#ifdef VDMSL
// g : [REP`TypeRep]
#endif // VDMSL
#ifdef VDMPP
// g : [REP`TypeRep | ENV`AccessType |  
//     set of (ENV`AccessOpTypeRep | ENV`AccessFnTypeRep | ENV`AccessPolyTypeRep) |  
//     set of REP`TypeRep]
#endif // VDMPP
// -> (seq of REP`TypeRep) * REP`TypeRep
Tuple StatSem::extractDomRng(const Generic & g)
{
  if (g.IsRecord()) {
    Record r (g);
    switch(r.GetTag()) {
      case TAG_TYPE_REP_PartialFnTypeRep: {
        return mk_(r.GetSequence(pos_REP_PartialFnTypeRep_fndom), r.GetRecord(pos_REP_PartialFnTypeRep_fnrng));
      }
      case TAG_TYPE_REP_TotalFnTypeRep: {
        return mk_(r.GetSequence(pos_REP_TotalFnTypeRep_fndom), r.GetRecord(pos_REP_TotalFnTypeRep_fnrng));
      }
#ifdef VDMPP
      case TAG_TYPE_SSENV_AccessFnTypeRep: {
        return extractDomRng(r.GetRecord(1));
      }
      case TAG_TYPE_SSENV_AccessPolyTypeRep: {
        TYPE_REP_PolyTypeRep ptr (r.GetRecord(1));
        return extractDomRng(ptr.GetRecord(pos_REP_PolyTypeRep_tp));
      }
#endif // VDMPP
    } 
  }
  InternalError(L"extractDomRng");
  return Tuple(2); // dummy
}

// VerifyRng
// tp : REP`TypeRep
// -> bool
bool StatSem::VerifyRng(const TYPE_REP_TypeRep & tp)
{
  switch(tp.GetTag()) {
    case TAG_TYPE_REP_ProductTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & tps (tp.GetSequence(pos_REP_ProductTypeRep_tps));
      bool forall = true;
      size_t len_tps = tps.Length();
      for (size_t idx = 1; (idx <= len_tps) && forall; idx++) {
        const TYPE_REP_TypeRep & t (tps[idx]);
        forall = (t.Is(TAG_TYPE_REP_NumericTypeRep) && ((t.GetInt(pos_REP_NumericTypeRep_qtp) == Int(NAT))));
      }
      return forall;
    }
    case TAG_TYPE_REP_NumericTypeRep: {
      return (tp.GetInt(pos_REP_NumericTypeRep_qtp) == Int(NAT));
    }
// for curry
    case TAG_TYPE_REP_PartialFnTypeRep: {
      return VerifyRng(tp.GetRecord(pos_AS_PartialFnType_fnrng));
    }
    case TAG_TYPE_REP_TotalFnTypeRep: {
      return VerifyRng(tp.GetRecord(pos_AS_TotalFnType_fnrng));
    }
    default: {
      return false;
    }
  }
}

// VerifyMutRec
// mutRec : set of AS`Name
// measdef : AS`FnDef
// nmq : AS`Name
// ==> bool
bool StatSem::VerifyMutRec(const SET<TYPE_AS_Name> & mutRec,
                                const TYPE_AS_FnDef & measdef,
                                const TYPE_AS_Name & nmq)
{
  bool res = true;
  SEQ<TYPE_AS_Name> mutRecL (mutRec.ToSequence());

  size_t len_mutRecL = mutRecL.Length();
//  while(!mutRecL.IsEmpty())
  for (size_t idx = 1; idx <= len_mutRecL; idx++) {
//    TYPE_AS_Name h (mutRecL.Hd());
//    Generic fndef (GetFuncDefWOCtxt(h));
    Generic fndef (GetFuncDefWOCtxt(mutRecL[idx]));
    if (!fndef.IsNil()) {
      Generic fnmeasu_ (ASTAUX::GetFnMeasu(fndef));
      if (fnmeasu_.IsNil()) {
        //----------------------------------------------------
        // Error message #415
        // "%1" is mutual recursive with %2 and %2 does not have measure defined
        //----------------------------------------------------
        //GenErr(nmq, WRN1, 415, mk_sequence(PrintName(nmq), PrintName(h)));
        GenErr(nmq, WRN1, 415, mk_sequence(PrintName(nmq), PrintName(mutRecL[idx])));
        res = false;
      }
      else {
        TYPE_AS_Name fnmeasu (fnmeasu_);
        //TYPE_AS_Name measunmwcl (ExtName(h, fnmeasu));
        TYPE_AS_Name measunmwcl (ExtName(mutRecL[idx], fnmeasu));
        Generic othmeasdef_ (GetFuncDefWOCtxt(measunmwcl));
        if (!othmeasdef_.IsNil()) {
          TYPE_AS_FnDef othmeasdef (othmeasdef_);
          if (ASTAUX::GetFnRestype(othmeasdef) != ASTAUX::GetFnRestype(measdef)) {
            //----------------------------------------------------
            // Error message #416
            // The measures of "%1" and "%2" must to have the same range
            //----------------------------------------------------
            //GenErr(nmq, WRN1, 416, mk_sequence(PrintName(nmq), PrintName(h)));
            GenErr(nmq, WRN1, 416, mk_sequence(PrintName(nmq), PrintName(mutRecL[idx])));
            res = false;
          }
        }
        else {
          // this happens if the measure is unexisting identifier so it is reported there.
          res = false;
        }
      }
    }
    else {
      res = false;
    }
  }
  return res;
}
  
// NameInSet
// n : AS`Name
// sn : set of AS`Name
// -> bool
bool StatSem::NameInSet(const TYPE_AS_Name & n, const SET<TYPE_AS_Name> & sn)
{
  return sn.InSet(n);
}

// wf_Measure
// i : TYPE`Ind
// nmq : AS`Name
// measu : [AS`Name]
// fndom  : seq of REP`TypeRep
// tp : AS`FnType
// ==> bool
bool StatSem::wf_Measure(const Int & i,
                         const TYPE_AS_Name & nmq,
                         const Generic & measu,
                         const SEQ<TYPE_REP_TypeRep> & fndom,
                         const TYPE_AS_FnType & tp)
{
  bool ok_out = true;
//  Set tpset;
//  Sequence tpdomrng;
  Map recMap (getRecMap());
#ifdef VDMSL
  TYPE_AS_Name clnm (GetCurMod());
#endif // VDMSL
#ifdef VDMPP
  TYPE_AS_Name clnm (GetCurClass());
#endif // VDMPP
  TYPE_AS_Name nm (ExtName(clnm, nmq));

  if (NameInSet(nm, recMap.Dom())) {
    if (measu.IsNil()) {
      if (Settings.ErrorLevel() >= PRF) {
        //----------------------------------------------------
        // Error message #412
        // "%1" is recursive but does not have a measure defined
        //----------------------------------------------------
        GenErr(nm, WRN1, 412, mk_sequence(PrintName(nmq)));
      }
      ok_out = false;
    }
    else if (measu == Int(NOTYETSPEC)) {
      return true;
    }
    else {
      Tuple infer (wf_Expr(i, measu, rep_alltp));
      const Bool & ok_id (infer.GetBool(1));

      if (!ok_id) {
        return ok_out && ok_id; // false 
      }
      else {
        const TYPE_REP_TypeRep & mtp (infer.GetRecord(2));
        switch (mtp.GetTag()) {
          case TAG_TYPE_REP_NumericTypeRep:
          case TAG_TYPE_REP_ProductTypeRep: {
            if (!VerifyRng(mtp)) {
              //----------------------------------------------------
              // Error message #414
              // "%1" measure range is not nat or a tuple of nat
              //----------------------------------------------------
              GenErr(measu, WRN1, 414, mk_sequence(PrintName(nmq)));
              ok_out = false;
            }
            break; 
          }
          case TAG_TYPE_REP_TotalFnTypeRep:
          case TAG_TYPE_REP_PartialFnTypeRep:
          case TAG_TYPE_REP_PolyTypeRep: {
            TYPE_AS_Name measuq (ExtName(clnm, measu));
            Generic measdef_ (GetFuncDefWOCtxt(measuq));
            if (!measdef_.IsNil()) {
              TYPE_AS_FnDef measdef (measdef_);
              SEQ<TYPE_REP_TypeRep> rfndt (TransTypeList(Nil(), tp.GetSequence(1)));
              TYPE_REP_TypeRep rfnrt (TransType(Nil(), tp.GetRecord(2)));
              SEQ<TYPE_REP_TypeRep> mfndt (TransTypeList(Nil(), ASTAUX::GetFnParms(measdef)));
              TYPE_REP_TypeRep mfnrt (TransType(Nil(), ASTAUX::GetFnRestype(measdef)));

              if (!EquivDomFn(i, mfndt, rfndt, mfnrt, rfnrt)) {
                //----------------------------------------------------
                // Error message #413
                // "%1" and its measure do not have the same domain
                //----------------------------------------------------
                GenErr(nm, WRN1, 413, mk_sequence(PrintName(nmq)));
                ok_out = false;
              }

              if (mfnrt.Is(TAG_TYPE_REP_PartialFnTypeRep) || mfnrt.Is(TAG_TYPE_REP_TotalFnTypeRep)) {
                //----------------------------------------------------
                // Error message #449
                // measure "%1" must not be curry function
                //----------------------------------------------------
                GenErr(measu, ERR, 449, mk_sequence(PrintName(measu)));
                ok_out = false;
              }
              if (!VerifyRng(mfnrt)) {
                //----------------------------------------------------
                // Error message #414
                // "%1" measure range is not nat or a tuple of nat
                //----------------------------------------------------
                GenErr(nm, WRN1, 414, mk_sequence(PrintName(nmq)));
                ok_out = false;
              }

              TYPE_SSENV_FunctionInfo fi (recMap[nm]);
              Set mutRec (fi.GetSet(pos_SSENV_FunctionInfo_rec));
              if (mutRec.InSet(nm)) {
                mutRec.RemElem(nm);
              }
              if (!mutRec.IsEmpty()) {
                ok_out = VerifyMutRec(mutRec, measdef, nmq) && ok_out;
              }
            }
            else {
              ok_out = false;
            }
            break;
          }
#ifdef VDMPP
          case TAG_TYPE_REP_OverTypeRep: {
            SET<TYPE_REP_TypeRep> tps (mtp.GetSet(pos_REP_OverTypeRep_tps));
            SET<TYPE_REP_TypeRep> metps;
            SEQ<TYPE_REP_TypeRep> rfndt (TransTypeList(Nil(), tp.GetSequence(1)));
            TYPE_REP_TypeRep rfnrt (TransType(Nil(), tp.GetRecord(2)));
            Generic t;
            for (bool bb = tps.First(t); bb; bb = tps.Next(t)) {
              SEQ<TYPE_REP_TypeRep> mfndt (Record(t).GetSequence(1));
              TYPE_REP_TypeRep mfnrt (Record(t).GetRecord(2));
              if (EquivDomFn(i, mfndt, rfndt, mfnrt, rfnrt)) {
                metps.Insert(t);
              }
            }
            if (metps.Card() == 1) {
              TYPE_REP_TypeRep tpr (metps.GetElem());
              TYPE_REP_TypeRep mfnrt (tpr.GetRecord(2));
              if (mfnrt.Is(TAG_TYPE_REP_PartialFnTypeRep) || mfnrt.Is(TAG_TYPE_REP_TotalFnTypeRep)) {
                //----------------------------------------------------
                // Error message #449
                // measure "%1" must not be curry function
                //----------------------------------------------------
                GenErr(measu, ERR, 449, mk_sequence(PrintName(measu)));
                ok_out = false;
              }
              if (!VerifyRng(mfnrt)) {
                //----------------------------------------------------
                // Error message #414
                // "%1" measure range is not nat or a tuple of nat
                //----------------------------------------------------
                GenErr(nm, WRN1, 414, mk_sequence(PrintName(nmq)));
                ok_out = false;
              }
            }
            else {
              //----------------------------------------------------
              // Error message #413
              // "%1" and its measure do not have the same domain
              //----------------------------------------------------
              GenErr(nm, WRN1, 413, mk_sequence(PrintName(nmq)));
              ok_out = false;
            }
            break;
          }
#endif // VDMPP
          default: {
            //----------------------------------------------------
            // Error message #414
            // "%1" measure range is not nat or a tuple of nat
            //----------------------------------------------------
            GenErr(measu, WRN1, 414, mk_sequence(PrintName(nmq)));
            return false;
          }
        } // 1219 switch
      }
    }
    if (ok_out) {
      TYPE_SSENV_FunctionInfo fi (recMap[nm]);
      fi.SetField(pos_SSENV_FunctionInfo_printPO,Bool(true));
      recMap.ImpModify(nm, fi);
      setRecMap(recMap);
    }
  }
  else {
    if (!measu.IsNil()) {
      //----------------------------------------------------
      // Error message #417
      // "%1" has a measure but it is not recursive
      //----------------------------------------------------
      GenErr(nmq, WRN1, 417, mk_sequence(PrintName(nmq)));
    }
  }
  return ok_out;
}
 
// 20130110 -->
// EquivDomFn
// i : TYPE`Ind
// fndom1 : seq of REP`TypeRep
// fndom2 : seq of REP`TypeRep
// fnrng1 : REP`TypeRep
// fnrng2 : REP`TypeRep
// +> bool
bool StatSem::EquivDomFn(const Int & i, const SEQ<TYPE_REP_TypeRep> & mfndom, const SEQ<TYPE_REP_TypeRep> & rfndom,
                                             const TYPE_REP_TypeRep & mfnrng, const TYPE_REP_TypeRep & rfnrng)
{
//  bool res = false;
  if (mfndom == rfndom) {
    //res = true;
    return true;
  }

  SEQ<TYPE_REP_TypeRep> mdom (mfndom);
  TYPE_REP_TypeRep mrng (mfnrng);
//  while ((mrng.Is(TAG_TYPE_REP_PartialFnTypeRep) || mrng.Is(TAG_TYPE_REP_TotalFnTypeRep)))
//  {
//    mdom.ImpConc(mrng.GetSequence(1));
//    mrng = mrng.GetRecord(2);
//  }
  SEQ<TYPE_REP_TypeRep> rdom (rfndom);
  TYPE_REP_TypeRep rrng (rfnrng);
  while ((rrng.Is(TAG_TYPE_REP_PartialFnTypeRep) || rrng.Is(TAG_TYPE_REP_TotalFnTypeRep))) {
    rdom.ImpConc(rrng.GetSequence(1));
    rrng = rrng.GetRecord(2);
  }
  bool forall = (mdom.Length() <= rdom.Length());
  this->MeasureCheckOn();
  size_t len = mdom.Length();
  for (size_t idx = 1; (idx <= len) && forall; idx++) {
    forall = IsEquivalent(i, mdom[idx], rdom[idx]);
  }
  this->MeasureCheckOff();
  return forall;

/*
  if (mfndom.Length() == rfndom.Length())
  {
    bool forall = true;
    this->MeasureCheckOn();
    size_t len = mfndom.Length();
    for (size_t idx = 1; (idx <= len) && forall; idx++)
      forall = IsEquivalent(i, mfndom[idx], rfndom[idx]);
    this->MeasureCheckOff();
    res = forall;
  }
// 20130703 -->
  else if ((rfnrng.Is(TAG_TYPE_REP_PartialFnTypeRep) || rfnrng.Is(TAG_TYPE_REP_TotalFnTypeRep)))
  {
    SEQ<TYPE_REP_TypeRep> mdom (mfndom);
    TYPE_REP_TypeRep mrng (mfnrng);
//    while ((mrng.Is(TAG_TYPE_REP_PartialFnTypeRep) || mrng.Is(TAG_TYPE_REP_TotalFnTypeRep)))
//    {
//      mdom.ImpConc(mrng.GetSequence(1));
//      mrng = mrng.GetRecord(2);
//    }
    SEQ<TYPE_REP_TypeRep> rdom (rfndom);
    TYPE_REP_TypeRep rrng (rfnrng);
    while ((rrng.Is(TAG_TYPE_REP_PartialFnTypeRep) || rrng.Is(TAG_TYPE_REP_TotalFnTypeRep)))
    {
      rdom.ImpConc(rrng.GetSequence(1));
      rrng = rrng.GetRecord(2);
    }
    bool forall = (mdom.Length() <= rdom.Length());
    this->MeasureCheckOn();
    size_t len = mdom.Length();
    for (size_t idx = 1; (idx <= len) && forall; idx++)
      forall = IsEquivalent(i, mdom[idx], rdom[idx]);
    this->MeasureCheckOff();
    res = forall;
  }
// <-- 20130703
//  if (res)
//  {
//    // check for curried function
//    if ((mfnrng.Is(TAG_TYPE_REP_PartialFnTypeRep) || mfnrng.Is(TAG_TYPE_REP_TotalFnTypeRep)) &&
//        (rfnrng.Is(TAG_TYPE_REP_PartialFnTypeRep) || rfnrng.Is(TAG_TYPE_REP_TotalFnTypeRep)))
//    {
//      res = EquivDomFn(i, mfnrng.GetSequence(1), rfnrng.GetSequence(1), mfnrng.GetRecord(2), rfnrng.GetRecord(2)); 
//    }
//    else if (!(mfnrng.Is(TAG_TYPE_REP_PartialFnTypeRep) || mfnrng.Is(TAG_TYPE_REP_TotalFnTypeRep)) &&
//             !(rfnrng.Is(TAG_TYPE_REP_PartialFnTypeRep) || rfnrng.Is(TAG_TYPE_REP_TotalFnTypeRep)))
//    {
//    }
//    else
//      res = false;
//  }
//  return res;
*/
}
// <-- 20130110

// CheckAllType
// tp : AS`Type
// +> bool
bool StatSem::CheckAllType(const TYPE_AS_Type & tp)
{
//  if (tp_.IsNil())
//    return false;
//
//  TYPE_AS_Type tp (tp_);
  switch(tp.GetTag()) {
    case TAG_TYPE_AS_AllType: {
      return true;
    }
    case TAG_TYPE_AS_BooleanType:
    case TAG_TYPE_AS_NumericType:
    case TAG_TYPE_AS_TokenType:
    case TAG_TYPE_AS_CharType:
    case TAG_TYPE_AS_VoidType:
    case TAG_TYPE_AS_QuoteType:
    case TAG_TYPE_AS_TypeName:
    case TAG_TYPE_AS_TypeVar: {
      return false;
    }
    case TAG_TYPE_AS_CompositeType: {
      const SEQ<TYPE_AS_Field> & flds (tp.GetSequence(pos_AS_CompositeType_fields));
      bool exists = false;
      size_t len_flds = flds.Length();
      for(size_t idx = 1; (idx <= len_flds) && !exists; idx++) {
        exists = CheckAllType(flds[idx].GetRecord(pos_AS_Field_type));
      }
      return exists;
    }
    case TAG_TYPE_AS_UnionType: {
      const SEQ<TYPE_AS_Type> & tps (tp.GetSequence(pos_AS_UnionType_tps));
      size_t len_tps = tps.Length();
      bool exists = false;
      for (size_t idx = 1; (idx <= len_tps) && !exists; idx++) {
        exists = CheckAllType(tps[idx]);
      }
      return exists;
    }
    case TAG_TYPE_AS_ProductType: {
      const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence(pos_AS_ProductType_tps));
      bool exists = false;
      size_t len_tp_l = tp_l.Length();
      for (size_t idx = 1; (idx <= len_tp_l) && !exists; idx++) {
        exists = CheckAllType(tp_l[idx]);
      }
      return exists;
    }
    case TAG_TYPE_AS_OptionalType: {
      return CheckAllType(tp.GetRecord(pos_AS_OptionalType_tp));
    }
    case TAG_TYPE_AS_BracketedType: {
      return CheckAllType(tp.GetRecord(pos_AS_BracketedType_tp));
    }
    case TAG_TYPE_AS_Set0Type: {
      return CheckAllType(tp.GetRecord(pos_AS_Set0Type_elemtp));
    }
    case TAG_TYPE_AS_Set1Type: {
      return CheckAllType(tp.GetRecord(pos_AS_Set1Type_elemtp));
    }
    case TAG_TYPE_AS_Seq0Type: {
      return CheckAllType(tp.GetRecord(pos_AS_Seq0Type_elemtp));
    }
    case TAG_TYPE_AS_Seq1Type: {
      return CheckAllType(tp.GetRecord(pos_AS_Seq1Type_elemtp));
    }
    case TAG_TYPE_AS_GeneralMap0Type: {
      return CheckAllType(tp.GetRecord(pos_AS_GeneralMap0Type_mapdom)) &&
             CheckAllType(tp.GetRecord(pos_AS_GeneralMap0Type_maprng));
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      return CheckAllType(tp.GetRecord(pos_AS_GeneralMap1Type_mapdom)) &&
             CheckAllType(tp.GetRecord(pos_AS_GeneralMap1Type_maprng));
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      return CheckAllType(tp.GetRecord(pos_AS_InjectiveMap0Type_mapdom)) &&
             CheckAllType(tp.GetRecord(pos_AS_InjectiveMap0Type_maprng));
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      return CheckAllType(tp.GetRecord(pos_AS_InjectiveMap1Type_mapdom)) &&
             CheckAllType(tp.GetRecord(pos_AS_InjectiveMap1Type_maprng));
    }
    case TAG_TYPE_AS_PartialFnType: {
      const SEQ<TYPE_AS_Type> & fndom (tp.GetSequence(pos_AS_PartialFnType_fndom));
      bool exists (CheckAllType(tp.GetRecord(pos_AS_PartialFnType_fnrng)));
      size_t len_fndom = fndom.Length();
      for (size_t idx = 1; (idx <= len_fndom) && !exists; idx++) {
        exists = CheckAllType(fndom[idx]);
      }
      return exists;
    }
    case TAG_TYPE_AS_TotalFnType: {
      const SEQ<TYPE_AS_Type> & fndom (tp.GetSequence(pos_AS_TotalFnType_fndom));
      bool exists (CheckAllType(tp.GetRecord(pos_AS_TotalFnType_fnrng)));
      size_t len_fndom = fndom.Length();
      for (size_t idx = 1; (idx <= len_fndom) && !exists; idx++) {
        exists = CheckAllType(fndom[idx]);
      }
      return exists;
    }
    case TAG_TYPE_AS_OpType: {
      const SEQ<TYPE_AS_Type> & opdom (tp.GetSequence(pos_AS_OpType_opdom));
      bool exists (CheckAllType(tp.GetRecord(pos_AS_OpType_oprng)));
      size_t len_opdom = opdom.Length();
      for (size_t idx = 1; (idx <= len_opdom) && !exists; idx++) {
        exists = CheckAllType(opdom[idx]);
      }
      return exists;
    }
    default: {
      return false;
    }
  }
}

#endif //! NOSS
