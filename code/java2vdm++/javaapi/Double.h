#ifndef __Double_h__
#define __Double_h__

#include <dlclass.h>
#include <math.h>
#include "Number.h"
#include "Comparable.h"

#include "metaiv.h"

class vdm_Double : public vdm_Number //, public vdm_Comparable
{

public:
  static const wchar_t * name;

protected:
  double value;

private:
  static vdm_Double* GetDLPtr(const DLObject& obj);

public:
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);

  virtual const wchar_t * GetTypeName () const { return vdm_Double::name; }

  vdm_Double ();
  vdm_Double (const Real &);
  vdm_Double (const Sequence &);

  virtual ~vdm_Double () {}

public:

  Sequence vdm_toSeqOfChar();
//  Real vdm_valueOf (const Sequence &);
//  Int  vdm_hashCode ();
  Int  vdm_intValue ();
//  Bool vdm_isNaN ();
  Int  vdm_byteValue ();
//  Bool vdm_isNaN (const Real &);
  Int  vdm_longValue ();
  Real vdm_floatValue ();
  Int  vdm_shortValue ();
  Real vdm_doubleValue ();
  Real vdm_parseDouble (const Sequence &);
//  type_ref_JavaLangString vdm_toString ();
//  type_ref_JavaLangString vdm_toString (const Real &);
//  Bool vdm_isInfinite ();
//  Bool vdm_isInfinite (const Real &);
//  Int  vdm_doubleToLongBits (const Real &);
//  Real vdm_longBitsToDouble (const Int &);
//  Int  vdm_doubleToRawLongBits (const Real &);
}
;

#endif // __Double_h__

