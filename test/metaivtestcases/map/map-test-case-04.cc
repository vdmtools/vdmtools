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

  Generic g, g1(Real(3.14)), g2;
  bool b = m1.First(g);
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";
  wcout << "m3:" << m3.GetRefCount() << endl;

  b = m1.Next(g);
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";
  wcout << "m3:" << m3.GetRefCount() << endl;

  b = m3.First(g, g1);
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";
  wcout << "m3:" << m3.GetRefCount() << endl;

  b = m1.Next(g, g1);
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";
  wcout << "m3:" << m3.GetRefCount() << endl;
}

