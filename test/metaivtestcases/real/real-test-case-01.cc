#include "metaiv.h"

void metaivtest()
{
  Real x, y(1), z(3.14);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x, cahce(0.0))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y, cache(1.0))
  wcout << "z:" << z.GetRefCount() << endl; // 1 (z(3.14))
  
  Real u(y);
  wcout << "x:" << x.GetRefCount() << " "; 
  wcout << "y:" << y.GetRefCount() << " ";  // 3 (y, u, cache(1.0))
  wcout << "z:" << z.GetRefCount() << " ";
  wcout << "u:" << u.GetRefCount() << endl; // 3 (y, u, cache(1.0))

  y=z;
  wcout << "x:" << x.GetRefCount() << " ";  // 2
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y, z))
  wcout << "z:" << z.GetRefCount() << " ";  // 2 (y, z))
  wcout << "u:" << u.GetRefCount() << endl; // 2 (u, cache(1.0))
  
  z=3.5;
  wcout << "x:" << x.GetRefCount() << " ";  // 2
  wcout << "y:" << y.GetRefCount() << " ";  // 1
  wcout << "z:" << z.GetRefCount() << " ";  // 1
  wcout << "u:" << u.GetRefCount() << endl; // 2

  x=x;
  wcout << "x:" << x.GetRefCount() << " ";  // 2
  wcout << "y:" << y.GetRefCount() << " ";  // 1
  wcout << "z:" << z.GetRefCount() << " ";  // 1
  wcout << "u:" << u.GetRefCount() << endl; // 2
  
  x=double(3.14);
  wcout << "x:" << x.GetRefCount() << " ";  // 1
  wcout << "y:" << y.GetRefCount() << " ";  // 1
  wcout << "z:" << z.GetRefCount() << " ";  // 1
  wcout << "u:" << u.GetRefCount() << endl; // 2
  
  x=y=z=u;
  wcout << "x:" << x.GetRefCount() << " ";  // 5 (x, y, z, u, cache(1.0))
  wcout << "y:" << y.GetRefCount() << " ";  // 5 (x, y, z, u, cache(1.0))
  wcout << "z:" << z.GetRefCount() << " ";  // 5 (x, y, z, u, cache(1.0))
  wcout << "u:" << u.GetRefCount() << endl; // 5 (x, y, z, u, cache(1.0))
  
  x=Real(3.14);
  wcout << "x:" << x.GetRefCount() << " ";  // 1
  wcout << "y:" << y.GetRefCount() << " ";  // 4 (y, z, u, cache(1.0))
  wcout << "z:" << z.GetRefCount() << " ";  // 4 (y, z, u, cache(1.0))
  wcout << "u:" << u.GetRefCount() << endl; // 4 (y, z, u, cache(1.0))
  
  y=2.72;
  wcout << "x:" << x.GetRefCount() << " ";  // 1
  wcout << "y:" << y.GetRefCount() << " ";  // 1
  wcout << "z:" << z.GetRefCount() << " ";  // 3 (z, u, cache(1.0))
  wcout << "u:" << u.GetRefCount() << endl; // 3 (z, u, cache(1.0))

  Generic g(Real(1.41));
  wcout << "x:" << x.GetRefCount() << " ";  // 1
  wcout << "y:" << y.GetRefCount() << " ";  // 1
  wcout << "z:" << z.GetRefCount() << " ";  // 3
  wcout << "u:" << u.GetRefCount() << endl; // 3

  Real v(g);
  wcout << "x:" << x.GetRefCount() << " ";  // 1
  wcout << "y:" << y.GetRefCount() << " ";  // 1
  wcout << "z:" << z.GetRefCount() << " ";  // 3
  wcout << "u:" << u.GetRefCount() << " ";  // 3
  wcout << "v:" << v.GetRefCount() << endl; // 2

  u=g;
  wcout << "x:" << x.GetRefCount() << " ";  // 1
  wcout << "y:" << y.GetRefCount() << " ";  // 1
  wcout << "z:" << z.GetRefCount() << " ";  // 2
  wcout << "u:" << u.GetRefCount() << " ";  // 3
  wcout << "v:" << v.GetRefCount() << endl; // 3
}
