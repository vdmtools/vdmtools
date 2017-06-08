#ifndef __Number_h__
#define __Number_h__

#include "dlclass.h"
#include <math.h>
#include "metaiv.h"
#include "Object.h"
#include "Serializable.h"
#include "Number_anonym.h"


class vdm_Number : 
  public vdm_Serializable, 
  public vdm_Object {

public:
  static const wchar_t * name;

public:
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);

  virtual const wchar_t * GetTypeName () const { return vdm_Number::name; }

//  vdm_Number ();
  virtual ~vdm_Number () {}

//  vdm_Number (const Quote &);
  virtual Int vdm_intValue () =0;
  virtual Int vdm_byteValue ();
  virtual Int vdm_longValue () =0;
  virtual Real vdm_floatValue () =0;
  virtual Int vdm_shortValue ();
  virtual Real vdm_doubleValue () =0;

 private:

  static vdm_Number *GetDLPtr(const DLObject& obj);
  
};

#endif // __Number_h__

