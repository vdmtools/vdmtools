#include "metaiv.h"

void metaivtest()
{
  Set s1, s2;
  bool res;
  
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << endl;

  res=s1.Compare(s2); 
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << endl;

  res=(s1<s2); 
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << endl;

  res=(s1==s2); 
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << endl;

  res=(s1!=s2); 
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << endl;

  res=s1.IsNil();
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << endl;

  res=s1.IsChar(); 
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << endl;

  res=s1.IsInt();
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << endl;

  res=s1.IsReal();
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << endl;

  res=s1.IsQuote();
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << endl;

  res=s1.IsTuple();
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << endl;

  res=s1.IsRecord();
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << endl;

  res=s1.IsSet();
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << endl;

  res=s1.IsMap();
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << endl;
  
  res=s1.IsText();
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << endl;

  res=s1.IsToken();
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << endl;

  res=s1.IsBool();
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << endl;
  
  res=s1.IsSequence();
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << endl;

  res=s1.IsObjectRef();
  wcout << "s1:" << s1.GetRefCount() << " ";
  wcout << "s2:" << s2.GetRefCount() << endl;
  
}
