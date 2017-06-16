#include "metaiv.h"

void metaivtest()
{
  Set s1;
  wcout << "s1:" << s1.GetRefCount() << endl;
  
  Set s2(s1);
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << endl;

  Set s3;
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";	
  wcout << "s3:" << s3.GetRefCount() << endl;

  Generic g(s3);
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";	
  wcout << "s3:" << s3.GetRefCount() << endl;
  
  Set s4(g);
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";	
  wcout << "s3:" << s3.GetRefCount() << " ";
  wcout << "s4:" << s4.GetRefCount() << endl;

  s3=s2;
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";	
  wcout << "s3:" << s3.GetRefCount() << " ";
  wcout << "s4:" << s4.GetRefCount() << endl;

  s1=g;
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";	
  wcout << "s3:" << s3.GetRefCount() << " ";
  wcout << "s4:" << s4.GetRefCount() << endl;
  
}
