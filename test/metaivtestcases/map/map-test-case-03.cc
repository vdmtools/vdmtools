#include "metaiv.h"

void metaivtest()
{
  Map m1, m2, m3(m1);
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";
  wcout << "m3:" << m3.GetRefCount() << endl;

  m1.Insert(Real(3.14), Bool(true));
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";
  wcout << "m3:" << m3.GetRefCount() << endl;

  m2.Insert(Int(3.14), Real(3.14));
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";
  wcout << "m3:" << m3.GetRefCount() << endl;

  int size = m3.Size();
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";
  wcout << "m3:" << m3.GetRefCount() << endl;
  
  bool b = m1.IsEmpty();
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";
  wcout << "m3:" << m3.GetRefCount() << endl;

  b = m2.DomExists((Int)3);
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";
  wcout << "m3:" << m3.GetRefCount() << endl;

  Generic g = Real(12.3)=
  b = m1.DomExists((Int)3, g);
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";
  wcout << "m3:" << m3.GetRefCount() << endl;
  
  m1.Clear();
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";
  wcout << "m3:" << m3.GetRefCount() << endl;	
}
  
