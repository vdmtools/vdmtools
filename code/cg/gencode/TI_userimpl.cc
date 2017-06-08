#include "TI.h"
#include "vdmcg.h"
#include "codegen_tools.h"

#ifdef VDMPP

// IsInstVar
// nm : AS`Name
// ==> bool
Bool vdm_TI_IsInstVar(const TYPE_AS_Name& nm)
{ 
  return GetVDMCG().IsInstVar(nm);
}

#endif //VDMPP
