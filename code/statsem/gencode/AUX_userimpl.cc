#include "AUX.h"
#include "statsem.h"
#include "intconvquotes.h"

#ifdef VDMPP
// IsEmptyName
// nm : AS`Name
// -> bool
Bool vdm_AUX_IsEmptyName(const TYPE_AS_Name & nm)
{
  return Bool(GetStatSem().IsEmptyName(nm));
}
#endif // VDMPP
