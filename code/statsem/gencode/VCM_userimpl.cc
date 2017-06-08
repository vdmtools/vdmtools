#include "VCM.h"
#include "statsem.h"
#include "intconvquotes.h"

#ifdef VDMPP
// IsClassName
// nm : AS`Name
// ==> bool
Bool vdm_VCM_IsClassName (const TYPE_AS_Name & nm)
{
  return GetStatSem().IsClassName(INT2Q::g2hAS(nm));
}
#endif // VDMPP
