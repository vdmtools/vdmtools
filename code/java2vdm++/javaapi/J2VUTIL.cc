#include "j2vmain.h"
#include "J2VUTIL.h"
#include "JavaString.h"
#include "tb_wstring.h"
#ifdef DEF_J2VUTIL_USERIMPL
#include "J2VUTIL_userimpl.cpp"

#endif

const wchar_t* vdm_J2VUTIL::name = L"J2VUTIL";

Generic vdm_J2VUTIL::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  string sname   = TBWSTR::wstring2string(wstring(name));
  string sparams = TBWSTR::wstring2string(p.ascii());
  LOG("vdm_J2VUTIL::%s(%s)\n",sname.c_str(),sparams.c_str());

  Generic res;

  if (!wcscmp (name, L"Convert"))
    res = this->vdm_Convert(p[1], p[2]); 
  else if (!wcscmp(name, L"BitOp'")) {
    res = this->vdm_BitOp(p[1],p[2],p[3]);
//  else if (!wcscmp(name, L"vdm_ConcatStr")) {
//    DLObject o1(p[1]);
//    DLObject o2(p[2]);
//    vdm_String* s1 = vdm_String::GetDLPtr(o1);
//    vdm_String* s2 = vdm_String::GetDLPtr(o2);
//    res = this->vdm_ConcatStr(*s1,*s2);
  } else if (!wcscmp(name, L"int2char")) {
    res = this->vdm_int2char(p[1]);
  } else if (!wcscmp(name, L"char2int")) {
    res = this->vdm_char2int(p[1]);
  } else {
      // the method does not exist
    throw 1;
  }

  return res;
}

Char vdm_J2VUTIL::vdm_int2char(const Int& i)
{
  return Char((wchar_t)i.GetValue());
}

Int  vdm_J2VUTIL::vdm_char2int(const Char& c)
{
  return Int((int)c.GetValue());
}

#ifndef DEF_J2VUTIL_BitOp

Int vdm_J2VUTIL::vdm_BitOp (const Int &vdm_p_uleft, const Generic &vdm_p_uop, const Int &vdm_p_uright)
{ 
  int left  = vdm_p_uleft.GetValue();
  int right = vdm_p_uright.GetValue();
  Quote opQuote(vdm_p_uop);
  wstring op = opQuote.GetValue();
  if(op==L"AND") {
    left = left & right;
  } else if (op==L"OR") {
    left = left | right;
  } else if (op==L"XOR") {
    left = left ^ right;
  } else {
    throw 1;
  }
  return Int(left);
}

#endif

#ifndef DEF_J2VUTIL_Convert

Generic vdm_J2VUTIL::vdm_Convert (const type_cL &vdm_p_utype, const Generic &vdm_p_uval)
{
  return  (Generic)Int(111);
}

#endif

#ifndef DEF_J2VUTIL_pre_Convert

Bool vdm_J2VUTIL::vdm_pre_Convert (const type_cL &vdm_p_utype, const Generic &vdm_p_uval) {
  
  return new Bool(true);

}

#endif
/*
#ifndef DEF_J2VUTIL_ConcatStr

DLObject vdm_J2VUTIL::vdm_ConcatStr (const vdm_String &vdm_p_ustr1, const vdm_String &vdm_p_ustr2)
{
  vdm_String s = vdm_p_ustr1;
  return s.vdm_concat(vdm_p_ustr2);
}

#endif
*/
vdm_J2VUTIL *vdm_J2VUTIL::GetDLPtr(const DLObject& obj)
{
  vdm_J2VUTIL *objPtr = (vdm_J2VUTIL*) obj.GetPtr(); 
  return objPtr;
}

