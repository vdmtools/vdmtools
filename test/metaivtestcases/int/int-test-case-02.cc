//arithmetic operators testing for class Int
#include "metaiv.h"

void metaivtest()
{
  Int x(20), y=3, z(x);
  wcout << "x:" << x.GetRefCount() << " ";  // 3 (x + z + cache(20))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(3))
  wcout << "z:" << z.GetRefCount() << endl; // 3 (x + z + cache(20))

  x=x+Int(2);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(22))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(3))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(20))
  
  x=x+Real(2);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(24))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(3))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(20))
  
  z=x+Int(4);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(24))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(3))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(28))
  
  y=x+z;
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(24))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(52))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(28))

  x=z;
  z=z+x;
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(28))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(52))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(56))
  
  y=y+y;
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(28))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(104))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(56))
}
