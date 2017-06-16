#include "metaiv.h"

void metaivtest()
{
  ObjectRef or1, or2;
  bool res;
  
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;

  res=or1.Compare(or2); 
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;

  res=(or1<or2); 
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;

  res=(or1==or2); 
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;

  res=(or1!=or2); 
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;

  res=or1.IsNil();
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;

  res=or1.IsChar(); 
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;

  res=or1.IsInt();
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;

  res=or1.IsReal();
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;

  res=or1.IsQuote();
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;

  res=or1.IsTuple();
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;

  res=or1.IsRecord();
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;

  res=or1.IsSet();
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;

  res=or1.IsMap();
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;
  
  res=or1.IsText();
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;

  res=or1.IsToken();
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;

  res=or1.IsBool();
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;
  
  res=or1.IsSequence();
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;

  res=or1.IsObjectRef();
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;
  
}
