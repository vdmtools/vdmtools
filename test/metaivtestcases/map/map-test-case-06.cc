#include "metaiv.h"

void metaivtest()
{
  Map m1, m2;
  bool res;
  
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << endl;

  res=m1.Compare(m2); 
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << endl;

  res=(m1<m2); 
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << endl;

  res=(m1==m2); 
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << endl;

  res=(m1!=m2); 
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << endl;

  res=m1.IsNil();
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << endl;

  res=m1.IsChar(); 
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << endl;

  res=m1.IsInt();
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << endl;

  res=m1.IsReal();
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << endl;

  res=m1.IsQuote();
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << endl;

  res=m1.IsTuple();
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << endl;

  res=m1.IsRecord();
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << endl;

  res=m1.IsSet();
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << endl;

  res=m1.IsMap();
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << endl;
  
  res=m1.IsText();
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << endl;

  res=m1.IsToken();
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << endl;

  res=m1.IsBool();
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << endl;
  
  res=m1.IsSequence();
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << endl;

  res=m1.IsObjectRef();
  wcout << "m1:" << m1.GetRefCount() << " ";
  wcout << "m2:" << m2.GetRefCount() << endl;
  
}
