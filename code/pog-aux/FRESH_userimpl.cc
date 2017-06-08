#include "metaiv.h"
#include "astaux.h"
#include "contextinfo.h"

#ifdef DEF_FRESH_GetName
TYPE_AS_Name vdm_FRESH_GetName (const type_cL & basename, const type_7AS_NameCS & used_names)
{
  return FRESH::GetName(basename, used_names);
}
#endif // DEF_FRESH_GetName

#ifdef DEF_FRESH_GetNameList
type_7AS_NameCL vdm_FRESH_GetNameList (const type_cL & bname,
                                       const Int & num,
                                       const type_7AS_NameCS & used_names)
{
  return FRESH::GetNameList(bname, num, used_names);
}
#endif // DEF_FRESH_GetNameList

#ifdef DEF_FRESH_Nat2str
type_cL vdm_FRESH_Nat2str (const Int & n)
{
  return SEQ<Char>(n.ascii()); 
}
#endif // DEF_FRESH_Nat2str

// GetName
// basename : seq of char
// used_names : set of AS`Name
// -> AS`Name
TYPE_AS_Name FRESH::GetName (const type_cL & basename, const SET<TYPE_AS_Name> & used_names)
{
  TYPE_AS_Id id (basename);
  id.ImpAppend(Char(L'_')).ImpConc(ASTAUX::MkId(GetNext().ascii()));
  return ASTAUX::MkNameFromId(id, NilContextId);
}

// GetNameList
// bname : seq of char
// num : nat
// used_names : set of AS`Name
// -> seq of AS`Name
SEQ<TYPE_AS_Name> FRESH::GetNameList (const type_cL & bname,
                                      const Int & num,
                                      const SET<TYPE_AS_Name> & used_names)
{
  SEQ<TYPE_AS_Name> nm_l;
  int len = num.GetValue();
  for (int index = 1; index <= len; index++)
    nm_l.ImpAppend(GetName(bname, used_names));
  return nm_l;
}

// GetNext
// ==> nat
Int FRESH::GetNext ()
{
  Int next (vdm_FRESH_next);
  vdm_FRESH_next = next + Int(1);
  return next;
}
