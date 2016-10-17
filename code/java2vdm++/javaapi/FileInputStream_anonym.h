#ifndef __FileInputStream_anonym_h__
#define __FileInputStream_anonym_h__



#include "metaiv.h"
extern const Quote quote_NIL;

class type_ref_FileInputStream;
class type_cL;
class type_iiM;
class type_iiiM2P;
#ifndef TAG_type_cL
#define TAG_type_cL (TAG_FileInputStream + 1)
#endif

#ifndef TAG_type_iiM
#define TAG_type_iiM (TAG_FileInputStream + 2)
#endif

#ifndef TAG_type_iiiM2P
#define TAG_type_iiiM2P (TAG_FileInputStream + 3)
#endif

#ifndef TAG_quote_NIL
#define TAG_quote_NIL (TAG_FileInputStream + 4)
#endif


#include "String_anonym.h"
#include "FileDescriptor_anonym.h"
#include "FileInputStream_anonym.h"
#ifndef TAG_type_ref_FileInputStream
#define TAG_type_ref_FileInputStream (TAG_FileInputStream + 5)
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

#ifndef DECL_type_iiiM2P
#define DECL_type_iiiM2P 1

class type_iiiM2P : public Tuple {
public:

  type_iiiM2P () : Tuple(2) {}

  type_iiiM2P &Init (Int p2, type_iiM p3);

  type_iiiM2P (const Generic &c) : Tuple(c) {}


  const wchar_t * GetTypeName () const {
    return L"type_iiiM2P";
  }

  Int get_1 () const;
  void set_1 (const Int &p);
  type_iiM get_2 () const;
  void set_2 (const type_iiM &p);
}
 ;
#endif // DECL_type_iiiM2P


#endif // __FileInputStream_anonym_h__

