#include "j2vmain.h"
#include "Double.h"
#include "tb_wstring.h"

#include <stdio.h>
#include <string>
//using namespace std;

const wchar_t* vdm_Double::name = L"JavaLangDouble";

vdm_Double *vdm_Double::GetDLPtr(const DLObject& obj)
{
  vdm_Double *objPtr = (vdm_Double*) obj.GetPtr();
  return objPtr;
}

Generic vdm_Double::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  Generic res;
  if (!wcscmp(name,L"setValueReal") && p.Length()==1) {
    Real d = p[1];
    *this=vdm_Double(d);
  } else if (!wcscmp(name,L"setValueSeqOfChar") && p.Length()==1) {
    Sequence str = p[1];
    *this=vdm_Double(str);
//} else if (!wcscmp(name,L"hashCode") && p.Length() == 0) {
//  res = this->vdm_hashCode();
  } else if (!wcscmp(name,L"parseDouble'") && p.Length() == 1) {
    Sequence str = p[1];
    res = this->vdm_parseDouble(str);
  } else if (!wcscmp(name,L"toSeqOfChar") && p.Length() == 0) {
    res = this->vdm_toSeqOfChar();
  } else if (!wcscmp(name,L"byteValue") && p.Length() == 0) {
    res = this->vdm_byteValue ();
  } else if (!wcscmp(name,L"intValue") && p.Length() == 0) {
    res = this->vdm_intValue ();
  } else if (!wcscmp(name,L"longValue") && p.Length() == 0) {
    res = this->vdm_longValue ();
  } else if (!wcscmp(name,L"floatValue") && p.Length() == 0) {
    res = this->vdm_floatValue ();
  } else if (!wcscmp(name,L"doubleValue") && p.Length() == 0) {
    res = this->vdm_doubleValue ();
  } else {
    throw 1;
  }
  return res;
}

vdm_Double::vdm_Double ()
{
  value = 0.0;
}

vdm_Double::vdm_Double (const Real &d)
{ 
  value = d.GetValue();
}

vdm_Double::vdm_Double (const Sequence &str)
{
  value = vdm_parseDouble(str).GetValue();
}

Real vdm_Double::vdm_parseDouble (const Sequence &str)
{
  std::wstring wstr = str.GetString();
  double v;
#ifdef _MSC_VER
  swscanf(wstr.c_str(),L"%lf",&v);
#else
  std::string tmp = TBWSTR::wstring2string(wstr);
  sscanf(tmp.c_str(),"%lf",&v);
#endif // _MSC_VER
  return Real(v);
}

Sequence vdm_Double::vdm_toSeqOfChar()
{
#ifdef _MSC_VER
  wchar_t buf[80];
  swprintf(buf,L"%lf",value);
  std::wstring res (buf);
#else
  char buf[80];
  sprintf(buf,"%lf",value);
  std::wstring res = TBWSTR::string2wstring(string(buf));
#endif // _MSC_VER
  return Sequence(res);
}

Int  vdm_Double::vdm_intValue ()
{
  int i = (int)value;
  return Int(i);
}

Int  vdm_Double::vdm_byteValue ()
{
  char c = (char)value;
  return Int((long)c);
}

Int  vdm_Double::vdm_longValue ()
{
  long l = (long)value;
  return Int(l);
}

Real vdm_Double::vdm_floatValue ()
{
  return vdm_doubleValue();
}

Int  vdm_Double::vdm_shortValue ()
{
  return vdm_intValue ();
}

Real vdm_Double::vdm_doubleValue ()
{
  return Real(value);
}
