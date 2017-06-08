#ifndef _CGAUX_H_
#define _CGAUX_H_

#include "AS.h"
#include "CPP.h"

#define TAG_TYPE_CGAUX_SortFct (TAG_CGAUX + 1)
#define TAG_TYPE_CGAUX_AuxFct  (TAG_CGAUX + 2)

class TYPE_CGAUX_SortFct : public Record {
public:

//  TYPE_CGAUX_SortFct () : Record(SortFct_AUX, 3) {}
  TYPE_CGAUX_SortFct () : Record(TAG_TYPE_CGAUX_AuxFct, 3) {}

  TYPE_CGAUX_SortFct &Init (const Tuple &p1, const Tuple &p2, const Tuple &p3)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3); return *this; }

  TYPE_CGAUX_SortFct (const Generic &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_CGMAIN_VT";
  }

  Tuple get_sort () const { return this->GetField(1); };
  void set_sort (const Tuple &p) { this->SetField(1, p); };
  Tuple get_sortnls () const { return this->GetField(2); };
  void set_sortnls (const Tuple &p) { this->SetField(2, p); };
  Tuple get_sortseq () const { return this->GetField(3); };;
  void set_sortseq (const Tuple &p) { this->SetField(3, p); };
};

TYPE_AS_Id vdm_CGAUX_GiveLastName(const Record&);
TYPE_CPP_FileName vdm_CGAUX_GenFileExt(const TYPE_CPP_FileName&);
TYPE_AS_Id vdm_CGAUX_CleanIdentifier(const TYPE_AS_Id&);
type_cL vdm_CGAUX_StringNumber(const Int&);
Int vdm_CGAUX_FindScope(const TYPE_AS_Name&);
#ifdef VDMPP
void vdm_CGAUX_IncludeClass(const TYPE_AS_Name&);
Generic vdm_CGAUX_ConvAsAccess2CPP(const TYPE_AS_Access&);
Bool vdm_CGAUX_IsInstVar(const TYPE_AS_Name & nm);
#endif //VDMPP

#ifdef VDMSL
TYPE_AS_Id vdm_CGAUX_GiveCurCName();
#endif //VDMSL

#endif // _CGAUX_H_
