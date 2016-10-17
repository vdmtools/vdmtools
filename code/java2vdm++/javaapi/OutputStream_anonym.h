#ifndef __OutputStream_anonym_h__
#define __OutputStream_anonym_h__



#include "metaiv.h"
class vdm_OutputStream;
class type_iiM;
#ifndef TAG_type_iiM
#define TAG_type_iiM (TAG_OutputStream + 1)
#endif

#ifndef TAG_vdm_OutputStream
#define TAG_vdm_OutputStream (TAG_OutputStream + 2)
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

#endif // __OutputStream_anonym_h__

