#include "metaiv.h"

void metaivtest()
{
  Tuple tup1(2);
  wcout << "tup1:" << tup1.GetRefCount() << endl;
  
  Tuple tup2;
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << endl;  // 2 (tup2, default_cache)

  Tuple tup3(tup1);
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";	
  wcout << "tup3:" << tup3.GetRefCount() << endl;
  
  Generic g(tup3);
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";	
  wcout << "tup3:" << tup3.GetRefCount() << endl;

  Tuple tup4(g);
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";	
  wcout << "tup3:" << tup3.GetRefCount() << " ";	
  wcout << "tup4:" << tup4.GetRefCount() << endl;

  tup2=tup1;
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";	
  wcout << "tup3:" << tup3.GetRefCount() << " ";	
  wcout << "tup4:" << tup4.GetRefCount() << endl;
  
  tup1=g;
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";	
  wcout << "tup3:" << tup3.GetRefCount() << " ";	
  wcout << "tup4:" << tup4.GetRefCount() << endl;
}
