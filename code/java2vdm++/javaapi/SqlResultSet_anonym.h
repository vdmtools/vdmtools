
#ifndef __SqlResultSet_anonym_h__
#define __SqlResultSet_anonym_h__


#include "metaiv.h"
class vdm_SqlResultSet;
class type_iiM;
#ifndef TAG_type_iiM
#define TAG_type_iiM (TAG_SqlResultSet + 1)
#endif

#include "Object_anonym.h"
#include "String_anonym.h"
#include "SqlStatement_anonym.h"
#ifndef TAG_vdm_SqlResultSet
#define TAG_vdm_SqlResultSet (TAG_SqlResultSet + 2)
#endif

#ifndef DECL_type_iiM
#define DECL_type_iiM 1

class type_iiM : public MAP<Int,Int> {
public:

  type_iiM () : MAP<Int,Int>() {}


  type_iiM (const MAP<Int,Int> &c) : MAP<Int,Int>(c) {}


  type_iiM (const Generic &c) : MAP<Int,Int>(c) {}


  const wchar_t * GetTypeName () const {
    return L"type_iiM";
  }
};
#endif // DECL_type_iiM


#endif // __SqlResultSet_anonym_h__

