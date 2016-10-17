#include "metaiv.h"

void metaivtest()
{
  Char c1('A');
  wcout << "c1:" << c1.GetRefCount() << endl;
  
  Char c2;
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << endl;

  Char c3(c1);
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";	
  wcout << "c3:" << c3.GetRefCount() << endl;

  char ch='b';
  Char c4(ch);
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";	
  wcout << "c3:" << c3.GetRefCount() << " ";
  wcout << "c4:" << c4.GetRefCount() << endl;

  c2=c4;
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";	
  wcout << "c3:" << c3.GetRefCount() << " ";
  wcout << "c4:" << c4.GetRefCount() << endl;
  
  c3=(char)'c';
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";	
  wcout << "c3:" << c3.GetRefCount() << " ";
  wcout << "c4:" << c4.GetRefCount() << endl;

  Generic g(c3);
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";	
  wcout << "c3:" << c3.GetRefCount() << " ";
  wcout << "c4:" << c4.GetRefCount() << endl;
  
  Char c5(g);
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";	
  wcout << "c3:" << c3.GetRefCount() << " ";	
  wcout << "c4:" << c4.GetRefCount() << " ";
  wcout << "c5:" << c5.GetRefCount() << endl;

  c1=g;
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";	
  wcout << "c3:" << c3.GetRefCount() << " ";	
  wcout << "c4:" << c4.GetRefCount() << " ";
  wcout << "c5:" << c5.GetRefCount() << endl;
  
}
