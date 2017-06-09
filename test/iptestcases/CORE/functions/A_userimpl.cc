#ifdef VDMSL
type_ii2P vdm_A_h(const type_iL &, const type_ii2P &s)
#endif
#ifdef VDMPP
type_ii2P vdm_A::vdm_h(const type_iL &, const type_ii2P &s)
#endif
{
  return s;
}


#ifdef VDMSL
Int vdm_A_f1()
#endif //VDMSL
#ifdef VDMPP
Int vdm_A::vdm_f1()
#endif //VDMPP
{
  return 10;
}

#ifdef VDMSL
Int vdm_A_g(const Int &a, const Int &b, const type_iS &)
#endif //VDMSL
#ifdef VDMPP
Int vdm_A::vdm_g(const Int &a, const Int &b, const type_iS &)
#endif //VDMPP
{
  return a+b;
}


#ifdef VDMSL
type_iL vdm_A_h2(const type_iS &s)
#endif //VDMSL
#ifdef VDMPP
type_iL vdm_A::vdm_h2(const type_iS &s)
#endif //VDMPP
{
  Set s1(s);
  Sequence res;
  Generic g;
  for (bool bb = s1.First(g); bb; bb = s1.Next(g) ) {
    res.ImpAppend(g);
  }
  return (Generic) res;
}
