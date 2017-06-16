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

  s3 = s1.ImpUnion(s2);
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";
  wcout << "s3:" << s3.GetRefCount() << endl;

  s3 = s1.ImpIntersect(s2);
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";
  wcout << "s3:" << s3.GetRefCount() << endl;

  s3 = s1.ImpDiff(s2);
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";
  wcout << "s3:" << s3.GetRefCount() << endl;

  bool b = s1.SubSet(s2);
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << " ";
  wcout << "s3:" << s3.GetRefCount() << endl;

}
  
