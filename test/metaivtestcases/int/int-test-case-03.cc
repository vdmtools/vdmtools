#include "metaiv.h"

void metaivtest()
{
  Int x(20), y=3, z(x);
  wcout << "x:" << x.GetRefCount() << " ";  // 3 (x + z + cache(20)
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(3))
  wcout << "z:" << z.GetRefCount() << endl; // 3 (x + z + cache(20)
  
  x=x-Int(2);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(18))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(3))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(20)
  
  x=x-Real(2);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(18))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(3))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(20)

  z=x-Int(4);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(18))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(3))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(14))
  
  y=x-z;
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(18))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(4))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(14))
  
  x=z;
  z=z-x;
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(14))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(4))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(0))
  
  y=y-y;
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(14))
  wcout << "y:" << y.GetRefCount() << " ";  // 3 (y + z + cache(0))
  wcout << "z:" << z.GetRefCount() << endl; // 3 (y + z + cache(0))

  z=-x;
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(14))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(0))
  wcout << "z:" << z.GetRefCount() << endl; // 1 (z(-1))
  
  y=-x;
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(14))
  wcout << "y:" << y.GetRefCount() << " ";  // 1 (y(-1))
  wcout << "z:" << z.GetRefCount() << endl; // 1 (z(-1))
}
