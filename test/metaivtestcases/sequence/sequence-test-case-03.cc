#include "metaiv.h"

void metaivtest()
{
  //Sequence sq1(L"first"), sq2(L"second"), sq3(sq1);
  Sequence sq1(mk_sequence(Int(1), Int(1), Int(2))), sq2(mk_sequence(Char(L'x'), Char(L'y'), Char(L'z'))), sq3(sq1);
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << " ";
  wcout << "sq3:" << sq3.GetRefCount() << endl;

  Generic g = (Char)sq2.Hd();
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << " ";
  wcout << "sq3:" << sq3.GetRefCount() << endl;

  /* NOT WORKING
  sq3 = sq2.Tl();
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << " ";
  wcout << "sq3:" << sq3.GetRefCount() << endl;
  */

  int len = sq3.Length();
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << " ";
  wcout << "sq3:" << sq3.GetRefCount() << endl;

  bool b = sq1.IsEmpty();
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << " ";
  wcout << "sq3:" << sq3.GetRefCount() << endl;

  sq2.Clear();
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << " ";
  wcout << "sq3:" << sq3.GetRefCount() << endl;	

  sq2.ImpAppend(sq1);
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << " ";
  wcout << "sq3:" << sq3.GetRefCount() << endl;	

  sq1.RemElem(3);
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << " ";
  wcout << "sq3:" << sq3.GetRefCount() << endl;	

  sq1.ImpConc(sq3);	
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << " ";
  wcout << "sq3:" << sq3.GetRefCount() << endl;		

  sq1.ImpModify(1, (Char)L'A');
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << " ";
  wcout << "sq3:" << sq3.GetRefCount() << endl;		

  sq3.ImpPrepend((Char)L'A');
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << " ";
  wcout << "sq3:" << sq3.GetRefCount() << endl;	

  Set set = sq1.Elems();
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << " ";
  wcout << "sq3:" << sq3.GetRefCount() << endl;	

  wstring str = sq3.GetString();
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << " ";
  wcout << "sq3:" << sq3.GetRefCount() << endl;	
  
  b = sq1.GetString(str);
  wcout << "sq1:" << sq1.GetRefCount() << " ";
  wcout << "sq2:" << sq2.GetRefCount() << " ";
  wcout << "sq3:" << sq3.GetRefCount() << endl;	 
}
  
