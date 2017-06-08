#ifndef __FRESH_userdef_h__
#define __FRESH_userdef_h__

#include "main_tag.h"
#include "metaiv.h"
#include "AS.h"

#define DEF_FRESH_USERIMPL 1

//#define DEF_FRESH_GetName 1
//#define DEF_FRESH_GetNameList 1

//#define DEF_FRESH_Nat2str 1

#ifdef DEF_FRESH_USERIMPL
class FRESH
{
public:
  static TYPE_AS_Name GetName (const type_cL & basename, const SET<TYPE_AS_Name> & used_names);
  static SEQ<TYPE_AS_Name> GetNameList (const type_cL & bname, const Int & num,
                                        const SET<TYPE_AS_Name> & used_names);
private:
  static Int GetNext ();
};
#endif // DEF_FRESH_USERIMPL
#endif // __FRESH_userdef_h__
