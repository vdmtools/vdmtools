/***
*  * WHAT
*  *    This file implements a simple ASCII pretty-printer for the following
*  *    types in the AS module: AS`Type, AS`Pattern, AS`Expr.
*  *
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/uml/code/mini-p-print.cc,v $
*  * VERSION
*  *    $Revision: 1.21 $
*  * DATE
*  *    $Date: 2006/05/25 03:53:35 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "mini-p-print.h"
#include "rt_errmsg.h"
#include "metaiv.h"
#include "AS.h"
#include "asquotes.h"
#include "tbutils.h"
#include "tb_wstring.h"
#include "astaux.h"

// This table is used to construct a map from an AS-Quote representing
// an unary or a binary operator to the textual representation.
static struct OpTbl {
  const wchar_t* Qstr;
  const wchar_t* Op;
}
OP_TBL[] = {
  {L"AND", L" and "},
  {L"APPROX", L" ~= "},
  {L"COMPOSE", L"comp "},
  {L"EQ", L" = "},
  {L"EQUIV", L" <=> "},
  {L"FLOOR", L"floor "},
  {L"IMPLY", L" => "},
  {L"INSET", L" in set "},
  {L"INTDIV", L" div "},
  {L"MAPDISTRMERGE", L"merge "},
  {L"MAPINVERSE", L"inverse "},
  {L"MAPDOM", L"dom "},
  {L"MAPDOMRESTBY", L" <-: "},
  {L"MAPDOMRESTTO", L" <: "},
  {L"MAPMERGE", L" munion "},
  {L"MAPRNG", L"rng "},
  {L"MAPRNGRESTBY", L" :-> "},
  {L"MAPRNGRESTTO", L" :> "},
  {L"NE", L" <> "},
  {L"NOT", L"not "},
  {L"NOTINSET", L" not in set "},
  {L"NUMABS", L"abs "},
  {L"NUMDIV", L" / "},
  {L"NUMEXP", L" ** "},
  {L"NUMGE", L" >= "},
  {L"NUMGT", L" > "},
  {L"NUMLE", L" <= "},
  {L"NUMLT", L" < "},
  {L"NUMMINUS", L" - "},
  {L"NUMMOD", L" mod "},
  {L"NUMMULT", L" * "},
  {L"NUMPLUS", L" + "},
  {L"NUMREM", L" rem "},
  {L"OR", L" or "},
  {L"PROPERSUBSET", L" psubset "},
  {L"SEQCONC", L" ^ "},
  {L"SEQDISTRCONC", L"conc "},
  {L"SEQELEMS", L"elems "},
  {L"SEQHEAD", L"hd "},
  {L"SEQINDICES", L"inds "},
  {L"SEQLEN", L"len "},
  {L"SEQTAIL", L"tl "},
  {L"SETCARD", L"card "},
  {L"SETDISTRINTERSECT", L"dinter "},
  {L"SETDISTRUNION", L"dunion "},
  {L"SETINTERSECT", L" inter "},
  {L"SETMINUS", L" \\ "},
  {L"SETPOWER", L"power "},
  {L"SETUNION", L" union "},
  {L"SUBSET", L" subset "},
  {L"",L""}
};


wstring MPP::Seq2Ascii(const Sequence & s)
{
  wstring str;
  s.GetString(str);
  return str;
}


wstring MPP::ListSetOfSeq(const Set & lst_)
{
  Set lst (lst_);
  wstring str;
  Generic g;
  bool first = true;
  for(bool bb = lst.First(g); bb; bb = lst.Next(g))
  {
    if (first)
      first = false;
    else
      str += L", ";
    str += Seq2Ascii(g);
  }
  return str;
}

wstring MPP::RecordModList2Ascii(const Sequence & l)
{
  wstring str; 
  int first=1;
  size_t len_l = l.Length();
  for(size_t idx = 1; idx <= len_l; idx++)
  {
    Record r (l[idx]);
    str += first-- > 0 ? L"" : L", ";
    str += Type2Ascii(r.GetField(1)) + L" |-> " + Type2Ascii(r.GetField(2));
  }
  return str;
}

wstring MPP::MapletList2Ascii(const Sequence & l)
{
  wstring str; 
  if(!l.IsEmpty())
  {
    int first=1;
    size_t len_l = l.Length();
    for (size_t idx = 1; idx <= len_l; idx++)
    {
      //Record r(g);
      str += first-- > 0 ? L"" : L", ";
      //str += Type2Ascii(r.Getfield(1)) + L" |-> " + Type2Ascii(r.Getfield(2));
      str += Type2Ascii(l[idx]);
    }
  }
  else
    str = L"|->";
  return str;
}

wstring MPP::BindList2Ascii(const Sequence & l)
{
  wstring str; 
  int first=1;
  size_t len_l = l.Length();
  for(size_t idx = 1; idx <= len_l; idx++)
  {
    const TYPE_AS_MultBind & r (l[idx]);
    str += first-- > 0 ? L"" : L", ";
    switch(r.GetTag()){
      case TAG_TYPE_AS_MultSetBind: {
        str += PatternList2Ascii(r.GetField(1));
        str += L" in set " + Type2Ascii(r.GetField(2)); // Expr
        break;
      }
      case TAG_TYPE_AS_MultTypeBind: {
        str += PatternList2Ascii(r.GetField(1));
        str += L" : " + Type2Ascii(r.GetField(2)); // Type
        break;
      }
    }
  }
  return str;
}


wstring MPP::LocalDef2Ascii(const Sequence & def_l)
{
  return List2Ascii(def_l);  // This handles both patterns and FnDef's
}

wstring MPP::PatternList2Ascii(const Sequence & pat_l)
{
  return List2Ascii(pat_l);
}

wstring MPP::List2Ascii(const Sequence & l)
  // returns a wstring which is the comma separated sequence of the
  // (recursively) pretty-printed elements of the sequence l.
{
  wstring str;
  bool first = true;
  size_t len_l = l.Length(); 
  for(size_t idx = 1; idx <= len_l; idx++)
  {
    if (first)
      first = false;
    else
      str += L", ";

    str += Type2Ascii(l[idx]);
  }
  return str;
}

// ExprList2Ascii
wstring MPP::ExprList2Ascii(const Sequence & exp_l)
{
  return List2Ascii(exp_l);
}

wstring MPP::PrintNameCharSeq (const TYPE_AS_Name & rc)
{
  const TYPE_AS_Ids & ids (rc.GetSequence(pos_AS_Name_ids));
  switch (ids.Length()) {
    case 0:
      return L"";
    case 1:
      return ids.Index(1).GetString();
    case 2:
      return ids.Index(1).GetString() + L"`" + ids.Index(2).GetString();
    default: { // ids.Length() > 2 : will not occor this situation
      wstring res (ids.Index(1).GetString());
      size_t len_ids = ids.Length();
      for (size_t i = 2; i <= len_ids; i++)
      {
        res += L'`';
        res += ids.Index(i).GetString();
      }
      return res;
    }
  }
}
    
wstring MPP::Type2Ascii (const Record & tp)
{
  wstring str;

  static Map OpMap;
  if(OpMap.IsEmpty()) // Construct the map - only once:
    for(int i = 0; OP_TBL[i].Qstr[0]; i++)
      OpMap.Insert(Quote(wstring(OP_TBL[i].Qstr)), Text(wstring(OP_TBL[i].Op)));

  string tpstr = TBWSTR::wstring2string(tp.ascii());

  int tag = tp.GetTag ();
  switch (tag) {
    // ###########################
    // ## Tags from AS`Type     ##  
    // ###########################
  case TAG_TYPE_AS_BracketedType: {
    str += L"(";
    str += Type2Ascii(tp.GetRecord(pos_AS_BracketedType_tp));
    str += L")";
    break;
  }
  case TAG_TYPE_AS_AllType: {
    str += L"?";
    break;
  }
  case TAG_TYPE_AS_BooleanType: {
    str += L"bool";
    break;
  }
  case TAG_TYPE_AS_NumericType: {
    Quote btp (tp.GetField(pos_AS_NumericType_qtp));
    wstring btpstr (btp.GetValue());
    if(btpstr == L"NATONE")
      str += L"nat1";
    else if(btpstr == L"NAT")
      str += L"nat";
    else if(btpstr == L"INTEGER")
      str += L"int";
    else if(btpstr == L"RAT")
      str += L"rat";
    else if(btpstr == L"REAL")
      str += L"real";
    else {
      // RunTimeError(L"Type2Ascii", "Unknown basic type", btp);
      vdm_err << L"Warning: non-supported type encountered during mapping" << endl;
      str += L"NonSupType";
    }
    break;
  }
  case TAG_TYPE_AS_TokenType: {
    str += L"token";
    break;
  }
  case TAG_TYPE_AS_CharType: {
    str += L"char";
    break;
  }
  case TAG_TYPE_AS_VoidType: {
    //str += L"void";
    str += L"()";
    break;
  }
  case TAG_TYPE_AS_QuoteType: {
    str += Type2Ascii(tp.GetRecord(pos_AS_QuoteType_lit));   // field 1 is a QuoteLit
    break;
  }
  case TAG_TYPE_AS_CompositeType: {
    str += L"compose ";
    str += PrintNameCharSeq (tp.GetField (pos_AS_CompositeType_name));
    str += L" of ";

    SEQ<TYPE_AS_Field> fld_l (tp.GetSequence(pos_AS_CompositeType_fields));
    size_t len_fld_l = fld_l.Length();
    for(size_t idx = 1; idx <= len_fld_l; idx++) {
      str += Type2Ascii (fld_l[idx].GetRecord (pos_AS_Field_type));
      str += L" ";
    }

    str += L"end";
    break;
  }
  case TAG_TYPE_AS_UnionType: {
    // str += L"( ";
    SEQ<TYPE_AS_Type> tp_l (tp.GetSequence(pos_AS_UnionType_tps));
    size_t len_tp_l = tp_l.Length();
    for (size_t idx = 1; idx <= len_tp_l; idx++)
    {
      if (idx > 1)
	str += L" | ";
      str += Type2Ascii (tp_l[idx]);
    }
    // str += L" )";
    break;
  }
  case TAG_TYPE_AS_ProductType: {
    // str += L"( ";
    SEQ<TYPE_AS_Type> tp_l (tp.GetField(pos_AS_ProductType_tps));
    size_t len_tp_l = tp_l.Length();
    for(size_t j = 1; j < len_tp_l; j++) {
      str += Type2Ascii (tp_l[j]);
      str += L" * ";
    }
    str += Type2Ascii (tp_l[tp_l.Length ()]);
    // str += L" )";
    break;
  }
  case TAG_TYPE_AS_OptionalType: {
    str += L"[" + Type2Ascii(tp.GetRecord(pos_AS_OptionalType_tp)) + L"]";
    break;
  }
  case TAG_TYPE_AS_Set0Type: {
    str += L"set of ";
    str += Type2Ascii (tp.GetRecord (pos_AS_Set0Type_elemtp));
    break;
  }
  case TAG_TYPE_AS_Set1Type: {
    str += L"set1 of ";
    str += Type2Ascii (tp.GetRecord (pos_AS_Set1Type_elemtp));
    break;
  }
  case TAG_TYPE_AS_Seq0Type: {
    str += L"seq of ";
    str += Type2Ascii (tp.GetRecord (pos_AS_Seq0Type_elemtp));
    break;
  }
  case TAG_TYPE_AS_Seq1Type: { 
    str += L"seq1 of ";
    str += Type2Ascii (tp.GetRecord (pos_AS_Seq0Type_elemtp));
    break;
  }
  case TAG_TYPE_AS_GeneralMap0Type: { 
    str += L"map ";
    str += Type2Ascii (tp.GetRecord (pos_AS_GeneralMap0Type_mapdom));
    str += L" to ";
    str += Type2Ascii (tp.GetRecord (pos_AS_GeneralMap0Type_maprng));
    break;
  }
  case TAG_TYPE_AS_GeneralMap1Type: { 
    str += L"map1 ";
    str += Type2Ascii (tp.GetRecord (pos_AS_GeneralMap1Type_mapdom));
    str += L" to ";
    str += Type2Ascii (tp.GetRecord (pos_AS_GeneralMap1Type_maprng));
    break;
  }
  case TAG_TYPE_AS_InjectiveMap0Type: {
    str += L"inmap ";
    str += Type2Ascii (tp.GetRecord (pos_AS_InjectiveMap0Type_mapdom));
    str += L" to ";
    str += Type2Ascii (tp.GetRecord (pos_AS_InjectiveMap0Type_maprng));
    break;
  }
  case TAG_TYPE_AS_InjectiveMap1Type: {
    str += L"inmap1 ";
    str += Type2Ascii (tp.GetRecord (pos_AS_InjectiveMap1Type_mapdom));
    str += L" to ";
    str += Type2Ascii (tp.GetRecord (pos_AS_InjectiveMap1Type_maprng));
    break;
  }
  case TAG_TYPE_AS_TypeName: {
    str += PrintNameCharSeq (tp.GetRecord (pos_AS_TypeName_name));
    break;
  }
  case TAG_TYPE_AS_PartialFnType: {
    SEQ<TYPE_AS_Type> tp_l (tp.GetSequence (pos_AS_PartialFnType_fndom));
    str += L"(";
    bool first = true;
    Generic g;
    for (bool bb = tp_l.First(g); bb; bb = tp_l.Next(g)) {
      if (first)
        first = false;
      else
        str += L" * ";
      str += Type2Ascii(g);
    }
    str += L") -> ";
    str += Type2Ascii (tp.GetRecord (pos_AS_PartialFnType_fnrng));
    break;
  }
  case TAG_TYPE_AS_TotalFnType: { 
    SEQ<TYPE_AS_Type> tp_l (tp.GetSequence (pos_AS_TotalFnType_fndom));
    str += L"(";
    bool first = true;
    Generic g;
    for (bool bb = tp_l.First(g); bb; bb = tp_l.Next(g)) {
      if (first)
        first = false;
      else
        str += L" * ";
      str += Type2Ascii(g);
    }
    str += L") +> ";
    str += Type2Ascii (tp.GetRecord (pos_AS_TotalFnType_fnrng));
    break;
  }
 
    // ###########################
    // ## Tags from AS`Pattern  ##  
    // ###########################

  case TAG_TYPE_AS_PatternName: {
    if(!tp.GetField(pos_AS_PatternName_nm).IsNil())
      str += PrintNameCharSeq (tp.GetRecord (pos_AS_PatternName_nm));
    else
      str += L"-";
    break;
  }
  case TAG_TYPE_AS_MatchVal: {
    TYPE_AS_Expr pat (tp.GetRecord(pos_AS_MatchVal_val));
    switch(pat.GetTag()) {
      case TAG_TYPE_AS_CharLit:
      case TAG_TYPE_AS_BoolLit:
      case TAG_TYPE_AS_QuoteLit:
      case TAG_TYPE_AS_TextLit:
      case TAG_TYPE_AS_RealLit:
      case TAG_TYPE_AS_NumLit:
      case TAG_TYPE_AS_NilLit: {
        // Is this a 'symbolic literal'? Then translate this literal
        str += Type2Ascii(pat);
        break;
      }
      case TAG_TYPE_AS_SetEnumerationExpr:
      case TAG_TYPE_AS_SeqEnumerationExpr: {
        // For some reason MatchVal is used to represent the _empty_
        // SetEnumPattern and SeqEnumPattern.
        str += Type2Ascii(pat);
        break;
      }
      default: {
        str += L"(" + Type2Ascii(pat) + L")";
        break;
      }
    }
    break;
  }
  case TAG_TYPE_AS_SetEnumPattern: {
    str += L"{";
    str += PatternList2Ascii(tp.GetSequence(pos_AS_SetEnumPattern_Elems));
    str += L"}";
    break;
  }
  case TAG_TYPE_AS_SeqEnumPattern: {
    str += L"[";
    str += PatternList2Ascii(tp.GetSequence(pos_AS_SeqEnumPattern_els));
    str += L"]";
    break;
  }
  case TAG_TYPE_AS_MapEnumPattern: {
    str += L"{";
    str += PatternList2Ascii(tp.GetSequence(pos_AS_MapEnumPattern_mls));
    const SEQ<TYPE_AS_MapletPattern> & mls(tp.GetSequence(pos_AS_MapEnumPattern_mls));
    size_t len_mls = mls.Length();
    for (size_t i = 1; i <= len_mls; i++) {
      const TYPE_AS_MapletPattern & mp (mls[i]);
      str += Type2Ascii(mp.GetRecord(pos_AS_MapletPattern_dp));
      str += L" |-> ";
      str += Type2Ascii(mp.GetRecord(pos_AS_MapletPattern_rp));
      if (i < len_mls) {
        str += L", ";
      }
    }
    str += L"}";
    break;
  }
  case TAG_TYPE_AS_SetUnionPattern: {
    str += Type2Ascii(tp.GetRecord(pos_AS_SetUnionPattern_lp));
    str += L" union ";
    str += Type2Ascii(tp.GetRecord(pos_AS_SetUnionPattern_rp));
    break;
  }
  case TAG_TYPE_AS_SeqConcPattern: {
    str += Type2Ascii(tp.GetField(pos_AS_SeqConcPattern_lp));
    str += L" ^ ";
    str += Type2Ascii(tp.GetField(pos_AS_SeqConcPattern_rp));
    break;
  }
  case TAG_TYPE_AS_MapMergePattern: {
    str += Type2Ascii(tp.GetField(pos_AS_MapMergePattern_lp));
    str += L" munion ";
    str += Type2Ascii(tp.GetField(pos_AS_MapMergePattern_rp));
    break;
  }
  case TAG_TYPE_AS_TuplePattern: {
    str += L"mk_(";
    str += PatternList2Ascii(tp.GetSequence(pos_AS_TuplePattern_fields));
    str += L")";
    break;
  }
  case TAG_TYPE_AS_RecordPattern: {
    str += L"mk_";
    str += PrintNameCharSeq(tp.GetRecord(pos_AS_RecordPattern_nm));
    str += L"(";
    str += PatternList2Ascii(tp.GetSequence(pos_AS_RecordPattern_fields));
    str += L")";
    break;
  }
#ifdef VDMPP
  case TAG_TYPE_AS_ObjectPattern: {
    const SEQ<TYPE_AS_FieldPattern> & fields(tp.GetSequence(pos_AS_RecordPattern_fields));
    str += L"obj_";
    str += PrintNameCharSeq(tp.GetRecord(pos_AS_ObjectPattern_cls));
    str += L"(";
    size_t len_fields = fields.Length();
    for (size_t i = 1; i <= len_fields; i++) {
      const TYPE_AS_FieldPattern & field(fields[i]);
      str += PrintNameCharSeq(field.GetRecord(pos_AS_FieldPattern_nm));
      str += L" |-> ";
      str += Type2Ascii(field.GetRecord(pos_AS_FieldPattern_pat));
      if (i < len_fields) {
        str += L", ";
      }
    }
    str += L")";
    break;
  }
#endif // VDMPP
  case TAG_TYPE_AS_SetBind: {
    str += Type2Ascii(tp.GetRecord(pos_AS_SetBind_pat));
    str += L" in set ";
    str += Type2Ascii(tp.GetRecord(pos_AS_SetBind_Set));
    break;
  }
  case TAG_TYPE_AS_TypeBind: {
    str += Type2Ascii(tp.GetRecord(pos_AS_TypeBind_pat));
    str += L" : ";
    str += Type2Ascii(tp.GetRecord(pos_AS_TypeBind_tp));
    break;
  }

    // ###########################
    // ## Tags from AS`Expr  ##  
    // ###########################
  
  case TAG_TYPE_AS_BracketedExpr : {
    str += L"(" + Type2Ascii(tp.GetRecord(pos_AS_BracketedExpr_expr)) + L")";
    break;
  }
  case TAG_TYPE_AS_DefExpr : {
    str += L"def ";
    Sequence tpl_l(tp.GetSequence(pos_AS_DefExpr_Def));
    bool first = true;
    Generic g;
    for(bool bb = tpl_l.First(g); bb; bb = tpl_l.Next(g)) {
      Tuple t(g);
      if(first)
        first = false;
      else
        str += L"; ";
      str += Type2Ascii(t.GetField(1)) + L" = " + Type2Ascii(t.GetField(2));
    }
    break;
  }
  case TAG_TYPE_AS_LetExpr : {
    str += L"let ";
    str += LocalDef2Ascii(tp.GetSequence(pos_AS_LetExpr_localdef));
    str += L" in " + Type2Ascii(tp.GetRecord(pos_AS_LetExpr_body));
    break;
  }
  case TAG_TYPE_AS_ValueDef: {  // Part of the let expression
    str += Type2Ascii(tp.GetRecord(pos_AS_ValueDef_pat));            // Pattern
    if(!tp.GetField(pos_AS_ValueDef_tp).IsNil())
      str += L" : " + Type2Ascii(tp.GetRecord(pos_AS_ValueDef_tp));  // Type
    str += L" = " + Type2Ascii(tp.GetRecord(pos_AS_ValueDef_val));    // Expr
    break;
  }
  case TAG_TYPE_AS_ExplFnDef: {  // Part of the let expression
    str += L"Function definition in let expressions not supported";
    break;
  }
  case TAG_TYPE_AS_ImplFnDef: {  // Part of the let expression
    str += L"Function definition in let expressions not supported";
    break;
  }
  case TAG_TYPE_AS_LetBeSTExpr: {
//    str += L"let " + Type2Ascii(tp.GetRecord(pos_AS_LetBeSTExpr_lhs)); // Bind
    str += L"let " + BindList2Ascii(ASTAUX::BindToBindList(tp.GetRecord(pos_AS_LetBeSTExpr_lhs))); // Bind
    if(!tp.GetField(pos_AS_LetBeSTExpr_St).IsNil())
      str += L" be st " + Type2Ascii(tp.GetRecord(pos_AS_LetBeSTExpr_St));  // 'be st'
    str += Type2Ascii(tp.GetRecord(pos_AS_LetBeSTExpr_In));
    break;
  }
  case TAG_TYPE_AS_IfExpr : {
    str += L"if " + Type2Ascii(tp.GetRecord(pos_AS_IfExpr_test)) + L" then " +
      Type2Ascii(tp.GetRecord(pos_AS_IfExpr_cons));
    Sequence elif_l(tp.GetSequence(pos_AS_IfExpr_elsif));  // sequence of elseif branches
    Generic g;
    for(bool bb = elif_l.First(g); bb; bb = elif_l.Next(g)) {
      TYPE_AS_ElseifExpr r (g);
      str += L"elseif " + Type2Ascii(r.GetRecord(pos_AS_ElseifExpr_test)) + L" then " +
             Type2Ascii(r.GetRecord(pos_AS_ElseifExpr_cons));
    }
    str += L" else " + Type2Ascii(tp.GetRecord(pos_AS_IfExpr_altn)); // else branch
    break;
  }
  case TAG_TYPE_AS_CasesExpr : {
    str += L"cases " + Type2Ascii(tp.GetRecord(pos_AS_CasesExpr_sel)) + L": ";
    Sequence alt_l(tp.GetSequence(pos_AS_CasesExpr_altns));
    bool first = true;
    Generic g;
    for(bool bb = alt_l.First(g); bb; bb = alt_l.Next(g)) {
      TYPE_AS_CaseAltn r (g);
      if(first)
        first = false;
      else
        str += L", ";
      str += PatternList2Ascii(r.GetSequence(pos_AS_CaseAltn_match));
      str += L" -> " + Type2Ascii(r.GetRecord(pos_AS_CaseAltn_body));
    }
    if(!tp.GetField(pos_AS_CasesExpr_Others).IsNil())
      str += L", others -> " + Type2Ascii(tp.GetRecord(pos_AS_CasesExpr_Others));
    str += L" end";
    break;
  }

  case TAG_TYPE_AS_PrefixExpr : { // Part of UnaryExpr
    str += Text(OpMap[Quote(tp.GetField(pos_AS_PrefixExpr_opr))]).GetValue() +
           Type2Ascii(tp.GetRecord(pos_AS_PrefixExpr_arg));
    break;
  }
  case TAG_TYPE_AS_BinaryExpr : {
    str += Type2Ascii(tp.GetField(pos_AS_BinaryExpr_left)) + 
           Text(OpMap[Quote(tp.GetField(pos_AS_BinaryExpr_opr))]).GetValue() +
           Type2Ascii(tp.GetField(pos_AS_BinaryExpr_right));
    break;
  }
  case TAG_TYPE_AS_AllOrExistsExpr : {
    Quote q(tp.GetField(pos_AS_AllOrExistsExpr_quant));
    if(q == Quote(L"ALL"))
      str += L"forall ";
    else
      str += L"exists ";
    str += BindList2Ascii(tp.GetField(pos_AS_AllOrExistsExpr_bind)) + L" & " +
           Type2Ascii(tp.GetField(pos_AS_AllOrExistsExpr_pred));
    break;
  }
  case TAG_TYPE_AS_ExistsUniqueExpr : {
    str += L"exists1 " + Type2Ascii(tp.GetField(pos_AS_ExistsUniqueExpr_bind)) + L" & " +
           Type2Ascii(tp.GetField(pos_AS_ExistsUniqueExpr_pred));
    break;
  }
  case TAG_TYPE_AS_IotaExpr : {
    str += L"iota " + Type2Ascii(tp.GetField(pos_AS_IotaExpr_bind)) + L" & " +
           Type2Ascii(tp.GetField(pos_AS_IotaExpr_pred));
    break;
  }    
  case TAG_TYPE_AS_SetEnumerationExpr : {
    str += L"{" + ExprList2Ascii(tp.GetField(pos_AS_SetEnumerationExpr_els)) + L"}";
    break;
  }
  case TAG_TYPE_AS_SeqEnumerationExpr : {
    str += L"[" + ExprList2Ascii(tp.GetField(pos_AS_SeqEnumerationExpr_els)) + L"]";
    break;
  }
  case TAG_TYPE_AS_SetComprehensionExpr : {
    str += L"{" + Type2Ascii(tp.GetField(pos_AS_SetComprehensionExpr_elem)) + L" | " +
           BindList2Ascii(tp.GetField(pos_AS_SetComprehensionExpr_bind));
    if(!tp.GetField(pos_AS_SetComprehensionExpr_pred).IsNil()) {
      TYPE_AS_Expr r(tp.GetField(pos_AS_SetComprehensionExpr_pred));
      if(! (r.Is(TAG_TYPE_AS_BoolLit) && Bool(r.GetField(pos_AS_BoolLit_val)).GetValue()))
        str += L" & " + Type2Ascii(r);
    }
    str += L"}";
    break;
  }
  case TAG_TYPE_AS_SeqComprehensionExpr : {
    str += L"[" + Type2Ascii(tp.GetField(pos_AS_SeqComprehensionExpr_elem)) + L" | " +
           Type2Ascii(tp.GetField(pos_AS_SeqComprehensionExpr_bind));
    if(!tp.GetField(pos_AS_SeqComprehensionExpr_pred).IsNil()) {
      TYPE_AS_Expr r(tp.GetField(pos_AS_SeqComprehensionExpr_pred));
      if(! (r.Is(TAG_TYPE_AS_BoolLit) && Bool(r.GetField(pos_AS_BoolLit_val)).GetValue()))
        str += L" & " + Type2Ascii(r);
    }
    str += L"]";
    break;
  }
  case TAG_TYPE_AS_SetRangeExpr : {
    str += L"{" + Type2Ascii(tp.GetField(pos_AS_SetRangeExpr_lb)) + L", ..., " + 
           Type2Ascii(tp.GetField(pos_AS_SetRangeExpr_ub)) + L"}";
    break;
  }
  case TAG_TYPE_AS_SubSequenceExpr : {
    str += Type2Ascii(tp.GetField(pos_AS_SubSequenceExpr_sequence)) + L"(" +
           Type2Ascii(tp.GetField(pos_AS_SubSequenceExpr_frompos)) +
           L", ..., " + Type2Ascii(tp.GetField(pos_AS_SubSequenceExpr_topos)) + L")";
    break;
  }
  case TAG_TYPE_AS_SeqModifyMapOverrideExpr : {
    str += Type2Ascii(tp.GetField(pos_AS_SeqModifyMapOverrideExpr_seqmap)) + L" ++ " +
           Type2Ascii(tp.GetField(pos_AS_SeqModifyMapOverrideExpr_mapexp));
    break;
  }
  case TAG_TYPE_AS_MapEnumerationExpr : {
    str += L"{" + MapletList2Ascii(tp.GetField(pos_AS_MapEnumerationExpr_els)) + L"}";
    break;
  }
  case TAG_TYPE_AS_MapComprehensionExpr : {
    str += L"{" + Type2Ascii(tp.GetField(pos_AS_MapComprehensionExpr_elem)) + L" | " + 
           BindList2Ascii(tp.GetField(pos_AS_MapComprehensionExpr_bind));
    if(!tp.GetField(pos_AS_MapComprehensionExpr_pred).IsNil()){
      TYPE_AS_Expr r (tp.GetField(pos_AS_MapComprehensionExpr_pred));
      if(! (r.Is(TAG_TYPE_AS_BoolLit) && Bool(r.GetField(1)).GetValue()))
        str += L" & " + Type2Ascii(r);
    }
    str += L"}";
    break;
  }
  case TAG_TYPE_AS_Maplet : {
    str += Type2Ascii(tp.GetField(pos_AS_Maplet_mapdom)) + L" |-> " +
           Type2Ascii(tp.GetField(pos_AS_Maplet_maprng));    
    break;
  }
  case TAG_TYPE_AS_TupleConstructorExpr : {
    str += L"mk_(";
    str += ExprList2Ascii(tp.GetField(pos_AS_TupleConstructorExpr_fields));
    str += L")";
    break;
  }
  case TAG_TYPE_AS_RecordConstructorExpr : {
    str += L"mk_" + PrintNameCharSeq(tp.GetField(pos_AS_RecordConstructorExpr_tag)) + L"(";
    str += ExprList2Ascii(tp.GetField(pos_AS_RecordConstructorExpr_fields));
    str += L")";
    break;
  }
  case TAG_TYPE_AS_RecordModifierExpr : {
    str += L"mu (" + Type2Ascii(tp.GetField(pos_AS_RecordModifierExpr_rec)) + L", ";
    str += RecordModList2Ascii(tp.GetField(pos_AS_RecordModifierExpr_modifiers)) + L")";
    break;
  }
  case TAG_TYPE_AS_FieldSelectExpr : {
    str += Type2Ascii(tp.GetField(pos_AS_FieldSelectExpr_rec)) + L"." +
           Type2Ascii(tp.GetField(pos_AS_FieldSelectExpr_nm));
    break;
  }
  case TAG_TYPE_AS_ApplyExpr : {
    str += Type2Ascii(tp.GetField(pos_AS_ApplyExpr_fct));
    str += L"(";
    str += ExprList2Ascii(tp.GetField(pos_AS_ApplyExpr_arg));
    str += L")";
    break;
  }
  case TAG_TYPE_AS_IsExpr : {
    str += L"is_" + Type2Ascii(tp.GetField(pos_AS_IsExpr_type));
    str += L"(" + Type2Ascii(tp.GetField(pos_AS_IsExpr_arg)) + L")";
    break;
  }
  case TAG_TYPE_AS_CharLit :  {
    wchar_t ch = tp.GetChar(pos_AS_CharLit_val).GetValue();
    str += L"'";
    str += ch;
    str += L"'";
    break;
  }
  case TAG_TYPE_AS_TextLit : {
    Sequence s(tp.GetField(pos_AS_TextLit_val));
    wstring ss;
    s.GetString(ss);
    str += L"\"" + ss + L"\"";
    break;
  }
  case TAG_TYPE_AS_RealLit : {
    str += tp.GetField(pos_AS_RealLit_val).ascii();
    break;
  }
  case TAG_TYPE_AS_NumLit : {
    str += tp.GetField(pos_AS_NumLit_val).ascii();
    break;
  }
  case TAG_TYPE_AS_BoolLit : {
    str += tp.GetField(pos_AS_BoolLit_val).ascii();
    break;
  }
  case TAG_TYPE_AS_QuoteLit: {
    str += L"<";
    wstring qnm;
    Sequence(tp.GetField(pos_AS_QuoteLit_val)).GetString(qnm);
    str += qnm + L">";
    break;
  }
  
  case TAG_TYPE_AS_NilLit : {
    str += L"nil";
    break;
  }
  case TAG_TYPE_AS_Name : { 
    str += PrintNameCharSeq(tp);
    break;
  }

  case TAG_TYPE_AS_OldName : { 
    TYPE_AS_Name nm;
    nm.Init(tp.GetSequence(pos_AS_OldName_ids), tp.GetInt(pos_AS_OldName_cid));
    str += PrintNameCharSeq(nm);
    str += L"~";
    break;
  }
  case TAG_TYPE_AS_UndefinedExpr : {
    str += L"undefined"; 
    break;
  }
#ifdef VDMPP
  case TAG_TYPE_AS_SelfExpr : {
    break;
  }
  case TAG_TYPE_AS_NewExpr : {
//    str += L"new " + Type2Ascii(tp.GetField(pos_AS_NewExpr_cls)) + L"()";
    str += L"new " + Type2Ascii(tp.GetField(pos_AS_NewExpr_cls));
    str += L"(";
    str += ExprList2Ascii(tp.GetSequence(pos_AS_NewExpr_args));
    str += L")";
    break;
  }
  case TAG_TYPE_AS_IsOfBaseClassExpr : {
    str += L"isofbaseclass(" + Type2Ascii(tp.GetField(pos_AS_IsOfBaseClassExpr_cls)) + L", " +
           Type2Ascii(tp.GetField(pos_AS_IsOfBaseClassExpr_arg)) + L")";
    break;
  }
  case TAG_TYPE_AS_SameBaseClassExpr : {
    str += L"samebaseclass(" + Type2Ascii(tp.GetField(pos_AS_SameBaseClassExpr_expr1)) + L", " +
           Type2Ascii(tp.GetField(pos_AS_SameBaseClassExpr_expr2)) + L")";
    break;
  }
  case TAG_TYPE_AS_SameClassExpr : {
    str += L"sameclass(" + Type2Ascii(tp.GetField(pos_AS_SameClassExpr_expr1)) + L", " +
           Type2Ascii(tp.GetField(pos_AS_SameClassExpr_expr2)) + L")";
    break;
  }
  case TAG_TYPE_AS_IsOfClassExpr : {
    str += L"isofclass(" + Type2Ascii(tp.GetField(pos_AS_IsOfClassExpr_cls)) + L", " +
           Type2Ascii(tp.GetField(pos_AS_IsOfClassExpr_arg)) + L")";
    break;
  }
  case TAG_TYPE_AS_ActExpr : {
    str += L"#act(" + Type2Ascii(tp.GetField(pos_AS_ActExpr_mthd)) + L")";
    break;
  }
  case TAG_TYPE_AS_FinExpr : {
    str += L"#fin(" + Type2Ascii(tp.GetField(pos_AS_FinExpr_mthd)) + L")";
    break;
  }
  case TAG_TYPE_AS_ActiveExpr : {
    str += L"#active(" + Type2Ascii(tp.GetField(pos_AS_ActiveExpr_mthd)) + L")";
    break;
  }
  case TAG_TYPE_AS_WaitingExpr : {
    str += L"#waiting(" + Type2Ascii(tp.GetField(pos_AS_WaitingExpr_mthd)) + L")";
    break;
  }
  case TAG_TYPE_AS_ReqExpr : {
    str += L"#req(" + Type2Ascii(tp.GetField(pos_AS_ReqExpr_mthd)) + L")";
    break;
  }
#endif // VDMPP
  case TAG_TYPE_AS_TokenConstructorExpr : {
    str += L"mk_token(" + Type2Ascii(tp.GetField(pos_AS_TokenConstructorExpr_field)) + L")";
    break;
  }
  case TAG_TYPE_AS_FctTypeInstExpr : {
    str += Type2Ascii(tp.GetField(pos_AS_FctTypeInstExpr_polyfct)) + L"[" +
           List2Ascii(tp.GetField(pos_AS_FctTypeInstExpr_inst)) + L"]";
    break;
  }
  case TAG_TYPE_AS_LambdaExpr : {
    str += L"lambda " + List2Ascii(tp.GetField(pos_AS_LambdaExpr_parm)) + L" & " +
           Type2Ascii(tp.GetField(pos_AS_LambdaExpr_body));
    break;
  }
  // 20060525
  case TAG_TYPE_AS_TypeVar : {
    str += L"@" + Type2Ascii(tp.GetField(pos_AS_TypeVar_name));
    break;
  }
  case TAG_TYPE_AS_TypeJudgementExpr : {
    str += L"is_(" + Type2Ascii(tp.GetField(pos_AS_TypeJudgementExpr_expr)) + L", " +
                     Type2Ascii(tp.GetField(pos_AS_TypeJudgementExpr_type)) + L")";
    break;
  }
  case TAG_TYPE_AS_NarrowExpr : {
    str += L"narrow_(" + Type2Ascii(tp.GetField(pos_AS_NarrowExpr_expr)) + L", " +
                         Type2Ascii(tp.GetField(pos_AS_NarrowExpr_type)) + L")";
    break;
  }
  case TAG_TYPE_AS_CaseAltn: {
    str += List2Ascii(tp.GetSequence(pos_AS_CaseAltn_match)) + L" -> " +
           Type2Ascii(tp.GetRecord(pos_AS_CaseAltn_body));
    break;
  }
  case TAG_TYPE_AS_TupleSelectExpr: {
    str += Type2Ascii(tp.GetRecord(pos_AS_TupleSelectExpr_tuple)) + L".#" +
           Type2Ascii(tp.GetRecord(pos_AS_TupleSelectExpr_no));
    break;
  }
  case TAG_TYPE_AS_Macro: {
    const Int & mtp (tp.GetInt(pos_AS_Macro_tp));
    switch (mtp.GetValue()) {
      case MACRO_FILE:   { str += L"__FILE__"; }
      case MACRO_LINE:   { str += L"__LINE__"; }
      case MACRO_COLUMN: { str += L"__COLUMN__"; }
      case MACRO_CLMOD:  { str += L"__CLMOD__"; }
      case MACRO_FNOP:   { str += L"__FNOP__"; }
    }
    break;
  }
  default: { 
    // RunTimeError(L"Type2Ascii", "Unknown type", tp);
    vdm_log << L"NonSupType: " << tp.ascii() << endl;
    str += L"NonSupType";
    break;
  }
 
  }
  return str;
}


wstring MPP::MiniPP(const Generic & g)
  // Constructs strings containing ASCII pretty print of either
  // AS`Type, AS`Pattern or AS`Expr.
  // For AS`Expr only simple expressions are supported
{
  if(g.IsNil())
    return wstring();
  wstring s;
  if(g.IsRecord())
    s = Type2Ascii(g);
  else
    //    RunTimeError(L"MiniPP", REC_EXPECTED, g);  //
    s = L"Runtime error in MiniPP";
  return s;
}
