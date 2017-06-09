#ifdef VDMPP
// to avoid problems with the linker
Int vdm_A::vdm_f1() { return Int(1); }
#endif


#ifdef VDMSL
Int vdm_A_g(const Int& n1, const Int& n2, const type_iS& s)
#endif
#ifdef VDMPP
Int vdm_A::vdm_g(const Int& n1, const Int& n2, const type_iS& )
#endif
{
  return n1 + n2;
}

#ifdef VDMSL
type_ii2P vdm_A_h(const type_iL& s, const type_ii2P& tup)
#endif
#ifdef VDMPP
type_ii2P vdm_A::vdm_h(const type_iL& s, const type_ii2P& tup)
#endif
{
  if (s.Length() != 2) 
    RunTime(L"Error in h");
  return mk_(s[1], s[2]);
}

#ifdef VDMSL
type_iL vdm_A_h2(const type_iS& s)
#endif
#ifdef VDMPP
type_iL vdm_A::vdm_h2(const type_iS& s)
#endif
{
  Sequence res;
  Generic g;
  for (bool cont=s.First(g); cont; cont=s.Next(g))
    res.ImpAppend(g);
  return (Generic) res;
}
