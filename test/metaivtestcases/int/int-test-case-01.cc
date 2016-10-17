//Refernce counting class Int
#include "metaiv.h"

void metaivtest()
{
  Int x, y(1), z(3.14);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(0))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(1))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(3))
  
  Int u(y);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(0))
  wcout << "y:" << y.GetRefCount() << " ";  // 3 (y + u + cache(1))
  wcout << "z:" << z.GetRefCount() << " ";  // 2 (z + cache(3))
  wcout << "u:" << u.GetRefCount() << endl; // 3 (y + u + cache(1))

  y = z;
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(0))
  wcout << "y:" << y.GetRefCount() << " ";  // 3 (y + z + cache(3))
  wcout << "z:" << z.GetRefCount() << " ";  // 3 (y + z + cache(3))
  wcout << "u:" << u.GetRefCount() << endl; // 2 (u + cache(1))
  
  z = 3;
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(0))
  wcout << "y:" << y.GetRefCount() << " ";  // 3 (y + z + cache(3))
  wcout << "z:" << z.GetRefCount() << " ";  // 3 (y + z + cache(3))
  wcout << "u:" << u.GetRefCount() << endl; // 2 (u + cache(1))
  
  x = x;
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(0))
  wcout << "y:" << y.GetRefCount() << " ";  // 3 (y + z + cache(3))
  wcout << "z:" << z.GetRefCount() << " ";  // 3 (y + z + cache(3))
  wcout << "u:" << u.GetRefCount() << endl; // 2 (u + cache(1))
  
  int i = int(x);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(0))
  wcout << "y:" << y.GetRefCount() << " ";  // 3 (y + z + cache(3))
  wcout << "z:" << z.GetRefCount() << " ";  // 3 (y + z + cache(3))
  wcout << "u:" << u.GetRefCount() << endl; // 2 (u + cache(1))
  
  x = y = z = u;
  wcout << "x:" << x.GetRefCount() << " ";  // 5 (x + y + z + u + cache(1))
  wcout << "y:" << y.GetRefCount() << " ";  // 5 (x + y + z + u + cache(1))
  wcout << "z:" << z.GetRefCount() << " ";  // 5 (x + y + z + u + cache(1))
  wcout << "u:" << u.GetRefCount() << endl; // 5 (x + y + z + u + cache(1))
  
  x = Real(3.14);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(3))
  wcout << "y:" << y.GetRefCount() << " ";  // 4 (y + z + u + cache(1))
  wcout << "z:" << z.GetRefCount() << " ";  // 4 (y + z + u + cache(1))
  wcout << "u:" << u.GetRefCount() << endl; // 4 (y + z + u + cache(1))
  
  y = 2.72;
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(3))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(2))
  wcout << "z:" << z.GetRefCount() << " ";  // 3 (z + u + cache(1))
  wcout << "u:" << u.GetRefCount() << endl; // 3 (z + u + cache(1))

  Generic g(Int(1.41));
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(3))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(2))
  wcout << "z:" << z.GetRefCount() << " ";  // 4 (z + u + g + cache(1))
  wcout << "u:" << u.GetRefCount() << endl; // 4 (z + u + g + cache(1))

  Int v(g);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(3))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(2))
  wcout << "z:" << z.GetRefCount() << " ";  // 5 (z + u + g + v + cache(1))
  wcout << "u:" << u.GetRefCount() << " ";  // 5 (z + u + g + v + cache(1))
  wcout << "v:" << v.GetRefCount() << endl; // 5 (z + u + g + v + cache(1))

  u = g;
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x + cache(3))
  wcout << "y:" << y.GetRefCount() << " ";  // 2 (y + cache(2))
  wcout << "z:" << z.GetRefCount() << " ";  // 5 (z + u + g + v + cache(1))
  wcout << "u:" << u.GetRefCount() << " ";  // 5 (z + u + g + v + cache(1))
  wcout << "v:" << v.GetRefCount() << endl; // 5 (z + u + g + v + cache(1))
}
