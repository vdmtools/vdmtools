#ifndef __Comparable_h__
#define __Comparable_h__

#include <math.h>
#include "metaiv.h"
#include "Comparable_anonym.h"


class vdm_Comparable {

public:

  virtual ~vdm_Comparable () {}

public:
  virtual Int vdm_compareTo (const vdm_Object &) =0;
}
;

#endif // __Comparable_h__

