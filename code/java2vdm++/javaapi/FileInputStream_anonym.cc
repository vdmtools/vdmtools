#include "metaiv.h"
#include "FileInputStream.h"
#include "JavaString.h"
#include "FileDescriptor.h"
#include "FileInputStream.h"
#include "FileDescriptor.h"
#if !DEF_quote_NIL && DECL_quote_NIL
#define DEF_quote_NIL 1
const Quote quote_NIL(L"NIL");

#endif

#if !DEF_type_iiiM2P && DECL_type_iiiM2P
#define DEF_type_iiiM2P 1

type_iiiM2P &type_iiiM2P::Init (Int p1, type_iiM p2) {  SetField(1, p1);
  SetField(2, p2);
  return * this;

}


Int type_iiiM2P::get_1 () const {
  return (Int) GetField(1);
}


void type_iiiM2P::set_1 (const Int &p) {  SetField(1, p);
}


type_iiM type_iiiM2P::get_2 () const {
  return (type_iiM) GetField(2);
}


void type_iiiM2P::set_2 (const type_iiM &p) {  SetField(2, p);
}

#endif

