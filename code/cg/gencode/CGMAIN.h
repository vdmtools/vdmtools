#ifndef __cgmain_h__
#define __cgmain_h__

#include "metaiv.h"
#include "main_tag.h"
#include "CPP.h"

#define TAG_TYPE_CGMAIN_VT     (TAG_CGMAIN + 1)

enum  {
  vdm_CGMAIN_VT = TAG_TYPE_CGMAIN_VT,
  length_CGMAIN_VT = 2,
  pos_CGMAIN_VT_name = 1,
  pos_CGMAIN_VT_type = 2
};

class TYPE_CGMAIN_VT : public Record {
public:

  TYPE_CGMAIN_VT () : Record(TAG_TYPE_CGMAIN_VT, 2) {}

  TYPE_CGMAIN_VT &Init (const TYPE_CPP_Expr &p1, const Generic &p2)
  { SetField(pos_CGMAIN_VT_name, p1);
    SetField(pos_CGMAIN_VT_type, p2);
    return *this; };

  TYPE_CGMAIN_VT (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_CGMAIN_VT";
  }

  const TYPE_CPP_Expr & get_name () const
  { return this->GetRecord(pos_CGMAIN_VT_name); };
  void set_name (const TYPE_CPP_Expr &p)
  { this->SetField(pos_CGMAIN_VT_name, p); };
  const Generic & get_type () const
  { return this->GetField(pos_CGMAIN_VT_type); };
  void set_type (const Generic &p)
  { this->SetField(pos_CGMAIN_VT_type, p); };
};

Bool vdm_CGMAIN_get_uwchar();

#endif // __cgmain_h__
