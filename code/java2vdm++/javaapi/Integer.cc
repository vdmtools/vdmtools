#include "Integer.h"
#include "Object.h"
#include "JavaString.h"
#include "tb_wstring.h"
#ifdef DEF_Integer_USERIMPL
#include "Integer_userimpl.cpp"

#endif

#if defined(_MSC_VER) && defined(_DEBUG)
#include <crtdbg.h>
#endif

#define MAXLEN ((size_t)80)

const wchar_t* vdm_Integer::name = L"JavaLangInteger";

Generic vdm_Integer::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  Generic res;

  if (!wcscmp (name, L"hashCode")) {
    res = this->vdm_hashCode();
  } else if (!wcscmp(name, L"compareToInt")) {
    vdm_Integer vdm_anObject = *vdm_Integer::GetDLPtr(p[1]);
    res = this->vdm_compareToInt(vdm_anObject);
  } else if (!wcscmp(name, L"equals'")) {
    vdm_Integer vdm_anObject = *vdm_Integer::GetDLPtr(p[1]);
    res = this->vdm_equals(vdm_anObject);
  } else if (!wcscmp(name, L"setValueChar")) {
      Char c = p[1];
      Int i((int)c.GetValue());
      this->setValue(i);
  } else if (!wcscmp(name, L"setValueInt")) {
    if(p[1].IsInt()) {
      Int i = p[1];
      this->setValue(i);
    } else if(p[1].IsReal()) {
      Real r = p[1];
      this->setValue(r);
    }
  } else if (!wcscmp(name, L"setValueStr")) {
    vdm_String vdm_anObject = *vdm_String::GetDLPtr(p[1]);
    this->setValue(vdm_anObject);
  } else if (!wcscmp(name, L"intValue")) {
    res = this->vdm_intValue();
  } else if (!wcscmp(name, L"byteValue")) {
    res = this->vdm_byteValue();
  } else if (!wcscmp(name, L"longValue")) {
    res = this->vdm_longValue();
  } else if (!wcscmp(name, L"shortValue")) {
    res = this->vdm_shortValue();
  } else if (!wcscmp(name, L"floatValue")) {
    res = this->vdm_floatValue();
  } else if (!wcscmp(name, L"charValue")) {
    Int i = this->vdm_intValue();
    res = Char((wchar_t)i.GetValue());
  } else if (!wcscmp(name, vdm_Integer::name)) {
    if(p.Length()==0) {
      *this=vdm_Integer();
      DLObject res(vdm_Integer::name, this);
      return res;
    } else if (p.Length()==1 && p[1].IsSequence()) {
      if(p[1].IsInt()) {
        *this=vdm_Integer(p[1]);
      } else {
        vdm_String vdm_anObject = *vdm_String::GetDLPtr(p[1]);
        *this=vdm_Integer(vdm_anObject);
      }
      DLObject res(vdm_Integer::name, this);
      return res;
    }
  } else {
      // the method does not exist
  }
  return res;
}

vdm_Integer::vdm_Integer ()
{
  value = 0;
}

#ifndef DEF_Integer_Integer

vdm_Integer::vdm_Integer (const Int &vdm_value_u_u)
{
  value = vdm_value_u_u.GetValue();
}

#endif

void vdm_Integer::setValue(const Int &vdm_i)
{
  value = vdm_i.GetValue();
}

void vdm_Integer::setValue(const Real &vdm_i)
{
  value = (int)vdm_i.GetValue();
}

void vdm_Integer::setValue(const vdm_String &vdm_nm)
{
//  _CrtDbgBreak();
  wstring s=vdm_nm.getValue();
#ifdef _MSC_VER
  swscanf(s.c_str(),L"%d",&value);
#else
  string tmp = TBWSTR::wstring2string(s);
  sscanf(tmp.c_str(),"%d",&value);
#endif// _ंMSC_VER
}


#ifndef DEF_Integer_decode

DLObject vdm_Integer::vdm_decode (const vdm_String &vdm_nm)
{
  DLObject res (vdm_Integer::name, new vdm_Integer());
  return res;
}

#endif

#ifndef DEF_Integer_equals

Bool vdm_Integer::vdm_equals (const vdm_Integer &vdm_obj)
{
#if defined(_MSC_VER) && defined(_DEBUG)
//  _CrtDbgBreak();
#endif
  bool b = value==vdm_obj.value;
  return Bool(b);
}

#endif

#ifndef DEF_Integer_valueOf

DLObject vdm_Integer::vdm_valueOf (const vdm_String &vdm_s)
{
  DLObject res (vdm_Integer::name, new vdm_Integer());
  return res;
}

#endif

#ifndef DEF_Integer_hashCode

Int vdm_Integer::vdm_hashCode ()
{
  return Int(value);
}

#endif

#ifndef DEF_Integer_intValue

Int vdm_Integer::vdm_intValue ()
{
  Int res(value);
  return res;
}

#endif

#ifndef DEF_Integer_byteValue

Int vdm_Integer::vdm_byteValue ()
{
  int b = (wchar_t)value;
  return Int(b);
}

#endif

#ifndef DEF_Integer_compareTo

Int vdm_Integer::vdm_compareToInt (const vdm_Integer &vdm_o)
{
  int thisVal = value;
  int anotherVal = vdm_o.value;
  return Int(thisVal<anotherVal ? -1 : (thisVal==anotherVal ? 0 : 1));
}

#endif

Int vdm_Integer::vdm_compareTo (const vdm_Object &)
{
  return Int(0);
}

#ifndef DEF_Integer_longValue

Int vdm_Integer::vdm_longValue ()
{
  int b = (long)value;
  return Int(b);
}

#endif

#ifndef DEF_Integer_floatValue

Real vdm_Integer::vdm_floatValue ()
{
  double d = (double)value;
  return Real(d);
}

#endif

#ifndef DEF_Integer_shortValue

Int vdm_Integer::vdm_shortValue ()
{
  int s = (short)value;
  return Int(s);
}

#endif

#ifndef DEF_Integer_doubleValue

Real vdm_Integer::vdm_doubleValue ()
{
  return Real(Int(value));
}

#endif

#ifndef DEF_Integer_toHexString

DLObject vdm_Integer::vdm_toHexString (const Int &vdm_i)
{
  DLObject res (vdm_String::name, new vdm_String());
  return res;
}

#endif

#ifndef DEF_Integer_toString

DLObject vdm_Integer::vdm_toString ()
{
#ifdef _MSC_VER
  wchar_t text[MAXLEN];
  swprintf(text,L"%d",value);
  wstring str(text);
#else
  char text[MAXLEN];
  sprintf(text,"%d",value);
  wstring str=TBWSTR::string2wstring(string(text));
#endif // _MSC_VER

  DLObject res (vdm_String::name, new vdm_String(str));
  return res;
}

#endif

#ifndef DEF_Integer_toString

DLObject vdm_Integer::vdm_toString (const Int &vdm_i)
{
  DLObject res (vdm_String::name, new vdm_String());
  return res;
}

#endif

#ifndef DEF_Integer_toOctalString

DLObject vdm_Integer::vdm_toOctalString (const Int &vdm_i)
{
  DLObject res (vdm_String::name, new vdm_String());
  return res;
}

#endif

#ifndef DEF_Integer_toString

DLObject vdm_Integer::vdm_toString (const Int &vdm_i, const Int &vdm_radix)
{
/*
#ifdef _MSC_VER
  wchar_t text[MAXLEN];
  swprintf(text,L"%d",vdm_i.GetValue());
  wstring str(text);
#else
  char text[MAXLEN];
  sprintf(text,"%d",vdm_i.GetValue());
  wstring str = TBWSTR::string2wstring(string(text));
#endif // _MSC_VER
*/
  wstring str (vdm_i.ascii());

  DLObject res (vdm_String::name, new vdm_String(str));
  return res;
}

#endif

#ifndef DEF_Integer_Integer

vdm_Integer::vdm_Integer (const vdm_String &vdm_s)
{  ;
}

#endif

#ifndef DEF_Integer_parseInt

Int vdm_Integer::vdm_parseInt (const vdm_String &vdm_s)
{
  return Int(0);
}

#endif

#ifndef DEF_Integer_getInteger

DLObject vdm_Integer::vdm_getInteger (const vdm_String &vdm_nm)
{
  DLObject res (vdm_String::name, new vdm_String());
  return res;
}

#endif

#ifndef DEF_Integer_parseInt

Int vdm_Integer::vdm_parseInt (const vdm_String &vdm_s, const Int &vdm_radix)
{
  return Int(0);
}

#endif

#ifndef DEF_Integer_getInteger

DLObject vdm_Integer::vdm_getInteger (const vdm_String &vdm_nm, const Int &vdm_val)
{
  DLObject res (vdm_Integer::name, new vdm_Integer());
  return res;
}

#endif

#ifndef DEF_Integer_getInteger

DLObject vdm_Integer::vdm_getInteger (const vdm_String &vdm_nm, const vdm_Integer &vdm_val)
{
  DLObject res (vdm_Integer::name, new vdm_Integer());
  return res;
}

#endif

vdm_Integer *vdm_Integer::GetDLPtr(const DLObject& obj)
{
  vdm_Integer *objPtr = (vdm_Integer*) obj.GetPtr();
  return objPtr;
}
