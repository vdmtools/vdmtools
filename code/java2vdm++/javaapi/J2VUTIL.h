#ifndef __J2VUTIL_h__
#define __J2VUTIL_h__

#include "dlclass.h"
#include <math.h>
#include "metaiv.h"
#include "J2VUTIL_anonym.h"

class vdm_J2VUTIL : public DlClass
{

public:
  static const wchar_t * name;

public:
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);

  virtual Int vdm_BitOp (const Int &, const Generic &, const Int &);
  virtual Generic vdm_Convert (const type_cL &, const Generic &);
  Bool vdm_pre_Convert (const type_cL &, const Generic &);
  Char vdm_int2char(const Int &);
  Int  vdm_char2int(const Char &);
//  static DLObject vdm_ConcatStr (const vdm_String &, const vdm_String &);

 private:

  static vdm_J2VUTIL *GetDLPtr(const DLObject& obj);
};

#endif // __J2VUTIL_h__

