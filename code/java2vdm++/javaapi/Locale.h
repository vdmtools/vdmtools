
#ifndef __Locale_h__
#define __Locale_h__

#include <math.h>
#include "metaiv.h"
#include "Object.h"
#include "Locale_anonym.h"

class vdm_Locale : public vdm_Object
{
public:
  static const wchar_t * name;

public:
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);

  vdm_Locale ();
//  vdm_Locale (const Quote &);
  virtual ~vdm_Locale () {}

 private:

  static vdm_Locale *GetDLPtr(const DLObject& obj);
};

#endif // __Locale_h__

