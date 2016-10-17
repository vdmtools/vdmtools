#include "metaiv.h"

void metaivtest()
{
  Token tok1(L"one");
  wcout << "tok1:" << tok1.GetRefCount() << endl;
  
  Token tok2;
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << endl;  // 2 (tok2, default_cache)

  Token tok3(tok1);
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";	
  wcout << "tok3:" << tok3.GetRefCount() << endl;

  wstring s=L"two";
  Token tok4(s);
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";	
  wcout << "tok3:" << tok3.GetRefCount() << " ";
  wcout << "tok4:" << tok4.GetRefCount() << endl;

  tok2=tok4;
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";	
  wcout << "tok3:" << tok3.GetRefCount() << " ";
  wcout << "tok4:" << tok4.GetRefCount() << endl;
  
  tok3=(wstring)L"three";
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";	
  wcout << "tok3:" << tok3.GetRefCount() << " ";
  wcout << "tok4:" << tok4.GetRefCount() << endl;

  Generic g(tok3);
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";	
  wcout << "tok3:" << tok3.GetRefCount() << " ";
  wcout << "tok4:" << tok4.GetRefCount() << endl;
  
  Token tok5(g);
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";	
  wcout << "tok3:" << tok3.GetRefCount() << " ";
  wcout << "tok4:" << tok4.GetRefCount() << " ";
  wcout << "tok5:" << tok5.GetRefCount() << endl;	

  tok1=g;
  wcout << "tok1:" << tok1.GetRefCount() << " ";
  wcout << "tok2:" << tok2.GetRefCount() << " ";	
  wcout << "tok3:" << tok3.GetRefCount() << " ";
  wcout << "tok4:" << tok4.GetRefCount() << " ";
  wcout << "tok5:" << tok5.GetRefCount() << endl;	
}
