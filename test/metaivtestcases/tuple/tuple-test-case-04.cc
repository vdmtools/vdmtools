#include "metaiv.h"

void metaivtest()
{
  Tuple tup1, tup2(2), tup3(tup1);
  bool res;
  
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";
  wcout << "tup3:" << tup3.GetRefCount() << endl;

  res=tup1.Compare(tup2); 
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";
  wcout << "tup3:" << tup3.GetRefCount() << endl;

  res=(tup1<tup2); 
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";
  wcout << "tup3:" << tup3.GetRefCount() << endl;
  
  res=(tup1==tup2); 
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";
  wcout << "tup3:" << tup3.GetRefCount() << endl;

  res=(tup1!=tup2); 
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";
  wcout << "tup3:" << tup3.GetRefCount() << endl;

  res=tup1.IsNil();
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";
  wcout << "tup3:" << tup3.GetRefCount() << endl;

  res=tup1.IsChar(); 
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";
  wcout << "tup3:" << tup3.GetRefCount() << endl;

  res=tup1.IsInt();
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";
  wcout << "tup3:" << tup3.GetRefCount() << endl;

  res=tup1.IsReal();
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";
  wcout << "tup3:" << tup3.GetRefCount() << endl;

  res=tup1.IsTuple();
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";
  wcout << "tup3:" << tup3.GetRefCount() << endl;

  res=tup1.IsTuple();
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";
  wcout << "tup3:" << tup3.GetRefCount() << endl;

  res=tup1.IsRecord();
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";
  wcout << "tup3:" << tup3.GetRefCount() << endl;

  res=tup1.IsSet();
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";
  wcout << "tup3:" << tup3.GetRefCount() << endl;

  res=tup1.IsMap();
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";
  wcout << "tup3:" << tup3.GetRefCount() << endl;
  
  res=tup1.IsText();
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";
  wcout << "tup3:" << tup3.GetRefCount() << endl;

  res=tup1.IsToken();
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";
  wcout << "tup3:" << tup3.GetRefCount() << endl;

  res=tup1.IsBool();
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";
  wcout << "tup3:" << tup3.GetRefCount() << endl;
  
  res=tup1.IsSequence();
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";
  wcout << "tup3:" << tup3.GetRefCount() << endl;

  res=tup1.IsObjectRef();
  wcout << "tup1:" << tup1.GetRefCount() << " ";
  wcout << "tup2:" << tup2.GetRefCount() << " ";
  wcout << "tup3:" << tup3.GetRefCount() << endl;
}
