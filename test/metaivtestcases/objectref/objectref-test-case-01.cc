#include "metaiv.h"

void metaivtest()
{
  ObjectRef or1;
  wcout << "or1:" << or1.GetRefCount() << endl;
  
  ObjectRef or2(or1);
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;

  vdmBase *p = new vdmBase();
  ObjectRef or3(p);
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << " ";	
  wcout << "or3:" << or3.GetRefCount() << endl;

  Generic g(or3);
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << " ";	
  wcout << "or3:" << or3.GetRefCount() << endl;
  
  ObjectRef or4(g);
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << " ";	
  wcout << "or3:" << or3.GetRefCount() << " ";
  wcout << "or4:" << or4.GetRefCount() << endl;

  or3=or2;
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << " ";	
  wcout << "or3:" << or3.GetRefCount() << " ";
  wcout << "or4:" << or4.GetRefCount() << endl;

  or1=g;
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << " ";	
  wcout << "or3:" << or3.GetRefCount() << " ";
  wcout << "or4:" << or4.GetRefCount() << endl;
}
