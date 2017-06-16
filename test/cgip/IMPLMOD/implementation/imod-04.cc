#include <iostream.h>

#include "metaiv.h"
#include "cg_aux.h"

extern "C" {
  void InitDLModule(bool init);
  Generic test1(Sequence sq);
  Generic test2(Sequence sq);
  Generic test3(Sequence sq);
  Generic test4(Sequence sq);
  Generic test5(Sequence sq);
  Generic test6(Sequence sq);
  Generic test7(Sequence sq);
  Generic test7b(Sequence sq);
  Generic test8(Sequence sq);
  Generic test9(Sequence sq);
  Generic test10(Sequence sq);
  Generic test11(Sequence sq);
  Generic test12(Sequence sq);
  Generic test13(Sequence sq);
  Generic test14(Sequence sq);
  Generic test15(Sequence sq);
  Generic test16(Sequence sq);
  Generic test17(Sequence sq);
  Generic test18(Sequence sq);
  Generic test19(Sequence sq);
  Generic test20(Sequence sq);
  Generic test21(Sequence sq);
  Generic test22(Sequence sq);
  Generic test23(Sequence sq);
  Generic test24(Sequence sq);
  Generic test25(Sequence sq);
  Generic test26(Sequence sq);
  Generic test27(Sequence sq);
  
}

  const int tag_A_A = 1;
  const int size_A_A = 2;
  const int tag_A_B = 2;
  const int size_A_B = 1;

void InitDLModule(bool init)
{
  cout << "InitDLModule\n";
  if (init) {
    VDMGetDefaultRecInfoMap().NewTag(tag_A_A,size_A_A);
    VDMGetDefaultRecInfoMap().SetSymTag(tag_A_A,"A`A");
    VDMGetDefaultRecInfoMap().NewTag(tag_A_B,size_A_B);
    VDMGetDefaultRecInfoMap().SetSymTag(tag_A_B,"A`B");
  }
}

  Generic test1(Sequence sq) {
    Bool b0 (sq[1]);
    Bool b (b0.GetValue());
    return b;
  }
  Generic test2(Sequence sq) {
    Int i0 (sq[1]);
    Int i (i0.GetValue());
    return i;
  }
  Generic test3(Sequence sq) {
    Int i (sq[1]);
    return i;
  }
  Generic test4(Sequence sq) {
    Int i (sq[1]);
    return i;
  }
  Generic test5(Sequence sq) {
    Real i0 (sq[1]);
    Real i (i0.GetValue());
    return i;
  }
  Generic test6(Sequence sq) {
    Real i0 (sq[1]);
    Real i (i0.GetValue());
    return i;
  }
  Generic test7(Sequence sq) {
    Char c0 (sq[1]);
    Char c (c0.GetValue());
    return c;
  }
  Generic test7b(Sequence sq) {
    Record c0 (sq[1]);
    Record t (TOKEN,1);
    t.SetField(1,c0.GetField(1));
    return t;
  }
  Generic test8(Sequence sq) {
    Set s;
    Set ss (sq[1]);
    Generic g;
    for (int ii=ss.First(g);ii;ii=ss.Next(g)) {
      Int ni (Int(g).GetValue());
      s.Insert(ni);
    }
    return s;
  }
  Generic test9(Sequence sq) {
    Set ss (sq[1]);
    Set s;
    Generic g;
    for (int ii=ss.First(g);ii;ii=ss.Next(g)) {
      Generic gg;
      Set sg(g);
      Set sr;
      for (int jj=sg.First(gg);jj;jj=sg.Next(gg)) {
	Int ni (Int(gg).GetValue());
	sr.Insert(ni);
      }
      s.Insert(sr);
    }
    return s;
  }
  Generic test10(Sequence sq) {
    Set s;
    Set ss (sq[1]);
    Generic g;
    for (int ii=ss.First(g);ii;ii=ss.Next(g)) {
      if (g.IsChar()) {
	Char c (Char(g).GetValue());
	s.Insert(c);
      }
      else {
	Int ni (Int(g).GetValue());
	s.Insert(ni);
      }
    }
    return s;
  }
  Generic test11(Sequence sq) {
    Sequence ss (sq[1]);
    Sequence s;
    Generic g;
    for (int ii=ss.First(g);ii;ii=ss.Next(g)) {
      Int ni (Int(g).GetValue());
      s.ImpAppend(ni);
    }
    return s;
  }
  Generic test12(Sequence sq) {
    Sequence ss (sq[1]);
    Sequence s;
    Generic g;
    for (int ii=ss.First(g);ii;ii=ss.Next(g)) {
      Generic gg;
      Sequence sg(g);
      Sequence sr;
      for (int jj=sg.First(gg);jj;jj=sg.Next(gg)) {
	Int ni (Int(gg).GetValue());
	sr.ImpAppend(ni);
      }
      s.ImpAppend(sr);
    }
    return s;
  }
  Generic test13(Sequence sq) {
    Sequence ss (sq[1]);
    Sequence s;
    Generic g;
    for (int ii=ss.First(g);ii;ii=ss.Next(g)) {
      if (g.IsChar()) {
	Char c (Char(g).GetValue());
	s.ImpAppend(c);
      }
      else {
	Int ni (Int(g).GetValue());
	s.ImpAppend(ni);
      }
    }
    return s;
  }
  Generic test14(Sequence sq) {
    Map ss (sq[1]);
    Map s;
    Generic g;
    for (int ii=ss.First(g);ii;ii=ss.Next(g)) {
      Int ni (Int(g).GetValue());
      Int mi (ss[ni]);
      s.Insert(ni,mi);
    }
    return s;
  }
  Generic test15(Sequence sq) {
    Map ss (sq[1]);
    Map s;
    Generic g;
    for (int ii=ss.First(g);ii;ii=ss.Next(g)) {
      Generic gg;
      Set sg(g);
      Set sr;
      for (int jj=sg.First(gg);jj;jj=sg.Next(gg)) {
	Int ni (Int(gg).GetValue());
	sr.Insert(ni);
      }
      Int sa (ss[sr]);
      s.Insert(sr,sa);
    }
    return s;
  }
  Generic test16(Sequence sq) {
    Map s;
    Map ss (sq[1]);
    Generic g;
    for (int ii=ss.First(g);ii;ii=ss.Next(g)) {
      if (g.IsChar()) {
	Char c (Char(g).GetValue());
	Generic cm (ss[c]);
	s.Insert(c,cm);
      }
      else {
	Int ni (Int(g).GetValue());
	Generic nm (ss[ni]);
	s.Insert(ni,nm);
      }
    }
    return s;
  }
  Generic test17(Sequence sq) {
    Record r (sq[1]);
    Int x (r.GetField(1));
    Bool y (r.GetField(2));
    Int nx (x.GetValue());
    Bool ny (y.GetValue());
    Record res (tag_A_A,size_A_A);
    res.SetField (1,nx);
    res.SetField (2,ny);
    return res;
  }
  Generic test18(Sequence sq) {
    Record res (sq[1]);
    return res;
  }
  Generic test19(Sequence sq) {
    Sequence l (sq[1]);
    Sequence res;
    Generic g;
    for (int ii=l.First(g);ii;ii=l.Next(g)) {
      Record r(g);
      res.ImpAppend(r);
    }
    return res;
  }
  Generic test20(Sequence sq) {
    Generic g (sq[1]);
    if (g.IsNil())
      return Nil();
    else
      return g;
  }
  Generic test21(Sequence sq) {
    Generic g (sq[1]);
    if (g.IsNil())
      return Nil();
    else
      return g;
  }
  Generic test22(Sequence sq) {
    Tuple t0 (sq[1]);
    Tuple t (2);
    t.SetField (1,t0.GetField(1));
    t.SetField (2,t0.GetField(2));
    return t;
  }
  Generic test24(Sequence sq) {
    Tuple t0 (sq[1]);
    Tuple t (2);
    t.SetField (1,t0.GetField(1));
    t.SetField (2,t0.GetField(2));
    return t;
  }
  Generic test25(Sequence sq) {
    Quote q0 (sq[1]);
    Quote q (q0.GetValue());
    return q;
  }
  Generic test26(Sequence sq) {
    Sequence txt(sq[1]);
    Generic ch;
    string text;
    for (int bb=txt.First(ch);bb;bb=txt.Next(ch)) {
      text += ((Char) ch).GetValue();
    }
    Text res(text);
    return res;
  }

  Generic test27(Sequence sq) {
    Tuple res(sq.Length());
    Generic elm;
    for (int i=1;i<=sq.Length();i++) {
      res.SetField(i,sq[i]);
    }
    return res;
  }

