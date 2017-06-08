#include <iostream>
#include <fstream>
#include "j2vmain.h"
#include "FileInputStream.h"
#include "JavaString.h"
#include "FileDescriptor.h"
#ifdef DEF_FileInputStream_USERIMPL
#include "FileInputStream_userimpl.cpp"
#endif

#ifdef __linux__
#include <stdlib.h>
#include <string.h>
#endif // __linux__

#ifdef __Cygwin__
#include <stdlib.h>
#include <string.h>
#endif // __Cygwin__

const wchar_t* vdm_FileInputStream::name = L"JavaIoFileInputStream";

Generic vdm_FileInputStream::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  Generic res;

//  string sname   = wstring2string(wstring(name));
//  string sparams = wstring2string(p.ascii());
//  LOG("vdm_StringBuffer::%s(%s)\n",sname.c_str(),sparams.c_str());

  if (!wcscmp (name, L"read'")) {
    res=this->vdm_read(); 
  } else if (!wcscmp (name, L"readBytes'")) {
    res=this->vdm_readBytes(p[1]); 
  } else if (!wcscmp (name, L"skip__")) {
    res=this->vdm_skip_u_u(p[1]); 
  } else if (!wcscmp(name, L"available")) {
    res=this->vdm_available();
  } else if (!wcscmp(name, L"finalize")) {
    this->vdm_finalize();
  } else if (!wcscmp(name, L"close")) {
    this->vdm_close();
  } else if (!wcscmp(name, L"open")) {
    if(p[1].IsSequence()) {
      vdm_String str(p[1]);
      res=vdm_open(str);
//    } else {
//      vdm_String filename = *vdm_String::GetDLPtr(p[1]);
//      res=vdm_open(filename);
    }
  } else {
      // the method does not exist
  }
  return res;
}

vdm_FileInputStream::vdm_FileInputStream ()
{
}

#ifndef DEF_FileInputStream_open

Int vdm_FileInputStream::vdm_open (const vdm_String &vdm_name)
{
  int res = 0;
  if(!in.is_open()) {
//    mbstate_t ps;
    size_t len = vdm_name.getValue().length();
    char* dst=(char*)malloc(sizeof(wchar_t)*len+1);
    memset(dst,0,sizeof(wchar_t)*len+1);
    const wchar_t *src = vdm_name.getValue().c_str(); 
    wcsrtombs(dst, &src, len, NULL);

    string fn(dst);
    delete dst;

    in.open(fn.c_str(),ios::in);
//    res = out.is_open() ? 0 : -1;
    res=in.rdstate();
//    if(in.rdstate()!=ios::goodbit) in.setmode(filebuf::binary);
  } 
  return Int(res);
}

#endif

#ifndef DEF_FileInputStream_read

Int vdm_FileInputStream::vdm_read ()
{
  int res = -1; int c;
  if(!(in.rdstate()&ios::eofbit)) { 
    if(in.rdstate()==ios::goodbit) {
//      in >> c;
      c=in.get();

      if(in.rdstate()!=ios::goodbit) { 
        if(in.rdstate()&ios::eofbit) { 
          res=-1;
        } else {
          res=-2;
        }
      } else {
        res = (int)c;
      }
    }
  }
  return Int(res);
}

#endif

#ifndef DEF_FileInputStream_close

void vdm_FileInputStream::vdm_close ()
{
  in.close();
}

#endif

#if 0

vdm_FileDescriptor vdm_FileInputStream::vdm_getFD ()
{
}

#endif

#ifndef DEF_FileInputStream_skip_u_u

Int vdm_FileInputStream::vdm_skip_u_u (const Int &vdm_n)
{
  int i=vdm_n.GetValue();
  int res=0;
  while(i>0) {
    Int r=vdm_read ();
    if(r.GetValue() < 0) break;
    i--;
    res++;
  }
  return Int(res);
}

#endif

#ifndef DEF_FileInputStream_finalize

void vdm_FileInputStream::vdm_finalize ()
{
  vdm_close ();
}

#endif

#ifndef DEF_FileInputStream_available

Int vdm_FileInputStream::vdm_available ()
{
  int res;
  int i = in.peek();
  return Int(i==EOF ? 0 : 1);
//    return Int (in.rdbuf()->in_avail());
}

#endif

#ifndef DEF_FileInputStream_readBytes

//type_iiiM2P vdm_FileInputStream::vdm_readBytes (const Int& nBytes)
Tuple vdm_FileInputStream::vdm_readBytes (const Int& nBytes)
{
  int idx = 0;
  int n = nBytes.GetValue();
  //type_iiiM2P res;
  //type_iiM map;
  Map map;
  int success=0;
  int i;

  i = vdm_read().GetValue();
  if(i<0) {
    success=i;
  } else {
    map.ImpModify(Int(idx),Int(i));
    n--;
    idx++;
    while(n>0) {
      i = vdm_read().GetValue();
      if(i<0) {
        success=i;
        break;
      }
      map.ImpModify(Int(idx),Int(i));
      n--;
      idx++;
    }
    if(success==0 || success==-1) success=idx; 
  }
  //res.set_1(Int(success));
  //res.set_2(map);
  //return res;
  return mk_(Int(success), map);
}

#endif


#ifndef DEF_FileInputStream_FileInputStream

vdm_FileInputStream::vdm_FileInputStream (const vdm_String &vdm_name)
{ 
  vdm_open(vdm_name);
}

#endif

vdm_FileInputStream *vdm_FileInputStream::GetDLPtr(const DLObject& obj)
{
  vdm_FileInputStream *objPtr = (vdm_FileInputStream*) obj.GetPtr(); 
  return objPtr;
}
