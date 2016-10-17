#include "BUILD.h"
#include "MANGLE.h"
#include "astaux.h"


TYPE_AUML_Name vdm_BUILD_MkUniqueName(const TYPE_AS_Name& p_nm, const type_dL& p_tps)
{
  TYPE_AS_Name mangledName;
  if (MANGLE::IsMangled(p_nm))
    mangledName = p_nm;
  else 
    mangledName = MANGLE::Mangle(p_nm, p_tps);

  return TYPE_AUML_Name (mangledName.get_ids()[mangledName.get_ids().Length()]);
}

type_dL vdm_BUILD_GetFnParms(const TYPE_AS_FnDef& p_fn)
{
  return ASTAUX::GetFnParms(p_fn);
}

type_dL vdm_BUILD_GetOpParms(const TYPE_AS_OpDef& p_op)
{
  return ASTAUX::GetOpParms(p_op);
}
