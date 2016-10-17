#ifndef __File_h__
#define __File_h__

#include "Object.h"
//#include <m_apm.h>

#include <string>
//using namespace std;

class vdm_File: public vdm_Object
{
public:
  static const wchar_t * name;

protected:
  wstring lpath;
  wstring lname;
  wchar_t pathSeparatorChar;
  wchar_t separatorChar;

  private:
  static vdm_File *GetDLPtr(const DLObject &obj);

  public:
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);

  vdm_File ();
  vdm_File(const Sequence &,int);

  virtual Bool vdm_exists();
  virtual DLObject vdm_getName ();
  virtual DLObject vdm_getPath ();
  virtual Bool vdm_isDirectory();
};

#endif // __File_h__
