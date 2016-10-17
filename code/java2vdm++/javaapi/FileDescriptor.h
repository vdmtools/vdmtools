#ifndef __FileDescriptor_h__
#define __FileDescriptor_h__

#include <math.h>
#include "metaiv.h"
#include "Object.h"
#include "FileDescriptor_anonym.h"

class vdm_FileDescriptor : public virtual vdm_Object
{
public:
  static const wchar_t * name;

public:
  vdm_FileDescriptor ();

  virtual Bool vdm_valid ();
  virtual void vdm_sync_u_u ();
};

#endif // __FileDescriptor_h__

