#include <iostream.h>

#include "metaiv.h"


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
    return sq[1];
  }
  Generic test2(Sequence sq) {
    return sq[1];
  }
  Generic test3(Sequence sq) {
    return sq[1];
  }
  Generic test4(Sequence sq) {
    return sq[1];
  }
  Generic test5(Sequence sq) {
    return sq[1];
  }
  Generic test6(Sequence sq) {
    return sq[1];
  }
  Generic test7(Sequence sq) {
    return sq[1];
  }
  Generic test7b(Sequence sq) {
    return sq[1];
  }
  Generic test8(Sequence sq) {
    return sq[1];
  }
  Generic test9(Sequence sq) {
    return sq[1];
  }
  Generic test10(Sequence sq) {
    return sq[1];
  }
  Generic test11(Sequence sq) {
    return sq[1];
  }
  Generic test12(Sequence sq) {
    return sq[1];
  }
  Generic test13(Sequence sq) {
    return sq[1];
  }
  Generic test14(Sequence sq) {
    return sq[1];
  }
  Generic test15(Sequence sq) {
    return sq[1];
  }
  Generic test16(Sequence sq) {
    return sq[1];
  }
  Generic test17(Sequence sq) {
    return sq[1];
  }
  Generic test18(Sequence sq) {
    return sq[1];
  }
  Generic test19(Sequence sq) {
    return sq[1];
  }
  Generic test20(Sequence sq) {
    return sq[1];
  }
  Generic test21(Sequence sq) {
    return sq[1];
  }
  Generic test22(Sequence sq) {
    return sq[1];
  }
  Generic test24(Sequence sq) {
    return sq[1];
  }
  Generic test25(Sequence sq) {
    return sq[1];
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

