#include "metaiv.h"

void metaivtest()
{
  //Sequence sq1(L"sequence"), sq2(sq1);
  Sequence sq1(mk_sequence(Char(L'A'), Char(L'B'))), sq2(sq1);
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;

  Generic g(sq1);
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;
  
  g = (Char)sq1[1]; 
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;

  g = (Char)sq2.Index(1);
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;
  
  int i = sq1.First(g);
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;

  i = sq1.Next(g);
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << endl;
}
  
