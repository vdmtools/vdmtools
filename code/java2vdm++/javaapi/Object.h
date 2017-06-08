#ifndef __Object_h__
#define __Object_h__

#include "dlclass.h"
#include <math.h>
#include "metaiv.h"
//#include L"Nullable.h"
//#include L"dlclass.h"
#include "Object_anonym.h"


class vdm_Object : public DlClass  //: public vdm_Nullable
{

public:
  static const wchar_t * name;

public:
//  vdm_Object (const Quote &);
  vdm_Object ();

  virtual ~vdm_Object () {}
  virtual const wchar_t * GetTypeName () const { return vdm_Object::name; }

protected:
  virtual vdm_Object vdm_clone ();
  virtual void vdm_finalize ();
public:
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);

  virtual void vdm_wait ();
  virtual Bool vdm_equals (const vdm_Object &);
  virtual void vdm_notify ();
  virtual Int vdm_hashCode ();
  virtual void vdm_notifyAll ();

private:

  static vdm_Object *GetDLPtr(const DLObject& obj);

};

#endif // __Object_h__

