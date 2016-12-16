/***
*  * WHAT
*  *    Static Semantics.
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/statsem/statsem.h,v $
*  * VERSION
*  *    $Revision: 1.120 $
*  * DATE
*  *    $Date: 2006/06/12 08:36:17 $
*  * STATUS
*  *    Under development.
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __statsem_h__
#define __statsem_h__

#include "metaiv.h"        // include IPTES MetaIV class prototype
#include "main_tag.h"
#include "asquotes.h"
#include "contextinfo.h"

#ifdef VDMPP
#include "ProjectTypes.h"
#endif // VDMPP

#ifdef __SunOS__
#undef ERR
#endif // __SunOS__

class TypeCheckInfo
{
public:
  TypeCheckInfo() {  }

  void resetInfo()
    {
      names = Set();
    }

  void addName(const TYPE_AS_Name & nm)
    {
      names.Insert(nm);
    }

  bool nameSeen(const TYPE_AS_Name & nm)
    {
      return names.InSet(nm);
    }

private:
  Set names;
};

enum {
  length_TYPE_SSENV_TypeRepElem = 3,
  pos_SSENV_TypeRepElem_tp = 1,
  pos_SSENV_TypeRepElem_used = 2,
  pos_SSENV_TypeRepElem_stat = 3,
  length_TYPE_SSENV_OpTypeRepElem = 4,
  pos_SSENV_OpTypeRepElem_tp = 1,
  pos_SSENV_OpTypeRepElem_used = 2,
  pos_SSENV_OpTypeRepElem_stat = 3,
  pos_SSENV_OpTypeRepElem_oppure = 4,
  length_TYPE_SSENV_PolyTypeRepElem = 3,
  pos_SSENV_PolyTypeRepElem_poly = 1,
  pos_SSENV_PolyTypeRepElem_tp = 2,
  pos_SSENV_PolyTypeRepElem_used = 3,
  length_TYPE_SSENV_FunctionInfo = 3,
  pos_SSENV_FunctionInfo_rec = 1,
  pos_SSENV_FunctionInfo_meas = 2,
  pos_SSENV_FunctionInfo_printPO = 3,

#ifdef VDMSL
  length_TYPE_SSENV_ExpSigRep = 7,
  pos_SSENV_ExpSigRep_tps = 1,
  pos_SSENV_ExpSigRep_val = 2,
  pos_SSENV_ExpSigRep_fns = 3,
  pos_SSENV_ExpSigRep_polys = 4,
  pos_SSENV_ExpSigRep_ops = 5,
  pos_SSENV_ExpSigRep_tag = 6,
  pos_SSENV_ExpSigRep_fcts = 7
#endif // VDMSL
#ifdef VDMPP
  length_TYPE_SSENV_AccessTypeRep = 3,
  pos_SSENV_AccessTypeRep_tp = 1,
  pos_SSENV_AccessTypeRep_a = 2,
  pos_SSENV_AccessTypeRep_stat = 3,
  length_TYPE_SSENV_AccessFnTypeRep = 3,
  pos_SSENV_AccessFnTypeRep_tp = 1,
  pos_SSENV_AccessFnTypeRep_a = 2,
  pos_SSENV_AccessFnTypeRep_stat = 3,
  length_TYPE_SSENV_AccessPolyTypeRep = 3,
  pos_SSENV_AccessPolyTypeRep_tp = 1,
  pos_SSENV_AccessPolyTypeRep_a = 2,
  pos_SSENV_AccessPolyTypeRep_stat = 3,
  length_TYPE_SSENV_AccessOpTypeRep = 4,
  pos_SSENV_AccessOpTypeRep_tp = 1,
  pos_SSENV_AccessOpTypeRep_a = 2,
  pos_SSENV_AccessOpTypeRep_stat = 3,
  pos_SSENV_AccessOpTypeRep_oppure = 4,
  length_TYPE_SSENV_AccessFieldRep = 2,
  pos_SSENV_AccessFieldRep_tp = 1,
  pos_SSENV_AccessFieldRep_a = 2,
  length_TYPE_SSENV_ParseTypeInfo = 16,
  pos_SSENV_ParseTypeInfo_nm = 1,
  pos_SSENV_ParseTypeInfo_sys = 2,
  pos_SSENV_ParseTypeInfo_super = 3,
  pos_SSENV_ParseTypeInfo_tps = 4,
  pos_SSENV_ParseTypeInfo_vals = 5,
  pos_SSENV_ParseTypeInfo_insts = 6,
  pos_SSENV_ParseTypeInfo_fcts = 7,
  pos_SSENV_ParseTypeInfo_polys = 8,
  pos_SSENV_ParseTypeInfo_ops = 9,
  pos_SSENV_ParseTypeInfo_tags = 10,
  pos_SSENV_ParseTypeInfo_overloaded = 11,
  pos_SSENV_ParseTypeInfo_thread = 12,
  pos_SSENV_ParseTypeInfo_constructors = 13,
  pos_SSENV_ParseTypeInfo_fndefs = 14,
  pos_SSENV_ParseTypeInfo_subresps = 15,
  pos_SSENV_ParseTypeInfo_instinv = 16
#endif // VDMPP
};

class TYPE_SSENV_TypeRepElem : public Record {
  public:
  TYPE_SSENV_TypeRepElem () : Record(TAG_TYPE_SSENV_TypeRepElem, 3) {}
  TYPE_SSENV_TypeRepElem &Init(const TYPE_REP_TypeRep & p1,
                               const Bool & p2,
                               const Bool & p3 )
  { SetField(1, p1); SetField(2, p2); SetField(3, p3); return *this; };
  TYPE_SSENV_TypeRepElem (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const { return L"TYPE_SSENV_TypeRepElem"; }

  const TYPE_REP_TypeRep & get_tp() const { return GetRecord(1); }
  void set_tp(const TYPE_REP_TypeRep & p) { SetField(1,p); }
  const Bool & get_used() const { return GetBool(2); }
  void set_used(const Bool & p) { SetField(2,p); }
  const Bool & get_stat() const { return GetBool(3); }
  void set_stat(const Bool & p) { SetField(3,p); }
};

class TYPE_SSENV_OpTypeRepElem : public Record {
  public:
  TYPE_SSENV_OpTypeRepElem () : Record(TAG_TYPE_SSENV_OpTypeRepElem, 4) {}
  TYPE_SSENV_OpTypeRepElem &Init(const TYPE_REP_TypeRep & p1,
                                 const Bool & p2,
                                 const Bool & p3,
                                 const Bool & p4)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3);  SetField(4, p4); return *this; };
  TYPE_SSENV_OpTypeRepElem (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const { return L"TYPE_SSENV_OpTypeRepElem"; }

  const TYPE_REP_TypeRep & get_tp() const { return GetRecord(1); }
  void set_tp(const TYPE_REP_TypeRep & p) { SetField(1,p); }
  const Bool & get_used() const { return GetBool(2); }
  void set_used(const Bool & p) { SetField(2,p); }
  const Bool & get_stat() const { return GetBool(3); }
  void set_stat(const Bool & p) { SetField(3,p); }
  const Bool & get_oppure() const { return GetBool(4); }
  void set_oppure(const Bool & p) { SetField(4,p); }
};

class TYPE_SSENV_PolyTypeRepElem : public Record {
  public:
  TYPE_SSENV_PolyTypeRepElem () : Record(TAG_TYPE_SSENV_PolyTypeRepElem, 3) {}
  TYPE_SSENV_PolyTypeRepElem &Init (const SEQ<TYPE_REP_TypeVarRep> & p1,
                                    const TYPE_REP_TypeRep & p2,
                                    const Bool & p3 )
  { SetField(1, p1); SetField(2, p2); SetField(3, p3); return *this; };
  TYPE_SSENV_PolyTypeRepElem (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const { return L"TYPE_SSENV_PolyTypeRepElem"; }

  const SEQ<TYPE_REP_TypeVarRep> & get_poly() const { return static_cast<const SEQ<TYPE_REP_TypeVarRep> &>(GetSequence(1)); }
  void set_poly(const SEQ<TYPE_REP_TypeVarRep> & p) { SetField(1,p); }
  const TYPE_REP_TypeRep & get_tp() const { return GetRecord(2); }
  void set_tp(const TYPE_REP_TypeRep & p) { SetField(2,p); }
  const Bool & get_used() const { return GetBool(3); }
  void set_used(const Bool & p) { SetField(3,p); }
};

class TYPE_SSENV_FunctionInfo : public Record {
  public:
  TYPE_SSENV_FunctionInfo () : Record(TAG_TYPE_SSENV_FunctionInfo, 3) {}
  TYPE_SSENV_FunctionInfo (const Common &c) : Record(c) {}
  TYPE_SSENV_FunctionInfo & Init (const SET<TYPE_AS_Name> & p1,
                                  const Generic & p2,
                                  const Bool & p3 )
  { SetField(1, p1); SetField(2, p2); SetField(3, p3); return *this; };

  const wchar_t * GetTypeName () const { return L"TYPE_SSENV_FunctionInfo"; }

  const SET<TYPE_AS_Name> & get_rec() const { return static_cast<const SET<TYPE_AS_Name> &>(GetSet(1)); }
  void set_rec(const SET<TYPE_AS_Name> & p) { SetField(1,p); }
  const Generic & get_meas() const { return GetField(2); }
  void set_meas(const Generic & p) { SetField(2,p); }
  const Bool & get_printPO() const { return GetBool(3); }
  void set_printPO(const Bool & p) { SetField(3,p); }
};

#ifdef VDMPP

typedef Record TYPE_SSENV_AccessType;

class TYPE_SSENV_AccessTypeRep : public Record {
  public:
  TYPE_SSENV_AccessTypeRep () : Record(TAG_TYPE_SSENV_AccessTypeRep, 3) {}
  TYPE_SSENV_AccessTypeRep &Init (const TYPE_REP_TypeRep & p1,
                                  const TYPE_AS_Access & p2,
                                  const Bool & p3 )
  { SetField(1, p1); SetField(2, p2); SetField(3, p3); return *this; };
  TYPE_SSENV_AccessTypeRep (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const { return L"TYPE_SSENV_AccessTypeRep"; }

  const TYPE_REP_TypeRep & get_tp() const { return GetRecord(1); }
  void set_tp(const TYPE_REP_TypeRep & p) { SetField(1,p); }
  const TYPE_AS_Access & get_a() const { return GetField(2); }
  void set_a(const TYPE_AS_Access & p) { SetField(2,p); }
  const Bool & get_stat() const { return GetBool(3); }
  void set_stat(const Bool & p) { SetField(3,p); }
};

class TYPE_SSENV_AccessFnTypeRep : public Record {
  public:
  TYPE_SSENV_AccessFnTypeRep () : Record(TAG_TYPE_SSENV_AccessFnTypeRep, 3) {}
  TYPE_SSENV_AccessFnTypeRep &Init (const TYPE_REP_TypeRep & p1,
                                    const TYPE_AS_Access & p2,
                                    const Bool & p3 )
  { SetField(1, p1); SetField(2, p2); SetField(3, p3); return *this; };
  TYPE_SSENV_AccessFnTypeRep (const Common &c) : Record(c) {};

  const wchar_t * GetTypeName () const { return L"TYPE_SSENV_AccessFnTypeRep"; }

  const TYPE_REP_TypeRep & get_tp() const { return GetRecord(1); }
  void set_tp(const TYPE_REP_TypeRep & p) { SetField(1,p); }
  const TYPE_AS_Access & get_a() const { return GetField(2); }
  void set_a(const TYPE_AS_Access & p) { SetField(2,p); }
  const Bool & get_stat() const { return GetBool(3); }
  void set_stat(const Bool & p) { SetField(3,p); }
};

class TYPE_SSENV_AccessPolyTypeRep : public Record {
  public:
  TYPE_SSENV_AccessPolyTypeRep () : Record(TAG_TYPE_SSENV_AccessPolyTypeRep, 3) {}
  TYPE_SSENV_AccessPolyTypeRep &Init (const TYPE_REP_TypeRep & p1,
                                      const TYPE_AS_Access & p2,
                                      const Bool & p3 )
  { SetField(1, p1); SetField(2, p2); SetField(3, p3); return *this; };
  TYPE_SSENV_AccessPolyTypeRep (const Common &c) : Record(c) {};

  const wchar_t * GetTypeName () const { return L"TYPE_SSENV_AccessPolyTypeRep"; }

  const TYPE_REP_TypeRep & get_tp() const { return GetRecord(1); }
  void set_tp(const TYPE_REP_TypeRep & p) { SetField(1,p); }
  const TYPE_AS_Access & get_a() const { return GetField(2); }
  void set_a(const TYPE_AS_Access & p) { SetField(2,p); }
  const Bool & get_stat() const { return GetBool(3); }
  void set_stat(const Bool & p) { SetField(3,p); }
};

class TYPE_SSENV_AccessOpTypeRep : public Record {
  public:
  TYPE_SSENV_AccessOpTypeRep () : Record(TAG_TYPE_SSENV_AccessOpTypeRep, 4) {}
  TYPE_SSENV_AccessOpTypeRep &Init (const TYPE_REP_TypeRep & p1,
                                    const TYPE_AS_Access & p2,
                                    const Bool & p3,
                                    const Bool & p4 )
  { SetField(1, p1); SetField(2, p2); SetField(3, p3); SetField(4, p4); return *this; };
  TYPE_SSENV_AccessOpTypeRep (const Common &c) : Record(c) {};

  const wchar_t * GetTypeName () const { return L"TYPE_SSENV_AccessOpTypeRep"; }

  const TYPE_REP_TypeRep & get_tp() const { return GetRecord(1); }
  void set_tp(const TYPE_REP_TypeRep & p) { SetField(1,p); }
  const TYPE_AS_Access & get_a() const { return GetField(2); }
  void set_a(const TYPE_AS_Access & p) { SetField(2,p); }
  const Bool & get_stat() const { return GetBool(3); }
  void set_stat(const Bool & p) { SetField(3,p); }
  const Bool & get_oppure() const { return GetBool(4); }
  void set_oppure(const Bool & p) { SetField(4,p); }
};

class TYPE_SSENV_AccessFieldRep : public Record {
  public:
  TYPE_SSENV_AccessFieldRep () : Record(TAG_TYPE_SSENV_AccessFieldRep, 2) {}
  TYPE_SSENV_AccessFieldRep &Init (const SEQ<TYPE_REP_FieldRep> & p1,
                                   const TYPE_AS_Access & p2)
  { SetField(1, p1); SetField(2, p2); return *this; };
  TYPE_SSENV_AccessFieldRep (const Common &c) : Record(c) {};

  const wchar_t * GetTypeName () const { return L"TYPE_SSENV_AccessFieldRep"; }

  const SEQ<TYPE_REP_FieldRep> & get_tp() const { return static_cast<const SEQ<TYPE_REP_FieldRep> &>(GetSequence(1)); }
  void set_tp(const SEQ<TYPE_REP_FieldRep> & p) { SetField(1,p); }
  const TYPE_AS_Access & get_a() const { return GetField(2); }
  void set_a(const TYPE_AS_Access & p) { SetField(2,p); }
};

class TYPE_SSENV_ParseTypeInfo : public Record {
  public:
  TYPE_SSENV_ParseTypeInfo () : Record(TAG_TYPE_SSENV_ParseTypeInfo, 16) {}
  TYPE_SSENV_ParseTypeInfo &Init(const TYPE_AS_Name & p1,
                                 const Bool & p2,
                                 const SET<TYPE_AS_Name> & p3,
                                 const MAP<TYPE_AS_Name,TYPE_SSENV_AccessTypeRep> & p4,
                                 const MAP<TYPE_AS_Name,TYPE_SSENV_AccessTypeRep> & p5,
                                 const MAP<TYPE_AS_Name,TYPE_SSENV_AccessTypeRep> & p6,
                                 const MAP<TYPE_AS_Name,TYPE_SSENV_AccessFnTypeRep> & p7,
                                 const MAP<TYPE_AS_Name,TYPE_SSENV_AccessPolyTypeRep> & p8,
                                 const MAP<TYPE_AS_Name,TYPE_SSENV_AccessOpTypeRep> & p9,
                                 const MAP<TYPE_AS_Name,TYPE_SSENV_AccessFieldRep> & p10,
                                 const Map & p11,
                                 const Int & p12,
                                 const SET<TYPE_SSENV_AccessOpTypeRep> & p13,
                                 const Map & p14,
                                 const Map & p15,
                                 const Bool & p16)
  { SetField(1, p1); SetField(2, p2); SetField(3, p3); SetField(4, p4);
    SetField(5, p5); SetField(6, p6); SetField(7, p7); SetField(8, p8);
    SetField(9, p9); SetField(10, p10); SetField(11, p11); SetField(12, p12);
    SetField(13, p13); SetField(14, p14); SetField(15, p15); SetField(16, p16);
    return *this; };
  TYPE_SSENV_ParseTypeInfo (const Common &c) : Record(c) {}

  const wchar_t * GetTypeName () const { return L"TYPE_SSENV_ParseTypeInfo"; }

  const TYPE_AS_Name & get_nm() const { return static_cast<const TYPE_AS_Name &>(GetRecord(1)); }
  void set_nm(const TYPE_AS_Name & p) { SetField(1,p); }
  const Bool & get_sys() const { return GetBool(2); }
  void set_sys(const Bool & p) { SetField(2,p); }
  SET<TYPE_AS_Name> get_super() const { return GetSet(3); }
  void set_super(const SET<TYPE_AS_Name> & p) { SetField(3,p); }
  MAP<TYPE_AS_Name,TYPE_SSENV_AccessTypeRep> get_tps() const { return GetMap(4); }
  void set_tps(const MAP<TYPE_AS_Name,TYPE_SSENV_AccessTypeRep> & p) { SetField(4,p); }
  MAP<TYPE_AS_Name,TYPE_SSENV_AccessTypeRep> get_vals() const { return GetMap(5); }
  void set_vals(const MAP<TYPE_AS_Name,TYPE_SSENV_AccessTypeRep> & p) { SetField(5,p); }
  MAP<TYPE_AS_Name,TYPE_SSENV_AccessTypeRep> get_insts() const { return GetMap(6); }
  void set_insts(const MAP<TYPE_AS_Name,TYPE_SSENV_AccessTypeRep> & p) { SetField(6,p); }
  MAP<TYPE_AS_Name,TYPE_SSENV_AccessFnTypeRep> get_fcts() const { return GetMap(7); }
  void set_fcts(const MAP<TYPE_AS_Name,TYPE_SSENV_AccessFnTypeRep> & p) { SetField(7,p); }
  MAP<TYPE_AS_Name,TYPE_SSENV_AccessPolyTypeRep> get_polys() const { return GetMap(8); }
  void set_polys(const MAP<TYPE_AS_Name,TYPE_SSENV_AccessPolyTypeRep> & p) { SetField(8,p); }
  MAP<TYPE_AS_Name,TYPE_SSENV_AccessOpTypeRep> get_ops() const { return GetMap(9); }
  void set_ops(const MAP<TYPE_AS_Name,TYPE_SSENV_AccessOpTypeRep> & p) { SetField(9,p); }
  MAP<TYPE_AS_Name,TYPE_SSENV_AccessFieldRep> get_tags() const { return GetMap(10); }
  void set_tags(const MAP<TYPE_AS_Name,TYPE_SSENV_AccessFieldRep> & p) { SetField(10,p); }
  const Map & get_overloaded() const { return GetMap(11); }
  void set_overloaded(const Map & p) { SetField(11,p); }
  const Int & get_thread() const { return GetInt(12); }
  void set_thread(const Int & p) { SetField(12,p); }
  SET<TYPE_SSENV_AccessOpTypeRep> get_constructors() const { return GetSet(13); }
  void set_constructors(const SET<TYPE_SSENV_AccessOpTypeRep> & p) { SetField(13,p); }
  Map get_fndefs() const { return GetField(14); }
  void set_fndefs(const Map & p) { SetField(14,p); }
  Map get_subresps() const { return GetField(15); }
  void set_subresps(const Map & p) { SetField(15,p); }
  Bool get_instinv() const { return GetField(16); }
  void set_instinv(const Bool & p) { SetField(16,p); }
};
#endif // VDMPP

#ifdef VDMSL
class TYPE_SSENV_ExpSigRep : public Record {
  public:
  TYPE_SSENV_ExpSigRep () : Record(TAG_TYPE_SSENV_ExpSigRep, 7) {}
  TYPE_SSENV_ExpSigRep (const Common &c) : Record(c) {}
  TYPE_SSENV_ExpSigRep & Init(const Map & p1, const Map & p2, const Map & p3,
                              const Map & p4, const Map & p5, const Map & p6,
                              const Map & p7)
  { SetField(1,p1); SetField(2,p2); SetField(3,p3); SetField(4,p4);
    SetField(5,p5); SetField(6,p6); SetField(7,p7); return *this; }
  const wchar_t * GetTypeName () const { return L"TYPE_SSENV_ExpSigRep"; }

  const Map & get_tps() const { return GetMap(1); }
  void set_tps(const Map & p) { SetField(1,p); }
  const Map & get_val() const { return GetMap(2); }
  void set_val(const Map & p) { SetField(2,p); }
  const Map & get_fns() const { return GetMap(3); }
  void set_fns(const Map & p) { SetField(3,p); }
  const Map & get_polys() const { return GetMap(4); }
  void set_polys(const Map & p) { SetField(4,p); }
  const Map & get_ops() const { return GetMap(5); }
  void set_ops(const Map & p) { SetField(5,p); }
  const Map & get_tag() const { return GetMap(6); }
  void set_tag(const Map & p) { SetField(6,p); }
  const Map & get_fcts() const { return GetMap(7); }
  void set_fcts(const Map & p) { SetField(7,p); }
};
#endif // VDMSL

class TYEP_SSERR_ErrMsg : public Record
{
  public:
  TYEP_SSERR_ErrMsg () : Record(TAG_TYPE_SSENV_ErrMsg, 5) {}
  TYEP_SSERR_ErrMsg  & Init(const Int & p1, const Int & p2, const Int & p3,
                            const type_cL & p4, const Quote & p5 )
  { SetField(1,p1); SetField(2,p2); SetField(3,p3); SetField(4,p4);
    SetField(5,p5); return *this; }
  TYEP_SSERR_ErrMsg (const Common &c) : Record(c) {}
  const wchar_t * GetTypeName () const { return L"TYEP_SSERR_ErrMsg"; }

  Int get_fid() const { return GetField(1); }
  void set_fid(const Int & p) { SetField(1,p); }
  Int get_line() const { return GetField(2); }
  void set_line(const Int & p) { SetField(2,p); }
  Int get_col() const { return GetField(3); }
  void set_col(const Int & p) { SetField(3,p); }
  type_cL get_err() const { return GetField(4); }
  void set_err(const type_cL & p) { SetField(4,p); }
  Quote get_severity() const { return GetField(5); }
  void set_severity(const Quote & p) { SetField(5,p); }
};

#ifdef VDMSL
#define mk_SSENV_ExpSigRep(tps, val, fns, polys, ops, tag, fcts) \
               TYPE_SSENV_ExpSigRep().Init(tps, val, fns, polys, ops, tag, fcts)
#endif // VDMSL

#define mk_SSENV_TypeRepElem(tp, used, stat)      TYPE_SSENV_TypeRepElem().Init(tp, used, stat)
#define mk_SSENV_OpTypeRepElem(tp, used, stat, oppure) TYPE_SSENV_OpTypeRepElem().Init(tp, used, stat, oppure)
#define mk_SSENV_PolyTypeRepElem(poly, tp, used)  TYPE_SSENV_PolyTypeRepElem().Init(poly, tp, used)

#ifdef VDMPP
#define mk_SSENV_AccessTypeRep(tr, acc, stat)     TYPE_SSENV_AccessTypeRep().Init(tr, acc, stat)
#define mk_SSENV_AccessFnTypeRep(tr, acc, stat)   TYPE_SSENV_AccessFnTypeRep().Init(tr, acc, stat)
#define mk_SSENV_AccessPolyTypeRep(tr, acc, stat) TYPE_SSENV_AccessPolyTypeRep().Init(tr, acc, stat)
#define mk_SSENV_AccessOpTypeRep(tr, acc, stat, oppure) TYPE_SSENV_AccessOpTypeRep().Init(tr, acc, stat, oppure)
#define mk_SSENV_AccessFieldRep(fields, acc)      TYPE_SSENV_AccessFieldRep().Init(fields, acc)
#endif // VDMPP

class StatSem
{
public:
  enum ErrorLevel { ERR, WRN1, WRN2, ERR2, PRF };

private:
  Sequence StatSemErrors;

public:
  TypeCheckInfo namesTypeChecked;
  TYPE_REP_TypeRep TransType (const Generic & modnm,
                              const TYPE_AS_Type & tp) const;
  SEQ<TYPE_REP_TypeRep> TransTypeList (const Generic & modid,
                                       const SEQ<TYPE_AS_Type> & tp_l) const;

private:
// defined in def.vdm
  SET<TYPE_AS_OpDef> trapops;           // set of AS`OpDef

// defined in env.vdm
#ifdef VICE
  bool staticrequired;   //-- special use for setPriority
#endif // VICE
  SET<TYPE_AS_Name> ValueIds;          // set of AS`Name
  SEQ<Int> WhereAmI;     //  seq of <PURE>|<ALL>|<PRE>|<POST>|<IMPL>|<VALUE>
                         // |<PERM>|<PTHREAD> (VDMPP only)
  Map ExtEnv;            // map AS`Name to AS`Mode
  MAP<TYPE_AS_Name,TYPE_SSENV_TypeRepElem> TypeEnv; // map AS`Name to TypeRepElem

#ifdef VDMSL
  bool InsideInter;      // bool
  MAP<TYPE_AS_Name,TYPE_AS_Name> RenamedTypes;   // map AS`Name to AS`Name
  MAP<TYPE_AS_Name,TYPE_REP_TypeRep> TypeParEnv; // map AS`Name to REP`TypeRep
#endif // VDMSL

  SEQ< SET<TYPE_REP_TypeVarRep> > TypeVars; // set of REP`TypeVarRep
  Map TagEnv;            // map AS`Name to TagRepElem(seq of REP`FieldRep)

#ifdef VDMSL
  Map LocTagEnv;         // map AS`Name to TagRepElem(seq of REP`FieldRep)
#endif // VDMSL

  MAP<TYPE_AS_Name,TYPE_SSENV_TypeRepElem> ConstEnv; // map AS`Name to TypeRepElem
  MAP<TYPE_AS_Name,TYPE_SSENV_TypeRepElem> StateEnv; // map AS`Name to TypeRepElem
  Sequence LocalEnv;     // seq of map AS`Name to (REP`TypeRep * nat1)
  MAP<TYPE_AS_Name,TYPE_SSENV_TypeRepElem> FunctionEnv; // map AS`Name to TypeRepElem
  MAP<TYPE_AS_Name,TYPE_SSENV_PolyTypeRepElem> PolyEnv; // map AS`Name to PolyTypeRepElem
  //MAP<TYPE_AS_Name,TYPE_SSENV_TypeRepElem> OperationEnv; // map AS`Name to TypeRepElem
  MAP<TYPE_AS_Name,TYPE_SSENV_OpTypeRepElem> OperationEnv; // map AS`Name to TypeRepElem

#ifdef VDMPP
  Map OverloadedEnv;     // map AS`Name to set of TypeRepElem
  Map SubrespsEnv;        // map AS`Name to map ...
#endif // VDMPP

#ifdef VDMSL
  SET<TYPE_AS_Name> Renamed;    // set of AS`Name
  Generic CheckModule;          // [AS`Name]
#endif // VDMSL

  SET<TYPE_AS_Name> UsedName;   // set of AS`Name
  //Sequence SEnv;                // seq of map AS`Name to TypeRepElem
  SEQ<Map> SEnv;                // seq of map AS`Name to TypeRepElem

#ifdef VDMPP
  Generic CheckClass;           // [AS`Name]
  SET<TYPE_AS_Name> Super;      // set of AS`Name
  SET<TYPE_AS_Name> SuperSuper; // set of AS`Name
  SET<TYPE_AS_Name> TraceEnv;   // set of AS`Name
  Generic CurrentMethod;        // [AS`Name]
  Generic FoundClass;           // [AS`Name]
  SEQ<TYPE_AS_Name> DefiningClass; // seq of AS`Name
                         // Name of the class in which a type was found or NIL
                         // (Used by CG LTO)
  Bool isStatic ;        // bool
#endif //VDMPP
  Map RecMap;                   // map AS`Name to FunctionInfo -- AUGUSTO

  SEQ<TYPE_CI_ContextId> ValueDefContext;

#ifdef VDMPP
  Map LookUpInObjectCache;
  Map LookUpTypeNameCache;
#endif //VDMPP
// defined in vcm.vdm
#ifdef VDMPP
  MAP<TYPE_AS_Name, TYPE_SSENV_ParseTypeInfo> ParseEnv; // map AS`Name to ENV`ParseTypeInfo
  SET<TYPE_AS_Name> classNames; // set of AS`Name
  SET<TYPE_AS_Name> clstypes;
#endif //VDMPP
#ifdef VDMSL
  MAP<TYPE_AS_Name, TYPE_SSENV_ExpSigRep> ModuleEnv; // map AS`Name to ExpSigRep
#endif //VDMSL

  // stmt
  bool fullcheck;
  bool traps;
  Generic curop;                               // [AS`Name]
  SET<TYPE_AS_Stmt> trapids;                   // set of AS`Stmt
  MAP<TYPE_AS_Name, TYPE_REP_TypeRep> direxit; // map AS`Name to REP`TypeRep
  MAP<TYPE_AS_Name, Set> opcalls;              // map AS`Name to set of AS`Name
  MAP<Tuple, Set> trapopcalls;                 // map AS`Name * AS`Stmt to set of AS`Name

  // NumTypes
  SET<Int> intTypes;
  SET<TYPE_REP_TypeRep> natSubTypeSet;
  SET<TYPE_REP_TypeRep> intSubTypeSet;
  SET<TYPE_REP_TypeRep> ratSubTypeSet;
  SET<TYPE_REP_TypeRep> realSubTypeSet;
  bool measureCheck;

  int lastline;
  int lastcol;
  int lastfile;

  bool exp_defined;
  Record expNUMEXPType;

#ifndef NOSS
  /** AS module ***************************************************************/
  /** Reference : Doc. afro-ifad-pgl-statsem-v1.0 *****************************/
  /****************************************************************************/

  /** AUX module **************************************************************/
  /** Reference : Doc. afro-ifad-pgl-statsem-v1.0 *****************************/
  /****************************************************************************/

private:
  bool ReservedPrefix (const TYPE_AS_Name & nm) const;
  TYPE_AS_Name Init (const TYPE_AS_Name & nm) const;
  TYPE_AS_Name Inv (const TYPE_AS_Name & nm) const;
  TYPE_AS_Name Post (const TYPE_AS_Name & nm) const;
  TYPE_AS_Name Pre (const TYPE_AS_Name & nm) const;
  bool IsPrePostFn(const TYPE_AS_Name &) const;
public:
  TYPE_AS_Name ExtName (const TYPE_AS_Name & mod, const TYPE_AS_Name & nm) const;
private:
  TYPE_REP_FnTypeRep MakePostType (const TYPE_REP_FnTypeRep & fntp) const;
  TYPE_REP_FnTypeRep MakePreType (const TYPE_REP_FnTypeRep & fntp) const;
  TYPE_REP_TypeRep SeqMapTypeModify (const TYPE_REP_TypeRep & seqtp,
                                     const TYPE_REP_TypeRep & maptp,
                                     const TYPE_REP_TypeRep & modtp);
  bool RngSubTypeDom(const SET<TYPE_REP_TypeRep> &);
  TYPE_REP_TypeRep InverseMapType (const Generic &) const;
  Tuple CanBeApplied (const Generic & tpi); // not const
  Generic TcauxLookUpTypeName (const TYPE_AS_Name &);
  SET<TYPE_REP_TypeRep> MergeNumericTypes (const SET<TYPE_REP_TypeRep> & sttp) const ;
  SET<TYPE_REP_TypeRep> ExpandNumericTypes (const SET<TYPE_REP_TypeRep> & sttp);
  TYPE_REP_TypeRep MostGeneralNumericType (const SET<TYPE_REP_TypeRep> & ntp);
  TYPE_REP_TypeRep MergeTypes (const Generic &, const Generic &) const;
  Generic IntersectTypes (const TYPE_REP_TypeRep & l, const TYPE_REP_TypeRep & r);
  TYPE_REP_TypeRep IntersectTypeReps (const TYPE_REP_TypeRep & l, const TYPE_REP_TypeRep & r);
  SET<TYPE_REP_TypeRep> IntersectTypeRepInner (const TYPE_REP_TypeRep & TpR1, const TYPE_REP_TypeRep & TpR2);
  SET<TYPE_REP_TypeRep> FlatternTypes (const SET<TYPE_REP_TypeRep> & tps);
  TYPE_REP_TypeRep ExtractIntNumType (const TYPE_REP_TypeRep & numtp);
  TYPE_REP_TypeRep ExtractNumericType (const TYPE_REP_TypeRep & numtp);
  SET<TYPE_REP_TypeRep> ExtractNumericTypeInner (const TYPE_REP_TypeRep & numtp);
  Generic UnmaskSetType(const Int & i, const TYPE_REP_TypeRep & tp) const;
  Generic UnmaskSetTypeForLoop(const Int & i, const TYPE_REP_TypeRep & tp) const;
  TYPE_REP_TypeRep ExtractSetType (const TYPE_REP_TypeRep & sttp);
  SET<TYPE_REP_TypeRep> ExtractSetTypeInner (const TYPE_REP_TypeRep & sttp);
  TYPE_REP_TypeRep SetTypeUnion (const SET<TYPE_REP_TypeRep> & St);
  TYPE_REP_TypeRep SetTypeInter (const SET<TYPE_REP_TypeRep> & St);
  Generic SetTypeMinus (const TYPE_REP_TypeRep & St1, const TYPE_REP_TypeRep & St2);
  Generic UnmaskSeqType(const Int & i, const TYPE_REP_TypeRep & tp);
  Generic UnmaskSeqTypeForLoop(const Int & i, const TYPE_REP_TypeRep & tp);
  TYPE_REP_TypeRep ExtractSeqType (const TYPE_REP_TypeRep & sqtp);
  SET<TYPE_REP_TypeRep> ExtractSeqTypeInner (const TYPE_REP_TypeRep & sqtp);
  TYPE_REP_TypeRep SeqTypeConc (const SET<TYPE_REP_TypeRep> & St);
  Generic UnmaskMapDomType(const Int & i, const TYPE_REP_TypeRep & tp);
  Generic UnmaskMapRngType(const Int & i, const TYPE_REP_TypeRep & tp);
  Generic ExtractMapType (const TYPE_REP_TypeRep & sttp); // not const
  SET<TYPE_REP_TypeRep> ExtractMapTypeInner (const TYPE_REP_TypeRep & sttp); // not const
  TYPE_REP_TypeRep MapTypeMerge (const SET<TYPE_REP_TypeRep> & St);
  Tuple MapTypeDomRestrict (const TYPE_REP_TypeRep &, const Generic &); // not
  Tuple MapTypeRngRestrict (const Generic &, const TYPE_REP_TypeRep &); // not
  Generic ExtractCompositeType (const TYPE_REP_TypeRep & sttp); // not const
  SET<TYPE_REP_TypeRep> ExtractCompositeTypeInner (const TYPE_REP_TypeRep & sttp); // not const
  Generic ExtractProductType (const TYPE_REP_TypeRep & sttp, int n, int op);
  SET<TYPE_REP_TypeRep> ExtractProductTypeInner (const TYPE_REP_TypeRep & sttp, int n, int op);
  SEQ<TYPE_REP_TypeRep> MakeOneProductTypeRep (const SET<TYPE_REP_ProductTypeRep> & prod_s);
  Generic RemoveUnitType (const TYPE_REP_TypeRep & tp);
  TYPE_REP_TypeRep RemoveEmptySetType(const TYPE_REP_TypeRep &);
  TYPE_REP_TypeRep RemoveEmptySeqType(const TYPE_REP_TypeRep &);
  TYPE_REP_TypeRep RemoveEmptyMapType(const TYPE_REP_TypeRep &);
public:
  bool IsEmptyName(const TYPE_AS_Name &) const;
private:
  TYPE_AS_CallStmt ApplyToCall (const TYPE_AS_ApplyExpr & arc);
  bool CompareProductLength (int s, int n, int op) const;

  TYPE_REP_TypeRep AlwaysType(const TYPE_REP_TypeRep &, const TYPE_REP_TypeRep &) const;
  Generic ExtractExitTypeRep (const TYPE_REP_TypeRep &) const;
  Tuple MakePolyPostType (const SEQ<TYPE_REP_TypeVarRep> & tv_l, const TYPE_REP_FnTypeRep & fntp) const;
  Tuple MakePolyPreType (const SEQ<TYPE_REP_TypeVarRep> & tv_l, const TYPE_REP_FnTypeRep & fntp) const;
  Record InstFnAS (const Record & gtp, const MAP<TYPE_AS_Name,TYPE_AS_Type> & bind) const;
  TYPE_REP_TypeRep InstFn (const Int & kind,
                           const TYPE_REP_TypeRep & tp,
                           const MAP<TYPE_AS_Name,TYPE_REP_TypeRep> & bind) const;
  Tuple SplitFunType(const Generic &) const;
  Generic ExtractFunType (const TYPE_REP_TypeRep &);
  SET<TYPE_REP_TypeRep> ExtractFunTypeInner (const TYPE_REP_TypeRep &);
  Tuple ExtractComposeType(const Int &, const TYPE_REP_TypeRep &, const TYPE_REP_TypeRep &);
#ifdef VDMSL
  bool TypeUsedImplicitly(const TYPE_AS_Name &, const SET<TYPE_REP_TypeRep> &);
  bool ImplicitlyRec(const TYPE_AS_Name &, const TYPE_REP_TypeRep&);
  TYPE_REP_TypeRep RenameTag (const TYPE_REP_TypeRep & tp, const MAP<TYPE_AS_Name,TYPE_AS_Name> & bind) const;
//  TYPE_REP_TypeRep MakeOpPostType (const Generic & modid, const TYPE_REP_OpTypeRep & optp, const Generic & stdef);
//  TYPE_REP_TypeRep MakeOpPreType (const Generic & modid, const TYPE_REP_OpTypeRep & optp, const Generic & stdef);
  bool CheckImportedTag(const TYPE_AS_Name &);
#endif //VDMSL
  TYPE_REP_TypeRep MakeOpPostType (const Generic & modid, const TYPE_REP_OpTypeRep & optp, const Generic & stdef);
  TYPE_REP_TypeRep MakeOpPreType (const Generic & modid, const TYPE_REP_OpTypeRep & optp, const Generic & stdef);
  Tuple SplitMapType(const Generic &) const;

#ifdef VDMPP
  TYPE_REP_TypeRep QualifyTypeNames(const TYPE_REP_TypeRep &, const TYPE_AS_Name &);
  Generic ExtractSetObjRefType (const TYPE_REP_TypeRep &);
  Generic ExtractObjRefType (const TYPE_REP_TypeRep & sttp);
  SET<TYPE_REP_TypeRep> ExtractObjectTypeInner (const TYPE_REP_TypeRep & sttp);
#endif //VDMPP

  /** DEF module **************************************************************/
  /** Reference : Doc. afro-ifad-pgl-statsem-v1.0 *****************************/
  /****************************************************************************/

private:
  bool IsFlat (const Generic & tp, const SET<TYPE_AS_Name> & checked);
  bool wf_TypeList (const Int &, const SEQ<TYPE_AS_Type> &);
  bool wf_Type (const Int & i, const TYPE_AS_Type & tp);
  bool wf_TypeInv (const Int & i, const TYPE_REP_TypeRep & Type, const Generic & Inv, const TYPE_AS_Name & nm);
  bool wf_ImplFunction (const Int & i, const TYPE_AS_ImplFnDef & vFnDef);
  bool wf_ExplFunction (const Int & i, const TYPE_AS_ExplFnDef & vFnDef);
  bool wf_ExtExplFunction (const Int & i, const TYPE_AS_ExtExplFnDef & vFnDef);
  Tuple wf_Pred (const Int & i, const Generic & Pred, const Int & kind);
  Tuple wf_Body(const Int & i, const TYPE_REP_TypeRep & body, const Record & rng_tp, const SET<TYPE_AS_Name> & names);
  bool IsPrelim (const Record & bdy);

#ifdef VDMPP
  bool wf_InstanceVars (const Int &, const SEQ<TYPE_AS_InstanceVarDef> &);
  bool wf_Sync (const Int & i, const TYPE_AS_Name & clsnm, const SEQ<TYPE_AS_SyncDef> & syncs);
  bool wf_DeclarativeSync (const Int & i, const TYPE_AS_Name & clsnm, const SEQ<TYPE_AS_DeclarativeSync> & syncs);
  bool wf_Thread (const Int & i, const Generic & thread, const TYPE_AS_Name & clsnm);
  bool wf_PeriodicThread (const Int & i, const TYPE_AS_PerObl & thread, const TYPE_AS_Name & clsnm);
  bool wf_SporadicThread (const Int & i, const TYPE_AS_SpoObl & thread, const TYPE_AS_Name & clsnm);
  bool wf_ProceduralThread (const Int & i, const TYPE_AS_Stmt & thread);
#endif //VDMPP
  bool wf_Traces (const Int & i, const Map & thread);
  bool wf_TraceDefList(const Int & i, const TYPE_AS_TraceDefList & tdl);
  bool wf_TraceDefTerm(const Int & i, const TYPE_AS_TraceDefTerm & tdt);

#ifdef VDMSL
  bool wf_TraceApplyExpr(const Int & i,
                         const TYPE_AS_Name & op,
                         const SEQ<TYPE_AS_Expr> & args);
#endif // VDMSL
#ifdef VDMPP
  bool wf_TraceApplyExpr(const Int & i,
                         const Generic & obj,
                         const TYPE_AS_Name & op,
                         const SEQ<TYPE_AS_Expr> & args);
#endif // VDMPP
  bool wf_QualifiedTrace(const Int & i,
                         const SEQ<TYPE_AS_TraceBind> & binds,
                         const TYPE_AS_SimpleTrace & tdef);
  bool wf_RepeatTrace(const Int & i, const TYPE_AS_SimpleTrace & tdef, const TYPE_AS_TraceRepeatPattern & pat);
  bool wf_QualifiedRepeatTrace(const Int & i,
                               const SEQ<TYPE_AS_TraceBind> & binds,
                               const TYPE_AS_SimpleTrace & tdef,
                               const TYPE_AS_TraceRepeatPattern & pat);
  bool wf_TraceDefAltn(const Int & i, const TYPE_AS_TraceDefTerm & term, const TYPE_AS_TraceDef & tdef);
  bool wf_TraceRepeatPattern(const Int & i, const TYPE_AS_TraceRepeatPattern & trp);
  Tuple wf_TraceBind(const Int & i, const TYPE_AS_TraceBind & bind);
  Tuple wf_LocalTraceBind(const Int & i, const TYPE_AS_LocalTraceBind & bind);
  Tuple wf_LetTraceBind(const Int & i, const TYPE_AS_LetTraceBind & bind);
  Tuple wf_LetBeTraceBind(const Int & i, const TYPE_AS_LetBeTraceBind & bind);

#ifdef VDMSL
  bool wf_State (const Int & i, const TYPE_AS_StateDef & state);
#endif //VDMSL

  bool wf_Operations (const Int & i, const Map & opmap);
  bool wf_ExplOperation (const Int & i, const TYPE_AS_ExplOpDef & vOpDef);
  bool wf_ImplOperation (const Int & i, const TYPE_AS_ImplOpDef & vOpDef);
  bool wf_ExtExplOperation (const Int & i, const TYPE_AS_ExtExplOpDef & vOpDef);
  bool wf_TypeDefs (const Int & i, const Map & tp_m);
  bool wf_Functions (const Int & i, const Map & fn_l);

public:
  Tuple extractDomRng(const Generic & g);
  bool VerifyRng(const TYPE_REP_TypeRep & tp);
  bool VerifyMutRec(const SET<TYPE_AS_Name> & mutRec, const TYPE_AS_FnDef & measdef, const TYPE_AS_Name & nmq);
  bool NameInSet(const TYPE_AS_Name & n, const SET<TYPE_AS_Name> & sn);
  bool wf_Measure(const Int & i, const TYPE_AS_Name & nmq,
                  const Generic & measu, const SEQ<TYPE_REP_TypeRep> & fndom,
                  const TYPE_AS_FnType & tp);

  /** ENV module **************************************************************/
  /** Reference : Doc. afro-ifad-pgl-statsem-v1.0 *****************************/
  /****************************************************************************/

private:
  //bool EquivDomFn(const Int & i, const SEQ<TYPE_REP_TypeRep> & fndom1, const SEQ<TYPE_REP_TypeRep> & fndom2);
  bool EquivDomFn(const Int & i, const SEQ<TYPE_REP_TypeRep> & fndom1, const SEQ<TYPE_REP_TypeRep> & fndom2,
                  const TYPE_REP_TypeRep & fnrng1, const TYPE_REP_TypeRep & fnrng2);
  bool CheckAllType(const TYPE_AS_Type & tp);
  Tuple wf_ValueDef (const Int & i, const TYPE_AS_ValueDef & def);
  Map   TransBinds (const TYPE_AS_Name & clsnm, const Map & binds);
  Int GetContext ();
  void PushContext (const Int & kind);
  void PopContext ();
  bool ColRecs (const TYPE_AS_Type & tp);
  bool ExpandCompositeType (const TYPE_AS_CompositeType & ctp);
  bool CheckTag (const TYPE_AS_Name & nm);
  TYPE_REP_TypeRep TransTypeDef (const Generic & modnm, const TYPE_AS_Name & nm,
                                 const TYPE_AS_Type & tp, const Generic & inv);
  SET<TYPE_REP_TypeRep> FlatternUnion (const SET<TYPE_REP_TypeRep> & s) const;
  SEQ<TYPE_REP_FieldRep> TransFields (const Generic & modnm, const SEQ<TYPE_AS_Field> & f_l) const;
  bool CheckTypeName (const TYPE_AS_Name & nm);

public:
  void setRecMap(const Map & rmap);
  Map getRecMap();

private:
  void UpdateScope (const MAP<TYPE_AS_Name,Tuple> & bindings);
  void EnterScope (const MAP<TYPE_AS_Name,Tuple> & bindings);
  void LeaveScope ();
public:
  SEQ<TYPE_AS_Type> ConstructImplDomType (const SEQ<TYPE_AS_PatTypePair> & partps) const;
  TYPE_AS_Type ConstructImplRngType (const SEQ<TYPE_AS_NameType> & nmtps) const;
private:
  SEQ<TYPE_AS_Pattern> ConstructImplParms (const SEQ<TYPE_AS_PatTypePair> & partps) const;
  SEQ<Char> PrintName(const TYPE_AS_Name & name) const;
  MAP<TYPE_AS_Name, Tuple> ExtractValueBindings (const SEQ<TYPE_AS_NameType> & valtps);
  void EnterLocalScope (const MAP<TYPE_AS_Name,Tuple> & bindings);
  void LeaveLocalScope ();
  void UpdateLocalScope (const MAP<TYPE_AS_Name,Tuple> & bindings);
public:
  Generic LookUp (const TYPE_AS_Name & nm, bool printErr);
#ifdef VDMSL
  Generic LookUpTypeName (const TYPE_AS_Name & nm);
#endif // VDMSL
#ifdef VDMPP
  Generic LookUpTypeName (const TYPE_AS_Name & nm, bool printerr);
  Generic LookUpTypeName_q (const TYPE_AS_Name & nm, bool printerr);
#endif // VDMLL
  Generic LookUpTag (const Int & i, const TYPE_AS_Name & nm, const SET<TYPE_AS_Name> &);
  Generic GetCurModOrNil() const;
#ifdef VDMSL
private:
  TYPE_AS_Name GetCurMod () const;
  void SetCurMod (const TYPE_AS_Name & modnm);
#endif
private:
#ifdef VDMPP
  void CheckLookupStatic(const TYPE_AS_Name & nm, const Bool & stat);
#endif //VDMPP
  bool ExtractExplicitFunctionSignature (const TYPE_AS_ExplFnDef & fndef);
  bool ExtractImplicitFunctionSignature (const TYPE_AS_ImplFnDef & fndef);
  bool ExtractExtExplicitFunctionSignature (const TYPE_AS_ExtExplFnDef & fndef);
  SET<TYPE_AS_Name> TransClos (const TYPE_AS_Name & nm);
  SET<TYPE_AS_Name> TransClosAux (const TYPE_AS_Name & nm, const SET<TYPE_AS_Name> & done);

  Bool CheckParmsInHierarchy(const TYPE_AS_Name & nm,
                             const SEQ<TYPE_REP_TypeRep> & ptps,
                             const TYPE_AS_Access & p_acc,
                             const Bool & op_or_fn);
  Map LookUpParmsInHierarchy(const TYPE_AS_Name & nm, const Bool & op_or_fn);
  Map ExtractParmsMap(const TYPE_AS_Name & nm, const TYPE_AS_Name & classnm, const Map & meths);

  bool ExtractValueDefs (const Int & i, const SEQ<TYPE_AS_ValueDef> & vals);
  bool CheckName (const TYPE_AS_Name & nm);
  void MarkUsedType (const TYPE_AS_Type & tp);
  bool CheckLocalTag (const TYPE_AS_Name & nm);
  bool SetExt (const SEQ<TYPE_AS_ExtInf> & ext_l, const SET<TYPE_AS_Name> & nm_s);

#ifdef VDMPP
  Generic LookUpInObject(const TYPE_AS_Name & obj, const TYPE_AS_Name & nm, bool writable, bool printError);
  Generic LookUpInObjectImpl(const TYPE_AS_Name & obj, const TYPE_AS_Name & nm, bool writable, bool printError);
#ifdef VICE
  void NotStaticOk();
  void StaticRequired();
#endif // VICE
  TYPE_SSENV_ParseTypeInfo MakeEmptyClass (const TYPE_AS_Name & nm);
  bool IsLocalSuperSuper (const TYPE_AS_Name & nm) const;
  Tuple ExtractPatterns (const Int & i, const SEQ<TYPE_AS_LocalDef> & vals);

  bool ExpandInstanceVars (const SEQ<TYPE_AS_InstanceVarDef> & inst_l);
  bool ExtractOverloadedSignatures(const MAP<TYPE_AS_Name,TYPE_AS_OpDef> & p_opm,
                                   const MAP<TYPE_AS_Name,TYPE_AS_FnDef> & p_fnm);
  bool OverloadedTypeConsistent(const TYPE_AS_Name & p_nm, const SEQ<TYPE_REP_TypeRep> & p_domtp, bool p_isop);
  void InsertOverloadedName(const TYPE_AS_Name & p_nm,
                            const SEQ<TYPE_REP_TypeRep> & p_domtps,
                            const TYPE_REP_TypeRep & p_rngtp,
                            bool p_isop,
                            bool p_static);
  SEQ<TYPE_REP_TypeRep> NormaliseTypeNameList(const SEQ<TYPE_REP_TypeRep> & p_tpL);
  SET<TYPE_SSENV_AccessType> NormaliseAccessTypeSet(const SET<TYPE_SSENV_AccessType> & p_acctpS);
  TYPE_SSENV_AccessType NormaliseAccessType(const TYPE_SSENV_AccessType & p_acctp);
public:
  TYPE_REP_TypeRep NormaliseTypeName(const TYPE_REP_TypeRep & p_tp);
private:
  Tuple GetOperationSignature(const TYPE_AS_OpDef & op);
  Tuple GetFunctionSignature(const TYPE_AS_FnDef & fn);
  bool GetOperationStaticStatus(const TYPE_AS_OpDef & op);
  bool GetFunctionStaticStatus(const TYPE_AS_FnDef & op);
  Map TransClosIndex(const TYPE_AS_Name & nm);
  Map TransClosIndexAux(const TYPE_AS_Name & nm, int base, const SET<TYPE_AS_Name> & done);
  void SetStatic(const Bool &);
  void UnsetStatic();
  Bool GetStatic() const;
  bool IsInstanceVar_q(const TYPE_AS_Name & nm, bool);
  TYPE_REP_TypeRep PrependClassName (const TYPE_AS_Name & clsnm, const TYPE_REP_TypeRep & tp);
  bool IsSubClass (const TYPE_AS_Name & nm1, const TYPE_AS_Name & nm2);
  Generic LookUpInHierarchy (const TYPE_AS_Name & nm, const Generic & clsid, int, int);
  Map LookUpInHierarchyAux (const TYPE_AS_Name & nm, const TYPE_AS_Name & sup, int, int, const Set & checked);
  bool InstallCurClass (const TYPE_AS_Name &, const SEQ<TYPE_AS_Name> &);
  Map RemoveHidden(const Map &, const Map &);
  TYPE_REP_TypeRep SubSuperClType(const TYPE_AS_Name & clsnm, const TYPE_REP_TypeRep & typerep);
public:
  Generic LookUpTypeInSuper(const TYPE_AS_Name & tpnm, const TYPE_AS_Name & clsnm);
  TYPE_AS_Name GetCurClass () const;
  void  SetCurClass (const TYPE_AS_Name&);
private:
  bool CheckClassName (const TYPE_AS_Name &);
  bool HasThread(const TYPE_AS_Name & nm);
  bool CheckTraceName (const TYPE_AS_Name &) const;
  Generic FoundAsInstanceInSEnv(const TYPE_AS_Name & nm);
  TYPE_SSENV_ParseTypeInfo LookUpClass (const TYPE_AS_Name &);
  bool DefaultConstructorAccessible(const TYPE_AS_Name &);
  bool ConstructorExists (const Int & p_i,
                          const TYPE_AS_Name & p_nm,
                          const SEQ<TYPE_REP_TypeRep> & p_argTps,
                          const TYPE_AS_Expr & p_expr,
                          bool local);
  Map LookUpConstructors(const TYPE_AS_Name & p_nm);
  SET<TYPE_SSENV_AccessType> TypesAreConsistent(const Set &);
  Map ExtractValue(const Set& p_defms, bool p_isGlobal, const TYPE_AS_Name & p_qname, const TYPE_AS_Name & p_sup);
  bool ExtractOperationSignatures (const MAP<TYPE_AS_Name,TYPE_AS_OpDef> & fn_l);
  bool ExtractExplicitOperationSignature (const TYPE_AS_ExplOpDef & fndef);
  bool ExtractImplicitOperationSignature (const TYPE_AS_ImplOpDef & fndef);
  bool ExtractExtExplicitOperationSignature (const TYPE_AS_ExtExplOpDef & fndef);
  Generic CheckAccess (const TYPE_AS_Name &, const Generic &);
  Generic CheckAccessCurClass (const Generic &);
  bool IsAccessible (const TYPE_AS_Name &, const TYPE_SSENV_AccessType &);
  bool IsAccessibleCurClass (const TYPE_SSENV_AccessType &);
 public:
  bool IsAccessType (const Generic &) const;
  bool IsSetOfAccessType (const Generic &) const;
 private:
  bool IsAccessTypeSet(const Generic & a) const;
  Bool GetSSStatic(const Generic & a) const;
  TYPE_AS_Access GetSSAccess(const Generic & a) const;
  bool IsAccessOpFnPolyTypeRep(const Generic & a) const;
  bool IsOpFnTypeRep(const Generic & a) const;
  bool IsOpFnTypeRepSet(const Generic & a) const;
  bool IsTypeRep(const Generic & a) const;
  SEQ<TYPE_REP_TypeRep> GetMethDom(const TYPE_REP_TypeRep&);
  TYPE_REP_TypeRep GetMethRng(const TYPE_REP_TypeRep& p_atr);
  Generic StripAccessType (const Generic &);
  bool CheckPatternBindAccess (const TYPE_AS_Access &, const MAP<TYPE_AS_Name,Tuple> &);
  bool CheckTypeListAccess (const TYPE_AS_Name &, const SEQ<TYPE_AS_Type> &);
  bool CheckInstAccess(const TYPE_AS_Access &, const TYPE_AS_Name &);
  bool CheckDefAccess(const TYPE_SSENV_AccessType &, const TYPE_AS_Name &);
  bool CheckTypeDefAccess(const TYPE_AS_Access &, const TYPE_AS_Name &);
  bool CheckTypeAccess (const TYPE_AS_Name &, const TYPE_AS_Type &);
  bool CheckTypeAccessAux (const TYPE_AS_Access &, const TYPE_AS_Type &);
  bool CheckThreadInherit(const TYPE_AS_Name & clsnm);
  TYPE_AS_Access LookUpAccessForType (const Record &, const TYPE_AS_Name &);
  bool NotNarrowedAccess (const TYPE_AS_Access &, const TYPE_AS_Access &);
  bool CheckSubRespIsImplemented(const Int & i, const TYPE_AS_Name & nm);
  Tuple GetSubResps(const TYPE_AS_Name & nm);
  Map ChangeToStatic(const Map & m);
#endif //VDMPP

#ifdef VDMSL
  Generic LookUpPolyFn (const TYPE_AS_Name & nm);
#endif // VDMSL
  bool CheckTypeVar (const TYPE_REP_TypeVarRep &);
  bool CheckOperationName (const TYPE_AS_Name&);
  bool ExpandDefinitions (const Int & i, const TYPE_AS_Name & nm, const TYPE_AS_Definitions & defs);
  SET<TYPE_AS_Name> UsedStateIds(const SET<TYPE_AS_Name> &);
  bool ExpandTypes (const TYPE_AS_Name &, const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> &);
  bool ExtractFunctionSignatures (const MAP<TYPE_AS_Name,TYPE_AS_FnDef> &);

  bool SetExtAll (const SET<TYPE_AS_Name> &);
  void PushTypeVarContext (const SEQ<TYPE_REP_TypeVarRep> &);
  void PopTypeVarContext ();
  Generic LookUpState (const TYPE_AS_Name& nm, const Bool &, const Int &);
#ifdef VDMSL
  bool CheckImpTypeDef(const TYPE_REP_TypeRep & tp, const TYPE_AS_Name & modid);
  bool ExpandTypeImports (const Int & i, const TYPE_AS_Name & modid, const TYPE_AS_Interface & intf);
  Tuple CheckTypeRename (const Map & tps, const MAP<TYPE_AS_Name, TYPE_AS_Name> & ren, const TYPE_AS_Name & modnm);
  bool ExpandTypeImport (const Int & i, const TYPE_AS_Name & modid, const Generic & sig, const TYPE_AS_Name & curmod);
  bool ExpandExport (const TYPE_AS_ExportSig & es);
  bool ExpandStateIdentifiers (const TYPE_AS_Name & modid, const Generic & stdef);
  void LeaveCurModule (const TYPE_AS_Name & modid, const Generic & exp);
  TYPE_REP_TypeRep CurMod (int kind, const TYPE_AS_Name & curmod, const TYPE_REP_TypeRep & tp);
  TYPE_REP_TypeRep TransStateType (const Generic &, const TYPE_AS_StateDef &);
  Tuple ExtractPatterns (const Int & i, const SEQ<TYPE_AS_LocalDef> & vals);
  bool ExtractOperationSignatures (const MAP<TYPE_AS_Name,TYPE_AS_OpDef> & fn_l, const Generic & stdef);
  bool ExtractExplicitOperationSignature (const TYPE_AS_ExplOpDef & fndef, const Generic & stdef);
  bool ExtractImplicitOperationSignature (const TYPE_AS_ImplOpDef & fndef, const Generic & stdef);
  bool ExtractExtExplicitOperationSignature (const TYPE_AS_ExtExplOpDef & fndef, const Generic & stdef);
  void InstallParEnv (const MAP<TYPE_AS_Name,TYPE_REP_TypeRep> &);
  bool ExpandInterface (const Int & i, const TYPE_AS_Name & modid, const TYPE_AS_Interface & intf);
  bool ExpandImports (const Int & i, const TYPE_AS_Name & modid, const Map & imp_m);
  bool ExpandImport (const Int & i, const TYPE_AS_Name & modid, const Generic & sig, const TYPE_AS_Name & curmod);
  bool ImportExpl (const Int &,
                   const TYPE_AS_ImportSig &,
                   const TYPE_SSENV_ExpSigRep &,
                   const Bool &,
                   const TYPE_AS_Name &,
                   const TYPE_AS_Name &);
  Tuple CheckRename (const TYPE_AS_ImportSig & is, const TYPE_AS_Name & modid);
  bool ImportTypes (const Int & i,
                    const MAP<TYPE_AS_Name, TYPE_AS_Name> & ren,
                    const Map & imptps,
                    const MAP<TYPE_AS_Name, TYPE_REP_TypeRep> & exptps,
                    const TYPE_AS_Name & modid,
                    const TYPE_AS_Name & curmod);
  bool ImportValues (const Int & i,
                     const MAP<TYPE_AS_Name, TYPE_AS_Name> & ren,
                     const Map & impval,
                     const MAP<TYPE_AS_Name, TYPE_REP_TypeRep> & expval,
                     const Bool &,
                     const TYPE_AS_Name & modid,
                     const TYPE_AS_Name & curmod);
  bool ImportFunctions (const Int & i,
                        const MAP<TYPE_AS_Name, TYPE_AS_Name> & ren,
                        const Map & impfns,
                        const MAP<TYPE_AS_Name, TYPE_REP_FnTypeRep> & expfns,
                        const MAP<TYPE_AS_Name, TYPE_REP_PolyTypeRep> & polyfns,
                        const Bool &,
                        const TYPE_AS_Name & modid,
                        const TYPE_AS_Name & curmod);
  bool ImportOperations (const Int & i,
                         const MAP<TYPE_AS_Name, TYPE_AS_Name> & ren,
                         const Map & impops,
                         const MAP<TYPE_AS_Name, TYPE_REP_OpTypeRep> & expops,
                         const MAP<TYPE_AS_Name, TYPE_REP_FnTypeRep> & expfns,
                         const Bool &,
                         const TYPE_AS_Name & modid,
                         const TYPE_AS_Name & curmod);
  bool ImportTags (const Map & exptag, const MAP<TYPE_AS_Name, TYPE_AS_Name> & ren);
#endif //VDMSL
  bool ExtractValueDefsFirst (const Int & i, const SEQ<TYPE_AS_ValueDef> & vals);

#ifdef VDMPP
  bool IsStaticInstanceVar(const TYPE_AS_Name & nm);
  bool IsInstanceVar(const TYPE_AS_Name & nm);
  bool HasInstInv(const TYPE_AS_Name & nm);
  Generic LookUpInstanceVar(const TYPE_AS_Name & nm);
#endif //VDMPP

  void PushVDContext(const TYPE_CI_ContextId & cid);
  void PopVDContext();
  bool IsInVDContext();

  /** ERR module **************************************************************/
  /** Reference : Doc. afro-ifad-pgl-statsem-v1.0 *****************************/
  /****************************************************************************/

  void UpdateLastPosition (const Record & ast);
  SEQ<Char> LookUpErrMsg(int num, const SEQ< SEQ<Char> > & Txts);

  void GenErr (const Generic &, int, int, const SEQ< SEQ<Char> > & Txts);

  void GenErrTp (const Generic & ast, int SvTp, int errnum,
                 const Generic & tp1, const Generic & tp2,
                 const SEQ< SEQ<Char> > & Txts);

  void InternalError(const wstring &) const;

  const wchar_t * getErrMsg(int num);
  string getErrMsgJp(int num);

  wstring GenLineColNo (const Record & ast);

public:
  Sequence GetErrors() const;
  void ResetErrors();
  wstring ASType2Ascii (const TYPE_AS_Type & tp) const;
  wstring Type2Ascii (const TYPE_REP_TypeRep & tp, int level = 0) const;

private:

  /** EXPR module *************************************************************/
  /** Reference : Doc. afro-ifad-pgl-statsem-v1.0 *****************************/
  /****************************************************************************/

//  Tuple wf_GetValueExpr (Int i, Record rc);
  Tuple CheckUnionApply (const Int & i,
                         const TYPE_REP_TypeRep & tp1,
                         const SEQ<TYPE_REP_TypeRep> & tp_l,
                         const Record & vApply);
  Tuple wf_BracketedExpr (const Int & i, const TYPE_AS_BracketedExpr & vBracket, const TYPE_REP_TypeRep & exptp);
  Tuple wf_Macro (const Int & i, const TYPE_AS_Macro & vMacro, const TYPE_REP_TypeRep & exptp);
  Tuple wf_LambdaExpr (const Int & i, const TYPE_AS_LambdaExpr & VLambdaExpr, const TYPE_REP_TypeRep &);
  Tuple wf_COMPOSE (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep &);
  Tuple wf_FctTypeInstExpr (const Int & i, const TYPE_AS_FctTypeInstExpr& FctTypeExpr, const TYPE_REP_TypeRep &);
  Tuple CheckFctTypeInst (const Int & i, const Generic & poly, const TYPE_AS_FctTypeInstExpr & field);
  TYPE_REP_TypeRep CheckFctTypeInstAux (const Tuple & poly, const TYPE_AS_FctTypeInstExpr & field);
  Tuple wf_TokenConstructorExpr (const Int & i,
                                 const TYPE_AS_TokenConstructorExpr & TokenConstr,
                                 const TYPE_REP_TypeRep & tp);
  Tuple wf_Expr(const Int & i, const TYPE_AS_Expr & exp, const TYPE_REP_TypeRep & tp);
  Tuple wf_BinaryExpr (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_NUMPLUS (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_NUMMINUS (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_NUMMULT (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_NUMDIV (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_NUMREM (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_NUMMOD (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_INTDIV (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_NUMEXP (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple EXPType(const Int & i, const TYPE_REP_TypeRep &, const TYPE_REP_TypeRep &, const Bool &, const TYPE_AS_Expr &);
  TYPE_REP_TypeRep ExpectedNUMEXPType ();
  Tuple wf_NUMLT (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_NUMLE (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_NUMGT (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_NUMGE (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_AND (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_OR (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_IMPLY (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_EQUIV (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_EQ (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_NE (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_SETUNION (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_SETINTERSECT (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_SETMINUS (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_SUBSET (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_PROPERSUBSET (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_INSET (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_NOTINSET (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_SEQCONC (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_MAPMERGE (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_MAPDOMRESTTO (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_MAPDOMRESTBY (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_MAPRNGRESTTO (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);
  Tuple wf_MAPRNGRESTBY (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & tp);

  Tuple wf_PrefixExpr (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_UNARYNUMPLUS (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_UNARYNUMMINUS (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_NUMABS (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_FLOOR (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_NOT (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_SETCARD (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_SETDISTRUNION (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_SETDISTRINTERSECT (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_SETPOWER (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_SEQLEN (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_SEQDISTRCONC (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_SEQELEMS (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_SEQINDICES (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_SEQHEAD (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_SEQTAIL (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_SEQREVERSE (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_MAPDOM (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_MAPRNG (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_MAPDISTRMERGE (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_MAPINVERSE (const Int & i, const TYPE_AS_PrefixExpr &, const TYPE_REP_TypeRep &);

  Tuple wf_DefExpr (const Int & i, const TYPE_AS_DefExpr & exp, const TYPE_REP_TypeRep & tp);
  Tuple wf_LetExpr (const Int & i, const TYPE_AS_LetExpr & exp, const TYPE_REP_TypeRep & tp);
  Bool wf_LocalDefs(const Int & i, const SEQ<TYPE_AS_LocalDef> & defs);
  Tuple wf_LetBeSTExpr (const Int & i, const TYPE_AS_LetBeSTExpr & exp, const TYPE_REP_TypeRep & tp);
  Tuple wf_IfExpr (const Int & i, const TYPE_AS_IfExpr & exp, const TYPE_REP_TypeRep & tp);
  Tuple wf_ElseIfExprs (const Int & i, const SEQ<TYPE_AS_ElseifExpr> & else_l, const TYPE_REP_TypeRep & tp);
  Tuple wf_ElseIfExpr (const Int & i, const TYPE_AS_ElseifExpr & exp, const TYPE_REP_TypeRep & tp);
  Tuple wf_CasesExpr (const Int & i, const TYPE_AS_CasesExpr  & exp, const TYPE_REP_TypeRep & tp);
  Tuple wf_CaseAltn (const Int & i,
                     const SEQ<TYPE_AS_Pattern> &,
                     const TYPE_AS_Expr & b,
                     const TYPE_REP_TypeRep & tp,
                     const TYPE_REP_TypeRep &);
  Tuple wf_AllOrExistsExpr (const Int & i, const TYPE_AS_AllOrExistsExpr & exp, const TYPE_REP_TypeRep &);
  Tuple wf_ExistsUniqueExpr (const Int & i, const TYPE_AS_ExistsUniqueExpr & exp, const TYPE_REP_TypeRep &);
  Tuple wf_IotaExpr (const Int & i, const TYPE_AS_IotaExpr & exp, const TYPE_REP_TypeRep &);
  Tuple wf_SetEnumerationExpr (const Int & i, const TYPE_AS_SetEnumerationExpr & SetEnum, const TYPE_REP_TypeRep &);
  Tuple wf_SetComprehensionExpr (const Int & i,
                                 const TYPE_AS_SetComprehensionExpr & vSetComprehension,
                                 const TYPE_REP_TypeRep &);
  Tuple wf_SetRangeExpr (const Int & i, const TYPE_AS_SetRangeExpr & vSetRange, const TYPE_REP_TypeRep &);
  Tuple wf_SeqEnumerationExpr (const Int & i, const TYPE_AS_SeqEnumerationExpr & SeqEnum, const TYPE_REP_TypeRep &);
  Tuple wf_SeqComprehensionExpr (const Int & i,
                                 const TYPE_AS_SeqComprehensionExpr & vSeqComprehension,
                                 const TYPE_REP_TypeRep &);
  Tuple wf_SeqModify (const Int &, const TYPE_REP_TypeRep & s, const TYPE_REP_TypeRep & m, const TYPE_AS_Expr &);
  Tuple wf_MapOverride (const TYPE_REP_TypeRep & m1, const TYPE_REP_TypeRep & m2, const TYPE_AS_Expr &);
  Tuple wf_SeqModifyMapOverrideExpr (const Int & i,
                                     const TYPE_AS_SeqModifyMapOverrideExpr & SeqModMapOverride,
                                     const TYPE_REP_TypeRep &);
  Tuple wf_SubSequenceExpr (const Int & i, const TYPE_AS_SubSequenceExpr & vSubSequence, const TYPE_REP_TypeRep &);
  Tuple wf_Maplet (const Int & i, const TYPE_AS_Maplet & maplet, const TYPE_REP_TypeRep &);
  Tuple wf_MapEnumerationExpr (const Int & i, const TYPE_AS_MapEnumerationExpr & MapEnum, const TYPE_REP_TypeRep &);
  Tuple wf_MapComprehensionExpr (const Int & i,
                                 const TYPE_AS_MapComprehensionExpr & vMapComprehension,
                                 const TYPE_REP_TypeRep &);
  Tuple wf_TupleConstructorExpr (const Int & i,
                                 const TYPE_AS_TupleConstructorExpr & TupleConstr,
                                 const TYPE_REP_TypeRep &);
  Tuple wf_TupleSelectExpr (const Int &, const TYPE_AS_TupleSelectExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_TypeJudgementExpr (const Int &, const TYPE_AS_TypeJudgementExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_PreConditionApplyExpr (const Int &, const TYPE_AS_PreConditionApplyExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_RecordConstructorExpr (const Int & i, const TYPE_AS_RecordConstructorExpr&, const TYPE_REP_TypeRep &);
  bool CheckModifications (const Int & i,
                           const TYPE_REP_TypeRep & tp,
                           const SEQ<TYPE_AS_RecordModification> & mods,
                           const TYPE_REP_TypeRep &);
  Tuple wf_RecordModifierExpr (const Int & i,
                         const TYPE_AS_RecordModifierExpr & vRecordModifier,
                         const TYPE_REP_TypeRep &);
#ifdef VDMPP
  Tuple CheckOverloadedApply(const Int & p_i,
                             const SET<TYPE_REP_TypeRep> & p_overloadedTps,
                             const SEQ<TYPE_REP_TypeRep> & p_argTps,
                             const TYPE_AS_Expr& p_exr,
                             bool p_repErr,
                             bool p_retRng);
#endif //VDMPP
  Tuple CheckBasicApply (const Int & i,
                         const TYPE_REP_TypeRep & tp1,
                         const SEQ<TYPE_REP_TypeRep> & tp_l,
                         const Record &,
                         const Bool &);
  Tuple CheckHigherOrder (const Int &,
                          const TYPE_REP_TypeRep &,
                          const SEQ<TYPE_REP_TypeRep> &,
                          const TYPE_REP_TypeRep &,
                          const Record &);
  TYPE_REP_PartialFnTypeRep CreateFnType (const Sequence &);
  Tuple wf_ApplyExpr (const Int & i, const TYPE_AS_ApplyExpr & vApply, const TYPE_REP_TypeRep &);
  Tuple CheckApply (const Int & i,
                    const TYPE_REP_TypeRep & fcttp,
                    const SEQ<TYPE_REP_TypeRep> & argtp_l,
                    const Record &,
                    const Bool &);
  Tuple wf_FieldSelectExpr (const Int & i, const TYPE_AS_FieldSelectExpr & vFieldSelect, const TYPE_REP_TypeRep &);
  Tuple IsAValidField (const TYPE_AS_Name & fnm, const TYPE_REP_CompositeTypeRep & tp);
  bool CheckQualifiedField (const Int & i, const Record & field, bool objRefComp, const TYPE_REP_TypeRep & objtp);
  Tuple CheckFieldSelect (const Int & i, const TYPE_REP_TypeRep & rectp, const Record & field, bool writable);
  Tuple CheckRecordFieldSelect (const Int & i, const TYPE_REP_TypeRep & rectp, const Record & field);
#ifdef VDMPP
  Tuple CheckObjRefFieldSelect (const Int & i, const TYPE_REP_TypeRep & objtp, const Record & field, bool writable);
private:
  Tuple MergeOpTypes(const TYPE_REP_TypeRep & p_optp, const Generic& p_nmtp, const Generic& p_restp);
#endif //VDMPP
  Tuple wf_IsExpr (const Int & i, const Record & vIsExpr, const TYPE_REP_TypeRep &);
  Tuple wf_Name (const Int & i, const TYPE_AS_Name& nm);
  Tuple wf_OldName (const Int & i, const TYPE_AS_OldName & onm);
  Tuple wf_Literal (const Int & i, const TYPE_AS_Literal & lit, const TYPE_REP_TypeRep &);
  Tuple wf_NarrowExpr (const Int & i, const TYPE_AS_NarrowExpr & vNarrowExpr, const TYPE_REP_TypeRep &);

#ifdef VDMPP
  Tuple wf_SelfExpr (const Int & i, const TYPE_AS_SelfExpr & selexp, const TYPE_REP_TypeRep &);
#ifdef VICE
  bool CpuOrBusOutSideSys(const TYPE_AS_Name & nm);
#endif // VICE
  Tuple wf_ThreadIdExpr (const Int &, const TYPE_AS_ThreadIdExpr & threadidexp, const TYPE_REP_TypeRep &);
  Tuple wf_NewExpr (const Int & i, const TYPE_AS_NewExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_SameClassExpr (const Int & i, const Record & sameexp, const TYPE_REP_TypeRep &);
  Tuple wf_IsOfClassExpr (const Int & i, const Record & isofexp, const TYPE_REP_TypeRep &);
  Tuple wf_NameList (const SEQ<TYPE_AS_Name> &);
  Tuple wf_ActExpr (const Int &, const TYPE_AS_ActExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_FinExpr (const Int &, const TYPE_AS_FinExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_ActiveExpr (const Int &, const TYPE_AS_ActiveExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_WaitingExpr (const Int &, const TYPE_AS_WaitingExpr &, const TYPE_REP_TypeRep &);
  Tuple wf_ReqExpr (const Int &, const TYPE_AS_ReqExpr &, const TYPE_REP_TypeRep &);
#ifdef VICE
  Tuple wf_CurTimeExpr (const Int &, const TYPE_AS_CurTimeExpr & ctimeexp, const TYPE_REP_TypeRep &);
#endif // VICE
#endif //VDMPP

  /** PAT module **************************************************************/
  /** Reference : Doc. afro-ifad-pgl-statsem-v1.0 *****************************/
  /****************************************************************************/

  Tuple wf_PatternBind (const Int & i, const TYPE_AS_PatternBind & patbind, const TYPE_REP_TypeRep & tp);
  Tuple wf_Pattern (const Int & i, const TYPE_AS_Pattern & pat, const TYPE_REP_TypeRep & tp);
  Tuple wf_PatternName (const Int & i, const TYPE_AS_PatternName & pat, const TYPE_REP_TypeRep & tp);
  Tuple wf_MatchVal (const Int & i, const TYPE_AS_MatchVal & pat, const TYPE_REP_TypeRep & tp);
  Tuple wf_SetEnumPattern (const Int & i, const TYPE_AS_SetEnumPattern & pat, const TYPE_REP_TypeRep & tp);
  Tuple wf_SetUnionPattern (const Int & i, const TYPE_AS_SetUnionPattern & pat, const TYPE_REP_TypeRep & tp);
  Tuple wf_SeqEnumPattern (const Int & i, const TYPE_AS_SeqEnumPattern & pat, const TYPE_REP_TypeRep & tp);
  Tuple wf_SeqConcPattern (const Int & i, const TYPE_AS_SeqConcPattern & pat, const TYPE_REP_TypeRep & tp);
  Tuple wf_MapEnumPattern (const Int & i, const TYPE_AS_MapEnumPattern & pat, const TYPE_REP_TypeRep & tp);
  Tuple wf_MapMergePattern (const Int & i, const TYPE_AS_MapMergePattern & pat, const TYPE_REP_TypeRep & tp);
  Tuple wf_RecordPattern (const Int & i, const TYPE_AS_RecordPattern & pat, const TYPE_REP_TypeRep & tp);
  Tuple wf_TuplePattern (const Int & i, const TYPE_AS_TuplePattern & pat, const TYPE_REP_TypeRep & tp);
  Tuple wf_MapletPattern (const Int & i, const TYPE_AS_MapletPattern & pat, const TYPE_REP_TypeRep & tp);
#ifdef VDMPP
  Tuple wf_ObjectPattern (const Int & i, const TYPE_AS_ObjectPattern & pat, const TYPE_REP_TypeRep & tp);
#endif // VDMPP
  Tuple wf_PatternList (const Int & i,
                        const SEQ<TYPE_AS_Pattern> & p_l,
                        const SEQ<TYPE_REP_TypeRep> & tp_l,
                        const TYPE_AS_Name &);
  Tuple wf_Bind (const Int & i, const TYPE_AS_Bind & bind);
  Tuple wf_SetBind (const Int & i, const TYPE_AS_SetBind & bind);
  Tuple wf_TypeBind (const Int & i, const TYPE_AS_TypeBind & bind);
  Tuple wf_SeqBind (const Int & i, const TYPE_AS_SeqBind & bind);
  Tuple wf_BindAndType (const Int & i, const TYPE_AS_Bind & bind);
  Tuple wf_TypeBindList (const Int & i, const SEQ<TYPE_AS_TypeBind> & parms);
  Tuple wf_MultiBindList (const Int & i, const SEQ<TYPE_AS_MultBind> & mbs, bool mustmatch);
  Tuple wf_MultiSetBind (const Int & i, const TYPE_AS_MultSetBind & msb, bool mustmatch);
  Tuple wf_MultiTypeBind (const Int & i, const TYPE_AS_MultTypeBind & msb);
  Tuple MergeBindings (const Int &, const Sequence & s);
public:
  MAP<TYPE_AS_Name, Int> ExtractPatternName (const TYPE_AS_Pattern & pat);
private:
  MAP<TYPE_AS_Name, Int> MergePatternName (const SEQ<Map> & ptml);
  MAP<TYPE_AS_Name,Tuple> ExtractBindings (const TYPE_AS_Pattern & pat);
  bool SimplePatterns (const TYPE_AS_Pattern & pat);
  TYPE_REP_TypeRep GetRealType (const TYPE_REP_TypeRep & tp);
  TYPE_REP_TypeRep PatternBind2TypeRep (const TYPE_AS_PatternBind & patbind);
  TYPE_REP_TypeRep Pattern2TypeRep (const TYPE_AS_Pattern & pat);


  /** REP module **************************************************************/
  /** Reference : Doc. afro-ifad-pgl-statsem-v1.0 *****************************/
  /****************************************************************************/

  /** STMT module *************************************************************/
  /** Reference : Doc. afro-ifad-pgl-statsem-v1.0 *****************************/
  /****************************************************************************/

  public:
  Tuple CheckOperationCall (const Int &,
                            const SET<TYPE_AS_Name> &,
                            const TYPE_AS_Name&,
                            const Generic &,
                            const SEQ<TYPE_REP_TypeRep> &);
  private:
  TYPE_REP_TypeRep ExtractExpectedSDArgType (const TYPE_REP_TypeRep &);

  Tuple wf_CallStmt (const Int & i, const TYPE_AS_CallStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_ErrorStmt (const Int & i, const TYPE_AS_ErrorStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_AlwaysStmt (const Int & i, const TYPE_AS_AlwaysStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_ExitStmt (const Int & i, const TYPE_AS_ExitStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_TrapStmt (const Int & i, const TYPE_AS_TrapStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_RecTrapStmt (const Int & i, const TYPE_AS_RecTrapStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_TrapAltn (const Int &,
                     const TYPE_AS_PatternBind &,
                     const TYPE_AS_Stmt &,
                     const TYPE_REP_TypeRep &,
                     const TYPE_REP_TypeRep &);
  void SetOpContext (const TYPE_AS_Name &);
  bool AnyTraps () const;
  void FullOpTest (bool);
  void InsertExitType (const TYPE_REP_TypeRep &);
  void InsertTrapId (const TYPE_AS_Stmt &);
  void LeaveTrapId (const TYPE_AS_Stmt &);
  void InsertOpCall (const TYPE_AS_Name &);
  Generic ExtractRealExitTypeRep(const TYPE_REP_TypeRep &, const TYPE_AS_Stmt &);
  Generic TransClosExit(const SET<TYPE_AS_Name> &, const SET<TYPE_AS_Name> &);
  Tuple wf_NonDetStmt (const Int & i, const TYPE_AS_NonDetStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_SpecificationStmt (const Int & i, const TYPE_AS_SpecificationStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_AssertStmt (const Int & i, const TYPE_AS_AssertStmt & stmt, const TYPE_REP_TypeRep &);
  Bool  wf_ImplOpBody(const Int &,
                      const SEQ<TYPE_AS_ExtInf> &,
                      const Generic &,
                      const TYPE_AS_Expr &,
                      const SEQ<TYPE_AS_Error> &,
                      const MAP<TYPE_AS_Name,Tuple> &);
#ifdef VDMPP
  Tuple wf_StartStmt (const Int & i, const TYPE_AS_StartStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_StartListStmt (const Int & i, const TYPE_AS_StartListStmt & stmt, const TYPE_REP_TypeRep &);
  bool CheckThread(const Int & i, const SET<TYPE_AS_Name> & classes);
  Tuple wf_StopStmt (const Int & i, const TYPE_AS_StopStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_StopListStmt (const Int & i, const TYPE_AS_StopListStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple CheckObjectRef (const Int &, const TYPE_AS_Expr &);
#endif //VDMPP

#ifdef VICE
  Tuple wf_DurationStmt(const Int & i, const TYPE_AS_DurationStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_CycleStmt(const Int & i, const TYPE_AS_CycleStmt & stmt, const TYPE_REP_TypeRep &);
#endif //VICE

  Tuple wf_Stmt(const Int &, const TYPE_AS_Stmt &, const TYPE_REP_TypeRep &);
  Tuple wf_DefStmt (const Int & i, const TYPE_AS_DefStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_LetStmt (const Int & i, const TYPE_AS_LetStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_LetBeSTStmt (const Int & i, const TYPE_AS_LetBeSTStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_BlockStmt (const Int & i, const TYPE_AS_BlockStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_Dcl (const Int & i, const TYPE_AS_AssignDef & dcl);
  Tuple wf_AssignStmt (const Int & i, const TYPE_AS_AssignStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_AtomicAssignStmt (const Int &, const TYPE_AS_AtomicAssignStmt &, const TYPE_REP_TypeRep &);
  Tuple wf_StateDesignator (const Int & i, const TYPE_AS_StateDesignator & stmt, const Bool &);
  Tuple wf_IfStmt (const Int & i, const TYPE_AS_IfStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_ElseIfStmts (const Int & i, const SEQ<TYPE_AS_ElseifStmt> & else_l, const TYPE_REP_TypeRep &);
  Tuple wf_ElseIfStmt (const Int & i, const TYPE_AS_ElseifStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_CasesStmt (const Int & i, const TYPE_AS_CasesStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_CasesStmtAltn (const Int &,
                          const SEQ<TYPE_AS_Pattern> &,
                          const TYPE_AS_Stmt &,
                          const TYPE_REP_TypeRep & tp,
                          const TYPE_REP_TypeRep &);
  Tuple wf_SeqForLoopStmt (const Int & i, const TYPE_AS_SeqForLoopStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_SetForLoopStmt (const Int & i, const TYPE_AS_SetForLoopStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_IndexForLoopStmt (const Int & i, const TYPE_AS_IndexForLoopStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_WhileLoopStmt (const Int & i, const TYPE_AS_WhileLoopStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_ReturnStmt (const Int & i, const TYPE_AS_ReturnStmt & stmt, const TYPE_REP_TypeRep &);
  Tuple wf_IdentStmt (const Int & i, const TYPE_AS_IdentStmt & stmt, const TYPE_REP_TypeRep &);

  /** TEST module *************************************************************/
  /** Reference : Doc. afro-ifad-pgl-statsem-v1.0 *****************************/
  /****************************************************************************/

  /** TYPE module *************************************************************/
  /** Reference : Doc. afro-ifad-pgl-statsem-v1.0 *****************************/
  /****************************************************************************/

  bool AuxIsUnionRec (const TYPE_REP_TypeRep &, const SET<TYPE_REP_TypeRep> &);
  bool IsUnionRecursive (const TYPE_REP_TypeRep &);
  bool IsCompatible (const Int &, const TYPE_REP_TypeRep &, const TYPE_REP_TypeRep &);
  bool IsEmpty (const Int &, const TYPE_REP_TypeRep &);
  bool IsEmptyAux (const Int &, const TYPE_REP_TypeRep &, const SET<TYPE_REP_TypeRep> &);
  Int NegWfClass (const Int &) const;
  void MeasureCheckOn() { this->measureCheck = true; };
  void MeasureCheckOff() { this->measureCheck = false; };
  bool MeasureCheck() { return this->measureCheck; };
public:
  bool IsSubType (const TYPE_REP_TypeRep &, const TYPE_REP_TypeRep &, const Set &);
private:
  bool IsOverlapping (const TYPE_REP_TypeRep &, const TYPE_REP_TypeRep &, const Set &);
  bool IsEquivalent (const Int &, const TYPE_REP_TypeRep &, const TYPE_REP_TypeRep &);
  bool IsOneValueType (const Int &, const TYPE_REP_TypeRep &);
  bool IsOneValueTypeAux (const Int &, const TYPE_REP_TypeRep &, const SET<TYPE_REP_TypeRep> &);

  /** VCM module **************************************************************/
  /** Reference : Doc. afro-ifad-pgl-statsem-v1.0 *****************************/
  /****************************************************************************/

#ifdef VDMPP
  void UpdateParseEnv (const TYPE_AS_Name &, const TYPE_SSENV_ParseTypeInfo &);
  void UpdateValuesInParseEnv(const TYPE_AS_Name & nm, const MAP<TYPE_AS_Name, TYPE_REP_TypeRep> & vals);

  TYPE_AS_Access SetDefAccess (const TYPE_AS_Access & acs, const TYPE_AS_Access & df) const;
  Tuple ExportTypes (const TYPE_AS_Name & cls, const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & typem);
  Tuple ExportInstVars (const TYPE_AS_Name &, const SEQ<TYPE_AS_InstanceVarDef> &);
  int ExportThread(const Generic & thread);
  Map ExportOverloads(const TYPE_AS_Name & p_nm,
                      const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & p_opm,
                      const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & p_fnm);
  Map MakeOverloadedMap(const TYPE_AS_Name & p_nm, const MAP<TYPE_AS_Name, Record> & p_methmp, bool p_isop);
  Map ExportSubresps(const TYPE_AS_Name & p_nm,
                     const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & p_fnm,
                     const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & p_pfm,
                     const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & p_opm);
#endif //VDMPP

  TYPE_AS_Name SetName (const Generic &, const TYPE_AS_Name &);
  Record MakeTpRng (const TYPE_REP_TypeRep & t, const TYPE_AS_Access &, const Bool &);
  Record MakeFnRng (const TYPE_REP_FnTypeRep & f, const TYPE_AS_Access &, const Bool &);
  Record MakeOpRng (const TYPE_REP_OpTypeRep & o, const TYPE_AS_Access &, const Bool &, const Bool &);
  Record MakePolyRng (const TYPE_REP_PolyTypeRep & p, const TYPE_AS_Access &, const Bool &);
  Map ExportValues (const Generic & modid, const SEQ<TYPE_AS_ValueDef> & valuem);
  Map ExportFunctions (const Generic & modid, const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & fnm);
  Map ExportOperations (const Generic & modid, const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & opm);
#ifdef VDMPP
  SET<TYPE_SSENV_AccessOpTypeRep> ExportConstructors(const Generic & modid,
                                                     const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & p_opm);
#endif //VDMPP
  Map ExportInvs (const Generic & modid, const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & typem);
  Map ExportPrePostFns (const Generic & modid, const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & fnm);
  Map ExportPolyFns (const Generic & modid, const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & fnm);
  Map ColTags (const Generic & modid, const TYPE_AS_Type & tp) const;
  TYPE_REP_FnTypeRep ExportExplicitFunction (const Generic & modid, const TYPE_AS_ExplFnDef & efd) const;
  TYPE_REP_FnTypeRep ExportImplicitFunction (const Generic & modid, const TYPE_AS_ImplFnDef & ifd) const;
  TYPE_REP_FnTypeRep ExportExtExplicitFunction (const Generic & modid, const TYPE_AS_ExtExplFnDef & efd) const;
  TYPE_REP_OpTypeRep ExportExplicitOperation (const Generic & modid, const TYPE_AS_ExplOpDef & efd) const;
  TYPE_REP_OpTypeRep ExportImplicitOperation (const Generic & modid, const TYPE_AS_ImplOpDef & ifd) const;
  TYPE_REP_OpTypeRep ExportExtExplicitOperation (const Generic & modid, const TYPE_AS_ExtExplOpDef & efd) const;
  SEQ<TYPE_AS_TypeVar> FindTypeVars (const TYPE_AS_FnDef & fndef) const;

#ifdef VDMSL
public:
  Generic GetFuncDefWOCtxt(const TYPE_AS_Name & nm);
private:
  Generic GetModuleTypeRepWOCtxt(const TYPE_AS_Name & nm);
  Tuple ExportTypes (const Generic & modid, const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & typem, const Generic & state);
  Tuple GetModuleRep (const TYPE_AS_Name & nm) const;
  void ExportSome (const TYPE_AS_Name & modid,
                   const TYPE_AS_ExportSig & expsig,
                   const TYPE_AS_Definitions & defs,
                   const MAP<TYPE_AS_Name, TYPE_AS_Name> & bind);
  TYPE_SSENV_ExpSigRep UseRenamedTypes (const TYPE_SSENV_ExpSigRep & exp, const MAP<TYPE_AS_Name, TYPE_AS_Name> & bind);
  Map FindRenamedTypes (const TYPE_AS_Name & modid, const MAP<TYPE_AS_Name, TYPE_AS_Name> & imp_m);
  void ExportAll (const TYPE_AS_Name & modid,
                  const TYPE_AS_Definitions & defs,
                  const MAP<TYPE_AS_Name, TYPE_AS_Name> & bind);
  Map ExportPrePostOps (const Generic & modid, const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & opm, const Generic & state);
  Tuple ExportSomeTypes (const Generic & modid,
                         const MAP<TYPE_AS_Name, Generic> & tps,
                         const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & typem,
                         const Generic &);
  Map ExportSimple (const Generic & modid, const MAP<TYPE_AS_Name, TYPE_AS_Type> & simple);
#endif //VDMSL

  /** mk aux functions ********************************************************/
  /** Reference : None ********************************************************/
  /****************************************************************************/

  static TYPE_REP_UnionTypeRep mk_SetOfAllType();
  static TYPE_REP_UnionTypeRep mk_SeqOfAllType ();
  static TYPE_REP_UnionTypeRep mk_MapOfAllType ();

  /** gen_parse_info **********************************************************/
  /** Reference : None ********************************************************/
  /****************************************************************************/


  /** misc             ********************************************************/
  wstring WhileChecking;
  MAP<TYPE_REP_TypeRep, Bool> UnionRecursiveResults;

public:

  void InitParseEnv();
#ifdef VDMPP
  bool wf_Class(const Int & i, const TYPE_AS_Class & cls);
  bool ExpandClass (const Int &, const TYPE_AS_Class &);
  bool IsClassName(const TYPE_AS_Name &) const;
  SET<TYPE_AS_Name> GetTypes() const;
  bool UpdateParseEnvWithAST(const TYPE_AS_Document & ast_l);
  Generic GetFuncDefWOCtxt(const TYPE_AS_Name & nm);
  Generic SearchFuncInSuper(const TYPE_AS_Name & nm, const SET<TYPE_AS_Name> & sn);
  Map GetParseEnv();
  Generic GetClassTypeRepWOCtxt(const TYPE_AS_Name & nm);
  Generic GetClassTypeRep (const TYPE_AS_Name &) const;
  Set LookUpOperationName (const Generic & clsnm, const TYPE_AS_Name& nm, const Generic & objnm);
  Generic LookUpOpOrFnName (const Generic & clsnm, const TYPE_AS_Name& nm);
  Tuple ExpandClassName(const TYPE_AS_Name & name, const SET<TYPE_AS_Name> & nm_s);
  void RemoveClassFromParseEnv(const SET<TYPE_AS_Name> & cls_s);
#ifdef VICE
  bool IsSysClass(const TYPE_AS_Name &) const;
  TYPE_SSENV_ParseTypeInfo CPUInfo();
  TYPE_SSENV_ParseTypeInfo BUSInfo();
  Map CPUPriorityOperation();
  Map CPUDeployOperation();
  Set CPUConstructor();
  Set BUSConstructor();
#endif // VICE
#endif //VDMPP
  Tuple FindConstruct (const Record & nm, const Generic & defg);
  int AnyHandlers (const Record &);
  int AnyHandlersAux (const Record &);

#ifdef VDMSL
  bool ExpandDLModule (const Int & i, const TYPE_AS_DLModule & imdo);
  bool ExpandDLImport (const TYPE_AS_Name & fromid, const TYPE_AS_DLImportSig & imp, const TYPE_AS_Name & curmod);
  bool ExpandModule (const Int &, const Record &);
  bool wf_Module (const Int & i, const Record & mod);
  MAP<TYPE_AS_Name, TYPE_SSENV_ExpSigRep> GetModuleEnv () const;
  void SetModuleEnv (const MAP<TYPE_AS_Name, TYPE_SSENV_ExpSigRep> & _ModuleEnv);
  void UpdateModuleEnv (const Record & mod);
  void UpdateStdModuleEnv (const TYPE_AS_Module & mod);
  void UpdateDLModuleEnv (const TYPE_AS_DLModule & mod);
  TYPE_SSENV_ExpSigRep ExportDLModule (const TYPE_AS_Name & modid, const TYPE_AS_DLExportSig & iexp) const;
#endif //VDMSL

#ifdef VDMSL
  Generic PublicLookUpTypeName (const TYPE_AS_Name&, const TYPE_AS_Name&);
#endif // VDMSL
#ifdef VDMPP
  Tuple PublicLookUpTypeName (const TYPE_AS_Name&, const TYPE_AS_Name&);
#endif // VDMPP
  Tuple GetTCState ();
  void SetTCState (const Tuple & st);

#ifdef VDMPP
  Generic LookUpDefClassForTypeName(const TYPE_AS_Name & nm);
  Generic LookUpDefClass ();
  void SetDefClass(const TYPE_AS_Name &);
  void UnSetDefClass();
//  int NO_OF_WARNINGS, NO_OF_ERRORS;
#endif // VDMPP

  Tuple TestFunc (const Record & seqenum, const Int & ind);
  Bool TestStmt (const Record & cls, const Record & see, const Int & ind);
  void  InitEnv ();
  Generic LookUpStateEnv(const TYPE_AS_Name & nm);

  StatSem () {
//    generating_parse_info = 0;

    this->WhereAmI.ImpPrepend(Int(AS_PURE));
#ifdef VDMSL
    this->InsideInter = false;
#endif // VDMSL
#ifdef VDMPP
    this->isStatic = Bool(false);
#endif // VDMPP
    this->fullcheck = false;
    this->traps = false;

    this->lastline = 0;
    this->lastcol  = 0;
    this->lastfile = 0;

    this->exp_defined = false;
    //
    this->intTypes.Insert((Int)NATONE);
    this->intTypes.Insert((Int)NAT);
    this->intTypes.Insert((Int)INTEGER);

    this->natSubTypeSet.Insert(TYPE_REP_NumericTypeRep().Init((Int)NATONE));
    this->intSubTypeSet.Insert(TYPE_REP_NumericTypeRep().Init((Int)NAT)).ImpUnion(this->natSubTypeSet);
    this->ratSubTypeSet.Insert(TYPE_REP_NumericTypeRep().Init((Int)INTEGER)).ImpUnion(this->intSubTypeSet);
    this->realSubTypeSet.Insert(TYPE_REP_NumericTypeRep().Init((Int)RAT)).ImpUnion(this->ratSubTypeSet);
    this->measureCheck = false;
  }

  ~StatSem () {}

  /** type consts      ********************************************************/
  /** Reference : None ********************************************************/
  /****************************************************************************/

  static const Record btp_bool;
  static const Record btp_int;
  static const Record btp_real;
  static const Record btp_char;
  static const Record btp_nat;
  static const Record btp_natone;
  static const Record btp_rat;
  static const Record btp_token;

  static const Record rep_alltp;
  static const Record rep_tmptp;
  static const Record rep_rettp;
  static const Record rep_unittp;
  static const Record rep_niltp;

//  static const Record rep_emptyset;
//  static const Record rep_emptyseq;
//  static const Record rep_emptymap;

  static const Record set_alltp;
  static const Record seq_alltp;
  static const Record map_alltp;

  friend class vdmcg;
#endif //! NOSS
};

StatSem & GetStatSem();

#endif // __statsem_h__
