
Int vdm_A_g(const Int& n1, const Int& n2, const type_iS&)
{
  return n1 + n2;
}


void vdm_A_g_q(const Int&, const Int&) {}


type_ii2P vdm_A_h(const type_iL& s, const type_ii2P&)
{
  if (s.Length() != 2) 
    RunTime(L"Error in h");
  return mk_(s[1], s[2]);
}

type_ii2PiL2P vdm_A_h09(const type_iL& s, const type_ii2P&)
{
  if (s.Length() != 2) 
    RunTime(L"Error in h09");
  return mk_(s[1], s[2]);
}

type_iL vdm_A_h2(const type_iS& s)
{
  Set s1 (s);
  Sequence res;
  Generic g;
  for (bool cont = s1.First(g); cont; cont = s1.Next(g))
    res.ImpAppend(g);
  return Generic(res);
}

