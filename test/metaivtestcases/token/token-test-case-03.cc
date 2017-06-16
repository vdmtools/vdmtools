#include "metaiv.h"

void metaivtest()
{
  Token tok1, tok2(L"Token"), tok3(tok1);
  bool res;
  
  wcout << "tok1:" << tok1.GetRefCount() << " ";  // 3 (tok1, tok3, default_cache)
  wcout << "tok2:" << tok2.GetRefCount() << " ";
  wcout << "tok3:" << tok3.GetRefCount() << endl;

  res=tok1.Compare(tok2); 
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";
  wcout << "tok3:" << tok3.GetRefCount() << endl;

  res=(tok1<tok2); 
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";
  wcout << "tok3:" << tok3.GetRefCount() << endl;
  
  res=(tok1==tok2); 
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";
  wcout << "tok3:" << tok3.GetRefCount() << endl;

  res=(tok1!=tok2); 
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";
  wcout << "tok3:" << tok3.GetRefCount() << endl;

  res=tok1.IsNil();
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";
  wcout << "tok3:" << tok3.GetRefCount() << endl;

  res=tok1.IsChar(); 
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";
  wcout << "tok3:" << tok3.GetRefCount() << endl;

  res=tok1.IsInt();
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";
  wcout << "tok3:" << tok3.GetRefCount() << endl;

  res=tok1.IsReal();
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";
  wcout << "tok3:" << tok3.GetRefCount() << endl;

  res=tok1.IsToken();
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";
  wcout << "tok3:" << tok3.GetRefCount() << endl;

  res=tok1.IsTuple();
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";
  wcout << "tok3:" << tok3.GetRefCount() << endl;

  res=tok1.IsRecord();
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";
  wcout << "tok3:" << tok3.GetRefCount() << endl;

  res=tok1.IsSet();
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";
  wcout << "tok3:" << tok3.GetRefCount() << endl;

  res=tok1.IsMap();
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";
  wcout << "tok3:" << tok3.GetRefCount() << endl;
  
  res=tok1.IsText();
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";
  wcout << "tok3:" << tok3.GetRefCount() << endl;

  res=tok1.IsToken();
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";
  wcout << "tok3:" << tok3.GetRefCount() << endl;

  res=tok1.IsBool();
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";
  wcout << "tok3:" << tok3.GetRefCount() << endl;
  
  res=tok1.IsSequence();
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";
  wcout << "tok3:" << tok3.GetRefCount() << endl;

  res=tok1.IsObjectRef();
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";
  wcout << "tok3:" << tok3.GetRefCount() << endl;
  
  Generic val=tok1.GetValue();
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";
  wcout << "tok3:" << tok3.GetRefCount() << endl;

  
}
