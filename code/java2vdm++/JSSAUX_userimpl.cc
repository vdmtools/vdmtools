#include "metaiv.h"
#include "JSSAUX.h"

type_cL vdm_JSSAUX_PrintName (const TYPE_AS_Name &p_name) 
{
  Sequence result;
  Sequence ids (p_name.get_ids());
  Generic g;
  size_t len_ids = ids.Length();
  for (size_t i = 1; i <= len_ids; i++)
  {
    result.ImpConc(Sequence(ids[i]));
    if (i != (size_t)(ids.Length())) 
      result.ImpAppend(Char(L'.'));
  }
  return (Generic) result;
}
