#ifndef __FileInputStream_h__
#define __FileInputStream_h__

#include "dlclass.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include "metaiv.h"
#include "JavaString.h"

class vdm_FileInputStream : public DlClass
{
public:
  static const wchar_t * name;

  ifstream in;

public:
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);

  virtual const wchar_t * GetTypeName () const { return vdm_FileInputStream::name; }

  vdm_FileInputStream (const vdm_String &vdm_name);
  vdm_FileInputStream ();
  virtual ~vdm_FileInputStream () {}

protected:
  virtual void vdm_finalize ();
public:
  virtual Int vdm_read ();
  virtual void vdm_close ();
//  virtual type_ref_FileDescriptor vdm_getFD ();
  virtual Int vdm_skip_u_u (const Int &);
  virtual Int vdm_available ();

private:
  virtual Int vdm_open (const vdm_String &vdm_name);
  //virtual type_iiiM2P vdm_readBytes (const Int &);
  virtual Tuple vdm_readBytes (const Int &);

  static vdm_FileInputStream *GetDLPtr(const DLObject& obj);

};

#endif // __FileInputStream_h__

