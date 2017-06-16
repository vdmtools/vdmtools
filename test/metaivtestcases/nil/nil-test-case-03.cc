#include "metaiv.h"


void metaivtest()
{
  Bool n1, n2;
  bool res;
  
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << endl;

  res=n1.Compare(n2); 
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << endl;

  res=(n1<n2); 
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << endl;

  res=(n1==n2); 
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << endl;

  res=(n1!=n2); 
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << endl;

  res=n1.IsNil();
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << endl;

  res=n1.IsChar(); 
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << endl;

  res=n1.IsInt();
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << endl;

  res=n1.IsReal();
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << endl;

  res=n1.IsQuote();
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << endl;

  res=n1.IsTuple();
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << endl;

  res=n1.IsRecord();
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << endl;

  res=n1.IsSet();
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << endl;

  res=n1.IsMap();
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << endl;
  
  res=n1.IsText();
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << endl;

  res=n1.IsToken();
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << endl;

  res=n1.IsBool();
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << endl;
  
  res=n1.IsSequence();
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << endl;

  res=n1.IsObjectRef();
  wcout << "n1:" << n1.GetRefCount() << " ";
  wcout << "n2:" << n2.GetRefCount() << endl;
  
}
