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

  m3 = m1.ImpModify(Real(3.14), Bool(false));
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";
  wcout << "m3:" << m3.GetRefCount() << endl;

  m3.ImpOverride(m2);
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";
  wcout << "m3:" << m3.GetRefCount() << endl;

  Set dom = m1.Dom();
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";
  wcout << "m3:" << m3.GetRefCount() << endl;

  Set rng = m1.Rng();
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";
  wcout << "m3:" << m3.GetRefCount() << endl;

  m3.RemElem(Real(3.14));
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << " ";
  wcout << "m3:" << m3.GetRefCount() << endl;

}
  
