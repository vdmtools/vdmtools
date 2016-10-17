#include "metaiv.h"

void metaivtest()
{
  Int x(20), y=3, z(x);
  wcout << "x:" << x.GetRefCount() << " ";  // 3 (x + z + cache(20))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(3))
  wcout << "z:" << z.GetRefCount() << endl; // 3 (x + z + cache(20))

  x=x*Int(2);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(40))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(3))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(20))
  
  x=x*Real(2);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(80))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(3))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(20))

  z=x*Int(0);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(80))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(3))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(0))
  
  y=x*z;
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(80))
  wcout << "y:" << y.GetRefCount() << " ";  // 3 (y + z + cache(0))
  wcout << "z:" << z.GetRefCount() << endl; // 3 (y + z + cache(0))

  x=z;
  z=z*x;
  wcout << "x:" << x.GetRefCount() << " ";  // 4 (x + y + z + cache(0))
  wcout << "y:" << y.GetRefCount() << " ";  // 4 (x + y + z + cache(0))
  wcout << "z:" << z.GetRefCount() << endl; // 4 (x + y + z + cache(0))

  y=y*y;
  wcout << "x:" << x.GetRefCount() << " ";  // 4 (x + y + z + cache(0))
  wcout << "y:" << y.GetRefCount() << " ";  // 4 (x + y + z + cache(0))
  wcout << "z:" << z.GetRefCount() << endl; // 4 (x + y + z + cache(0))
}
