#include "metaiv.h"

void metaivtest()
{
  Text t1, t2(L"Text"), t3(t1);
  bool res;
  
  wcout << "t1:" << t1.GetRefCount() << " ";  // 3 (t1, t3, default_cache)
  wcout << "t2:" << t2.GetRefCount() << " ";
  wcout << "t3:" << t3.GetRefCount() << endl; // 3 (t1, t3, default_cache)

  res=t1.Compare(t2); 
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";
  wcout << "t3:" << t3.GetRefCount() << endl;

  res=(t1<t2); 
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";
  wcout << "t3:" << t3.GetRefCount() << endl;
  
  res=(t1==t2); 
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";
  wcout << "t3:" << t3.GetRefCount() << endl;

  res=(t1!=t2); 
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";
  wcout << "t3:" << t3.GetRefCount() << endl;

  res=t1.IsNil();
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";
  wcout << "t3:" << t3.GetRefCount() << endl;

  res=t1.IsChar(); 
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";
  wcout << "t3:" << t3.GetRefCount() << endl;

  res=t1.IsInt();
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";
  wcout << "t3:" << t3.GetRefCount() << endl;

  res=t1.IsReal();
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";
  wcout << "t3:" << t3.GetRefCount() << endl;

  res=t1.IsText();
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";
  wcout << "t3:" << t3.GetRefCount() << endl;

  res=t1.IsTuple();
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";
  wcout << "t3:" << t3.GetRefCount() << endl;

  res=t1.IsRecord();
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";
  wcout << "t3:" << t3.GetRefCount() << endl;

  res=t1.IsSet();
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";
  wcout << "t3:" << t3.GetRefCount() << endl;

  res=t1.IsMap();
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";
  wcout << "t3:" << t3.GetRefCount() << endl;
  
  res=t1.IsText();
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";
  wcout << "t3:" << t3.GetRefCount() << endl;

  res=t1.IsToken();
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";
  wcout << "t3:" << t3.GetRefCount() << endl;

  res=t1.IsBool();
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";
  wcout << "t3:" << t3.GetRefCount() << endl;
  
  res=t1.IsSequence();
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";
  wcout << "t3:" << t3.GetRefCount() << endl;

  res=t1.IsObjectRef();
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";
  wcout << "t3:" << t3.GetRefCount() << endl;
  
  wstring val=t1.GetValue();
  wcout << "t1:" << t1.GetRefCount() << " ";
  wcout << "t2:" << t2.GetRefCount() << " ";
  wcout << "t3:" << t3.GetRefCount() << endl;

  
}
