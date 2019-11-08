/***
*  * Id
*  *    $Id: err.cc,v 1.49 2006/03/20 07:35:17 vdmtools Exp $
*  * WHAT
*  *    err.cc: Implementation of err.vdm 1.19
***/

#include "statsem.h"
#include "UTIL.h"
#include "contextinfo.h"  // to get GetCI
#include "astaux.h"       // to get GetCid
#include "settings.h"
#include "tbutils.h"
#include "tb_wstring.h"

#ifndef NOSS

void StatSem::UpdateLastPosition (const Record & ast)
{
  if (ast.Length () > 0 ) {
    int no_of_fields = ast.Length ();
    int l = -1 , c = -1 , f = -1;
    if (ast.GetField (no_of_fields).IsTuple ()) {
      Tuple posinfo (ast.GetField (no_of_fields));
      no_of_fields = posinfo.Length ();
      if (no_of_fields < 3) {
        return;
      }
      if (posinfo.GetField (1).IsInt ()) {
        l = Int (posinfo.GetField (1));
      }
      if (posinfo.GetField (2).IsInt ()) {
        c = Int (posinfo.GetField (2));
      }
      if (posinfo.GetField (3).IsInt ()) {
        f = Int (posinfo.GetField (3));
      }
    }
  
    if (l != -1) {
      this->lastline = l;
    }
    if (c != -1) {
      this->lastcol  = c;
    }
    if (f != -1) {
      this->lastfile = f;
    }
  }
}
  
wstring StatSem::GenLineColNo (const Record & ast)
{
  wstring PosStr;
  if (ast.Length () > 0) {
    wstring message;
    int no_of_fields = ast.Length ();
    if (no_of_fields < 1) {
      return PosStr;
    }
    int l = -1 , c = -1 , f = -1;
    if (ast.GetField (no_of_fields).IsTuple ()) {
      Tuple posinfo (ast.GetField (no_of_fields));
      no_of_fields = posinfo.Length ();
      if (no_of_fields < 3) {
        return PosStr;
      }
      if (posinfo.GetField (1).IsInt ()) {
        l = Int (posinfo.GetField (1));
      }
      if (posinfo.GetField (2).IsInt ()) {
        c = Int (posinfo.GetField (2));
      }
      if (posinfo.GetField (3).IsInt ()) {
        f = Int (posinfo.GetField (3));
      }
    }
    if (l != -1) {
      this->lastline = l;
    }
    if (c != -1) {
      this->lastcol  = c;
    }
    if (f != -1) {
      this->lastfile = f;
    }
    if ((this->lastline != 0) && (this->lastcol != 0)) {
      message = L"line : " + Int(this->lastline).ascii() + 
                L" Column : " + Int(this->lastcol).ascii() + L" : ";
    }
    PosStr += message;
    return PosStr;
  }
  else {
    return PosStr;
  }
}

// LookUpErrMsg
// num : nat
// Txts : seq of seq of char
// ==> seq of char
SEQ<Char> StatSem::LookUpErrMsg(int num, const SEQ< SEQ<Char> > & Txts)
{
  if (TBUTILS::isJapanese() && Settings.UseTranslation()) {
    SEQ<Char> err (TBWSTR::string2wstring(getErrMsgJp(num)));
    return UTIL::ReplacePercent(err, Txts);
  } 
  else {
    SEQ<Char> err (getErrMsg(num));
    return UTIL::ReplacePercent(err, Txts);
  }
}

void StatSem::GenErr (const Generic & ast, int SvTp, int errnum, const SEQ< SEQ<Char> > & Txts)
{
  GenErrTp (ast, SvTp, errnum, Nil (), Nil (), Txts);
}

void StatSem::GenErrTp (const Generic & ast, int SvTp, int errnum,
                             const Generic & tp1, const Generic & tp2, 
                             const SEQ< SEQ<Char> > & Txts)
{
  if (SvTp > Settings.ErrorLevel()) return;

  SEQ<Char> Txt (LookUpErrMsg(errnum, Txts));

  Int pos_fid (this->lastfile);
  Int pos_line (this->lastline);
  Int pos_col (this->lastcol);

  Sequence descr_l;
  if (!ast.IsNil()) {
    Tuple gfp (GetCI().GetFilePos(ASTAUX::GetCid(ast)));
    if (gfp.GetBoolValue(1)) {
      const TYPE_CI_FileId & fid (gfp.GetInt(2));
      const TYPE_CI_TokenPos & astpos (gfp.GetRecord(4));
      pos_fid = fid;
      pos_line = astpos.get_abs_uline();
      pos_col  = astpos.get_column();
    }
    else {
      descr_l.ImpAppend (SEQ<Char>(L"(Imprecise position information.)"));
    }
  }
  else {
    descr_l.ImpAppend (SEQ<Char>(L"(Imprecise position information.)"));
  }
  
  if (!tp1.IsNil () && !tp2.IsNil ()) {
#ifdef VDMSL
    TYPE_REP_TypeRep tp1_t (tp1);
    TYPE_REP_TypeRep tp2_t (tp2);
#endif //VDMSL
#ifdef VDMPP
    TYPE_REP_TypeRep tp1_t (StripAccessType (tp1));
    TYPE_REP_TypeRep tp2_t (StripAccessType (tp2));
#endif // VDMPP
    if (TBUTILS::isJapanese() && Settings.UseTranslation()) {
      descr_l.ImpAppend (SEQ<Char>(L"act : " + Type2Ascii (tp1_t)));
      descr_l.ImpAppend (SEQ<Char>(L"exp : " + Type2Ascii (tp2_t)));
    }
    else {
      descr_l.ImpAppend (SEQ<Char>(L"act : " + Type2Ascii (tp1_t)));
      descr_l.ImpAppend (SEQ<Char>(L"exp : " + Type2Ascii (tp2_t)));
    }
  }
 
  SEQ<Char> errStr (SEQ<Char>(L"[").ImpConc(SEQ<Char>(Int(errnum).ascii())).ImpConc(SEQ<Char>(L"] : ")).ImpConc(Txt));

  TYPE_ProjectTypes_Message ss;
  switch(SvTp) {
    case WRN1:
    case WRN2: {
      descr_l.ImpPrepend(SEQ<Char>(L"Warning").ImpConc(errStr));
      ss = TYPE_ProjectTypes_WarnMsg().Init(pos_fid, pos_line, pos_col, descr_l);
      break;
    }
    default: {
      switch(SvTp) {
        case PRF: {
          descr_l.ImpPrepend(SEQ<Char>(L"Proof obligation").ImpConc(errStr));
          break;
        }
        case ERR:
        case ERR2: {
          descr_l.ImpPrepend(SEQ<Char>(L"Error").ImpConc(errStr));
          break;
        }
        default: {
          descr_l.ImpPrepend(SEQ<Char>(L"Severe error").ImpConc(errStr));
          break;
        }
      }
      ss = TYPE_ProjectTypes_ErrMsg().Init(pos_fid, pos_line, pos_col, descr_l);
      break;
    }
  }

  //  bool runningTestEnv = (getenv(L"STATSEM_KEEP_DUPLICATES") != 0);
  //  if (!already_reported || runningTestEnv) {
  if (!this->StatSemErrors.Elems().InSet(ss)) {
    this->StatSemErrors.ImpAppend (ss);
    if (SvTp == WRN1 || SvTp == WRN2) {
      TBUTILS::IncrementWarningsCount();
    }
    else {
      TBUTILS::IncrementErrorsCount();
    }
  }
}

void StatSem::InternalError (const wstring & str) const
{
  vdm_log << L"Internal error '" << str << L"'" << endl;
  vdm_log << L"Please report this error" << endl;
}

wstring StatSem::ASType2Ascii (const TYPE_AS_Type & tp) const
{
  return Type2Ascii(TransType(GetCurModOrNil(),tp));
}

wstring StatSem::Type2Ascii (const TYPE_REP_TypeRep & tp, int level) const
{
  int newlevel = level + 1;
// This is not yet testet (the right level should be selected):
//   if (level > 5)
//     return L"...";

  switch (tp.GetTag ()) {
    case TAG_TYPE_REP_BooleanTypeRep: {
      return L"bool";
    }
    case TAG_TYPE_REP_NumericTypeRep: {
      switch (tp.GetIntValue(pos_REP_NumericTypeRep_qtp)) {
        case NATONE:     return L"nat1";
        case NAT:        return L"nat";
        case INTEGER:    return L"int";
        case RAT:        return L"rat";
        case REAL:       return L"real";
        default:         return L"";
      }
    }
    case TAG_TYPE_REP_TokenTypeRep: {
      return L"token";
    }
    case TAG_TYPE_REP_CharTypeRep: {
      return L"char";
    }
    case TAG_TYPE_REP_QuoteTypeRep: {
      wstring str;
      str += L"<";
      str += PrintName (tp.GetRecord(pos_REP_QuoteTypeRep_lit)).GetString();        
      str += L">";
      return str;
    }
    case TAG_TYPE_REP_TypeParRep: {
      return PrintName (tp.GetRecord(pos_REP_TypeParRep_nm)).GetString();
    }
    case TAG_TYPE_REP_TypeNameRep: {
#ifdef VDMSL
      TYPE_AS_Name name (tp.GetRecord(pos_REP_TypeNameRep_nm));
      if (name.GetSequence(pos_AS_Name_ids).Length() == 2) {
	if (ASTAUX::GetFirstName(name) == ASTAUX::GetDefaultModASName()) {
          name = ASTAUX::GetSecondName(name);
        }
      }
      return PrintName (name).GetString();
#endif // VDMSL
#ifdef VDMPP
      return PrintName (tp.GetRecord(pos_REP_TypeNameRep_nm)).GetString();
#endif // VDMPP
    }
    case TAG_TYPE_REP_InvTypeRep: {
      wstring str;
      str += L"inv_(";
      str += Type2Ascii (tp.GetRecord(pos_REP_InvTypeRep_shape), newlevel);
      str += L")";
      return str;
    }
    case TAG_TYPE_REP_TypeVarRep: {
      wstring str;
      str += L"@";
      str += PrintName (tp.GetRecord(pos_REP_TypeVarRep_nm)).GetString();
      return str;
    }
#ifdef VDMPP
    case TAG_TYPE_REP_ObjRefTypeRep: {
      wstring str;
      const TYPE_AS_Name & nm (tp.GetRecord(pos_REP_ObjRefTypeRep_nm));
      if (nm.get_ids().IsEmpty()) {
        str += L"a object reference type";
      }
      else {
        str += PrintName (nm).GetString();
      }
      return str;
    }
    case TAG_TYPE_REP_OverTypeRep: {
      wstring str;
      str += L"( ";
      SET<TYPE_REP_TypeRep> tp_s (tp.GetSet(pos_REP_OverTypeRep_tps));
      bool first = true;
      Generic g;
      for(bool bb = tp_s.First(g); bb; bb = tp_s.Next(g)) {
        if (!first) {
          str += L" | ";
        }
        else {
          first = false;
        }
        str += Type2Ascii (g, newlevel);
      }
      str += L" )";
      return str;
    }
#endif //VDMPP
    case TAG_TYPE_REP_CompositeTypeRep: {
      const TYPE_AS_Name & tag (tp.GetRecord(pos_REP_CompositeTypeRep_nm));
      if (!tag.get_ids().IsEmpty()) {
        wstring str;
        str += L"compose " + PrintName (tag).GetString() + L" of ";

        const SEQ<TYPE_REP_FieldRep> & fld_l (tp.GetField(pos_REP_CompositeTypeRep_fields));
        size_t len_fld_l = fld_l.Length();
        for (size_t idx = 1; idx <= len_fld_l; idx++) {
          str += Type2Ascii (fld_l[idx].GetRecord(pos_REP_FieldRep_tp), newlevel) + L" ";
        }
        str += L"end";
        return str;
      }
      else {
        return wstring(L"a record type");
      }
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (tp.GetSet(pos_REP_UnionTypeRep_tps));
      if (tp_s.Card() == 2) {
        SET<TYPE_REP_TypeRep> tp_s2 (tp_s);
        Generic g;
        for(bool bb = tp_s2.First(g); bb; bb = tp_s2.Next(g)) {
          TYPE_REP_TypeRep t (g);
          switch (t.GetTag()) {
            case TAG_TYPE_REP_SeqTypeRep: {
              SET<TYPE_REP_TypeRep> tmp (tp_s);
              tmp.RemElem(g);
              TYPE_REP_TypeRep e (tmp.GetElem());
              if (e.Is(TAG_TYPE_REP_EmptySeqTypeRep) &&
                 (e.GetRecord(pos_REP_EmptySeqTypeRep_elemtp) == t.GetRecord(pos_REP_SeqTypeRep_elemtp))) {
                wstring str;
                str += L"seq of ";
                str += Type2Ascii (t.GetRecord(pos_REP_SeqTypeRep_elemtp), newlevel);
                return str;
              }
              break;
            }
            case TAG_TYPE_REP_SetTypeRep: {
              SET<TYPE_REP_TypeRep> tmp (tp_s);
              tmp.RemElem(g);
              TYPE_REP_TypeRep e (tmp.GetElem());
              if (e.Is(TAG_TYPE_REP_EmptySetTypeRep) &&
                 (e.GetRecord(pos_REP_EmptySetTypeRep_elemtp) == t.GetRecord(pos_REP_SetTypeRep_elemtp))) {
                return Type2Ascii(t, newlevel);
              }
              break;
            }
            case TAG_TYPE_REP_GeneralMapTypeRep: {
              SET<TYPE_REP_TypeRep> tmp (tp_s);
              tmp.RemElem(g);
              TYPE_REP_TypeRep e (tmp.GetElem());
              if (e.Is(TAG_TYPE_REP_EmptyMapTypeRep) &&
                 (e.GetRecord(pos_REP_EmptyMapTypeRep_mapdom) == t.GetRecord(pos_REP_GeneralMapTypeRep_mapdom)) &&
                 (e.GetRecord(pos_REP_EmptyMapTypeRep_maprng) == t.GetRecord(pos_REP_GeneralMapTypeRep_maprng))) {
                return Type2Ascii(t, newlevel);
              }
              break;
            }
            case TAG_TYPE_REP_InjectiveMapTypeRep: {
              SET<TYPE_REP_TypeRep> tmp (tp_s);
              tmp.RemElem(g);
              TYPE_REP_TypeRep e (tmp.GetElem());
              if (e.Is(TAG_TYPE_REP_EmptyMapTypeRep) &&
                 (e.GetRecord(pos_REP_EmptyMapTypeRep_mapdom) == t.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom)) &&
                 (e.GetRecord(pos_REP_EmptyMapTypeRep_maprng) == t.GetRecord(pos_REP_InjectiveMapTypeRep_maprng))) {
                return Type2Ascii(t, newlevel);
              }
              break;
            }
          }
        }
      }
      wstring str;
      str += L"( ";
      bool first = true;
      Generic g;
      for(bool bb = tp_s.First(g); bb; bb = tp_s.Next(g)) {
        if (!first) {
          str += L" | ";
        }
        else {
          first = false;
        }
        str += Type2Ascii (g, newlevel);
      }
      str += L" )";
      return str;
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & tp_l (tp.GetSequence(pos_REP_ProductTypeRep_tps));
      wstring str;
      str += L"( ";
      size_t len_tp_l = tp_l.Length();
      for(size_t j = 1; j < len_tp_l; j++) {
        str += Type2Ascii (tp_l[j], newlevel);
        str += L" * ";
      }
      str += Type2Ascii (tp_l[tp_l.Length ()],newlevel);
      str += L" )";
      return str;
    }
    case TAG_TYPE_REP_SetTypeRep: {
      wstring str;
      str += L"set1 of ";
      str += Type2Ascii (tp.GetRecord(pos_REP_SetTypeRep_elemtp), newlevel);
      return str;
    }
    case TAG_TYPE_REP_SeqTypeRep: { 
      wstring str;
      str += L"seq1 of ";
      str += Type2Ascii (tp.GetRecord(pos_REP_SeqTypeRep_elemtp), newlevel);
      return str;
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: { 
      wstring str;
      str += L"map1 ";
      str += Type2Ascii (tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom), newlevel);
      str += L" to ";
      str += Type2Ascii (tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng), newlevel);
      return str;
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      wstring str;
      str += L"inmap1 ";
      str += Type2Ascii (tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom), newlevel);
      str += L" to ";
      str += Type2Ascii (tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng), newlevel);
      return str;
    }
    case TAG_TYPE_REP_PartialFnTypeRep: { 
      const Generic & fndom (tp.GetField(pos_REP_PartialFnTypeRep_fndom));
      wstring str;
      str += L"( ";
      if (fndom.IsSequence()) {
        SEQ<TYPE_REP_TypeRep> tp_l (fndom);
        if (tp_l.IsEmpty()) {
          str += L"()";
        }
        else if (tp_l.Length () == 1) {
          str += Type2Ascii (tp_l.Hd (), newlevel);
        }
        else {
          TYPE_REP_ProductTypeRep rc;
          rc.SetField (pos_REP_ProductTypeRep_tps, tp_l);
          str += Type2Ascii (rc, newlevel);
        }
      }
      else {
        str += L"*";
      }
      str += L" -> ";
      str += Type2Ascii (tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng), newlevel);
      str += L" )";
      return str;
    }
    case TAG_TYPE_REP_TotalFnTypeRep: { 
      const Generic & fndom (tp.GetField(pos_REP_TotalFnTypeRep_fndom));
      wstring str;
      str += L"( ";
      if (fndom.IsSequence()) {
        SEQ<TYPE_REP_TypeRep> tp_l (fndom);
        if (tp_l.IsEmpty()) {
          str += L"()";
        }
        else if (tp_l.Length () == 1) {
          str += Type2Ascii (tp_l.Hd (), newlevel);
        }
        else {
          TYPE_REP_ProductTypeRep rc;
          rc.SetField (pos_REP_ProductTypeRep_tps, tp_l);
          str += Type2Ascii (rc, newlevel);
        }
      }
      else {
        str += L"*";
      }
      str += L" +> ";
      str += Type2Ascii (tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng), newlevel);
      str += L" )";
      return str;
    }
    case TAG_TYPE_REP_PolyTypeRep: { 
      const SEQ<TYPE_REP_TypeVarRep> & l_vars (tp.GetSequence(pos_REP_PolyTypeRep_vars));
      const TYPE_REP_FnTypeRep & l_tp (tp.GetRecord(pos_REP_PolyTypeRep_tp));
    
      wstring str;
      str += L"[ ";
      if (!l_vars.IsEmpty()) {
        bool first = true;
        size_t len_l_vars = l_vars.Length();
        for(size_t idx = 1; idx <= len_l_vars; idx++) {
          if (!first) {
            str += L" , ";
          }
          else {
            first = false;
          }
          str += Type2Ascii (l_vars[idx], newlevel);
        }
      }
      str += L" ] ";    
      str += Type2Ascii (l_tp, newlevel);
      return str;
    }
    case TAG_TYPE_REP_OpTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & tp_l (tp.GetSequence(pos_REP_OpTypeRep_Dom));
      wstring str;
      if (tp_l.IsEmpty()) {
        str += L"()";
      }
      else if (tp_l.Length () == 1) {
        str += Type2Ascii (tp_l.Hd (), newlevel);
      }
      else {
        TYPE_REP_ProductTypeRep rc;
        rc.SetField (pos_REP_ProductTypeRep_tps, tp_l);
        str += Type2Ascii (rc, newlevel);
      }
      str += L" ==> ";
      str += Type2Ascii (tp.GetRecord(pos_REP_OpTypeRep_Rng), newlevel);
      return str;
    }
    case TAG_TYPE_REP_ExitTypeRep: {
      wstring str;
      str += L"exit " + Type2Ascii (tp.GetRecord(pos_REP_ExitTypeRep_tp), newlevel);
      return str;
    }
    case TAG_TYPE_REP_AllTypeRep: {
      return L"#";
    }
    case TAG_TYPE_REP_NilTypeRep: {
      return  L"nil";
    }
    case TAG_TYPE_REP_UnitTypeRep: {
      return L"()";
    }
    case TAG_TYPE_REP_RetTypeRep: {
      return L"()";
    }
    case TAG_TYPE_REP_EmptySetTypeRep: {
      return L"{}";
    }
    case TAG_TYPE_REP_EmptySeqTypeRep: {
      if (tp.GetRecord(pos_REP_EmptySeqTypeRep_elemtp) == btp_char) {
        return L"\"\"";
      }
      else {
        return L"[]";
      }
    }
    case TAG_TYPE_REP_EmptyMapTypeRep: {
      return L"{|->}";
    }
    default: {
      InternalError(L"Type2Ascii");
      return L"";
    }
  }
  return L""; // dummy
}

Sequence StatSem::GetErrors() const
{
  return this->StatSemErrors;
}

void StatSem::ResetErrors()
{
  this->StatSemErrors.Clear();
}

#endif //!NOSS
