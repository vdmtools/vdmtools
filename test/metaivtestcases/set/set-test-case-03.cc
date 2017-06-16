#include "metaiv.h"

void metaivtest()
{
  Set s1, s2, s3(s1);
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";
  wcout << "s3:" << s3.GetRefCount() << endl;

  s1.Insert(Real(3.14)).Insert(Bool(true));
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";
  wcout << "s3:" << s3.GetRefCount() << endl;

  s2.Insert(Int(3.14)).Insert(Real(3.14));
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";
  wcout << "s3:" << s3.GetRefCount() << endl;

  int card = s3.Card();
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";
  wcout << "s3:" << s3.GetRefCount() << endl;
  
  bool b = s1.IsEmpty();
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";
  wcout << "s3:" << s3.GetRefCount() << endl;

  b = s2.InSet((Int)3);
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";
  wcout << "s3:" << s3.GetRefCount() << endl;
  
  Generic g = s2.GetElem();
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";
  wcout << "s3:" << s3.GetRefCount() << endl;

  s3 = s1;
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";
  wcout << "s3:" << s3.GetRefCount() << endl;

  s1.RemElem(Bool(true));
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";
  wcout << "s3:" << s3.GetRefCount() << endl;

  s2.Clear();
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";
  wcout << "s3:" << s3.GetRefCount() << endl;	
  
  int i = s1.First(g);
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";
  wcout << "s3:" << s3.GetRefCount() << endl;

 i = s1.Next(g);
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";
  wcout << "s3:" << s3.GetRefCount() << endl;
}
  
