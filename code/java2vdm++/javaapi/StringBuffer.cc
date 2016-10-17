#include "j2vmain.h"
#include "JavaString.h"
#include "StringBuffer.h"
#include "tb_wstring.h"

#define BUFFER_SIZE 100

#if defined(_MSC_VER) && defined(_DEBUG)
#include <crtdbg.h>
#endif

#ifdef __linux__
#include <stdlib.h>
#endif // __linux__

#ifdef __Cygwin__
#include <stdlib.h>
#endif // __Cygwin__

const wchar_t* vdm_StringBuffer::name = L"JavaLangStringBuffer";

vdm_StringBuffer *GetDLPtr(const DLObject& obj) {
  vdm_StringBuffer *objPtr = (vdm_StringBuffer*) obj.GetPtr();
  return objPtr;
}

Generic vdm_StringBuffer::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  string sname   = TBWSTR::wstring2string(wstring(name));
  string sparams = TBWSTR::wstring2string(p.ascii());
  LOG("vdm_StringBuffer::%s(%s)\n",sname.c_str(),sparams.c_str());

  Int cp;
  type_cL sc;
  Generic res;
  if (!wcscmp(name, L"JavaLangStringBuffer'")) {
    if(p.Length()==0) {
      *this=vdm_StringBuffer();
    } else if (p.Length()==1 && p[1].IsSequence()) {
      sc = p[1];
      *this=vdm_StringBuffer(sc);
    } else if (p.Length()==1 && p[1].IsInt()) {
      cp = (Int) p[1];
      *this=vdm_StringBuffer(cp);
    } else if (p.Length()==1 && p[1].IsReal()) {
      cp = Int((int)((Real)p[1]).GetValue());
      *this=vdm_StringBuffer(cp);
    }
/*
    else {    // No String any more
      vdm_String *argStr = vdm_String::GetDLPtr(p[1]);
      *this=vdm_StringBuffer(argStr->value);
    }
*/
  } else if (!wcscmp(name,L"toString")) {
    DLObject str(vdm_String::name,new vdm_String(strbuffer));
    res=str;
  } else if (!wcscmp(name,L"append'") && p.Length() == 1) {
    res = this->vdm_append(p);
  } else if (!wcscmp(name,L"capacity") && p.Length() == 0) {
    res = this->vdm_capacity();
  } else if (!wcscmp(name,L"deleteCharAt'") && p.Length() == 1) {
    res = this->vdm_deleteCharAt(p);
  } else if (!wcscmp(name,L"ensureCapacity") && p.Length() == 1) {
    this->vdm_ensureCapacity(p);
  } else if (!wcscmp(name,L"insert'") && p.Length() == 2) {
    res = this->vdm_insert(p);
  } else if (!wcscmp(name,L"length") && p.Length() == 0) {
    res = this->vdm_length();
  } else if (!wcscmp(name,L"toSeqOfChar") && p.Length() == 0) {
    res = this->vdm_toStringVDM();
  }
  return res;
}

vdm_StringBuffer::vdm_StringBuffer() {
  buffer_size  = BUFFER_SIZE;
  strbuffer = (wchar_t *) calloc(buffer_size+1,sizeof(wchar_t));
}

vdm_StringBuffer::~vdm_StringBuffer() {
  free(strbuffer);
}

vdm_StringBuffer::vdm_StringBuffer(const Int &capacity) {
  buffer_size  = capacity.GetValue();
  strbuffer = (wchar_t *) calloc(buffer_size+1,sizeof(wchar_t));
}

vdm_StringBuffer::vdm_StringBuffer(const type_cL &seqOfChar) {
  wstring value;
  seqOfChar.GetString(value);
  buffer_size  = BUFFER_SIZE + value.length();
  strbuffer = (wchar_t *) calloc(buffer_size+1,sizeof(wchar_t));
  wcsncpy(strbuffer,value.data(),value.length());
}

vdm_StringBuffer::vdm_StringBuffer(const wstring &seqOfChar) {
  buffer_size  = BUFFER_SIZE + seqOfChar.length();
  strbuffer = (wchar_t *) calloc(buffer_size+1,sizeof(wchar_t));
  wcscpy(strbuffer,seqOfChar.data());
}

const vdm_StringBuffer& vdm_StringBuffer::operator=(const vdm_StringBuffer &s) {
  buffer_size = s.buffer_size;
  strbuffer = (wchar_t *) calloc(buffer_size+1,sizeof(wchar_t));
  wcscpy(strbuffer,s.strbuffer);
  return *this;
}

DLObject vdm_StringBuffer::vdm_append(const Sequence &p)
{
  string sstrbuffer = TBWSTR::wstring2string(wstring(strbuffer));
  LOG("strbuffer=%s\n",sstrbuffer.c_str());

  // Must ensure capacity before appending data.
  wchar_t *l_chars;
  wstring l_str;
  int sb_len;
  if (p[1].IsReal() || p[1].IsInt() || p[1].IsBool()) {
    l_str = p[1].ascii();
    sb_len = l_str.length();
    if (wcslen(strbuffer) + sb_len > buffer_size) {
      l_chars = (wchar_t *) calloc(2*(wcslen(strbuffer)+sb_len),sizeof(wchar_t));
      buffer_size = 2*(wcslen(strbuffer)+sb_len);
      wcscpy(l_chars,strbuffer);
      wcsncat(l_chars,l_str.data(),l_str.length());
//      wcscpy(strbuffer,l_chars);
      free(strbuffer);
      strbuffer = l_chars;
    } else {
      wcsncat(strbuffer,l_str.data(),l_str.length());
    }
  } else if (p[1].IsChar()) {
    if (wcslen(strbuffer) + 1 > buffer_size) {
      l_chars = (wchar_t *) calloc(2*(wcslen(strbuffer)+1),sizeof(wchar_t));
      buffer_size = 2*(wcslen(strbuffer)+1);
      wcscpy(l_chars,strbuffer);
      sb_len = wcslen(strbuffer);
      l_chars[sb_len] = ((Char) p[1]).GetValue();
      l_chars[sb_len+1] = '\0';
//      wcscpy(strbuffer,l_chars);
      free(strbuffer);
      strbuffer = l_chars;
    } else {
      sb_len = wcslen(strbuffer);
      strbuffer[sb_len] = ((Char) p[1]).GetValue();
      strbuffer[sb_len+1] = '\0';
    }
  } else if (p[1].IsSequence()) {
    if (((Sequence) p[1]).GetString(l_str)) {
      if (wcslen(strbuffer) + l_str.length() > buffer_size) {
        l_chars = (wchar_t *) calloc(2*(wcslen(strbuffer)+l_str.length()),sizeof(wchar_t));
        buffer_size = 2*(wcslen(strbuffer)+l_str.length());
        wcscpy(l_chars,strbuffer);
        wcsncat(l_chars,l_str.data(),l_str.length());
//        wcscpy(strbuffer,l_chars);
        free(strbuffer);
        strbuffer = l_chars;
      } else {
        wcsncat(strbuffer,l_str.data(),l_str.length());
      }
    }
  }
/*  else {  // No strings any more
    vdm_String *argStr = vdm_String::GetDLPtr(p[1]);
    l_str= argStr->value;
    if (wcslen(strbuffer) + l_str.length() > buffer_size) {
      int newsize = 2*(wcslen(strbuffer)+l_str.length());
      l_chars = (wchar_t *) calloc(newsize,sizeof(wchar_t));
      buffer_size = newsize;
      wcscpy(l_chars,strbuffer);
      wcsncat(l_chars,l_str.data(),l_str.length());
//      wcscpy(strbuffer,l_chars);
      free(strbuffer);
      strbuffer=l_chars;
    } else {
      wcsncat(strbuffer,l_str.data(),l_str.length());
    }
  }
*/
  DLObject res(vdm_StringBuffer::name,this);
  return res;
}

Int vdm_StringBuffer::vdm_capacity() {
  return Int(buffer_size);
}

DLObject vdm_StringBuffer::vdm_deleteCharAt(const Sequence &p) {
  int l_index;
  int l_size;
  l_size = wcslen(strbuffer);
  l_index = ((Int) p[1]).GetValue();
  wcscpy(strbuffer+l_index,strbuffer+l_index+1);
  DLObject res(vdm_StringBuffer::name,this);
  return res;
}

void vdm_StringBuffer::vdm_ensureCapacity(const Sequence &p)
{
  wchar_t *l_chars;
  int l_newcap = ((Int) p[1]).GetValue();
  if (buffer_size < l_newcap) {
    if (2*wcslen(strbuffer)+2 > l_newcap) {
      buffer_size = 2*wcslen(strbuffer)+2;
    } else {
      buffer_size = l_newcap;
    }
    l_chars = (wchar_t *) calloc(buffer_size+1,sizeof(wchar_t));
    wcscpy(l_chars,strbuffer);
    free(strbuffer);
    strbuffer = l_chars;
  }
}

DLObject vdm_StringBuffer::vdm_insert(const Sequence &p) {
  wstring l_str;
  wchar_t *l_chars;
  int sb_len;
  int l_offset = ((Int) p[1]).GetValue();
  if (p[2].IsReal() || p[2].IsInt() || p[2].IsBool()) {
    l_str = p[2].ascii();
    sb_len = l_str.length();
    if (wcslen(strbuffer) + sb_len > buffer_size) {
      l_chars = (wchar_t *) calloc(2*(wcslen(strbuffer)+sb_len),sizeof(wchar_t));
      buffer_size = 2*(wcslen(strbuffer)+sb_len);
      wcsncpy(l_chars,strbuffer,l_offset);
      wcsncat(l_chars,l_str.data(),l_str.length());
      wcscat(l_chars,strbuffer+l_offset);
      free(strbuffer);
      strbuffer=l_chars;
    } else {
      l_chars = (wchar_t *) calloc(buffer_size+1,sizeof(wchar_t));
      wcsncpy(l_chars,strbuffer,l_offset);
      wcsncat(l_chars,l_str.data(),l_str.length());
      wcscat(l_chars,strbuffer+l_offset);
      free(strbuffer);
      strbuffer=l_chars;
    }
  } else if (p[2].IsChar()) {
    if (wcslen(strbuffer) + 1 > buffer_size) {
      l_chars = (wchar_t *) calloc(2*(wcslen(strbuffer)+1),sizeof(wchar_t));
      buffer_size = 2*(wcslen(strbuffer)+1);
      wcsncpy(l_chars,strbuffer,l_offset);
      l_chars[l_offset] = ((Char) p[2]).GetValue();
      l_chars[l_offset+1] = L'\0';
      wcscat(l_chars,strbuffer+l_offset);
      free(strbuffer);
      strbuffer=l_chars;
    } else {
      l_chars = (wchar_t *) calloc(buffer_size+1,sizeof(wchar_t));
      wcsncpy(l_chars,strbuffer,l_offset);
      l_chars[l_offset] = ((Char) p[2]).GetValue();
      l_chars[l_offset+1] = L'\0';
      wcscat(l_chars,strbuffer+l_offset);
      free(strbuffer);
      strbuffer=l_chars;
    }
  } else if (p[2].IsSequence()) {
    if (((Sequence) p[2]).GetString(l_str)) {
      if (wcslen(strbuffer) + l_str.length() > buffer_size) {
        l_chars = (wchar_t *) calloc(2*(wcslen(strbuffer)+l_str.length()),sizeof(wchar_t));
        buffer_size = 2*(wcslen(strbuffer)+l_str.length());
        wcsncpy(l_chars,strbuffer,l_offset);
        wcsncat(l_chars,l_str.data(),l_str.length());
        wcscat(l_chars,strbuffer+l_offset);
        free(strbuffer);
        strbuffer=l_chars;
      } else {
        l_chars = (wchar_t *) calloc(buffer_size+1,sizeof(wchar_t));
        wcsncpy(l_chars,strbuffer,l_offset);
        wcsncat(l_chars,l_str.data(),l_str.length());
        wcscat(l_chars,strbuffer+l_offset);
        free(strbuffer);
        strbuffer=l_chars;
      }
    }
  }
/*
  else {    // No Strings any more
    vdm_String *argStr = vdm_String::GetDLPtr(p[2]);
    l_str=argStr->value;
    if (wcslen(strbuffer) + l_str.length() > buffer_size) {
      l_chars = (wchar_t *) calloc(2*(wcslen(strbuffer)+l_str.length()),sizeof(wchar_t));
      buffer_size = 2*(wcslen(strbuffer)+l_str.length());
      wcsncpy(l_chars,strbuffer,l_offset);
      wcsncat(l_chars,l_str.data(),l_str.length());
      wcscat(l_chars,strbuffer+l_offset);
      free(strbuffer);
      strbuffer=l_chars;
    } else {
      l_chars = (wchar_t *) calloc(buffer_size+1,sizeof(wchar_t));
      wcsncpy(l_chars,strbuffer,l_offset);
      wcsncat(l_chars,l_str.data(),l_str.length());
      wcscat(l_chars,strbuffer+l_offset);
      free(strbuffer);
      strbuffer=l_chars;
    }
  }
*/
  DLObject res(vdm_StringBuffer::name,this);
  return res;
}


Int vdm_StringBuffer::vdm_length() {
  return Int(wcslen(strbuffer));
}

Sequence vdm_StringBuffer::vdm_toStringVDM() {
//  return Sequence(wstring(strbuffer));
//
//  Just to avoid annoying debug breaks in J2VLIB!free...
//
  string sstrbuffer = TBWSTR::wstring2string(wstring(strbuffer));
  LOG("strbuffer=%s\n",sstrbuffer.c_str());

  int len = wcslen(strbuffer);
  Sequence res;
  for(int i=0; i<len; i++) {
    res.ImpAppend(Char(strbuffer[i]));
  }
  return res;
}


