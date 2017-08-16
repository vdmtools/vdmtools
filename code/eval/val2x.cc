/***
*  * WHAT
*  *    VDM semantic value handling. Old code from metaiv.cc
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/eval/val2x.cc,v $
*  * VERSION
*  *    $Revision: 1.85 $
*  * DATE
*  *    $Date: 2006/10/14 02:29:32 $
*  * STATUS
*  *    $State: Exp $
*  * PROJECT
*  *    Afrodite - ESPRIT III programme, project no. 6500.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "val2x.h"
#include "tbutils.h"
#include "rt_errmsg.h"
#include "RTERR.h"
#include "stackeval.h"
#include "settings.h"
#include "astaux.h"
#include "pog-interface.h"
#include "tb_wstring.h"
#include "statsem.h"
#include "CGMAIN.h"
#include "CONC.h"
#ifdef VDMPP
#include "MANGLE.h"
#include "libman.h"
#endif // VDMPP

////////////////////////////////////////
/// Name: init_EVAL_SEM()
/// Descr: Initialises the the pretty printer functions
///        to the records of the the semantic values
///////////////////////////////////////


void VAL2X::my_space(wostream&os, int indent)
{
  for (int i=0; i<indent; i++)
    os << L" ";
}

void VAL2X::init_EVAL_SEM()
{
  // A semrec now has a second don't care field, see eval_sem.vdm
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_DYNSEM_SEM_REC, 3);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_DYNSEM_SEM_REC, L"SEM`REC");
  VDMGetDefaultRecInfoMap().SetDontCare(TAG_TYPE_DYNSEM_SEM_REC, 3);

#ifdef VDMPP
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_DYNSEM_SEM_OBJ_Ref_PP,3);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_DYNSEM_SEM_OBJ_Ref_PP, L"SEM`OBJ_REF_V_PP");
  VDMGetDefaultRecInfoMap().SetPrintFunction(TAG_TYPE_DYNSEM_SEM_OBJ_Ref_PP, print_obj_ref_v);
#endif //VDMPP
}

void VAL2X::init_STKM()
{
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_MatchVal, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_MatchVal, L"STKM`MatchVal");

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_PatternName, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_PatternName, L"STKM`PatternName");

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_RecordPattern, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_RecordPattern, L"STKM`RecordPattern");

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_SeqConcPattern, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_SeqConcPattern, L"STKM`SeqConcPattern");

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_SeqEnumPattern, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_SeqEnumPattern, L"STKM`SeqEnumPattern");

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_SetEnumPattern, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_SetEnumPattern, L"STKM`SetEnumPattern");

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_SetUnionPattern, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_SetUnionPattern, L"STKM`SetUnionPattern");

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_MapEnumPattern, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_MapEnumPattern, L"STKM`MapEnumPattern");

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_MapMergePattern, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_MapMergePattern, L"STKM`MapMergePattern");

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_MapletPattern, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_MapletPattern, L"STKM`MapletPattern");

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_TuplePattern, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_TuplePattern, L"STKM`TuplePattern");

#ifdef VDMPP
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_ObjectPattern, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_ObjectPattern, L"STKM`ObjectPattern");

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_FieldPattern, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_FieldPattern, L"STKM`FieldPattern");
#endif // VDMPP

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_Trap, 9);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_Trap, L"STKM`Trap");

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_FieldRef, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_FieldRef, L"STKM`FieldRef");

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_MapOrSeqRef, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_MapOrSeqRef, L"STKM`MapOrSeqRef");

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_Breakpoint, 0);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_Breakpoint, L"STKM`Breakpoint"); 

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_Interrupt, 0);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_Interrupt, L"STKM`Interrupt"); 

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_Success, 0);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_Success, L"STKM`Success"); 

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_Guard, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_Guard, L"STKM`Guard"); 

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_EndOfSlice, 0);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_EndOfSlice, L"STKM`EndOfSlice"); 

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_EndOfCPUSlice, 0);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_EndOfCPUSlice, L"STKM`EndOfCPUSlice"); 

#ifdef VDMSL
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_EvaluatorStatus, 17);
#endif // VDMSL
#ifdef VDMPP
#ifdef VICE
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_EvaluatorStatus, 21);
#else
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_EvaluatorStatus, 19);
#endif // VICE
#endif // VDMPP
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_EvaluatorStatus, L"STKM`EvaluatorStatus"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_Step, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_Step, L"STKM`Step"); 

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_StepIn, 0);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_StepIn, L"STKM`StepIn"); 

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_SingleStep, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_SingleStep, L"STKM`SingleStep"); 

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_Finish, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_Finish, L"STKM`Finish"); 

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_Continue, 0);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_Continue, L"STKM`Continue"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_CallStackItem, 13);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_CallStackItem, L"STKM`CallStackItem"); 

#ifdef VDMPP
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_ProcThread, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_ProcThread, L"STKM`ProcThread"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_PerThread, 5);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_PerThread, L"STKM`PerThread"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_SpoThread, 4);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_SpoThread, L"STKM`SpoThread"); 
#ifdef VICE
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_CPUSigma, 16);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_CPUSigma, L"STKM`CPUSigma"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_BUSSigma, 7);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_BUSSigma, L"STKM`BUSSigma"); 
#else
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_CPUSigma, 3);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_CPUSigma, L"STKM`CPUSigma"); 
#endif //VICE
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_Message, 5);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_Message, L"STKM`Message"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_MessageInfo, 5);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_MessageInfo, L"STKM`MessageInfo"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_MessageResponse, 4);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_MessageResponse, L"STKM`MessageResponse"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_MessageBroadcast, 5);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_MessageBroadcast, L"STKM`MessageBroadcast"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_MessageReq, 9);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_MessageReq, L"STKM`MessageReq"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_MessageReplyReq, 9);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_MessageReplyReq, L"STKM`MessageReplyReq"); 
#endif // VDMPP
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_STKM_DebugCmd, 1);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_STKM_DebugCmd, L"STKM`DebugCmd"); 
}

void VAL2X::init_STAT_SEM()
{
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_SSENV_TypeRepElem, 3);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_SSENV_TypeRepElem, L"ENV`TypeRepElem"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_SSENV_OpTypeRepElem, 4);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_SSENV_OpTypeRepElem, L"ENV`OpTypeRepElem"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_SSENV_ParseTypeInfo, 16);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_SSENV_ParseTypeInfo, L"ENV`ParseTypeInfo"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_SSENV_ExpSigRep, 7);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_SSENV_ExpSigRep, L"ENV`ExpSigRep"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_SSENV_PolyTypeRep, 3);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_SSENV_PolyTypeRep, L"ENV`PolyTypeRep"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_SSENV_PolyTypeRepElem, 3);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_SSENV_PolyTypeRepElem, L"ENV`PolyTypeRepElem"); 
#ifdef VDMPP
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_SSENV_AccessTypeRep, 3);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_SSENV_AccessTypeRep, L"ENV`AccessTypeRep"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_SSENV_AccessFnTypeRep, 3);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_SSENV_AccessFnTypeRep, L"ENV`AccessFnTypeRep"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_SSENV_AccessPolyTypeRep, 3);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_SSENV_AccessPolyTypeRep, L"ENV`AccessPolyTypeRep"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_SSENV_AccessOpTypeRep, 4);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_SSENV_AccessOpTypeRep, L"ENV`AccessOpTypeRep"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_SSENV_AccessFieldRep, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_SSENV_AccessFieldRep, L"ENV`AccessFieldRep"); 
#endif // VDMPP
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_SSENV_FunctionInfo, 3);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_SSENV_FunctionInfo, L"ENV`FunctionInfo"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_SSENV_ErrMsg, 5);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_SSENV_ErrMsg, L"ERR`ErrMsg"); 

  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_CGMAIN_VT, 2);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_CGMAIN_VT, L"CGMAIN`VT"); 
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_CONC_ClassInfo, 9);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_CONC_ClassInfo, L"CONC`ClassInfo"); 
}

void VAL2X::init_POG_TEST()
{
  VDMGetDefaultRecInfoMap().NewTag(TAG_TYPE_TEST_ProofObligationPP, 4);
  VDMGetDefaultRecInfoMap().SetSymTag(TAG_TYPE_TEST_ProofObligationPP, L"TEST`ProofObligationPP"); 
}

#ifdef VDMPP
void VAL2X::print_obj_ref_v(wostream&os, const Record & r, VDMFormatter vg, bool /*simple*/)
{
  // Tag == TAG_TYPE_SEM_OBJ_uRef_PP means printing an object value.
  // The format is the following:
  // 
  //   typename :: seq of wchar_t (the name of the class of the object),
  //                           (implemented as a Token).
  //   content  :: map name to val (the name of the instance variable
  //                               (implemented as a token (ex. A`i,
  //                               (instance variable i of class A).
  //   objectname :: seq of wchar_t (Token, given by getobjrefid). 

  Token uniquename (r.GetField(3)); 
  os << uniquename.GetString() << L"("; // 20100616
  Token tpnm (r.GetField(1));
  os << tpnm.GetString() << L"):" << endl; // 20100616

  vg.IncrementIndent();
  my_space(os, vg.GetIndent());
  os << L"< ";
  vg.IncrementIndent();

  Map m (r.GetMap(2));
  Set dom_m (m.Dom());

  bool first (true);
  Generic instvar;
  for (bool bb = dom_m.First(instvar); bb; bb = dom_m.Next(instvar)) {
    if (! first) {
      os << L"," << endl;
      my_space(os, vg.GetIndent());
    }
    else {
      first = false;
    }

    os  << Token(instvar).GetString() << L" = "; // 20100616
    m[instvar].ostream_out(os, vg);
  }
  os << L" >" << flush;
  return;
}

wstring VAL2X::getobjrefid(const TYPE_SEM_OBJ_uRef & r)
{
  Int uniquekey (r.GetInt(pos_SEM_OBJ_uRef_ref));
  return L"objref" + uniquekey.ascii(); 
}

// IsStatic
// clsnm : AS`Name
// valnm : AS`Name
// ==> bool * [SEM`VAL * AS`Access]
Tuple VAL2X::IsStatic(const TYPE_AS_Name & clsnm, const TYPE_AS_Name & valnm)
{
  MAP<TYPE_AS_Name,TYPE_GLOBAL_SigmaClass> classes (theState().GetClasses());

  //Generic cls_g;
  if (!classes.DomExists(clsnm)) {
    return mk_(Bool(false), Nil());
  }

  TYPE_GLOBAL_SigmaClass sigmacl (classes[clsnm]);
  const SEQ<TYPE_AS_InstAssignDef> & instvars (sigmacl.GetSequence(pos_GLOBAL_SigmaClass_instvars));
  size_t len_instvars = instvars.Length();
  for (size_t idx = 1; idx <= len_instvars; idx++) {
    const TYPE_AS_InstAssignDef & instdef (instvars[idx]);
    const TYPE_AS_AssignDef & ad (instdef.GetRecord(pos_AS_InstAssignDef_ad));
    if( ad.GetRecord(pos_AS_AssignDef_var) == valnm ) {
      if(instdef.GetBoolValue(pos_AS_InstAssignDef_stat)) {
        const Map & statics (sigmacl.GetMap(pos_GLOBAL_SigmaClass_statics)); // map AS`Name to (SEM`VAL * AS`Type * AS`Access)
        if(statics.DomExists(valnm)) {
          return mk_(Bool(true), statics[valnm]); // SEM`VAL
        }
      }
      else {
        return mk_(Bool(false), Nil());
      }
    }
  }
  return mk_(Bool(false), Nil());
}

// objval2generic
// sem_val : SEM`OBJ
// obj_ref : SEM`OBJ_Ref
// obj_refs : set of SEM`OBJ_Ref
// ==> 
Generic VAL2X::objval2generic(const TYPE_SEM_OBJ & sem_val,
                              const TYPE_SEM_OBJ_uRef & obj_ref,
                              const Set & obj_refs)
{
  if (obj_refs.InSet(obj_ref)) {
    return Token (L"Cyclic Object Reference");
  }

  Set new_obj_refs (obj_refs);
  new_obj_refs.Insert(obj_ref);
  
  Token tp_nm (ASTAUX::ASName2String(sem_val.GetRecord(pos_SEM_OBJ_tp)));

  const TYPE_SEM_InsStrct & ins (sem_val.GetMap(pos_SEM_OBJ_ins));
  SET<TYPE_AS_Name> dom_ins (ins.Dom());
  
  Map classes (theState().GetClasses());
  Map m;
  Generic clnm;
  for (bool bb = dom_ins.First(clnm); bb; bb = dom_ins.Next(clnm)) {
    TYPE_GLOBAL_ValueMap valmap (ins[clnm]);
    SET<TYPE_AS_Name> dom_valmap (valmap.Dom());
    Generic var_nm;
    // valmap : GLOBAL`ValueMap = map AS`Name to (SEM`VAL * AS`Type * AS`Access)
    for (bool bb = dom_valmap.First(var_nm); bb; bb = dom_valmap.Next(var_nm)) {
      Tuple infer (IsStatic(clnm, var_nm)); // bool * (SEM`VAL * AS`Type * AS`Access)
      Tuple t (infer.GetBoolValue(1) ? Tuple(infer.GetField(2)) : Tuple(valmap[var_nm]));

      const TYPE_SEM_VAL & semval (t.GetRecord(1));
      const TYPE_AS_Access & access (t.GetField(3));

      wstring astr (infer.GetBoolValue(1) ? L"S" : L" ");
      if (access.IsInt()) {
        switch(Int(access).GetValue()) {
          case PRIVATE_AS: {
            astr += L"- ";
            break;
          }
          case PROTECTED_AS: {
            astr += L"# ";
            break;
          }
          case PUBLIC_AS: {
            astr += L"+ ";
            break;
          }
          case NOT_INITIALISED_AS:
          default: {
            astr += L"  ";
            break;
          }
        }
      }

      wstring str (astr + ASTAUX::ASName2String(clnm) + L"`" + ASTAUX::ASName2String(var_nm));

      //HACK, TOBEFIXED: 
      //Do not follow ojectrefs, 
      // and perhaps print unique object number instead of OBJREF.
      if (semval.Is(TAG_TYPE_SEM_OBJ_uRef)) {     
        m.ImpModify(Token(str), Token(getobjrefid(semval)));
      } else {       
        m.ImpModify(Token(str), val2generic(semval, new_obj_refs));
      }
    }
  }
  
  Record rc (TAG_TYPE_DYNSEM_SEM_OBJ_Ref_PP, 3);
  rc.SetField(1, tp_nm); // Token
  rc.SetField(2, m); // map Token to Token
  rc.SetField(3, Token(getobjrefid(obj_ref))); // Token
  return rc;
}
#endif //VDMPP

// val2generic
// value : SEM`VAL
// obj_refs : set of SEM`OBJ_Ref
// ==> Generic
Generic VAL2X::val2generic(const TYPE_SEM_VAL & value, const Set & obj_refs)
{
  switch (value.GetTag()) {
    case TAG_TYPE_SEM_BOOL: {
      return value.GetBool(pos_SEM_BOOL_v); 
    }
    case TAG_TYPE_SEM_NUM: {
      return value.GetReal(pos_SEM_NUM_v); 
    }
    case TAG_TYPE_SEM_CHAR: {
      return value.GetChar(pos_SEM_CHAR_v); 
    }
    case TAG_TYPE_SEM_SEQ: {
      const SEQ<TYPE_SEM_VAL> & v (value.GetSequence(pos_SEM_SEQ_v));
      Sequence l;
      size_t len_v = v.Length();
      for(size_t i = 1; i <= len_v; i++) {
        const Generic & e (v[i]);
        if(e.IsChar()) {
          l.ImpAppend(e);
        }
        else {
          l.ImpAppend(val2generic(e, obj_refs));
        }
      }
      return l;
    }
    case TAG_TYPE_SEM_SET: {
      SET<TYPE_SEM_VAL> v (value.GetSet(pos_SEM_SET_v));
      Set s;
      Generic e;
      for (bool bb = v.First(e); bb; bb = v.Next (e)) {
        s.Insert(val2generic(e, obj_refs));
      }
      return s;
    }
    case TAG_TYPE_SEM_MAP: {
      const Map & v (value.GetMap(pos_SEM_MAP_v));
      Set dom_v (v.Dom());
      Map m;
      Generic d;
      for (bool bb = dom_v.First(d); bb; bb = dom_v.Next (d)) {
        m.Insert (val2generic(d, obj_refs), val2generic(v[d], obj_refs));
      }
      return m;
    }
    case TAG_TYPE_SEM_TUPLE: {
      const SEQ<TYPE_SEM_VAL> & v (value.GetSequence(pos_SEM_TUPLE_v));
      size_t len_v = v.Length ();
      Tuple t (len_v);
      for(size_t i = 1; i <= len_v; i++) {
        t.SetField (i, val2generic(v.Index(i), obj_refs));
      }
      return t;
    }
    case TAG_TYPE_DYNSEM_SEM_REC: {
      Record rec (value.GetRecord(pos_DYNSEM_SEM_SemRecord_value));
      SEQ<TYPE_SEM_VAL> fields_v (rec.GetFields());
      Sequence res_fields_v;
      size_t len_fields_v = fields_v.Length();
      for (size_t i = 1; i <= len_fields_v; i++) {
        res_fields_v.ImpAppend(val2generic(fields_v[i], obj_refs));
      }
      rec.SetFields(res_fields_v);
      return rec;
    }
    case TAG_TYPE_SEM_QUOTE: {
      const type_cL & seq (value.GetField(pos_SEM_QUOTE_v));
      wstring seqStr;
      seq.GetString(seqStr);
      return Quote (seqStr);
    }
    case TAG_TYPE_SEM_NIL: {
      return Nil();  
    }
    case TAG_TYPE_SEM_CONT: {
      return Nil ();
    }
    case TAG_TYPE_SEM_ExplFN: {
// 20150325 -->
      //return Token (L"FUNCTION_VAL");
      wstring res (L"FUNCTION_VAL: ");
      res += GetStatSem().ASType2Ascii(value.GetRecord(pos_SEM_ExplFN_tp));
      return Token (res);
// <-- 20150325
    }
    case TAG_TYPE_SEM_ExplPOLY: {
// 20150325 -->
      //return Token (L"FUNCTION_VAL");
      wstring res (L"FUNCTION_VAL: ");
      res += GetStatSem().ASType2Ascii(value.GetRecord(pos_SEM_ExplPOLY_tp));
      return Token (res);
// <-- 20150325
    }
    case TAG_TYPE_SEM_CompExplFN: {
// 20150325 -->
      //return Token (L"FUNCTION_VAL");
      const SEQ<TYPE_SEM_ExplFN> & fl (value.GetSequence(pos_SEM_CompExplFN_fl));
      const TYPE_SEM_ExplFN & ffn (fl[1]);
      TYPE_AS_Type ftp (ffn.GetRecord(pos_SEM_ExplFN_tp));

      if (fl.Length() > 1) {
        const TYPE_SEM_ExplFN & lfn (fl[fl.Length()]);
        TYPE_AS_Type ltp (lfn.GetRecord(pos_SEM_ExplFN_tp));
  
        switch (ftp.GetTag()) {
          case TAG_TYPE_AS_PartialFnType: {
            switch (ltp.GetTag()) {
              case TAG_TYPE_AS_PartialFnType: {
                ftp.SetField(pos_AS_PartialFnType_fnrng, ltp.GetRecord(pos_AS_PartialFnType_fnrng));
                break;
              }
              case TAG_TYPE_AS_TotalFnType: {
                ftp.SetField(pos_AS_PartialFnType_fnrng, ltp.GetRecord(pos_AS_TotalFnType_fnrng));
                break;
              }
            }
            break;
          }
          case TAG_TYPE_AS_TotalFnType: {
            switch (ltp.GetTag()) {
              case TAG_TYPE_AS_PartialFnType: {
                ftp.SetField(pos_AS_TotalFnType_fnrng, ltp.GetRecord(pos_AS_PartialFnType_fnrng));
                break;
              }
              case TAG_TYPE_AS_TotalFnType: {
                ftp.SetField(pos_AS_TotalFnType_fnrng, ltp.GetRecord(pos_AS_TotalFnType_fnrng));
                break;
              }
            }
            break;
          }
        }
      }
      wstring res (L"FUNCTION_VAL: ");
      res += GetStatSem().ASType2Ascii(ftp);
      return Token (res);
// <-- 20150325
    }
    case TAG_TYPE_SEM_ExplOP: {
// 20150325 -->
      //return Token (L"OPERATION_VAL");
      wstring res (L"OPERATION_VAL: ");
      res += GetStatSem().ASType2Ascii(value.GetRecord(pos_SEM_ExplOP_tp));
      return Token (res);
// <-- 20150325
    }
#ifdef VDMPP
    case TAG_TYPE_SEM_OverOPFN: {
      const Map & m (value.GetMap(pos_SEM_OverOPFN_overload));
      wstring res (L"OVERLOADED_VAL");
      if (!m.IsEmpty())
      {
        Tuple t (m.Dom().GetElem());
        const TYPE_AS_Name & overnm (t.GetRecord(1));
// 20120723 -->
        //Tuple tlacc (m[t]);
        //const SEQ<TYPE_AS_Type> & tp_l (tlacc.GetSequence(1));
        //res += L": " + ASTAUX::ASName2String(MANGLE::GetUnmangledName(overnm)) + ASTypeList2Ascii(tp_l);
        res += L": " + ASTAUX::ASName2String(MANGLE::GetUnmangledName(overnm));
// <-- 20120723
      }
      return Token (res);
    }
#endif // VDMPP
    case TAG_TYPE_SEM_TOKEN: {
      return mk_token(val2generic(value.GetField(pos_SEM_TOKEN_v), obj_refs));
    }
    case TAG_TYPE_SEM_UNDEF: {
      return Quote (L"<UNDEFINED>");
    }
    case TAG_TYPE_SEM_EXIT: {
// 20120425 -->
//      return val2generic(value.GetField(pos_SEM_EXIT_v), obj_refs);
      return SEQ<Char>(L"exit ").ImpConc(Sequence(val2generic(value.GetField(pos_SEM_EXIT_v), obj_refs).ascii()));
// <-- 20120425
    }
    case TAG_TYPE_SEM_RETURN: {
      return Token (L"RETURN_VAL");
    }
#ifdef VDMPP
    case TAG_TYPE_SEM_OBJ_uRef: {
      if (theState().IsInObjTab(value)) {
        return objval2generic(theState().GetSemObjInTab(value), value, obj_refs);
      }
      else {
        vdm_iplog << L"WRONG OBJ Ref FOR 'val2generic' - was:" << value << L"." << endl << flush;
        return Generic();
      }
    }
    
#endif //VDMPP        
    default:{
      vdm_iplog << L"WRONG TAG FOR 'val2generic' - was:" << value.GetTag() << L"." << endl << flush;
      return Generic();
    }
  }
}

// val2asc
wstring VAL2X::val2asc(const TYPE_SEM_VAL & r) 
{
  Generic g (val2generic(r, Set()));
  return (g.ascii());
}

// val2stream
void VAL2X::val2stream(const TYPE_SEM_VAL & r, wostream & str, int ind)
{
  Generic g (val2generic(r, Set()));
  g.pr_ascii(str, ind);
}

// PrintSemValue
// 
void VAL2X::PrintSemValue (const TYPE_SEM_VAL & val_v, wostream & ostr)
{
  switch (val_v.GetTag()) {
    case TAG_TYPE_SEM_CONT: {
      ostr << L"(no return value)";
      break;
    }
    case TAG_TYPE_SEM_EXIT: {
      ostr << L"exit";
      const Generic & value (val_v.GetField(pos_SEM_EXIT_v));
      if (!value.IsNil())
      {
        ostr << L" ";
        val2stream (value, ostr, (Settings.PrintFormat() ? 0 : -1));
      }
      break;
    }
    default: {
      val2stream (val_v, ostr, (Settings.PrintFormat() ? 0 : -1));
      break;
    }
  }
  ostr << endl;
}

// ASTypeList2Ascii
// tp_l : seq of AS`Type
// +> seq of char
wstring VAL2X::ASTypeList2Ascii(const SEQ<TYPE_AS_Type> & tp_l)
{
  if (tp_l.IsEmpty())
    return L"()";

  wstring res (L"(");
  size_t len_tp_l = tp_l.Length();
  for (size_t i = 1; i <= len_tp_l; i++)
  {
    if (i > 1)
      res += L", ";

    const TYPE_AS_Type & tp (tp_l[i]);
    res += GetStatSem().ASType2Ascii(tp);
  }
  res += L")";
  return res;
}
