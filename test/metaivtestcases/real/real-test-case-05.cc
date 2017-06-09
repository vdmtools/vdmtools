#include "metaiv.h"

void metaivtest()
{
  Real x(20.2), y=3.14, z(x);
  wcout << "x:" << x.GetRefCount() << " ";  // 2 (x, z) ... 20.2
  wcout << "y:" << y.GetRefCount() << " ";  // 1 ... 3.14
  wcout << "z:" << z.GetRefCount() << endl; // 2 (x, z) ... 20.2

  x=x.Exp(Int(2));
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;

  y=x.Exp(Real(2.1));
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;

  z=y.Exp(y);
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;
  
  x=z;
  z=x.Exp(Real(0.0));
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z, cache(1.0))
}
