#ifndef _bigint_dl_h_
#define _bigint_dl_h_

#include "dlclass.h"
#include "m_apm.h"
#ifdef CG
#include "BigInt.h"
#endif //CG

class BigIntDL : public DlClass
{

  MAPM val;

 private:
#ifdef CG
  BigIntDL *BigIntDL::GetDLPtr(const ObjectRef& obj);
#else
  BigIntDL *BigIntDL::GetDLPtr(const DLObject& obj);
#endif //CG
 
 public:
  void setVal(MAPM);
  MAPM getVal();
  Generic DlMethodCall (const char* name, const Sequence &p);
  Generic SetVal (const Sequence &p);
#ifdef CG
  ObjectRef plus (const Sequence &p);
#else
  DLObject plus (const Sequence &p);
#endif //CG
  Sequence toString();
};

#endif //_bigint_dl_h_

