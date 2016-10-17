
Tuple vdm_B::vdm_m1(const Int& i, const Int& j)
{
  Tuple t(2);
  vdm_i = i;
  vdm_j = j;
  t.SetField(1,vdm_i).SetField(2,vdm_j);
  return t;
}

Tuple vdm_B::vdm_m2(const Char& ch1, const Char& ch2)
{
  Tuple t(2);
  vdm_c = ch1;
  vdm_d = ch2;
  t.SetField(1,vdm_c).SetField(2,vdm_d);
  return t;
}

void vdm_B::vdm_m3()
{
  vdm_s.ImpAppend(vdm_d).ImpAppend(vdm_c);
  vdm_s.ImpAppend(vdm_j).ImpAppend(vdm_i);
}
