#include "metaiv.h"

void metaivtest()
{
  
  ObjectRef or1, or2(or1);
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;

  int id = or2.MyObjectId();
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;
  
  /* needs initialization
  or1.GetRef();
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;

  Bool b = or1.SameBaseClass(or2);
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;

  b = or1.IsOfClass(2);
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;
  
  b = or2.IsOfBaseClass(1);
  wcout << "or1:" << or1.GetRefCount() << " ";
  wcout << "or2:" << or2.GetRefCount() << endl;
  */
}
