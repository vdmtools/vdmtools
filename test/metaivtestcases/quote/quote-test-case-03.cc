#include "metaiv.h"

void metaivtest()
{
  Quote q1, q2(L"quote"), q3(q1);
  bool res;
  
  wcout << "q1:" << q1.GetRefCount() << " ";  // 3 (q1 + q3 + default_cache)
  wcout << "q2:" << q2.GetRefCount() << " ";  // 1
  wcout << "q3:" << q3.GetRefCount() << endl; // 3 (q1 + q3 + default_cache)

  res=q1.Compare(q2); 
  wcout << "q1:" << q1.GetRefCount() << " ";
  wcout << "q2:" << q2.GetRefCount() << " ";
  wcout << "q3:" << q3.GetRefCount() << endl;

  res=(q1<q2); 
  wcout << "q1:" << q1.GetRefCount() << " ";
  wcout << "q2:" << q2.GetRefCount() << " ";
  wcout << "q3:" << q3.GetRefCount() << endl;
  
  res=(q1==q2); 
  wcout << "q1:" << q1.GetRefCount() << " ";
  wcout << "q2:" << q2.GetRefCount() << " ";
  wcout << "q3:" << q3.GetRefCount() << endl;

  res=(q1!=q2); 
  wcout << "q1:" << q1.GetRefCount() << " ";
  wcout << "q2:" << q2.GetRefCount() << " ";
  wcout << "q3:" << q3.GetRefCount() << endl;

  res=q1.IsNil();
  wcout << "q1:" << q1.GetRefCount() << " ";
  wcout << "q2:" << q2.GetRefCount() << " ";
  wcout << "q3:" << q3.GetRefCount() << endl;

  res=q1.IsChar(); 
  wcout << "q1:" << q1.GetRefCount() << " ";
  wcout << "q2:" << q2.GetRefCount() << " ";
  wcout << "q3:" << q3.GetRefCount() << endl;

  res=q1.IsInt();
  wcout << "q1:" << q1.GetRefCount() << " ";
  wcout << "q2:" << q2.GetRefCount() << " ";
  wcout << "q3:" << q3.GetRefCount() << endl;

  res=q1.IsReal();
  wcout << "q1:" << q1.GetRefCount() << " ";
  wcout << "q2:" << q2.GetRefCount() << " ";
  wcout << "q3:" << q3.GetRefCount() << endl;

  res=q1.IsQuote();
  wcout << "q1:" << q1.GetRefCount() << " ";
  wcout << "q2:" << q2.GetRefCount() << " ";
  wcout << "q3:" << q3.GetRefCount() << endl;

  res=q1.IsTuple();
  wcout << "q1:" << q1.GetRefCount() << " ";
  wcout << "q2:" << q2.GetRefCount() << " ";
  wcout << "q3:" << q3.GetRefCount() << endl;

  res=q1.IsRecord();
  wcout << "q1:" << q1.GetRefCount() << " ";
  wcout << "q2:" << q2.GetRefCount() << " ";
  wcout << "q3:" << q3.GetRefCount() << endl;

  res=q1.IsSet();
  wcout << "q1:" << q1.GetRefCount() << " ";
  wcout << "q2:" << q2.GetRefCount() << " ";
  wcout << "q3:" << q3.GetRefCount() << endl;

  res=q1.IsMap();
  wcout << "q1:" << q1.GetRefCount() << " ";
  wcout << "q2:" << q2.GetRefCount() << " ";
  wcout << "q3:" << q3.GetRefCount() << endl;
  
  res=q1.IsText();
  wcout << "q1:" << q1.GetRefCount() << " ";
  wcout << "q2:" << q2.GetRefCount() << " ";
  wcout << "q3:" << q3.GetRefCount() << endl;

  res=q1.IsToken();
  wcout << "q1:" << q1.GetRefCount() << " ";
  wcout << "q2:" << q2.GetRefCount() << " ";
  wcout << "q3:" << q3.GetRefCount() << endl;

  res=q1.IsBool();
  wcout << "q1:" << q1.GetRefCount() << " ";
  wcout << "q2:" << q2.GetRefCount() << " ";
  wcout << "q3:" << q3.GetRefCount() << endl;
  
  res=q1.IsSequence();
  wcout << "q1:" << q1.GetRefCount() << " ";
  wcout << "q2:" << q2.GetRefCount() << " ";
  wcout << "q3:" << q3.GetRefCount() << endl;

  res=q1.IsObjectRef();
  wcout << "q1:" << q1.GetRefCount() << " ";
  wcout << "q2:" << q2.GetRefCount() << " ";
  wcout << "q3:" << q3.GetRefCount() << endl;
  
  wstring val=q1.GetValue();
  wcout << "q1:" << q1.GetRefCount() << " ";
  wcout << "q2:" << q2.GetRefCount() << " ";
  wcout << "q3:" << q3.GetRefCount() << endl;
}
