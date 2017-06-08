#include "MANGLE.h"
#include "mangle_impl.h"

Bool vdm_MANGLE_IsMangled(const TYPE_AS_Name& p_nm)
{
  return Bool(IsMangled(p_nm));
}

TYPE_AS_Name vdm_MANGLE_GetUnmangledName(const TYPE_AS_Name& p_nm)
{
  return GetUnmangledName(p_nm);
}


