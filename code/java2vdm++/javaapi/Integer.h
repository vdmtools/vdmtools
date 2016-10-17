#ifndef __Integer_h__
#define __Integer_h__

#include "dlclass.h"
#include <math.h>
#include "metaiv.h"
#include "Number.h"
#include "Comparable.h"
#include "Integer_anonym.h"

class vdm_Integer : 
  public vdm_Number, 
  public vdm_Comparable {

  int value;

  public:
  static const wchar_t * name;

public:
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);

  virtual const wchar_t * GetTypeName () const { return vdm_Integer::name; }

  vdm_Integer ();
  vdm_Integer (const Int &);
  vdm_Integer (const vdm_String &);

//  vdm_Integer (const Quote &);
 
  virtual ~vdm_Integer () {}

  static DLObject vdm_decode (const vdm_String &);
  virtual Bool vdm_equals (const vdm_Integer &);
  static DLObject vdm_valueOf (const vdm_String &);
  virtual Int vdm_hashCode ();
  virtual Int vdm_intValue ();
  virtual Int vdm_byteValue ();
  virtual Int vdm_compareTo (const vdm_Object &);
  virtual Int vdm_compareToInt (const vdm_Integer &);
  virtual Int vdm_longValue ();
  virtual Real vdm_floatValue ();
  virtual Int vdm_shortValue ();
  virtual Real vdm_doubleValue ();
  static DLObject vdm_toHexString (const Int &);
  virtual DLObject vdm_toString ();
  static DLObject vdm_toString (const Int &);
  static DLObject vdm_toOctalString (const Int &);
  static DLObject vdm_toString (const Int &, const Int &);
  static Int vdm_parseInt (const vdm_String &);
  static DLObject vdm_getInteger (const vdm_String &);
  static Int vdm_parseInt (const vdm_String &, const Int &);
  static DLObject vdm_getInteger (const vdm_String &, const Int &);
  static DLObject vdm_getInteger (const vdm_String &, const vdm_Integer &);

  void setValue(const Int &);
  void setValue(const Real &);
  void setValue(const vdm_String &);

 private:

  static vdm_Integer *GetDLPtr(const DLObject& obj);

}
;

#endif // __Integer_h__

