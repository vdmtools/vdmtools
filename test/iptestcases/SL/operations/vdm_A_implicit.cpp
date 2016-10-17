
Int vdm_A_g(const Int& n1, const Int& n2, const type_iS&)
{
  return n1 + n2;
}


void vdm_A_g_q(const Int&, const Int&) {}


type_ii2P vdm_A_h(const type_iL& s, const type_ii2P&)
{
  if (s.Length() != 2) 
    RunTime(L"Error in h");

  {
    Tuple t(2);
    Int i = s[1];
    t.SetField(1, i);
    i = s[2];
    t.SetField(2, i);
    return Generic (t);
  }
}

type_ii2PiL2P vdm_A_h09(const type_iL& s, const type_ii2P&)
{
  if (s.Length() != 2) 
    RunTime(L"Error in h09");

  {
    Tuple t(2);
    Int i = s[1];
    t.SetField(1, i);
    i = s[2];
    t.SetField(2, i);
    return Generic (t);
  }
}

type_iL vdm_A_h2(const type_iS& s)
{
  Sequence res;
  Set s_q (s);
  Generic g;
  for (bool cont=s_q.First(g); cont; cont=s_q.Next(g))
    res.ImpAppend(g);
  return Generic(res);
}

