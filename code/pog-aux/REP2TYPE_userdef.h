#ifndef __REP2TYPE_userdef_h__
#define __REP2TYPE_userdef_h__

#include "main_tag.h"
#include "metaiv.h"
#include "AS.h"
#include "REP.h"
#include "contextinfo.h"

#define DEF_REP2TYPE_USERIMPL 1

#define DEF_REP2TYPE_SetToSeq 1
#define DEF_REP2TYPE_TypeRepToType 1

#ifdef DEF_REP2TYPE_USERIMPL
class REP2TYPE
{
public:
  static TYPE_AS_Type TypeRepToType (const TYPE_REP_TypeRep & tyrep,
                                     const TYPE_CI_ContextId & cid);
  static SEQ<TYPE_AS_Type> SetToSeq (const SET<TYPE_AS_Type> & s);
};
#endif // DEF_REP2TYPE_USERIMPL

#endif // __REP2TYPE_userdef_h__
