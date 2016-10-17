#ifndef __StringBuffer_anonym_h__
#define __StringBuffer_anonym_h__

#include "metaiv.h"
extern const Quote quote_NIL;

class vdm_StringBuffer;
#ifndef TAG_quote_NIL
#define TAG_quote_NIL (TAG_StringBuffer + 1)
#endif

#include "StringBuffer_anonym.h"
#ifndef TAG_vdm_StringBuffer
#define TAG_vdm_StringBuffer (TAG_StringBuffer + 2)
#endif

#ifndef DECL_type_cL
#define DECL_type_cL 1

class type_cL : public SEQ<Char> {
public:
  type_cL () : SEQ<Char>() {}
  type_cL (const SEQ<Char> &c) : SEQ<Char>(c) {}
  type_cL (const Generic &c) : SEQ<Char>(c) {}

  const wchar_t * GetTypeName () const {
    return L"type_cL";
  }
};
#endif // DECL_type_cL

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

#endif // __StringBuffer_anonym_h__

