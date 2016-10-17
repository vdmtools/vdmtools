#include "metaiv.h"

void metaivtest()
{
  Nil n1;
  wcout << "n1:" << n1.GetRefCount() << endl;
  
  Nil n2(n1);
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << endl;
  
  Nil n3;
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << " ";
  wcout << "n3:" << n3.GetRefCount() << endl;
  
  n3=n1;
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << " ";
  wcout << "n3:" << n3.GetRefCount() << endl;

  Generic g(n1); //different results because of the caching
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << " ";
  wcout << "n3:" << n3.GetRefCount() << endl;

  Nil n4(g);
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << " ";
  wcout << "n3:" << n3.GetRefCount() << " ";
  wcout << "n4:" << n4.GetRefCount() << endl;	

  n3=g;
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << " ";
  wcout << "n3:" << n3.GetRefCount() << " ";
  wcout << "n4:" << n4.GetRefCount() << endl;	
}
  
