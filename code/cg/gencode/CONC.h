#ifndef __CONC_h__
#define __CONC_h__

#include "metaiv.h"
#include "AS.h"

#define TAG_TYPE_CONC_ClassInfo (TAG_CONC + 1)

enum {
  vdm_CONC_ClassInfo = TAG_TYPE_CONC_ClassInfo,
  length_CONC_ClassInfo = 9,
  pos_CONC_ClassInfo_topClass = 1,
  pos_CONC_ClassInfo_topPerThread = 2,
  pos_CONC_ClassInfo_topProcThread = 3,
  pos_CONC_ClassInfo_newGuards = 4,
  pos_CONC_ClassInfo_baseClasses = 5,
  pos_CONC_ClassInfo_accPermPred = 6,
  pos_CONC_ClassInfo_mutexTable = 7,
  pos_CONC_ClassInfo_nrOpers = 8,
  pos_CONC_ClassInfo_startCount = 9
};

class TYPE_CONC_ClassInfo: public Record {
public:

  TYPE_CONC_ClassInfo () : Record(TAG_TYPE_CONC_ClassInfo, 9) {}

  TYPE_CONC_ClassInfo &Init (const Bool &p1, const Bool &p2, const Bool &p3, const Bool &p4,
                             const Sequence & p5, const Map & p6, const Map & p7, const Int & p8, const Int & p9)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3); SetField(4, p4); SetField(5, p5);
    SetField(6, p6); SetField(7, p7); SetField(8, p8); SetField(9, p9); return *this; }

  TYPE_CONC_ClassInfo (const Generic &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_CONC_ClassInfo";
  }

  const Bool & get_topClass () const { return this->GetBool(1); };
  void set_topClass (const Bool &p) { this->SetField(1, p); };
  const Bool & get_topPerThread () const { return this->GetBool(2); };
  void set_topPerThread (const Bool &p) { this->SetField(2, p); };
  const Bool & get_topProcThread () const { return this->GetBool(3); };
  void set_topProcThread (const Bool &p) { this->SetField(3, p); };
  const Bool & get_newGuards () const { return this->GetBool(4); };
  void set_newGuards (const Bool &p) { this->SetField(4, p); };
  SEQ<TYPE_AS_Name> get_baseClasses () const { return this->GetField(5); };
  void set_baseClasses (const SEQ<TYPE_AS_Name> &p) { this->SetField(5, p); };
  MAP<TYPE_AS_Name,Set> get_accPermPred () const { return this->GetField(6); };
  void set_accPermPred (const MAP<TYPE_AS_Name,Set> &p) { this->SetField(6, p); };
  MAP<TYPE_AS_Name,Sequence> get_mutexTable () const { return this->GetField(7); };
  void set_mutexTable (const MAP<TYPE_AS_Name,Sequence> &p) { this->SetField(7, p); };
  const Int & get_nrOpers () const { return this->GetInt(8); };
  void set_nrOpers (const Int &p) { this->SetField(8, p); };
  const Int & get_startCount () const { return this->GetInt(9); };
  void set_startCount (const Int &p) { this->SetField(9, p); };
};

#endif // __CONC.h__
