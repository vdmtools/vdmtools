#ifndef __TYPE2REP_userdef_h__
#define __TYPE2REP_userdef_h__

#include "main_tag.h"
#include "metaiv.h"
#include "AS.h"
#include "REP.h"

#define DEF_TYPE2REP_USERIMPL 1

#define DEF_TYPE2REP_TransType 1

#ifdef DEF_TYPE2REP_USERIMPL
class TYPE2REP
{
public:
  static TYPE_REP_TypeRep TransType (const Record & tp);

private:
  static SEQ<TYPE_REP_TypeRep> TransTypeList (const SEQ<TYPE_AS_Type> & tplst);
  static SEQ<TYPE_REP_FieldRep> TransFields (const SEQ<TYPE_AS_Field> & f_l);
  static SET<TYPE_REP_TypeRep> FlattenUnion (const SET<TYPE_REP_TypeRep> & tps);
};
#endif // DEF_TYPE2REP_USERIMPL

#endif // __TYPE2REP_userdef_h__
