#include "metaiv.h"

void metaivtest()
{
  Map m1;
  wcout << "m1:" << m1.GetRefCount() << endl;
  
  Map m2(m1);
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << endl;

  Map m3;
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";	
  wcout << "m3:" << m3.GetRefCount() << endl;

  Generic g(m3);
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";	
  wcout << "m3:" << m3.GetRefCount() << endl;
  
  Map m4(g);
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";	
  wcout << "m3:" << m3.GetRefCount() << " ";
  wcout << "m4:" << m4.GetRefCount() << endl;

  m3=m2;
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";	
  wcout << "m3:" << m3.GetRefCount() << " ";
  wcout << "m4:" << m4.GetRefCount() << endl;

  m1=g;
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";	
  wcout << "m3:" << m3.GetRefCount() << " ";
  wcout << "m4:" << m4.GetRefCount() << endl;
  
}
