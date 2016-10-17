#ifndef __pog_interface_h__
#define __pog_interface_h__

#include "metaiv.h"
#include "AS.h"
#include "POG_uMEDIATOR.h"
 
#define TAG_TYPE_TEST_ProofObligationPP   (TAG_POG_TEST + 0)

class TYPE_TEST_ProofObligationPP : public Record {
public:

  TYPE_TEST_ProofObligationPP () : Record(TAG_TYPE_TEST_ProofObligationPP, 4) {}

  TYPE_TEST_ProofObligationPP &Init (const TYPE_POG_uMEDIATOR_String &p1, const TYPE_POGTP_Classification &p2, const type_7AS_NameCL &p3, const TYPE_POGTP_LocContext &p4)
{
  SetField(1, p1); SetField(2, p2); SetField(3, p3); SetField(4, p4);
  return *this;
};

  TYPE_TEST_ProofObligationPP (const Generic &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_TEST_ProofObligationPP";
  }

  TYPE_POG_uMEDIATOR_String get_po () const { return GetField(1); };
  void set_po (const TYPE_POG_uMEDIATOR_String &p) { SetField(1, p); };
  TYPE_POGTP_Classification get_kind () const { return GetField(2); };
  void set_kind (const TYPE_POGTP_Classification &p) { SetField(2, p); };
  type_7AS_NameCL get_source () const { return GetField(3); };
  void set_source (const type_7AS_NameCL &p) { SetField(3, p); };
  TYPE_POGTP_LocContext get_loc () const { return GetField(4); };
  void set_loc (const TYPE_POGTP_LocContext &p) { SetField(4, p); };
};

class PogInterface {
 public:
    void setup();

    void genPOForTypeDef (int index);
    void genPOForFnDef (int index);
    void genPOForValueDef ();
    
    int getNumberOfPO();
    wstring getPO(int index);
    wstring getPOStatus(int index);
    wstring getPOName(int index);
    wstring getPOLocation(int index);
    int getPOLine(int index);
    int getPOCol(int index);
    wstring getPODueTo(int index);
    wstring getPOClassification(int index);
    //wstring getPOLocClassification(int index);
    void genPO (const SEQ<TYPE_AS_Name> & modNames);
    wstring getPrettyPO();
    wstring getSpecWithPO();
    SEQ<TYPE_POG_uMEDIATOR_ProofObligationPP> getPOs();
    Sequence getTextPOs();
    SET<TYPE_TEST_ProofObligationPP> getTestPOs();

//------- Entry dealing with displaying and filtering POGs
    
    static Set getModnms();
    static Set getMemnms();
    static Set getLocs();
    static Set getKinds();
    static Set getChckMarks();

    static void setModnms(const Set&);
    static void setMemnms(const Set&);
    static void setLocs(const Set&);
    static void setKinds(const Set&);
    static void setChckMarks(const Set&);

    static wstring printName(const Generic &);
    static wstring printPOKind(const Generic &);
    static wstring printPOLocClassification(const Generic &);
    static wstring printBool(const Generic &);

    SEQ<TYPE_POG_uMEDIATOR_ProofObligationPP> applyFilters();
    Sequence applyFiltersText();

    Tuple toText(const TYPE_POG_uMEDIATOR_ProofObligationPP &);
    Sequence toTextSeq(const SEQ<TYPE_POG_uMEDIATOR_ProofObligationPP> &);
    Tuple getPoId(const TYPE_POG_uMEDIATOR_ProofObligationPP &);
    Bool setCheckMark(const Tuple&);

    static Quote toLocs(const wstring & kind);
    static Set toKind(const wstring & kind);
};
     
PogInterface& GetPogInterface();

#endif // __pog_interface_h__
