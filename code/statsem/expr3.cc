/***
*  * ID
*  *    $Id: expr3.cc,v 1.66 2006/03/22 07:15:37 vdmtools Exp $
*  * SPECIFICATION VERSION
*  *    expr3.cc: Implementation of expr.vdm 1.131
***/

#include "statsem.h"
#include "intconvquotes.h"
#include "settings.h"

//wcout << L"intertp: " << INT2Q::h2gAS(intertp) << endl;

#ifndef NOSS

// wf_SEQTAIL
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SEQTAIL (const Int & i, const TYPE_AS_PrefixExpr & unexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & rhs (unexpr.GetRecord(pos_AS_PrefixExpr_arg));

  TYPE_REP_TypeRep ExpectedRhsType (seq_alltp);

  TYPE_REP_TypeRep intertp (IntersectTypeReps(ExpectedRhsType, exptp));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_arg (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & argtp (inferrhs.GetRecord(2));

  if (IsCompatible (i, argtp, ExpectedRhsType))
  {
    TYPE_REP_TypeRep restp (ExtractSeqType(argtp));
    switch(restp.GetTag()) {
      case TAG_TYPE_REP_EmptySeqTypeRep: {
        //--------------------------------------
        // Error message #142
        // Argument of 'tl' is an empty sequence
        //--------------------------------------
        GenErr (unexpr, ERR, 142, Sequence());
        return mk_(Bool(false), ExpectedRhsType);
      }
      case TAG_TYPE_REP_SeqTypeRep: {
        return mk_(wf_arg, mk_REP_UnionTypeRep(
                             mk_set(restp, mk_REP_EmptySeqTypeRep(restp.GetRecord(pos_REP_SeqTypeRep_elemtp)))));
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        SET<TYPE_REP_TypeRep> utps (restp.GetSet(pos_REP_UnionTypeRep_tps));
        bool exists = false; 
        Generic t;
        for (bool bb = utps.First(t); bb && !exists; bb = utps.Next(t))
          exists = (t.Is(TAG_TYPE_REP_SeqTypeRep) || t.Is(TAG_TYPE_REP_EmptySeqTypeRep));

        if (exists && (i == DEF) && (Settings.ErrorLevel() >= ERR2))
        {
          //------------------------------------------
          // Error message #143
          // Argument of 'tl' can be an empty sequence
          //------------------------------------------
          GenErr (unexpr, ERR2, 143, Sequence());
          utps.Insert(mk_REP_EmptySeqTypeRep(rep_alltp));
          return mk_(Bool(false), mk_REP_UnionTypeRep(utps));
        }
        else
        {
          utps.Insert(mk_REP_EmptySeqTypeRep(rep_alltp));
          return mk_(wf_arg, mk_REP_UnionTypeRep(utps));
        }
      }
      default:
        return Tuple(); //This is only to avoid warnings from the compiler
    }
  }
  else
  {
    //----------------------------------------
    // Error message #144
    // Argument of 'tl' is not a sequence type
    //----------------------------------------
    GenErrTp (unexpr, ERR, 144, argtp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), ExpectedRhsType);
  }
}

// wf_SEQREVERSE
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SEQREVERSE (const Int & i, const TYPE_AS_PrefixExpr & unexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & rhs (unexpr.GetRecord(pos_AS_PrefixExpr_arg));

  TYPE_REP_TypeRep ExpectedRhsType (seq_alltp);

  TYPE_REP_TypeRep intertp (IntersectTypeReps(ExpectedRhsType, exptp));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_arg (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & argtp (inferrhs.GetRecord(2));

  if (IsCompatible (i, argtp, ExpectedRhsType))
  {
    return mk_(wf_arg, ExtractSeqType(argtp));
  }
  else
  {
    //----------------------------------------
    // Error message #437
    // Argument of 'reverse' is not a sequence type
    //----------------------------------------
    GenErrTp (unexpr, ERR, 437, argtp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), ExpectedRhsType);
  }
}

// wf_MAPDOM
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_MAPDOM (const Int & i, const TYPE_AS_PrefixExpr & unexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & rhs (unexpr.GetRecord(pos_AS_PrefixExpr_arg));

  TYPE_REP_TypeRep domtp;
  if (IsCompatible (i, exptp, set_alltp))
    domtp = UnmaskSetType(i, ExtractSetType(exptp));
  else
    domtp = rep_alltp;

  TYPE_REP_TypeRep ExpectedType (mk_REP_UnionTypeRep(mk_set(mk_REP_EmptyMapTypeRep(domtp, rep_alltp),
                                                            mk_REP_GeneralMapTypeRep(domtp, rep_alltp))));
  TYPE_REP_TypeRep ExpectedRhsType (mk_REP_UnionTypeRep(mk_set(mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp),
                                                               mk_REP_GeneralMapTypeRep(rep_alltp, rep_alltp))));
  TYPE_REP_TypeRep intertp (IntersectTypeReps (ExpectedRhsType, ExpectedType));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_arg (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & argtp (inferrhs.GetRecord(2));

  if (IsCompatible (i, argtp, ExpectedRhsType))
  {
    Generic r (ExtractMapType(argtp)); // [REP`TypeRep]
    if (r.IsNil ())
      return mk_(wf_arg, mk_REP_EmptySetTypeRep(rep_alltp));

    TYPE_REP_TypeRep r1tp (r);
    switch(r1tp.GetTag()) {
      case TAG_TYPE_REP_EmptyMapTypeRep: {
        return mk_(wf_arg, mk_REP_EmptySetTypeRep(r1tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom)));
      }
      case TAG_TYPE_REP_GeneralMapTypeRep: {
        return mk_(wf_arg, mk_REP_SetTypeRep(r1tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom)));
      }
      case TAG_TYPE_REP_InjectiveMapTypeRep: {
        return mk_(wf_arg, mk_REP_SetTypeRep(r1tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom)));
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        SET<TYPE_REP_TypeRep> utps (r1tp.GetSet(pos_REP_UnionTypeRep_tps));
        SET<TYPE_REP_TypeRep> r2tp;
        Generic gutp;
        for (bool bb = utps.First(gutp); bb ; bb = utps.Next(gutp))
        {
          TYPE_REP_TypeRep utp (gutp);
          switch(utp.GetTag()) {
            case TAG_TYPE_REP_GeneralMapTypeRep: {
              r2tp.Insert(utp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom));
              break;
            }
            case TAG_TYPE_REP_InjectiveMapTypeRep: {
              r2tp.Insert(utp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom));
              break;
            }
          }
        }
        SET<TYPE_REP_TypeRep> r3tp (MergeNumericTypes(r2tp));
        switch (r3tp.Card()) {
          case 1: {
            TYPE_REP_TypeRep tp (r3tp.GetElem());
            if (utps.InSet(mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp)))
              return mk_(wf_arg, mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySetTypeRep(tp), mk_REP_SetTypeRep(tp))));
            else
              return mk_(wf_arg, mk_REP_SetTypeRep(tp));
          }
          default: {
            TYPE_REP_TypeRep tp (mk_REP_UnionTypeRep(r3tp));
            if (utps.InSet(mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp)))
              return mk_(wf_arg, mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySetTypeRep(tp), mk_REP_SetTypeRep(tp))));
            else
              return mk_(wf_arg, mk_REP_SetTypeRep(tp));
          }
        }
      }
    }
  }
  else
  {
    //------------------------------------
    // Error message #145
    // Argument of 'dom' is not a map type
    //------------------------------------
    GenErrTp (unexpr, ERR, 145, argtp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), mk_REP_SetTypeRep(rep_alltp));
  }
  return Tuple(); //This is only to avoid warnings from the compiler
}

// wf_MAPRNG
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_MAPRNG (const Int & i, const TYPE_AS_PrefixExpr & unexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & rhs (unexpr.GetRecord(pos_AS_PrefixExpr_arg));

  TYPE_REP_TypeRep rngtp;
  if (IsCompatible (i, exptp, set_alltp))
    rngtp = UnmaskSetType(i, ExtractSetType(exptp));
  else
    rngtp = rep_alltp;

  TYPE_REP_TypeRep ExpectedType (mk_REP_UnionTypeRep(mk_set(mk_REP_EmptyMapTypeRep(rep_alltp, rngtp),
                                                            mk_REP_GeneralMapTypeRep(rep_alltp, rngtp))));
  TYPE_REP_UnionTypeRep ExpectedRhsType (mk_REP_UnionTypeRep(mk_set(mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp),
                                                             mk_REP_GeneralMapTypeRep(rep_alltp, rep_alltp))));
  TYPE_REP_TypeRep intertp (IntersectTypeReps (ExpectedRhsType, ExpectedType));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_arg (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & argtp (inferrhs.GetRecord(2));

  if (IsCompatible (i, argtp, ExpectedRhsType))
  {
    Generic r (ExtractMapType(argtp));
    if (r.IsNil ())
      return mk_(wf_arg, mk_REP_EmptySetTypeRep(rep_alltp));

    TYPE_REP_TypeRep r1tp (r);
    switch(r1tp.GetTag()) {
      case TAG_TYPE_REP_EmptyMapTypeRep: {
        return mk_(wf_arg, mk_REP_EmptySetTypeRep(r1tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng)));
      }
      case TAG_TYPE_REP_GeneralMapTypeRep: {
        return mk_(wf_arg, mk_REP_SetTypeRep(r1tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng)));
      }
      case TAG_TYPE_REP_InjectiveMapTypeRep: {
        return mk_(wf_arg, mk_REP_SetTypeRep(r1tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng)));
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        SET<TYPE_REP_TypeRep> utps (r1tp.GetSet(pos_REP_UnionTypeRep_tps));
        SET<TYPE_REP_TypeRep> r2tp;
        Generic gutp;
        for (bool bb = utps.First(gutp); bb ; bb = utps.Next(gutp))
        {
          TYPE_REP_TypeRep utp (gutp);
          switch(utp.GetTag()) {
            case TAG_TYPE_REP_GeneralMapTypeRep: {
              r2tp.Insert(utp.GetRecord(pos_REP_GeneralMapTypeRep_maprng));
              break;
            }
            case TAG_TYPE_REP_InjectiveMapTypeRep: {
              r2tp.Insert(utp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng));
              break;
            }
          }
        }
        SET<TYPE_REP_TypeRep> r3tp (MergeNumericTypes(r2tp));
        switch (r3tp.Card()) {
          case 1: {
            TYPE_REP_TypeRep tp (r3tp.GetElem());
            if (utps.InSet(mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp)))
              return mk_(wf_arg, mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySetTypeRep(tp), mk_REP_SetTypeRep(tp))));
            else
              return mk_(wf_arg, mk_REP_SetTypeRep(tp));
          }
          default: {
            TYPE_REP_TypeRep tp (mk_REP_UnionTypeRep(r3tp));
            if (utps.InSet(mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp)))
              return mk_(wf_arg, mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySetTypeRep(tp), mk_REP_SetTypeRep(tp))));
            else
              return mk_(wf_arg, mk_REP_SetTypeRep(tp));
          }
        }
      }
    }
  }
  else
  {
    //------------------------------------
    // Error message #146
    // Argument of 'rng' is not a map type
    //------------------------------------
    GenErrTp (unexpr, ERR, 146, argtp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), mk_REP_SetTypeRep(rep_alltp));
  }
  return Tuple(); //This is only to avoid warnings from the compiler
}

// wf_MAPDISTRMERGE
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_MAPDISTRMERGE (const Int & i, const TYPE_AS_PrefixExpr & unexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & rhs (unexpr.GetRecord(pos_AS_PrefixExpr_arg));

  TYPE_REP_TypeRep maptp;
  if (IsCompatible (i, exptp, map_alltp))
    maptp = ExtractMapType(exptp);
  else
    maptp = map_alltp;

  TYPE_REP_TypeRep ExpectedType (mk_REP_SetTypeRep(maptp));
  TYPE_REP_UnionTypeRep ExpectedRhsType (mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySetTypeRep(rep_alltp),
                                                                    mk_REP_SetTypeRep(map_alltp))));
  TYPE_REP_TypeRep intertp (IntersectTypeReps (ExpectedRhsType, ExpectedType));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_arg (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & argtp (inferrhs.GetRecord(2));

  if (IsCompatible (i, argtp, ExpectedRhsType))
  {
    Generic elmtp (UnmaskSetType (i, ExtractSetType (argtp)));

    if (elmtp.IsNil() || argtp.Is(TAG_TYPE_REP_EmptySetTypeRep))
    {
      //--------------------------------------
      // Error message #147
      // 'merge' will always give an empty map
      //--------------------------------------
      GenErr (unexpr, ERR, 147, Sequence());
      return mk_(Bool (false), mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp));
    }
    else
    {
      elmtp = ExtractMapType (elmtp);
      if (elmtp.IsNil ())
        return mk_(wf_arg, mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp));
      else if (elmtp.Is(TAG_TYPE_REP_UnionTypeRep))
      {
        SET<TYPE_REP_TypeRep> tps (Record (elmtp).GetSet (pos_REP_UnionTypeRep_tps));
        return mk_(wf_arg, MapTypeMerge (tps));
      }
      else
        return mk_(wf_arg, elmtp);
    }
  }
  else
  {
    //-----------------------------------------
    // Error message #148
    // Argument of 'merge' is not a set of maps
    //-----------------------------------------
    GenErrTp (unexpr, ERR, 148, argtp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), ExpectedType);
  }
}

// wf_MAPINVERSE
// i : TYPE`Ind
// unexpr : AS`PrefixExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_MAPINVERSE (const Int & i, const TYPE_AS_PrefixExpr & unexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & oper (unexpr.GetRecord(pos_AS_PrefixExpr_arg));
  const TYPE_CI_ContextId & cid (unexpr.GetInt(pos_AS_PrefixExpr_cid));

  TYPE_REP_TypeRep ExpectedType (map_alltp);

  TYPE_REP_TypeRep intertp (IntersectTypeReps (ExpectedType, InverseMapType(exptp)));

  Tuple infer (wf_Expr (i, oper, intertp));
  const Bool & wf_arg (infer.GetBool (1));
  const TYPE_REP_TypeRep & tp (infer.GetRecord (2));

  if (IsCompatible(i, tp, ExpectedType))
  {
    Generic mtp (ExtractMapType (tp));
    TYPE_REP_TypeRep restp;
    if (mtp.IsNil ())
      restp = mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp);
    else
      restp = InverseMapType (mtp);

    GetCI().SetTypeInfo (cid, restp);
    return mk_(wf_arg, restp);
  }
  else
  {
    //-----------------------------------------------------
    // Error message #149
    // Argument to Map-Inverse-Expr is not an injective map
    //-----------------------------------------------------
    GenErrTp (unexpr, ERR, 149, tp, ExpectedType, Sequence());
    GetCI().SetTypeInfo (cid, ExpectedType);
    return mk_(Bool(false), ExpectedType);
  }
}

// wf_BinaryExpr
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_BinaryExpr (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const Int & op (binexpr.GetInt(pos_AS_BinaryExpr_opr));
  const TYPE_CI_ContextId & cid (binexpr.GetInt(pos_AS_BinaryExpr_cid));

  Tuple tpl;
  switch(op.GetValue ()) {
    case NUMPLUS:
      tpl = wf_NUMPLUS(i, binexpr, exptp);
      break;
    case NUMMINUS:
      tpl = wf_NUMMINUS(i, binexpr, exptp);
      break;
    case NUMMULT:
      tpl = wf_NUMMULT(i, binexpr, exptp);
      break;
    case NUMDIV:
      tpl = wf_NUMDIV(i, binexpr, exptp);
      break;
    case NUMREM:
      tpl = wf_NUMREM(i, binexpr, exptp);
      break;
    case NUMMOD:
      tpl = wf_NUMMOD(i, binexpr, exptp);
      break;
    case INTDIV:
      tpl = wf_INTDIV(i, binexpr, exptp);
      break;
    case NUMEXP:
      tpl = wf_NUMEXP(i, binexpr, exptp);
      break;
    case NUMLT:
      tpl = wf_NUMLT(i, binexpr, exptp);
      break;
    case NUMLE:
      tpl = wf_NUMLE(i, binexpr, exptp);
      break;
    case NUMGT:
      tpl = wf_NUMGT(i, binexpr, exptp);
      break;
    case NUMGE:
      tpl = wf_NUMGE(i, binexpr, exptp);
      break;
    case AND:
      tpl = wf_AND(i, binexpr, exptp);
      break;
    case OR:
      tpl = wf_OR(i, binexpr, exptp);
      break;
    case IMPLY:
      tpl = wf_IMPLY(i, binexpr, exptp);
      break;
    case EQUIV:
      tpl = wf_EQUIV(i, binexpr, exptp);
      break;
    case EQ:
      tpl = wf_EQ(i, binexpr, exptp);
      break;
    case NE:
      tpl = wf_NE(i, binexpr, exptp);
      break;
    case SETUNION:
      tpl = wf_SETUNION (i, binexpr, exptp);
      break;
    case SETINTERSECT:
      tpl = wf_SETINTERSECT (i, binexpr, exptp);
      break;
    case SETMINUS:
      tpl = wf_SETMINUS (i, binexpr, exptp);
      break;
    case SUBSET:
      tpl = wf_SUBSET (i, binexpr, exptp);
      break;
    case PROPERSUBSET:
      tpl = wf_PROPERSUBSET (i, binexpr, exptp);
      break;
    case INSET:
      tpl = wf_INSET (i, binexpr, exptp);
      break;
    case NOTINSET:
      tpl = wf_NOTINSET (i, binexpr, exptp);
      break;
    case SEQCONC:
      tpl = wf_SEQCONC (i, binexpr, exptp);
      break;
    case MAPMERGE:
      tpl = wf_MAPMERGE (i, binexpr, exptp);
      break;
    case MAPDOMRESTTO:
      tpl = wf_MAPDOMRESTTO (i, binexpr, exptp);
      break;
    case MAPDOMRESTBY:
      tpl = wf_MAPDOMRESTBY (i, binexpr, exptp);
      break;
    case MAPRNGRESTTO:
      tpl = wf_MAPRNGRESTTO (i, binexpr, exptp);
      break;
    case MAPRNGRESTBY:
      tpl = wf_MAPRNGRESTBY (i, binexpr, exptp);
      break;
    case COMPOSE:
      tpl =  wf_COMPOSE (i, binexpr, exptp);
      break;
    default:
      InternalError(L"wf_BinaryExpr");
      tpl = mk_(Bool(false), rep_alltp);
      break;
  }

  GetCI().SetTypeInfo (cid, tpl.GetRecord(2));
  return tpl;
}

// wf_NUMPLUS
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_NUMPLUS (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType (btp_real);

  TYPE_REP_TypeRep intertp(IntersectTypeReps(ExpectedLhsAndRhsType, exptp));

  Tuple inferlhs (wf_Expr(i, lhs, intertp));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);

  if (!lhscomp)
  {
    //---------------------------------
    // Error message #150
    // Lhs of '+' is not a numeric type
    //---------------------------------
    GenErrTp (binexpr, ERR, 150, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp)
  {
    //---------------------------------
    // Error message #151
    // Rhs of '+' is not a numeric type
    //---------------------------------
    GenErrTp (binexpr, ERR, 151, rhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (lhscomp && rhscomp)
  {
    TYPE_REP_TypeRep lhsrestp (ExtractNumericType (lhstp));
    TYPE_REP_TypeRep rhsrestp (ExtractNumericType (rhstp));

    TYPE_REP_TypeRep newtp (MostGeneralNumericType (mk_set(lhsrestp, rhsrestp)));

    if ((newtp == btp_nat) && ((lhsrestp == btp_natone) || (rhsrestp == btp_natone)))
      return mk_(wf_lhs && wf_rhs, btp_natone);
    else
      return mk_(wf_lhs && wf_rhs, newtp);
  }
  else
    return mk_(Bool(false), ExpectedLhsAndRhsType);
}

// wf_NUMMINUS
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_NUMMINUS (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType (btp_real);
  TYPE_REP_TypeRep intertp(IntersectTypeReps(ExpectedLhsAndRhsType, exptp));

  Tuple inferlhs (wf_Expr(i, lhs, intertp));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);

  if (!lhscomp)
  {
    //---------------------------------
    // Error message #152
    // Lhs of '-' is not a numeric type
    //---------------------------------
    GenErrTp (binexpr, ERR, 152, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp)
  {
    //---------------------------------
    // Error message #153
    // Rhs of '-' is not a numeric type
    //---------------------------------
    GenErrTp (binexpr, ERR, 153, rhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (lhscomp && rhscomp)
  {
    TYPE_REP_TypeRep lhsrestp (ExtractNumericType (lhstp));
    TYPE_REP_TypeRep rhsrestp (ExtractNumericType (rhstp));

    return mk_(wf_lhs && wf_rhs, MostGeneralNumericType(mk_set(lhsrestp,
                                                               rhsrestp,
                                                               btp_int)));
  }
  else
    return mk_(Bool(false), ExpectedLhsAndRhsType);
}

// wf_NUMMULT
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_NUMMULT (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType (btp_real);

  TYPE_REP_TypeRep intertp(IntersectTypeReps(ExpectedLhsAndRhsType, exptp));

  Tuple inferlhs (wf_Expr(i, lhs, intertp));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);

  if (!lhscomp)
  {
    //---------------------------------
    // Error message #154
    // Lhs of '*' is not a numeric type
    //---------------------------------
    GenErrTp (binexpr, ERR, 154, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp)
  {
    //---------------------------------
    // Error message #155
    // Rhs of '*' is not a numeric type
    //---------------------------------
    GenErrTp (binexpr, ERR, 155, rhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (lhscomp && rhscomp)
  {
    TYPE_REP_TypeRep lhsrestp (ExtractNumericType (lhstp));
    TYPE_REP_TypeRep rhsrestp (ExtractNumericType (rhstp));

    return mk_(wf_lhs && wf_rhs, MostGeneralNumericType(mk_set(lhsrestp, rhsrestp)));
  }
  else
    return mk_(Bool(false), ExpectedLhsAndRhsType);
}

// wf_NUMDIV
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_NUMDIV (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType (btp_real);
  TYPE_REP_TypeRep intertp (IntersectTypeReps(ExpectedLhsAndRhsType, exptp));

  Tuple inferlhs (wf_Expr(i, lhs, intertp));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);

  if (!lhscomp)
  {
    //---------------------------------
    // Error message #156
    // Lhs of '/' is not a numeric type
    //---------------------------------
    GenErrTp (binexpr, ERR, 156, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp)
  {
    //---------------------------------
    // Error message #157
    // Rhs of '/' is not a numeric type
    //---------------------------------
    GenErrTp (binexpr, ERR, 157, rhstp, ExpectedLhsAndRhsType, Sequence());
  }
  else if ((rhstp != btp_natone) && (i == DEF) && (Settings.ErrorLevel() >= PRF))
  {
    //----------------------------
    // Error message #158
    // Rhs of '/' must be non zero
    //----------------------------
    GenErrTp (binexpr, PRF, 158, rhstp, btp_natone, Sequence());
  }

  if (lhscomp && rhscomp && (i == POS))
    return mk_(wf_lhs && wf_rhs, ExpectedLhsAndRhsType);
  else if (rhstp != btp_natone)
    return mk_(Bool(false), ExpectedLhsAndRhsType);
  else
    return mk_(wf_lhs && wf_rhs && lhscomp && rhscomp, ExpectedLhsAndRhsType);
}

// wf_NUMREM
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_NUMREM (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType (btp_int);

  TYPE_REP_TypeRep intertp (IntersectTypeReps(ExpectedLhsAndRhsType, exptp));

  Tuple inferlhs (wf_Expr(i, lhs, intertp));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);

  if (!lhscomp)
  {
    //------------------------------------
    // Error message #159
    // Lhs of 'rem' is not an integer type
    //------------------------------------
    GenErrTp (binexpr, ERR, 159, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp)
  {
    //------------------------------------
    // Error message #160
    // Rhs of 'rem' is not an integer type
    //------------------------------------
    GenErrTp (binexpr, ERR, 160, rhstp, ExpectedLhsAndRhsType, Sequence());
  }
  else if ((rhstp != btp_natone) && (i == DEF) )
  {
    //------------------------------
    // Error message #161
    // Rhs of 'rem' must be non zero
    //------------------------------
    GenErrTp (binexpr, PRF, 161, rhstp, btp_natone, Sequence());
  }

  if (lhscomp && rhscomp)
  {
    TYPE_REP_TypeRep lhsrestp (ExtractIntNumType (lhstp));
    TYPE_REP_TypeRep rhsrestp (ExtractIntNumType (rhstp));

    return mk_(wf_lhs && wf_rhs, MostGeneralNumericType(mk_set(lhsrestp, rhsrestp)));
  }
  else
    return mk_(Bool(false), ExpectedLhsAndRhsType);
}

// wf_NUMMOD
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_NUMMOD (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType (btp_int);

  TYPE_REP_TypeRep intertp (IntersectTypeReps(ExpectedLhsAndRhsType, exptp));

  Tuple inferlhs (wf_Expr(i, lhs, intertp));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);

  if (!lhscomp)
  {
    //------------------------------------------
    // Error message #162
    // Lhs of 'mod' is not an integer number type
    //------------------------------------------
    GenErrTp ( binexpr, ERR, 162, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp)
  {
    //------------------------------------------
    // Error message #163
    // Rhs of 'mod' is not an integer number type
    //------------------------------------------
    GenErrTp ( binexpr, ERR, 163, rhstp, ExpectedLhsAndRhsType, Sequence());
  }
  else if ((rhstp != btp_natone) && i == DEF)
  {
    //------------------------------
    // Error message #164
    // Rhs of 'mod' must be non zero
    //------------------------------
    GenErrTp (binexpr, PRF, 164, rhstp, btp_natone, Sequence());
  }

  if (lhscomp && rhscomp)
    return mk_(wf_lhs && wf_rhs, ExpectedLhsAndRhsType);
  else
    return mk_(Bool(false), ExpectedLhsAndRhsType);
}

// wf_INTDIV
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_INTDIV (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType (btp_int);

  TYPE_REP_TypeRep intertp (IntersectTypeReps(ExpectedLhsAndRhsType, exptp));

  Tuple inferlhs (wf_Expr(i, lhs, intertp));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);

  if (!lhscomp)
  {
    //------------------------------------
    // Error message #165
    // Lhs of 'div' is not an integer type
    //------------------------------------
    GenErrTp (binexpr, ERR, 165, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp)
  {
    //------------------------------------
    // Error message #166
    // Rhs of 'div' is not an integer type
    //------------------------------------
    GenErrTp (binexpr, ERR, 166, rhstp, ExpectedLhsAndRhsType, Sequence());
  }
  else if ((rhstp != btp_natone) && (i == DEF) && (Settings.ErrorLevel() >= PRF))
  {
    //------------------------------
    // Error message #167
    // Rhs of 'div' must be non zero
    //------------------------------
    GenErrTp (binexpr, PRF, 167, rhstp, btp_natone, Sequence());
  }

  if (lhscomp && rhscomp && (i == POS))
  {
    TYPE_REP_TypeRep lhsrestp (ExtractIntNumType (lhstp));
    TYPE_REP_TypeRep rhsrestp (ExtractIntNumType (rhstp));

    return mk_(wf_lhs && wf_rhs, MostGeneralNumericType(mk_set(lhsrestp, rhsrestp)));
  }
  else
  {
    if (!(rhstp == btp_natone))
      return mk_(Bool(false), ExpectedLhsAndRhsType);
    else
    {
// 20140726 -->
      if (lhscomp) {
        TYPE_REP_TypeRep lhsrestp (ExtractIntNumType (lhstp));
        if ((lhsrestp == btp_natone) || (lhsrestp == btp_nat))
          return mk_(wf_lhs && wf_rhs && lhscomp && rhscomp, btp_nat);
      }
// <-- 20140726
      return mk_(wf_lhs && wf_rhs && lhscomp && rhscomp, ExpectedLhsAndRhsType);
    }
  }
}

// wf_NUMEXP
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_NUMEXP (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsType (ExpectedNUMEXPType());
  TYPE_REP_TypeRep ExpectedRhsType (btp_real);

  TYPE_REP_TypeRep interlhstp (IntersectTypeReps(ExpectedLhsType, exptp));

  Tuple inferlhs (wf_Expr(i, lhs, interlhstp));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

// 20150620 -->
  if (!IsCompatible (POS, btp_real, lhstp))
    ExpectedRhsType = btp_nat;
// <-- 20150620

  Tuple inferrhs (wf_Expr(i, rhs, ExpectedRhsType));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedRhsType);

  if (!lhscomp)
  {
    //-----------------------------------------------------------------
    // Error message #168
    // Lhs of '**' is neither a numeric type, map type or function type
    //-----------------------------------------------------------------
    GenErrTp (lhs, ERR, 168, lhstp, ExpectedLhsType, Sequence());
  }

  if (!rhscomp)
  {
    //----------------------------------
    // Error message #169
    // Rhs of '**' is not a numeric type
    //----------------------------------
    GenErrTp (rhs, ERR, 169, rhstp, ExpectedRhsType, Sequence());
  }

  Tuple tmp (EXPType(i,lhstp,rhstp,lhscomp, rhs));
  Bool wf_comp (tmp.GetBool (1));
  TYPE_REP_TypeRep tp (tmp.GetRecord (2));

  return mk_(wf_comp && lhscomp && rhscomp && wf_lhs && wf_rhs, tp);
}

// EXPType
// i : TYPE`Ind
// lhstp : REP`TypeRep
// rhstp : REP`TypeRep
// lhswf : bool
// rhs : AS`Expr
// ==> bool * REP`TypeRep
Tuple StatSem::EXPType(const Int & i,
                            const TYPE_REP_TypeRep & lhstp,
                            const TYPE_REP_TypeRep & rhstp,
                            const Bool & lhswf,
                            const TYPE_AS_Expr & rhs)
{
  if (lhswf)
  {
    Generic maptp (ExtractMapType(lhstp));
    Generic fntp (ExtractFunType(lhstp));
    TYPE_REP_TypeRep numtp (ExtractNumericType(lhstp));

    Set ress (mk_set(maptp, fntp, numtp).Diff(mk_set(Nil(), rep_alltp)));

    Bool reswf (true);
    if (numtp.Is (TAG_TYPE_REP_AllTypeRep))
    {
      reswf = IsCompatible(i, rhstp, btp_nat);
      if (!reswf)
      {
        //-------------------------------------
        // Error message #170
        // Rhs of L"%1" must be a natural number
        //-------------------------------------
        GenErrTp(rhs, ERR, 170, rhstp, btp_nat, mk_sequence(SEQ<Char>(L"function and map iteration")));
      }

      if ((i == DEF) && !RngSubTypeDom(ress))
      {
        //-----------------------------------------------------------------
        // Error message #171
        // If Rhs of L"%1" is larger than 2 range must be a subset of domain
        //-----------------------------------------------------------------
        GenErr(rhs, ERR, 171, mk_sequence(SEQ<Char>(L"function and map iteration")));
        reswf = Bool (false);
      }
    }
    else if ( rhstp.Is(TAG_TYPE_REP_AllTypeRep) )
      return mk_(Bool (false), ExpectedNUMEXPType());
    else
    {
      ress.Insert(rhstp);
      ress = MergeNumericTypes(ress);
      if ((i == DEF) && (!maptp.IsNil () || !fntp.IsNil ()) && !RngSubTypeDom(ress))
      {
        //-----------------------------------------------------------------
        // Error message #171
        // If Rhs of L"%1" is larger than 2 range must be a subset of domain
        //-----------------------------------------------------------------
        GenErr(rhs, ERR, 171, mk_sequence(SEQ<Char>(L"function and map iteration")));
        reswf = Bool (false);
      }
    }
    switch (ress.Card ()) {
      case 1:
        return mk_(reswf, ress.GetElem ());
      case 2: {
        Generic t1 (ress.GetElem ());
        ress.RemElem (t1);
        Generic t2 (ress.GetElem ());
        return mk_(reswf, MergeTypes(t1, t2));
      }
      case 3: {
        Generic t1 (ress.GetElem ());
        ress.RemElem (t1);
        Generic t2 (ress.GetElem ());
        ress.RemElem (t2);
        return mk_(reswf, MergeTypes(t1, MergeTypes(t2, ress.GetElem ())));
      }
      default: {
        InternalError(L"EXPType");
        return Tuple (0); // to avoid warnings
      }
    }
  }
  else
    return mk_(Bool (false), ExpectedNUMEXPType());
}

// ExpectedNUMEXPType
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::ExpectedNUMEXPType ()
{
  if (!this->exp_defined)
  {
    this->exp_defined = true;

    SEQ<TYPE_REP_TypeRep> atr_l;
    atr_l.ImpAppend (rep_alltp);

    this->expNUMEXPType = mk_REP_UnionTypeRep(mk_set(mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp),
                                                     btp_real,
                                                     mk_REP_GeneralMapTypeRep(rep_alltp, rep_alltp),
                                                     mk_REP_InjectiveMapTypeRep(rep_alltp, rep_alltp),
                                                     mk_REP_TotalFnTypeRep(atr_l, rep_alltp),
                                                     mk_REP_PartialFnTypeRep(atr_l, rep_alltp)));
  }
  return this->expNUMEXPType;
}

// wf_NUMLT
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_NUMLT (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType (btp_real);

  Tuple inferlhs (wf_Expr(i, lhs, ExpectedLhsAndRhsType));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, ExpectedLhsAndRhsType));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  //bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType) || HasOrderFn(i, lhstp);
  //bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType) || HasOrderFn(i, rhstp);

  if (!lhscomp) {
    //---------------------------------
    // Error message #172
    // Lhs of '<' is not a numeric type
    //---------------------------------
    GenErrTp (binexpr, ERR, 172, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp) {
    //---------------------------------
    // Error message #173
    // Rhs of '<' is not a numeric type
    //---------------------------------
    GenErrTp (binexpr, ERR, 173, rhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (lhscomp && rhscomp) {
    return mk_(wf_lhs && wf_rhs, btp_bool);
  }
  else {
    return mk_(Bool(false), btp_bool);
  }
}

// wf_NUMLE
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_NUMLE (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType (btp_real);

  Tuple inferlhs (wf_Expr(i, lhs, ExpectedLhsAndRhsType));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, ExpectedLhsAndRhsType));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  //bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType) || HasOrderFn(i, lhstp);
  //bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType) || HasOrderFn(i, rhstp);

  if (!lhscomp) {
    //----------------------------------
    // Error message #174
    // Lhs of '<=' is not a numeric type
    //----------------------------------
    GenErrTp (binexpr, ERR, 174, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp) {
    //----------------------------------
    // Error message #175
    // Rhs of '<=' is not a numeric type
    //----------------------------------
    GenErrTp (binexpr, ERR, 175, rhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (lhscomp && rhscomp) {
    return mk_(wf_lhs && wf_rhs, btp_bool);
  }
  else {
    return mk_(Bool(false), btp_bool);
  }
}

// wf_NUMGT
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_NUMGT (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType (btp_real);

  Tuple inferlhs (wf_Expr(i, lhs, ExpectedLhsAndRhsType));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, ExpectedLhsAndRhsType));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  //bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType) || HasOrderFn(i, lhstp);
  //bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType) || HasOrderFn(i, rhstp);

  if (!lhscomp) {
    //---------------------------------
    // Error message #176
    // Lhs of '>' is not a numeric type
    //---------------------------------
    GenErrTp (binexpr, ERR, 176, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp) {
    //---------------------------------
    // Error message #177
    // Rhs of '>' is not a numeric type
    //---------------------------------
    GenErrTp (binexpr, ERR, 177, rhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (lhscomp && rhscomp) {
    return mk_(wf_lhs && wf_rhs, btp_bool);
  }
  else {
    return mk_(Bool(false), btp_bool);
  }
}

// wf_NUMGE
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_NUMGE (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType (btp_real);

  Tuple inferlhs (wf_Expr(i, lhs, ExpectedLhsAndRhsType));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, ExpectedLhsAndRhsType));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  //bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType) || HasOrderFn(i, lhstp);
  //bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType) || HasOrderFn(i, rhstp);

  if (!lhscomp) {
    //----------------------------------
    // Error message #178
    // Lhs of '>=' is not a numeric type
    //----------------------------------
    GenErrTp (binexpr, ERR, 178, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp) {
    //----------------------------------
    // Error message #179
    // Rhs of '>=' is not a numeric type
    //----------------------------------
    GenErrTp (binexpr, ERR, 179, rhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (lhscomp && rhscomp) {
    return mk_(wf_lhs && wf_rhs, btp_bool);
  }
  else {
    return mk_(Bool(false), btp_bool);
  }
}

// wf_AND
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_AND (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType (btp_bool);

  Tuple inferlhs (wf_Expr(i, lhs, ExpectedLhsAndRhsType));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, ExpectedLhsAndRhsType));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);

  if (!lhscomp) {
    //-----------------------------------
    // Error message #180
    // Lhs of 'and' is not a boolean type
    //-----------------------------------
    GenErrTp (binexpr, ERR, 180, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp) {
    //-----------------------------------
    // Error message #181
    // Rhs of 'and' is not a boolean type
    //-----------------------------------
    GenErrTp (binexpr, ERR, 181, rhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (lhscomp && rhscomp) {
    return mk_(wf_lhs && wf_rhs, ExpectedLhsAndRhsType);
  }
  else {
    return mk_(Bool(false), ExpectedLhsAndRhsType);
  }
}

// wf_OR
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_OR (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType (btp_bool);

  Tuple inferlhs (wf_Expr(i, lhs, ExpectedLhsAndRhsType));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, ExpectedLhsAndRhsType));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);

  if (!lhscomp) {
    //----------------------------------
    // Error message #182
    // Lhs of 'or' is not a boolean type
    //----------------------------------
    GenErrTp (binexpr, ERR, 182, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp) {
    //----------------------------------
    // Error message #183
    // Rhs of 'or' is not a boolean type
    //----------------------------------
    GenErrTp (binexpr, ERR, 183, rhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (lhscomp && rhscomp) {
    return mk_(wf_lhs && wf_rhs, ExpectedLhsAndRhsType);
  }
  else {
    return mk_(Bool(false), ExpectedLhsAndRhsType);
  }
}

// wf_IMPLY
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_IMPLY (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType (btp_bool);

  Tuple inferlhs (wf_Expr(i, lhs, ExpectedLhsAndRhsType));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, ExpectedLhsAndRhsType));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);

  if (!lhscomp) {
    //----------------------------------
    // Error message #184
    // Lhs of '=>' is not a boolean type
    //----------------------------------
    GenErrTp (binexpr, ERR, 184, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp) {
    //----------------------------------
    // Error message #185
    // Rhs of '=>' is not a boolean type
    //----------------------------------
    GenErrTp (binexpr, ERR, 185, rhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (lhscomp && rhscomp) {
    return mk_(wf_lhs && wf_rhs, ExpectedLhsAndRhsType);
  }
  else {
    return mk_(Bool(false), ExpectedLhsAndRhsType);
  }
}

// wf_EQUIV
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_EQUIV (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType (btp_bool);

  Tuple inferlhs (wf_Expr(i, lhs, ExpectedLhsAndRhsType));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, ExpectedLhsAndRhsType));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);

  if (!lhscomp) {
    //-----------------------------------
    // Error message #186
    // Lhs of '<=>' is not a boolean type
    //-----------------------------------
    GenErrTp (binexpr, ERR, 186, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp) {
    //-----------------------------------
    // Error message #187
    // Rhs of '<=>' is not a boolean type
    //-----------------------------------
    GenErrTp (binexpr, ERR, 187, rhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (lhscomp && rhscomp) {
    return mk_(wf_lhs && wf_rhs, ExpectedLhsAndRhsType);
  }
  else {
    return mk_(Bool(false), ExpectedLhsAndRhsType);
  }
}

// wf_EQ
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_EQ (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  Tuple inferlhs (wf_Expr(i, lhs, rep_alltp));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, rep_alltp));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool iscomp = IsCompatible (Int(POS), lhstp, rhstp);

  if (!iscomp) {
    //-----------------------------------
    // Error message #188
    // This equality will always be false
    //-----------------------------------
    GenErrTp (binexpr, ERR, 188, lhstp, rhstp, Sequence());
  }

  return mk_(wf_lhs && wf_rhs && iscomp, btp_bool);
}

// wf_NE
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_NE (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  Tuple inferlhs (wf_Expr(i, lhs, rep_alltp));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & tp_lhs (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, rep_alltp));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & tp_rhs (inferrhs.GetRecord(2));

  bool iscomp = IsCompatible (Int(POS), tp_lhs, tp_rhs);

  if (!iscomp)
  {
    //------------------------------------
    // Error message #189
    // This inequality will always be true
    //------------------------------------
    GenErrTp (binexpr, ERR, 189, tp_lhs, tp_rhs, Sequence());
  }

  return mk_(wf_lhs && wf_rhs && iscomp, btp_bool);
}

// wf_SETUNION
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SETUNION (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType(set_alltp);
  TYPE_REP_TypeRep intertp (IntersectTypeReps (ExpectedLhsAndRhsType, exptp));

  Tuple inferlhs (wf_Expr(i, lhs, intertp));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);

  if (!lhscomp)
  {
    //---------------------------------
    // Error message #190
    // Lhs of 'union' is not a set type
    //---------------------------------
    GenErrTp (binexpr, ERR, 190, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp)
  {
    //---------------------------------
    // Error message #191
    // Rhs of 'union' is not a set type
    //---------------------------------
    GenErrTp (binexpr, ERR, 191, rhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (lhscomp && rhscomp)
  {
    TYPE_REP_TypeRep lhsrestp (ExtractSetType (lhstp));
    TYPE_REP_TypeRep rhsrestp (ExtractSetType (rhstp));

// 20101013 -->
    if (i == POS)
    {
      if (!IsCompatible(i, lhsrestp, rhsrestp) &&
          IsCompatible(i, SetTypeUnion(mk_set(lhsrestp, rhsrestp)), intertp))
      {

        if (!lhsrestp.Is(TAG_TYPE_REP_EmptySetTypeRep) && !IsCompatible(i, lhsrestp, intertp))
        {
          //----------------------------------
          // Error message #443
          // Lhs of 'union' is not a correct type
          //----------------------------------
          GenErrTp (lhs, ERR, 443, lhstp, exptp, Sequence());
        }
        if (!rhsrestp.Is(TAG_TYPE_REP_EmptySetTypeRep) && !IsCompatible(i, rhsrestp, intertp))
        {
          //----------------------------------
          // Error message #444
          // Rhs of 'union' is not a correct type
          //----------------------------------
          GenErrTp (rhs, ERR, 444, rhstp, exptp, Sequence());
        }
      }
    }
// <-- 20101013
    return mk_(wf_lhs && wf_rhs, SetTypeUnion(mk_set(lhsrestp, rhsrestp)));
  }
  else
    return mk_(Bool(false), ExpectedLhsAndRhsType);
}

// wf_SETINTERSECT
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SETINTERSECT (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType(set_alltp);
  TYPE_REP_TypeRep intertp (IntersectTypeReps (ExpectedLhsAndRhsType, exptp));

  Tuple inferlhs (wf_Expr(i, lhs, ExpectedLhsAndRhsType));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, ExpectedLhsAndRhsType));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);

  if (!lhscomp)
  {
    //---------------------------------
    // Error message #192
    // Lhs of 'inter' is not a set type
    //---------------------------------
    GenErrTp (binexpr, ERR, 192, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp)
  {
    //---------------------------------
    // Error message #193
    // Rhs of 'inter' is not a set type
    //---------------------------------
    GenErrTp (binexpr, ERR, 193, rhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!IsCompatible (POS, lhstp, rhstp))
  {
    //-----------------------------------------------------------
    // Error message #194
    // This set intersection will always be equal to an empty set
    //-----------------------------------------------------------
    GenErrTp (binexpr, ERR, 194, lhstp, rhstp, Sequence());
    return mk_(Bool(false), mk_REP_EmptySetTypeRep(rep_alltp));
  }

  if (lhscomp && rhscomp)
  {
    TYPE_REP_TypeRep lhsrestp (ExtractSetType (lhstp));
    TYPE_REP_TypeRep rhsrestp (ExtractSetType (rhstp));
    TYPE_REP_TypeRep tp (SetTypeInter(mk_set(lhsrestp, rhsrestp)));
    if (tp.Is(TAG_TYPE_REP_EmptySetTypeRep))
    {
      //-----------------------------------------------------------
      // Error message #194
      // This set intersection will always be equal to an empty set
      //-----------------------------------------------------------
      GenErr (binexpr, WRN2, 194, Sequence());
    }

    switch(tp.GetTag()) {
      case TAG_TYPE_REP_EmptySetTypeRep: {
        return mk_(wf_lhs && wf_rhs, tp);
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        SET<TYPE_REP_TypeRep> utps (tp.GetSet(pos_REP_UnionTypeRep_tps));
        utps.Insert(mk_REP_EmptySetTypeRep(rep_alltp));
        return mk_(wf_lhs && wf_rhs, mk_REP_UnionTypeRep( utps ));
      }
      default: {
        return mk_(wf_lhs && wf_rhs, mk_REP_UnionTypeRep(mk_set(tp, mk_REP_EmptySetTypeRep(rep_alltp))));
      }
    }
  }
  else
    return mk_(Bool(false), ExpectedLhsAndRhsType);
}

// wf_SETMINUS
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SETMINUS (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType(set_alltp);

  TYPE_REP_TypeRep intertp (IntersectTypeReps(ExpectedLhsAndRhsType, exptp));

  Tuple inferlhs (wf_Expr(i, lhs, ExpectedLhsAndRhsType));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, ExpectedLhsAndRhsType));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);

  if (!lhscomp)
  {
    //-------------------------------------
    // Error message #195
    // Lhs of 'set minus' is not a set type
    //-------------------------------------
    GenErrTp (binexpr, ERR, 195, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp)
  {
    //-------------------------------------
    // Error message #196
    // Rhs of 'set minus' is not a set type
    //-------------------------------------
    GenErrTp (binexpr, ERR, 196, rhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!IsCompatible (POS, lhstp, rhstp))
  {
    //----------------------------------------
    // Error message #197
    // Set minus operation removes no elements
    //----------------------------------------
    GenErrTp (binexpr, ERR, 197 ,lhstp, rhstp, Sequence());

    if (lhscomp)
      return mk_(wf_lhs && wf_rhs && lhscomp && rhscomp, lhstp);
    else
      return mk_(wf_lhs && wf_rhs && lhscomp && rhscomp, ExpectedLhsAndRhsType);
  }

  if (lhscomp && rhscomp)
  {
    TYPE_REP_TypeRep lhsrestp (ExtractSetType (lhstp));
    TYPE_REP_TypeRep rhsrestp (ExtractSetType (rhstp));

    Generic tp (SetTypeMinus(lhsrestp, rhsrestp));
    if (tp.IsNil())
      return mk_(wf_lhs && wf_rhs, lhsrestp);
    else
      return mk_(wf_lhs && wf_rhs, tp);
  }
  else
    return mk_(Bool(false), ExpectedLhsAndRhsType);
}

// wf_SUBSET
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SUBSET (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType(set_alltp);

  Tuple inferlhs (wf_Expr(i, lhs, ExpectedLhsAndRhsType));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, ExpectedLhsAndRhsType));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);

  // combine = lhstp <> mk_REP`EmptySetTypeRep() =>
  //           IsCompatible(<POS>,RemoveEmptySetType(lhstp),rhstp) in
  //bool combine = lhstp.Is(TAG_TYPE_REP_EmptySetTypeRep) || IsCompatible(POS, RemoveEmptySetType(lhstp), rhstp);
  bool combine = lhstp.Is(TAG_TYPE_REP_EmptySetTypeRep) || IsCompatible(POS, lhstp, rhstp);

  if (!lhscomp)
  {
    //----------------------------------
    // Error message #198
    // Lhs of 'subset' is not a set type
    //----------------------------------
    GenErrTp (binexpr, ERR, 198, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp)
  {
    //----------------------------------
    // Error message #199
    // Rhs of 'subset' is not a set type
    //----------------------------------
    GenErrTp (binexpr, ERR, 199, rhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (lhscomp && rhscomp && !combine)
  {
    //---------------------------------------------------
    //-- Error message #392
    //-- subset will only be true if the Lhs set is empty
    //---------------------------------------------------
    GenErrTp (binexpr, ERR, 392, lhstp, rhstp, Sequence());
  }

  if (lhscomp && rhscomp && combine)
    return mk_(wf_lhs && wf_rhs, btp_bool);
  else
    return mk_(Bool(false), btp_bool);
}

// wf_PROPERSUBSET
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_PROPERSUBSET (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType(set_alltp);
  TYPE_REP_TypeRep intertp(IntersectTypeReps(ExpectedLhsAndRhsType, exptp));

  Tuple inferlhs (wf_Expr(i, lhs, ExpectedLhsAndRhsType));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, ExpectedLhsAndRhsType));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = (IsCompatible (i, lhstp, ExpectedLhsAndRhsType));
  bool rhscomp = (IsCompatible (i, rhstp, ExpectedLhsAndRhsType));

  // combine = lhstp <> mk_REP`EmptySetTypeRep() =>
  //           IsCompatible(<POS>,RemoveEmptySetType(lhstp),rhstp) in
//  bool combine = lhstp.Is(TAG_TYPE_REP_EmptySetTypeRep) || IsCompatible(POS, RemoveEmptySetType(lhstp), rhstp);
  bool combine = lhstp.Is(TAG_TYPE_REP_EmptySetTypeRep) || IsCompatible(POS, lhstp, rhstp);

  if (!lhscomp)
  {
    //-----------------------------------
    // Error message #200
    // Lhs of 'psubset' is not a set type
    //-----------------------------------
    GenErrTp (binexpr, ERR, 200, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp)
  {
    //------------------------------------
    // Error message #201
    // Rhs of 'psubset' is not a set type
    //------------------------------------
    GenErrTp (binexpr, ERR, 201, rhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (lhscomp && rhscomp && !combine)
  {
    //---------------------------------------------------
    //-- Error message #392
    //-- subset will only be true if the Lhs set is empty
    // ---------------------------------------------------
    GenErrTp (binexpr, ERR, 392, lhstp, rhstp, Sequence());
  }

  if (lhscomp && rhscomp && combine)
    return mk_(wf_lhs && wf_rhs, btp_bool);
  else
    return mk_(Bool(false), btp_bool);
}

// wf_INSET
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_INSET (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedRhsType(set_alltp);
  TYPE_REP_TypeRep ExpectedLhsType(rep_alltp);

  Tuple inferlhs (wf_Expr(i, lhs, ExpectedLhsType));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, ExpectedRhsType));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool rhscomp = IsCompatible (i, rhstp, ExpectedRhsType);

  if (!rhscomp)
  {
    //----------------------------------
    // Error message #202
    // Rhs of 'in set' is not a set type
    //----------------------------------
    GenErrTp (binexpr, ERR, 202, rhstp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), btp_bool);
  }
  else
  {
    TYPE_REP_TypeRep lhstp_s (mk_REP_SetTypeRep(lhstp));
    bool iscomp = (rhstp.Is(TAG_TYPE_REP_EmptySetTypeRep) ? false :
                                                          IsCompatible (POS, lhstp_s, RemoveEmptySetType(rhstp)));

    if(!iscomp)
    {
      //----------------------------------------------
      // Error message #203
      // This membership check will always yield false
      //----------------------------------------------
      GenErrTp (binexpr, ERR, 203, lhstp_s, rhstp, Sequence());
    }

    return mk_(wf_lhs && wf_rhs && iscomp, btp_bool);
  }
}

// wf_NOTINSET
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_NOTINSET (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedRhsType(set_alltp);
  TYPE_REP_TypeRep ExpectedLhsType(rep_alltp);

  Tuple inferlhs (wf_Expr(i, lhs, ExpectedLhsType));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, ExpectedRhsType));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool rhscomp = IsCompatible (i, rhstp, ExpectedRhsType);

  if (!rhscomp)
  {
    //--------------------------------------
    // Error message #204
    // Rhs of 'not in set' is not a set type
    //--------------------------------------
    GenErrTp (binexpr, ERR, 204, rhstp, ExpectedRhsType, Sequence());
    return mk_(Bool(false), btp_bool);
  }
  else
  {
    TYPE_REP_TypeRep lhstp_s (mk_REP_SetTypeRep( lhstp ));
    bool iscomp = (rhstp.Is(TAG_TYPE_REP_EmptySetTypeRep) ? false
                                                          : IsCompatible (POS, lhstp_s, RemoveEmptySetType(rhstp)));

    if (!(iscomp))
      //-------------------------------------------------
      // Error message #205
      // This not-membership check will always yield true
      //-------------------------------------------------
      GenErrTp (binexpr, ERR, 205, lhstp_s, rhstp, Sequence());

    return mk_(wf_lhs && wf_rhs && iscomp, btp_bool);
  }
}

// wf_SEQCONC
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_SEQCONC (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  TYPE_REP_TypeRep ExpectedLhsAndRhsType(seq_alltp);
  TYPE_REP_TypeRep intertp(IntersectTypeReps(ExpectedLhsAndRhsType, exptp));

  Tuple inferlhs (wf_Expr(i, lhs, intertp));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);

  if (!lhscomp)
  {
    //----------------------------------
    // Error message #206
    // Lhs of '^' is not a sequence type
    //----------------------------------
    GenErrTp (binexpr, ERR, 206, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp)
  {
    //----------------------------------
    // Error message #207
    // Rhs of '^' is not a sequence type
    //----------------------------------
    GenErrTp (binexpr, ERR, 207, rhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (lhscomp && rhscomp)
  {
    TYPE_REP_TypeRep lhsrestp (ExtractSeqType (lhstp));
    TYPE_REP_TypeRep rhsrestp (ExtractSeqType (rhstp));
// 20101013 -->
    if (i == POS)
    {
      if (!IsCompatible(i, lhsrestp, rhsrestp) &&
          IsCompatible(i, SeqTypeConc(mk_set(lhsrestp, rhsrestp)), intertp))
      {
        if (!lhsrestp.Is(TAG_TYPE_REP_EmptySeqTypeRep) && !IsCompatible(i, lhsrestp, intertp))
        {
          //----------------------------------
          // Error message #441
          // Lhs of '^' is not a correct type
          //----------------------------------
          GenErrTp (lhs, ERR, 441, lhstp, exptp, Sequence());
        }
        if (!rhsrestp.Is(TAG_TYPE_REP_EmptySeqTypeRep) && !IsCompatible(i, rhsrestp, intertp)) 
        {
          //----------------------------------
          // Error message #442
          // Rhs of '^' is not a correct type
          //----------------------------------
          GenErrTp (rhs, ERR, 442, rhstp, exptp, Sequence());
        }
      }
    }
// <-- 20101013
    return mk_(wf_lhs && wf_rhs, SeqTypeConc( mk_set(lhsrestp, rhsrestp) ));
  }
  else
    return mk_(Bool(false), ExpectedLhsAndRhsType);
}

// wf_MAPMERGE
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_MAPMERGE (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  //TYPE_REP_TypeRep ExpectedLhsAndRhsType (mk_REP_GeneralMapTypeRep(rep_alltp,rep_alltp));
  TYPE_REP_TypeRep ExpectedLhsAndRhsType (map_alltp);

  TYPE_REP_TypeRep intertp (IntersectTypeReps(ExpectedLhsAndRhsType, exptp));

  Tuple inferlhs (wf_Expr(i, lhs, intertp));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, intertp));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsAndRhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedLhsAndRhsType);

  if (!lhscomp)
  {
    //----------------------------------
    // Error message #208
    // Lhs of 'munion' is not a map type
    //----------------------------------
    GenErrTp (binexpr, ERR, 208, lhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (!rhscomp)
  {
    //----------------------------------
    // Error message #209
    // Rhs of 'munion' is not a map type
    //----------------------------------
    GenErrTp (binexpr, ERR, 209, rhstp, ExpectedLhsAndRhsType, Sequence());
  }

  if (lhscomp && rhscomp)
  {
    TYPE_REP_TypeRep lhsrestp (ExtractMapType (lhstp));
    TYPE_REP_TypeRep rhsrestp (ExtractMapType (rhstp));

    return mk_(wf_lhs && wf_rhs, MapTypeMerge(mk_set(lhsrestp, lhsrestp)));
  }
  else
    return mk_(Bool(false), ExpectedLhsAndRhsType);
}

// wf_MAPDOMRESTTO
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_MAPDOMRESTTO (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  //TYPE_REP_TypeRep ExpectedRhsType (mk_REP_GeneralMapTypeRep(rep_alltp, rep_alltp));
  TYPE_REP_TypeRep ExpectedRhsType (map_alltp);
  TYPE_REP_TypeRep ExpectedLhsType (set_alltp);

  Generic expdomtp (rep_alltp);
  if (IsCompatible(i, ExpectedRhsType, exptp))
  {
    Tuple tmp (SplitMapType(ExtractMapType(exptp)));
    expdomtp = tmp.GetField(1);
  }

  TYPE_REP_TypeRep interrhstp(IntersectTypeReps(ExpectedRhsType, exptp));
  TYPE_REP_TypeRep interlhstp(IntersectTypeReps(ExpectedLhsType, expdomtp));

  Tuple inferlhs (wf_Expr(i, lhs, interlhstp));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, interrhstp));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedRhsType);

  if (!lhscomp)
  {
    //------------------------------
    // Error message #210
    // Lhs of '<:' is not a set type
    //------------------------------
    GenErrTp (binexpr, ERR, 210, lhstp, ExpectedLhsType, Sequence());
  }

  if (!rhscomp)
  {
    //------------------------------
    // Error message #211
    // Rhs of '<:' is not a map type
    //------------------------------
    GenErrTp (binexpr, ERR, 211, rhstp, ExpectedRhsType, Sequence());
  }

  if (lhscomp && rhscomp)
  {
    TYPE_REP_TypeRep lhsrestp (ExtractSetType (lhstp));
    TYPE_REP_TypeRep rhsrestp (ExtractMapType (rhstp));

    Tuple infer (MapTypeDomRestrict (lhsrestp, rhsrestp));
    const TYPE_REP_TypeRep & restp (infer.GetRecord (1));
    int overlap = infer.GetIntValue (2);

    switch(overlap) {
      case OK: {
        return mk_(wf_lhs && wf_rhs, restp);
      }
      case EMPTYMAP: {
        //-------------------------------
        // Error message #212
        // An empty map is used with '<:'
        //-------------------------------
        GenErr (binexpr, WRN1, 212, Sequence());
        return mk_(wf_lhs && wf_rhs, rhsrestp);
      }
      default: { // NOTOVERLAP
        //----------------------------------------
        // Error message #213
        // The '<:' will always yield an empty map
        //----------------------------------------
        GenErr (binexpr, ERR, 213, Sequence());
        return mk_(Bool(false), mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp));
      }
    }
  }
  else
    return mk_(Bool(false), ExpectedRhsType);
}

// wf_MAPDOMRESTBY
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_MAPDOMRESTBY (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

//  TYPE_REP_TypeRep ExpectedRhsType (mk_REP_GeneralMapTypeRep(rep_alltp,rep_alltp));
  TYPE_REP_TypeRep ExpectedRhsType (map_alltp);
  TYPE_REP_TypeRep ExpectedLhsType (set_alltp);

  Generic expdomtp (rep_alltp);
  if (IsCompatible(i, ExpectedRhsType, exptp))
  {
    Tuple tmp (SplitMapType(ExtractMapType(exptp)));
    expdomtp = tmp.GetField(1);
  }

  TYPE_REP_TypeRep interlhstp(IntersectTypeReps(ExpectedLhsType, expdomtp));
  TYPE_REP_TypeRep interrhstp(IntersectTypeReps(ExpectedRhsType, exptp));

  Tuple inferlhs (wf_Expr(i, lhs, interlhstp));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, interrhstp));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = IsCompatible (i, lhstp, ExpectedLhsType);
  bool rhscomp = IsCompatible (i, rhstp, ExpectedRhsType);

  if (!lhscomp)
  {
    //-------------------------------
    // Error message #214
    // Lhs of '<-:' is not a set type
    //-------------------------------
    GenErrTp (binexpr, ERR, 214, lhstp, ExpectedLhsType, Sequence());
  }

  if (!rhscomp)
  {
    //-------------------------------
    // Error message #215
    // Rhs of '<-:' is not a map type
    //-------------------------------
    GenErrTp (binexpr, ERR, 215, rhstp, ExpectedRhsType, Sequence());
  }

  if (lhscomp && rhscomp)
  {
    TYPE_REP_TypeRep lhsrestp (ExtractSetType (lhstp));
    TYPE_REP_TypeRep rhsrestp (ExtractMapType (rhstp));

    Tuple infer (MapTypeDomRestrict (lhsrestp, rhsrestp));
    const TYPE_REP_TypeRep & restp (infer.GetRecord (1));
    int overlap = infer.GetIntValue (2);

    switch(overlap) {
      case OK: {
        return mk_(wf_lhs && wf_rhs, restp);
      }
      case EMPTYMAP: {
        //--------------------------------
        // Error message #216
        // An empty map is used with '<-:'
        //--------------------------------
        GenErr (binexpr, WRN1, 216, Sequence());
        return mk_(wf_lhs && wf_rhs, rhsrestp);
      }
      default: { // NOTOVERLAP
        //------------------------------------------------------
        // Error message #217
        // The '<-:' will never remove any elements from the map
        //------------------------------------------------------
        GenErr (binexpr, ERR, 217, Sequence());
        return mk_(Bool(false), rhsrestp);
      }
    }
  }
  else
    return mk_(Bool(false), ExpectedRhsType);
}

// wf_MAPRNGRESTTO
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_MAPRNGRESTTO (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  //TYPE_REP_TypeRep ExpectedLhsType (mk_REP_GeneralMapTypeRep(rep_alltp,rep_alltp));
  TYPE_REP_TypeRep ExpectedLhsType (map_alltp);
  TYPE_REP_TypeRep ExpectedRhsType (set_alltp);

  Generic exprngtp(rep_alltp);
  if (IsCompatible(i, ExpectedLhsType, exptp))
  {
    Tuple tmp(SplitMapType(ExtractMapType(exptp)));
    exprngtp = tmp.GetField(2);
  }

  TYPE_REP_TypeRep interlhstp(IntersectTypeReps(ExpectedLhsType, exptp));
  TYPE_REP_TypeRep interrhstp(IntersectTypeReps(ExpectedRhsType, exprngtp));

  Tuple inferlhs (wf_Expr(i, lhs, interlhstp));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, interrhstp));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = (IsCompatible (i, lhstp, ExpectedLhsType));
  bool rhscomp = (IsCompatible (i, rhstp, ExpectedRhsType));

  if (!lhscomp)
  {
    //------------------------------
    // Error message #218
    // Lhs of ':>' is not a map type
    //------------------------------
    GenErrTp (binexpr, ERR, 218, lhstp, ExpectedLhsType, Sequence());
  }

  if (!rhscomp)
  {
    //------------------------------
    // Error message #219
    // Rhs of ':>' is not a set type
    //------------------------------
    GenErrTp (binexpr, ERR, 219, rhstp, ExpectedRhsType, Sequence());
  }

  if (lhscomp && rhscomp)
  {
    TYPE_REP_TypeRep lhsrestp (ExtractMapType (lhstp));
    TYPE_REP_TypeRep rhsrestp (ExtractSetType (rhstp));

    Tuple infer (MapTypeRngRestrict (lhsrestp, rhsrestp));
    const TYPE_REP_TypeRep & restp (infer.GetRecord (1));
    int overlap = infer.GetIntValue (2);

    switch(overlap) {
      case OK: {
        return mk_(wf_lhs && wf_rhs, restp);
      }
      case EMPTYMAP: {
        //-------------------------------
        // Error message #220
        // An empty map is used with ':>'
        //-------------------------------
        GenErr (binexpr, WRN1, 220, Sequence());
        return mk_(wf_lhs && wf_rhs, rhsrestp);
      }
      default: { // NOTOVERLAP
        //----------------------------------------
        // Error message #221
        // The ':>' will always yield an empty map
        //----------------------------------------
        GenErr (binexpr, ERR, 221, Sequence());
        return mk_(Bool(false), mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp));
      }
    }
  }
  else
    return mk_(Bool(false), ExpectedLhsType);
}

// wf_MAPRNGRESTBY
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_MAPRNGRESTBY (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  //TYPE_REP_TypeRep ExpectedLhsType (mk_REP_GeneralMapTypeRep(rep_alltp,rep_alltp));
  TYPE_REP_TypeRep ExpectedLhsType (map_alltp);
  TYPE_REP_TypeRep ExpectedRhsType (set_alltp);

  Generic exprngtp (rep_alltp);
  if (IsCompatible(i, ExpectedLhsType, exptp))
  {
    Tuple tmp (SplitMapType(ExtractMapType(exptp)));
    exprngtp = tmp.GetField(2);
  }
  TYPE_REP_TypeRep interlhstp(IntersectTypeReps(ExpectedLhsType, exptp));
  TYPE_REP_TypeRep interrhstp(IntersectTypeReps(ExpectedRhsType, exprngtp));

  Tuple inferlhs (wf_Expr(i, lhs, interlhstp));
  const Bool & wf_lhs (inferlhs.GetBool(1));
  const TYPE_REP_TypeRep & lhstp (inferlhs.GetRecord(2));

  Tuple inferrhs (wf_Expr(i, rhs, interrhstp));
  const Bool & wf_rhs (inferrhs.GetBool(1));
  const TYPE_REP_TypeRep & rhstp (inferrhs.GetRecord(2));

  bool lhscomp = (IsCompatible (i, lhstp, ExpectedLhsType));
  bool rhscomp = (IsCompatible (i, rhstp, ExpectedRhsType));

  if (!lhscomp)
  {
    //-------------------------------
    // Error message #222
    // Lhs of ':->' is not a map type
    //-------------------------------
    GenErrTp (binexpr, ERR, 222, lhstp, ExpectedLhsType, Sequence());
  }

  if (!rhscomp)
  {
    //-------------------------------
    // Error message #223
    // Rhs of ':->' is not a set type
    //-------------------------------
    GenErrTp (binexpr, ERR, 223, rhstp, ExpectedRhsType, Sequence());
  }

  if (lhscomp && rhscomp)
  {
    TYPE_REP_TypeRep lhsrestp (ExtractMapType (lhstp));
    TYPE_REP_TypeRep rhsrestp (ExtractSetType (rhstp));

    Tuple infer (MapTypeRngRestrict (lhsrestp, rhsrestp));
    const TYPE_REP_TypeRep & restp (infer.GetRecord (1));
    int overlap = infer.GetIntValue (2);

    switch(overlap) {
      case OK: {
        return mk_(wf_lhs && wf_rhs, restp);
      }
      case EMPTYMAP: {
        //--------------------------------
        // Error message #224
        // An empty map is used with ':->'
        //--------------------------------
        GenErr (binexpr, WRN1, 224, Sequence());
        return mk_(wf_lhs && wf_rhs, lhsrestp);
      }
      default: { // NOTOVERLAP
        //------------------------------------------------------
        // Error message #225
        // The ':->' will never remove any elements from the map
        //------------------------------------------------------
        GenErr (binexpr, ERR, 225, Sequence());
        return mk_(Bool(false), lhsrestp);
      }
    }
  }
  else
    return mk_(Bool(false), ExpectedLhsType);
}

// wf_COMPOSE
// i : TYPE`Ind
// binexpr : AS`BinaryExpr
// exptp : REP`TypeRep
// ==> bool * REP`TypeRep
Tuple StatSem::wf_COMPOSE (const Int & i, const TYPE_AS_BinaryExpr & binexpr, const TYPE_REP_TypeRep & exptp)
{
  const TYPE_AS_Expr & lhs (binexpr.GetRecord(pos_AS_BinaryExpr_left));
  const TYPE_AS_Expr & rhs (binexpr.GetRecord(pos_AS_BinaryExpr_right));

  SEQ<TYPE_REP_TypeRep> alltp_l;
  alltp_l.ImpAppend (rep_alltp);

  TYPE_REP_TypeRep ExpectedType (mk_REP_UnionTypeRep(mk_set(mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp),
                                                            mk_REP_GeneralMapTypeRep(rep_alltp, rep_alltp),
                                                            mk_REP_InjectiveMapTypeRep(rep_alltp, rep_alltp),
                                                            mk_REP_TotalFnTypeRep(alltp_l, rep_alltp),
                                                            mk_REP_PartialFnTypeRep(alltp_l, rep_alltp))));

  TYPE_REP_TypeRep intertp (IntersectTypeReps(ExpectedType, exptp));

  Tuple infer (wf_Expr (i, lhs, intertp));
  const Bool & wf_lhs (infer.GetBool (1));
  const TYPE_REP_TypeRep & tplhs (infer.GetRecord (2));

  Tuple infer2 (wf_Expr (i, rhs, intertp));
  const Bool & wf_rhs (infer2.GetBool (1));
  const TYPE_REP_TypeRep & tprhs (infer2.GetRecord (2));

  bool lhscomp = (IsCompatible(i, tplhs, ExpectedType));
  bool rhscomp = (IsCompatible(i, tprhs, ExpectedType));

  if (!lhscomp)
  {
    //--------------------------------------------------------
    // Error message #226
    // Lhs of 'comp' is neither a map type nor a function type
    //--------------------------------------------------------
    GenErrTp (lhs, ERR, 226, tplhs, ExpectedType, Sequence());
  }

  if (!rhscomp)
  {
    //--------------------------------------------------------
    // Error message #227
    // Rhs of 'comp' is neither a map type nor a function type
    //--------------------------------------------------------
    GenErrTp (rhs, ERR, 227, tprhs, ExpectedType, Sequence());
  }

  Tuple infer3 (ExtractComposeType(i, tplhs, tprhs));
  const Bool & wf_comp (infer3.GetBool (1));
  const TYPE_REP_TypeRep & restp (infer3.GetRecord (2));

  if (!wf_comp && lhscomp && rhscomp)
  {
    //--------------------------------------------------------------------------
    // Error message #228
    // Rhs range type is not compatible with lhs domain type in composition
    // expression
    //--------------------------------------------------------------------------
    GenErrTp(binexpr, ERR, 228, infer3.GetRecord (3), infer3.GetRecord (4), Sequence());
  }

  return mk_(wf_comp && wf_lhs && wf_rhs && lhscomp && rhscomp, restp);
}

Tuple StatSem::wf_Macro (const Int & i, const TYPE_AS_Macro & vMacro, const TYPE_REP_TypeRep & exptp)
{
  const Int & tp (vMacro.GetInt(pos_AS_Macro_tp));
  const TYPE_CI_ContextId & cid (vMacro.GetInt(pos_AS_Macro_cid));

  switch (tp.GetValue()) {
    case MACRO_FILE:
    case MACRO_CLMOD:
    case MACRO_FNOP: {
      TYPE_REP_TypeRep restp (mk_REP_SeqTypeRep(btp_char));
      if (cid != NilContextId)
        GetCI().SetTypeInfo (cid, restp);
      return mk_(Bool(true), restp);
    }
    case MACRO_LINE:
    case MACRO_COLUMN: {
      if (cid != NilContextId)
        GetCI().SetTypeInfo (cid, btp_int);
      return mk_(Bool(true), btp_int);
    }
  }
  return Tuple(); // dummy
}

#endif //!NOSS

