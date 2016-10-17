//
// UTIL.cc
//
// Implementation of util.vdm v1.3
//
// $Id: UTIL.cc,v 1.6 2001/09/06 10:10:01 mikhail Exp $
//

#include "UTIL.h"
#include "projectval.h"
#include "tbutils.h"
#include "rt_errmsg.h"
#include "tb_exceptions.h"
#include "astaux.h"
#include "statsem.h"

Real vdm_UTIL_minset(const SET<Real> & p_s)
{
  return UTIL::minset(p_s);
}

SEQ<Char> vdm_UTIL_itoa(const Int& i )
{
  return UTIL::itoa(i);
}

SEQ<Char> vdm_UTIL_ReplacePercent(const SEQ<Char>& err, 
				  const SEQ<SEQ<Char> >& Txts)
{
  return UTIL::ReplacePercent(err, Txts);
}

// set2seq
// s : set of @tp
// -> seq of @tp
Sequence UTIL::set2seq(const Set & s)
{
  Sequence res;
  Generic v;
  for (bool bb = s.First(v); bb; bb = s.Next(v)) {
    res.ImpAppend(v);
  }
  return res;
}

// minset
// p_s : set of real
// -> real
Real UTIL::minset(const SET<Real> & p_s)
{
  Real l_v (p_s.GetElem());
  if( p_s.Card() == 1 )
    return l_v;
  else
  {
    Set new_p_s( p_s );
    new_p_s.RemElem( l_v );
    Real l_minrest (minset( new_p_s ));
    if( l_v > l_minrest )
      return l_minrest;
    else
      return l_v;
  }
}

// itoa
// n : nat
// -> seq of char
SEQ<Char> UTIL::itoa(const Int & n)
{
  return SEQ<Char>(n.ascii());
}

// ReplacePercent
// err : seq of char
// Txts : seq of seq of char
// ==> seq of char
SEQ<Char> UTIL::ReplacePercent(const SEQ<Char> & err, 
	                       const SEQ< SEQ<Char> > & Txts)
{
  int count = Txts.Length();
  bool percent = false;
  SEQ<Char> res;
  int max = 0;

  Generic g;
  for (bool bb = err.First(g); bb; bb = err.Next(g))
  {
    Char ch (g);
    switch (ch.GetValue()) {

      case L'%': {
        if (percent) {
          percent = false;
          res.ImpAppend(ch);
        } 
        else {
          percent = true;
        }
        break;
      }

      case L'1':
      case L'2':
      case L'3':
      case L'4':
      case L'5':
      case L'6':
      case L'7':
      case L'8':
      case L'9': {
        if (percent) {
          int num = ch - L'0';
          if (count < num) {
#ifndef TESTSPEC
            vdm_err << endl;
            vdm_err << L"Number of arguments doesn't match expected number of arguments ";
            vdm_err << L"to error function in UTIL::ReplacePercent" << endl;
            vdm_err << L"message: " << err << endl;
            vdm_err << L"arguments: " << Txts << endl << flush;
//            vdm_err << L"Error index out of range in UTIL::ReplacePercent" << endl;
#endif //TESTSPEC
            throw TB_Exception(ERR_IP);
          }
          percent = false;
          if (num > max)
            max = num;
          res.ImpConc(Txts[num]);
        }
        else {
          res.ImpAppend(ch);
        }
        break;
      }
//      case L'"': {
//        res.ImpAppend(ch);
//        break;
//      }
      default: {
        res.ImpAppend(ch);
        break;
      }
    }
  }
  if (count > max) {
#ifndef TESTSPEC
    vdm_err << endl;
    vdm_err << L"Number of arguments doesn't match expected number of arguments ";
    vdm_err << L"to error function in UTIL::ReplacePercent" << endl;
    vdm_err << L"message: " << err << endl;
    vdm_err << L"arguments: " << Txts << endl << flush;
#endif //TESTSPEC
    throw TB_Exception(ERR_IP);
  }
  return res;
}

// GetOpParms
// op : AS`OpDef
// -> seq of AS`Type
SEQ<TYPE_AS_Type> UTIL::GetOpParms(const TYPE_AS_OpDef & op)
{
  switch (op.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef:
      return op.GetRecord(pos_AS_ExplOpDef_tp).GetSequence(pos_AS_OpType_opdom);
    case TAG_TYPE_AS_ImplOpDef: {
      const SEQ<TYPE_AS_PatTypePair> & partps (op.GetSequence(pos_AS_ImplOpDef_partps));
      return UTIL::ConstructImplDomType(partps);
    }
    case TAG_TYPE_AS_ExtExplOpDef: {
      const SEQ<TYPE_AS_PatTypePair> & partps (op.GetSequence(pos_AS_ExtExplOpDef_partps));
      return UTIL::ConstructImplDomType(partps);
    }
    default: {
#ifndef TESTSPEC
      vdm_err << L"internal error in GetOpParms.";
#endif //TESTSPEC
      throw TB_Exception(ERR_IP);
    }
  }
}

// GetFnParms
// fn : AS`FnDef
// -> seq of AS`Type
SEQ<TYPE_AS_Type> UTIL::GetFnParms(const TYPE_AS_FnDef & fn)
{
  switch (fn.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:
      return fn.GetRecord(pos_AS_ExplFnDef_tp).GetSequence(1);
    case TAG_TYPE_AS_ImplFnDef: {
      const SEQ<TYPE_AS_PatTypePair> & partps (fn.GetSequence(pos_AS_ImplFnDef_partps));
      return UTIL::ConstructImplDomType(partps);
    }
    case TAG_TYPE_AS_ExtExplFnDef: {
      const SEQ<TYPE_AS_PatTypePair> & partps (fn.GetSequence(pos_AS_ExtExplFnDef_partps));
      return UTIL::ConstructImplDomType(partps);
    }
    default: {
#ifndef TESTSPEC
      vdm_err << L"internal error in GetFnParms.";
#endif //TESTSPEC
      throw TB_Exception(ERR_IP);
    }
  }
}

// GetOpRestype
// op : AS`OpDef
// -> [AS`Type]
Generic UTIL::GetOpRestype(const TYPE_AS_OpDef& op)
{
  switch (op.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef:
      return op.GetRecord(pos_AS_ExplOpDef_tp).GetField(pos_AS_OpType_oprng);
    case TAG_TYPE_AS_ImplOpDef: {
      const SEQ<TYPE_AS_NameType> & restps (op.GetSequence(pos_AS_ImplOpDef_resnmtps));
      return UTIL::ConstructImplResType(restps);
    }
    case TAG_TYPE_AS_ExtExplOpDef: {
      const SEQ<TYPE_AS_NameType> & restps (op.GetSequence(pos_AS_ExtExplOpDef_resnmtps));
      return UTIL::ConstructImplResType(restps);
    }
    default: {
#ifndef TESTSPEC
      vdm_err << L"internal error in GetOpRestype.";
#endif //TESTSPEC
      throw TB_Exception(ERR_IP);
    }
  }
}

// GetFnRestype
// fn : AS`FnDef
// -> AS`Type
TYPE_AS_Type UTIL::GetFnRestype(const TYPE_AS_FnDef& fn)
{
  switch (fn.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:
      return fn.GetRecord(pos_AS_ExplFnDef_tp).GetRecord(2);
    case TAG_TYPE_AS_ImplFnDef: {
      const SEQ<TYPE_AS_NameType> & restps (fn.GetSequence(pos_AS_ImplFnDef_resnmtps));
      return UTIL::ConstructImplResType(restps);
    }
    case TAG_TYPE_AS_ExtExplFnDef: {
      const SEQ<TYPE_AS_NameType> & restps (fn.GetSequence(pos_AS_ExtExplFnDef_resnmtps));
      return UTIL::ConstructImplResType(restps);
    }
    default: {
#ifndef TESTSPEC
      vdm_err << L"internal error in GetFnRestype.";
#endif //TESTSPEC
      throw TB_Exception(ERR_IP);
    }
  }
}

// ConstructImplDomType
// partps : seq of AS`PatTypePair
// -> seq of AS`Type
SEQ<TYPE_AS_Type> UTIL::ConstructImplDomType(
                          const SEQ<TYPE_AS_PatTypePair> & partps)
{
  SEQ<TYPE_AS_Type> result;
  int len = partps.Length();
  for (int i = 1; i <= len; i++)
  {
    TYPE_AS_PatTypePair ptp (partps[i]);
    const TYPE_AS_Type & tp (ptp.GetRecord(pos_AS_PatTypePair_tp));
    int len_ptp = ptp.GetSequence(pos_AS_PatTypePair_pats).Length();
    for (int j = 1; j <= len_ptp; j++)
      result.ImpAppend(tp);
  }
  return result;
}

// ConstructImplResType
// restps : seq of AS`NameType
// -> [AS`Type]
Generic UTIL::ConstructImplResType(const SEQ<TYPE_AS_NameType> & restps)
{
  switch(restps.Length()) {
    case 0:
      return Nil();
    case 1:
      return restps[1].GetRecord(pos_AS_NameType_tp);
    default: {
      SEQ<TYPE_AS_Type> result;
      int len = restps.Length();
      for (int i = 1; i <= len; i++)
        result.ImpAppend(restps[i].GetRecord(pos_AS_NameType_tp));
      return result;
    }
  }
}

// GetFnMeasu
// fn : AS`FnDef
// -> [AS`Name]
Generic UTIL::GetFnMeasu(const TYPE_AS_FnDef & fn)
{
  switch (fn.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:
      return fn.GetField(pos_AS_ExplFnDef_measu);
    case TAG_TYPE_AS_ImplFnDef:
    case TAG_TYPE_AS_ExtExplFnDef: {
      return Nil();
    }
    default: {
#ifndef TESTSPEC
      vdm_err << L"internal error in GetFnMeasu.";
#endif //TESTSPEC
      throw TB_Exception(ERR_IP);
    }
  }
}

// QualiName
// p_cl : AS`Name
// p_nm : AS`Name -> AS`Name
TYPE_AS_Name UTIL::QualiName(const TYPE_AS_Name & p_cl,
                             const TYPE_AS_Name & p_nm)
{
  return ((p_nm.GetSequence(pos_AS_Name_ids).Length() == 2)
                    ? p_nm
                    : ASTAUX::Combine2Names(p_cl, p_nm));
}

// UnqualiName
// p_nm : AS`Name
// -> AS`Name
TYPE_AS_Name UTIL::UnqualiName(const TYPE_AS_Name& p_nm)
{
  return TYPE_AS_Name().Init(TYPE_AS_Ids().ImpAppend(GetLastId(p_nm)),
                             p_nm.GetInt(pos_AS_Name_cid));
}

// split
// cs : seq of char
// ch : char
// -> seq of seq of char
SEQ< SEQ<Char> > UTIL::split(const SEQ<Char> & cs, char ch)
{
  if (cs.IsEmpty())
    return SEQ< SEQ<Char> >();
  
  int j = 1, length = cs.Length();
  Char cch (ch);
  SEQ< SEQ<Char> > allChunks;

  while (j <= length)
  {
    int i = j;
    SEQ<Char> thisChunk;
    while (i <= length && cs[i] != cch)
    {
      thisChunk.ImpAppend(cs[i]);
      i++;
    }
    allChunks.ImpAppend(thisChunk);
    j = i+1;
  }
  return allChunks;
}

