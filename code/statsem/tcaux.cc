/***
*  * Id
*  *   $Id: aux.cc,v 1.66 2006/03/22 07:15:37 vdmtools Exp $
*  *
*  *   aux.cc: Implementation of tcaux.vdm 1.77
***/

#include "statsem.h"
#include "astaux.h"
#include "intconvquotes.h"

//wcout << L"intertp: " << INT2Q::h2gAS(intertp) << endl;

#ifndef NOSS

// TcauxLookUpTypeName
// nm :AS`Name
// -> [REP`TypeRep]
Generic StatSem::TcauxLookUpTypeName (const TYPE_AS_Name & nm)
{
#ifdef VDMPP
  Generic lok (LookUpTypeName (nm, true));
  if (IsAccessType (lok))
    return StripAccessType (lok);
  else
    return lok;
#endif // VDMPP
#ifdef VDMSL
  Generic lok (LookUpTypeName (nm));
  return lok;
#endif // VDMSL
}

// MergeNumericTypes
// sttp : set of REP`TypeRep
// -> set of REP`TypeRep
SET<TYPE_REP_TypeRep> StatSem::MergeNumericTypes (const SET<TYPE_REP_TypeRep> & sttp) const
{
  if (sttp.InSet(btp_real))
    return sttp.Diff(this->realSubTypeSet);
  else if (sttp.InSet(btp_rat))
    return sttp.Diff(this->ratSubTypeSet);
  else if (sttp.InSet(btp_int))
    return sttp.Diff(this->intSubTypeSet);
  else if (sttp.InSet(btp_nat))
    return sttp.Diff(this->natSubTypeSet);
  else
    return sttp;
}

// ExpandNumericTypes
// sttp : set of REP`TypeRep
// -> set of REP`TypeRep
SET<TYPE_REP_TypeRep> StatSem::ExpandNumericTypes (const SET<TYPE_REP_TypeRep> & sttp)
{
  if (sttp.InSet(btp_real))
    return sttp.Union(this->realSubTypeSet);
  else if (sttp.InSet(btp_rat))
    return sttp.Union(this->ratSubTypeSet);
  else if (sttp.InSet(btp_int))
    return sttp.Union(this->intSubTypeSet);
  else if (sttp.InSet(btp_nat))
    return sttp.Union(this->natSubTypeSet);
  else
    return sttp;
}

// MostGeneralNumericType
// stp : set of REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::MostGeneralNumericType (const SET<TYPE_REP_TypeRep> & ntp)
{
  if (ntp.Card () == 1)
    return ntp.GetElem ();
  else {
    SET<TYPE_REP_TypeRep> cases (MergeNumericTypes(ntp.Diff(mk_set(rep_alltp))));
    return cases.GetElem();
  }
}

// MergeTypes
// l : [REP`TypeRep | set of REP`TypeRep]
// r : [REP`TypeRep | set of REP`TypeRep]
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::MergeTypes (const Generic & gl, const Generic & gr) const
{
  if (gr.IsSet())
  {
    if (gl.IsSet())
      return MergeTypes(Set(gl).Union(gr), Nil ());
    else
      return MergeTypes(gr, gl);
  }

  if (gl.IsSet())
  {
    if (Set(gl).IsEmpty())
      return MergeTypes(Nil (), gr);
    else {
      Generic tp (Set(gl).GetElem());
      return MergeTypes(Set(gl).Diff(mk_set(tp)), MergeTypes(tp, gr));
    }
  }

  // !gr.IsSet() && !gl.IsSet()
  SET<TYPE_REP_TypeRep> r1tp;

  if (gl.IsNil ()) {
    if (gr.IsNil ())
      r1tp.Insert (rep_unittp);
    else
      r1tp.Insert (gr);
  } else {
    if (gr.IsNil ())
      r1tp.Insert (gl);
    else 
    {
      TYPE_REP_TypeRep l (gl);
      TYPE_REP_TypeRep r (gr);

      switch (l.GetTag()) {
        case TAG_TYPE_REP_SetTypeRep: {
          if (r.Is(TAG_TYPE_REP_SetTypeRep))
            r1tp.Insert(mk_REP_SetTypeRep(MergeTypes(l.GetRecord(pos_REP_SetTypeRep_elemtp),
                                                     r.GetRecord(pos_REP_SetTypeRep_elemtp))));
          else if (r.Is(TAG_TYPE_REP_EmptySetTypeRep))
            r1tp.Insert(mk_REP_UnionTypeRep(mk_set(l, r)));
          else if (r.Is(TAG_TYPE_REP_UnionTypeRep))
            r1tp.ImpUnion(r.GetSet(pos_REP_UnionTypeRep_tps)).Insert(l);
          else
            r1tp.Insert(l).Insert(r);
          break; 
        }
        case TAG_TYPE_REP_EmptySetTypeRep: {
          if (r.Is(TAG_TYPE_REP_SetTypeRep))
            r1tp.Insert(mk_REP_UnionTypeRep(mk_set(l, r)));
          else if (r.Is(TAG_TYPE_REP_UnionTypeRep))
            r1tp.ImpUnion(r.GetSet(pos_REP_UnionTypeRep_tps)).Insert(l);
          else
            r1tp.Insert(l).Insert(r);
          break; 
        }
        case TAG_TYPE_REP_SeqTypeRep: {
          if (r.Is(TAG_TYPE_REP_SeqTypeRep))
            r1tp.Insert(mk_REP_SeqTypeRep(MergeTypes(l.GetRecord(pos_REP_SeqTypeRep_elemtp), 
                                                     r.GetRecord(pos_REP_SeqTypeRep_elemtp))));
          else if (r.Is(TAG_TYPE_REP_EmptySeqTypeRep))
            r1tp.Insert(mk_REP_UnionTypeRep(mk_set(l, r)));
          else if (r.Is(TAG_TYPE_REP_UnionTypeRep))
            r1tp.ImpUnion(r.GetSet(pos_REP_UnionTypeRep_tps)).Insert(l);
          else
            r1tp.Insert(l).Insert(r);
          break; 
        }
        case TAG_TYPE_REP_EmptySeqTypeRep: {
          if (r.Is(TAG_TYPE_REP_SeqTypeRep))
            r1tp.Insert(mk_REP_UnionTypeRep(mk_set(l, r)));
          else if (r.Is(TAG_TYPE_REP_UnionTypeRep))
            r1tp.ImpUnion(r.GetSet(pos_REP_UnionTypeRep_tps)).Insert(l);
          else
            r1tp.Insert(l).Insert(r);
          break; 
        }
        case TAG_TYPE_REP_GeneralMapTypeRep: {
          if (r.Is(TAG_TYPE_REP_GeneralMapTypeRep))
            r1tp.Insert(mk_REP_GeneralMapTypeRep(MergeTypes(l.GetRecord(pos_REP_GeneralMapTypeRep_mapdom),
                                                            r.GetRecord(pos_REP_GeneralMapTypeRep_mapdom)),
                                                 MergeTypes(l.GetRecord(pos_REP_GeneralMapTypeRep_maprng),
                                                            r.GetRecord(pos_REP_GeneralMapTypeRep_maprng))));
          else if (r.Is(TAG_TYPE_REP_InjectiveMapTypeRep)) 
            r1tp.Insert(mk_REP_GeneralMapTypeRep(MergeTypes(l.GetRecord(pos_REP_GeneralMapTypeRep_mapdom),
                                                            r.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom)),
                                                 MergeTypes(l.GetRecord(pos_REP_GeneralMapTypeRep_maprng),
                                                            r.GetRecord(pos_REP_InjectiveMapTypeRep_maprng))));
          else if (r.Is(TAG_TYPE_REP_EmptyMapTypeRep)) 
            r1tp.Insert(mk_REP_UnionTypeRep(mk_set(l, r)));
          else if (r.Is(TAG_TYPE_REP_UnionTypeRep))
            r1tp.ImpUnion(r.GetSet(pos_REP_UnionTypeRep_tps)).Insert(l);
          else
            r1tp.Insert(l).Insert(r);
          break; 
        }
        case TAG_TYPE_REP_InjectiveMapTypeRep: {
          if (r.Is(TAG_TYPE_REP_GeneralMapTypeRep))
            r1tp.Insert(mk_REP_GeneralMapTypeRep(MergeTypes(l.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom),
                                                            r.GetRecord(pos_REP_GeneralMapTypeRep_mapdom)),
                                                 MergeTypes(l.GetRecord(pos_REP_InjectiveMapTypeRep_maprng),
                                                            r.GetRecord(pos_REP_GeneralMapTypeRep_maprng))));
          else if (r.Is(TAG_TYPE_REP_InjectiveMapTypeRep))
            r1tp.Insert(mk_REP_GeneralMapTypeRep(MergeTypes(l.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom),
                                                            r.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom)),
                                                 MergeTypes(l.GetRecord(pos_REP_InjectiveMapTypeRep_maprng),
                                                            r.GetRecord(pos_REP_InjectiveMapTypeRep_maprng))));
          else if (r.Is(TAG_TYPE_REP_EmptyMapTypeRep)) 
            r1tp.Insert(mk_REP_UnionTypeRep(mk_set(l, r)));
          else if (r.Is(TAG_TYPE_REP_UnionTypeRep))
            r1tp.ImpUnion(r.GetSet(pos_REP_UnionTypeRep_tps)).Insert(l);
          else
            r1tp.Insert(l).Insert(r);
          break; 
        }
        case TAG_TYPE_REP_EmptyMapTypeRep: {
          if (r.Is(TAG_TYPE_REP_GeneralMapTypeRep) || r.Is(TAG_TYPE_REP_InjectiveMapTypeRep))
            r1tp.Insert(mk_REP_UnionTypeRep(mk_set(l, r)));
          else if (r.Is(TAG_TYPE_REP_UnionTypeRep))
            r1tp.ImpUnion(r.GetSet(pos_REP_UnionTypeRep_tps)).Insert(l);
          else
            r1tp.Insert(l).Insert(r);
          break; 
        }
        case TAG_TYPE_REP_UnionTypeRep: {
          if (r.Is(TAG_TYPE_REP_UnionTypeRep))
            r1tp.ImpUnion(l.GetSet(pos_REP_UnionTypeRep_tps)).ImpUnion(r.GetSet(pos_REP_UnionTypeRep_tps));
          else
            r1tp.ImpUnion(l.GetSet(pos_REP_UnionTypeRep_tps)).Insert(r);
          break; 
        }
        case TAG_TYPE_REP_ExitTypeRep: {
          if (r.Is(TAG_TYPE_REP_ExitTypeRep))
            r1tp.Insert (mk_REP_ExitTypeRep(MergeTypes (l.GetRecord(pos_REP_ExitTypeRep_tp),
                                                        r.GetRecord(pos_REP_ExitTypeRep_tp))));
          else if (r.Is(TAG_TYPE_REP_UnionTypeRep))
            r1tp.ImpUnion(r.GetSet(pos_REP_UnionTypeRep_tps)).Insert(l);
          else
            r1tp.Insert(l).Insert(r);
          break; 
        }
        default: {
          if (r.Is(TAG_TYPE_REP_UnionTypeRep))
            r1tp.ImpUnion(r.GetSet(pos_REP_UnionTypeRep_tps)).Insert(l);
          else
            r1tp.Insert(l).Insert(r);
          break; 
        }
      }
    }
  }

  SET<TYPE_REP_TypeRep> restp (MergeNumericTypes(r1tp));
  switch(restp.Card()) {
    case 0: {
      InternalError (L"MergeTypes");
      return rep_alltp;
    }
    case 1: {
      return restp.GetElem();
    }
    default: {
      if (restp.InSet(rep_alltp))
        return rep_alltp;
      else
        return mk_REP_UnionTypeRep(restp);
    }
  }
}

// RemoveUnitType
// tp : REP`TypeRep
// -> [REP`TypeRep]
Generic StatSem::RemoveUnitType (const TYPE_REP_TypeRep & tp)
{
  switch(tp.GetTag()) {
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (tp.GetSet(pos_REP_UnionTypeRep_tps));
      Set st;
      Generic t;
      for (bool bb = tp_s.First(t); bb; bb = tp_s.Next(t))
      {
//// 20060308
//        if (!g.Is(TAG_TYPE_REP_UnitTypeRep))
//          st.Insert (g);
////        Generic tp_q = RemoveUnitType(g);
////        if( !tp_q.IsNil() ) st.Insert(tp_q);
        st.Insert(RemoveUnitType(t));
      }
      return mk_REP_UnionTypeRep(st.Diff(mk_set(Nil())));
    }
    case TAG_TYPE_REP_UnitTypeRep: {
      return Nil ();
    }
    default: {
      return tp;
    }
  }
}

// RemoveEmptySetType
// tp : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::RemoveEmptySetType(const TYPE_REP_TypeRep & tp)
{
  if (tp.Is(TAG_TYPE_REP_UnionTypeRep))
  {
    SET<TYPE_REP_TypeRep> tp_s (tp.GetSet(pos_REP_UnionTypeRep_tps).Diff(mk_set(mk_REP_EmptySetTypeRep(rep_alltp))));

    if (tp_s.Card() == 1)
      return tp_s.GetElem();
    else
      return mk_REP_UnionTypeRep(tp_s);
  }
  else
    return tp;
}

// RemoveEmptySeqType
// tp : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::RemoveEmptySeqType(const TYPE_REP_TypeRep & tp)
{
  if (tp.Is(TAG_TYPE_REP_UnionTypeRep))
  {
    SET<TYPE_REP_TypeRep> tp_s (tp.GetSet(pos_REP_UnionTypeRep_tps).Diff(mk_set(mk_REP_EmptySeqTypeRep(rep_alltp))));

    if (tp_s.Card() == 1)
      return tp_s.GetElem();
    else
      return mk_REP_UnionTypeRep(tp_s);
  }
  else
    return tp;
}

// RemoveEmptyMapType
// tp : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::RemoveEmptyMapType(const TYPE_REP_TypeRep & tp)
{
  if (tp.Is(TAG_TYPE_REP_UnionTypeRep))
  {
    SET<TYPE_REP_TypeRep> tp_s (tp.GetSet(pos_REP_UnionTypeRep_tps).Diff(mk_set(mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp))));

    if (tp_s.Card() == 1)
      return tp_s.GetElem();
    else
      return mk_REP_UnionTypeRep(tp_s);
  }
  else
    return tp;
}

// IntersectTypes
// l : REP`TypeRep
// r : REP`TypeRep
// -> [REP`TypeRep]
Generic StatSem::IntersectTypes (const TYPE_REP_TypeRep & l, const TYPE_REP_TypeRep & r)
{
  SET<TYPE_REP_TypeRep> r1tp;
  switch (l.GetTag()) {
    case TAG_TYPE_REP_AllTypeRep: { r1tp.Insert(r); break; }
    case TAG_TYPE_REP_UnionTypeRep: {
      if (r.Is(TAG_TYPE_REP_UnionTypeRep))
      {
        const SET<TYPE_REP_TypeRep> & usl (l.GetSet(pos_REP_UnionTypeRep_tps));
        const SET<TYPE_REP_TypeRep> & usr (r.GetSet(pos_REP_UnionTypeRep_tps));
        r1tp = ExpandNumericTypes (FlatternTypes (usl)).ImpIntersect(ExpandNumericTypes (FlatternTypes (usr)));
      }
      else
      {
        const SET<TYPE_REP_TypeRep> & usl (l.GetSet(pos_REP_UnionTypeRep_tps));
        SET<TYPE_REP_TypeRep> usr (mk_set(r));
        r1tp = ExpandNumericTypes (FlatternTypes (usl)).ImpIntersect(ExpandNumericTypes (FlatternTypes (usr)));
      }
      break;
    }
    default: {
      switch (r.GetTag()) {
        case TAG_TYPE_REP_AllTypeRep: { r1tp.Insert(l); break; }
        case TAG_TYPE_REP_UnionTypeRep: {
          SET<TYPE_REP_TypeRep> usl (mk_set(l));
          const SET<TYPE_REP_TypeRep> & usr (r.GetSet(pos_REP_UnionTypeRep_tps));
          r1tp = ExpandNumericTypes (FlatternTypes (usl)).ImpIntersect(ExpandNumericTypes (FlatternTypes (usr)));
          break;
        }
        default: {
          SET<TYPE_REP_TypeRep> usl (mk_set(l));
          SET<TYPE_REP_TypeRep> usr (mk_set(r));
          r1tp = ExpandNumericTypes (FlatternTypes (usl)).ImpIntersect(ExpandNumericTypes (FlatternTypes (usr)));
          break;
        }
      }
      break;
    }
  }

  SET<TYPE_REP_TypeRep> restp (MergeNumericTypes(r1tp));
  switch(restp.Card()) {
    case 0: {
      return Nil();
    }
    case 1: {
      return restp.GetElem();
    }
    default: {
      return mk_REP_UnionTypeRep(restp);
    }
  }
}

// IntersectTypeReps
// l : REP`TypeRep
// r : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::IntersectTypeReps (const TYPE_REP_TypeRep & l, const TYPE_REP_TypeRep & r)
{
  SET<TYPE_REP_TypeRep> ltp;
  switch(l.GetTag()) {
    case TAG_TYPE_REP_AllTypeRep:   { break; }
    case TAG_TYPE_REP_UnionTypeRep: { ltp.ImpUnion(l.GetSet(pos_REP_UnionTypeRep_tps)); break; }
    default:                        { ltp.Insert(l); break; }
  }

  SET<TYPE_REP_TypeRep> rtp;
  switch(r.GetTag()) {
    case TAG_TYPE_REP_AllTypeRep:   { break; }
    case TAG_TYPE_REP_UnionTypeRep: { rtp.ImpUnion(r.GetSet(pos_REP_UnionTypeRep_tps)); break; }
    default:                        { rtp.Insert(r); break; }
  }

  SET<TYPE_REP_TypeRep> restps;
  if (ltp.IsEmpty()) {
    if (rtp.IsEmpty())
      restps.Insert(rep_alltp);
    else
      restps = rtp;
  } else {
    if (rtp.IsEmpty())
      restps = ltp;
    else {
      Set intertp;
      Generic tp1;
      for (bool bb1 = ltp.First(tp1); bb1; bb1 = ltp.Next(tp1))
      {
        Generic tp2;
        for (bool bb2 = rtp.First(tp2); bb2; bb2 = rtp.Next(tp2)) {
          intertp.Insert(IntersectTypeRepInner(tp1, tp2));
        }
      }

      if (intertp.IsEmpty()) {
        restps.Insert(rep_alltp);
      }
      else {
        restps.ImpUnion(intertp.DUnion());
      }
    }
  }

  SET<TYPE_REP_TypeRep> restp (MergeNumericTypes(restps));
  switch(restp.Card()) {
    case 0:
      return l;
    case 1:
      return restp.GetElem();
    default:
      return mk_REP_UnionTypeRep(restp);
  }
}

// IntersectTypeRepInner
// TpR1 : REP`TypeRep
// TpR2 : REP`TypeRep
// -> set of REP`TypeRep
SET<TYPE_REP_TypeRep> StatSem::IntersectTypeRepInner (const TYPE_REP_TypeRep & TpR1, const TYPE_REP_TypeRep & TpR2)
{
  if (TpR1 == TpR2)
    return mk_set(TpR1);

  switch (TpR1.GetTag()) {
    case TAG_TYPE_REP_AllTypeRep: { return mk_set(TpR2); }
    case TAG_TYPE_REP_TypeNameRep: {
      if (TpR2.Is(TAG_TYPE_REP_TypeNameRep))
      {
        if (IsUnionRecursive(TpR1) || IsUnionRecursive(TpR2))
          return SET<TYPE_REP_TypeRep>();
        else
        {
          Generic newtp1 (TcauxLookUpTypeName(TpR1.GetRecord(pos_REP_TypeNameRep_nm)));
          Generic newtp2 (TcauxLookUpTypeName(TpR2.GetRecord(pos_REP_TypeNameRep_nm)));
          if (newtp1.IsNil() || newtp2.IsNil())
            return SET<TYPE_REP_TypeRep>();
          else
            return mk_set(IntersectTypeReps(newtp1, newtp2));
        }
      }
      else
      {
        if (IsUnionRecursive(TpR1))
          return SET<TYPE_REP_TypeRep>();
        else
        {
          Generic newtp (TcauxLookUpTypeName(TpR1.GetRecord(pos_REP_TypeNameRep_nm)));
          if (newtp.IsNil())
            return SET<TYPE_REP_TypeRep>();
          else
            return mk_set(IntersectTypeReps(newtp,TpR2));
        }
      }
    }
    // Note that the order is different from the spec here for
    // efficiency reasons, since it allows the blanket IsRecord test on
    // both types, below.
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> Lhs (TpR1.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> unioninnerset;
      Generic tp;
      for (bool bb = Lhs.First(tp); bb; bb = Lhs.Next(tp))
        unioninnerset.Insert(IntersectTypeReps(tp,TpR2));

      //return mk_set(mk_REP_UnionTypeRep(unioninnerset));
      return unioninnerset;
    }
  }

  switch (TpR2.GetTag()) {
    case TAG_TYPE_REP_AllTypeRep: { return mk_set(TpR1); }
    case TAG_TYPE_REP_TypeNameRep: {
      if (IsUnionRecursive(TpR2))
        return SET<TYPE_REP_TypeRep>();
      else
      {
        Generic newtp (TcauxLookUpTypeName(TpR2.GetRecord(pos_REP_TypeNameRep_nm)));
        if (newtp.IsNil())
          return SET<TYPE_REP_TypeRep>();
        else
          return mk_set(IntersectTypeReps(TpR1,newtp));
      }
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> Rhs (TpR2.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> unioninnerset;
      Generic tp;
      for (bool bb = Rhs.First(tp); bb; bb = Rhs.Next(tp))
        unioninnerset.Insert(IntersectTypeReps(TpR1,tp));

      //return mk_set(mk_REP_UnionTypeRep(unioninnerset));
      return unioninnerset;
    }
    case TAG_TYPE_REP_NumericTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_NumericTypeRep)) {
        SET<TYPE_REP_TypeRep> NumTp1 (ExpandNumericTypes(mk_set(TpR1)));
        SET<TYPE_REP_TypeRep> NumTp2 (ExpandNumericTypes(mk_set(TpR2)));
        NumTp1.ImpIntersect(NumTp2);
        return NumTp1;
      }
      else
        return SET<TYPE_REP_TypeRep>();
    }
    case TAG_TYPE_REP_SetTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_SetTypeRep)) {
        const TYPE_REP_TypeRep & STp1 (TpR1.GetRecord(pos_REP_SetTypeRep_elemtp));
        const TYPE_REP_TypeRep & STp2 (TpR2.GetRecord(pos_REP_SetTypeRep_elemtp));
        return mk_set(mk_REP_SetTypeRep(IntersectTypeReps(STp1,STp2)));
      }
      else
        return SET<TYPE_REP_TypeRep>();
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_SeqTypeRep)) {
        const TYPE_REP_TypeRep & STp1 (TpR1.GetRecord(pos_REP_SeqTypeRep_elemtp));
        const TYPE_REP_TypeRep & STp2 (TpR2.GetRecord(pos_REP_SeqTypeRep_elemtp));
        return mk_set(mk_REP_SeqTypeRep(IntersectTypeReps(STp1,STp2)));
      }
      else
        return SET<TYPE_REP_TypeRep>();
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_ProductTypeRep)) {
        const SEQ<TYPE_REP_TypeRep> & PTp1 (TpR1.GetSequence(pos_REP_ProductTypeRep_tps));
        const SEQ<TYPE_REP_TypeRep> & PTp2 (TpR2.GetSequence(pos_REP_ProductTypeRep_tps));
        if (PTp1.Length() == PTp2.Length())
        {
          bool forall = true;
          size_t len_PTp1 = PTp1.Length();
          for (size_t i = 1; i <= len_PTp1 && forall; i++)
            forall = IsCompatible(POS, PTp1[i], PTp2[i]);

          if (forall)
          {
            SEQ<TYPE_REP_TypeRep> intertps;
            for (size_t i = 1; i <= len_PTp1; i++)
            {
              intertps.ImpAppend(IntersectTypeReps(PTp1[i], PTp2[i]));
            }
            return mk_set(mk_REP_ProductTypeRep(intertps));
          }
        }
      }
      return SET<TYPE_REP_TypeRep>();
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_CompositeTypeRep)) {
        const TYPE_AS_Name & LhsName (TpR1.GetRecord(pos_REP_CompositeTypeRep_nm));
        const TYPE_AS_Name & RhsName (TpR2.GetRecord(pos_REP_CompositeTypeRep_nm));
        const SEQ<TYPE_REP_FieldRep> & LhsFields (TpR1.GetSequence(pos_REP_CompositeTypeRep_fields));
        const SEQ<TYPE_REP_FieldRep> & RhsFields (TpR2.GetSequence(pos_REP_CompositeTypeRep_fields));

        if ((LhsName == RhsName) && (LhsFields.Length() == RhsFields.Length()))
        {
          SEQ<TYPE_REP_FieldRep> compseq;
          size_t len_LhsFields = LhsFields.Length();
          for (size_t i = 1; i <= len_LhsFields; i++)
          {
            const TYPE_REP_FieldRep & lhs_fr = LhsFields[i];
            const TYPE_REP_FieldRep & rhs_fr = RhsFields[i];
            TYPE_REP_TypeRep intertp (IntersectTypeReps (lhs_fr.GetRecord(pos_REP_FieldRep_tp),
                                                         rhs_fr.GetRecord(pos_REP_FieldRep_tp)));
            compseq.ImpAppend(TYPE_REP_FieldRep().Init(lhs_fr.GetField(pos_REP_FieldRep_sel),
                                                       intertp,
                                                       lhs_fr.GetBool(pos_REP_FieldRep_dc)));
          }
          return mk_set(mk_REP_CompositeTypeRep(LhsName, compseq));
        }
      }
      return SET<TYPE_REP_TypeRep>();
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_PartialFnTypeRep)) {
        const Generic & LhsDom (TpR1.GetField(pos_REP_PartialFnTypeRep_fndom));
        const Generic & RhsDom (TpR2.GetField(pos_REP_PartialFnTypeRep_fndom)); // It was 2 before!!! which is rng but not dom
        Generic fndomtp;

        if (LhsDom.Is(TAG_TYPE_REP_AllTypeRep)) {
          if (RhsDom.Is(TAG_TYPE_REP_AllTypeRep))
            fndomtp =  rep_alltp;
          else
            fndomtp = RhsDom;
        } else {
          if (RhsDom.Is(TAG_TYPE_REP_AllTypeRep))
            fndomtp = LhsDom;
          else
          {
            SEQ<TYPE_REP_TypeRep> LhsDomS (LhsDom);
            SEQ<TYPE_REP_TypeRep> RhsDomS (RhsDom);
            if (LhsDomS.Length() == RhsDomS.Length())
            {
              SEQ<TYPE_REP_TypeRep> domseq;
              size_t len_LhsDomS = LhsDomS.Length();
              for (size_t i = 1; i <= len_LhsDomS; i++)
                domseq.ImpAppend(IntersectTypeReps(LhsDomS[i], RhsDomS[i]));
              fndomtp = domseq;
            }
            else
              fndomtp = rep_alltp;
          }
        }
        return mk_set(mk_REP_PartialFnTypeRep(fndomtp,
                                              IntersectTypeReps(TpR1.GetRecord(pos_REP_PartialFnTypeRep_fnrng),
                                                                TpR2.GetRecord(pos_REP_PartialFnTypeRep_fnrng))));
      }
      else if (TpR1.Is(TAG_TYPE_REP_TotalFnTypeRep)) {
        const Generic & LhsDom (TpR1.GetField(pos_REP_TotalFnTypeRep_fndom));
        const Generic & RhsDom (TpR2.GetField(pos_REP_PartialFnTypeRep_fndom));
                           // It was 2 before!!! Why?
        Generic fndomtp;
        if (LhsDom.Is(TAG_TYPE_REP_AllTypeRep)) {
          if (RhsDom.Is(TAG_TYPE_REP_AllTypeRep)) {
            fndomtp = rep_alltp;
          }
          else {
            fndomtp = RhsDom;
          }
        }
        else {
          if (RhsDom.Is(TAG_TYPE_REP_AllTypeRep)) {
            fndomtp = LhsDom;
          }
          else {
            SEQ<TYPE_REP_TypeRep> LhsDomS(LhsDom), RhsDomS(RhsDom);
            if (LhsDomS.Length() == RhsDomS.Length()) {
              SEQ<TYPE_REP_TypeRep> domseq;
              size_t len_LhsDomS = LhsDomS.Length();
              for (size_t i = 1; i <= len_LhsDomS; i++) {
                domseq.ImpAppend(IntersectTypeReps(LhsDomS[i], RhsDomS[i]));
              }
              fndomtp = domseq;
            }
            else {
              fndomtp = rep_alltp;
            }
          }
        }
        return mk_set(mk_REP_PartialFnTypeRep(fndomtp,
                                              IntersectTypeReps(TpR1.GetRecord(pos_REP_TotalFnTypeRep_fnrng),
                                                                TpR2.GetRecord(pos_REP_PartialFnTypeRep_fnrng))));
      }
      return SET<TYPE_REP_TypeRep>();
    }
    case TAG_TYPE_REP_TotalFnTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_PartialFnTypeRep)) {
        const Generic & LhsDom (TpR1.GetField(pos_REP_PartialFnTypeRep_fndom));
        const Generic & RhsDom (TpR2.GetField(pos_REP_TotalFnTypeRep_fndom)); // It was 2 before!!!
        Generic fndomtp;
        if (LhsDom.Is(TAG_TYPE_REP_AllTypeRep)) {
          if (RhsDom.Is(TAG_TYPE_REP_AllTypeRep))
            fndomtp = rep_alltp;
          else
            fndomtp = RhsDom;
        } else {
          if (RhsDom.Is(TAG_TYPE_REP_AllTypeRep))
            fndomtp = LhsDom;
          else
          {
            SEQ<TYPE_REP_TypeRep> LhsDomS(LhsDom);
            SEQ<TYPE_REP_TypeRep> RhsDomS(RhsDom);
            if (LhsDomS.Length() == RhsDomS.Length())
            {
              SEQ<TYPE_REP_TypeRep> domseq;
              size_t len_LhsDomS = LhsDomS.Length();
              for (size_t i = 1; i <= len_LhsDomS; i++)
                domseq.ImpAppend(IntersectTypeReps(LhsDomS[i], RhsDomS[i]));
              fndomtp = domseq;
            }
            else
              fndomtp = rep_alltp;
          }
        }
        return mk_set(mk_REP_PartialFnTypeRep(fndomtp,
                                              IntersectTypeReps(TpR1.GetRecord(pos_REP_PartialFnTypeRep_fnrng),
                                                                TpR2.GetRecord(pos_REP_TotalFnTypeRep_fnrng))));
      }
      else if (TpR1.Is(TAG_TYPE_REP_TotalFnTypeRep)) {
        const Generic & LhsDom (TpR1.GetField(pos_REP_TotalFnTypeRep_fndom));
        const Generic & RhsDom (TpR2.GetField(pos_REP_TotalFnTypeRep_fndom)); // It was 2 before!!!
        Generic fndomtp;
        if (LhsDom.Is(TAG_TYPE_REP_AllTypeRep)) {
          if (RhsDom.Is(TAG_TYPE_REP_AllTypeRep))
            fndomtp = rep_alltp;
          else
            fndomtp = RhsDom;
        } else {
          if (RhsDom.Is(TAG_TYPE_REP_AllTypeRep))
            fndomtp = LhsDom;
          else
          {
            SEQ<TYPE_REP_TypeRep> LhsDomS(LhsDom), RhsDomS(RhsDom);
            if (LhsDomS.Length() == RhsDomS.Length())
            {
              SEQ<TYPE_REP_TypeRep> domseq;
              size_t len_LhsDomS = LhsDomS.Length();
              for (size_t i = 1; i <= len_LhsDomS; i++)
                domseq.ImpAppend(IntersectTypeReps(LhsDomS[i], RhsDomS[i]));
              fndomtp = domseq;
            }
            else
              fndomtp = rep_alltp;
          }
        }
        return mk_set(mk_REP_TotalFnTypeRep(fndomtp,
                                            IntersectTypeReps(TpR1.GetRecord(pos_REP_TotalFnTypeRep_fnrng),
                                                              TpR2.GetRecord(pos_REP_TotalFnTypeRep_fnrng))));
      }
      return SET<TYPE_REP_TypeRep>();
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_GeneralMapTypeRep)) {
        return mk_set(mk_REP_GeneralMapTypeRep(IntersectTypeReps(TpR1.GetRecord(pos_REP_GeneralMapTypeRep_mapdom),
                                                                 TpR2.GetRecord(pos_REP_GeneralMapTypeRep_mapdom)),
                                               IntersectTypeReps(TpR1.GetRecord(pos_REP_GeneralMapTypeRep_maprng),
                                                                 TpR2.GetRecord(pos_REP_GeneralMapTypeRep_maprng))));
      }
      else if (TpR1.Is(TAG_TYPE_REP_InjectiveMapTypeRep)) {
        return mk_set(mk_REP_InjectiveMapTypeRep(IntersectTypeReps(TpR1.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom),
                                                                   TpR2.GetRecord(pos_REP_GeneralMapTypeRep_mapdom)),
                                                 IntersectTypeReps(TpR1.GetRecord(pos_REP_InjectiveMapTypeRep_maprng),
                                                                   TpR2.GetRecord(pos_REP_GeneralMapTypeRep_maprng))));
      }
      else
        return SET<TYPE_REP_TypeRep>();
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      if (TpR1.Is(TAG_TYPE_REP_GeneralMapTypeRep)) {
        return mk_set(mk_REP_InjectiveMapTypeRep(IntersectTypeReps(TpR1.GetRecord(pos_REP_GeneralMapTypeRep_mapdom),
                                                                   TpR2.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom)),
                                                 IntersectTypeReps(TpR1.GetRecord(pos_REP_GeneralMapTypeRep_maprng),
                                                                   TpR2.GetRecord(pos_REP_InjectiveMapTypeRep_maprng))));
      }
      else if (TpR1.Is(TAG_TYPE_REP_InjectiveMapTypeRep)) {
        return mk_set(mk_REP_InjectiveMapTypeRep(IntersectTypeReps(TpR1.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom),
                                                                   TpR2.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom)),
                                                 IntersectTypeReps(TpR1.GetRecord(pos_REP_InjectiveMapTypeRep_maprng),
                                                                   TpR2.GetRecord(pos_REP_InjectiveMapTypeRep_maprng))));
      }
      else
        return SET<TYPE_REP_TypeRep>();
    }
    default: { return SET<TYPE_REP_TypeRep>(); }
  }
  return SET<TYPE_REP_TypeRep>(); // dummy
}

// FlatternTypes
// tps : set of REP`TypeRep
// +> set of REP`TypeRep
SET<TYPE_REP_TypeRep> StatSem::FlatternTypes (const SET<TYPE_REP_TypeRep> & tps_)
{
  SET<TYPE_REP_TypeRep> tps (tps_);
  SET<TYPE_REP_TypeRep> res_s;
  Generic tp;
  for (bool bb = tps.First (tp); bb; bb = tps.Next (tp))
  {
    if (tp.Is (TAG_TYPE_REP_TypeNameRep))
    {
      if (! IsUnionRecursive (tp))
      {
        Generic newtp (TcauxLookUpTypeName(Record(tp).GetRecord(pos_REP_TypeNameRep_nm)));
        if (!newtp.IsNil ())
        {
          if (newtp.Is (TAG_TYPE_REP_UnionTypeRep))
            res_s.ImpUnion (FlatternTypes (Record(newtp).GetSet(pos_REP_UnionTypeRep_tps)));
          else
            res_s.Insert (newtp);
        }
      }
    }
    else
      res_s.Insert (tp);
  }
  return res_s;
}

// ExtractIntNumType
// numtp : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::ExtractIntNumType (const TYPE_REP_TypeRep & numtp)
{
  if (numtp.Is(TAG_TYPE_REP_AllTypeRep)) {
    return btp_int;
  }
  else
  {
    SET<TYPE_REP_TypeRep> r1tp (ExtractNumericTypeInner(numtp));
    switch(r1tp.Card()) {
      case 1: {
        TYPE_REP_TypeRep EINT (r1tp.GetElem());
        if (EINT.Is (TAG_TYPE_REP_NumericTypeRep) &&
            ((EINT.GetIntValue(pos_REP_NumericTypeRep_qtp) == REAL) ||
             (EINT.GetIntValue(pos_REP_NumericTypeRep_qtp) == RAT)))
          return btp_int;
        else
          return EINT;
      }
      default: {
        InternalError (L"ExtractIntNumType");
        return btp_int;
      }
    }
  }
}

// ExtractNumericType
// numtp : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::ExtractNumericType (const TYPE_REP_TypeRep & numtp)
{
  if (numtp.Is(TAG_TYPE_REP_AllTypeRep))
    return btp_real;
  else {
    SET<TYPE_REP_TypeRep> r1tp (ExtractNumericTypeInner(numtp));
    switch (r1tp.Card()) {
      case 0: {
        return rep_alltp;
      }
      case 1: {
        return r1tp.GetElem();
      }
      default: {
        InternalError (L"ExtractNumericType");
        return btp_real;
      }
    }
  }
}

// ExtractNumericTypeInner
// numtp : REP`TypeRep
// -> set of REP`TypeRep
SET<TYPE_REP_TypeRep> StatSem::ExtractNumericTypeInner (const TYPE_REP_TypeRep & numtp)
{
  switch (numtp.GetTag ()) {
    case TAG_TYPE_REP_NumericTypeRep: {
      return mk_set(numtp);
    }
    case TAG_TYPE_REP_TypeNameRep: {
      if (IsUnionRecursive (numtp))
        return SET<TYPE_REP_TypeRep>();
      else
      {
        Generic tp (TcauxLookUpTypeName (numtp.GetRecord(pos_REP_TypeNameRep_nm)));
        if (tp.IsNil())
          return SET<TYPE_REP_TypeRep>();
        else
          return ExtractNumericTypeInner (tp);
      }
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> Utp (numtp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> r1tp;
      Generic g;
      for (bool bb = Utp.First(g); bb ; bb = Utp.Next(g))
        r1tp.ImpUnion (ExtractNumericTypeInner(g));
      return MergeNumericTypes (r1tp);
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return ExtractNumericTypeInner (numtp.GetRecord(pos_REP_InvTypeRep_shape));
    }
    default: {
      return SET<TYPE_REP_TypeRep>();
    }
  }
}

// UnmaskSetType
// i : TYPE`Ind
// tp : REP`TypeRep
// -> [REP`TypeRep]
Generic StatSem::UnmaskSetType(const Int & i, const TYPE_REP_TypeRep & tp) const
{
  switch (tp.GetTag()) {
    case TAG_TYPE_REP_EmptySetTypeRep: {
      return Nil();
    }
    case TAG_TYPE_REP_SetTypeRep: {
      return tp.GetRecord(pos_REP_SetTypeRep_elemtp);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> utps (tp.GetSet(pos_REP_UnionTypeRep_tps));
      Set t_s;

      Generic utp;
      for(bool bb = utps.First (utp); bb; bb = utps.Next (utp))
        t_s.Insert (UnmaskSetType (i, utp));

      t_s.ImpDiff(mk_set(Nil()));

      switch(t_s.Card()) {
        case 0: {
          return Nil();
        }
        case 1: {
          return t_s.GetElem();
        }
        default: {
          return mk_REP_UnionTypeRep(t_s);
        }
      }
    }
    case TAG_TYPE_REP_AllTypeRep: {
      return tp;
    }
    default: {
      InternalError (L"UnmaskSetType");
      return Nil();
    }
  }
}

// UnmaskSetTypeForLoop
// i : TYPE`Ind
// tp : REP`TypeRep
// -> [REP`TypeRep]
Generic StatSem::UnmaskSetTypeForLoop(const Int & i, const TYPE_REP_TypeRep & tp) const
{
  switch (tp.GetTag()) {
    case TAG_TYPE_REP_EmptySetTypeRep: {
      return Nil();
    }
    case TAG_TYPE_REP_SetTypeRep: {
      return tp.GetRecord(pos_REP_SetTypeRep_elemtp);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> utps (tp.GetSet(pos_REP_UnionTypeRep_tps));
      Set t_s;

      Generic utp;
      for(bool bb = utps.First (utp); bb; bb = utps.Next (utp))
        t_s.Insert (UnmaskSetType (i, utp));

      t_s.ImpDiff(mk_set(Nil()));

      switch(t_s.Card()) {
        case 0: {
          return Nil();
        }
        case 1: {
          return t_s.GetElem();
        }
        default: {
          return mk_REP_UnionTypeRep(t_s);
        }
      }
    }
    default: {
      InternalError (L"UnmaskSetTypeForLoop");
      return Nil();
    }
  }
}

// ExtractSetType
// sttp : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::ExtractSetType (const TYPE_REP_TypeRep & sttp)
{
  SET<TYPE_REP_TypeRep> r1tp (ExtractSetTypeInner(sttp));
  switch(r1tp.Card()) {
    case 0: {
      return mk_REP_EmptySetTypeRep(rep_alltp);
    }
    case 1: {
      return r1tp.GetElem();
    }
    default: {
      return mk_REP_UnionTypeRep(r1tp);
    }
  }
}

// ExtractSetTypeInner
// sttp : REP`TypeRep
// -> set of REP`TypeRep
SET<TYPE_REP_TypeRep> StatSem::ExtractSetTypeInner (const TYPE_REP_TypeRep & sttp)
{
  switch (sttp.GetTag()) {
    case TAG_TYPE_REP_TypeNameRep: {
      if (IsUnionRecursive (sttp))
        return SET<TYPE_REP_TypeRep>();
      else
      {
        Generic tp (TcauxLookUpTypeName (sttp.GetRecord(pos_REP_TypeNameRep_nm)));
        if (tp.IsNil())
          return SET<TYPE_REP_TypeRep>();
        else
          return ExtractSetTypeInner (tp);
      }
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return ExtractSetTypeInner (sttp.GetRecord(pos_REP_InvTypeRep_shape));
    }
    case TAG_TYPE_REP_TypeVarRep:
    case TAG_TYPE_REP_AllTypeRep: {
      return mk_set(mk_REP_SetTypeRep(rep_alltp), mk_REP_EmptySetTypeRep(rep_alltp));
    }
    case TAG_TYPE_REP_SetTypeRep: {
      return mk_set(sttp);
    }
    case TAG_TYPE_REP_EmptySetTypeRep: {
      return mk_set(sttp);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> Utp (sttp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> res;
      Generic i;
      for (bool bb = Utp.First(i); bb ; bb = Utp.Next(i))
        res.ImpUnion (ExtractSetTypeInner(i));
      return res;
    }
    default: {
      return SET<TYPE_REP_TypeRep>();
    }
  }
}

// SetTypeUnion
// stp_s : set of REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::SetTypeUnion (const SET<TYPE_REP_TypeRep> & stp_s)
{
  switch(stp_s.Card ()) {
    case 0: {
      return mk_REP_EmptySetTypeRep(rep_alltp);
    }
    case 1: {
      return stp_s.GetElem();
    }
    default: {
      SET<TYPE_REP_TypeRep> St (stp_s);
      TYPE_REP_TypeRep St1 (St.GetElem()); St.RemElem (St1);
      TYPE_REP_TypeRep St2 (St.GetElem()); St.RemElem (St2);
  
      TYPE_REP_TypeRep ust (mk_REP_SetTypeRep(rep_alltp));
      TYPE_REP_TypeRep newtp;
  
      if ((St1 == ust) || (St2 == ust))
        return ust;
      else if (St1.Is(TAG_TYPE_REP_EmptySetTypeRep))
      {
        return mk_REP_UnionTypeRep(mk_set(St1, SetTypeUnion (St.Union(mk_set(St2)))));
      }
      else if (St2.Is(TAG_TYPE_REP_EmptySetTypeRep))
      {
        return mk_REP_UnionTypeRep(mk_set(St2, SetTypeUnion (St.Union(mk_set(St1)))));
      }
      else if (St1.Is(TAG_TYPE_REP_SetTypeRep) && St2.Is(TAG_TYPE_REP_SetTypeRep))
      {
        newtp = mk_REP_SetTypeRep(MergeTypes(St1.GetRecord(pos_REP_SetTypeRep_elemtp),
                                             St2.GetRecord(pos_REP_SetTypeRep_elemtp)));
      }
      else if (St1.Is(TAG_TYPE_REP_UnionTypeRep) && St2.Is(TAG_TYPE_REP_SetTypeRep))
      {
        newtp = SetTypeUnion (St1.GetSet(pos_REP_UnionTypeRep_tps).Union(mk_set(St2)));
      }
      else if (St2.Is(TAG_TYPE_REP_UnionTypeRep) && St1.Is(TAG_TYPE_REP_SetTypeRep))
      {
        newtp = SetTypeUnion (St2.GetSet(pos_REP_UnionTypeRep_tps).Union(mk_set(St1)));
      }
      else if (St1.Is(TAG_TYPE_REP_UnionTypeRep) && St2.Is(TAG_TYPE_REP_UnionTypeRep))
      {
        newtp = SetTypeUnion (St1.GetSet(pos_REP_UnionTypeRep_tps).Union(St2.GetSet(pos_REP_UnionTypeRep_tps)));
      }
      else
      {
        InternalError (L"SetTypeUnion");
        newtp = ust;
      }
  
      if (St.IsEmpty())
        return newtp;
      else {
        St.Insert (newtp);
        return SetTypeUnion (St);
      }
    }
  }
}

// SetTypeInter
// stp_s : set of REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::SetTypeInter (const SET<TYPE_REP_TypeRep> & stp_s)
{
  switch(stp_s.Card ()) {
    case 0: {
      return mk_REP_EmptySetTypeRep(rep_alltp);
    }
    case 1: {
      return stp_s.GetElem();
    }
    default: {
      SET<TYPE_REP_TypeRep> St(stp_s);
      TYPE_REP_TypeRep St1 (St.GetElem()); St.RemElem (St1);
      TYPE_REP_TypeRep St2 (St.GetElem()); St.RemElem (St2);
  
      TYPE_REP_TypeRep ust (mk_REP_SetTypeRep(rep_alltp));
      TYPE_REP_TypeRep newtp;
  
      if (St1.Is(TAG_TYPE_REP_EmptySetTypeRep) || St2.Is(TAG_TYPE_REP_EmptySetTypeRep))
        newtp = mk_REP_EmptySetTypeRep(rep_alltp);
      else if (St1 == ust)
        newtp = St2;
      else if (St2 == ust)
        newtp = St1;
      else if (St1.Is(TAG_TYPE_REP_SetTypeRep) && St2.Is(TAG_TYPE_REP_SetTypeRep))
      {
        const TYPE_REP_TypeRep & sl (St1.GetRecord(pos_REP_SetTypeRep_elemtp));
        const TYPE_REP_TypeRep & sr (St2.GetRecord(pos_REP_SetTypeRep_elemtp));
  
        Generic tp (IntersectTypes (sl, sr));
  
        if (tp.IsNil())
          newtp = mk_REP_EmptySetTypeRep(rep_alltp);
        else
          newtp = mk_REP_SetTypeRep(tp);
      }
      else if (St1.Is(TAG_TYPE_REP_UnionTypeRep) && St2.Is(TAG_TYPE_REP_SetTypeRep))
      {
        SET<TYPE_REP_TypeRep> utps (St1.GetSet(pos_REP_UnionTypeRep_tps));
        SET<TYPE_REP_TypeRep> s;
        Generic utp;
        for (bool bb = utps.First (utp); bb; bb = utps.Next (utp)) {
          s.Insert (SetTypeInter (mk_set(St2, utp)));
        }
        s.ImpDiff(mk_set(mk_REP_EmptySetTypeRep(rep_alltp)));
  
        switch (s.Card ()) {
          case 0:  { newtp = mk_REP_EmptySetTypeRep(rep_alltp); break; }
          case 1:  { newtp = s.GetElem (); break; }
          default: { newtp = mk_REP_UnionTypeRep(s); break; }
        }
      }
      else if (St1.Is(TAG_TYPE_REP_SetTypeRep) && St2.Is(TAG_TYPE_REP_UnionTypeRep))
      {
        SET<TYPE_REP_TypeRep> utps (St2.GetSet(pos_REP_UnionTypeRep_tps));
        SET<TYPE_REP_TypeRep> s;
        Generic utp;
        for (bool bb = utps.First (utp); bb; bb = utps.Next (utp))
        {
          s.Insert (SetTypeInter (mk_set(St1, utp)));
        }
        s.ImpDiff(mk_set(mk_REP_EmptySetTypeRep(rep_alltp)));
  
        switch (s.Card ()) {
          case 0:  { newtp = mk_REP_EmptySetTypeRep(rep_alltp); break; }
          case 1:  { newtp = s.GetElem (); break; }
          default: { newtp = mk_REP_UnionTypeRep(s); break; }
        }
      }
      else if (St1.Is(TAG_TYPE_REP_UnionTypeRep) && St2.Is(TAG_TYPE_REP_UnionTypeRep))
      {
        SET<TYPE_REP_TypeRep> ul (St1.GetSet(pos_REP_UnionTypeRep_tps).Diff(mk_set(mk_REP_EmptySetTypeRep(rep_alltp))));
        SET<TYPE_REP_TypeRep> ur (St2.GetSet(pos_REP_UnionTypeRep_tps).Diff(mk_set(mk_REP_EmptySetTypeRep(rep_alltp))));
  
        SET<TYPE_REP_TypeRep> s;
        Generic ultp;
        for (bool bb = ul.First (ultp); bb; bb = ul.Next (ultp)) {
          Generic urtp;
          for (bool cc = ur.First (urtp); cc; cc = ur.Next (urtp)) {
            s.Insert (SetTypeInter (mk_set(ultp, urtp)));
          }
        }

        switch (s.Card ()) {
          case 0:  { newtp = mk_REP_EmptySetTypeRep(rep_alltp); break; }
          case 1:  { newtp = s.GetElem (); break; }
          default: { newtp = mk_REP_UnionTypeRep(s); break; }
        }
      }
      else
      {
        InternalError (L"SetTypeInter");
        newtp = mk_REP_EmptySetTypeRep(rep_alltp);
      }
  
      if (St.IsEmpty())
        return newtp;
      else
      {
        St.Insert (newtp);
        return  SetTypeInter (St);
      }
    }
  }
}

// SetTypeMinus
// St1 : REP`TypeRep
// St2 : REP`TypeRep
// -> [REP`TypeRep]
Generic StatSem::SetTypeMinus (const TYPE_REP_TypeRep & St1, const TYPE_REP_TypeRep & St2)
{
  Set assume;
  if (IsOverlapping(St1, St2, assume))
  {
    switch(St1.GetTag()) {
      case TAG_TYPE_REP_EmptySetTypeRep: {
        return mk_REP_EmptySetTypeRep(rep_alltp);
      }
      case TAG_TYPE_REP_SetTypeRep: {
        return mk_REP_UnionTypeRep(mk_set(St1, mk_REP_EmptySetTypeRep(rep_alltp)));
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        return mk_REP_UnionTypeRep(St1.GetSet(pos_REP_UnionTypeRep_tps).Union(mk_set(mk_REP_EmptySetTypeRep(rep_alltp))));
      }
      default: {
        InternalError (L"SetTypeMinus");
        return Nil();
      }
    }
  }
  else
    return Nil();
}

// UnmaskSeqType
// i : TYPE`Ind
// tp : REP`TypeRep
// -> [REP`TypeRep]
Generic StatSem::UnmaskSeqType(const Int & i, const TYPE_REP_TypeRep & tp)
{
  switch(tp.GetTag()) {
// 20111212 -->
    case TAG_TYPE_REP_AllTypeRep:
    case TAG_TYPE_REP_TypeVarRep: {
      return rep_alltp;
    }
// <-- 20111212
    case TAG_TYPE_REP_EmptySeqTypeRep: {
      return Nil();
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return tp.GetRecord(pos_REP_SeqTypeRep_elemtp);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> utps (tp.GetSet(pos_REP_UnionTypeRep_tps));
      Set t_s;

      Generic utp;
      for(bool bb = utps.First (utp); bb; bb = utps.Next (utp))
        t_s.Insert (UnmaskSeqType (i, utp));

// 20130108 -->
//      if (t_s.InSet(Nil()))
//      {
//        if (i == POS)
//          t_s.RemElem (Nil());
//        else
//          return Nil();
//      }
      t_s.ImpDiff(mk_set(Nil()));
// <-- 20130108

      switch(t_s.Card()) {
        case 0:  { return Nil(); }
        case 1:  { return t_s.GetElem(); }
        default: { return mk_REP_UnionTypeRep(t_s); }
      }
    }
    default: {
      InternalError (L"UnmaskSeqType");
      return Nil();
    }
  }
}

// UnmaskSeqTypeForLoop
// i : TYPE`Ind
// tp : REP`TypeRep
// -> [REP`TypeRep]
Generic StatSem::UnmaskSeqTypeForLoop(const Int & i, const TYPE_REP_TypeRep & tp)
{
  switch(tp.GetTag()) {
    case TAG_TYPE_REP_EmptySeqTypeRep: {
      return Nil();
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return tp.GetRecord(pos_REP_SeqTypeRep_elemtp);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> utps (tp.GetSet(pos_REP_UnionTypeRep_tps));
      Set t_s;
      Generic utp;
      for(bool bb = utps.First (utp); bb; bb = utps.Next (utp))
        t_s.Insert (UnmaskSeqType (i, utp));

      t_s.ImpDiff(mk_set(Nil()));

      switch(t_s.Card()) {
        case 0:  { return Nil(); }
        case 1:  { return t_s.GetElem(); }
        default: { return mk_REP_UnionTypeRep((Generic)t_s); }
      }
    }
    default: {
      InternalError (L"UnmaskSeqTypeForLoop");
      return Nil();
    }
  }
}

// ExtractSeqType
// sqtp : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::ExtractSeqType (const TYPE_REP_TypeRep & sqtp)
{
  SET<TYPE_REP_TypeRep> r1tp (ExtractSeqTypeInner(sqtp));
  switch(r1tp.Card()) {
    case 0:  { return mk_REP_EmptySeqTypeRep(rep_alltp); }
    case 1:  { return r1tp.GetElem(); }
    default: { return mk_REP_UnionTypeRep(r1tp); }
  }
}

// ExtractSeqTypeInner
// sqtp : REP`TypeRep
// -> set of REP`TypeRep
SET<TYPE_REP_TypeRep> StatSem::ExtractSeqTypeInner (const TYPE_REP_TypeRep & sqtp)
{
  switch(sqtp.GetTag()) {
    case TAG_TYPE_REP_TypeNameRep: {
      if (IsUnionRecursive (sqtp))
        return SET<TYPE_REP_TypeRep>();
      else
      {
        Generic tp (TcauxLookUpTypeName (sqtp.GetRecord(pos_REP_TypeNameRep_nm)));
        if (tp.IsNil())
          return SET<TYPE_REP_TypeRep>();
        else
          return ExtractSeqTypeInner (tp);
      }
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return ExtractSeqTypeInner (sqtp.GetRecord(pos_REP_InvTypeRep_shape));
    }
    case TAG_TYPE_REP_TypeVarRep:
    case TAG_TYPE_REP_AllTypeRep: {
      return mk_set(mk_REP_SeqTypeRep(rep_alltp), mk_REP_EmptySeqTypeRep(rep_alltp));
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return mk_set(sqtp);
    }
    case TAG_TYPE_REP_EmptySeqTypeRep: {
      return mk_set(sqtp);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> Utp (sqtp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> res;
      Generic i;
      for (bool bb = Utp.First(i); bb ; bb = Utp.Next(i))
        res.ImpUnion (ExtractSeqTypeInner(i));
      return res;
    }
    default: {
      return SET<TYPE_REP_TypeRep>();
    }
  }
}

// SeqTypeConc
// stp_s : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::SeqTypeConc (const SET<TYPE_REP_TypeRep> & stp_s)
{
  switch(stp_s.Card ()) {
    case 0: {
      return mk_REP_EmptySeqTypeRep(rep_alltp);
    }
    case 1: {
      return stp_s.GetElem();
    }
    default: {
      SET<TYPE_REP_TypeRep> St(stp_s);
      TYPE_REP_TypeRep St1 (St.GetElem()); St.RemElem (St1);
      TYPE_REP_TypeRep St2 (St.GetElem()); St.RemElem (St2);
  
      TYPE_REP_TypeRep ust (mk_REP_SeqTypeRep(rep_alltp));
      TYPE_REP_TypeRep newtp;
  
      if ((St1 == ust) || (St2 == ust)) {
        return ust;
      }
      else if (St1.Is(TAG_TYPE_REP_EmptySeqTypeRep)) {
        return SeqTypeConc (St.Union(mk_set(St2)));
      }
      else if (St2.Is(TAG_TYPE_REP_EmptySeqTypeRep)) {
        return SeqTypeConc (St.Union(mk_set(St1)));
      }
      else if (St1.Is(TAG_TYPE_REP_SeqTypeRep) && St2.Is(TAG_TYPE_REP_SeqTypeRep))
      {
        newtp = mk_REP_SeqTypeRep(MergeTypes(St1.GetRecord(pos_REP_SeqTypeRep_elemtp),
                                             St2.GetRecord(pos_REP_SeqTypeRep_elemtp)));
      }
      else if (St1.Is(TAG_TYPE_REP_UnionTypeRep) &&
               (St2.Is(TAG_TYPE_REP_SeqTypeRep) || St2.Is(TAG_TYPE_REP_EmptySeqTypeRep)))
      {
        newtp = SeqTypeConc (St1.GetSet(pos_REP_UnionTypeRep_tps).Union(mk_set(St2)));
      }
      else if (St2.Is(TAG_TYPE_REP_UnionTypeRep) &&
               (St1.Is(TAG_TYPE_REP_SeqTypeRep) || St1.Is(TAG_TYPE_REP_EmptySeqTypeRep)))
      {
        newtp = SeqTypeConc (St2.GetSet(pos_REP_UnionTypeRep_tps).Union(mk_set(St1)));
      }
      else if (St1.Is(TAG_TYPE_REP_UnionTypeRep) && St2.Is(TAG_TYPE_REP_UnionTypeRep))
      {
        newtp = SeqTypeConc (St1.GetSet(pos_REP_UnionTypeRep_tps).Union(St2.GetSet(pos_REP_UnionTypeRep_tps)));
      }
      else
      {
        InternalError (L"SeqTypeConc");
        newtp = ust;
      }
  
      if (St.IsEmpty())
        return newtp;
      else
      {
        St.Insert (newtp);
        return SeqTypeConc (St);
      }
    }
  }
}

// SeqMapTypeModify
// seqtp : REP`TypeRep
// maptp : REP`TypeRep
// modtp : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::SeqMapTypeModify (const TYPE_REP_TypeRep & seqtp,
                                                 const TYPE_REP_TypeRep & maptp,
                                                 const TYPE_REP_TypeRep & modtp)
{
  Generic Rng;
  switch(modtp.GetTag()) {
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      Rng = modtp.GetRecord(pos_REP_GeneralMapTypeRep_maprng);
      break;
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      Rng = modtp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng);
      break;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> utps (modtp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> u1tp;
      Generic g;
      for (bool bb = utps.First(g); bb; bb = utps.Next(g))
      {
        TYPE_REP_TypeRep tp (g);
        switch(tp.GetTag()) {
          case TAG_TYPE_REP_GeneralMapTypeRep: {
            u1tp.Insert (tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng));
            break;
          } 
          case TAG_TYPE_REP_InjectiveMapTypeRep: {
            u1tp.Insert (tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng));
            break;
          } 
          default:
            break;
        }
      }
      SET<TYPE_REP_TypeRep> u2tp (MergeNumericTypes (u1tp));
      switch(u2tp.Card ()) {
        case 0: {
          InternalError (L"SeqMapTypeModify");
          Rng = rep_alltp;
          break;
        }
        case 1: {
          Rng = u2tp.GetElem ();
          break;
        }
        default: {
          Rng = mk_REP_UnionTypeRep(u2tp);
          break;
        }
      }
      break;
    }
    default: {
      InternalError (L"SeqMapTypeModify");
      Rng = rep_alltp;
      break;
    }
  }

//  TYPE_REP_TypeRep sqtr (mk_SeqTypeRep(Rng));
  return MergeTypes (MergeTypes (seqtp, mk_REP_SeqTypeRep(Rng)), MergeTypes (maptp, modtp));
}

// RngSubTypeDom
// tps : set of REP`TypeRep
// -> bool
bool StatSem::RngSubTypeDom(const SET<TYPE_REP_TypeRep> & tps_)
{
  SET<TYPE_REP_TypeRep> tps (tps_);
  bool forall = true;
  Generic g;
  for (bool bb = tps.First(g); bb && forall; bb = tps.Next(g))
  {
    TYPE_REP_TypeRep tp (g);
    switch (tp.GetTag ()) {
      case TAG_TYPE_REP_TypeNameRep: {
        if (!IsUnionRecursive(tp)) {
          Generic fntp (TcauxLookUpTypeName (tp.GetRecord(pos_REP_TypeNameRep_nm)));
          if (!fntp.IsNil ()) {
            forall = RngSubTypeDom (mk_set(fntp));
          }
        }
        break;
      }
      case TAG_TYPE_REP_AllTypeRep: {
        break;
      }
      case TAG_TYPE_REP_InvTypeRep: {
        forall = RngSubTypeDom (mk_set(tp.GetRecord(pos_REP_InvTypeRep_shape)));
        break;
      }
      case TAG_TYPE_REP_PartialFnTypeRep: {
        const Generic & dom (tp.GetField(pos_REP_PartialFnTypeRep_fndom));
        if (dom.IsSequence ()) {
          SEQ<TYPE_REP_TypeRep> dom_l (dom);
          if (dom_l.Length () == 1) {
            forall = IsCompatible(DEF, tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng), dom_l.Hd ());
          }
        }
        break;
      }
      case TAG_TYPE_REP_TotalFnTypeRep: {
        const Generic & dom (tp.GetField(pos_REP_TotalFnTypeRep_fndom));
        if (dom.IsSequence ()) {
          SEQ<TYPE_REP_TypeRep> dom_l (dom);
          if (dom_l.Length () == 1) {
            forall = IsCompatible(DEF, tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng), dom_l.Hd ());
          }
        }
        break;
      }
      case TAG_TYPE_REP_GeneralMapTypeRep: {
        forall = IsCompatible(DEF, tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng),
                                   tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom));
        break;
      }
      case TAG_TYPE_REP_InjectiveMapTypeRep: {
        forall = IsCompatible(DEF, tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng),
                                   tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom));
        break;
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        forall = RngSubTypeDom(tp.GetSet(pos_REP_UnionTypeRep_tps));
        break;
      }
      default: {
        break;
      }
    }
  }
  //return true;
  return forall;
}

// ExtractComposeType
// i : TYPE`Ind
// lhstp : REP`TypeRep
// rhstp : REP`TypeRep
// -> bool * REP`TypeRep * REP`TypeRep * REP`TypeRep
Tuple StatSem::ExtractComposeType(const Int & i, const TYPE_REP_TypeRep & lhstp, const TYPE_REP_TypeRep & rhstp)
{
  Generic maptp1 (ExtractMapType(lhstp)); // [REP`TypeRep]
  Generic fntp1 (ExtractFunType(lhstp));  // [REP`TypeRep]
  Generic maptp2 (ExtractMapType(rhstp)); // [REP`TypeRep]
  Generic fntp2 (ExtractFunType(rhstp));  // [REP`TypeRep]

  // mk_(mdom1,mrng1) = SplitMapType(maptp1),
  // -> [REP`TypeRep] * [REP`TypeRep]
  Tuple smt1 (SplitMapType(maptp1));
  const Generic & mdom1 (smt1.GetField (1)); // [REP`TypeRep]
  const Generic & mrng1 (smt1.GetField (2)); // [REP`TypeRep]

  // mk_(mdom2,mrng2) = SplitMapType(maptp2),
  Tuple smt2 (SplitMapType(maptp2));
  const Generic & mdom2 (smt2.GetField (1)); // [REP`TypeRep]
  const Generic & mrng2 (smt2.GetField (2)); // [REP`TypeRep]

  // mk_(fdom1,frng1,tp1ok) = SplitFunType(fntp1),
  // -> [REP`TypeRep] * [REP`TypeRep] * bool
  Tuple sft1 (SplitFunType(fntp1));
  const Generic & fdom1 (sft1.GetField (1)); // [REP`TypeRep]
  const Generic & frng1 (sft1.GetField (2)); // [REP`TypeRep]
  const Bool & tp1ok (sft1.GetBool (3));

  //  mk_(fdom2,frng2,tp2ok) = SplitFunType(fntp2)
  Tuple sft2 (SplitFunType(fntp2));
  const Generic & fdom2 (sft2.GetField (1)); // [REP`TypeRep]
  const Generic & frng2 (sft2.GetField (2)); // [REP`TypeRep]
  const Bool & tp2ok (sft2.GetBool (3));

  bool mcomp = (mrng2.IsNil() || (!mdom1.IsNil() && IsCompatible(i, mrng2, mdom1)));

  bool fcomp = (frng2.IsNil() || (!fdom1.IsNil() && IsCompatible(i, frng2, fdom1)));

  Generic mtp;
  if (mdom2.IsNil () || mrng1.IsNil ())
    mtp = Nil ();
  else {
    mtp = mk_REP_GeneralMapTypeRep(mdom2, mrng1);
  }

  Generic ftp;
  if (fdom2.IsNil () || frng1.IsNil ())
    ftp = Nil ();
  else {
    ftp = mk_REP_PartialFnTypeRep(mk_sequence(fdom2), frng1);
  }

  TYPE_REP_TypeRep rhsrngtp ((mrng2.IsNil () && frng2.IsNil ()) ? rep_alltp : MergeTypes(frng2, mrng2));

  TYPE_REP_TypeRep lhsdomtp ((mdom1.IsNil () && fdom1.IsNil ()) ? rep_alltp : MergeTypes(mdom1, fdom1));

  Bool compok ((i == POS) ? ((mcomp && (!mtp.IsNil () ||
                                        maptp1.Is(TAG_TYPE_REP_EmptyMapTypeRep) ||
                                        maptp2.Is(TAG_TYPE_REP_EmptyMapTypeRep))) ||
                             (fcomp && !ftp.IsNil () && (tp1ok.GetValue () || tp2ok.GetValue ())))
                          : mcomp && fcomp && tp1ok.GetValue() && tp2ok.GetValue());

  Set s (mk_set(mtp, ftp).Diff(mk_set(Nil())));

  TYPE_REP_TypeRep restp;
  switch (s.Card ()) {
    case 0: {
      if (maptp1.Is (TAG_TYPE_REP_EmptyMapTypeRep))
        restp = maptp1;
      else {
        SEQ<TYPE_REP_TypeRep> Sq (mk_sequence(rep_alltp));
        restp = mk_REP_UnionTypeRep(mk_set(mk_REP_GeneralMapTypeRep(rep_alltp, rep_alltp),
                                           mk_REP_InjectiveMapTypeRep(rep_alltp, rep_alltp),
                                           mk_REP_TotalFnTypeRep(Sq, rep_alltp),
                                           mk_REP_PartialFnTypeRep(Sq, rep_alltp)));
      }
      break;
    }
    case 1: {
      restp = s.GetElem ();
      break;
    }
    case 2: {
      Generic t1 (s.GetElem ());
      s.RemElem (t1);
      Generic t2 (s.GetElem ());
      restp = MergeTypes(t1,t2);
      break;
    }
  }
  return mk_(compok, restp, rhsrngtp, lhsdomtp);
}

// SplitMapType
// mtp : [REP`TypeRep]
// -> [REP`TypeRep] * [REP`TypeRep]
Tuple StatSem::SplitMapType(const Generic & mtp) const
{
  if (mtp.IsNil ())
    return mk_(Nil(), Nil());

  TYPE_REP_TypeRep tp (mtp);
  switch (tp.GetTag ()) {
    case TAG_TYPE_REP_EmptyMapTypeRep: {
// 20111213 -->
      //return mk_(Nil(), Nil());
      return mk_(tp.GetRecord(pos_REP_EmptyMapTypeRep_mapdom), tp.GetRecord(pos_REP_EmptyMapTypeRep_maprng));
// <-- 20111213
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return mk_(tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom), tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng));
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return mk_(tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom), tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng));
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> utps (tp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> d1tp, r1tp;
      Generic g;
      for (bool bb = utps.First (g); bb; bb = utps.Next (g)) {
        TYPE_REP_TypeRep rtp (g);
        switch(rtp.GetTag()) {
          case TAG_TYPE_REP_GeneralMapTypeRep: {
            d1tp.Insert (rtp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom));
            r1tp.Insert (rtp.GetRecord(pos_REP_GeneralMapTypeRep_maprng));
          }
          case TAG_TYPE_REP_InjectiveMapTypeRep: {
            d1tp.Insert (rtp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom));
            r1tp.Insert (rtp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng));
          }
          default:
            break;
        }
      }
      SET<TYPE_REP_TypeRep> d2tp (MergeNumericTypes(d1tp));
      SET<TYPE_REP_TypeRep> r2tp (MergeNumericTypes(r1tp));
  
      TYPE_REP_TypeRep d;
      switch (d2tp.Card ()) {
        case 0: {
          InternalError (L"SplitMapType");
          return Tuple (0); // to avoid warnings
        }
        case 1: {
          d = d2tp.GetElem ();
          break;
        }
        default: {
          d = mk_REP_UnionTypeRep(d2tp);
          break;
        }
      }
  
      TYPE_REP_TypeRep r;
      switch (r2tp.Card ()) {
        case 0: {
          InternalError (L"SplitMapType");
          return Tuple (0); // to avoid warnings
        }
        case 1: {
          r = r2tp.GetElem ();
          break;
        }
        default: {
          r = mk_REP_UnionTypeRep(r2tp);
          break;
        }
      }
      return mk_(d, r);
    }
    default: {
      return Tuple (0); // to avoid warnings
    }
  }
}

// SplitFunType
// mtp : [REP`TypeRep]
// -> [REP`TypeRep] * [REP`TypeRep] * bool
Tuple StatSem::SplitFunType(const Generic & mtp) const
{
  Nil nil;
  if (mtp.IsNil ())
    return mk_(Nil(), Nil(), Bool(true));

  TYPE_REP_TypeRep tp (mtp);
  switch (tp.GetTag ()) {
    case TAG_TYPE_REP_TotalFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & Dom (tp.GetSequence(pos_REP_TotalFnTypeRep_fndom));
      switch (Dom.Length ()) {
        case 1: {
          return mk_(Dom.Hd (), tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng), Bool(true));
        }
        case 0: {
          return mk_(rep_alltp, tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng), Bool(false));
        }
        default: {
          return mk_(Dom.Hd (), tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng), Bool(false));
        }
      }
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & Dom (tp.GetSequence(pos_REP_PartialFnTypeRep_fndom));
      switch (Dom.Length ()) {
        case 1: {
          return mk_(Dom.Hd (), tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng), Bool(true));
        }
        case 0: {
          return mk_(rep_alltp, tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng), Bool(false));
        }
        default: {
          return mk_(Dom.Hd (), tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng), Bool(false));
        }
      }
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> utps (tp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<Sequence> d1tp;
      SET<TYPE_REP_TypeRep> r1tp;
      Generic g;
      for (bool bb = utps.First (g); bb; bb = utps.Next (g)) {
        TYPE_REP_TypeRep rtp (g);
        switch(rtp.GetTag()) {
          case TAG_TYPE_REP_TotalFnTypeRep: {
            d1tp.Insert (rtp.GetField(pos_REP_TotalFnTypeRep_fndom));
            r1tp.Insert (rtp.GetRecord(pos_REP_TotalFnTypeRep_fnrng));
            break;
          }
          case TAG_TYPE_REP_PartialFnTypeRep: {
            d1tp.Insert (rtp.GetField(pos_REP_PartialFnTypeRep_fndom));
            r1tp.Insert (rtp.GetRecord(pos_REP_PartialFnTypeRep_fnrng));
            break;
          }
          default:
            break;
        }
      }
      SET<TYPE_REP_TypeRep> d1tp_; // (MergeNumericTypes(d1tp));
      bool allok = true;
      for (bool cc = d1tp.First (g); cc; cc = d1tp.Next (g)) {
        SEQ<TYPE_REP_TypeRep> tp_l (g);
        if (tp_l.Length () == 1)
          d1tp_.Insert (tp_l.Hd ());
        else
          allok = false;
      }
      SET<TYPE_REP_TypeRep> d2tp (MergeNumericTypes(d1tp_));
      SET<TYPE_REP_TypeRep> r2tp (MergeNumericTypes(r1tp));
  
      TYPE_REP_TypeRep d;
      switch (d2tp.Card ()) {
        case 0: {
          InternalError (L"SplitFunType");
          return Tuple (0); // to avoid warnings
        }
        case 1: {
          d = d2tp.GetElem ();
          break;
        }
        default: {
          d = mk_REP_UnionTypeRep(d2tp);
          break;
        }
      }
  
      TYPE_REP_TypeRep r;
      switch (r2tp.Card ()) {
        case 0: {
          InternalError (L"SplitFunType");
          return Tuple (0); // to avoid warnings
        }
        case 1: {
          r = r2tp.GetElem ();
          break;
        }
        default: {
          r = mk_REP_UnionTypeRep(r2tp);
          break;
        }
      }
      return mk_(d, r, Bool(allok));
    }
    default: {
      return Tuple (0); // to avoid warnings
    }
  }
}

// ExtractFunType
// sttp : REP`TypeRep
// -> [REP`TypeRep]
Generic StatSem::ExtractFunType (const TYPE_REP_TypeRep & sttp)
{
  SET<TYPE_REP_TypeRep> r1tp (ExtractFunTypeInner(sttp));
  switch (r1tp.Card()) {
    case 0: {
      return Nil ();
    }
    case 1: {
      return r1tp.GetElem();
    }
    default: {
      return mk_REP_UnionTypeRep(r1tp);
    }
  }
}

// ExtractFunTypeInner
// sttp : REP`TypeRep
// -> set of REP`TypeRep
SET<TYPE_REP_TypeRep> StatSem::ExtractFunTypeInner (const TYPE_REP_TypeRep & sttp)
{
  switch (sttp.GetTag ()) {
    case TAG_TYPE_REP_TypeNameRep: {
      if (IsUnionRecursive (sttp))
        return SET<TYPE_REP_TypeRep>();
      else {
        Generic tp (TcauxLookUpTypeName (sttp.GetRecord(pos_REP_TypeNameRep_nm)));
        if (tp.IsNil())
          return SET<TYPE_REP_TypeRep>();
        else
          return ExtractFunTypeInner (tp);
      }
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return ExtractFunTypeInner (sttp.GetRecord(pos_REP_InvTypeRep_shape));
    }
    case TAG_TYPE_REP_AllTypeRep: {
      SEQ<TYPE_REP_TypeRep> seq;
      seq.ImpAppend (rep_alltp);
      SET<TYPE_REP_TypeRep> res;
      res.Insert(mk_REP_PartialFnTypeRep(seq, rep_alltp));
      return res;
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      SET<TYPE_REP_TypeRep> res;
      res.Insert(sttp);
      return res;
    }
    case TAG_TYPE_REP_TotalFnTypeRep: {
      SET<TYPE_REP_TypeRep> res;
      res.Insert(sttp);
      return res;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> Utp (sttp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> res;
      Generic i;
      for (bool bb = Utp.First(i); bb ; bb = Utp.Next(i))
        res.ImpUnion (ExtractFunTypeInner(i));
      return res;
    }
    default: {
      return SET<TYPE_REP_TypeRep>();
    }
  }
}

// UnmaskMapDomType
// i : TYPE`Ind
// tp : REP`TypeRep
// -> [REP`TypeRep]
Generic StatSem::UnmaskMapDomType(const Int & i, const TYPE_REP_TypeRep & tp)
{
  switch (tp.GetTag()) {
    case TAG_TYPE_REP_EmptyMapTypeRep: {
// 20130108 -->
      //return tp.GetRecord(pos_REP_EmptyMapTypeRep_mapdom);
      return Nil();
// <-- 20130108
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom);
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tps (tp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> mdtps;
      Generic t;
      for (bool bb = tps.First(t); bb; bb = tps.Next(t))
      {
        Generic gtp (UnmaskMapDomType(i, t));
        if (!gtp.IsNil())
          mdtps.Insert(gtp);
      }
      switch (mdtps.Card()) {
        case 0: { return Nil(); }
        case 1: { return mdtps.GetElem(); }
        default : { return mk_REP_UnionTypeRep(mdtps); }
      }
    }
    case TAG_TYPE_REP_AllTypeRep: {
      return tp;
    }
    default: {
      return Nil();
    }
  }
}

// UnmaskMapRngType
// i : TYPE`Ind
// tp : REP`TypeRep
// -> [REP`TypeRep]
Generic StatSem::UnmaskMapRngType(const Int & i, const TYPE_REP_TypeRep & tp)
{
  switch (tp.GetTag()) {
    case TAG_TYPE_REP_EmptyMapTypeRep: {
// 20130108 -->
      //return tp.GetRecord(pos_REP_EmptyMapTypeRep_maprng);
      return Nil();
// <-- 20130108
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng);
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tps (tp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> mrtps;
      Generic t;
      for (bool bb = tps.First(t); bb; bb = tps.Next(t))
      {
        Generic gtp (UnmaskMapRngType(i, t));
        if (!gtp.IsNil())
          mrtps.Insert(gtp);
      }
      switch (mrtps.Card()) {
        case 0: { return Nil(); }
        case 1: { return mrtps.GetElem(); }
        default : { return mk_REP_UnionTypeRep(mrtps); }
      }
    }
    case TAG_TYPE_REP_AllTypeRep: {
      return tp;
    }
    default: {
      return Nil();
    }
  }
}

// ExtractMapType
// sttp : REP`TypeRep
// -> [REP`TypeRep]
Generic StatSem::ExtractMapType (const TYPE_REP_TypeRep & sttp)
{
  SET<TYPE_REP_TypeRep> r1tp (ExtractMapTypeInner(sttp));
  switch (r1tp.Card()) {
    case 0: {
      return Nil ();
    }
    case 1: {
      return r1tp.GetElem();
    }
    default: {
      return mk_REP_UnionTypeRep(r1tp);
    }
  }
}

// ExtractMapTypeInner
// sttp : REP`TypeRep
// -> set of REP`TypeRep
SET<TYPE_REP_TypeRep> StatSem::ExtractMapTypeInner (const TYPE_REP_TypeRep & sttp)
{
  switch (sttp.GetTag ()) {
    case TAG_TYPE_REP_TypeNameRep: {
      if (IsUnionRecursive (sttp))
        return SET<TYPE_REP_TypeRep>();
      else {
        Generic tp (TcauxLookUpTypeName (sttp.GetRecord(pos_REP_TypeNameRep_nm)));
        if (tp.IsNil())
          return SET<TYPE_REP_TypeRep>();
        else
          return ExtractMapTypeInner (tp);
      }
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return ExtractMapTypeInner (sttp.GetRecord(pos_REP_InvTypeRep_shape));
    }
    case TAG_TYPE_REP_TypeVarRep:
    case TAG_TYPE_REP_AllTypeRep: {
      return mk_set(mk_REP_GeneralMapTypeRep(rep_alltp, rep_alltp),
                    mk_REP_InjectiveMapTypeRep(rep_alltp, rep_alltp),
                    mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp));
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return mk_set(sttp);
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return mk_set(sttp);
    }
    case TAG_TYPE_REP_EmptyMapTypeRep: {
      return mk_set(sttp);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> Utp (sttp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> res;
      Generic tp;
      for (bool bb = Utp.First(tp); bb ; bb = Utp.Next(tp))
        res.ImpUnion (ExtractMapTypeInner(tp));
      return res;
    }
    default: {
      return SET<TYPE_REP_TypeRep>();
    }
  }
}

// MapTypeMerge
// St : set of [REP`TypeRep]
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::MapTypeMerge (const SET<TYPE_REP_TypeRep> & St)
{
  switch(St.Card ()) {
    case 0: {
      return mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp);
    }
    case 1: {
      Generic tp (St.GetElem());
      if (tp.IsNil ())
        return mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp);
      else
        return tp;
    }
    default: { // St.Card () > 1
      SET<TYPE_REP_TypeRep> St_q (St);
      TYPE_REP_TypeRep St1 (St_q.GetElem()); St_q.RemElem (St1);
      TYPE_REP_TypeRep St2 (St_q.GetElem()); St_q.RemElem (St2);
  
      TYPE_REP_TypeRep ust (mk_REP_GeneralMapTypeRep(rep_alltp,rep_alltp));
      TYPE_REP_TypeRep newtp;
  
      if ((St1 == ust) || (St2 == ust))
        newtp = ust;
      else if (St1.Is(TAG_TYPE_REP_EmptyMapTypeRep))
        newtp = St2;
      else if (St2.Is(TAG_TYPE_REP_EmptyMapTypeRep))
        newtp = St1;
//      else if ((St1.Is(TAG_TYPE_REP_GeneralMapTypeRep) &&
//                St2.Is(TAG_TYPE_REP_GeneralMapTypeRep)) ||
//               (St1.Is(TAG_TYPE_REP_GeneralMapTypeRep) &&
//                St2.Is(TAG_TYPE_REP_InjectiveMapTypeRep)) ||
//               (St1.Is(TAG_TYPE_REP_InjectiveMapTypeRep) &&
//                St2.Is(TAG_TYPE_REP_GeneralMapTypeRep)) ||
//               (St1.Is(TAG_TYPE_REP_InjectiveMapTypeRep) &&
//                St2.Is(TAG_TYPE_REP_InjectiveMapTypeRep)))
      else if ((St1.Is(TAG_TYPE_REP_GeneralMapTypeRep) || St1.Is(TAG_TYPE_REP_InjectiveMapTypeRep)) &&
               (St2.Is(TAG_TYPE_REP_GeneralMapTypeRep) || St2.Is(TAG_TYPE_REP_InjectiveMapTypeRep)))
      {
        const TYPE_REP_TypeRep & d1 (St1.GetRecord (1));
        const TYPE_REP_TypeRep & r1 (St1.GetRecord (2));
        const TYPE_REP_TypeRep & d2 (St2.GetRecord (1));
        const TYPE_REP_TypeRep & r2 (St2.GetRecord (2));
  
        newtp = mk_REP_GeneralMapTypeRep(MergeTypes(d1, d2), MergeTypes(r1, r2));
      }
//      else if ((St1.Is(TAG_TYPE_REP_UnionTypeRep) &&
//                St2.Is(TAG_TYPE_REP_GeneralMapTypeRep)) ||
//               (St1.Is(TAG_TYPE_REP_UnionTypeRep) &&
//                St2.Is(TAG_TYPE_REP_InjectiveMapTypeRep)))
      else if (St1.Is(TAG_TYPE_REP_UnionTypeRep) &&
               (St2.Is(TAG_TYPE_REP_GeneralMapTypeRep) || St2.Is(TAG_TYPE_REP_InjectiveMapTypeRep)))
      {
        SET<TYPE_REP_TypeRep> utps (St1.GetSet(pos_REP_UnionTypeRep_tps));
        utps.Insert (St2);
        newtp = MapTypeMerge (utps);
      }
//      else if ((St2.Is(TAG_TYPE_REP_UnionTypeRep) &&
//                St1.Is(TAG_TYPE_REP_GeneralMapTypeRep)) ||
//               (St2.Is(TAG_TYPE_REP_UnionTypeRep) &&
//                St1.Is(TAG_TYPE_REP_InjectiveMapTypeRep)))
      else if (St2.Is(TAG_TYPE_REP_UnionTypeRep) &&
               (St1.Is(TAG_TYPE_REP_GeneralMapTypeRep) || St1.Is(TAG_TYPE_REP_InjectiveMapTypeRep)))
      {
        SET<TYPE_REP_TypeRep> utps (St2.GetSet(pos_REP_UnionTypeRep_tps));
        utps.Insert (St1);
        newtp = MapTypeMerge (utps);
      }
      else if (St1.Is(TAG_TYPE_REP_UnionTypeRep) && St2.Is(TAG_TYPE_REP_UnionTypeRep))
      {
        SET<TYPE_REP_TypeRep> ul (St1.GetSet(pos_REP_UnionTypeRep_tps));
        const SET<TYPE_REP_TypeRep> & ur (St2.GetSet(pos_REP_UnionTypeRep_tps));
        ul.ImpUnion (ur);
        newtp = MapTypeMerge (ul);
      }
      else
      {
        InternalError (L"MapTypeMerge");
        newtp = ust;
      }
  
      if (St.IsEmpty())
      {
        if (IsCompatible(POS, newtp, mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp))) {
          return newtp;
        }
        else
        {
          if (newtp.Is(TAG_TYPE_REP_UnionTypeRep))
          {
            SET<TYPE_REP_TypeRep> uts (newtp.GetSet(pos_REP_UnionTypeRep_tps));
            uts.Insert(mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp));
            newtp.SetField(1, uts);
            return newtp;
          }
          else
          {
            SET<TYPE_REP_TypeRep> uts;
            uts.Insert(newtp)
               .Insert(mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp));
            return mk_REP_UnionTypeRep(uts);
          }
        }
      }
      else
      {
        St_q.Insert (newtp);
        return MapTypeMerge (St_q);
      }
    }
  }
}

// MapTypeDomRestrict
// St : REP`TypeRep
// mt : [REP`TypeRep]
// -> REP`TypeRep * (<OK> | <NOTOVERLAP> | <EMPTYMAP>)
Tuple StatSem::MapTypeDomRestrict (const TYPE_REP_TypeRep & St, const Generic & mt)
{
  Generic s1tp;
  switch(St.GetTag()) {
    case TAG_TYPE_REP_EmptySetTypeRep: {
      s1tp = Nil();
      break;
    }
    case TAG_TYPE_REP_SetTypeRep: {
      s1tp = St.GetRecord(pos_REP_SetTypeRep_elemtp);
      break;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> utps (St.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> u1tp;
      Generic u;
      for (bool bb = utps.First(u) ; bb ; bb = utps.Next(u))
      {
        TYPE_REP_TypeRep tp (u);
        if (tp.Is(TAG_TYPE_REP_SetTypeRep))
          u1tp.Insert(tp.GetRecord(pos_REP_SetTypeRep_elemtp));
      }

      SET<TYPE_REP_TypeRep> u2tp (MergeNumericTypes(u1tp));
      switch(u2tp.Card()) {
        case 0: {
          InternalError (L"MapTypeDomRestrict1");
          break;
        }
        case 1: {
          s1tp = u2tp.GetElem();
          break;
        }
        default: {
          s1tp = mk_REP_UnionTypeRep(u2tp);
          break;
        }
      }
      break;
    }
    default: {
      InternalError (L"MapTypeDomRestrict2");
      break;
    }
  }

  Generic m1tp;
  TYPE_REP_TypeRep Mt;
  if (mt.IsNil ())
    m1tp = Nil();
  else {
    Mt = mt;
    switch(Mt.GetTag()) {
      case TAG_TYPE_REP_EmptyMapTypeRep: {
        m1tp = Nil();
        break;
      }
      case TAG_TYPE_REP_GeneralMapTypeRep: {
        m1tp = Mt.GetRecord(pos_REP_GeneralMapTypeRep_mapdom);
        break;
      }
      case TAG_TYPE_REP_InjectiveMapTypeRep: {
        m1tp = Mt.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom);
        break;
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        SET<TYPE_REP_TypeRep> utps (Mt.GetSet(pos_REP_UnionTypeRep_tps)), u1tp;
        Generic u;
        for (bool bb = utps.First(u) ; bb ; bb = utps.Next(u))
        {
          TYPE_REP_TypeRep tp (u);
          switch(tp.GetTag()) {
            case TAG_TYPE_REP_GeneralMapTypeRep: {
              u1tp.Insert(tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom));
              break;
            }
            case TAG_TYPE_REP_InjectiveMapTypeRep: {
              u1tp.Insert(tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom));
              break;
            }
            default:
              break;
          }
        }

        SET<TYPE_REP_TypeRep> u2tp (MergeNumericTypes(u1tp));
        switch(u2tp.Card()) {
          case 0: {
            InternalError (L"MapTypeDomRestrict3");
            break;
          }
          case 1: {
            m1tp = u2tp.GetElem();
            break;
          }
          default: {
            m1tp = mk_REP_UnionTypeRep(u2tp);
            break;
          }
        }
        break;
      }
      default: {
        InternalError (L"MapTypeDomRestrict4");
        break;
      }
    }
  }

  if (m1tp.IsNil())
    return mk_(mt, (Int)EMPTYMAP);
  else if (s1tp.IsNil() || IsOverlapping (s1tp, m1tp, Set()))
  {
    if (Mt.Is(TAG_TYPE_REP_UnionTypeRep))
    {
      SET<TYPE_REP_TypeRep> u (Mt.GetSet(pos_REP_UnionTypeRep_tps));
      u.Insert(mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp));
      return mk_(mk_REP_UnionTypeRep(u), (Int)OK);
    }
    else
    {
      SET<TYPE_REP_TypeRep> tmp2;
      tmp2.Insert(Mt);
      tmp2.Insert(mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp));
      return mk_(mk_REP_UnionTypeRep(tmp2), (Int)OK);
    }
  }
  else
    return mk_(Mt, (Int)NOTOVERLAP);
}

// MapTypeRngRestrict
// mt : [REP`TypeRep]
// St : REP`TypeRep
// -> REP`TypeRep * (<OK> | <NOTOVERLAP> | <EMPTYMAP>)
Tuple StatSem::MapTypeRngRestrict (const Generic & mt, const TYPE_REP_TypeRep & St)
{
  Generic s1tp;
  switch(St.GetTag()) {
    case TAG_TYPE_REP_EmptySetTypeRep: {
      s1tp = Nil();
      break;
    }
    case TAG_TYPE_REP_SetTypeRep: {
      s1tp = St.GetRecord(pos_REP_SetTypeRep_elemtp);
      break;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> utps (St.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> u1tp;
      Generic u;
      for (bool bb = utps.First(u) ; bb ; bb = utps.Next(u))
      {
        TYPE_REP_TypeRep tp (u);
        if (tp.Is(TAG_TYPE_REP_SetTypeRep))
          u1tp.Insert(tp.GetRecord(pos_REP_SetTypeRep_elemtp));
      }

      SET<TYPE_REP_TypeRep> u2tp (MergeNumericTypes(u1tp));
      switch(u2tp.Card()) {
        case 0: {
          InternalError (L"MapTypeRngRestrict1");
          break;
        }
        case 1: {
          s1tp = u2tp.GetElem();
          break;
        }
        default: {
          s1tp = mk_REP_UnionTypeRep(u2tp);
          break;
        }
      }
      break;
    }
    default: {
      InternalError (L"MapTypeRngRestrict2");
      break;
    }
  }

  Generic m1tp;
  TYPE_REP_TypeRep Mt;
  if (mt.IsNil ())
    m1tp = Nil ();
  else {
    Mt = mt;
    switch(Mt.GetTag()) {
      case TAG_TYPE_REP_EmptyMapTypeRep: {
        m1tp = Nil();
        break;
      }
      case TAG_TYPE_REP_GeneralMapTypeRep: {
        m1tp = Mt.GetRecord(pos_REP_GeneralMapTypeRep_maprng);
        break;
      }
      case TAG_TYPE_REP_InjectiveMapTypeRep: {
        m1tp = Mt.GetRecord(pos_REP_InjectiveMapTypeRep_maprng);
        break;
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        SET<TYPE_REP_TypeRep> utps (Mt.GetSet(pos_REP_UnionTypeRep_tps));
        SET<TYPE_REP_TypeRep> u1tp;
        Generic u;
        for (bool bb = utps.First(u) ; bb ; bb = utps.Next(u))
        {
          TYPE_REP_TypeRep tp (u);
          switch(tp.GetTag()) {
            case TAG_TYPE_REP_GeneralMapTypeRep: {
              u1tp.Insert(tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng));
              break;
            }
            case TAG_TYPE_REP_InjectiveMapTypeRep: {
              u1tp.Insert(tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng));
              break;
            }
            default:
              break;
          }
        }

        SET<TYPE_REP_TypeRep> u2tp (MergeNumericTypes(u1tp));
        switch(u2tp.Card()) {
          case 0: {
            InternalError (L"MapTypeRngRestrict3");
            break;
          }
          case 1: {
            m1tp = u2tp.GetElem();
            break;
          }
          default: {
            m1tp = mk_REP_UnionTypeRep(u2tp);
            break;
          }
        }
        break;
      }
      default: {
        InternalError (L"MapTypeRngRestrict4");
        break;
      }
    }
  }

  if (m1tp.IsNil())
    return mk_(mt, (Int)EMPTYMAP);
  else if (s1tp.IsNil() || IsOverlapping (s1tp, m1tp, Set()))
  {
    if (Mt.Is(TAG_TYPE_REP_UnionTypeRep))
    {
      SET<TYPE_REP_TypeRep> u (Mt.GetSet(pos_REP_UnionTypeRep_tps));
      u.Insert(mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp));
      return mk_(mk_REP_UnionTypeRep(u), (Int)OK);
    }
    else
    {
      return mk_(mk_REP_UnionTypeRep(mk_set(Mt, mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp))), (Int)OK);
    }
  }
  else
    return mk_(Mt, (Int)NOTOVERLAP);
}

// InverseMapType
// Tp : [REP`TypeRep]
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::InverseMapType (const Generic & Tp) const
{
  if (Tp.IsNil ())
    return mk_REP_EmptyMapTypeRep(rep_alltp, rep_alltp);

  TYPE_REP_TypeRep tp (Tp);
  switch (tp.GetTag()) {
    case TAG_TYPE_REP_EmptyMapTypeRep: {
      return tp;
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return mk_REP_GeneralMapTypeRep(tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng),
                                      tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom));
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return mk_REP_InjectiveMapTypeRep(tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng),
                                        tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom));
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> utps (tp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> st;
      Generic g;
      for (bool bb = utps.First(g); bb; bb = utps.Next(g))
      {
        if (g.Is(TAG_TYPE_REP_GeneralMapTypeRep) || g.Is(TAG_TYPE_REP_InjectiveMapTypeRep))
        st.Insert (InverseMapType(g));
      }
      return mk_REP_UnionTypeRep(st);
    }
    case TAG_TYPE_REP_AllTypeRep: {
      return rep_alltp;
    }
    default: {
      InternalError (L"InverseMapType");
      return tp;
    }
  }
}

// ExtractCompositeType
// sttp : REP`TypeRep
// -> [REP`TypeRep]
Generic StatSem::ExtractCompositeType (const TYPE_REP_TypeRep & sttp)
{
  SET<TYPE_REP_TypeRep> r1tp (ExtractCompositeTypeInner(sttp));
  switch(r1tp.Card()) {
    case 0: {
      return Nil();
    }
    case 1: {
      return r1tp.GetElem();
    }
    default: {
      return mk_REP_UnionTypeRep(r1tp);
    }
  }
}

// ExtractCompositeTypeInner
// sttp : REP`TypeRep
// -> set of REP`TypeRep
SET<TYPE_REP_TypeRep> StatSem::ExtractCompositeTypeInner (const TYPE_REP_TypeRep & sttp)
{
  switch(sttp.GetTag()) {
    case TAG_TYPE_REP_TypeNameRep: {
      if (IsUnionRecursive (sttp))
        return SET<TYPE_REP_TypeRep>();
      else
      {
        Generic tp (TcauxLookUpTypeName (sttp.GetRecord(pos_REP_TypeNameRep_nm)));
        if (tp.IsNil())
          return SET<TYPE_REP_TypeRep>();
        else {
          return ExtractCompositeTypeInner (tp);
        }
      }
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return ExtractCompositeTypeInner (sttp.GetRecord(pos_REP_InvTypeRep_shape));
    }
    case TAG_TYPE_REP_AllTypeRep: {
      SET<TYPE_REP_TypeRep> res;
      res.Insert(mk_REP_CompositeTypeRep(ASTAUX::MkNameFromVoid(), SEQ<TYPE_REP_FieldRep>()));
      return res;
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
      SET<TYPE_REP_TypeRep> res;
      res.Insert(sttp);
      return res;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> Utp (sttp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> res;
      Generic i;
      for (bool bb = Utp.First(i); bb ; bb = Utp.Next(i))
        res.ImpUnion (ExtractCompositeTypeInner(i));
      return res;
    }
    default:
      return SET<TYPE_REP_TypeRep>();
  }
}

// CompareProductLength
//
bool StatSem::CompareProductLength (int s, int n, int op) const
{
  switch(op) {
    case EQ:
      return (s == n);
    case NUMGE:
      return true; // was s >= n;
    default:
      return false;
  }
}

// ExtractProductType
// sttp : REP`TypeRep
// n : nat
// op : (<EQ>|<GE>)
// -> [REP`ProductTypeRep]
//TYPE_REP_ProductTypeRep StatSem::ExtractProductType (const TYPE_REP_TypeRep & sttp, int n, int op)
Generic StatSem::ExtractProductType (const TYPE_REP_TypeRep & sttp, int n, int op)
{
  Set r1tp (ExtractProductTypeInner(sttp,n,op));
  switch(r1tp.Card()) {
    case 0: {
      //return TYPE_REP_ProductTypeRep().Init(SEQ<TYPE_REP_TypeRep>());
      return Nil();
    }
    case 1: {
      return r1tp.GetElem();
    }
    default: {
      return TYPE_REP_ProductTypeRep().Init(MakeOneProductTypeRep (r1tp));
    }
  }
}

// ExtractProductTypeInner
// sttp : REP`TypeRep
// n : nat
// op : (<EQ>|<GE>)
// -> set of REP`ProductTypeRep
SET<TYPE_REP_TypeRep> StatSem::ExtractProductTypeInner (const TYPE_REP_TypeRep & sttp, int n, int op)
{
  switch (sttp.GetTag()) {
    case TAG_TYPE_REP_TypeNameRep: {
      if (IsUnionRecursive (sttp))
        return SET<TYPE_REP_TypeRep>();
      else
      {
        Generic tp (TcauxLookUpTypeName (sttp.GetRecord(pos_REP_TypeNameRep_nm)));
        if (tp.IsNil())
          return SET<TYPE_REP_TypeRep>();
        else
          return ExtractProductTypeInner (tp, n, op);
      }
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return ExtractProductTypeInner (sttp.GetRecord(pos_REP_InvTypeRep_shape), n, op);
    }
    case TAG_TYPE_REP_AllTypeRep: {
      SEQ<TYPE_REP_TypeRep> sq;
      for (int j = 1; j <= n; j++)
        sq.ImpAppend (rep_alltp);

      SET<TYPE_REP_TypeRep> res;
      res.Insert(mk_REP_ProductTypeRep(sq));
      return res;
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & s (sttp.GetSequence(pos_REP_ProductTypeRep_tps));
      SET<TYPE_REP_TypeRep> res;
      if (CompareProductLength (s.Length (), n, op))
      //      if (s.Length() == n)
      //      if (s.Length() >= n)
        res.Insert(sttp);
      return res;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> Utp (sttp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> res;
      Generic i;
      for (bool bb = Utp.First(i); bb ; bb = Utp.Next(i))
        res.ImpUnion (ExtractProductTypeInner(i, n, op));
      return res;
    }
    default:
      return SET<TYPE_REP_TypeRep>();
  }
}

#ifdef VDMPP
// ExtractSetObjRefType
// stp : REP`TypeRep
// return : [REP`TypeRep]
Generic StatSem::ExtractSetObjRefType (const TYPE_REP_TypeRep & stp)
{
  TYPE_REP_TypeRep sstp (ExtractSetType(stp));
  switch (sstp.GetTag()) {
    case TAG_TYPE_REP_SetTypeRep: {
      return ExtractObjRefType(sstp.GetRecord(pos_REP_SetTypeRep_elemtp));
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> utps (sstp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> utps2;
      Generic g;
      for (bool bb = utps.First(g); bb; bb = utps.Next(g))
      {
        TYPE_REP_TypeRep tp (g);
        if (tp.Is(TAG_TYPE_REP_SetTypeRep)) {
          Generic tp_g (ExtractObjRefType(tp.GetRecord(pos_REP_SetTypeRep_elemtp)));
          if (!tp_g.IsNil())
            utps2.Insert(tp_g);
        }
      }
      switch (utps2.Card()) {
        case 0: {
          return Nil();
        }
        case 1: {
          return utps2.GetElem();
        }
        default: {
          sstp.SetField(1,utps2);
          return sstp;
        }
      }
    }
    default: {
      return Nil();
    }
  }
}

// ExtractObjRefType
// sttp : REP`TypeRep
// -> [REP`TypeRep]
Generic StatSem::ExtractObjRefType (const TYPE_REP_TypeRep & sttp)
{
  SET<TYPE_REP_TypeRep> r1tp (ExtractObjectTypeInner(sttp));
  switch(r1tp.Card()) {
    case 0: {
      return Nil();
    }
    case 1: {
      return r1tp.GetElem();
    }
    default: {
      return mk_REP_UnionTypeRep(r1tp);
    }
  }
}

// ExtractObjectTypeInner
// sttp : REP`TypeRep
// -> set of REP`TypeRep
SET<TYPE_REP_TypeRep> StatSem::ExtractObjectTypeInner (const TYPE_REP_TypeRep & sttp)
{
  switch (sttp.GetTag()) {
    case TAG_TYPE_REP_TypeNameRep: {
      if (IsUnionRecursive (sttp))
        return SET<TYPE_REP_TypeRep>();
      else
      {
        Generic tp (TcauxLookUpTypeName (sttp.GetRecord(pos_REP_TypeNameRep_nm)));
        if (tp.IsNil())
          return SET<TYPE_REP_TypeRep>();
        else
          return ExtractObjectTypeInner (tp);
      }
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return ExtractObjectTypeInner (sttp.GetRecord(pos_REP_InvTypeRep_shape));
    }
    case TAG_TYPE_REP_ObjRefTypeRep: {
      SET<TYPE_REP_TypeRep> res;
      res.Insert(sttp);
      return res;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> Utp (sttp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> res;
      Generic i;
      for (bool bb = Utp.First(i); bb ; bb = Utp.Next(i))
        res.ImpUnion (ExtractObjectTypeInner(i));
      return res;
    }
    default: {
      return SET<TYPE_REP_TypeRep>();
    }
  }
}
#endif //VDMPP

// MakeOneProductTypeRep
// prod_s : set of REP`ProductTypeRep
// -> seq1 of REP`TypeRep
SEQ<TYPE_REP_TypeRep> StatSem::MakeOneProductTypeRep (const SET<TYPE_REP_ProductTypeRep> & prod_s)
{

  if (prod_s.Card () == 1)
  {
    TYPE_REP_ProductTypeRep ptp (prod_s.GetElem());
    return ptp.GetSequence(pos_REP_ProductTypeRep_tps);
  }
  else
  {
    TYPE_REP_ProductTypeRep ptp (prod_s.GetElem());
    SEQ<TYPE_REP_TypeRep> restseq (MakeOneProductTypeRep (prod_s.Diff(mk_set(ptp))));
    const SEQ<TYPE_REP_TypeRep> & seq (ptp.GetSequence(pos_REP_ProductTypeRep_tps));
    size_t mx = restseq.Length ();
    if (mx < (size_t)(seq.Length ()))
      mx = seq.Length ();

    SEQ<TYPE_REP_TypeRep> res;
    for (size_t j = 1; j <= mx; j++)
    {
      if (j > (size_t)(seq.Length ()))
        res.ImpAppend (restseq[j]);
      else if (j > (size_t)(restseq.Length ()))
        res.ImpAppend (seq[j]);
      else
        res.ImpAppend (MergeTypes (seq[j], restseq[j]));
    }
    return res;
  }
}

// InstFn
// kind : (<PAR> | <VAR> | <REN>)
// tp : REP`TypeRep
// bind : map AS`Name to REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::InstFn (const Int & kind,
                                  const TYPE_REP_TypeRep & tp,
                                  const MAP<TYPE_AS_Name,TYPE_REP_TypeRep> & bind) const
{
  switch (tp.GetTag()) {
    case TAG_TYPE_REP_AllTypeRep:
    case TAG_TYPE_REP_UnitTypeRep:
    case TAG_TYPE_REP_NilTypeRep:
    case TAG_TYPE_REP_BooleanTypeRep:
    case TAG_TYPE_REP_NumericTypeRep:
    case TAG_TYPE_REP_TokenTypeRep:
    case TAG_TYPE_REP_CharTypeRep:
    case TAG_TYPE_REP_QuoteTypeRep: {
      return tp;
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return mk_REP_InvTypeRep(tp.GetRecord(pos_REP_InvTypeRep_name),
                               InstFn (kind, tp.GetRecord(pos_REP_InvTypeRep_shape), bind),
                               tp.GetField(pos_REP_InvTypeRep_invariant));
    }
    case TAG_TYPE_REP_TypeNameRep: {
      const TYPE_AS_Name & nm (tp.GetRecord(pos_REP_TypeNameRep_nm));
      if (((kind == REN) || (kind == PAR)) && (bind.DomExists (nm)))
        return bind[nm];
      else
        return tp;
    }
    case TAG_TYPE_REP_TypeVarRep: {
      const TYPE_AS_Name & nm (tp.GetRecord(pos_REP_TypeVarRep_nm));
      if ((kind == VAR) && (bind.DomExists (nm)))
        return bind[nm];
      else
        return tp;
    }
    case TAG_TYPE_REP_TypeParRep: {
      const TYPE_AS_Name & nm (tp.GetRecord(pos_REP_TypeParRep_nm));
      if ((kind == PAR) && (bind.DomExists (nm)))
        return bind[nm];
      else
        return tp;
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
      const SEQ<TYPE_REP_FieldRep> & fields (tp.GetSequence(pos_REP_CompositeTypeRep_fields));
      SEQ<TYPE_REP_FieldRep> res;
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++)
      {
        const TYPE_REP_FieldRep & field (fields[idx]);
        res.ImpAppend (TYPE_REP_FieldRep().Init(field.GetField(pos_REP_FieldRep_sel),
                                                InstFn (kind, field.GetRecord(pos_REP_FieldRep_tp), bind),
                                                field.GetBool(pos_REP_FieldRep_dc)));
      }
      return mk_REP_CompositeTypeRep(tp.GetRecord(pos_REP_CompositeTypeRep_nm), res);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> utps (tp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> res;
      Generic utp;
      for (bool bb = utps.First(utp); bb; bb = utps.Next(utp))
        res.Insert (InstFn (kind, utp, bind));
      return mk_REP_UnionTypeRep(res);
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & tps (tp.GetSequence(pos_REP_ProductTypeRep_tps));
      SEQ<TYPE_REP_TypeRep> res;
      size_t len_tps = tps.Length();
      for (size_t idx = 1; idx <= len_tps; idx++)
        res.ImpAppend (InstFn (kind, tps[idx], bind));
      return mk_REP_ProductTypeRep(res);
    }
    case TAG_TYPE_REP_SetTypeRep: {
      return mk_REP_SetTypeRep(InstFn (kind, tp.GetRecord(pos_REP_SetTypeRep_elemtp), bind));
    }
    case TAG_TYPE_REP_EmptySetTypeRep: {
      return mk_REP_EmptySetTypeRep(InstFn (kind, tp.GetRecord(pos_REP_EmptySetTypeRep_elemtp), bind));
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return mk_REP_SeqTypeRep(InstFn (kind, tp.GetRecord(pos_REP_SeqTypeRep_elemtp), bind));
    }
    case TAG_TYPE_REP_EmptySeqTypeRep: {
      return mk_REP_EmptySeqTypeRep(InstFn (kind, tp.GetRecord(pos_REP_EmptySeqTypeRep_elemtp), bind));
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return mk_REP_GeneralMapTypeRep(
          InstFn (kind, tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom), bind),
          InstFn (kind, tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng), bind));
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return mk_REP_InjectiveMapTypeRep(
          InstFn (kind, tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom), bind),
          InstFn (kind, tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng), bind));
    }
    case TAG_TYPE_REP_EmptyMapTypeRep: {
      return mk_REP_EmptyMapTypeRep(
          InstFn (kind, tp.GetRecord(pos_REP_EmptyMapTypeRep_mapdom), bind),
          InstFn (kind, tp.GetRecord(pos_REP_EmptyMapTypeRep_maprng), bind));
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & fndom (tp.GetSequence(pos_REP_PartialFnTypeRep_fndom));
      SEQ<TYPE_REP_TypeRep> res;
      size_t len_fndom = fndom.Length();
      for (size_t idx = 1; idx <= len_fndom; idx++)
        res.ImpAppend (InstFn (kind, fndom[idx], bind));
      return mk_REP_PartialFnTypeRep(res, InstFn (kind, tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng), bind));
    }
    case TAG_TYPE_REP_TotalFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & fndom (tp.GetSequence(pos_REP_TotalFnTypeRep_fndom));
      SEQ<TYPE_REP_TypeRep> res;
      size_t len_fndom = fndom.Length();
      for (size_t idx = 1; idx <= len_fndom; idx++)
        res.ImpAppend (InstFn (kind, fndom[idx], bind));
      return mk_REP_TotalFnTypeRep(res, InstFn (kind, tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng), bind));
    }
    case TAG_TYPE_REP_OpTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & Dom (tp.GetSequence(pos_REP_OpTypeRep_Dom));
      SEQ<TYPE_REP_TypeRep> res;
      size_t len_Dom = Dom.Length();
      for (size_t idx = 1; idx <= len_Dom; idx++)
        res.ImpAppend (InstFn (kind, Dom[idx], bind));
      return mk_REP_OpTypeRep(res, InstFn (kind, tp.GetRecord(pos_REP_OpTypeRep_Rng), bind));
    }
    case TAG_TYPE_REP_PolyTypeRep: {
      return mk_REP_PolyTypeRep(tp.GetSequence(pos_REP_PolyTypeRep_vars),
                                InstFn (kind, tp.GetRecord(pos_REP_PolyTypeRep_tp), bind));
    }
    default: {
      InternalError (L"InstFn");
      return tp;
    }
  }
  return tp; //only to avoid warnings from the C++ parser
}

// InstFnAS
// gtp : (AS`Type | AS`TypeDef)
// bind : map AS`Name to AS`Type
// -> (AS`Type | AS`TypeDef)
Record StatSem::InstFnAS (const Record & tp, const MAP<TYPE_AS_Name,TYPE_AS_Type> & bind) const
{
  switch (tp.GetTag()) {
    case TAG_TYPE_AS_TypeDef: {
      TYPE_AS_TypeDef td (tp);
      td.SetField(pos_AS_TypeDef_shape, InstFnAS (tp.GetRecord(pos_AS_TypeDef_shape), bind));
      return td;
    }
    case TAG_TYPE_AS_BooleanType:
    case TAG_TYPE_AS_NumericType:
    case TAG_TYPE_AS_TokenType:
    case TAG_TYPE_AS_CharType:
    case TAG_TYPE_AS_VoidType:
    case TAG_TYPE_AS_QuoteType:
    case TAG_TYPE_AS_TypeVar: {
      return tp;
    }
    case TAG_TYPE_AS_TypeName: {
      const TYPE_AS_Name & nm (tp.GetRecord(pos_AS_TypeName_name));
      if (bind.DomExists (nm))
        return bind[nm];
      else
        return tp;
    }
    case TAG_TYPE_AS_CompositeType: {
      const SEQ<TYPE_AS_Field> & fields (tp.GetSequence(pos_AS_CompositeType_fields));
      SEQ<TYPE_AS_Field> new_fields;
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++)
      {
        TYPE_AS_Field fld (fields[idx]);
        fld.SetField(pos_AS_Field_type, InstFnAS (fld.GetRecord(pos_AS_Field_type), bind));
        new_fields.ImpAppend (fld);
      }
      TYPE_AS_CompositeType res (tp);
      res.SetField(pos_AS_CompositeType_fields, new_fields);
      return res;
    }
    case TAG_TYPE_AS_UnionType: {
      const SEQ<TYPE_AS_Type> & utps (tp.GetSequence(pos_AS_UnionType_tps));
      SEQ<TYPE_AS_Type> new_utps;
      size_t len_utps = utps.Length();
      for (size_t idx = 1; idx <= len_utps; idx++)
        new_utps.ImpAppend (InstFnAS (utps[idx], bind));

      TYPE_AS_UnionType res (tp);
      res.SetField(pos_AS_UnionType_tps, new_utps);
      return res;
    }
    case TAG_TYPE_AS_ProductType: {
      const SEQ<TYPE_AS_Type> & fields (tp.GetSequence(pos_AS_ProductType_tps));
      SEQ<TYPE_AS_Type> new_fields;
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++)
        new_fields.ImpAppend (InstFnAS (fields[idx], bind));

      TYPE_AS_ProductType res (tp);
      res.SetField(pos_AS_ProductType_tps, new_fields);
      return res;
    }
    case TAG_TYPE_AS_BracketedType: {
      TYPE_AS_BracketedType res (tp);
      res.SetField(pos_AS_BracketedType_tp, InstFnAS (tp.GetRecord(pos_AS_BracketedType_tp), bind));
      return res;
    }
    case TAG_TYPE_AS_OptionalType: {
      TYPE_AS_OptionalType res (tp);
      res.SetField(pos_AS_OptionalType_tp, InstFnAS (tp.GetRecord(pos_AS_OptionalType_tp), bind));
      return res;
    }
    case TAG_TYPE_AS_Set0Type: {
      TYPE_AS_Set0Type res (tp);
      res.SetField(pos_AS_Set0Type_elemtp, InstFnAS (tp.GetRecord(pos_AS_Set0Type_elemtp), bind));
      return res;
    }
    case TAG_TYPE_AS_Set1Type: {
      TYPE_AS_Set1Type res (tp);
      res.SetField(pos_AS_Set1Type_elemtp, InstFnAS (tp.GetRecord(pos_AS_Set1Type_elemtp), bind));
      return res;
    }
    case TAG_TYPE_AS_Seq0Type: {
      TYPE_AS_Seq0Type res (tp);
      res.SetField(pos_AS_Seq0Type_elemtp, InstFnAS (tp.GetRecord(pos_AS_Seq0Type_elemtp), bind));
      return res;
    }
    case TAG_TYPE_AS_Seq1Type: {
      TYPE_AS_Seq1Type res (tp);
      res.SetField(pos_AS_Seq1Type_elemtp, InstFnAS (tp.GetRecord(pos_AS_Seq1Type_elemtp), bind));
      return res;
    }
    case TAG_TYPE_AS_GeneralMap0Type: {
      TYPE_AS_GeneralMap0Type res (tp);
      res.SetField(pos_AS_GeneralMap0Type_mapdom, InstFnAS (tp.GetRecord(pos_AS_GeneralMap0Type_mapdom), bind));
      res.SetField(pos_AS_GeneralMap0Type_maprng, InstFnAS (tp.GetRecord(pos_AS_GeneralMap0Type_maprng), bind));
      return res;
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      TYPE_AS_GeneralMap1Type res (tp);
      res.SetField(pos_AS_GeneralMap1Type_mapdom, InstFnAS (tp.GetRecord(pos_AS_GeneralMap1Type_mapdom), bind));
      res.SetField(pos_AS_GeneralMap1Type_maprng, InstFnAS (tp.GetRecord(pos_AS_GeneralMap1Type_maprng), bind));
      return res;
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      TYPE_AS_InjectiveMap0Type res (tp);
      res.SetField(pos_AS_InjectiveMap0Type_mapdom, InstFnAS (tp.GetRecord(pos_AS_InjectiveMap0Type_mapdom), bind));
      res.SetField(pos_AS_InjectiveMap0Type_maprng, InstFnAS (tp.GetRecord(pos_AS_InjectiveMap0Type_maprng), bind));
      return res;
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      TYPE_AS_InjectiveMap1Type res (tp);
      res.SetField(pos_AS_InjectiveMap1Type_mapdom, InstFnAS (tp.GetRecord(pos_AS_InjectiveMap1Type_mapdom), bind));
      res.SetField(pos_AS_InjectiveMap1Type_maprng, InstFnAS (tp.GetRecord(pos_AS_InjectiveMap1Type_maprng), bind));
      return res;
    }
    case TAG_TYPE_AS_OpType: {
      const SEQ<TYPE_AS_Type> & opdom (tp.GetSequence(pos_AS_OpType_opdom));
      SEQ<TYPE_AS_Type> new_opdom;
      size_t len_opdom = opdom.Length();
      for (size_t idx = 1; idx <= len_opdom; idx++)
        new_opdom.ImpAppend (InstFnAS (opdom[idx], bind));

      TYPE_AS_OpType res (tp);
      res.SetField(pos_AS_OpType_opdom, new_opdom);
      res.SetField(pos_AS_OpType_oprng, InstFnAS (tp.GetRecord(pos_AS_OpType_oprng), bind));
      return res;
    }
    case TAG_TYPE_AS_PartialFnType: {
      const SEQ<TYPE_AS_Type> & fndom (tp.GetSequence(pos_AS_PartialFnType_fndom));
      SEQ<TYPE_AS_Type> new_fndom;
      size_t len_fndom = fndom.Length();
      for (size_t idx = 1; idx <= len_fndom; idx++)
        new_fndom.ImpAppend (InstFnAS (fndom[idx], bind));

      TYPE_AS_PartialFnType res (tp);
      res.SetField(pos_AS_PartialFnType_fndom, new_fndom);
      res.SetField(pos_AS_PartialFnType_fnrng, InstFnAS (tp.GetRecord(pos_AS_PartialFnType_fnrng), bind));
      return res;
    }
    case TAG_TYPE_AS_TotalFnType: {
      const SEQ<TYPE_AS_Type> & fndom (tp.GetSequence(pos_AS_TotalFnType_fndom));
      SEQ<TYPE_AS_Type> new_fndom;
      size_t len_fndom = fndom.Length();
      for (size_t idx = 1; idx <= len_fndom; idx++)
        new_fndom.ImpAppend (InstFnAS (fndom[idx], bind));

      TYPE_AS_TotalFnType res (tp);
      res.SetField(pos_AS_TotalFnType_fndom, new_fndom);
      res.SetField(pos_AS_TotalFnType_fnrng, InstFnAS (tp.GetRecord(pos_AS_TotalFnType_fnrng), bind));
      return res;
    }
    default: {
      InternalError (L"InstFnAS");
      return tp;
    }
  }
}

#ifdef VDMPP
// QualifyTypeNames
// tp_o : REP`TypeRep
// nm : AS`Name
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::QualifyTypeNames(const TYPE_REP_TypeRep & tp, const TYPE_AS_Name & nm)
{
  switch (tp.GetTag()) {
    case TAG_TYPE_REP_InvTypeRep: {
      return mk_REP_InvTypeRep(tp.GetRecord(pos_REP_InvTypeRep_name),
                               QualifyTypeNames(tp.GetRecord(pos_REP_InvTypeRep_shape),nm),
                               tp.GetField(pos_REP_InvTypeRep_invariant));
    }
    case TAG_TYPE_REP_SetTypeRep: {
      return mk_REP_SetTypeRep(QualifyTypeNames(tp.GetRecord(pos_REP_SetTypeRep_elemtp),nm));
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return mk_REP_SeqTypeRep(QualifyTypeNames(tp.GetRecord(pos_REP_SeqTypeRep_elemtp),nm));
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
      TYPE_AS_Name newtag (ExtName(nm, tp.GetRecord(pos_REP_CompositeTypeRep_nm)));
      const SEQ<TYPE_REP_FieldRep> & fields (tp.GetSequence(pos_REP_CompositeTypeRep_fields));
      SEQ<TYPE_REP_FieldRep> newfields;
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++)
      {
        const TYPE_REP_FieldRep & field (fields[idx]);
        newfields.ImpAppend(TYPE_REP_FieldRep().Init(field.GetField(pos_REP_FieldRep_sel),
                                                     QualifyTypeNames(field.GetRecord(pos_REP_FieldRep_tp),nm),
                                                     field.GetBool(pos_REP_FieldRep_dc)));
      }
      return mk_REP_CompositeTypeRep(newtag, newfields);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tps (tp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> ntps;
      Generic gtp;
      for (bool bb = tps.First(gtp); bb; bb = tps.Next(gtp))
        ntps.Insert(QualifyTypeNames(gtp, nm));
      return mk_REP_UnionTypeRep(ntps);
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & tps (tp.GetSequence(pos_REP_ProductTypeRep_tps));
      SEQ<TYPE_REP_TypeRep> ntps;
      size_t len_tps = tps.Length();
      for (size_t idx = 1; idx <= len_tps; idx++)
        ntps.ImpAppend(QualifyTypeNames(tps[idx], nm));
      return mk_REP_ProductTypeRep(ntps);
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return mk_REP_GeneralMapTypeRep(QualifyTypeNames(tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom), nm),
                                      QualifyTypeNames(tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng), nm));
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return mk_REP_InjectiveMapTypeRep(QualifyTypeNames(tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom), nm),
                                        QualifyTypeNames(tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng), nm));
    }
    case TAG_TYPE_REP_TypeVarRep: {
      return mk_REP_TypeVarRep(ExtName(nm, tp.GetRecord(pos_REP_TypeVarRep_nm)));
    }
    case TAG_TYPE_REP_TypeNameRep: {
      return mk_REP_TypeNameRep(ExtName(nm, tp.GetRecord(pos_REP_TypeNameRep_nm)));
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      const Generic & gfndom (tp.GetField(pos_REP_PartialFnTypeRep_fndom));
      if (gfndom.IsSequence()) {
        SEQ<TYPE_REP_TypeRep> fndom(gfndom), newfndom;
        size_t len_fndom = fndom.Length(); 
        for (size_t idx = 1; idx <= len_fndom; idx++)
          newfndom.ImpAppend(QualifyTypeNames(fndom[idx], nm));
        return mk_REP_PartialFnTypeRep(newfndom, QualifyTypeNames(tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng), nm));
      }
      else
       return mk_REP_PartialFnTypeRep(gfndom, QualifyTypeNames(tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng), nm));
    }
    case TAG_TYPE_REP_TotalFnTypeRep: {
      const Generic & gfndom (tp.GetField(pos_REP_TotalFnTypeRep_fndom));
      if (gfndom.IsSequence()) {
        SEQ<TYPE_REP_TypeRep> fndom (gfndom), newfndom;
        size_t len_fndom = fndom.Length();
        for (size_t idx = 1; idx <= len_fndom; idx++)
          newfndom.ImpAppend(QualifyTypeNames(fndom[idx], nm));
        return mk_REP_TotalFnTypeRep(newfndom, QualifyTypeNames(tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng), nm));
      }
      else
       return mk_REP_TotalFnTypeRep(gfndom, QualifyTypeNames(tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng), nm));
    }
// 20070830 ->
    case TAG_TYPE_REP_OpTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & opdom (tp.GetSequence(pos_REP_OpTypeRep_Dom));
      SEQ<TYPE_REP_TypeRep> newdom;
      size_t len_opdom = opdom.Length();
      for (size_t idx = 1; idx <= len_opdom; idx++)
        newdom.ImpAppend(QualifyTypeNames(opdom[idx], nm));
      return mk_REP_TotalFnTypeRep(newdom, QualifyTypeNames(tp.GetRecord(pos_REP_OpTypeRep_Rng), nm));
    }
// <-
    case TAG_TYPE_REP_PolyTypeRep: {
      SEQ<TYPE_REP_TypeVarRep> fndom (tp.GetSequence(pos_REP_PolyTypeRep_vars));
      SEQ<TYPE_REP_TypeVarRep> newfndom;
      size_t len_fndom = fndom.Length();
      for (size_t idx = 1; idx <= len_fndom; idx++)
        newfndom.ImpAppend(QualifyTypeNames(fndom[idx], nm));
      return mk_REP_PolyTypeRep(newfndom, QualifyTypeNames(tp.GetRecord(pos_REP_PolyTypeRep_tp), nm));
    }
    default:
      return tp;
  }
}
#endif // VDMPP

#ifdef VDMSL
// RenameTag
// tp0 : REP`TypeRep
// bind : map AS`Name to AS`Name
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::RenameTag (const TYPE_REP_TypeRep & tp,
                                          const MAP<TYPE_AS_Name,TYPE_AS_Name> & bind) const
{
  switch (tp.GetTag()) {
    case TAG_TYPE_REP_AllTypeRep:
    case TAG_TYPE_REP_UnitTypeRep:
    case TAG_TYPE_REP_NilTypeRep:
    case TAG_TYPE_REP_EmptySetTypeRep:
    case TAG_TYPE_REP_EmptySeqTypeRep:
    case TAG_TYPE_REP_EmptyMapTypeRep:
    case TAG_TYPE_REP_BooleanTypeRep:
    case TAG_TYPE_REP_NumericTypeRep:
    case TAG_TYPE_REP_TokenTypeRep:
    case TAG_TYPE_REP_CharTypeRep:
    case TAG_TYPE_REP_QuoteTypeRep:
    case TAG_TYPE_REP_InvTypeRep:
    case TAG_TYPE_REP_TypeVarRep:
    case TAG_TYPE_REP_TypeParRep:
    case TAG_TYPE_REP_TypeNameRep: {
      return tp;
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
      const TYPE_AS_Name & tag (tp.GetRecord(pos_REP_CompositeTypeRep_nm));
      const SEQ<TYPE_REP_FieldRep> & fields (tp.GetSequence(pos_REP_CompositeTypeRep_fields));
      SEQ<TYPE_REP_FieldRep> f_l;
      size_t len_fields = fields.Length();
      for (size_t index = 1; index <= len_fields; index++)
      {
        const TYPE_REP_FieldRep & field (fields[index]);
        f_l.ImpAppend (TYPE_REP_FieldRep().Init(field.GetField(pos_REP_FieldRep_sel),
                                   RenameTag (field.GetRecord(pos_REP_FieldRep_tp), bind),
                                   field.GetBool(pos_REP_FieldRep_dc)));
      }
      if (bind.DomExists (tag))
        return mk_REP_CompositeTypeRep(bind[tag], f_l);
      else
        return mk_REP_CompositeTypeRep(tag, f_l);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> utps (tp.GetSet(pos_REP_UnionTypeRep_tps)), res;
      Generic g;
      for (bool bb = utps.First(g); bb; bb = utps.Next(g))
        res.Insert (RenameTag (g, bind));
      return mk_REP_UnionTypeRep(res);
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & tp_l (tp.GetSequence(pos_REP_ProductTypeRep_tps));
      SEQ<TYPE_REP_TypeRep> l;
      size_t len_tp_l = tp_l.Length();
      for (size_t index = 1; index <= len_tp_l; index++)
        l.ImpAppend (RenameTag (tp_l[index], bind));
      return mk_REP_ProductTypeRep(l);
    }
    case TAG_TYPE_REP_SetTypeRep: {
      return mk_REP_SetTypeRep(RenameTag (tp.GetRecord(pos_REP_SetTypeRep_elemtp), bind));
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return mk_REP_SeqTypeRep(RenameTag (tp.GetRecord(pos_REP_SeqTypeRep_elemtp), bind));
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return mk_REP_GeneralMapTypeRep(
          RenameTag (tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom), bind),
          RenameTag (tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng), bind));
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return mk_REP_InjectiveMapTypeRep(
          RenameTag (tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom), bind),
          RenameTag (tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng), bind));
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      const Generic & tp_l_g (tp.GetField(pos_REP_PartialFnTypeRep_fndom));

      if (tp_l_g.Is(TAG_TYPE_REP_AllTypeRep))
        return rep_alltp;
      else
      {
        SEQ<TYPE_REP_TypeRep> tp_l (tp_l_g);
        SEQ<TYPE_REP_TypeRep> l;
        size_t len_tp_l = tp_l.Length();
        for (size_t index = 1; index <= len_tp_l; index++)
          l.ImpAppend (RenameTag (tp_l[index], bind));
        return mk_REP_PartialFnTypeRep(l, RenameTag (tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng), bind));
      }
    }
    case TAG_TYPE_REP_TotalFnTypeRep: {
      const Generic & tp_l_g (tp.GetField(pos_REP_TotalFnTypeRep_fndom));

      if (tp_l_g.Is(TAG_TYPE_REP_AllTypeRep))
        return rep_alltp;
      else
      {
        SEQ<TYPE_REP_TypeRep> tp_l (tp_l_g);
        SEQ<TYPE_REP_TypeRep> l;
        size_t len_tp_l = tp_l.Length();
        for (size_t index = 1; index <= len_tp_l; index++)
          l.ImpAppend (RenameTag (tp_l[index], bind));
        return mk_REP_TotalFnTypeRep(l, RenameTag (tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng), bind));
      }
    }
    default: {
      InternalError (L"RenameTag");
      return tp;
    }
  }
}

// TypeUsedImplicitly
// nm : AS`Name
// tps : set of REP`TypeRep
// -> bool
bool StatSem::TypeUsedImplicitly(const TYPE_AS_Name & nm, const SET<TYPE_REP_TypeRep> & tps_)
{
  SET<TYPE_REP_TypeRep> tps (tps_);
  bool exists (false);
  Generic tp;
  for ( bool bb = tps.First(tp); bb && !exists; bb = tps.Next(tp) ) {
    exists = ImplicitlyRec(nm, tp);
  }
  return exists;
}

// ImplicitlyRec
// nm : AS`Name
// tp : REP`TypeRep
// -> bool
bool StatSem::ImplicitlyRec(const TYPE_AS_Name & nm, const TYPE_REP_TypeRep & tp)
{
  switch (tp.GetTag()) {
    case TAG_TYPE_REP_AllTypeRep:
    case TAG_TYPE_REP_UnitTypeRep:
    case TAG_TYPE_REP_NilTypeRep:
    case TAG_TYPE_REP_EmptySetTypeRep:
    case TAG_TYPE_REP_EmptySeqTypeRep:
    case TAG_TYPE_REP_EmptyMapTypeRep:
    case TAG_TYPE_REP_BooleanTypeRep:
    case TAG_TYPE_REP_NumericTypeRep:
    case TAG_TYPE_REP_TokenTypeRep:
    case TAG_TYPE_REP_CharTypeRep:
    case TAG_TYPE_REP_TypeVarRep:
    case TAG_TYPE_REP_TypeParRep:
    case TAG_TYPE_REP_QuoteTypeRep: {
      return false;
    }
    case TAG_TYPE_REP_SetTypeRep: {
      return ImplicitlyRec(nm, tp.GetRecord(pos_REP_SetTypeRep_elemtp));
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return ImplicitlyRec(nm, tp.GetRecord(pos_REP_SeqTypeRep_elemtp));
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return ImplicitlyRec(nm, tp.GetRecord(pos_REP_InvTypeRep_shape));
    }
    case TAG_TYPE_REP_TypeNameRep: {
      return (nm == tp.GetRecord(pos_REP_TypeNameRep_nm));
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
      const SEQ<TYPE_REP_FieldRep> & fields (tp.GetSequence(pos_REP_CompositeTypeRep_fields));
      SET<TYPE_REP_TypeRep> t_s;
      size_t len_fields = fields.Length();
      for (size_t index = 1; index <= len_fields; index++)
        t_s.Insert(fields[index].GetRecord(pos_REP_FieldRep_tp));
      return TypeUsedImplicitly(nm, t_s);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      return TypeUsedImplicitly(nm, tp.GetSet(pos_REP_UnionTypeRep_tps));
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      SEQ<TYPE_REP_TypeRep> t_l (tp.GetSequence(pos_REP_ProductTypeRep_tps));
      return TypeUsedImplicitly(nm, t_l.Elems());
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return (ImplicitlyRec(nm, tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom))  ||
              ImplicitlyRec(nm, tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng)) );
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return (ImplicitlyRec(nm, tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng))  ||
              ImplicitlyRec(nm, tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng)) );
    }
    case TAG_TYPE_REP_PartialFnTypeRep : {
      const Generic & tp_l (tp.GetField(pos_REP_PartialFnTypeRep_fndom));
// 20130830 -->
/*
      if (tp_l.Is(TAG_TYPE_REP_AllTypeRep)) {
        return false;
      }
      else
        return TypeUsedImplicitly(nm, ((Sequence) tp_l).Elems());
*/
      SET<TYPE_REP_TypeRep> tp_s (mk_set(tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng)));
      if (!tp_l.Is(TAG_TYPE_REP_AllTypeRep))
        tp_s.ImpUnion(((Sequence) tp_l).Elems());
      return TypeUsedImplicitly(nm, tp_s);
// <-- 20130830
    }
    case TAG_TYPE_REP_TotalFnTypeRep : {
      const Generic & tp_l (tp.GetField(pos_REP_TotalFnTypeRep_fndom));
// 20130830 -->
/*
      if (tp_l.Is(TAG_TYPE_REP_AllTypeRep)) {
        return false;
      }
      else
        return TypeUsedImplicitly(nm, ((Sequence) tp_l).Elems());
*/
      SET<TYPE_REP_TypeRep> tp_s (mk_set(tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng)));
      if (!tp_l.Is(TAG_TYPE_REP_AllTypeRep))
        tp_s.ImpUnion(((Sequence) tp_l).Elems());
      return TypeUsedImplicitly(nm, tp_s);
// <-- 20130830
    }
    default: {
      InternalError(L"ImplicitlyRec");
    }
  }
  return false; // Dummy
}


#endif // VDMSL

// AlwaysType
// tppost : REP`TypeRep
// tpstmt : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::AlwaysType(const TYPE_REP_TypeRep & tppost, const TYPE_REP_TypeRep & tpstmt) const
{
  Generic etp (ExtractExitTypeRep(tppost));
  if (etp.IsNil ())
    return tpstmt;
  else if (tppost.Is (TAG_TYPE_REP_ExitTypeRep) &&
           (tppost.GetRecord(pos_REP_ExitTypeRep_tp) == etp))
    return tppost;
  else
    return MergeTypes( tpstmt, mk_REP_ExitTypeRep( etp ) );
}

// ExtractExitTypeRep
// tp : REP`TypeRep
// -> [REP`TypeRep]
Generic StatSem::ExtractExitTypeRep (const TYPE_REP_TypeRep & tp) const
{
  switch (tp.GetTag()) {
    case TAG_TYPE_REP_ExitTypeRep: {
      return tp.GetRecord(pos_REP_ExitTypeRep_tp);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> Utp (tp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> s;
      Generic g;
      for (bool bb = Utp.First (g); bb; bb = Utp.Next (g))
      {
        TYPE_REP_TypeRep t (g);
        if (t.Is (TAG_TYPE_REP_ExitTypeRep))
          s.Insert (t.GetRecord(pos_REP_ExitTypeRep_tp));
      }
      switch (s.Card ()) {
        case 0:
          return Nil ();
        case 1:
          return s.GetElem ();
        default: {
          return TYPE_REP_UnionTypeRep().Init(s);
        }
      }
    }
    default:
      return Nil ();
  }
}

// CanBeApplied
// tpi : [REP`TypeRep]
// -> bool * [REP`TypeRep]
Tuple StatSem::CanBeApplied (const Generic & tpi)
{
  if (tpi.IsNil())
    return mk_(Bool(false), Nil());
  else
  {
    TYPE_REP_TypeRep tp (tpi);
    switch (tp.GetTag()) {
      case TAG_TYPE_REP_SeqTypeRep:
      case TAG_TYPE_REP_GeneralMapTypeRep:
      case TAG_TYPE_REP_InjectiveMapTypeRep:
      case TAG_TYPE_REP_PartialFnTypeRep:
      case TAG_TYPE_REP_TotalFnTypeRep: {
        return mk_(Bool(true), tp);
      }
      case TAG_TYPE_REP_UnionTypeRep: {
        SET<TYPE_REP_TypeRep> utps (tp.GetSet(pos_REP_UnionTypeRep_tps));
        SET<TYPE_REP_TypeRep> st;
        Generic t;
        for (bool bb = utps.First(t); bb; bb = utps.Next(t))
        {
          Tuple infer (CanBeApplied (t));
          if (infer.GetBoolValue (1))
            st.Insert (infer.GetRecord (2));
        }
        switch(st.Card ()) {
          case 0:
            return mk_(Bool(false), Nil());
          case 1:
            return mk_(Bool(true), st.GetElem ());
          default:
            return mk_(Bool(true), mk_REP_UnionTypeRep(st));
        }
      }
      case TAG_TYPE_REP_TypeNameRep: {
        if (!IsUnionRecursive (tp))
          return CanBeApplied (TcauxLookUpTypeName (tp.GetRecord(pos_REP_TypeNameRep_nm)));
        else
          return mk_(Bool(false), Nil());
      }
      case TAG_TYPE_REP_InvTypeRep: {
        return CanBeApplied (tp.GetRecord(pos_REP_InvTypeRep_shape));
      }
      default: {
        return mk_(Bool(false), Nil());
      }
    }
  }
}

// MakePreType
// fntp : REP`FnTypeRep
// -> REP`FnTypeRep
TYPE_REP_FnTypeRep StatSem::MakePreType (const TYPE_REP_FnTypeRep & fntp) const
{
  switch(fntp.GetTag()) {
    case TAG_TYPE_REP_TotalFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & fndom (fntp.GetSequence(pos_REP_TotalFnTypeRep_fndom));
      return mk_REP_TotalFnTypeRep(fndom, btp_bool);
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & fndom (fntp.GetSequence(pos_REP_PartialFnTypeRep_fndom));
      return mk_REP_TotalFnTypeRep(fndom, btp_bool);
    }
    default: {
      InternalError (L"MakeRpeType");
      return rep_alltp;
    }
  }
}

// MakePostType
// fntp : REP`FnTypeRep
// -> REP`FnTypeRep
TYPE_REP_FnTypeRep StatSem::MakePostType (const TYPE_REP_FnTypeRep & fntp) const
{
  switch(fntp.GetTag()) {
    case TAG_TYPE_REP_TotalFnTypeRep: {
      SEQ<TYPE_REP_TypeRep> fndom (fntp.GetSequence(pos_REP_TotalFnTypeRep_fndom));
      fndom.ImpAppend (fntp.GetRecord(pos_REP_TotalFnTypeRep_fnrng));
      return mk_REP_TotalFnTypeRep(fndom, btp_bool);
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      SEQ<TYPE_REP_TypeRep> fndom (fntp.GetSequence(pos_REP_PartialFnTypeRep_fndom));
      fndom.ImpAppend (fntp.GetRecord(pos_REP_PartialFnTypeRep_fnrng));
      return mk_REP_TotalFnTypeRep(fndom, btp_bool);
    }
    default: {
      InternalError (L"MakeRostType");
      return rep_alltp;
    }
  }
}

// MakePolyPreType
// tv_l : seq of REP`TypeVarRep
// fntp : REP`FnTypeRep
// -> seq of REP`TypeVarRep * REP`FnTypeRep
Tuple StatSem::MakePolyPreType (const SEQ<TYPE_REP_TypeVarRep> & tv_l, const TYPE_REP_FnTypeRep & fntp) const
{
  switch(fntp.GetTag()) {
    case TAG_TYPE_REP_TotalFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & fndom (fntp.GetSequence(pos_REP_TotalFnTypeRep_fndom));
      return mk_(tv_l, mk_REP_TotalFnTypeRep(fndom, btp_bool));
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & fndom (fntp.GetSequence(pos_REP_PartialFnTypeRep_fndom));
      return mk_(tv_l, mk_REP_TotalFnTypeRep(fndom, btp_bool));
    } 
    default: {
      InternalError (L"MakePolyPreType");
      return Tuple(0);
    }
  }
}

// MakePolyPostType
// tv_l : seq of REP`TypeVarRep
// fntp : REP`FnTypeRep
// -> seq of REP`TypeVarRep * REP`FnTypeRep
Tuple StatSem::MakePolyPostType (const SEQ<TYPE_REP_TypeVarRep> & tv_l, const TYPE_REP_FnTypeRep & fntp) const
{
  switch(fntp.GetTag()) {
    case TAG_TYPE_REP_TotalFnTypeRep: {
      SEQ<TYPE_REP_TypeRep> fndom (fntp.GetSequence(pos_REP_TotalFnTypeRep_fndom));
      fndom.ImpAppend(fntp.GetRecord(pos_REP_TotalFnTypeRep_fnrng));
      return mk_(tv_l, mk_REP_TotalFnTypeRep(fndom, btp_bool));
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      SEQ<TYPE_REP_TypeRep> fndom (fntp.GetSequence(pos_REP_PartialFnTypeRep_fndom));
      fndom.ImpAppend(fntp.GetRecord(pos_REP_PartialFnTypeRep_fnrng));
      return mk_(tv_l, mk_REP_TotalFnTypeRep(fndom, btp_bool));
    } 
    default: {
      InternalError (L"MakePolyPostType");
      return Tuple(0);
    }
  }
}

//#ifdef VDMSL
// MakeOpPreType
// modid : [AS`Name]
// optp : REP`OpTypeRep
// stdef : [AS`StateDef]
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::MakeOpPreType (const Generic & modid,
                                              const TYPE_REP_OpTypeRep & optp,
                                              const Generic & stdef)
{
  SEQ<TYPE_REP_TypeRep> domtp (optp.GetSequence(pos_REP_OpTypeRep_Dom));

#ifdef VDMSL
  if (!stdef.IsNil ())
    domtp.ImpAppend (TransStateType (modid, stdef));
#endif // VDMSL

  return mk_REP_TotalFnTypeRep( domtp, btp_bool );
}

// MakeOpPostType
// modid : [AS`Name]
// optp : REP`OpTypeRep
// stdef : [AS`StateDef]
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::MakeOpPostType (const Generic & modid,
                                               const TYPE_REP_OpTypeRep & optp,
                                               const Generic & stdef)
{
  SEQ<TYPE_REP_TypeRep> domtp (optp.GetSequence(pos_REP_OpTypeRep_Dom));
  const TYPE_REP_TypeRep & rngtp (optp.GetRecord(pos_REP_OpTypeRep_Rng));

  if (!rngtp.Is(TAG_TYPE_REP_UnitTypeRep))
    domtp.ImpAppend (rngtp);

#ifdef VDMSL
  if (!stdef.IsNil ())
  {
    TYPE_REP_TypeRep sttp (TransStateType (modid, stdef));
    domtp.ImpAppend (sttp).ImpAppend (sttp);
  }
#endif // VDMSL

  return mk_REP_TotalFnTypeRep( domtp, btp_bool );
}
//#endif //VDMSL

// IsPrePostFn
// nm : AS`Name
// -> bool
bool StatSem::IsPrePostFn(const TYPE_AS_Name & nm) const
{
  const TYPE_AS_Ids & ids (nm.GetSequence(pos_AS_Name_ids));
  TYPE_AS_Id lastNameSeq (ids[ids.Length()]);
  wstring lastname;
  lastNameSeq.GetString(lastname);
  return ( (lastname.find(L"pre_") == 0) || (lastname.find(L"post_") == 0 ) );
}

// ExtName
// mod : AS`Name
// nm_org : AS`Name
// -> AS`Name
TYPE_AS_Name StatSem::ExtName (const TYPE_AS_Name & mod, const TYPE_AS_Name & nm) const
{
  TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
  if (ids.Length () <= 1)
  {
    ids.ImpPrepend (mod.GetSequence(pos_AS_Name_ids).Hd());
  }
  return TYPE_AS_Name().Init(ids, nm.GetInt(pos_AS_Name_cid));
}

// Pre
// nm : AS`Name
// -> AS`Name
TYPE_AS_Name StatSem::Pre (const TYPE_AS_Name & nm) const
{
  TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
  TYPE_AS_Id id (ASTAUX::MkId(L"pre_").ImpConc(ids[ids.Length()]));
  TYPE_AS_Name name (nm);
  name.SetField(pos_AS_Name_ids, ids.SubSequence(1,ids.Length() -1).ImpAppend(id));
  return name;
}

// Post
// nm : AS`Name
// -> AS`Name
TYPE_AS_Name StatSem::Post (const TYPE_AS_Name & nm) const
{
  TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
  TYPE_AS_Id id (ASTAUX::MkId(L"post_").ImpConc(ids[ids.Length()]));
  TYPE_AS_Name name (nm);
  name.SetField(pos_AS_Name_ids, ids.SubSequence(1,ids.Length() -1).ImpAppend(id));
  return name;
}

// Inv
// nm : AS`Name
// -> AS`Name
TYPE_AS_Name StatSem::Inv (const TYPE_AS_Name & nm) const
{
  TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
  TYPE_AS_Id id (ASTAUX::MkId(L"inv_").ImpConc(ids[ids.Length()]));
  TYPE_AS_Name name (nm);
  name.SetField(pos_AS_Name_ids, ids.SubSequence(1,ids.Length() -1).ImpAppend(id));
  return name;
}

// Init
// nm : AS`Name
// -> AS`Name
TYPE_AS_Name StatSem::Init (const TYPE_AS_Name & nm) const
{
  TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
  TYPE_AS_Id id (ASTAUX::MkId(L"init_").ImpConc(ids[ids.Length()]));
  TYPE_AS_Name name (nm);
  name.SetField(pos_AS_Name_ids, ids.SubSequence(1,ids.Length() -1).ImpAppend(id));
  return name;
}

// Equality
// nm : AS`Name
// -> AS`Name
TYPE_AS_Name StatSem::Equality (const TYPE_AS_Name & nm) const
{
  TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
  TYPE_AS_Id id (ASTAUX::MkId(L"eq_").ImpConc(ids[ids.Length()]));
  TYPE_AS_Name name (nm);
  name.SetField(pos_AS_Name_ids, ids.SubSequence(1,ids.Length() -1).ImpAppend(id));
  return name;
}

// Order
// nm : AS`Name
// -> AS`Name
TYPE_AS_Name StatSem::Order (const TYPE_AS_Name & nm) const
{
  TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
  TYPE_AS_Id id (ASTAUX::MkId(L"ord_").ImpConc(ids[ids.Length()]));
  TYPE_AS_Name name (nm);
  name.SetField(pos_AS_Name_ids, ids.SubSequence(1,ids.Length() -1).ImpAppend(id));
  return name;
}

// Max
// nm : AS`Name
// -> AS`Name
TYPE_AS_Name StatSem::Max (const TYPE_AS_Name & nm) const
{
  TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
  TYPE_AS_Id id (ASTAUX::MkId(L"max_").ImpConc(ids[ids.Length()]));
  TYPE_AS_Name name (nm);
  name.SetField(pos_AS_Name_ids, ids.SubSequence(1,ids.Length() -1).ImpAppend(id));
  return name;
}
// Min
// nm : AS`Name
// -> AS`Name
TYPE_AS_Name StatSem::Min (const TYPE_AS_Name & nm) const
{
  TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
  TYPE_AS_Id id (ASTAUX::MkId(L"min_").ImpConc(ids[ids.Length()]));
  TYPE_AS_Name name (nm);
  name.SetField(pos_AS_Name_ids, ids.SubSequence(1,ids.Length() -1).ImpAppend(id));
  return name;
}

// ReservedPrefix
// nm : AS`Name
// -> bool
bool StatSem::ReservedPrefix (const TYPE_AS_Name & nm) const
{
  TYPE_AS_Id id (nm.GetSequence(pos_AS_Name_ids).Hd());
  wstring txt;
  id.GetString(txt);
  return (txt.find(L"is_") == 0 || txt.find(L"mk_") == 0 || txt.find(L"eq_") == 0 ||
          txt.find(L"pre_") == 0 || txt.find(L"inv_") == 0 || txt.find(L"ord_") == 0 ||
          txt.find(L"max_") == 0 || txt.find(L"min_") == 0 ||
          txt.find(L"post_") == 0 || txt.find(L"init_") == 0);
}

// IsEmptyName
// nm : AS`Name
// -> bool
bool StatSem::IsEmptyName (const TYPE_AS_Name & nm) const
{
  return nm.GetSequence(pos_AS_Name_ids).IsEmpty();
}

#endif //! NOSS
