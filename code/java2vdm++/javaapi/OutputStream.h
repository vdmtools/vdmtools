#ifndef __OutputStream_h__
#define __OutputStream_h__

#include "dlclass.h"
#include <math.h>
#include "metaiv.h"
#include "Object.h"
#include "OutputStream_anonym.h"

class vdm_OutputStream : public vdm_Object {

protected:

  vdm_OutputStream ();
  virtual ~vdm_OutputStream () {}

public:

  virtual void vdm_close ();
  virtual void vdm_flush ();
  virtual void vdm_write (const Int &) =0;
  virtual void vdm_write (const type_iiM &);
  virtual void vdm_write (const type_iiM &, const Int &, const Int &);

}
;

#endif // __OutputStream_h__

