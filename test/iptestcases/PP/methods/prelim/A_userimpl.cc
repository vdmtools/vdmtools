#if defined(__prelim_01__) || defined(__prelim_02__) || defined(__prelim_04__)
type_ii2P vdm_A::vdm_m1(const Int& i, const Int& j)
{
  vdm_i = i;
  vdm_j = j;
  return mk_(vdm_i, vdm_j);
}
  
type_cc2P vdm_A::vdm_m2(const Char& ch1, const Char& ch2)
{
  vdm_c = ch1;
  vdm_d = ch2;
  return mk_(vdm_c, vdm_d);
}

void vdm_A::vdm_m3()
{
  vdm_s.ImpAppend(vdm_i).ImpAppend(vdm_j);
  vdm_s.ImpAppend(vdm_c).ImpAppend(vdm_d);
}
#endif

#if defined(__prelim_05__) || defined(__prelim_06__)
type_ii2P vdm_A::vdm_m (const Int& i, const Int& j)
{
  return mk_(i, j);
}
#endif

#ifdef __prelim_06__
type_ii2P vdm_A::vdm_G (const Int & i, const Int & j)
{
  return mk_(i, j);
}
#endif // __prelim_06__

