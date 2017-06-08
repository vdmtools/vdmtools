#include "j2vmain.h"
#include "File.h"
#include "JavaString.h"
#include "tb_wstring.h"

//#include <math.h>
//#include <ctype.h>
#include <metaiv.h>
#include <sys/stat.h>

#include <string>
//using namespace std;

#if defined(_MSC_VER) && defined(_DEBUG)
#include <crtdbg.h>
#endif

#ifndef _MSC_VER
#define _S_IFDIR S_IFDIR
#endif // _MSC_VER

const wchar_t* vdm_File::name = L"JavaIoFile";

vdm_File *vdm_File::GetDLPtr(const DLObject& obj) {
  vdm_File *objPtr = (vdm_File*) obj.GetPtr();
  return objPtr;
}

Generic vdm_File::DlMethodCall (const wchar_t* name, const Sequence &p) {
  Generic res;
  if (!wcscmp(name,L"JavaIoFile''") && p.Length() == 1) {
    *this = vdm_File(p,0);
  } else if (!wcscmp(name,L"exists__") && p.Length() == 0) {
    res = this->vdm_exists();
  } else if (!wcscmp(name,L"getName") && p.Length() == 0) {
    res = this->vdm_getName();
  } else if (!wcscmp(name,L"getPath") && p.Length() == 0) {
    res = this->vdm_getPath();
  } else if (!wcscmp(name,L"isDirectory") && p.Length() == 0) {
    res = this->vdm_isDirectory();
  }
  return res;

}

vdm_File::vdm_File() {
  separatorChar = L'/';
  pathSeparatorChar = L':';
  return;
}

vdm_File::vdm_File(const Sequence &p, int whatcall) {

//  _CrtDbgBreak();

  int i;
  separatorChar = L'/';
  pathSeparatorChar = L':';
  if (whatcall == 0) {
//    vdm_String *argStr = vdm_String::GetDLPtr(p[1]);
    Sequence aux = p[1]; // = argStr->vdm_toStringVDM();
    aux.GetString(lpath);
    i = lpath.size()-1;
    while (i >= 0 && lpath[i] != separatorChar) {
      i--;
    }
    if (i < 0) {
      lname = lpath;
    } else {
      lname = lpath.substr(i+1,lpath.size()-i-1);

    }
  }
}

Bool vdm_File::vdm_exists() {
  struct stat aux;

  std::string slpath = TBWSTR::wstring2string(lpath);

  LOG("path=%s\n",slpath.c_str());

  if (stat(slpath.data(),&aux) == 0) {
    return Bool(true);
  } else {
    return Bool(false);
  }
}

Bool vdm_File::vdm_isDirectory() {
  struct stat aux;

  std::string slpath = TBWSTR::wstring2string(lpath);

  if (stat(slpath.data(),&aux) == 0) {
    return Bool(aux.st_mode & _S_IFDIR );
  } else {
    return Bool(false);
  }
}

DLObject vdm_File::vdm_getName() {
  wchar_t* aux=new wchar_t[lname.size()];
  wcsncpy(aux,lname.data(),lname.size());
  aux[lname.size()] = L'\0';

//  cerr << "DEBUG getName:" << lname << "." << endl;

//  vdm_String str(aux);
  DLObject res(vdm_String::name, new vdm_String (aux));
  delete [] aux;
  return res;
}

DLObject vdm_File::vdm_getPath() {
  wchar_t* aux= new wchar_t[lpath.size()];
  wcsncpy(aux,lpath.data(),lpath.size());
  aux[lpath.size()] = L'\0';
//  cerr << "DEBUG getPath:" << lpath << "." << endl;
//  vdm_String str(aux);
  DLObject res(vdm_String::name,new vdm_String (aux));
  delete [] aux;
  return res;
}

