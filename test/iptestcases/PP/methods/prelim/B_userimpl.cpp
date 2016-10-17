
#ifndef __prelim_04__
#ifndef __prelim_01__

type_ii2P vdm_B::vdm_m1(const Int& i, const Int& j)
{
  vdm_i = i;
  vdm_j = j;
  return mk_(vdm_i, vdm_j);
}

type_cc2P vdm_B::vdm_m2(const Char& ch1, const Char& ch2)
{
  vdm_c = ch1;
  vdm_d = ch2;
  return mk_(vdm_c, vdm_d);
}

void vdm_B::vdm_m3()
{
  vdm_s.ImpAppend(vdm_d).ImpAppend(vdm_c);
  vdm_s.ImpAppend(vdm_j).ImpAppend(vdm_i);
}

#endif //PRELIM01
#endif //PRELIM04
