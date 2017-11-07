#include "asquotes.h"

#ifdef DEF_CAST_Pattern2Expr
TYPE_AS_Expr vdm_CAST_Pattern2Expr (const TYPE_AS_Pattern & pat)
{
  return CAST::Pattern2Expr(pat);
}
#endif // DEF_CAST_Pattern2Expr

#ifdef DEF_CAST_PatternName2Expr
TYPE_AS_Expr vdm_CAST_PatternName2Expr (const TYPE_AS_PatternName & pat)
{
  return CAST::PatternName2Expr(pat);
}
#endif // DEF_CAST_PatternName2Expr

// Pattern2Expr
// pat : AS`Pattern
// -> AS`Expr
TYPE_AS_Expr CAST::Pattern2Expr (const TYPE_AS_Pattern & pat)
{
  switch(pat.GetTag()) {
    case TAG_TYPE_AS_PatternName:
      return PatternName2Expr(pat);
    case TAG_TYPE_AS_MatchVal:
      return MatchVal2Expr(pat);
    case TAG_TYPE_AS_SetEnumPattern:
      return SetEnumPattern2Expr(pat);
    case TAG_TYPE_AS_SetUnionPattern:
      return SetUnionPattern2Expr(pat);
    case TAG_TYPE_AS_SeqEnumPattern:
      return SeqEnumPattern2Expr(pat);
    case TAG_TYPE_AS_SeqConcPattern:
      return SeqConcPattern2Expr(pat);
    case TAG_TYPE_AS_MapEnumPattern:
      return MapEnumPattern2Expr(pat);
    case TAG_TYPE_AS_MapMergePattern:
      return MapMergePattern2Expr(pat);
    case TAG_TYPE_AS_RecordPattern:
      return RecordPattern2Expr(pat);
    case TAG_TYPE_AS_TuplePattern:
      return TuplePattern2Expr(pat);
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern:
      return ObjectPattern2Expr(pat);
#endif // VDMPP
    default:
      return TYPE_AS_Expr();
  }
}

// PatternName2Expr
// pat : AS`PatternName
// -> AS`Expr
TYPE_AS_Expr CAST::PatternName2Expr (const TYPE_AS_PatternName & pat)
{
  const Generic & nmg (pat.GetField(pos_AS_PatternName_nm));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_PatternName_cid));
  if (nmg.IsRecord())
  {
    Record nm (nmg);
    switch(nm.GetTag()) {
      case TAG_TYPE_AS_Name: {
        nm.SetField(pos_AS_Name_cid, cid);
        return nm; 
        break;
      }
      case TAG_TYPE_AS_OldName: {
        return TYPE_AS_Name().Init(nm.GetSequence(pos_AS_OldName_ids), cid); 
        break;
      }
      default: {
        break;
      }
    }
  }
  wcout << L"Don't care patterns are not accepted at CAST::PatternName2Expr" << endl;
  return TYPE_AS_Expr();
}

// MatchVal2Expr
// pat : AS`MatchVal
// -> AS`Expr
TYPE_AS_Expr CAST::MatchVal2Expr (const TYPE_AS_MatchVal & pat)
{
  return TYPE_AS_BracketedExpr().Init(pat.GetRecord(pos_AS_MatchVal_val),
                                      pat.GetInt(pos_AS_MatchVal_cid));
}

// SetEnumPattern2Expr
// pat : AS`SetEnumPattern
// -> AS`SetEnumerationExpr
TYPE_AS_SetEnumerationExpr CAST::SetEnumPattern2Expr (const TYPE_AS_SetEnumPattern & pat)
{
  const SEQ<TYPE_AS_Pattern> & els (pat.GetSequence(pos_AS_SetEnumPattern_Elems));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_SetEnumPattern_cid));
  int len_els = els.Length();
  SEQ<TYPE_AS_Expr> expr_els;
  for (int index = 1; index <= len_els; index++)
    expr_els.ImpAppend(Pattern2Expr(els[index]));
  return TYPE_AS_SetEnumerationExpr().Init(expr_els, cid);
}

// SetUnionPattern2Expr
// pat : AS`SetUnionPattern
// -> AS`BinaryExpr
TYPE_AS_BinaryExpr CAST::SetUnionPattern2Expr (const TYPE_AS_SetUnionPattern & pat)
{
  const TYPE_AS_Pattern & lp (pat.GetRecord(pos_AS_SetUnionPattern_lp));
  const TYPE_AS_Pattern & rp (pat.GetRecord(pos_AS_SetUnionPattern_rp));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_SetUnionPattern_cid));
  return TYPE_AS_BinaryExpr().Init(Pattern2Expr(lp),
                                   //Int(SETUNION),
                                   Quote(L"SETUNION"),
                                   Pattern2Expr(rp),
                                   cid);  
}

// SeqEnumPattern2Expr
// pat : AS`SeqEnumPattern
// -> AS`SeqEnumerationExpr
TYPE_AS_SeqEnumerationExpr CAST::SeqEnumPattern2Expr (const TYPE_AS_SeqEnumPattern & pat)
{
  const SEQ<TYPE_AS_Pattern> & els (pat.GetSequence(pos_AS_SeqEnumPattern_els));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_SeqEnumPattern_cid));
  int len_els = els.Length();
  SEQ<TYPE_AS_Expr> expr_els;
  for (int index = 1; index <= len_els; index++)
    expr_els.ImpAppend(Pattern2Expr(els[index]));
  return TYPE_AS_SeqEnumerationExpr().Init(expr_els, cid); 
}

// SeqConcPattern2Expr
// pat : AS`SeqConcPattern
// ->  AS`BinaryExpr
TYPE_AS_BinaryExpr CAST::SeqConcPattern2Expr (const TYPE_AS_SeqConcPattern & pat)
{
  const TYPE_AS_Pattern & lp (pat.GetRecord(pos_AS_SeqConcPattern_lp));
  const TYPE_AS_Pattern & rp (pat.GetRecord(pos_AS_SeqConcPattern_rp));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_SeqConcPattern_cid));
  return TYPE_AS_BinaryExpr().Init(Pattern2Expr(lp),
                                   //Int(SEQCONC),
                                   Quote(L"SEQCONC"),
                                   Pattern2Expr(rp),
                                   cid);  
}

// MapletPattern2Expr
// pat : AS`MapletPattern
// -> AS`Maplet
TYPE_AS_Maplet CAST::MapletPattern2Expr (const TYPE_AS_MapletPattern & pat)
{
  const TYPE_AS_Pattern & dp (pat.GetRecord(pos_AS_MapletPattern_dp));
  const TYPE_AS_Pattern & rp (pat.GetRecord(pos_AS_MapletPattern_rp));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_MapletPattern_cid));
  return TYPE_AS_Maplet().Init(Pattern2Expr(dp), Pattern2Expr(rp), cid);
}

// MapEnumPattern2Expr
// pat : AS`MapEnumPattern
// -> AS`MapEnumerationExpr
TYPE_AS_MapEnumerationExpr CAST::MapEnumPattern2Expr (const TYPE_AS_MapEnumPattern & pat)
{
  const SEQ<TYPE_AS_MapletPattern> & mls (pat.GetSequence(pos_AS_MapEnumPattern_mls));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_MapEnumPattern_cid));
  int len_mls = mls.Length();
  SEQ<TYPE_AS_Maplet> expr_mls;
  for (int index = 1; index <= len_mls; index++) {
    expr_mls.ImpAppend(MapletPattern2Expr(mls[index]));
  }
  return TYPE_AS_MapEnumerationExpr().Init(expr_mls, cid); 
}

// MapMergePattern2Expr
// pat : AS`MapMergePattern
// ->  AS`BinaryExpr
TYPE_AS_BinaryExpr CAST::MapMergePattern2Expr (const TYPE_AS_MapMergePattern & pat)
{
  const TYPE_AS_Pattern & lp (pat.GetRecord(pos_AS_MapMergePattern_lp));
  const TYPE_AS_Pattern & rp (pat.GetRecord(pos_AS_MapMergePattern_rp));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_MapMergePattern_cid));
  return TYPE_AS_BinaryExpr().Init(Pattern2Expr(lp), Quote(L"MAPMERGE"), Pattern2Expr(rp), cid);  
}

// RecordPattern2Expr
// pat : AS`RecordPattern
// -> AS`RecordConstructorExpr
TYPE_AS_RecordConstructorExpr CAST::RecordPattern2Expr (const TYPE_AS_RecordPattern & pat)
{
  const TYPE_AS_Name nm (pat.GetRecord(pos_AS_RecordPattern_nm));
  const SEQ<TYPE_AS_Pattern> & els (pat.GetSequence(pos_AS_RecordPattern_fields));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_RecordPattern_cid));
  int len_els = els.Length();
  SEQ<TYPE_AS_Expr> expr_els;
  for (int index = 1; index <= len_els; index++) {
    expr_els.ImpAppend(Pattern2Expr(els[index]));
  }
  return TYPE_AS_RecordConstructorExpr().Init(nm, expr_els, cid); 
}

// TuplePattern2Expr
// pat : AS`TuplePattern
// -> AS`TupleConstructorExpr
TYPE_AS_TupleConstructorExpr CAST::TuplePattern2Expr (const TYPE_AS_TuplePattern & pat)
{
  const SEQ<TYPE_AS_Pattern> & els (pat.GetSequence(pos_AS_TuplePattern_fields));
  const TYPE_CI_ContextId & cid (pat.GetInt(pos_AS_TuplePattern_cid));
  int len_els = els.Length();
  SEQ<TYPE_AS_Expr> expr_els;
  for (int index = 1; index <= len_els; index++) {
    expr_els.ImpAppend(Pattern2Expr(els[index]));
  }
  return TYPE_AS_TupleConstructorExpr().Init(expr_els, cid); 
}

#ifdef VDMPP
// ObjectPattern2Expr
// pat : AS`ObjectPattern
// -> AS`Expr
TYPE_AS_Expr CAST::ObjectPattern2Expr (const TYPE_AS_ObjectPattern & pat)
{
  return TYPE_AS_Expr();
}
// FieldPattern2Expr
// pat : AS`FieldPattern
// -> AS`Expr
TYPE_AS_Expr CAST::FieldPattern2Expr (const TYPE_AS_FieldPattern & pat)
{
  return TYPE_AS_Expr();
}
#endif // VDMPP
