#include "bigint_dl.h"

DlClass* DlClass_new (const char* name)
{
#ifdef DEBUG
  cerr << "DlClass_new called" << endl;
#endif //DEBUG
  if (!strcmp(name, "BigInt"))
    return new BigIntDL ();
  else
    return 0;
}

int DlClass_delete (DlClass* c)
{
#ifdef DEBUG
  cerr << "DlClass_delete called" << endl;
#endif //DEBUG
  try {
    delete c;
  } catch (...) {
    return 0;
  }
  return 1;
}

Generic DlClass_call (DlClass* c, const char* name, const Sequence& params, 
                      int& success)
{
#ifdef DEBUG
  cerr << "DlClass_call: " << name << "(" << params << ")" << endl;
#endif //DEBUG
  Generic result;
  try {
      result = c->DlMethodCall (name, params);
  } catch (...) {
    success = 0;
    return result;
  }
  success = 1;
  return result;
}

Generic BigIntDL::DlMethodCall (const char* name, const Sequence &p)
{
  Generic res;
	
  if (!strcmp (name, "SetVal"))
    res = this->SetVal(p); 
  else if (!strcmp(name, "plus"))
    res = this->plus(p);
  else if (!strcmp(name, "toString"))
    res = this->toString();
  else {
      // the method does not exist
  }
	
  return res;
}

Generic BigIntDL::SetVal (const Sequence &p)
{
#ifdef DEBUG
  cout << "BigIntDL::SetVal" << endl;
#endif //DEBUG
  Int n(p.Hd());
  int nVal = n.GetValue();
  val = MAPM(nVal);
  return Generic();
}

#ifdef CG
BigIntDL *BigIntDL::GetDLPtr(const ObjectRef& obj)
#else
BigIntDL *BigIntDL::GetDLPtr(const DLObject& obj)
#endif //CG
{
#ifdef CG
  vdm_BigInt *objRefPtr = ObjGet_vdm_BigInt(obj);
  BigIntDL *objPtr = (BigIntDL*) objRefPtr->BigInt_dlClassPtr;
#else
  BigIntDL *objPtr = (BigIntDL*) obj.GetPtr(); 
#endif
  return objPtr;
}


#ifdef CG
ObjectRef BigIntDL::plus (const Sequence &p)
#else
DLObject BigIntDL::plus (const Sequence &p)
#endif //CG
{
#ifdef DEBUG
  cout << "BigIntDL::plus" << endl;
#endif //DEBUG
  // Extract arguments
  BigIntDL *argPtr = GetDLPtr(p.Hd());

  // Set up result object
#ifdef CG
  ObjectRef result (new vdm_BigInt);
#else
  DLObject result("BigInt", new BigIntDL);
#endif 

  BigIntDL *resPtr = GetDLPtr(result);

  // Perform manipulation on pointers, as needed for function
  resPtr->setVal( val + argPtr->getVal());

  return result;
}

Sequence BigIntDL::toString()
{
#ifdef DEBUG
  cout << "BigIntDL::toString" << endl;
#endif //DEBUG
  char res[100];
  val.toIntegerString(res);
  return Sequence(string(res));

}

void BigIntDL::setVal(MAPM newVal)
{
  val = newVal;
}

MAPM BigIntDL::getVal()
{
  return val;
}
