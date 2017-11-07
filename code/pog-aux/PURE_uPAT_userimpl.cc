
#ifdef DEF_PURE_uPAT_Pattern2PurePattern
type_d7AS_NameCS7AS_NameCS3P vdm_PURE_uPAT_Pattern2PurePattern (const TYPE_AS_Pattern & pat,
                                            const TYPE_PURE_uPAT_Identifiers & ids)
{
  return PUREPAT::Pattern2PurePattern(pat, ids);
}
#endif // DEF_PURE_uPAT_Pattern2PurePattern

#ifdef DEF_PURE_uPAT_BindList2PureBindList
type_dL7AS_NameCS7AS_NameCS3P vdm_PURE_uPAT_BindList2PureBindList (const TYPE_AS_BindList & binds,
                                             const TYPE_PURE_uPAT_Identifiers & ids)
{
  return PUREPAT::BindList2PureBindList(binds, ids);
}
#endif // DEF_PURE_uPAT_BindList2PureBindList

#ifdef DEF_PURE_uPAT_Bind2PureBind
type_d7AS_NameCS7AS_NameCS3P vdm_PURE_uPAT_Bind2PureBind (const TYPE_AS_Bind & bind,
                                           const TYPE_PURE_uPAT_Identifiers & ids)
{
  return PUREPAT::Bind2PureBind(bind, ids);
}
#endif // DEF_PURE_uPAT_Bind2PureBind

// Pattern2PurePatternSeq
// pats : seq of AS`Pattern
// ids : Identifiers
// -> seq of AS`Pattern * Identifiers * Identifiers
Tuple PUREPAT::Pattern2PurePatternSeq (const SEQ<TYPE_AS_Pattern> & pats, const SET<TYPE_AS_Name> & ids)
{
  if (pats.IsEmpty())
    return mk_(SEQ<TYPE_AS_Pattern>(), ids, SET<TYPE_AS_Name>());
  else
  {
    Tuple th (Pattern2PurePattern(pats.Hd(), ids));
    Tuple tt (Pattern2PurePatternSeq(pats.Tl(), th.GetSet(3)));
    
    return mk_(SEQ<TYPE_AS_Pattern>().ImpAppend(th.GetRecord(1)).ImpConc(tt.GetSequence(1)),
               tt.GetSet(2),
               SET<TYPE_AS_Name>().ImpUnion(th.GetSet(3)).ImpUnion(tt.GetSet(3)));
  }
}

// Pattern2PurePattern
// pat : AS`Pattern
// ids : Identifiers
// -> AS`Pattern * Identifiers * Identifiers
Tuple PUREPAT::Pattern2PurePattern (const TYPE_AS_Pattern & pat, const SET<TYPE_AS_Name> & ids)
{
  switch(pat.GetTag()) {
    case TAG_TYPE_AS_PatternName:     { return PatternName2PurePattern(pat, ids); }
    case TAG_TYPE_AS_MatchVal:        { return MatchVal2PurePattern(pat, ids); }
    case TAG_TYPE_AS_SetEnumPattern:  { return SetEnumPattern2PurePattern(pat, ids); }
    case TAG_TYPE_AS_SetUnionPattern: { return SetUnionPattern2PurePattern(pat, ids); }
    case TAG_TYPE_AS_SeqEnumPattern:  { return SeqEnumPattern2PurePattern(pat, ids); }
    case TAG_TYPE_AS_SeqConcPattern:  { return SeqConcPattern2PurePattern(pat, ids); }
    case TAG_TYPE_AS_MapletPattern:   { return MapletPattern2PurePattern(pat, ids); }
    case TAG_TYPE_AS_MapEnumPattern:  { return MapEnumPattern2PurePattern(pat, ids); }
    case TAG_TYPE_AS_MapMergePattern: { return MapMergePattern2PurePattern(pat, ids); }
    case TAG_TYPE_AS_RecordPattern:   { return RecordPattern2PurePattern(pat, ids); }
    case TAG_TYPE_AS_TuplePattern:    { return TuplePattern2PurePattern(pat, ids); }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern:   { return ObjectPattern2PurePattern(pat, ids); }
    case TAG_TYPE_AS_FieldPattern:    { return FieldPattern2PurePattern(pat, ids); }
#endif // VDMPP
    default: {
      wcout << L"unknown pattern" << endl;
      return mk_(pat, ids, SET<TYPE_AS_Name>());
    }
  }
}

// PatternName2PurePattern
// pat : AS`PatternName
// ids : Identifiers
// -> AS`Pattern * Identifiers * Identifiers
Tuple PUREPAT::PatternName2PurePattern (const TYPE_AS_PatternName & pat, const SET<TYPE_AS_Name> & ids)
{
  const Generic & nm (pat.GetField(pos_AS_PatternName_nm));
  if (nm.IsNil())
  {
    TYPE_AS_Name new_nm (FRESH::GetName(SEQ<Char>(L"xx"),ids));
    TYPE_AS_PatternName new_pat (pat);
    new_pat.SetField(pos_AS_PatternName_nm, new_nm);
    return mk_(new_pat,
               SET<TYPE_AS_Name>(ids).Insert(new_nm),
               SET<TYPE_AS_Name>().Insert(new_nm));
  }
  else
    return mk_(pat, ids, SET<TYPE_AS_Name>().Insert(nm));
}

// MatchVal2PurePattern
// pat : AS`PatternName
// ids : Identifiers
// -> AS`Pattern * Identifiers * Identifiers
Tuple PUREPAT::MatchVal2PurePattern (const TYPE_AS_MatchVal & pat, const SET<TYPE_AS_Name> & ids)
{
  return mk_(pat, ids, SET<TYPE_AS_Name>());
}

// SetEnumPattern2PurePattern
// pat : AS`PatternName
// ids : Identifiers
// -> AS`Pattern * Identifiers * Identifiers
Tuple PUREPAT::SetEnumPattern2PurePattern (const TYPE_AS_SetEnumPattern & pat, const SET<TYPE_AS_Name> & ids)
{
  Tuple t (Pattern2PurePatternSeq(pat.GetSequence(pos_AS_SetEnumPattern_Elems), ids));
  return mk_(TYPE_AS_SetEnumPattern().Init(t.GetSequence(1), pat.GetInt(pos_AS_SetEnumPattern_cid)),
             t.GetSet(2),
             t.GetSet(3));
}

// SetUnionPattern2PurePattern
// pat : AS`PatternName
// ids : Identifiers
// -> AS`Pattern * Identifiers * Identifiers
Tuple PUREPAT::SetUnionPattern2PurePattern (const TYPE_AS_SetUnionPattern & pat, const SET<TYPE_AS_Name> & ids)
{
  Tuple tl (Pattern2PurePattern(pat.GetRecord(pos_AS_SetUnionPattern_lp), ids));
  Tuple tr (Pattern2PurePattern(pat.GetRecord(pos_AS_SetUnionPattern_rp), tl.GetSet(2)));
  return mk_(TYPE_AS_SetUnionPattern().Init(tl.GetRecord(1), tr.GetRecord(1),
                                            pat.GetInt(pos_AS_SetUnionPattern_cid)),
             tr.GetSet(2),
             SET<TYPE_AS_Name>(tl.GetSet(3)).ImpUnion(tr.GetSet(3)));
}

// SeqEnumPattern2PurePattern
// pat : AS`PatternName
// ids : Identifiers
// -> AS`Pattern * Identifiers * Identifiers
Tuple PUREPAT::SeqEnumPattern2PurePattern (const TYPE_AS_SeqEnumPattern & pat, const SET<TYPE_AS_Name> & ids)
{
  Tuple t (Pattern2PurePatternSeq(pat.GetSequence(pos_AS_SeqEnumPattern_els), ids));
  return mk_(TYPE_AS_SeqEnumPattern().Init(t.GetSequence(1), pat.GetInt(pos_AS_SeqEnumPattern_cid)),
             t.GetSet(2),
             t.GetSet(3));
}

// SeqConcPattern2PurePattern
// pat : AS`PatternName
// ids : Identifiers
// -> AS`Pattern * Identifiers * Identifiers
Tuple PUREPAT::SeqConcPattern2PurePattern (const TYPE_AS_SeqConcPattern & pat, const SET<TYPE_AS_Name> & ids)
{
  Tuple tl (Pattern2PurePattern(pat.GetRecord(pos_AS_SeqConcPattern_lp), ids));
  Tuple tr (Pattern2PurePattern(pat.GetRecord(pos_AS_SeqConcPattern_rp), tl.GetSet(2)));
  return mk_(TYPE_AS_SeqConcPattern().Init(tl.GetRecord(1), tr.GetRecord(1),
                                            pat.GetInt(pos_AS_SeqConcPattern_cid)),
             tr.GetSet(2),
             SET<TYPE_AS_Name>(tl.GetSet(3)).ImpUnion(tr.GetSet(3)));
}

// MapletPattern2PurePattern
// pat : AS`PatternName
// ids : Identifiers
// -> AS`Pattern * Identifiers * Identifiers
Tuple PUREPAT::MapletPattern2PurePattern (const TYPE_AS_MapletPattern & pat, const SET<TYPE_AS_Name> & ids)
{
  Tuple tl (Pattern2PurePattern(pat.GetRecord(pos_AS_MapletPattern_dp), ids));
  Tuple tr (Pattern2PurePattern(pat.GetRecord(pos_AS_MapletPattern_rp), tl.GetSet(2)));
  return mk_(TYPE_AS_MapletPattern().Init(tl.GetRecord(1), tr.GetRecord(1), pat.GetInt(pos_AS_MapletPattern_cid)),
             tr.GetSet(2),
             SET<TYPE_AS_Name>(tl.GetSet(3)).ImpUnion(tr.GetSet(3)));
}

// MapEnumPattern2PurePattern
// pat : AS`PatternName
// ids : Identifiers
// -> AS`Pattern * Identifiers * Identifiers
Tuple PUREPAT::MapEnumPattern2PurePattern (const TYPE_AS_MapEnumPattern & pat, const SET<TYPE_AS_Name> & ids)
{
  Tuple t (Pattern2PurePatternSeq(pat.GetSequence(pos_AS_MapEnumPattern_mls), ids));
  return mk_(TYPE_AS_MapEnumPattern().Init(t.GetSequence(1), pat.GetInt(pos_AS_MapEnumPattern_cid)),
             t.GetSet(2),
             t.GetSet(3));
}

// MapMergePattern2PurePattern
// pat : AS`PatternName
// ids : Identifiers
// -> AS`Pattern * Identifiers * Identifiers
Tuple PUREPAT::MapMergePattern2PurePattern (const TYPE_AS_MapMergePattern & pat, const SET<TYPE_AS_Name> & ids)
{
  Tuple tl (Pattern2PurePattern(pat.GetRecord(pos_AS_MapMergePattern_lp), ids));
  Tuple tr (Pattern2PurePattern(pat.GetRecord(pos_AS_MapMergePattern_rp), tl.GetSet(2)));
  return mk_(TYPE_AS_MapMergePattern().Init(tl.GetRecord(1), tr.GetRecord(1), pat.GetInt(pos_AS_MapMergePattern_cid)),
             tr.GetSet(2),
             SET<TYPE_AS_Name>(tl.GetSet(3)).ImpUnion(tr.GetSet(3)));
}

// RecordPattern2PurePattern
// pat : AS`PatternName
// ids : Identifiers
// -> AS`Pattern * Identifiers * Identifiers
Tuple PUREPAT::RecordPattern2PurePattern (const TYPE_AS_RecordPattern & pat, const SET<TYPE_AS_Name> & ids)
{
  Tuple t (Pattern2PurePatternSeq(pat.GetSequence(pos_AS_RecordPattern_fields), ids));
  return mk_(TYPE_AS_RecordPattern().Init(pat.GetRecord(pos_AS_RecordPattern_nm),
                                          t.GetSequence(1),
                                          pat.GetInt(pos_AS_RecordPattern_cid)),
             t.GetSet(2),
             t.GetSet(3));
}

// TuplePattern2PurePattern
// pat : AS`PatternName
// ids : Identifiers
// -> AS`Pattern * Identifiers * Identifiers
Tuple PUREPAT::TuplePattern2PurePattern (const TYPE_AS_TuplePattern & pat, const SET<TYPE_AS_Name> & ids)
{
  Tuple t (Pattern2PurePatternSeq(pat.GetSequence(pos_AS_TuplePattern_fields), ids));
  return mk_(TYPE_AS_TuplePattern().Init(t.GetSequence(1), pat.GetInt(pos_AS_TuplePattern_cid)),
             t.GetSet(2),
             t.GetSet(3));
}

#ifdef VDMPP
// ObjectPattern2PurePattern
// pat : AS`PatternName
// ids : Identifiers
// -> AS`Pattern * Identifiers * Identifiers
Tuple PUREPAT::ObjectPattern2PurePattern (const TYPE_AS_ObjectPattern & pat, const SET<TYPE_AS_Name> & ids)
{
  Tuple t (Pattern2PurePatternSeq(pat.GetSequence(pos_AS_ObjectPattern_fields), ids));
  return mk_(TYPE_AS_ObjectPattern().Init(pat.GetRecord(pos_AS_ObjectPattern_cls),
                                          t.GetSequence(1),
                                          pat.GetInt(pos_AS_ObjectPattern_cid)),
             t.GetSet(2),
             t.GetSet(3));
}
// FieldPattern2PurePattern
// pat : AS`PatternName
// ids : Identifiers
// -> AS`Pattern * Identifiers * Identifiers
Tuple PUREPAT::FieldPattern2PurePattern (const TYPE_AS_FieldPattern & pat, const SET<TYPE_AS_Name> & ids)
{
  Tuple t (Pattern2PurePattern(pat.GetRecord(pos_AS_FieldPattern_pat), ids));
  return mk_(TYPE_AS_FieldPattern().Init(pat.GetRecord(pos_AS_FieldPattern_nm),
                                         t.GetRecord(1),
                                         pat.GetInt(pos_AS_FieldPattern_cid)),
             t.GetSet(2),
             t.GetSet(3));
}
#endif // VDMPP

// Bind2PureBind
// bind : AS`Bind
// ids : Identifiers
// -> AS`Bind * Identifiers * Identifiers
Tuple PUREPAT::Bind2PureBind (const TYPE_AS_Bind & bind, const SET<TYPE_AS_Name> & ids)
{
  switch(bind.GetTag()) {
    case TAG_TYPE_AS_SetBind:
      return SetBind2PureSetBind(bind, ids);
    case TAG_TYPE_AS_TypeBind:
      return TypeBind2PureTypeBind(bind, ids);
    default: {
      wcout << L"unknown bind" << endl;
      return mk_(bind, ids, SET<TYPE_AS_Name>());
    }
  }
}

// SetBind2PureSetBind
// bind : AS`SetBind
// ids : Identifiers
// -> AS`SetBind * Identifiers * Identifiers
Tuple PUREPAT::SetBind2PureSetBind (const TYPE_AS_SetBind & bind, const SET<TYPE_AS_Name> & ids)
{
  Tuple t (Pattern2PurePattern(bind.GetRecord(pos_AS_SetBind_pat), ids));
  return mk_(TYPE_AS_SetBind().Init(t.GetRecord(1),
                                    bind.GetRecord(pos_AS_SetBind_Set),
                                    bind.GetInt(pos_AS_SetBind_cid)),
             t.GetSet(2),
             t.GetSet(3));
}

// TypeBind2PureTypeBind
// bind : AS`TypeBind
// ids : Identifiers
// -> AS`TypeBind * Identifiers * Identifiers
Tuple PUREPAT::TypeBind2PureTypeBind (const TYPE_AS_TypeBind & bind, const SET<TYPE_AS_Name> & ids)
{
  Tuple t (Pattern2PurePattern(bind.GetRecord(pos_AS_TypeBind_pat), ids));
  return mk_(TYPE_AS_TypeBind().Init(t.GetRecord(1),
                                    bind.GetRecord(pos_AS_TypeBind_tp),
                                    bind.GetInt(pos_AS_TypeBind_cid)),
             t.GetSet(2),
             t.GetSet(3));
}

// BindList2PureBindList
// binds : AS`BindList
// ids : Identifiers
// -> AS`BindList * Identifiers * Identifiers
Tuple PUREPAT::BindList2PureBindList (const TYPE_AS_BindList & binds, const SET<TYPE_AS_Name> & ids)
{
  if (binds.IsEmpty())
    return mk_(SEQ<TYPE_AS_MultBind>(), ids, SET<TYPE_AS_Name>());
  else
  {
    Tuple th (MultBind2PureMultBind(binds.Hd(), ids));
    Tuple tt (BindList2PureBindList(binds.Tl(), th.GetSet(2)));
    return mk_(SEQ<TYPE_AS_MultBind>().ImpAppend(th.GetRecord(1)).ImpConc(tt.GetSequence(1)),
               tt.GetSet(2),
               SET<TYPE_AS_Name>(th.GetSet(3)).ImpUnion(tt.GetSet(3)));
  }
}

// MultBind2PureMultBind
// mbinds : AS`MultBind
// ids : Identifiers
// -> AS`MultBind * Identifiers * Identifiers
Tuple PUREPAT::MultBind2PureMultBind (const TYPE_AS_MultBind & mbinds, const SET<TYPE_AS_Name> & ids)
{
  switch(mbinds.GetTag()) {
    case TAG_TYPE_AS_MultSetBind:
      return MultSetBind2PureMultSetBind(mbinds, ids);
    case TAG_TYPE_AS_MultTypeBind:
      return MultTypeBind2PureMultTypeBind(mbinds, ids);
    default: {
      wcout << L"unknown multi bind" << endl;
      return mk_(mbinds, ids, SET<TYPE_AS_Name>());
    }
  }
}

// MultSetBind2PureMultSetBind
// bind : AS`MultSetBind
// ids : Identifiers
// -> AS`MultSetBind * Identifiers * Identifiers
Tuple PUREPAT::MultSetBind2PureMultSetBind (const TYPE_AS_MultSetBind & bind, const SET<TYPE_AS_Name> & ids)
{
  Tuple t (Pattern2PurePatternSeq(bind.GetSequence(pos_AS_MultSetBind_pat), ids));
  return mk_(TYPE_AS_MultSetBind().Init(t.GetRecord(1),
                                        bind.GetRecord(pos_AS_MultSetBind_Set),
                                        bind.GetInt(pos_AS_MultSetBind_cid)),
             t.GetSet(2),
             t.GetSet(3));
}

// MultTypeBind2PureMultTypeBind
// bind : AS`MultTypeBind
// ids : Identifiers
// -> AS`MultTypeBind * Identifiers * Identifiers
Tuple PUREPAT::MultTypeBind2PureMultTypeBind (const TYPE_AS_MultTypeBind & bind, const SET<TYPE_AS_Name> & ids)
{
  Tuple t (Pattern2PurePatternSeq(bind.GetSequence(pos_AS_MultTypeBind_pat), ids));
  return mk_(TYPE_AS_MultTypeBind().Init(t.GetRecord(1),
                                         bind.GetRecord(pos_AS_MultTypeBind_tp),
                                         bind.GetInt(pos_AS_MultTypeBind_cid)),
             t.GetSet(2),
             t.GetSet(3));
}


