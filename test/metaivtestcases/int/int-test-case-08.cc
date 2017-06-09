#include "metaiv.h"

void metaivtest()
{
  Int x(20), y(x), z;
  bool b;
  
  wcout << "x:" << x.GetRefCount() << " ";  // 3 (x + y + cache(20))
  wcout << "y:" << y.GetRefCount() << " ";  // 3 (x + y + cache(20))
  wcout << "z:" << z.GetRefCount() << endl; // 2 (z + cache(0))

  b=x.Compare(y); 
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;

  b=y.Compare(z); 
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;

  b=(x<y); 
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;

  b=(x==y); 
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;

  b=(x!=y); 
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;

  b=x.IsNil();
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;

  b=x.IsChar(); 
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;

  b=x.IsInt();
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;

  b=x.IsReal();
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;

  b=x.IsQuote();
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;

  b=x.IsTuple();
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;

  b=x.IsRecord();
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;

  b=x.IsSet();
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;

  b=x.IsMap();
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;
  
  b=x.IsText();
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;

  b=x.IsToken();
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;

  b=x.IsBool();
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;
  
  b=x.IsSequence();
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;

  b=x.IsObjectRef();
  wcout << "x:" << x.GetRefCount() << " ";
  wcout << "y:" << y.GetRefCount() << " ";
  wcout << "z:" << z.GetRefCount() << endl;
}
