#ifndef PRELIM03
#ifndef PRELIM02
#ifndef PRELIM01
#ifndef PRELIM04

type_ii2P vdm_A::vdm_m (const Int& i, const Int& j)
{
 Tuple t(2);
 t.SetField(1,i).SetField(2,j);
 return (Generic) t;
}

#endif
#endif
#endif
#endif

#ifndef PRELIM03
#ifndef PRELIM05

type_ii2P vdm_A::vdm_m1(const Int& i, const Int& j)
{
  Tuple t(2);
  vdm_i = i;
  vdm_j = j;
  t.SetField(1,vdm_i).SetField(2,vdm_j);
  return (Generic) t;
}
  
type_cc2P vdm_A::vdm_m2(const Char& ch1, const Char& ch2)
{
  Tuple t(2);
  vdm_c = ch1;
  vdm_d = ch2;
  t.SetField(1,vdm_c).SetField(2,vdm_d);
  return (Generic) t;
}

void vdm_A::vdm_m3()
{
  vdm_s.ImpAppend(vdm_i).ImpAppend(vdm_j);
  vdm_s.ImpAppend(vdm_c).ImpAppend(vdm_d);
}
#endif //PRELIM03
#endif





