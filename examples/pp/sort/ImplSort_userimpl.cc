

static type_iL Merge(const type_iL&, const type_iL&);

type_iL vdm_ImplSort::vdm_ImplSorter(const type_iL& l) {
  int len = l.Length();
  if (len <= 1)
    return l;
  else {
    int l2 = len/2;
    type_iL l_l, l_r;
    int i=1;
    for (; i<=l2; i++)
      l_l.ImpAppend(l[i]);
    for (; i<=len; i++)
      l_r.ImpAppend(l[i]);
    return Merge(vdm_ImplSorter(l_l), vdm_ImplSorter(l_r));
  }
}

type_iL Merge(const type_iL& _l1, const type_iL& _l2)
{
  type_iL l1(_l1), l2(_l2);
  if (l1.IsEmpty())
    return l2;
  else if (l2.IsEmpty())
    return l1;
  else { 
    type_iL res;
    Real e1 = l1.Hd();
    Real e2 = l2.Hd();
    if (e1 <= e2)
      return res.ImpAppend(e1).ImpConc(Merge(l1.ImpTl(), l2));
    else
      return res.ImpAppend(e2).ImpConc(Merge(l1, l2.ImpTl()));
  }
}
