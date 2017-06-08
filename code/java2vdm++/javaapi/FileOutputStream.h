#ifndef __FileOutputStream_h__
#define __FileOutputStream_h__

#include <fstream>
#include <math.h>
#include <math.h>
#include "metaiv.h"
#include "OutputStream.h"
#include "FileOutputStream_anonym.h"


class vdm_FileOutputStream : public vdm_OutputStream
{
public:
  static const wchar_t * name;

  ofstream out;

public:
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);

  virtual const wchar_t * GetTypeName () const { return vdm_FileOutputStream::name; }

  vdm_FileOutputStream (const vdm_String &);
  vdm_FileOutputStream ();
  virtual ~vdm_FileOutputStream () {}

protected:
  virtual void vdm_finalize ();
public:
  virtual void vdm_close ();
  virtual DLObject vdm_getFD ();
  virtual void vdm_write (const Int &);
  virtual Int vdm_writeInt (const Int &);
  virtual void vdm_write1 (const type_iiM &);
  virtual void vdm_write2 (const type_iiM &, const Int &, const Int &);

 private:

  Int open (const vdm_String &vdm_name);
  Int openAppend (const vdm_String &vdm_name);

  static vdm_FileOutputStream *GetDLPtr(const DLObject& obj);

}
;

#endif // __FileOutputStream_h__

