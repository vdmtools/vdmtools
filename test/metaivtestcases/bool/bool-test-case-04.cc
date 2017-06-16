#include "metaiv.h"

void metaivtest()
{
  Bool b1, b2(true);
  bool res;
  
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;

  res = b1.Compare(b2); 
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;

  res = (b1 < b2); 
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;

  res = (b1 == b2); 
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;

  res = (b1 != b2); 
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;

  res = b1.IsNil();
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;

  res = b1.IsChar(); 
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;

  res = b1.IsInt();
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;

  res = b1.IsReal();
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;

  res = b1.IsQuote();
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;

  res = b1.IsTuple();
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;

  res = b1.IsRecord();
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;

  res = b1.IsSet();
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;

  res = b1.IsMap();
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;
  
  res = b1.IsText();
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;

  res = b1.IsToken();
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;

  res = b1.IsBool();
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;
  
  res = b1.IsSequence();
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;

  res = b1.IsObjectRef();
  wcout << "b1:" << b1.GetRefCount() << " ";
  wcout << "b2:" << b2.GetRefCount() << endl;
  
}
