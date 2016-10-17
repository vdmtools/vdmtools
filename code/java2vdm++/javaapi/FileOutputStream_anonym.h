#ifndef __FileOutputStream_anonym_h__
#define __FileOutputStream_anonym_h__



#include "metaiv.h"
class vdm_FileOutputStream;
class type_iiM;
#ifndef TAG_type_iiM
#define TAG_type_iiM (TAG_FileOutputStream + 1)
#endif


#include "String_anonym.h"
#include "FileDescriptor_anonym.h"
#include "FileOutputStream_anonym.h"
#ifndef TAG_vdm_FileOutputStream
#define TAG_vdm_FileOutputStream (TAG_FileOutputStream + 2)
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

#endif // __FileOutputStream_anonym_h__

