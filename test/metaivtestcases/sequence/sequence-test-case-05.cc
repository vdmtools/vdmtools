#include "metaiv.h"

void metaivtest()
{
  //Sequence sq1(L"first"), sq2;
  Sequence sq1(mk_sequence(Int(0), Int(1), Int(2))), sq2;
  bool res;
  
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;

  res=sq1.Compare(sq2); 
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;

  res=(sq1<sq2); 
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;

  res=(sq1==sq2); 
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;

  res=(sq1!=sq2); 
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;

  res=sq1.IsNil();
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;

  res=sq1.IsChar(); 
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;

  res=sq1.IsInt();
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;

  res=sq1.IsReal();
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;

  res=sq1.IsQuote();
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;

  res=sq1.IsTuple();
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;

  res=sq1.IsRecord();
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;

  res=sq1.IsSet();
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;

  res=sq1.IsMap();
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;
  
  res=sq1.IsText();
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;

  res=sq1.IsToken();
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;

  res=sq1.IsBool();
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;
  
  res=sq1.IsSequence();
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;

  res=sq1.IsObjectRef();
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;
}
