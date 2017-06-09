#include "metaiv.h"

void metaivtest()
{
  Record r1, r2;
  bool res;
  
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << endl;

  res=r1.Compare(r2); 
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << endl;

  res=(r1<r2); 
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << endl;

  res=(r1==r2); 
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << endl;

  res=(r1!=r2); 
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << endl;

  res=r1.IsNil();
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << endl;

  res=r1.IsChar(); 
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << endl;

  res=r1.IsInt();
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << endl;

  res=r1.IsReal();
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << endl;

  res=r1.IsQuote();
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << endl;

  res=r1.IsTuple();
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << endl;

  res=r1.IsRecord();
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << endl;

  res=r1.IsSet();
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << endl;

  res=r1.IsMap();
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << endl;
  
  res=r1.IsText();
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << endl;

  res=r1.IsToken();
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << endl;

  res=r1.IsBool();
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << endl;
  
  res=r1.IsSequence();
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << endl;

  res=r1.IsObjectRef();
  wcout << "r1:" << r1.GetRefCount() << " ";
  wcout << "r2:" << r2.GetRefCount() << endl;
  
}
