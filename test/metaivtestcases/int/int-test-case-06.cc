#include "metaiv.h"

void metaivtest()
{
  Int x(20), y=3, z(x);
  wcout << "x:" << x.GetRefCount() << " ";  // 3 (x + z + cache(20))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cahce(3))
  wcout << "z:" << z.GetRefCount() << endl; // 3 (x + z + cache(20))

  x=x/Int(2);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(10))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cahce(3))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cahce(20))

  x=x/Real(2);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(5))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cahce(3))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cahce(20))

  z=x/Int(2);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(5))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cahce(3))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cahce(2))
  
  y=x/z;
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(5))
  wcout << "y:" << y.GetRefCount() << " ";  // 3 (y + z + cahce(2))
  wcout << "z:" << z.GetRefCount() << endl; // 3 (y + z + cahce(2))
  
  x=z;
  z=z/x;
  wcout << "x:" << x.GetRefCount() << " ";  // 3 (x + y + cahce(2))
  wcout << "y:" << y.GetRefCount() << " ";  // 3 (x + y + cahce(2))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (x + cache(1))

  y=y/y;
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cahce(2))
  wcout << "y:" << y.GetRefCount() << " ";  // 3 (y + z + cache(1))
  wcout << "z:" << z.GetRefCount() << endl; // 3 (y + z + cache(1))
}
