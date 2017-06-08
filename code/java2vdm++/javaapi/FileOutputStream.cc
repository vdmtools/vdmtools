#include "j2vmain.h"
#include "FileOutputStream.h"
#include "JavaString.h"
#include "FileDescriptor.h"
#include "tb_wstring.h"

#if defined(_MSC_VER) && defined(_DEBUG)
#include <crtdbg.h>
#endif // _MSC_VER

#ifdef __linux__
#include <stdlib.h>
#include <string.h>
#endif // __linux__

#ifdef __Cygwin__
#include <stdlib.h>
#include <string.h>
#endif // __Cygwin__

const wchar_t* vdm_FileOutputStream::name = L"JavaIoFileOutputStream";

Generic vdm_FileOutputStream::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  string sname   = TBWSTR::wstring2string(wstring(name));
  string sparams = TBWSTR::wstring2string(p.ascii());
  LOG("vdm_FileOutputStream::%s(%s)\n",sname.c_str(),sparams.c_str());

  Generic res;

  if (!wcscmp (name, L"writeInt")) {
    res=this->vdm_writeInt(p[1]); 
  } else if (!wcscmp (name, L"write")) {
    this->vdm_write(p[1]); 
  } else if (!wcscmp(name, L"write1")) {
    this->vdm_write1(p[1]);
  } else if (!wcscmp(name, L"write2")) {
    this->vdm_write2(p[1],p[2],p[3]);
  } else if (!wcscmp(name, L"close")) {
    this->vdm_close();
  } else if (!wcscmp(name, L"open")) {
    if(p[1].IsSequence()) {
      vdm_String str(p[1]);
      res=open(str);
//  } else {
//      vdm_String filename = *vdm_String::GetDLPtr(p[1]);
//      res=open(filename);
    }
  } else if (!wcscmp(name, L"openAppend")) {
    if(p[1].IsSequence()) {
      vdm_String str(p[1]);
      res=openAppend(str);
//    } else {
//      vdm_String filename = *vdm_String::GetDLPtr(p[1]);
//      res=openAppend(filename);
    }
  } else {
      // the method does not exist
  }
  return res;
}

vdm_FileOutputStream::vdm_FileOutputStream ()
{
}

#ifndef DEF_FileOutputStream_FileOutputStream

vdm_FileOutputStream::vdm_FileOutputStream (const vdm_String &vdm_name)
{  
  open(vdm_name);
}

Int vdm_FileOutputStream::open (const vdm_String &vdm_name)
{ 
  int res = 0;
  if(!out.is_open()) {
    size_t len = vdm_name.getValue().length();
    char* dst=(char*)malloc(sizeof(wchar_t)*len+1);
    memset(dst,0,sizeof(wchar_t)*len+1);
    const wchar_t *src = vdm_name.getValue().c_str();
    wcsrtombs(dst, &src, len, NULL); 

    string fn(dst);
    delete dst;

    out.open(fn.c_str(),ios::out);
//    res = out.is_open() ? 0 : -1;
    res=out.rdstate();
  }
  return Int(res);
}

Int vdm_FileOutputStream::openAppend (const vdm_String &vdm_name)
{ 
  int res = 0;
  if(!out.is_open()) {
    size_t len = vdm_name.getValue().length();
    char* dst=(char*)malloc(sizeof(wchar_t)*len+1);
    memset(dst,0,sizeof(wchar_t)*len+1);
    const wchar_t *src = vdm_name.getValue().c_str(); 
    wcsrtombs(dst, &src, len, NULL); 

    string fn(dst);
    delete dst;

    out.open(fn.c_str(),ios::app);
//    res = out.is_open() ? 0 : -1;
    res=out.rdstate();
  }
  return Int(res);
}


#endif

void vdm_FileOutputStream::vdm_write (const Int & vdm_i)
{
  vdm_writeInt (vdm_i);
}

Int vdm_FileOutputStream::vdm_writeInt (const Int & vdm_i)
{
//  _CrtDbgBreak();
  int res = 0; 
  if(out.rdstate()==ios::goodbit) {
    int i = vdm_i.GetValue();
    out.put(i);
  }
  res=out.rdstate();
  return Int(res);
}

void vdm_FileOutputStream::vdm_write1 (const type_iiM &)
{}

void vdm_FileOutputStream::vdm_write2 (const type_iiM &, const Int &, const Int &)
{}

vdm_FileOutputStream *vdm_FileOutputStream::GetDLPtr(const DLObject& obj)
{
  vdm_FileOutputStream *objPtr = (vdm_FileOutputStream*) obj.GetPtr(); 
  return objPtr;
}

void vdm_FileOutputStream::vdm_finalize ()
{}

void vdm_FileOutputStream::vdm_close ()
{
  out.close();
}

DLObject vdm_FileOutputStream::vdm_getFD ()
{
  return DLObject(L"FileOutputStream", new vdm_FileOutputStream());
}

