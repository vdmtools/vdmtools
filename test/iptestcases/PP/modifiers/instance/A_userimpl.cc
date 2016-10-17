#ifdef VDMSL
Int vdm_A_g1 (const Int & n)
#endif // VDMSL
#ifdef VDMPP
Int vdm_A::vdm_g1 (const Int & n)
#endif // VDMPP
{
  return n + Int(1);
}

#ifdef VDMSL
Int vdm_A_g2 (const Int & n)
#endif // VDMSL
#ifdef VDMPP
Int vdm_A::vdm_g2 (const Int & n)
#endif // VDMPP
{
  return n + Int(1);
}

#ifdef VDMSL
Int vdm_A_g3 (const Int & n)
#endif // VDMSL
#ifdef VDMPP
Int vdm_A::vdm_g3 (const Int & n)
#endif // VDMPP
{
  return n + Int(1);
}

#ifdef VDMSL
Int vdm_A_imp1 (const Int & rr)
#endif // VDMSL
#ifdef VDMPP
Int vdm_A::vdm_imp1 (const Int & rr)
#endif // VDMPP
{
  return rr + vdm_n;
}

#ifdef VDMSL
Int vdm_A_imp2 (const Int & rr)
#endif // VDMSL
#ifdef VDMPP
Int vdm_A::vdm_imp2 (const Int & rr)
#endif // VDMPP
{
  return rr + vdm_n;
}

#ifdef VDMSL
Int vdm_A_imp3 (const Int & rr)
#endif // VDMSL
#ifdef VDMPP
Int vdm_A::vdm_imp3 (const Int & rr)
#endif // VDMPP
{
  return rr + vdm_n;
}
