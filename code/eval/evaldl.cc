/***
*  * WHAT
*  *    DL Module functions.
*  * FILE
*  *    $Id: evaldl.cc,v 1.68 2006/09/22 11:14:03 vdmtools Exp $
*  * PROJECT
*  *    VDM-SL Toolbox
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "metaiv.h"
#include "AS.h"
#include "stackeval.h"
#include "dlfactory.h"
#include "libman.h"
#include "astaux.h"
#include "rt_info.h"
#include "position.h"
#include "evaldl.h"
#include "rt_errmsg.h"
#include <math.h>
#include "compile.h"
#include "RTERR.h"
#include "tbutils.h"
#include "evalaux.h"

#ifdef VDMPP
// GetDLObjectFromObjref
// obj_rf : SEM`OBJ_Ref
// ==> [DLObject]
DLObject EvalState::GetDLObjectFromSemObjRef(const TYPE_SEM_OBJ_uRef & obj_ref)
{
  if (this->obj_tab.DomExists(obj_ref)) {
    const TYPE_GLOBAL_OBJ_uDesc & desc (this->obj_tab[obj_ref]);
    const Generic & dcip (desc.GetField(pos_GLOBAL_OBJ_uDesc_DlClassInstancePtr));
    if (!dcip.IsNil()) {
      const TYPE_SEM_OBJ & sem (desc.GetRecord(pos_GLOBAL_OBJ_uDesc_sem));
      return DlClassFactory::CreateDLObject(sem.get_tp(), dcip);
    }
  }
  RTERR::Error(L"GetDLObjectFromSemObjRef", RTERR_UNKNOWN_OBJECT, Nil(), Nil(), Sequence());
  return Generic(); // dummy
}

// CreateNEWFromDLObject
// obj : DLObject
// ==> SEM`OBJ_Ref
TYPE_SEM_OBJ_uRef EvalState::CreateNEWFromDLObject(const DLObject & obj)
{
  wstring n (obj.GetName());
  TYPE_AS_Name name (ASTAUX::MkNameFromId(ASTAUX::MkId(n), NilContextId));

  if (! IsDLClass(name)) {
    RTERR::Error(L"CreateNEWFromDLObject",
                 RTERR_CLASS_NOT_DLCLASS,
                 M42Sem(AUX::SingleNameToString(name), NULL),
                 Nil(), Sequence());
  }

  // If the objects DlClass pointer is known we just that object.
  Set dom_obj_tab (this->obj_tab.Dom());
  Generic ref;
  for (bool bb = dom_obj_tab.First(ref); bb; bb = dom_obj_tab.Next(ref)) {
    if (DlClassFactory::DLClassEqual(this->obj_tab[ref].GetField(pos_GLOBAL_OBJ_uDesc_DlClassInstancePtr), obj)) {
      return ref;
    }
  }

  // Otherwise we need to create a new objects with that external DlClass associated.
  // Create a NewExpr and compile and execute it.
  TYPE_AS_NewExpr nexpr;
  nexpr.Init(name, SEQ<TYPE_AS_Expr>(), NilContextId);
  TYPE_STKM_SubProgram prog (theCompiler().CompileNewExpr(nexpr, obj));
  Tuple res (theStackMachine().EvalAuxProgram(prog, Sequence(L"new"), false));
  const TYPE_SEM_OBJ_uRef & tmp_ref (res.GetField(2));
  return tmp_ref;
}
#endif //VDMPP

void EvalState::dlclose()
{
#ifdef VDMPP
  Set dom_obj_tab (this->obj_tab.Dom());
  Generic ref;
  for (bool bb = dom_obj_tab.First(ref); bb; bb = dom_obj_tab.Next(ref)) {
    DlClassFactory::DLClose(this->obj_tab[ref].GetField(pos_GLOBAL_OBJ_uDesc_DlClassInstancePtr));
  }
#endif // VDMPP
  this->dlfactory.DLClose();
}

#ifdef VDMSL
// Corresponds to IsExtFnOp in eval_expr.vdm
Tuple EvalState::IsDLFnOP(const TYPE_AS_Name & id, const TYPE_AS_Name & mod_nm)
{
  TYPE_GLOBAL_SigmaIMO sigmaimo (GetDLModule(mod_nm));
  const Map & extfns (sigmaimo.GetMap(pos_GLOBAL_SigmaIMO_extfns));
  const Map & extops (sigmaimo.GetMap(pos_GLOBAL_SigmaIMO_extops));

  if (extfns.DomExists(id)) {
    return mk_(Bool(true), extfns[id]);
  }
  else if (extops.DomExists(id)) {
    return mk_(Bool(true), extops[id]);
  }
  else {
    return mk_(Bool(false), Nil());
  }
}

// Replaced by IsExtFnOP: Record GetExtFnOpVal(Record id)
// Corresponds to IsExtVal in eval_expr.vdm
Tuple EvalState::IsDLVal(const TYPE_AS_Name & id, const TYPE_AS_Name & mod_nm)
{
  TYPE_GLOBAL_SigmaIMO sigmaimo (GetDLModule(mod_nm));
  const Map & extv (sigmaimo.GetMap(pos_GLOBAL_SigmaIMO_extv));

  if (extv.DomExists(id)) {
    TYPE_SEM_VAL extval (this->dlfactory.CallOp(mod_nm, id, true, Nil()));
    return mk_(Bool(true), extval);
  }
  else {
    return mk_(Bool(false), Nil());
  }
}
#endif // VDMSL

// This function converts from semanic values to metaIV values
Sequence EvalState::Sem2M4(const SEQ<TYPE_SEM_VAL> & arg_l, DLRecInfoData * dldata)
{
  Sequence res;
  size_t len_arg_l = arg_l.Length();
  for (size_t idx = 1; idx <= len_arg_l; idx++) {
    TYPE_SEM_VAL val (arg_l[idx]);
    res.ImpAppend(Convert2M4(val, dldata));
  }
  return res;
}

// This function converts from metaIV values to sematic values
TYPE_SEM_VAL EvalState::M42Sem(const Generic & val, DLRecInfoData * dldata)
{
  // For the basic types, we take a GetValue in order to get an
  // explicit copy. The copy is needed in order to avoid references
  // crossing the DL boundary. Otherwise we can have dangeling pointers
  // when the DL module is closed (ex. last_result).

  if (val.IsInt()) {
    /************************* INT **************************/
    // Force copy
    Real r (Int(val).GetValue()); // TODO: 20100302
    return TYPE_SEM_NUM().Init(r);
  }
  if (val.IsReal()) {
    /************************* REAL **************************/
    // Force copy
    Real r (Real(val).GetValue());
    return TYPE_SEM_NUM().Init(r);
  }
  else if (val.IsSet()) {
    /************************* SET *******************************/
    Set val_s (val);
    SET<TYPE_SEM_VAL> set;
    Generic elm;
    for (bool bb = val_s.First(elm); bb; bb = val_s.Next(elm)) {
      set.Insert(M42Sem(elm,dldata));
    }
    return TYPE_SEM_SET().Init(set);
  }
  else if (val.IsRecord()) {
    /************************* RECORD ****************************/
    Record valr (val);
    int tag = valr.GetTag();
    size_t size = valr.Length();
    const wchar_t *p = NULL;
// for internal
    wstring tmptag;
    if( dldata == NULL ) {
      if (VDMGetDefaultRecInfoMap().GetSymTag(tag, tmptag)) {
        // Copy internal RecInfo to user RecInfo
        TYPE_AS_Name symtag (ASTAUX::MkName(tmptag));
        if( !SemRec::IsDecl(symtag) ) {
          SEQ<Bool> dc_s;
          for (size_t i = 1; i <= size; i++ ) {
            Bool dc (VDMGetDefaultRecInfoMap().AskDontCare(tag, i));
            dc_s.ImpAppend(dc);
          }
          SemRec::Decl_SEM_REC(symtag, size, dc_s);
        }
        p = tmptag.c_str();
      }
    }
    else {
      p = (*dldata->functions.vdmdlgetsymtag)(dldata->namemap, tag);
    }
     
    if (p) {
      TYPE_AS_Name symtag (ASTAUX::MkName(p));
      SEQ<TYPE_SEM_VAL> resseq;
      size_t len_valr = valr.Length();
      for (size_t i = 1; i <= len_valr; i++) {
        resseq.ImpAppend(M42Sem(valr.GetField(i), dldata));
      }
      if( !SemRec::IsDecl(symtag) ) {
        vdm_iplog << L"Unknown tag: " << tag << L"," << size << L","
                  << ASTAUX::ASName2String(symtag) << endl << flush;
        RTERR::Error(L"M42Sem", RTERR_TAG_UNKNOWN, Nil(), Nil(), Sequence());
      }
      return SemRec::mk_SEM_REC(symtag, resseq);
    } else {
      RTERR::Error(L"M42Sem", RTERR_TAG_UNKNOWN, Nil(), Nil(), Sequence());
      return Nil();
    }
  }
#ifdef VDMPP
  else if (val.IsDLObject()) {
    return theState().CreateNEWFromDLObject(val);
  }
#endif //VDMPP

  else if (val.IsTuple()) {
    /************************* TUPLE *****************************/
    Tuple tuple (val);
    SEQ<TYPE_SEM_VAL> resseq;
    size_t len_tuple = tuple.Length();
    for (size_t idx = 1; idx <= len_tuple; idx++)
      resseq.ImpAppend(M42Sem(tuple.GetField(idx), dldata));
    return TYPE_SEM_TUPLE().Init(resseq);
  }
  else if (val.IsNil()) {
    /************************* NIL *******************************/
    return TYPE_SEM_NIL();
  }
  else if (val.IsChar()) {
    /************************* CHAR ******************************/
    // Force copy
    wchar_t c (Char(val).GetValue ());
    return TYPE_SEM_CHAR().Init(Char(c));
  }
  else if (val.IsMap()) {
    /************************* MAP *******************************/
    Map mapV (val);
    Set dom_mapV (mapV.Dom());
    MAP<TYPE_SEM_VAL,TYPE_SEM_VAL> resmap;
    Generic key;
    for (bool bb = dom_mapV.First(key); bb; bb = dom_mapV.Next(key)) {
      TYPE_SEM_VAL newkey (M42Sem(key, dldata));
      TYPE_SEM_VAL newval (M42Sem(mapV[key], dldata));
      resmap.Insert(newkey,newval);
    }
    return TYPE_SEM_MAP().Init(resmap);
  }
  else if (val.IsSequence()) {
    /************************* SEQUENCE **************************/
    Sequence seq (val);
    size_t len_seq = seq.Length();
    SEQ<TYPE_SEM_VAL> resseq;
    for (size_t idx = 1; idx <= len_seq; idx++)
      resseq.ImpAppend(M42Sem(seq[idx], dldata));
    return TYPE_SEM_SEQ().Init(resseq);
  }
  else if (val.IsUndef()) {
    /************************* UNDEF *****************************/
    return TYPE_SEM_UNDEF();
  }
  else if (val.IsBool()) {
    /************************* BOOL ******************************/
    // Force copy
    return (Bool(val).GetValue () ? sem_true : sem_false);
  }
  else if (val.IsQuote()) {
    /************************* QUOTE *****************************/
    wstring quote (Quote(val).GetValue());
    return TYPE_SEM_QUOTE().Init(SEQ<Char>(quote));
  }
  else if (val.IsText()) {
    /************************* TEXT ******************************/
    wstring text (Text(val).GetValue());
    SEQ<Char> c_l (text);

    size_t len_c_l = c_l.Length();
    SEQ<TYPE_SEM_VAL> chars;
    for (size_t i = 1; i <= len_c_l; i++)
      chars.ImpAppend(mk_SEM_CHAR(c_l[i]));

    return TYPE_SEM_SEQ().Init(chars);
  }
  else if (val.IsToken()) {
    return TYPE_SEM_TOKEN().Init(M42Sem(Token(val).GetValue(), dldata));
  }
  else {
    /************************* CLASS *****************************/
    RTERR::Error(L"M42Sem", RTERR_TYPE_NOT_SUPPORTED, Nil(), Nil(), Sequence());
    return TYPE_SEM_VAL();
  }
}

// This function convert a single sematic value to a metaIV value
Generic EvalState::Convert2M4(const TYPE_SEM_VAL & arg, DLRecInfoData * dldata)
{
  switch (arg.GetTag()) {
    case TAG_TYPE_SEM_NUM: {
      /************************* NUM *******************/
      return arg.GetReal(pos_SEM_NUM_v);
    }
    case TAG_TYPE_SEM_BOOL: {
      /************************* BOOL *******************/
      return arg.GetBool(pos_SEM_BOOL_v);
    }
    case TAG_TYPE_SEM_CHAR: {
      /************************* CHAR *******************/
      return arg.GetChar(pos_SEM_CHAR_v);
    }
    case TAG_TYPE_SEM_SET: {
      /************************* SET *******************************/
      Set set (arg.GetSet(pos_SEM_SET_v));
      Set res;
      Generic elm;
      for (bool bb = set.First(elm); bb; bb = set.Next(elm))
        res.Insert(Convert2M4(elm, dldata));
      return res;
    }
    case TAG_TYPE_DYNSEM_SEM_REC: {
      /************************* RECORD ***************************/
      const TYPE_AS_Name & name (arg.GetRecord(pos_DYNSEM_SEM_SemRecord_tag));
      SEQ<TYPE_SEM_VAL> seq (arg.GetRecord(pos_DYNSEM_SEM_SemRecord_value).GetFields());
      wstring tagwstring (ASTAUX::ASName2String(name));
      int tag_i, size;
      if (! (*dldata->functions.vdmdlgettag)(dldata->namemap, tagwstring.c_str(), &tag_i, &size)) {
        RTERR::Error(L"Convert2M4", RTERR_TAG_NOT_IN_NAME_MAP, Nil(), Nil(), Sequence());
        //FIXME vdm_log << ASTAUX::ASName2String(name) << endl << flush;
      }
      Record res (tag_i, seq.Length(), *dldata->namemap);
      size_t len_seq = seq.Length();
      for (size_t idx = 1; idx <= len_seq; idx++) {
        Generic val(Convert2M4(seq[idx], dldata));
        res.SetField(idx, val);
      }
      return res;
    }
#ifdef VDMPP
    case TAG_TYPE_SEM_OBJ_uRef: {
      DLObject obj = theState().GetDLObjectFromSemObjRef(arg);
      return obj;
    }
#endif //VDMPP
  
    case TAG_TYPE_SEM_NIL: {
      /************************* NIL *******************************/
      return Nil();
    } 
    case TAG_TYPE_SEM_SEQ: {
      /************************* SEQUENCE **************************/
      const Sequence & seq (arg.GetSequence(pos_SEM_SEQ_v));
      size_t len = seq.Length();
      Sequence res;
      for (size_t idx = 1; idx <= len; idx++)
        res.ImpAppend(Convert2M4(seq[idx], dldata));
      return res;
    }
    case TAG_TYPE_SEM_MAP : {
      /************************* MAP *******************************/
      const Map & mapV (arg.GetMap(pos_SEM_MAP_v));
      Set dom_mapV (mapV.Dom());
      Map res;
      Generic key;
      for (bool bb = dom_mapV.First(key); bb; bb = dom_mapV.Next(key)) {
        Generic newkey (Convert2M4(key, dldata));
        Generic newval (Convert2M4(mapV[key], dldata));
        res.Insert(newkey, newval);
      }
      return res;
    }
    case TAG_TYPE_SEM_TUPLE: {
      /************************* TUPLE *****************************/
      const SEQ<TYPE_SEM_VAL> & seq (arg.GetSequence(pos_SEM_TUPLE_v));
      Tuple res (seq.Length());
      size_t len_seq = seq.Length();
      for (size_t i = 1; i <= len_seq; i++) {
        res.SetField(i, Convert2M4(seq[i], dldata));
      }
      return res;
    }
    case TAG_TYPE_SEM_QUOTE: {
      /************************* QUOTE *****************************/
      const type_cL & tk (arg.GetSequence(pos_SEM_QUOTE_v));
      wstring tkTxt;
      tk.GetString(tkTxt);
      Quote res (tkTxt);
      return res;
    }
    case TAG_TYPE_SEM_TOKEN: {
      /************************* TOKEN *****************************/
      const TYPE_SEM_VAL & svt (arg.GetRecord(pos_SEM_TOKEN_v));
      Generic seqv (Convert2M4(svt, dldata));
      return mk_token(seqv);
    }
    case TAG_TYPE_SEM_UNDEF:
      /************************* UNDEF *****************************/
    default: {
      RTERR::Error(L"Convert2M4", RTERR_TYPE_NOT_SUPPORTED, Nil(), Nil(), Sequence());
    }
  }
  return Nil(); // dummy
}
