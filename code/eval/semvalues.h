/***
*  * WHAT
*  *    creation of semantic values.
*  * FILE
*  *    $Id: semvalues.h,v 1.14 2006/04/10 06:11:12 vdmtools Exp $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __semvalues_h__
#define __semvalues_h__

#include "metaiv.h"
#include "AS.h"
#include "main_tag.h"
#include "SEM.h"

#define TAG_TYPE_DYNSEM_SEM_REC        (TAG_TYPE_DYNSEM_SEM +12)
#define TAG_TYPE_DYNSEM_SEM_OBJ_Ref_PP (TAG_TYPE_DYNSEM_SEM +34)
//#define TAG_TYPE_DYNSEM_SEM_TOKEN_PP   (TAG_TYPE_DYNSEM_SEM +35)

extern TYPE_SEM_BOOL sem_true;
extern TYPE_SEM_BOOL sem_false;
extern TYPE_SEM_NIL sem_nil;
extern TYPE_SEM_UNDEF sem_undef;
extern TYPE_SEM_Permission sem_read_only;
extern TYPE_SEM_Permission sem_read_write;
extern TYPE_SEM_CONT sem_cont;

inline
TYPE_SEM_BOOL & mk_sem_bool(bool b)
{
  return (b ? sem_true : sem_false);
}

enum {
  length_DYNSEM_SEM_SemRecord = 3,
  pos_DYNSEM_SEM_SemRecord_tag = 1,
  pos_DYNSEM_SEM_SemRecord_value = 2,
  pos_DYNSEM_SEM_SemRecord_checked = 3
};

//  SemRecord ::
//    value   : vdmlib_Record(tag,size)
//    checked : bool
class TYPE_DYNSEM_SEM_SemRecord : public Record {
public:

  TYPE_DYNSEM_SEM_SemRecord () : Record(TAG_TYPE_DYNSEM_SEM_REC, 3) {}

  TYPE_DYNSEM_SEM_SemRecord & Init (const TYPE_AS_Name & p1, const Record & p2, const Bool & p3)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3);return *this; };

  TYPE_DYNSEM_SEM_SemRecord (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_DYNSEM_SEM_SemRecord";
  }
  const TYPE_AS_Name & get_tag () const { return static_cast<const TYPE_AS_Name &>(GetRecord(1)); }
  void set_tag (const TYPE_AS_Name &p) { SetField(1, p ); }
  const Record & get_value () const { return GetRecord(2); }
  void set_value (const Record &p) { SetField(2, p ); }
  const Bool & get_checked () const { return GetBool(3); }
  void set_checked (const Bool &p) { SetField(3, p ); }
};

/**************************************************************/
/* The SemRec class implements the semantic value of records. */
/* The specification of SEM`Rec is:                           */
/*     REC::                                                  */
/*       tag: AS`Name                                         */
/*         v : map nat to VAL                                 */
/*         v_dc:- map nat to VAL;                             */
/* The implementation of the semantic records is based on the */
/* don't care support in the metaiv-library. The record tag info*/
/* table used is defined locally in the SemRec class (DefSemRecInfo) */
/*                                                            */
/* The semantic record is implemented as a Record with the    */
/* SEM`REC tag. This record contains one field, that contains */
/* the record that implements the semantic record             */
/**************************************************************/
class SemRecTable
{
private:
  VDMRecInfoMap DefSemRecInfo;
  int next_tag;
  TYPE_AS_Name currentmodcls; // Current Module or Class Name.

public:
  SemRecTable() { Reset(); };
  ~SemRecTable() { Reset(); };
  void Reset() { this->DefSemRecInfo.clear(); this->next_tag = 1; };

  void SetCurrentModClass(const TYPE_AS_Name & name)
  { this->currentmodcls = name; };

  void Decl_SEM_REC(const TYPE_AS_Name & tag,
                    int size,
                    const SEQ<Bool> & dc_s);
  TYPE_DYNSEM_SEM_SemRecord mk_SEM_REC(const TYPE_AS_Name & tag,
                                       const SEQ<TYPE_SEM_VAL> & v_l);
  bool IsDecl(const TYPE_AS_Name & tag) const;
};

class SemRec
{
private:
  static VDMRecInfoMap DefSemRecInfo;
  static int next_tag;
  static TYPE_AS_Name currentmodcls; // Current Module or Class Name.
  static MAP<Char, TYPE_SEM_CHAR> charMap;
  static MAP<SEQ<Char>, TYPE_SEM_QUOTE> quoteMap;

public:
  static TYPE_SEM_CompExplFN CompFN(const TYPE_SEM_ExplFN & efn);
  static TYPE_SEM_VAL UpdateClosEnv(const TYPE_SEM_VAL & fnval, const TYPE_SEM_BlkEnv & blkenv);

  static void ResetSemRecTable();
  static TYPE_DYNSEM_SEM_SemRecord mk_SEM_REC(const TYPE_AS_Name & tag, const SEQ<TYPE_SEM_VAL> & v_l);
  static void Decl_SEM_REC(const TYPE_AS_Name & tag, int size, const SEQ<Bool> & dc_s);
  static bool IsDecl(const TYPE_AS_Name & tag);
  static void SetCurrentModClass(const TYPE_AS_Name & name);

  static bool IsSemVal(const Generic & val);

  static TYPE_AS_Expr VAL2Expr(const TYPE_SEM_VAL & val);
  static SEQ<TYPE_AS_Expr> VALSet2Expr(const SET<TYPE_SEM_VAL> & val_s);
  static SEQ<TYPE_AS_Expr> VALSeq2Expr(const SEQ<TYPE_SEM_VAL> & val_s);
  static SEQ<TYPE_AS_Maplet> VALMap2Expr(const MAP<TYPE_SEM_VAL,TYPE_SEM_VAL> & val_m);

  static TYPE_SEM_REC SemRecord2REC(const TYPE_DYNSEM_SEM_SemRecord & srec);
  static TYPE_DYNSEM_SEM_SemRecord REC2SemRecord(const TYPE_SEM_REC & semrec);
  static TYPE_DYNSEM_SEM_SemRecord CreateSemRecordUndefinedFields(
                                   const TYPE_AS_CompositeType & tp,
                                   const TYPE_AS_Name & tag);
  static SEQ<TYPE_SEM_CHAR> GetSemChars(const SEQ<Char> & cs);
  static TYPE_SEM_CHAR GetSemChar(const Char & c);
  static TYPE_SEM_QUOTE GetSemQuote(const SEQ<Char> & cs);

#ifdef VICE
  static int Size(const TYPE_SEM_VAL & val);
  static wstring Val2String(const TYPE_SEM_VAL & val);
  static wstring ValSeq2String(const SEQ<TYPE_SEM_VAL> & v_l);
  static int SizeValSeq(const SEQ<TYPE_SEM_VAL> & val_l);
#endif // VICE

private:
  static int SizeExplFN(const TYPE_SEM_ExplFN &efn);
  static int SizeExplOP(const TYPE_SEM_ExplOP &eop);
  static int SizeCompExplFN(const TYPE_SEM_CompExplFN &cefn);
  static int SizeExplPOLY(const TYPE_SEM_ExplPOLY &epoly);
  static int SizeValSet(const SET<TYPE_SEM_VAL> & val_s);
  static int SizeValMap(const MAP<TYPE_SEM_VAL,TYPE_SEM_VAL> & val_m);
  static int SizeNatValMap(const Map & val_m);
  static int SizeOverload(const Map & over_m);
  static int SizeValueMap( const Map & val_m );
  static int SizeHist( const Map & hist_m );
  static int SizeInstStruct( const Map & ins_m );
  static wstring Num2String( const Generic & num);
  static wstring ValSet2String( const SET<TYPE_SEM_VAL> & v_s );
  static wstring ValNatMap2String( const Map & v_m1, const Map & v_m2 );
  static wstring ValMap2String( const MAP<TYPE_SEM_VAL,TYPE_SEM_VAL> & v_m );
  static wstring Ids2String(const TYPE_AS_Ids & ids_);
};

#endif // __semvalues_h__

// Force emacs to use C++ mode for this file.
//// Local Variables:
//// mode: c++
//// End:

