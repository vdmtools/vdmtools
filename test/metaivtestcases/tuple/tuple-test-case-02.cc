#include "metaiv.h"

void metaivtest()
{
  Tuple tup1(2), tup2(tup1);
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << endl;

  tup1.SetField(1, Int(3.14));
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << endl;
  
  tup1.SetField(2, Token());
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << endl;

  Generic g=tup1.GetField(1);
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << endl;
}  
  
