#include "metaiv.h"

void metaivtest()
{
  Record r1(1, 4);
  wcout << "r1:" << r1.GetRefCount() << endl;
  
  Record r2(r1);
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << endl;

  Record r3;
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << " ";	
  wcout << "r3:" << r3.GetRefCount() << endl;

  Generic g(r3);
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << " ";	
  wcout << "r3:" << r3.GetRefCount() << endl;
  
  Record r4(g);
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << " ";	
  wcout << "r3:" << r3.GetRefCount() << " ";
  wcout << "r4:" << r4.GetRefCount() << endl;

  r3=r2;
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << " ";	
  wcout << "r3:" << r3.GetRefCount() << " ";
  wcout << "r4:" << r4.GetRefCount() << endl;

  r1=g;
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << " ";	
  wcout << "r3:" << r3.GetRefCount() << " ";
  wcout << "r4:" << r4.GetRefCount() << endl;
  
}
