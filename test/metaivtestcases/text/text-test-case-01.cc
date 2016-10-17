#include "metaiv.h"

void metaivtest()
{
  Text t1(L"first");
  wcout << "t1:" << t1.GetRefCount() << endl;
  
  Text t2;
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << endl; // 2 (t2, default_cache)

  Text t3(t1);
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";	
  wcout << "t3:" << t3.GetRefCount() << endl;

  wstring s=L"st";
  Text t4(s);
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";	
  wcout << "t3:" << t3.GetRefCount() << " ";
  wcout << "t4:" << t4.GetRefCount() << endl;

  t2=t4;
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";	
  wcout << "t3:" << t3.GetRefCount() << " ";
  wcout << "t4:" << t4.GetRefCount() << endl;
  
  t3=(wstring)L"three";
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";	
  wcout << "t3:" << t3.GetRefCount() << " ";
  wcout << "t4:" << t4.GetRefCount() << endl;

  Generic g(t3);
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";	
  wcout << "t3:" << t3.GetRefCount() << " ";
  wcout << "t4:" << t4.GetRefCount() << endl;
  
  Text t5(g);
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";	
  wcout << "t3:" << t3.GetRefCount() << " ";
  wcout << "t4:" << t4.GetRefCount() << " ";
  wcout << "t5:" << t5.GetRefCount() << endl;

  t1=g;
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";	
  wcout << "t3:" << t3.GetRefCount() << " ";
  wcout << "t4:" << t4.GetRefCount() << " ";
  wcout << "t5:" << t5.GetRefCount() << endl;
  
}
