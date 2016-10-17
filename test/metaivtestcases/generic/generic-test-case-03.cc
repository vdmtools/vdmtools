#include "metaiv.h"

void metaivtest()
{
  Generic g1(Real(3.14)), g2;
  bool res;
  
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << endl;

  res=g1.Compare(g2); 
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << endl;

  res=(g1<g2); 
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << endl;

  res=(g1==g2); 
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << endl;

  res=(g1!=g2); 
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << endl;

  res=g1.IsNil();
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << endl;

  res=g1.IsChar(); 
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << endl;

  res=g1.IsInt();
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << endl;

  res=g1.IsReal();
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << endl;

  res=g1.IsQuote();
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << endl;

  res=g1.IsTuple();
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << endl;

  res=g1.IsRecord();
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << endl;

  res=g1.IsSet();
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << endl;

  res=g1.IsMap();
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << endl;
  
  res=g1.IsText();
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << endl;

  res=g1.IsToken();
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << endl;

  res=g1.IsBool();
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << endl;
  
  res=g1.IsSequence();
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << endl;

  res=g1.IsObjectRef();
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << endl;
  
}
