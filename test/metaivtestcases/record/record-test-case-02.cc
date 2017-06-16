#include "metaiv.h"

void metaivtest()
{
  Record r1(1,2), r2(r1), r3;
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << " ";
  wcout << "r3:" << r3.GetRefCount() << endl;

  r1.SetField(1,Bool(true));
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << " ";
  wcout << "r3:" << r3.GetRefCount() << endl;

  r2.SetField(1,Int(3.14));
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << " ";
  wcout << "r3:" << r3.GetRefCount() << endl;

  Generic g = (Bool)r1.GetField(1);
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << " ";
  wcout << "r3:" << r3.GetRefCount() << endl;

  int len = r1.Length();
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << " ";
  wcout << "r3:" << r3.GetRefCount() << endl;	

  int tag = r1.GetTag();
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << " ";
  wcout << "r3:" << r3.GetRefCount() << endl;

  wstring tagstr = r2.GetTextTag(); 
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << " ";
  wcout << "r3:" << r3.GetRefCount() << endl;

  bool b = r1.Is(1);
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << " ";
  wcout << "r3:" << r3.GetRefCount() << endl;

}
  
