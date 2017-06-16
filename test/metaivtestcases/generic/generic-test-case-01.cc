#include "metaiv.h"

void metaivtest()
{
  Generic g1, g2(Int(123));
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << endl;
  
  Generic g3;
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << " ";
  wcout << "g3:" << g3.GetRefCount() << endl;

  g1=Real(7);
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << " ";
  wcout << "g3:" << g3.GetRefCount() << endl;	

  Generic g4(g2);
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << " ";
  wcout << "g3:" << g3.GetRefCount() << " ";
  wcout << "g4:" << g4.GetRefCount() << endl;
  
  g2=g1;
  wcout << "g1:" << g1.GetRefCount() << " ";
  wcout << "g2:" << g2.GetRefCount() << " ";
  wcout << "g3:" << g3.GetRefCount() << " ";
  wcout << "g4:" << g4.GetRefCount() << endl;}
