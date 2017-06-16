#include "metaiv.h"

void metaivtest()
{
  Char c1, c2('a'), c3(c1);
  bool res;
  
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";
  wcout << "c3:" << c3.GetRefCount() << endl;

  res=c1.Compare(c2); 
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";
  wcout << "c3:" << c3.GetRefCount() << endl;

  res=(c1<c2); 
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";
  wcout << "c3:" << c3.GetRefCount() << endl;
  
  res=(c1==c2); 
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";
  wcout << "c3:" << c3.GetRefCount() << endl;

  res=(c1!=c2); 
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";
  wcout << "c3:" << c3.GetRefCount() << endl;

  res=c1.IsNil();
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";
  wcout << "c3:" << c3.GetRefCount() << endl;

  res=c1.IsChar(); 
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";
  wcout << "c3:" << c3.GetRefCount() << endl;

  res=c1.IsInt();
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";
  wcout << "c3:" << c3.GetRefCount() << endl;

  res=c1.IsReal();
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";
  wcout << "c3:" << c3.GetRefCount() << endl;

  res=c1.IsQuote();
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";
  wcout << "c3:" << c3.GetRefCount() << endl;

  res=c1.IsTuple();
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";
  wcout << "c3:" << c3.GetRefCount() << endl;

  res=c1.IsRecord();
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";
  wcout << "c3:" << c3.GetRefCount() << endl;

  res=c1.IsSet();
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";
  wcout << "c3:" << c3.GetRefCount() << endl;

  res=c1.IsMap();
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";
  wcout << "c3:" << c3.GetRefCount() << endl;
  
  res=c1.IsText();
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";
  wcout << "c3:" << c3.GetRefCount() << endl;

  res=c1.IsToken();
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";
  wcout << "c3:" << c3.GetRefCount() << endl;

  res=c1.IsBool();
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";
  wcout << "c3:" << c3.GetRefCount() << endl;
  
  res=c1.IsSequence();
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";
  wcout << "c3:" << c3.GetRefCount() << endl;

  res=c1.IsObjectRef();
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";
  wcout << "c3:" << c3.GetRefCount() << endl;
  
  char val=c1.GetValue();
  wcout << "c1:" << c1.GetRefCount() << " ";
  wcout << "c2:" << c2.GetRefCount() << " ";
  wcout << "c3:" << c3.GetRefCount() << endl;
}
