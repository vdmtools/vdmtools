#include "metaiv.h"

void metaivtest()
{
  Int x(20), y=3, z(x);
  wcout << "x:" << x.GetRefCount() << " ";  // 3 (x + z + cache(20))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(3))
  wcout << "z:" << z.GetRefCount() << endl; // 3 (x + z + cache(20))

  x=x.Exp(Int(2));
  wcout << "x:" << x.GetRefCount() << " ";  // 1 (x(400))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(3))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(20))

  y=x.Exp(Real(2));
  wcout << "x:" << x.GetRefCount() << " ";  // 1 (x(400))
  wcout << "y:" << y.GetRefCount() << " ";  // 1 (y(160000))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(20))

  z=y.Exp(y);
  wcout << "x:" << x.GetRefCount() << " ";  // 1 (x(400))
  wcout << "y:" << y.GetRefCount() << " ";  // 1 (y(160000))
  wcout << "z:" << z.GetRefCount() << endl; // 1 (z(....))
  
  x=z;
  z=x.Exp(Int(0));
  wcout << "x:" << x.GetRefCount() << " ";  // 1 (x(....))
  wcout << "y:" << y.GetRefCount() << " ";  // 1 (y(160000))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(1))
}
