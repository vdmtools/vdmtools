/***
*  * WHAT
*  *    creation of semantic values.
*  * FILE
*  *    $Id: semvalues.cc,v 1.31 2006/09/22 11:14:06 vdmtools Exp $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "semvalues.h"
#include "astaux.h"
#include "contextinfo.h"
#include "asquotes.h"
#ifdef VDMPP
#include "libman.h"
#ifdef VICE
#include "m4lib_errmsg.h"
#endif // VICE
#endif // VDMPP
#include "rt_errmsg.h"

TYPE_SEM_BOOL sem_true (TYPE_SEM_BOOL().Init(Bool(true)));
TYPE_SEM_BOOL sem_false (TYPE_SEM_BOOL().Init(Bool(false)));
TYPE_SEM_NIL sem_nil;
TYPE_SEM_UNDEF sem_undef;
TYPE_SEM_Permission sem_read_only = Int(READ_ONLY);
TYPE_SEM_Permission sem_read_write = Int(READ_WRITE);
TYPE_SEM_CONT sem_cont;

MAP<Char, TYPE_SEM_CHAR> SemRec::charMap;
MAP<SEQ<Char>, TYPE_SEM_QUOTE> SemRec::quoteMap;


//----------------------------------------------------------------------
// Definitions from eval_sem.vdm
//----------------------------------------------------------------------
// CompFN
// efn : ExplFN
// +> CompExplFN
TYPE_SEM_CompExplFN SemRec::CompFN(const TYPE_SEM_ExplFN & efn)
{
  return TYPE_SEM_CompExplFN().Init(SEQ<TYPE_SEM_ExplFN>().ImpAppend(efn), Nil());
}

// UpdateClosEnv
// fnval : VAL
// blkenv : BlkEnv
// +> VAL
TYPE_SEM_VAL SemRec::UpdateClosEnv(const TYPE_SEM_VAL & fnval, const TYPE_SEM_BlkEnv & blkenv)
{
  switch(fnval.GetTag()) {
    case TAG_TYPE_SEM_CompExplFN: {
      const SEQ<TYPE_SEM_ExplFN> & fl (fnval.GetSequence(pos_SEM_CompExplFN_fl));
      SEQ<TYPE_SEM_ExplFN> res_seq;

      size_t len = fl.Length();
      for (size_t i = 1; i <= len; i++) {
        TYPE_SEM_ExplFN efn (fl[i]);
        efn.SetField(pos_SEM_ExplFN_env, blkenv);
        res_seq.ImpAppend(efn);
      }
      return TYPE_SEM_CompExplFN().Init(res_seq, Nil());
    }
    case TAG_TYPE_SEM_ExplPOLY: {
      TYPE_SEM_ExplPOLY pfn (fnval);
      pfn.SetField(pos_SEM_ExplPOLY_env, blkenv);
      return pfn;
    }
    default: {
      return fnval;
    }
  }
}

//
// Semantic Record
//
/*
SemRecTable semrectbl;         // Semantic records.
*/

VDMRecInfoMap SemRec::DefSemRecInfo;
int SemRec::next_tag = 1;
TYPE_AS_Name SemRec::currentmodcls; // Current Module or Class Name.

void SemRec::ResetSemRecTable()
{
  DefSemRecInfo.clear();
  next_tag = 1;
}

TYPE_DYNSEM_SEM_SemRecord SemRec::mk_SEM_REC(const TYPE_AS_Name & astag, const SEQ<TYPE_SEM_VAL> & v_l)
{
  Record inner_rec (ASTAUX::ASName2String(astag), DefSemRecInfo);
  inner_rec.SetFields(v_l);
  return TYPE_DYNSEM_SEM_SemRecord().Init(astag, inner_rec, Bool(false));
}

void SemRec::Decl_SEM_REC(const TYPE_AS_Name & astag, int size, const SEQ<Bool> & dc_s)
{
  //TYPE_AS_Name qnm (AUX::ConstructDoubleName(currentmodcls, astag));
  TYPE_AS_Name qnm (astag.GetSequence(pos_AS_Name_ids).Length() == 1 
                      ? ASTAUX::Combine2Names(currentmodcls, astag)
                      : astag);

  int tag = next_tag;
  next_tag++;

  vector<int> dc;
  size_t len = dc_s.Length();
  for (size_t i = 1; i <= len; i++) {
    if (dc_s[i].GetValue()) {
      dc.push_back(i);
    }
  }
  DefSemRecInfo.NewTag(tag, size, ASTAUX::ASName2String(qnm), dc);
}

bool SemRec::IsDecl(const TYPE_AS_Name & astag)
{
  return DefSemRecInfo.IsDecl(ASTAUX::ASName2String(astag));
}

void SemRec::SetCurrentModClass(const TYPE_AS_Name & name)
{
  currentmodcls = name;
}

// IsSemVal
// val : STKM`EvalStackItem
// -> bool
bool SemRec::IsSemVal(const Generic & val)
{
  if(!val.IsRecord()) {
    return false;
  }

  switch(((Record)val).GetTag()) {
    case TAG_TYPE_SEM_EQORD:
    case TAG_TYPE_SEM_BOOL:
    case TAG_TYPE_SEM_NUM:
    case TAG_TYPE_SEM_CHAR:
    case TAG_TYPE_SEM_QUOTE:
    case TAG_TYPE_SEM_NIL:
    case TAG_TYPE_SEM_CompExplFN:
    case TAG_TYPE_SEM_ExplOP:
    case TAG_TYPE_SEM_ExplPOLY:
    case TAG_TYPE_SEM_SEQ:
    case TAG_TYPE_SEM_SET:
    case TAG_TYPE_SEM_MAP:
    case TAG_TYPE_SEM_TUPLE:
    case TAG_TYPE_DYNSEM_SEM_REC:
    case TAG_TYPE_SEM_TOKEN:
    case TAG_TYPE_SEM_UNDEF:
    case TAG_TYPE_SEM_EXIT:
    case TAG_TYPE_SEM_CONT:
    case TAG_TYPE_SEM_RETURN:
#ifdef VDMPP
    case TAG_TYPE_SEM_OverOPFN:
    case TAG_TYPE_SEM_OBJ:
    case TAG_TYPE_SEM_OBJ_uRef:
#endif // VDMPP
      return true;
    default: {
      return false;
    }
  }
}

// VAL2Expr
// val : VAL
// -> AS`Expr
TYPE_AS_Expr SemRec::VAL2Expr(const TYPE_SEM_VAL & val)
{
  switch(val.GetTag()) {
    case TAG_TYPE_SEM_EQORD: {
      return VAL2Expr(val.GetRecord(pos_SEM_EQORD_v));
    }
    case TAG_TYPE_SEM_BOOL: {
      return TYPE_AS_BoolLit().Init(val.GetBool(pos_SEM_BOOL_v), NilContextId);
    }
    case TAG_TYPE_SEM_NUM: {
      return TYPE_AS_RealLit().Init(val.GetReal(pos_SEM_NUM_v), NilContextId);
    }
    case TAG_TYPE_SEM_CHAR: {
      return TYPE_AS_CharLit().Init(val.GetChar(pos_SEM_CHAR_v), NilContextId);
    }
    case TAG_TYPE_SEM_QUOTE: {
      return TYPE_AS_QuoteLit().Init(val.GetSequence(pos_SEM_QUOTE_v), NilContextId);
    }
    case TAG_TYPE_SEM_NIL: {
      return TYPE_AS_NilLit().Init(NilContextId);
    }
    case TAG_TYPE_SEM_SET: {
      return TYPE_AS_SetEnumerationExpr().Init(VALSet2Expr(val.GetSet(pos_SEM_SET_v)), NilContextId);
    }
    case TAG_TYPE_SEM_SEQ: {
      return TYPE_AS_SeqEnumerationExpr().Init(VALSeq2Expr(val.GetSequence(pos_SEM_SEQ_v)), NilContextId);
    }
    case TAG_TYPE_SEM_MAP: {
      return TYPE_AS_MapEnumerationExpr().Init(VALMap2Expr(val.GetMap(pos_SEM_MAP_v)), NilContextId);
    }
    case TAG_TYPE_SEM_TUPLE: {
      return TYPE_AS_TupleConstructorExpr().Init(VALSeq2Expr(val.GetSequence(pos_SEM_TUPLE_v)), NilContextId);
    }
    case TAG_TYPE_DYNSEM_SEM_REC: {
      return TYPE_AS_RecordConstructorExpr().Init(
                       val.GetRecord(pos_DYNSEM_SEM_SemRecord_tag),
                       VALSeq2Expr(val.GetRecord(pos_DYNSEM_SEM_SemRecord_value).GetFields()),
                       NilContextId);
    }
    case TAG_TYPE_SEM_TOKEN: {
      return TYPE_AS_TokenConstructorExpr().Init(VAL2Expr(val.GetRecord(pos_SEM_TOKEN_v)), NilContextId);
    }
#ifdef VDMPP
    case TAG_TYPE_SEM_OBJ_uRef: {
      // TODO
      const TYPE_AS_Name & clnm (val.GetRecord(pos_SEM_OBJ_uRef_tp));
//      return TYPE_AS_NewExpr().Init(clnm, SEQ<TYPE_AS_Expr>(), NilContextId);
//      SEQ<TYPE_SEM_VAL> newargs (theState().GetNewArgs(val));
//      SEQ<TYPE_AS_Expr> exprs;
//      size_t len_newargs = newargs.Length();
//      for (size_t idx = 1; idx <= len_newargs; idx++)
//        exprs.ImpAppend(VAL2Expr(newargs[idx]));
//      return TYPE_AS_NewExpr().Init(clnm, exprs, NilContextId);
      Generic nm (theState().GetBindName(val));
      if (!nm.IsNil()) {
        return nm;
      }
      return TYPE_AS_NewExpr().Init(clnm, theState().GetNewArgs(val), NilContextId);
    }
#endif // VDMPP
    case TAG_TYPE_SEM_CONT:
    case TAG_TYPE_SEM_EXIT:
    default: {
      RTERR::Error(L"VAL2Expr", RTERR_TAG_UNKNOWN, Nil(), Nil(), Sequence());
      return TYPE_AS_UndefinedExpr(); // dummy
    }
  }
}

// VALSet2Expr
// val_s : set of VAL
// -> seq of AS`Expr
SEQ<TYPE_AS_Expr> SemRec::VALSet2Expr(const SET<TYPE_SEM_VAL> & val_s)
{
  switch(val_s.Card()) {
    case 0: {
      return SEQ<TYPE_AS_Expr>();
    }
    case 1: {
      return SEQ<TYPE_AS_Expr>().ImpAppend(VAL2Expr(val_s.GetElem()));
    }
    default: {
      SET<TYPE_SEM_VAL> new_val_s (val_s);
      TYPE_SEM_VAL val (val_s.GetElem());
      new_val_s.RemElem(val);
      return SEQ<TYPE_AS_Expr>().ImpAppend(VAL2Expr(val)).ImpConc(VALSet2Expr(new_val_s));
    }
  }
}

// VALSeq2Expr
// val_l : seq of VAL
// -> seq of AS`Expr
SEQ<TYPE_AS_Expr> SemRec::VALSeq2Expr(const SEQ<TYPE_SEM_VAL> & val_l)
{
  switch(val_l.Length()) {
    case 0: {
      return SEQ<TYPE_AS_Expr>();
    }
    case 1: {
      return SEQ<TYPE_AS_Expr>().ImpAppend(VAL2Expr(val_l.Hd()));
    }
    default: {
      SEQ<TYPE_SEM_VAL> new_val_l (val_l);
      return SEQ<TYPE_AS_Expr>().ImpAppend(VAL2Expr(val_l.Hd())).ImpConc(VALSeq2Expr(new_val_l.ImpTl()));
    }
  }
}

// VALMap2Expr
// val_m : map VAL to VAL
// -> seq of AS`Maplet
SEQ<TYPE_AS_Maplet> SemRec::VALMap2Expr(const MAP<TYPE_SEM_VAL,TYPE_SEM_VAL> & val_m)
{
  Set dom_val_m (val_m.Dom());
  switch(dom_val_m.Card()) {
    case 0: {
      return SEQ<TYPE_AS_Maplet>();
    }
    case 1: {
      TYPE_SEM_VAL v (dom_val_m.GetElem());
      return SEQ<TYPE_AS_Maplet>().ImpAppend(TYPE_AS_Maplet().Init(VAL2Expr(v), VAL2Expr(val_m[v]), NilContextId));
    }
    default: {
      TYPE_SEM_VAL v (dom_val_m.GetElem());
      MAP<TYPE_SEM_VAL,TYPE_SEM_VAL> new_val_m (val_m);
      new_val_m.RemElem(v);
      return SEQ<TYPE_AS_Maplet>().ImpAppend(TYPE_AS_Maplet().Init(VAL2Expr(v), VAL2Expr(val_m[v]), NilContextId))
                                  .ImpConc(VALMap2Expr(new_val_m));
    }
  }
}

// CreateSemRecordUndefinedFields
TYPE_DYNSEM_SEM_SemRecord SemRec::CreateSemRecordUndefinedFields(
                            const TYPE_AS_CompositeType & tp,
                            const TYPE_AS_Name & tag)
{
  size_t size = tp.GetSequence(pos_AS_CompositeType_fields).Length();
  SEQ<TYPE_SEM_VAL> pval_l;
  TYPE_SEM_UNDEF undef;
  for (size_t i = 1; i <= size; i++) {
    pval_l.ImpAppend (undef);
  }
  return mk_SEM_REC(tag, pval_l);
}

// SemRecord2REC
TYPE_SEM_REC SemRec::SemRecord2REC(const TYPE_DYNSEM_SEM_SemRecord & srec)
{
  const Record & value (srec.GetRecord(pos_DYNSEM_SEM_SemRecord_value));
  SEQ<TYPE_SEM_VAL> seq (value.GetFields());
  size_t len = seq.Length();
  MAP<Int,TYPE_SEM_VAL> v, v_dc;
  Generic g;
  for(size_t i = 1; i <= len; i++) {
    if (value.IsDontCare(i)) {
      v_dc.Insert(Int(i), seq[i]);
    }
    else {
      v.Insert(Int(i), seq[i]);
    }
  }
  TYPE_SEM_REC semrec;
  semrec.Init(srec.GetRecord(pos_DYNSEM_SEM_SemRecord_tag), v, v_dc);
  return semrec;
}

TYPE_DYNSEM_SEM_SemRecord SemRec::REC2SemRecord(const TYPE_SEM_REC & semrec)
{
  const TYPE_AS_Name & symtag (semrec.GetRecord(pos_SEM_REC_tag));
//  if( !IsDecl(symtag) )
//  {
//    vdm_iplog << L"Unknown tag: " << ASTAUX::ASName2String(symtag) << endl << flush;
//    RTERR::Error(L"REC2SemRecord", RTERR_TAG_UNKNOWN, Nil(), Nil(), Sequence());
//  }
  MAP<Int, TYPE_SEM_VAL> fieldmap (semrec.GetMap(pos_SEM_REC_v));  
  fieldmap.ImpOverride(semrec.GetMap(pos_SEM_REC_v_udc));
  SEQ<TYPE_SEM_VAL> fields;
  int len = fieldmap.Size();
  for (int i = 1; i <= len; i++) {
    if (fieldmap.DomExists(Int(i))) {
      fields.ImpAppend(fieldmap[Int(i)]);
    }
  }
  return mk_SEM_REC(symtag, fields);
}

#ifdef VICE
// SizeExplFN
// efn : SEM`ExplFN
// ==> nat
int SemRec::SizeExplFN(const TYPE_SEM_ExplFN &efn)
{
  Map tm (efn.get_tm());
  TYPE_SEM_BlkEnv env (efn.get_env());

  int sum = 128 + 16 * env.get_id_um().Size();
  sum += (16 * tm.Size());
  return sum;
}

// SizeExplOP
// eop : SEM`ExplOP
// ==> nat
int SemRec::SizeExplOP(const TYPE_SEM_ExplOP &eop)
{
  int sum = 144;
  return sum; 
}

// SizeCompExplFN
// cefn : SEM`CompExplFN
// ==> nat
int SemRec::SizeCompExplFN(const TYPE_SEM_CompExplFN &cefn)
{
  int sum = (cefn.get_objref().IsNil() ? 8 : 16 );
  SEQ<TYPE_SEM_ExplFN> fl (cefn.get_fl());
  Generic efn_g;
  for( bool bb = fl.First(efn_g); bb; bb = fl.Next(efn_g)) {
    sum += SizeExplFN(efn_g);
  }
  return sum;
}

// SizeExplPOLY
// epoly : SEM`ExplPOLY
// ==> nat 
int SemRec::SizeExplPOLY(const TYPE_SEM_ExplPOLY &epoly)
{
  SEQ<TYPE_AS_TypeVar> tvl (epoly.get_tpparms());
  TYPE_SEM_BlkEnv env (epoly.get_env());

  int sum = 128 + 16 * env.get_id_um().Size();
  sum += (16 * tvl.Length());
  return sum;
}

// SizeValSeq
// val_l : seq of VAL
// ==> nat
int SemRec::SizeValSeq(const SEQ<TYPE_SEM_VAL> & val_l)
{
  int sum = 8;
  size_t len_val_l = val_l.Length();
  for(size_t idx = 1; idx <= len_val_l; idx++) {
    sum += Size(val_l[idx]);
  }
  return sum;
}

// SizeValSet
// val_s : set of VAL
// ==> nat
int SemRec::SizeValSet(const SET<TYPE_SEM_VAL> & val_s)
{
  int sum = 8;
  SET<TYPE_SEM_VAL> val_s_q (val_s);
  Generic g;
  for(bool bb = val_s_q.First(g); bb; bb = val_s_q.Next(g)) {
    sum += Size(g);
  }
  return sum;
}

// SizeValMap
// val_m : map VAL to VAL
// ==> nat
int SemRec::SizeValMap(const MAP<TYPE_SEM_VAL,TYPE_SEM_VAL> & val_m)
{
  int sum = 8;
  Set dom_val_m (val_m.Dom());
  Generic dom;
  for(bool bb = dom_val_m.First(dom); bb; bb = dom_val_m.Next(dom)) {
    sum += (Size(dom) + Size(val_m[dom]));
  }
  return sum;
}

// SizeNatValMap
// val_m : map nat to VAL
// ==> nat
int SemRec::SizeNatValMap(const Map & val_m)
{
  int sum = 8;
  Set dom_val_m (val_m.Dom());
  Generic dom;
  for(bool bb = dom_val_m.First(dom); bb; bb = dom_val_m.Next(dom)) {
    sum += (32 + Size(val_m[dom]));
  }
  return sum;
}

// SizeOverload
// over_m : map (AS`Name * AS`Name) to ((seq of AS`Type) * AS`Access)
// ==> nat
int SemRec::SizeOverload(const Map & over_m)
{
  int sum = 8;
  Set dom_over_m (over_m.Dom());
  Generic dom;
  for(bool bb = dom_over_m.First(dom); bb; bb = dom_over_m.Next(dom)) {
    Tuple t (over_m[dom]);
    sum += (32 + 16 * t.GetSequence(1).Length());
  }
  return sum;
}

// SizeValueMap
// val_m : GLOBAL`ValueMap   // map AS`Name to (SEM`VAL * AS`Access)
// ==> nat
int SemRec::SizeValueMap( const Map & val_m )
{
  int sum = 8;
  Set dom_val_m (val_m.Dom());
  Generic dom;
  for(bool bb = dom_val_m.First(dom); bb; bb = dom_val_m.Next(dom)) {
    Tuple t (val_m[dom]);
    sum += (2 + Size(t.GetField(1)));
  }
  return sum;
}

// SizeHist
// hist_m : map AS`Name to History
// ==> nat
int SemRec::SizeHist( const Map & hist_m )
{
  int sum = 8;
  sum += (3 * 16 * hist_m.Size());
  return sum;
}

// SizeInstStruct
// ins_m : SEM`InsStrct   // map AS`Name to GLOBAL`ValueMap
// ==> nat
int SemRec::SizeInstStruct( const Map & ins_m )
{
  int sum = 8;
  Set dom_ins_m (ins_m.Dom());
  Generic dom;
  for(bool bb = dom_ins_m.First(dom); bb; bb = dom_ins_m.Next(dom)) {
    sum += SizeValueMap(ins_m[dom]);
  }
  return sum;
}

// Size
// val : SEM`VAL
// -> nat
int SemRec::Size(const TYPE_SEM_VAL & val)
{
  switch(val.GetTag()) {
    case TAG_TYPE_SEM_EQORD:      { return Size(val.GetRecord(pos_SEM_EQORD_v)); }
    case TAG_TYPE_SEM_BOOL:       { return 1; }
    case TAG_TYPE_SEM_NUM:        { return 64; }
    case TAG_TYPE_SEM_CHAR:       { return 16; }
    case TAG_TYPE_SEM_QUOTE:      { return 8; }
    case TAG_TYPE_SEM_NIL:        { return 1; }
    case TAG_TYPE_SEM_CompExplFN: { return SizeCompExplFN(val); }
    case TAG_TYPE_SEM_ExplOP:     { return SizeExplOP(val); }
    case TAG_TYPE_SEM_ExplPOLY:   { return SizeExplPOLY(val); }
    case TAG_TYPE_SEM_SEQ:        { return SizeValSeq(val.GetSequence(pos_SEM_SEQ_v)); }
    case TAG_TYPE_SEM_SET:        { return SizeValSet(val.GetSet(pos_SEM_SET_v)); }
    case TAG_TYPE_SEM_MAP:        { return SizeValMap(val.GetMap(pos_SEM_MAP_v)); }
    case TAG_TYPE_SEM_TUPLE:      { return SizeValSeq(val.GetSequence(pos_SEM_TUPLE_v)); }
    case TAG_TYPE_DYNSEM_SEM_REC: {
      // SEM`REC impl is differ from spec
      TYPE_SEM_REC sv (SemRecord2REC(val));
      return 16 + SizeNatValMap(sv.get_v()) + SizeNatValMap(sv.get_v_udc());
    }
    case TAG_TYPE_SEM_TOKEN:      { return 8; }
    case TAG_TYPE_SEM_UNDEF:      { return 1; }
    case TAG_TYPE_SEM_EXIT: {
      const Generic & v (val.GetField(pos_SEM_EXIT_v));
      return 1 + (v.IsNil() ? 0 : Size(v));
    }
    case TAG_TYPE_SEM_CONT:       { return 1; }
    case TAG_TYPE_SEM_RETURN:     { return 1; }
    case TAG_TYPE_SEM_OverOPFN: {
      return 16 + SizeOverload(val.GetMap(pos_SEM_OverOPFN_overload));
    }
    case TAG_TYPE_SEM_OBJ: {
      return 8 + SizeInstStruct(val.GetField(pos_SEM_OBJ_ins))
               + SizeHist(val.GetField(pos_SEM_OBJ_hist));
    }
    case TAG_TYPE_SEM_OBJ_uRef:   { return 8; }
    default: {
      wcout << L"Unknown semantic value found." << endl;;
      wcout << val << endl;
      M4LibError::ReportError(-1, L"Unknown semantic value found.");
      return 1;
    }
  }
}

// Num2String
// r : real
// -> seq of char
wstring SemRec::Num2String( const Generic & num)
{
  return num.ascii();
}

// ValSeq2String
// v_l : seq of VAL
// -> seq of char
wstring SemRec::ValSeq2String( const SEQ<TYPE_SEM_VAL> & v_l )
{
  if( v_l.IsEmpty() ) {
    return L" ";
  }
  else {
    wstring ret;
    SEQ<TYPE_SEM_VAL> tmp (v_l);
    while( !tmp.IsEmpty() ) {
      ret += Val2String(tmp.Hd());
      tmp.ImpTl();
      if( !tmp.IsEmpty() ) {
        ret += L", ";
      }
    }
    return ret;
  }
}

// ValSet2String
// v_s : set of VAL
// -> seq of char
wstring SemRec::ValSet2String( const SET<TYPE_SEM_VAL> & v_s )
{
  if( v_s.IsEmpty() ) {
    return L" ";
  }
  else {
    wstring ret;
    Set tmp (v_s);
    while( !tmp.IsEmpty() ) {
      Generic e (tmp.GetElem());
      ret += Val2String(e);
      tmp.RemElem(e);
      if( !tmp.IsEmpty() ) {
        ret += L", ";
      }
    }
    return ret;
  }
}

// ValNatMap2String
// v_m1 : (map nat to VAL)
// v_m2 : (map nat to VAL)
// -> seq of char
wstring SemRec::ValNatMap2String( const Map & v_m1, const Map & v_m2 )
{
  if( v_m1.IsEmpty() && v_m2.IsEmpty() ) {
    return L" ";
  }
  else {
    wstring ret;
    Map tmp_m (v_m1);
    tmp_m.ImpOverride(v_m2);
    Set tmp_s (tmp_m.Dom());
    while( !tmp_s.IsEmpty() ) {
      Generic e = (tmp_s.GetElem());
      Generic g;
      for(bool bb = tmp_s.First(g); bb; bb = tmp_s.Next(g)) {
        if(e.Compare(g) > 0 ) {
          e = g;
        }
      }
      ret += Val2String(tmp_m[e]); 
      tmp_s.RemElem(e);
      if( !tmp_s.IsEmpty() ) {
        ret += L", ";
      }
    } 
    return ret;
  }
}

// ValMap2String
// v_m : map VAL to VAL
// -> seq of char
wstring SemRec::ValMap2String( const MAP<TYPE_SEM_VAL,TYPE_SEM_VAL> & v_m )
{
  if( v_m.IsEmpty() ) {
    return L" ";
  }
  else {
    wstring ret;
    Set tmp (v_m.Dom());
    while( !tmp.IsEmpty() ) {
      Generic e (tmp.GetElem());
      ret += Val2String(e) + L"|->" + Val2String(v_m[e]);
      tmp.RemElem(e);
      if( !tmp.IsEmpty() ) {
        ret += L", ";
      }
    }
    return ret;
  }
}

// Ids2String
// ids : AS`Ids
// -> seq of char
wstring SemRec::Ids2String(const TYPE_AS_Ids & ids_)
{
  wstring res;
  TYPE_AS_Ids ids (ids_);
  while (!ids.IsEmpty()) {
    res += ids[1].GetString();
    ids.ImpTl();
    if (!ids.IsEmpty()) {
      res += L"`";
    }
  }
  return res;
}

// Val2String
// val : SEM`VAL
// -> seq of char
wstring SemRec::Val2String( const TYPE_SEM_VAL & val )
{
  switch(val.GetTag()) {
    case TAG_TYPE_SEM_EQORD: {
      return Val2String(val.GetRecord(pos_SEM_EQORD_v));
    }
    case TAG_TYPE_SEM_BOOL: {
      if (val.GetBoolValue(pos_SEM_BOOL_v)) {
        return L"true ";
      }
      else {
        return L"false ";
      }
    }
    case TAG_TYPE_SEM_NUM: {
      return Num2String(val.GetReal(pos_SEM_NUM_v));
    }
    case TAG_TYPE_SEM_CHAR: {
      return wstring(L"'") + val.GetChar(pos_SEM_CHAR_v).GetValue() + L"'";
    }
    case TAG_TYPE_SEM_QUOTE: {
      return wstring(L"<") + val.GetSequence(pos_SEM_QUOTE_v).GetString() + L">";
    }
    case TAG_TYPE_SEM_NIL:        { return L"nil "; }
    case TAG_TYPE_SEM_CompExplFN: { return L"function value"; }
    case TAG_TYPE_SEM_ExplOP :    { return L"operation value"; }
    case TAG_TYPE_SEM_ExplPOLY:   { return L"polymorphic function value"; }
    case TAG_TYPE_SEM_SEQ: {
      const Sequence & s (val.GetSequence(pos_SEM_SEQ_v));
      if (s.IsString()) {
        return wstring(L"\"") + s.GetString() + L"\"";
      }
      else {
        return wstring(L"[ ") + ValSeq2String(s) + L" ]";
      }
    }
    case TAG_TYPE_SEM_SET: {
      return wstring(L"{ ") + ValSet2String(val.GetSet(pos_SEM_SET_v)) + L" }";
    }
    case TAG_TYPE_SEM_MAP: {
      return wstring(L"{ ") + ValMap2String(val.GetMap(pos_SEM_MAP_v)) + L" }";
    }
    case TAG_TYPE_SEM_TUPLE: {
      return wstring(L"mk_( ") + ValSeq2String(val.GetSequence(pos_SEM_TUPLE_v)) + L" )";
    }
    case TAG_TYPE_DYNSEM_SEM_REC: {
      // SEM`REC impl differ from spec
      const TYPE_AS_Name & tag (val.GetRecord(pos_DYNSEM_SEM_SemRecord_tag));
      SEQ<TYPE_SEM_VAL> seq (val.GetRecord(pos_DYNSEM_SEM_SemRecord_value).GetFields());
      wstring ret (L"mk_");
      ret += Ids2String(tag.get_ids());
      ret += L"( " + ValSeq2String(seq) + L" )";
      return ret;
    }
    case TAG_TYPE_SEM_TOKEN: {
      return wstring(L"mk_token( ") + Val2String(val.GetRecord(pos_SEM_TOKEN_v)) + L" )";
    }
    case TAG_TYPE_SEM_UNDEF: { return L"undefined "; }
    case TAG_TYPE_SEM_EXIT: {
      Generic val_g (val.GetField(pos_SEM_EXIT_v));
      if( val_g.IsNil() )
        return L"exit ";
      else
        return wstring(L"exit with ") + Val2String(val_g);
    }
    case TAG_TYPE_SEM_CONT:     { return L"Continue "; }
    case TAG_TYPE_SEM_RETURN:   { return L"Return "; }
    case TAG_TYPE_SEM_OverOPFN: { return L"Overloaded op/fn"; }
    case TAG_TYPE_SEM_OBJ:      { return L"Object value"; }
    case TAG_TYPE_SEM_OBJ_uRef: {
      Int ref (val.GetInt(pos_SEM_OBJ_uRef_ref));
      return L"objref" + Num2String(ref);
    }
    default: {
      wcout << L"Unknown semantic value found." << endl;;
      wcout << val << endl;
      M4LibError::ReportError(-1, L"Unknown semantic value found.");
      return L"";
    }
  }
}
#endif // VICE

SEQ<TYPE_SEM_CHAR> SemRec::GetSemChars(const SEQ<Char> & cs)
{
  SEQ<TYPE_SEM_CHAR> chars;
  size_t len_cs = cs.Length();
  for (size_t index = 1; index <= len_cs; index++) {
    chars.ImpAppend(GetSemChar(cs[index]));
  }
  return chars;
}

TYPE_SEM_CHAR SemRec::GetSemChar(const Char & c)
{
  if (!charMap.DomExists(c)) {
    charMap.Insert(c, mk_SEM_CHAR(c));
  }
  return charMap[c];
}

TYPE_SEM_QUOTE SemRec::GetSemQuote(const SEQ<Char> & cs)
{
  if (!quoteMap.DomExists(cs)) {
    quoteMap.Insert(cs, mk_SEM_QUOTE(cs));
  }
  return quoteMap[cs];
}

