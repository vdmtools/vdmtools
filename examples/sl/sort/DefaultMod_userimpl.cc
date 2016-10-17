/***
*  * WHAT
*  *    Handwritten implementation of `merge sort'
*  * ID
*  *    $Id: DefaultMod_userimpl.cc,v 1.4 2005/05/13 00:41:46 vdmtools Exp $
*  * PROJECT
*  *    Toolbox
*  * COPYRIGHT
*  *    (C) 2016 Kyushu University
***/

static type_rL Merge(type_rL, type_rL);

type_rL vdm_DefaultMod_ImplSort(const type_rL &l) {
  int len = l.Length();
  if (len <= 1)
    return l;
  else {
    int l2 = len/2;
    type_rL l_l, l_r;
    for (int i = 1; i <= l2; i++)
      l_l.ImpAppend(l[i]);
    for (int j = l2 + 1; j <= len; j++)
      l_r.ImpAppend(l[j]);
    return Merge(vdm_DefaultMod_ImplSort(l_l), 
                 vdm_DefaultMod_ImplSort(l_r));
  }
}

type_rL Merge(type_rL l1, type_rL l2)
{
  if (l1.Length() == 0)
    return l2;
  else if (l2.Length() == 0)
    return l1;
  else {
    type_rL res;
    Real e1 = l1.Hd();
    Real e2 = l2.Hd();
    if (e1 <= e2)
      return res.ImpAppend(e1).ImpConc(Merge(l1.ImpTl(), l2));
    else
      return res.ImpAppend(e2).ImpConc(Merge(l1, l2.ImpTl()));
  }
}
