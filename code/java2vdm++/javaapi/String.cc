#include "j2vmain.h"
#include "JavaString.h"
#include "tb_wstring.h"

#include <math.h>
//#include <ctype.h>
#include <metaiv.h>

#include <string>
//using namespace std;

int tolower(int c);
int toupper(int c);

const wchar_t* vdm_String::name = L"JavaLangString";

vdm_String *vdm_String::GetDLPtr(const DLObject& obj) {
  vdm_String *objPtr = (vdm_String*) obj.GetPtr();
  return objPtr;
}

Generic vdm_String::DlMethodCall (const wchar_t* name, const Sequence &p) {
  type_cL sc;
  Generic res;
  if (!wcscmp(name, L"JavaLangString'")) {
    if(p.Length()==0) {
      *this=vdm_String();
//      DLObject res(vdm_String::name, this);
//      return res;
//    } else if (p.Length()==1 && p[1].IsSequence()) {
//      sc = p[1];
//      *this=vdm_String(sc);
    }
  } else if (!wcscmp(name,L"JavaLangString''")) {
      sc = p[1];
      *this=vdm_String(sc);
  } else if (!wcscmp(name,L"compareTo'") && p.Length() == 1) {
    res = this->vdm_compareTo(p);
  } else if (!wcscmp(name,L"concat'") && p.Length() == 1) {
//    res = this->vdm_concat(p);
    this->vdm_concat(p);
  } else if (!wcscmp(name,L"hashCode") && p.Length() == 0) {
    res = this->vdm_hashCode();
  } else if (!wcscmp(name,L"indexOfIC") && p.Length() == 1) {
    res = this->vdm_indexOf(p);
  } else if (!wcscmp(name,L"indexOfcLI")) {
    res = this->vdm_indexOfcLI(p);
  } else if (!wcscmp(name,L"indexOfII") && p.Length() == 2) {
    res = this->vdm_indexOf(p);
  } else if (!wcscmp(name,L"lastIndexOfI") && p.Length() == 1) {
    res = this->vdm_lastIndexOf(p);
  } else if (!wcscmp(name,L"lastIndexOfII") && p.Length() == 2) {
    res = this->vdm_lastIndexOf(p);
  } else if (!wcscmp(name,L"lastIndexOfcLI")) {
    res = this->vdm_lastIndexOfcLI(p);
  } else if (!wcscmp(name,L"length") && p.Length() == 0) {
    res = this->vdm_length();
  } else if (!wcscmp(name,L"replace") && p.Length() == 2) {
    res = this->vdm_replace(p);
  } else if (!wcscmp(name,L"toCharArray") && p.Length() == 0) {
    res = this->vdm_toCharArray();
  } else if (!wcscmp(name,L"toLowerCase") && p.Length() == 0) {
    res = this->vdm_toLowerCase();
  } else if (!wcscmp(name,L"toSeqOfChar") && p.Length() == 0) {
    res = this->vdm_toStringVDM();
  } else if (!wcscmp(name,L"toUpperCase") && p.Length() == 0) {
    res = this->vdm_toUpperCase();
  } else if (!wcscmp(name, L"charAt")) {
    res = this->vdm_charAt(p[1]);
  } else if (!wcscmp(name,L"valueOfBool") && p.Length() == 1) {
    res = this->vdm_valueOfBool(p);
  } else if (!wcscmp(name,L"valueOfChar") && p.Length() == 1) {
    res = this->vdm_valueOfChar(p);
  } else if (!wcscmp(name,L"valueOfSeqOfChar") && p.Length() == 1) {
    res = this->vdm_valueOfSeqOfChar(p);
  } else if (!wcscmp(name,L"valueOfInt") && p.Length() == 1) {
    res = this->vdm_valueOfInt(p);
  } else if (!wcscmp(name,L"valueOfReal") && p.Length() == 1) {
    res = this->vdm_valueOfReal(p);
  } else {
    throw 1;
  }
  return res;
}

vdm_String::vdm_String ()
{
}

vdm_String::vdm_String (const type_cL &seqOfChar)
{
//  value.erase();
  seqOfChar.GetString(value);
}

vdm_String::vdm_String (const wchar_t *str)
{
//  value.erase();
  value = str;
}

vdm_String::vdm_String (const wstring& s)
{
  value = s;
}

vdm_String::vdm_String (const vdm_String &vdm_value_u)
{
  value = vdm_value_u.value;
}

const vdm_String& vdm_String::operator=(const vdm_String &s) {
  value = s.value;
  return *this;
}

Int vdm_String::vdm_compareTo (const Sequence &p) {
  int res;
//  vdm_String *argStr = GetDLPtr(p[1]);
//  res = value.compare(argStr->value.c_str());
  wstring v;
  Sequence s(p[1]);
  for(int i=1; i<=s.Length(); i++) {
    v.append(1, Char(s[i]).GetValue());
  }
  res = value.compare(v.c_str());
  return Int(res);
}

void vdm_String::vdm_concat (const Sequence &p)
{
  Sequence s(p[1]);
//  for(int i=1; i<=s.Length(); i++) {
//    value.append(1, Char(s[i]).GetValue());
//  }
  value.append(s.GetString());
}

Int vdm_String::vdm_hashCode () {
  int i;
  int j = 0;
  long res = 0;
  int n = value.length();
  for (i=n-1;i>=0;i--) {
//    res += value[j] * (long) pow(31,i);
    res += value[j] * (long) pow((double)31,i);
    j++;
  }
  return Int(res);

}

Int vdm_String::vdm_indexOf(const Sequence &p) {
  string sparams = TBWSTR::wstring2string(p.ascii());
  LOG("vdm_String::vdm_indexOf(%s)\n",sparams.c_str());

  int found;
  int search;
  if (p[1].IsChar()) {
    search = (wchar_t)((Char)p[1]).GetValue();
  } else {
    search = ((Int) p[1]).GetValue();
  }
  Int start_search = Int(0);
  if (p.Length() >= 2) {
    start_search = (Int) p[2];
  }
  found = value.find(search,start_search.GetValue());
  if (found == wstring::npos) {
    return Int(-1);
  } else {
    return Int(found);
  }
}

Int vdm_String::vdm_indexOfcLI(const Sequence &p)
{
  string sparams = TBWSTR::wstring2string(p.ascii());
  LOG("vdm_String::vdm_indexOfcLI(%s)\n",sparams.c_str());

  int start_search=(Int)p[2];
  int found; 
  wstring search;
  type_cL sc = p[1];
  sc.GetString(search);

  found = value.find(search,start_search);
  if (found == wstring::npos) {
    return Int(-1);
  } else {
    return Int(found);
  }
}

Int vdm_String::vdm_lastIndexOf(const Sequence &p) {
  string sparams = TBWSTR::wstring2string(p.ascii());
  LOG("vdm_String::vdm_lastIndexOf(%s)\n",sparams.c_str());

  int found;
  wchar_t search = ((Int) p[1]).GetValue();
  Int start_search = Int(value.length());
  if (p.Length() >= 2) {
    start_search = (Int) p[2];
  }
  found = value.rfind(search,start_search.GetValue());
  if (found == wstring::npos) {
    return Int(-1);
  } else {
    return Int(found);
  }
}

Int vdm_String::vdm_lastIndexOfcLI(const Sequence &p)
{
  string sparams = TBWSTR::wstring2string(p.ascii());
  LOG("vdm_String::vdm_lastIndexOfcLI(%s)\n",sparams.c_str());

  int start_search=(Int)p[2];
  int found; 
  wstring search;
  type_cL sc = p[1];
  sc.GetString(search);

  found = value.rfind(search,start_search);
  if (found == wstring::npos) {
    return Int(-1);
  } else {
    return Int(found);
  }
}

Int vdm_String::vdm_length() {
  return Int(value.length());
}

DLObject vdm_String::vdm_replace (const Sequence &p) {
  int pos;
  wstring search = wstring(1,((Char) p[1]).GetValue());
  wstring subs   = wstring(1,((Char) p[2]).GetValue());
  pos = value.find(search,0);
  while (pos != wstring::npos) {
    value.replace(pos,1,subs);
    pos = value.find(search,pos);
  }
  DLObject res(vdm_String::name,this);
  return res;
}

type_icM vdm_String::vdm_toCharArray() {
  int i;
  type_icM res;
  for (i=0;i<value.length();i++) {
    res.Insert(Int(i),Char(value[i]));
  }
  return res;
}

DLObject vdm_String::vdm_toLowerCase () {
  int i;
  for (i=0;i<value.length();i++)
    value[i] = tolower(value[i]);
  DLObject res(vdm_String::name,this);
  return res;
}

Sequence vdm_String::vdm_toStringVDM() {
//  return Sequence(value);
//
//  Just to avoid annoying debug breaks in J2VLIB!free...
//
  Sequence res;
  for(int i=0; i<value.length(); i++) {
    res.ImpAppend(Char(value[i]));
  }
  return res;
}

DLObject vdm_String::vdm_toUpperCase () {
  int i;
  for (i=0;i<value.length();i++)
    value[i] = toupper(value[i]);
  DLObject res(vdm_String::name,this);
  return res;
}

Char vdm_String::vdm_charAt (const Int &vdm_index)
{
  int len=value.length();
  int idx = (int)vdm_index;
  if (0<=idx && idx<len) {
    return Char (value[idx]);
  } else {
    return Char ('\0');
  }
}

DLObject vdm_String::vdm_valueOfBool(const Sequence &p) {
  if (((Bool) p[1]).GetValue()) {
    value.erase();
    value = L"true";
  } else {
    value.erase();
    value = L"false";
  }
  DLObject res(vdm_String::name,this);
  return res;
}

DLObject vdm_String::vdm_valueOfChar(const Sequence &p) {
  wchar_t c = ((Char) p[1]).GetValue();
  value.erase();
  value = c;
  DLObject res(vdm_String::name,this);
  return res;
}

DLObject vdm_String::vdm_valueOfSeqOfChar(const Sequence &p) {
  type_cL seqOfChar = p[1];
  value.erase();
  seqOfChar.GetString(value);
  DLObject res(vdm_String::name,this);
  return res;
}

DLObject vdm_String::vdm_valueOfInt(const Sequence &p) {
  long i = ((Int) p[1]).GetValue();
#ifdef _MSC_VER
  wchar_t aux[80];
  swprintf(aux,L"%d",i);
  value = aux;
#else
  char aux[1024];
  sprintf(aux,"%ld",i);
  value = TBWSTR::string2wstring(string(aux));
#endif // _MSC_VER
  DLObject res(vdm_String::name,this);
  return res;
}

DLObject vdm_String::vdm_valueOfReal(const Sequence &p) {
  double f = ((Real) p[1]).GetValue();
#ifdef _MSC_VER
  wchar_t aux[80];
  swprintf(aux,L"%f",f);
  value = aux;
#else
  char aux[80];
  sprintf(aux,"%f",f);
  value = TBWSTR::string2wstring(string(aux));
#endif // _MSC_VER
  DLObject res(vdm_String::name,this);
  return res;
}
