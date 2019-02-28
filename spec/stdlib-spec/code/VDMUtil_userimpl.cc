#ifdef VDMPP
type_typevariableL vdm_VDMUtil::vdm_set2seq (const type_typevariableS & x)
#endif // VDMPP
#ifdef VDMSL
type_typevariableL vdm_VDMUtil_set2seq (const type_typevariableS & x)
#endif // VDMSL
{
  return x.ToSequence();
}

#ifdef VDMPP
Generic vdm_VDMUtil::vdm_get_ufile_upos ()
#endif // VDMPP
#ifdef VDMSL
Generic vdm_VDMUtil_get_ufile_upos ()
#endif // VDMSL
{
  return Nil();
}

#ifdef VDMPP
type_cL vdm_VDMUtil::vdm_val2seq_uof_uchar (const Generic & x)
#endif // VDMPP
#ifdef VDMSL
type_cL vdm_VDMUtil_val2seq_uof_uchar (const Generic & x)
#endif // VDMSL
{
  return Sequence(x.ascii());
}

#ifdef VDMPP
type_bU2P vdm_VDMUtil::vdm_seq_uof_uchar2val (const type_cL & s)
#endif // VDMPP
#ifdef VDMSL
type_bU2P vdm_VDMUtil_seq_uof_uchar2val (const type_cL & s)
#endif // VDMSL
{
  Generic res;
  wstring istr (s.GetString());
  bool ok = CGUTIL::ParseVDMValueString(istr, res);
  
  if (ok) {
    return mk_(Bool(true), res);
  }
  else {
    return mk_(Bool(false), Nil());
  }
}

#ifdef VDMPP
Bool vdm_VDMUtil::vdm_post_seq_uof_uchar2val (const type_cL & x, const type_bU2P & r)
#endif // VDMPP
#ifdef VDMSL
Bool vdm_VDMUtil_post_seq_uof_uchar2val (const type_cL & x, const type_bU2P & r)
#endif // VDMSL
{
  return (r.GetBoolValue(1) || r.GetField(2).IsNil());
}

#ifdef VDMPP
Generic vdm_VDMUtil::vdm_cast (const Generic & g)
#endif // VDMPP
#ifdef VDMSL
Generic vdm_VDMUtil_cast (const Generic & g)
#endif // VDMSL
{
  return g;
}

#ifdef VDMPP
Generic vdm_VDMUtil::vdm_clone (const Generic & g)
{
  // TODO: not yet implemented
  CGUTIL::RunTime(L"Clone isn't implemented");
  return g;
}

Generic vdm_VDMUtil::vdm_classname(const Generic & g)
{
  // TODO: not yet implemented
  CGUTIL::RunTime(L"Classname isn't implemented");
  return Nil();
}
#endif // VDMPP

#ifdef VDMPP
Int vdm_VDMUtil::vdm_char2code (const Char & c)
#endif // VDMPP
#ifdef VDMSL
Int vdm_VDMUtil_char2code (const Char & c)
#endif // VDMSL
{
  return Int((unsigned long)(c.GetValue()));
}

#ifdef VDMPP
Char vdm_VDMUtil::vdm_code2char (const Int & n)
#endif // VDMPP
#ifdef VDMSL
Char vdm_VDMUtil_code2char (const Int & n)
#endif // VDMSL
{
  return Char((wchar_t)(n.GetValue()));
}

#ifdef VDMPP
Int vdm_VDMUtil::vdm_current_utime ()
#endif // VDMPP
#ifdef VDMSL
Int vdm_VDMUtil_current_utime ()
#endif // VDMSL
{
  return CGUTIL::GetCurrentTimeInMillis();
}


