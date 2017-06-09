#include "metaiv.h"

void metaivtest()
{
  Sequence sq1;
  wcout << "sq1:" << sq1.GetRefCount() << endl;
  
  Sequence sq2(sq1);
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;

  //Sequence sq3 = (wstring)L"first";
  Sequence sq3 = mk_sequence(Int(0), Int(1), Int(2));
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << " ";	
  wcout << "sq3:" << sq3.GetRefCount() << endl;

  Generic g(sq3);
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << " ";	
  wcout << "sq3:" << sq3.GetRefCount() << endl;
  
  Sequence sq4(g);
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << " ";	
  wcout << "sq3:" << sq3.GetRefCount() << " ";
  wcout << "sq4:" << sq4.GetRefCount() << endl;

  sq3=sq2;
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << " ";	
  wcout << "sq3:" << sq3.GetRefCount() << " ";
  wcout << "sq4:" << sq4.GetRefCount() << endl;

  sq1=g;
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << " ";	
  wcout << "sq3:" << sq3.GetRefCount() << " ";
  wcout << "sq4:" << sq4.GetRefCount() << endl;
  
}
