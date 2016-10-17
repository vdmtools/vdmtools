#ifndef __String_anonym_h__
#define __String_anonym_h__

#include "metaiv.h"

extern const Quote quote_NIL;

class vdm_String;
class type_icM;
class type_iiM;
#ifndef TAG_type_icM
#define TAG_type_icM (TAG_String + 1)
#endif

#ifndef TAG_type_iiM
#define TAG_type_iiM (TAG_String + 2)
#endif

#ifndef TAG_quote_VOID
#define TAG_quote_VOID (TAG_String + 3)
#endif

#ifndef TAG_quote_NIL
#define TAG_quote_NIL (TAG_String + 4)
#endif


#include "Locale_anonym.h"
#include "Object_anonym.h"
#include "String_anonym.h"
#include "StringBuffer_anonym.h"
#ifndef TAG_vdm_String
#define TAG_vdm_String (TAG_String + 5)
#endif

#ifndef DECL_type_icM
#define DECL_type_icM 1

class type_icM : public MAP<Int,Char> {
public:
  type_icM () : MAP<Int,Char>() {}
  type_icM (const MAP<Int,Char> &c) : MAP<Int,Char>(c) {}
  type_icM (const Generic &c) : MAP<Int,Char>(c) {}

  const wchar_t * GetTypeName () const {
    return L"type_icM";
  }
};
#endif // DECL_type_icM

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


#endif // __String_anonym_h__

