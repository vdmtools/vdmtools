#include "metaiv.h"

void metaivtest()
{
  Bool b1, b2(true);
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;
  
  Bool b3(b1);
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << " ";
  wcout << "b3:" << b3.GetRefCount() << endl;
  
  b2 = b1;
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << " ";
  wcout << "b3:" << b3.GetRefCount() << endl;

  b3 = true;
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << " ";
  wcout << "b3:" << b3.GetRefCount() << endl;	

  
  Generic g(Bool(true)); //different results because of the caching
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << " ";
  wcout << "b3:" << b3.GetRefCount() << endl;

  Bool b4(g);
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << " ";
  wcout << "b3:" << b3.GetRefCount() << " ";
  wcout << "b4:" << b4.GetRefCount() << endl;

  b3 = g; //b3 is true
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << " ";
  wcout << "b3:" << b3.GetRefCount() << " ";
  wcout << "b4:" << b4.GetRefCount() << endl;

  b1 = g; //b1 is false
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << " ";
  wcout << "b3:" << b3.GetRefCount() << " ";
  wcout << "b4:" << b4.GetRefCount() << endl;
}
  
