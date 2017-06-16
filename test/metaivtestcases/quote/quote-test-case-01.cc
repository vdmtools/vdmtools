#include "metaiv.h"

void metaivtest()
{
  Quote q1(L"first");
  wcout << "q1:" << q1.GetRefCount() << endl; // 1
  
  Quote q2;
  wcout << "q1:" << q1.GetRefCount() << " ";  // 1
  wcout << "q2:" << q2.GetRefCount() << endl; // 2 (q2 + default_cache)

  Quote q3(q1);
  wcout << "q1:" << q1.GetRefCount() << " ";  // 2 (q1 + q3)
  wcout << "q2:" << q2.GetRefCount() << " ";  // 2 (q2 + default_cache)
  wcout << "q3:" << q3.GetRefCount() << endl; // 2 (q1 + q3)

  wstring s=L"st";
  Quote q4(s);
  wcout << "q1:" << q1.GetRefCount() << " ";  // 2
  wcout << "q2:" << q2.GetRefCount() << " ";  // 2 (q2 + default_cache)
  wcout << "q3:" << q3.GetRefCount() << " ";  // 2
  wcout << "q4:" << q4.GetRefCount() << endl; // 1

  q2=q4;
  wcout << "q1:" << q1.GetRefCount() << " ";  // 2 (q1 + q3)
  wcout << "q2:" << q2.GetRefCount() << " ";  // 2 (q2 + q4)
  wcout << "q3:" << q3.GetRefCount() << " ";  // 2
  wcout << "q4:" << q4.GetRefCount() << endl; // 2 (q2 + q4)
  
  q3=(wstring)L"three";
  wcout << "q1:" << q1.GetRefCount() << " ";  // 1
  wcout << "q2:" << q2.GetRefCount() << " ";  // 2
  wcout << "q3:" << q3.GetRefCount() << " ";  // 1
  wcout << "q4:" << q4.GetRefCount() << endl; // 2

  Generic g(q3);
  wcout << "q1:" << q1.GetRefCount() << " ";  // 1
  wcout << "q2:" << q2.GetRefCount() << " ";  // 2
  wcout << "q3:" << q3.GetRefCount() << " ";  // 2 (q3 + g)
  wcout << "q4:" << q4.GetRefCount() << endl; // 2
  
  q1=g;
  wcout << "q1:" << q1.GetRefCount() << " ";  // 3 (q1 + q3 + g)
  wcout << "q2:" << q2.GetRefCount() << " ";  // 2
  wcout << "q3:" << q3.GetRefCount() << " ";  // 3 (q1 + q3 + g)
  wcout << "q4:" << q4.GetRefCount() << endl; // 2
  
}
