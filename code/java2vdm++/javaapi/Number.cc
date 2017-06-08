#include "Number.h"
#ifdef DEF_Number_USERIMPL
#include "Number_userimpl.cpp"

#endif

const wchar_t* vdm_Number::name = L"JavaLangNumber";

Generic vdm_Number::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  Generic res;

//  _CrtDbgBreak();
  
  if (!wcscmp (name, L"shortValue"))
    res = this->vdm_shortValue(); 
  else if (!wcscmp(name, L"byteValue")) {
    res = this->vdm_byteValue();
//  } else if (!wcscmp(name, L"Number")) {
//    if(p.Length()==0) {
//      *this=vdm_String();
//      DLObject res(vdm_String::name, this);
//      return res;
//    } else if (p.Length()==1 && p[1].IsSequence()) {
//      type_cL sc = p[1];
//      *this=vdm_String(sc);
//      DLObject res(vdm_String::name, this);
//      return res;
//    } else if (p.Length()==1 && p[1].IsQuote()) {
//      *this=vdm_String(Quote());
//      DLObject res(vdm_String::name, this);
//      return res;
//    }
  } else {
      // the method does not exist
  }

  return res;
}

/*
vdm_Number::vdm_Number ()
{

}
*/
/*
#ifndef DEF_Number_Number

vdm_Number::vdm_Number (const Quote &var_1_1)
{
  vdm_isNil = (Bool) true;
}

#endif
*/
#ifndef DEF_Number_byteValue

Int vdm_Number::vdm_byteValue ()
{
  Int i(0);
  return i;
}

#endif

#ifndef DEF_Number_shortValue

Int vdm_Number::vdm_shortValue ()
{
  Int i(0);
  return i;
}

#endif

