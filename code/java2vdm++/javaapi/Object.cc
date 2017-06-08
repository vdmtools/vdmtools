#include "Object.h"
#include "JavaString.h"
#ifdef DEF_Object_USERIMPL
#include "Object_userimpl.cpp"
#endif

const wchar_t* vdm_Object::name = L"JavaLangObject";

Generic vdm_Object::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  Generic res;

  if (!wcscmp (name, L"wait"))
    this->vdm_wait();
  else if (!wcscmp(name, L"notify"))
    this->vdm_notify();
  else if (!wcscmp(name, L"notifyAll"))
    this->vdm_notifyAll();
  else if (!wcscmp(name, L"finalize"))
    this->vdm_finalize();
  else if (!wcscmp(name, L"equals")) {
    DLObject o(p[1]);
    vdm_Object* obj = vdm_Object::GetDLPtr(o);
    res = this->vdm_equals(*obj);
  } else if (!wcscmp(name, L"hashCode")) {
    res = this->vdm_hashCode();
//  else if (!wcscmp(name, L"Object")) {
//    if(p.Length()==0) {
//      DLObject res(L"Object", new vdm_Object());
//      return res;
//    } else if (p.Length()==1 && p[1].IsQuote()) {
//      DLObject res(L"Object", new vdm_Object(Quote()));
//      return res;
//    }
  } else {
      // the method does not exist
  }

  return res;
}

#ifndef DEF_Object_wait

void vdm_Object::vdm_wait ()
{
}

#endif

#ifndef DEF_Object_clone

vdm_Object vdm_Object::vdm_clone ()
{
  vdm_Object o;
  return o;
}

#endif

#ifndef DEF_Object_equals

Bool vdm_Object::vdm_equals (const vdm_Object &vdm_obj)
{
  Bool b(false);
  return b;
}

#endif

#ifndef DEF_Object_notify

void vdm_Object::vdm_notify ()
{
}

#endif

#ifndef DEF_Object_finalize

void vdm_Object::vdm_finalize ()
{
}

#endif

#ifndef DEF_Object_hashCode

Int vdm_Object::vdm_hashCode ()
{
  Int i(0);
  return i;
}

#endif

#ifndef DEF_Object_Object

vdm_Object::vdm_Object ()
{
}

#endif

#ifndef DEF_Object_notifyAll

void vdm_Object::vdm_notifyAll ()
{
}

#endif
/*
#ifndef DEF_Object_Object

vdm_Object::vdm_Object (const Quote &vdm_p)
{
  vdm_isNil = (Bool) true;
}

#endif
*/
/*
#ifndef DEF_Object_registerNatives

void vdm_Object::vdm_registerNatives ()
{
}

#endif
*/

vdm_Object *vdm_Object::GetDLPtr(const DLObject& obj)
{
  vdm_Object *objPtr = (vdm_Object*) obj.GetPtr();
  return objPtr;
}
