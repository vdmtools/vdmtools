#include "contextinfo.h"

#ifdef DEF_PID_Pattern2BindList
TYPE_AS_BindList vdm_PID_Pattern2BindList (const TYPE_AS_Pattern & pat)
{
  return PID::Pattern2BindList(pat);
}
#endif // DEF_PID_Pattern2BindList

#ifdef DEF_PID_Pattern2Pids
type_14AS_PatternNameCS vdm_PID_Pattern2Pids (const TYPE_AS_Pattern & pat)
{
  return PID::Pattern2Pids(pat);
}
#endif // DEF_PID_Pattern2Pids

#ifdef DEF_PID_NameInPattern
Bool vdm_PID_NameInPattern (const TYPE_AS_Name & nm, const TYPE_AS_Pattern & pat)
{
  return Bool(PID::NameInPattern(nm, pat));
}
#endif // DEF_PID_NameInPattern

#ifdef DEF_PID_NameInBindList
Bool vdm_PID_NameInBindList (const TYPE_AS_Name & nm, const TYPE_AS_BindList & binds)
{
  return Bool(PID::NameInBindList(nm, binds));
}
#endif // DEF_PID_NameInBindList

#ifdef DEF_PID_NameInBind
Bool vdm_PID_NameInBind (const TYPE_AS_Name & nm, const TYPE_AS_Bind & bind)
{
  return Bool(PID::NameInBind(nm, bind));
}
#endif // DEF_PID_NameInBind

// Pattern2BindList
// pat : AS`Pattern
// -> AS`BindList
SEQ<TYPE_AS_MultBind> PID::Pattern2BindList (const TYPE_AS_Pattern & pat)
{
  SET<TYPE_AS_PatternName> pid_set (Pattern2Pids(pat));
  SEQ<TYPE_AS_PatternName> pid_seq (SetToSeq(pid_set));

  SEQ<TYPE_AS_MultBind> bd_lst;
  int len_pid_seq = pid_seq.Length();
  for (int index = 1; index <= len_pid_seq; index++)
  {
    const TYPE_AS_PatternName & pn (pid_seq[index]);
    bd_lst.ImpAppend(TYPE_AS_MultTypeBind().Init(SEQ<TYPE_AS_Pattern>().ImpAppend(pn),
                                                 vdm_CI_uAUX_GetASType(pn.GetInt(pos_AS_PatternName_cid)),
                                                 NilContextId));
  }
  return bd_lst;
}

// Pattern2Pids
// pat : AS`Pattern
// -> set of AS`PatternName
SET<TYPE_AS_PatternName> PID::Pattern2Pids (const TYPE_AS_Pattern & pat)
{
  switch(pat.GetTag()) {
    case TAG_TYPE_AS_PatternName:
      return PatternName2Pids(pat);
    case TAG_TYPE_AS_MatchVal:
      return MatchVal2Pids(pat);
    case TAG_TYPE_AS_SetEnumPattern:
      return SetEnumPattern2Pids(pat);
    case TAG_TYPE_AS_SetUnionPattern:
      return SetUnionPattern2Pids(pat);
    case TAG_TYPE_AS_SeqEnumPattern:
      return SeqEnumPattern2Pids(pat);
    case TAG_TYPE_AS_SeqConcPattern:
      return SeqConcPattern2Pids(pat);
    case TAG_TYPE_AS_MapletPattern:
      return MapletPattern2Pids(pat);
    case TAG_TYPE_AS_MapEnumPattern:
      return MapEnumPattern2Pids(pat);
    case TAG_TYPE_AS_MapMergePattern:
      return MapMergePattern2Pids(pat);
    case TAG_TYPE_AS_RecordPattern:
      return RecordPattern2Pids(pat);
    case TAG_TYPE_AS_TuplePattern:
      return TuplePattern2Pids(pat);
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern:
      return ObjectPattern2Pids(pat);
    case TAG_TYPE_AS_FieldPattern:
      return FieldPattern2Pids(pat);
#endif // VDMPP
    default: {
      wcout << L"pattern unknown" << endl;
      return SET<TYPE_AS_PatternName>();
    }
  }
}

// PatternName2Pids
// pat : AS`PatternName
// -> set of AS`PatternName
SET<TYPE_AS_PatternName> PID::PatternName2Pids (const TYPE_AS_PatternName & pat)
{
  SET<TYPE_AS_PatternName> p_s;
  if (!pat.GetField(pos_AS_PatternName_nm).IsNil())
    p_s.Insert(pat);
  return p_s;
}

// MatchVal2Pids
// pat : AS`MatchVal
// -> set of AS`PatternName
SET<TYPE_AS_PatternName> PID::MatchVal2Pids (const TYPE_AS_MatchVal & pat)
{
  return SET<TYPE_AS_PatternName>();
}

// SetEnumPattern2Pids
// pat : AS`SetEnumPattern
// -> set of AS`PatternName
SET<TYPE_AS_PatternName> PID::SetEnumPattern2Pids (const TYPE_AS_SetEnumPattern & pat)
{
  const SEQ<TYPE_AS_Pattern> & els (pat.GetSequence(pos_AS_SetEnumPattern_Elems));
  SET<TYPE_AS_PatternName> p_s;
  int len_els = els.Length();
  for (int index = 1; index <= len_els; index++)
  {
    p_s.ImpUnion(Pattern2Pids(els[index]));
  }
  return p_s;
}

// SetUnionPattern2Pids
// pat : AS`SetUnionPattern
// -> set of AS`PatternName
SET<TYPE_AS_PatternName> PID::SetUnionPattern2Pids (const TYPE_AS_SetUnionPattern & pat)
{
  SET<TYPE_AS_PatternName> p_s;
  p_s.ImpUnion(Pattern2Pids(pat.GetRecord(pos_AS_SetUnionPattern_lp)));
  p_s.ImpUnion(Pattern2Pids(pat.GetRecord(pos_AS_SetUnionPattern_rp)));
  return p_s;
}

// SeqEnumPattern2Pids
// pat : AS`SeqEnumPattern
// -> set of AS`PatternName
SET<TYPE_AS_PatternName> PID::SeqEnumPattern2Pids (const TYPE_AS_SeqEnumPattern & pat)
{
  const SEQ<TYPE_AS_Pattern> & els (pat.GetSequence(pos_AS_SeqEnumPattern_els));
  SET<TYPE_AS_PatternName> p_s;
  int len_els = els.Length();
  for (int index = 1; index <= len_els; index++)
  {
    p_s.ImpUnion(Pattern2Pids(els[index]));
  }
  return p_s;
}

// SeqConcPattern2Pids
// pat : AS`SeqConcPattern
// -> set of AS`PatternName
SET<TYPE_AS_PatternName> PID::SeqConcPattern2Pids (const TYPE_AS_SeqConcPattern & pat)
{
  SET<TYPE_AS_PatternName> p_s;
  p_s.ImpUnion(Pattern2Pids(pat.GetRecord(pos_AS_SeqConcPattern_lp)));
  p_s.ImpUnion(Pattern2Pids(pat.GetRecord(pos_AS_SeqConcPattern_rp)));
  return p_s;
}

// MapletPattern2Pids
// pat : AS`MapletPattern
// -> set of AS`PatternName
SET<TYPE_AS_PatternName> PID::MapletPattern2Pids (const TYPE_AS_MapletPattern & pat)
{
  SET<TYPE_AS_PatternName> p_s;
  p_s.ImpUnion(Pattern2Pids(pat.GetRecord(pos_AS_MapletPattern_dp)));
  p_s.ImpUnion(Pattern2Pids(pat.GetRecord(pos_AS_MapletPattern_rp)));
  return p_s;
}

// MapEnumPattern2Pids
// pat : AS`MapEnumPattern
// -> set of AS`PatternName
SET<TYPE_AS_PatternName> PID::MapEnumPattern2Pids (const TYPE_AS_MapEnumPattern & pat)
{
  const SEQ<TYPE_AS_MapletPattern> & mls (pat.GetSequence(pos_AS_MapEnumPattern_mls));
  SET<TYPE_AS_PatternName> p_s;
  int len_mls = mls.Length();
  for (int index = 1; index <= len_mls; index++)
  {
    p_s.ImpUnion(Pattern2Pids(mls[index]));
  }
  return p_s;
}

// MapMergePattern2Pids
// pat : AS`MapMergePattern
// -> set of AS`PatternName
SET<TYPE_AS_PatternName> PID::MapMergePattern2Pids (const TYPE_AS_MapMergePattern & pat)
{
  SET<TYPE_AS_PatternName> p_s;
  p_s.ImpUnion(Pattern2Pids(pat.GetRecord(pos_AS_MapMergePattern_lp)));
  p_s.ImpUnion(Pattern2Pids(pat.GetRecord(pos_AS_MapMergePattern_rp)));
  return p_s;
}

// RecordPattern2Pids
// pat : AS`RecordPattern
// -> set of AS`PatternName
SET<TYPE_AS_PatternName> PID::RecordPattern2Pids (const TYPE_AS_RecordPattern & pat)
{
  const SEQ<TYPE_AS_Pattern> & els (pat.GetSequence(pos_AS_RecordPattern_fields));
  SET<TYPE_AS_PatternName> p_s;
  int len_els = els.Length();
  for (int index = 1; index <= len_els; index++)
  {
    p_s.ImpUnion(Pattern2Pids(els[index]));
  }
  return p_s;
}

// TuplePattern2Pids
// pat : AS`TuplePattern
// -> set of AS`PatternName
SET<TYPE_AS_PatternName> PID::TuplePattern2Pids (const TYPE_AS_TuplePattern & pat)
{
  const SEQ<TYPE_AS_Pattern> & els (pat.GetSequence(pos_AS_TuplePattern_fields));
  SET<TYPE_AS_PatternName> p_s;
  size_t len_els = els.Length();
  for (size_t index = 1; index <= len_els; index++)
  {
    p_s.ImpUnion(Pattern2Pids(els[index]));
  }
  return p_s;
}

#ifdef VDMPP
// ObjectPattern2Pids
// pat : AS`ObjectPattern
// -> set of AS`PatternName
SET<TYPE_AS_PatternName> PID::ObjectPattern2Pids (const TYPE_AS_ObjectPattern & pat)
{
  const SEQ<TYPE_AS_FieldPattern> & fields (pat.GetSequence(pos_AS_ObjectPattern_fields));
  SET<TYPE_AS_PatternName> p_s;
  size_t len_fields = fields.Length();
  for (size_t index = 1; index <= len_fields; index++)
  {
    p_s.ImpUnion(Pattern2Pids(fields[index]));
  }
  return p_s;
}

// FieldPattern2Pids
// pat : AS`FieldPattern
// -> set of AS`PatternName
SET<TYPE_AS_PatternName> PID::FieldPattern2Pids (const TYPE_AS_FieldPattern & pat)
{
  return Pattern2Pids(pat.GetRecord(pos_AS_FieldPattern_pat));
}
#endif // VDMPP

// NameInPatternBind
// nm : AS`Name
// patbind : AS`PatternBind
// -> bool
//bool PID::NameInPatternBind (const TYPE_AS_Name & nm, const TYPE_AS_PatternBind & patbind)
//{
//  switch(patbind.GetTag()) {
//    case TAG_TYPE_AS_SetBind:
//    case TAG_TYPE_AS_TypeBind:
//      return NameInBind(nm, patbind);
//    default: {
//      return NameInPattern(nm, patbind);
//    }
//  }
//}

// NameInPattern
// nm : AS`Name
// pat : AS`Pattern
// -> bool
bool PID::NameInPattern (const TYPE_AS_Name & nm, const TYPE_AS_Pattern & pat)
{
  switch(pat.GetTag()) {
    case TAG_TYPE_AS_PatternName: {
      return (pat.GetField(pos_AS_PatternName_nm) == nm);
    }
    case TAG_TYPE_AS_MatchVal: {
      return false;
    }
    case TAG_TYPE_AS_SetEnumPattern: {
      return NameInPatternSequence(nm, pat.GetSequence(pos_AS_SetEnumPattern_Elems));
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      SEQ<TYPE_AS_Pattern> p_l;
      p_l.ImpAppend(pat.GetRecord(pos_AS_SetUnionPattern_lp));
      p_l.ImpAppend(pat.GetRecord(pos_AS_SetUnionPattern_rp));
      return NameInPatternSequence(nm, p_l);
    }
    case TAG_TYPE_AS_SeqEnumPattern: {
      return NameInPatternSequence(nm, pat.GetSequence(pos_AS_SeqEnumPattern_els));
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      SEQ<TYPE_AS_Pattern> p_l;
      p_l.ImpAppend(pat.GetRecord(pos_AS_SeqConcPattern_lp));
      p_l.ImpAppend(pat.GetRecord(pos_AS_SeqConcPattern_rp));
      return NameInPatternSequence(nm, p_l);
    }
    case TAG_TYPE_AS_MapletPattern: {
      SEQ<TYPE_AS_Pattern> p_l;
      p_l.ImpAppend(pat.GetRecord(pos_AS_MapletPattern_dp));
      p_l.ImpAppend(pat.GetRecord(pos_AS_MapletPattern_rp));
      return NameInPatternSequence(nm, p_l);
    }
    case TAG_TYPE_AS_MapEnumPattern: {
      return NameInPatternSequence(nm, pat.GetSequence(pos_AS_MapEnumPattern_mls));
    }
    case TAG_TYPE_AS_MapMergePattern: {
      SEQ<TYPE_AS_Pattern> p_l;
      p_l.ImpAppend(pat.GetRecord(pos_AS_MapMergePattern_lp));
      p_l.ImpAppend(pat.GetRecord(pos_AS_MapMergePattern_rp));
      return NameInPatternSequence(nm, p_l);
    }
    case TAG_TYPE_AS_RecordPattern: {
      return NameInPatternSequence(nm, pat.GetSequence(pos_AS_RecordPattern_fields));
    }
    case TAG_TYPE_AS_TuplePattern: {
      return NameInPatternSequence(nm, pat.GetSequence(pos_AS_TuplePattern_fields));
    }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern: {
      return NameInPatternSequence(nm, pat.GetSequence(pos_AS_ObjectPattern_fields));
    }
    case TAG_TYPE_AS_FieldPattern: {
      SEQ<TYPE_AS_Pattern> p_l;
      p_l.ImpAppend(pat.GetRecord(pos_AS_FieldPattern_pat));
      return NameInPatternSequence(nm, p_l);
    }
#endif // VDMPP
    default: {
      wcout << L"unknown pattern" << endl;
      return false;
    }
  }
}

// NameInPatternSequence
// nm : AS`Name
// pats : seq of AS`Pattern
// -> bool
bool PID::NameInPatternSequence (const TYPE_AS_Name & nm, const SEQ<TYPE_AS_Pattern> & pats)
{
  bool exists = false;
  int len_pats = pats.Length();
  for (int index = 1; (index <= len_pats) && ! exists; index++)
    exists = NameInPattern(nm, pats[index]);
  return exists;
}

// NameInBind
// nm : AS`Name
// bind : AS`Bind
// -> bool
bool PID::NameInBind (const TYPE_AS_Name & nm, const TYPE_AS_Bind & bind)
{
  switch(bind.GetTag()) {
    case TAG_TYPE_AS_SetBind:
      return NameInPattern(nm, bind.GetRecord(pos_AS_SetBind_pat));
    case TAG_TYPE_AS_TypeBind:
      return NameInPattern(nm, bind.GetRecord(pos_AS_TypeBind_pat));
    default: {
      wcout << L"unknown bind" << endl;
      return false;
    }
  }
}

// NameInBindList
// nm : AS`Name
// binds : AS`BindList
// -> bool
bool PID::NameInBindList (const TYPE_AS_Name & nm, const SEQ<TYPE_AS_MultBind> & binds)
{
  bool exists = false;
  int len_binds = binds.Length();
  for (int index = 1; (index <= len_binds) && !exists; index++)
    exists = NameInMultBind(nm, binds[index]);
  return exists;
}

// NameInMultBind
// nm : AS`Name
// mbind : AS`MultBind
// -> bool
bool PID::NameInMultBind (const TYPE_AS_Name & nm, const TYPE_AS_MultBind & mbind)
{
  switch(mbind.GetTag()) {
    case TAG_TYPE_AS_MultSetBind:
      return NameInPatternSequence(nm, mbind.GetSequence(pos_AS_MultSetBind_pat));
    case TAG_TYPE_AS_MultTypeBind:
      return NameInPatternSequence(nm, mbind.GetSequence(pos_AS_MultTypeBind_pat));
    default: {
      wcout << L"unknown multi bind" << endl;
      return false;
    }
  }
}

// SetToSeq
// s : set of AS`PatternName
// -> seq of AS`PatternName
SEQ<TYPE_AS_PatternName> PID::SetToSeq (const SET<TYPE_AS_PatternName> & s)
{
  return s.ToSequence();
}

