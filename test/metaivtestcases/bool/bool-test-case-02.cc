#include "metaiv.h"

void metaivtest()
{
  Bool b1, b2(true);
  bool res;

  res = b1.GetValue();
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;

  res = !b1;
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;

  res = (b1 && b2);
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;

  res = (b1 || b2);
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;

  b1 = b2.mnot();
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;
  
  Bool b3;
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << " ";
  wcout << "b3:" << b3.GetRefCount() << endl;

  b3 = b1.mand(b2);
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << " ";
  wcout << "b3:" << b3.GetRefCount() << endl;

  b3 = b1.mor(b2);
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << " ";
  wcout << "b3:" << b3.GetRefCount() << endl;
}
