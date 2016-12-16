/***
*  * ID
*  *    $Id: env.cc,v 1.137 2006/06/12 08:36:17 vdmtools Exp $
*  * SPECIFICATION VERSION
*  *    env.cc: Implementation of env.vdm 1.152
***/

#include "statsem.h"
#ifdef VDMPP
#include "MANGLE.h"
#endif // VDMPP
#include "astaux.h"
#include "intconvquotes.h"
#include "settings.h"

//wcout << L"intertp: " << INT2Q::h2gAS(intertp) << endl;

static StatSem statsem;

StatSem & GetStatSem()
{
  return statsem;
}

#ifndef NOSS

// setRecMap
// rmap  :  map AS`Name to FunctionInfo
// ==> ()
void StatSem::setRecMap(const Map & rmap)
{
  this->RecMap = rmap;
}

// getRecMap
// ==>  map AS`Name to FunctionInfo
Map StatSem::getRecMap()
{
  return this->RecMap;
}

// EnterScope
// bindings : map AS`Name to (REP`TypeRep * nat1)
void StatSem::EnterScope (const MAP<TYPE_AS_Name,Tuple> & bindings)
{
  SET<TYPE_AS_Name> dom_bindings (bindings.Dom());
  MAP<TYPE_AS_Name,TYPE_SSENV_TypeRepElem> env;
  Generic nm;
  for (bool bb = dom_bindings.First(nm); bb; bb = dom_bindings.Next(nm))
    //env.Insert (nm, mk_SSENV_TypeRepElem(bindings[nm].GetRecord(1), Bool(false), Bool(false)));
    env.Insert (nm, mk_SSENV_TypeRepElem(bindings[nm].GetRecord(1),
                                         Bool(bindings[nm].GetIntValue(2) > 1), Bool(false)));
  this->SEnv.ImpPrepend (env);
}

// UpdateScope
// bindings : map AS`Name to (REP`TypeRep * nat1)
void StatSem::UpdateScope (const MAP<TYPE_AS_Name,Tuple> & bindings)
{
  SET<TYPE_AS_Name> dom_bindings (bindings.Dom());
  MAP<TYPE_AS_Name,TYPE_SSENV_TypeRepElem> env (this->SEnv.Hd());
  Generic nm;
  for (bool bb = dom_bindings.First(nm); bb; bb = dom_bindings.Next(nm))
  {
    env.ImpModify (nm,
                   mk_SSENV_TypeRepElem(bindings[nm].GetRecord(1),
                                  (env.DomExists(nm) ? env[nm].GetBool(pos_SSENV_TypeRepElem_used)
                                                     : Bool(false)),
                                  Bool(false)));
  }
  this->SEnv.ImpModify (1, env);
}

// LeaveScope
void StatSem::LeaveScope ()
{
  if (this->SEnv.IsEmpty())
    InternalError(L"LeaveScope");
  else
  {
    TYPE_AS_Name result (ASTAUX::MkNameFromId (ASTAUX::MkId(L"RESULT"), NilContextId));
    MAP<TYPE_AS_Name,TYPE_SSENV_TypeRepElem> env (this->SEnv.Hd());
    SET<TYPE_AS_Name> dom_env (env.Dom()); 
    Generic nm;
    for (bool bb = dom_env.First(nm); bb; bb = dom_env.Next(nm))
    {
      if (!env[nm].GetBool(pos_SSENV_TypeRepElem_used) &&
          (nm != result) &&
          !IsPrePostFn(nm))
        //--------------------
        // Error message #33
        // L"%1" is not used
        //--------------------
        GenErr(nm, WRN1, 33, mk_sequence(PrintName(nm)));
    }
    this->SEnv.ImpTl();
  }
}

// EnterLocalScope
// bindings : map AS`Name to (REP`TypeRep * nat1)
void StatSem::EnterLocalScope (const MAP<TYPE_AS_Name,Tuple> & bindings)
{
  this->LocalEnv.ImpPrepend (bindings);
}

// UpdateLocalScope
// bindings : map AS`Name to (REP`TypeRep * nat1)
void StatSem::UpdateLocalScope (const MAP<TYPE_AS_Name,Tuple> & bindings)
{
  Map LEnvHd (this->LocalEnv.Hd());
  this->LocalEnv.ImpModify (1, LEnvHd.ImpOverride (bindings));
}

// LeaveLocalScope
void StatSem::LeaveLocalScope ()
{
// 20120619 -->
//  if ((this->LocalEnv.IsEmpty()) || (this->SEnv.IsEmpty()))
  if (this->LocalEnv.IsEmpty())
// <-- 20120619
    InternalError(L"LeaveLocalScope");
  else
  {
    this->LocalEnv.ImpTl();
// 20120619 -->
//    LeaveScope();
// <-- 20120619
  }
}

#ifdef VDMPP
// LookUpInObject
// obj : AS`Name
// nm : AS`Name
// writable : bool
// printEror : bool
// return : [AccessType | set of (AccessOpTypeRep | AccessFnTypeRep | AccessPolyTypeRep)]
Generic StatSem::LookUpInObject(const TYPE_AS_Name & obj, const TYPE_AS_Name & nm,
                                bool writable, bool printError)
{
  Tuple key (mk_(obj, nm, Bool(writable), Bool(printError)));
  if (!LookUpInObjectCache.DomExists(key)) {
    Generic tp = LookUpInObjectImpl(obj, nm, writable, printError);
    if (!tp.IsNil() && !printError) {
      LookUpInObjectCache.Insert(key, tp);
    }
    return tp;
  }
  return LookUpInObjectCache[key];
}

// LookUpInObjectImpl
// obj : AS`Name
// nm : AS`Name
// writable : bool
// printEror : bool
// return : [AccessType | set of (AccessOpTypeRep | AccessFnTypeRep | AccessPolyTypeRep)]
Generic StatSem::LookUpInObjectImpl(const TYPE_AS_Name & obj, const TYPE_AS_Name & nm,
                                    bool writable, bool printError)
{
  if (nm.GetSequence(pos_AS_Name_ids).Length() == 2)
  {
    TYPE_AS_Name ncls (ASTAUX::GetFirstName(nm));
    TYPE_AS_Name nnm (ASTAUX::GetSecondName(nm));

    if (obj == ncls || IsSubClass(obj, ncls)) {
      Generic tp (writable ? LookUpInHierarchy(nnm, ncls, STATE, OBJECT)
                           : LookUpInHierarchy (nnm, ncls, VAL, OBJECT));
      Generic tp_q;
      if (tp.IsNil ())
      {
        if ((tp_q = LookUpInHierarchy (nnm, ncls, STATE, LOCAL)).IsNil ())
        {
          //------------------------
          // Error message #34
          // Unknown identifier L"%1"
          //------------------------
          if (printError)
            GenErr(nm, ERR, 34, mk_sequence(PrintName(nm)));
        }
        else
          return tp_q;
      }
      return tp;
    }
    else {
      Generic tp (LookUp(nm, false));
      if (!tp.IsNil())
        return tp;
      else
        //---------------------------------------
        // Error message #391
        // L"Class "%1" has no static member "%2"
        //---------------------------------------
      GenErr(nm, ERR, 391, mk_sequence(PrintName(ncls), PrintName(nnm)));
      return Nil();
    }
  }
  else {
    Generic valtp (writable ? LookUpInHierarchy(nm, obj, STATE, OBJECT)
                            : LookUpInHierarchy(nm, obj, VAL, OBJECT));
    if (valtp.IsNil ())
      return LookUpInHierarchy (nm, obj, STATE, LOCAL);
    else
      return valtp;
  }
}
#ifdef VICE

// NotStaticOk
// ==> ()
void StatSem::NotStaticOk()
{
  this->staticrequired = false;
}

// StaticRequired
// ==> ()
void StatSem::StaticRequired()
{
  this->staticrequired = true;
}
#endif // VICE
#endif // VDMPP

// LookUp
// nm : AS`Name
// printErr : bool
#ifdef VDMSL
// ==> [REP`TypeRep]
#endif // VDMSL
#ifdef VDMPP
// ==> [REP`TypeRep | AccessType  |
//      set of (AccessOpTypeRep | AccessFnTypeRep | AccessPolyTypeRep) | set of REP`TypeRep]
#endif // VDMPP
Generic StatSem::LookUp (const TYPE_AS_Name & nm, bool printErr)
{
// 20110629 -->
#ifdef VDMSL
  if (nm.GetSequence(pos_AS_Name_ids).Length() == 2)
  {
    if (ASTAUX::GetFirstName(nm) == GetCurMod ())
      return LookUp (ASTAUX::GetSecondName(nm), printErr);
  }
#endif // VDMSL
// <-- 20110629
#ifdef VDMPP
  if (nm.GetSequence(pos_AS_Name_ids).Length() == 2)
  {
// 20120216 -->
    //TYPE_AS_Name ncls (ASTAUX::GetFirstName(nm));
    TYPE_AS_Name ncls = ASTAUX::GetFirstName(nm);
    Tuple t (ExpandClassName(ncls, Set()));
    if (t.GetBoolValue(1))
      ncls = t.GetRecord(2);
// <--20120216
    TYPE_AS_Name nnm (ASTAUX::GetSecondName(nm));

    if (ncls == GetCurClass ())
      return LookUp (nnm, printErr);

    Generic tp = Nil();
    if (IsLocalSuperSuper(ncls))
      tp = LookUpInHierarchy(nnm, ncls, VAL, LOCAL);
    else
      tp = LookUpInHierarchy(nnm, ncls, VAL, GLOBAL);

    bool nilOrHasNotStaticAndNotSubClass = false;
    if (tp.IsNil ())
      nilOrHasNotStaticAndNotSubClass = true;
    else {
      bool hasNotStatic = false;
      if (IsAccessType(tp))
      {
        hasNotStatic = !(GetSSStatic(tp).GetValue());
      }
      else
      { //  set of AccessType
        Set tp_set (tp);
        bool exists = false;
        Generic t;
        for (bool bb = tp_set.First(t); bb && !exists; bb = tp_set.Next(t)) {
          exists = !GetSSStatic(t).GetValue();
        }
        hasNotStatic = exists;
      }
#ifdef VICE
      hasNotStatic = hasNotStatic && this->staticrequired;
#endif // VICE

      if (hasNotStatic && (!IsSubClass(CheckClass, ncls)))
        nilOrHasNotStaticAndNotSubClass = true;
      else
        nilOrHasNotStaticAndNotSubClass = false;
    }

    if (nilOrHasNotStaticAndNotSubClass)
    {
      if (printErr
#ifdef VICE
          && this->staticrequired
#endif // VICE
         )
      {
        //------------------------
        // Error message #391
        // L"Class \"%1\" has no static member \"%2\"
        //------------------------
        GenErr(nm, ERR, 391, mk_sequence(PrintName(ncls), PrintName(nnm)));

        return Nil();
      }
#ifdef VICE
      else if (printErr)
      {
        //------------------------
        // Error message #435
        // L"%1 is not defined"
        //------------------------
        GenErr(nm, ERR, 435, mk_sequence(PrintName(ExtName(ncls, nnm))));
        return Nil();
      }
#endif // VICE
      return Nil();
    }
    else
      return tp;
  }
  else
#endif //VDMPP
  {
    size_t depth = this->SEnv.Length();
    for (size_t i = 1 ; i <= depth ; i++)
    {
      MAP<TYPE_AS_Name,TYPE_SSENV_TypeRepElem> currentEnv (this->SEnv[i]);
      if (currentEnv.DomExists(nm))
      {
        TYPE_SSENV_TypeRepElem rc (currentEnv[nm]);
        rc.set_used (Bool(true));
        currentEnv.ImpModify(nm, rc);
        this->SEnv.ImpModify(i, currentEnv);
        if (rc.get_tp().Is(TAG_TYPE_REP_TmpTypeRep))
        {
          //----------------------------------
          // Error message #36
          // Missing type information for L"%1"
          //----------------------------------
          if (printErr)
            GenErr (nm, WRN1, 36, mk_sequence(PrintName (nm)));
          return rep_alltp;
        }
        return rc.get_tp ();
      }
    }

    if (this->ConstEnv.DomExists(nm))
    {
      TYPE_SSENV_TypeRepElem rc (this->ConstEnv[nm]);
      rc.set_used (Bool(true));
      this->ConstEnv.ImpModify (nm, rc);
      if (rc.get_tp().Is(TAG_TYPE_REP_TmpTypeRep))
        //----------------------------------
        // Error message #36
        // Missing type information for L"%1"
        //----------------------------------
      {
        if (printErr)
          GenErr (nm, WRN1, 36, mk_sequence(PrintName (nm)));
        return rep_alltp;
      }
      return rc.get_tp();
    }
    else if (this->ValueIds.InSet (nm))
      return rep_alltp;
    else if (this->StateEnv.DomExists(nm)) {
#ifdef VDMPP
      CheckLookupStatic(nm, TYPE_SSENV_TypeRepElem(this->StateEnv[nm]).get_stat());
#endif //VDMPP
      Int kind (GetContext());
      if ((kind == ALL) ||
#ifdef VDMPP
          (kind == PERM) || (kind == PTHREAD) ||
#endif //VDMPP
          //(((kind == IMPL) || (kind == PRE) || (kind == POST)) && this->ExtEnv.DomExists (nm))
          (((kind == IMPL) || (kind == PRE) || (kind == POST) || (kind == PUREOP)) && this->ExtEnv.DomExists (nm))
         )
      {
        TYPE_SSENV_TypeRepElem rc (this->StateEnv[nm]);
        rc.set_used (Bool(true));
        this->StateEnv.ImpModify (nm, rc);
        return rc.get_tp ();
      }
      else {
        //-----------------------------------------------
        // Error message #37
        // The state component L"%1" must not be used here
        //-----------------------------------------------
        if (printErr)
          GenErr (nm, ERR, 37, mk_sequence(PrintName (nm)));
        return Nil ();
      }
    }
    else if (this->FunctionEnv.DomExists(nm))
    {
      TYPE_SSENV_TypeRepElem rc (this->FunctionEnv[nm]);
#ifdef VDMPP
      CheckLookupStatic(nm, rc.get_stat());
#endif //VDMPP
      rc.set_used (Bool(true));
      this->FunctionEnv.ImpModify (nm, rc);
      return rc.get_tp();
    }
// 20130702 -->
#ifdef VDMSL
    else if (this->PolyEnv.DomExists(nm))
    {
      TYPE_SSENV_PolyTypeRepElem rc (this->PolyEnv[nm]);
      rc.set_used (Bool(true));
      this->PolyEnv.ImpModify (nm, rc);
// 20160525 -->
      //return rc.get_tp();
      return mk_REP_PolyTypeRep(rc.GetSequence(pos_SSENV_PolyTypeRepElem_poly),
                                rc.GetRecord(pos_SSENV_PolyTypeRepElem_tp));
// <-- 20160525
    }
#endif // VDMSL
// <-- 20130702
    else if (this->OperationEnv.DomExists(nm))
    {
      TYPE_SSENV_OpTypeRepElem rc (this->OperationEnv[nm]);
#ifdef VDMPP
      CheckLookupStatic(nm, rc.get_stat());
#endif //VDMPP
// 20151020 -->
      if ((GetContext() == PUREOP) && !rc.GetBoolValue(pos_SSENV_OpTypeRepElem_oppure)) {
        //-----------------------------
        // Error message #460
        // Impure operation call "%1" in pure operation
        //-----------------------------
        if (printErr)
          GenErr (nm, ERR, 460, mk_sequence(PrintName (nm)));
        //return mk_(Bool(false), Rng);
      }
// <-- 20151020
      rc.set_used (Bool(true));
      this->OperationEnv.ImpModify (nm, rc);
      return rc.get_tp();
    }
#ifdef VDMPP
    else if (this->OverloadedEnv.DomExists(nm))
    {
      SET<TYPE_SSENV_TypeRepElem> oe_nm (this->OverloadedEnv[nm]);
      SET<TYPE_SSENV_TypeRepElem> new_oe_nm;
      Set result;
      Generic g;
      for (bool i = oe_nm.First(g); i; i = oe_nm.Next(g))
      {
        TYPE_SSENV_TypeRepElem b_atr(g);
        CheckLookupStatic(nm, b_atr.get_stat());
        b_atr.set_used(Bool(true));
        new_oe_nm.Insert(b_atr);
        result.Insert(b_atr.get_tp());
      }
      this->OverloadedEnv.ImpModify(nm, new_oe_nm);
      return result;
    }
#endif //VDMPP
    else
#ifdef VDMPP
    {
// 20070305
//      Generic tp (LookUpInHierarchy (nm, Nil(), VAL, LOCAL));
      Generic DefClass = CheckClass;
      if  ( !DefiningClass.IsEmpty() )
        DefClass = DefiningClass.Hd();

      Generic tp (LookUpInHierarchy (nm, DefClass, VAL, LOCAL));

// 20070306
      if( tp.IsNil() )
        tp = LookUpInHierarchy (nm, Nil(), VAL, LOCAL);
      
      if (tp.IsNil())
#endif //VDMPP
      {
        //------------------------
        // Error message #34
        // Unknown identifier L"%1"
        //------------------------
        if (printErr)
          GenErr(nm, ERR, 34, mk_sequence(PrintName(nm)));
#ifndef TSI
// 20130710 -->
        //this->ConstEnv.ImpModify (nm, mk_SSENV_TypeRepElem(rep_alltp, Bool(true), Bool(true)));
// <-- 20130710
#endif
        return Nil ();
      }
#ifdef VDMPP
      else
        return tp;
    }
#endif //VDMPP
  }
}

#ifdef VDMPP
// CheckLookupStatic
// nm : AS`Name
// stat : bool
void StatSem::CheckLookupStatic(const TYPE_AS_Name & nm, const Bool & stat)
{
  if (GetStatic().GetValue() && !stat.GetValue())
    // -------------------------------------------------
    // -- Error message #380
    // -- Can not refer to non-static class member %1 in
    // -- a static class member
    // -------------------------------------------------
    GenErr(nm, ERR, 380, mk_sequence(PrintName(nm)));
}
#endif //VDMPP

#ifdef VDMSL
// LookUpTypeName
// nm : AS`Name
// ==> [REP`TypeRep]
Generic StatSem::LookUpTypeName (const TYPE_AS_Name & nm)
#endif // VDMSL
#ifdef VDMPP
// LookUpTypeName
// nm : AS`Name
// printerr : bool
// ==> [REP`TypeRep|AccessType]
Generic StatSem::LookUpTypeName (const TYPE_AS_Name & nm, bool printerr)
{

  Tuple key (mk_(nm, GetCurClass (), Bool(printerr)));
  Generic tpx;
  if (!this->LookUpTypeNameCache.DomExists(key)) {
    Generic tp = LookUpTypeName_q(nm, printerr);
    if (!tp.IsNil() || !printerr) {
      this->LookUpTypeNameCache.Insert(key, mk_(tp, this->FoundClass));
    }
    return tp;
  }
  else {
    Tuple t (this->LookUpTypeNameCache[key]);
    this->FoundClass = t.GetField(2);
    return t.GetField(1);
  }

//  return LookUpTypeName_q(nm, printerr);
}

// LookUpTypeName
// nm : AS`Name
// printerr : bool
// ==> [REP`TypeRep|AccessType]
Generic StatSem::LookUpTypeName_q (const TYPE_AS_Name & nm, bool printerr)
#endif // VDMPP
{
  Generic gtmp;
#ifdef VDMPP
// 20070309
  this->FoundClass = Nil();

  if (nm.GetSequence(pos_AS_Name_ids).Length() == 2)
  {
    TYPE_AS_Name ncls (ASTAUX::GetFirstName(nm));
    TYPE_AS_Name nnm (ASTAUX::GetSecondName(nm));

    if (ncls == GetCurClass ())
      //return LookUpTypeName (nnm, true);
      return LookUpTypeName_q (nnm, true);
    else
    {
      Generic tp (LookUpInHierarchy(nnm, ncls, TYPE, GLOBAL));

      if (tp.IsNil ())
      {
        if (CheckClassName(nnm))
          return mk_REP_ObjRefTypeRep(nnm);
        else
        {
          //------------------------
          // Error message #34
          // Unknown identifier L"%1"
          //------------------------
          if (printerr)
            GenErr(nm, ERR, 34, mk_sequence(PrintName(nm)));
        }
      }
      return tp;
    }
  }
  else
#endif //VDMPP
  { // (nm.GetSequence(pos_AS_Name_ids).Length() == 1)
#ifdef VDMSL
  if ((nm.GetSequence(pos_AS_Name_ids).Length() == 2) && 
       ASTAUX::GetFirstName(nm) == GetCurMod())
  {
    return LookUpTypeName (ASTAUX::GetSecondName(nm));
  }
  else
// 20140321 -->
//    if (this->RenamedTypes.DomExists (nm))
//      return LookUpTypeName (this->RenamedTypes[nm]);
//    else if (InsideInter && this->RenamedTypes.Rng ().InSet (nm))
//    {
//      //-----------------------------------------------------------
//      // Error message #38
//      // The renamed type L"%1" cannot be used in the interface part
//      //-----------------------------------------------------------
//      GenErr (nm, ERR, 38, mk_sequence(PrintName (nm)));
//      return Nil ();
//    }
    if (this->RenamedTypes.DomExists (nm))
    {
      if (InsideInter) {
        //-----------------------------------------------------------
        // Error message #38
        // The renamed type L"%1" cannot be used in the interface part
        //-----------------------------------------------------------
        GenErr (nm, ERR, 38, mk_sequence(PrintName (nm)));
        return Nil ();
      }
      else
        return LookUpTypeName (this->RenamedTypes[nm]);
    }
// <-- 20140321
    else
#endif //VDMSL
    if (this->TypeEnv.DomExists (nm))
    {
      TYPE_SSENV_TypeRepElem rc (this->TypeEnv[nm]);
      // PERFORMANCE: Only update the 'used' field if it is false.
      // ETN 960105.
      if (! rc.GetBool(pos_SSENV_TypeRepElem_used)) {
        rc.SetField(pos_SSENV_TypeRepElem_used, Bool(true));
        this->TypeEnv.ImpModify(nm, rc);
      }
#ifdef VDMPP
      this->FoundClass = GetCurClass();
#endif // VDMPP
      return rc.get_tp();
    }
    else
#ifdef VDMPP
    {
      Generic DefClass = CheckClass;
      if ( !DefiningClass.IsEmpty() )
        DefClass = DefiningClass.Hd();

      Generic tp (LookUpInHierarchy (nm, DefClass, TYPE, LOCAL));

      if (tp.IsNil())
        tp = LookUpInHierarchy (nm, Nil(), TYPE, LOCAL);

      if (tp.IsNil()) {
        if (CheckClassName(nm)) {
          return mk_REP_ObjRefTypeRep(nm);
        }
        else if (this->TagEnv.DomExists(nm)) {
          TYPE_REP_CompositeTypeRep ctr (mk_REP_CompositeTypeRep(nm, this->TagEnv[nm]));
          this->TypeEnv.Insert(nm, mk_SSENV_TypeRepElem(ctr, Bool(true), Bool(false)));
          return ctr;
        }
        else {
          //------------------------
          // Error message #34
          // Unknown identifier L"%1"
          //------------------------
          if (printerr)
            GenErr(nm, ERR, 34, mk_sequence(PrintName(nm)));
          this->TypeEnv.Insert(nm, mk_SSENV_TypeRepElem(rep_alltp, Bool(true), Bool(false)));
          return Nil ();
        }
      }
      return tp;
    }
#endif //VDMPP
#ifdef VDMSL
    {
      if (this->TagEnv.DomExists(nm))
      {
        TYPE_REP_CompositeTypeRep ctr (mk_REP_CompositeTypeRep(nm, this->TagEnv[nm]));
        this->TypeEnv.Insert(nm, mk_SSENV_TypeRepElem(ctr, Bool(true), Bool(false)));
        return ctr;
      }
      //------------------------
      // Error message #34
      // Unknown identifier L"%1"
      //------------------------
      GenErr(nm, ERR, 34, mk_sequence(PrintName(nm)));
      this->TypeEnv.Insert(nm, mk_SSENV_TypeRepElem(rep_alltp, Bool(true), Bool(false)));
      return Nil ();
    }
#endif //VDMSL
  }
}

#ifdef VDMPP
// LookUpDefClassForTypeName
// nm : AS`Name
// ==> [AS`Name]
Generic StatSem::LookUpDefClassForTypeName(const TYPE_AS_Name & nm)
{
  LookUpTypeName(nm, true);
  return this->FoundClass;
}

// LookUpDefClass
// ==> [AS`Name]
Generic StatSem::LookUpDefClass ()
{
/*
  if (DefiningClass.IsEmpty())
    return this->FoundClass;
  else
    return this->DefiningClass.Hd();
*/

// impl 20070306
  if (DefiningClass.IsEmpty())
    return Nil();
  else
    return this->FoundClass;
}

// SetDefClass
// clnm : [AS`Name]
// ==> ()
void StatSem::SetDefClass(const Generic & clnm)
{
  if (clnm.IsNil())
  {
    if (!this->DefiningClass.IsEmpty())
      this->DefiningClass.ImpTl();
  }
  else
    this->DefiningClass.ImpPrepend(clnm);
}

// UnSetDefClass
// ==> ()
void StatSem::UnSetDefClass()
{
  if (!this->DefiningClass.IsEmpty())
  {
    this->FoundClass = this->DefiningClass.Hd();
    this->DefiningClass.ImpTl();
  }
  else
    this->FoundClass = Nil();
}
#endif // VDMPP

// LookUpTag
// i : TYPE`Ind
// nm : AS`Name
#ifdef VDMSL
// ==> [AS`Name * seq of REP`FieldRep]
#endif // VDMLSL
#ifdef VDMPP
// ==> [AS`Name * (AccessFieldRep|TagRepElem)]
#endif // VDMPP
Generic StatSem::LookUpTag (const Int & i, const TYPE_AS_Name & nm, const SET<TYPE_AS_Name> & nm_s)
{
#ifdef VDMSL
  if (nm_s.InSet(nm))
    return Nil();
  if (nm.GetSequence(pos_AS_Name_ids).Length() == 2)
  {
    TYPE_AS_Name nmod (ASTAUX::GetFirstName(nm));
    if (nmod == GetCurMod ()) {
      TYPE_AS_Name nnm (ASTAUX::GetSecondName(nm));
      return LookUpTag (i, nnm, Set(nm_s).Insert(nm));
    }
    // skip
  }
  if (CheckLocalTag(nm))
    if (this->RenamedTypes.DomExists(nm))
      return mk_(this->RenamedTypes[nm], this->TagEnv[nm]);
    else
    return mk_(nm, this->TagEnv[nm]);
  else if (CheckImportedTag(nm))
    return mk_(nm, this->LocTagEnv[nm]);
  else
    return Nil ();
#endif //VDMSL
#ifdef VDMPP
  if (nm_s.InSet(nm))
    return Nil();
  if (nm.GetSequence(pos_AS_Name_ids).Length() == 2)
  {
    TYPE_AS_Name ncls (ASTAUX::GetFirstName(nm));
    TYPE_AS_Name nnm (ASTAUX::GetSecondName(nm));

    if (ncls == GetCurClass ())
      return LookUpTag (i, nnm, Set(nm_s).Insert(nm));
    else
      return LookUpInHierarchy(nnm, ncls, TAG, GLOBAL);
  }
  else
  if (CheckLocalTag(nm))
    return mk_(nm, this->TagEnv[nm]);
  else
    return LookUpInHierarchy (nm, Nil(), TAG, LOCAL);
#endif //VDMPP
}

#ifdef VDMPP
// LookUpOperationName
// classnm : [AS`Name]
// metnm : AS`Name
// objnm : [AS`Expr]
// ==> set of AccessType
Set StatSem::LookUpOperationName (const Generic & classnm, const TYPE_AS_Name & metnm, const Generic & objnm)
{
  Generic cls = classnm;
  if (cls.IsNil()) {
    if (metnm.GetSequence(pos_AS_Name_ids).Length() == 2) {
      cls = ASTAUX::GetFirstName(metnm);
    } else {
      cls = GetCurClass();
    }
  }
  Generic tps (LookUpInObject(cls, metnm, false, true));

  if (tps.IsNil() && objnm.IsNil())
    tps = LookUp(metnm, false);

  if (IsAccessType(tps))
  {
    switch(TYPE_SSENV_AccessType(tps).GetTag()) {
      case TAG_TYPE_SSENV_AccessOpTypeRep: {
        CheckLookupStatic(metnm, (!objnm.IsNil() ? Bool(true) : GetSSStatic(tps)));
        return Set().Insert(tps); // set of AccessOpTypeRep
        break;
      }
      case TAG_TYPE_SSENV_AccessFnTypeRep: {
        return Set().Insert(tps); // set of AccessFnTypeRep
        break;
      }
      default:
        return Set();
        break;
    }
  }
  else if (IsAccessTypeSet(tps))
  {
    SET<TYPE_SSENV_AccessType> tpss (tps);
    SET<TYPE_SSENV_AccessType> resvals;
    Generic b_tp;
    for (bool bb = tpss.First(b_tp); bb; bb = tpss.Next(b_tp))
    {
      if (b_tp.Is(TAG_TYPE_SSENV_AccessOpTypeRep)) {
        CheckLookupStatic(metnm, (!objnm.IsNil() ? Bool(true) : GetSSStatic(b_tp)));
        resvals.Insert(b_tp);
      }
    }
    return resvals; // set of AccessOpTypeRep
  }
  else 
    return Set();
}
#endif // VDMPP

#ifdef VDMSL
// LookUpState
// nm : AS`Name
// use :  bool
// kind : (<CUR>|<OLD>)
// ==> [REP`TypeRep]
Generic StatSem::LookUpState (const TYPE_AS_Name & nm, const Bool & use, const Int & kind)
{
  if (kind == CUR)
  {
    size_t depth = this->LocalEnv.Length();
    for (size_t i = 1 ; i <= depth ; i++)
    {
      MAP<TYPE_AS_Name, Tuple> currentEnv (this->LocalEnv[i]); // map AS`Name to (REP`TypeRep * nat1)
      if (currentEnv.DomExists (nm))
      {
        size_t len_SEnv = this->SEnv.Length();
        for (size_t j = 1; j <= len_SEnv; j++)
        {
          if (this->SEnv[j].DomExists (nm))
          {
            if (use)
            {
              Map mp (this->SEnv[j]);
              TYPE_SSENV_TypeRepElem rc (mp[nm]);
              rc.SetField(pos_SSENV_TypeRepElem_used, Bool(true));
              mp.ImpModify (nm, rc);
              this->SEnv.ImpModify (j, mp);
            }
            return currentEnv[nm].GetRecord(1);
          }
        }
      }
    }
  }
  else if (! this->ExtEnv.DomExists (nm))
  {
    //-------------------------
    // Error message #42
    // L"%1" cannot be used here
    //-------------------------
    GenErr (nm, ERR, 42, mk_sequence(PrintName(nm)));
    return Nil ();
  }
  else if (this->ExtEnv[nm] == Int (READ))
  {
    //-------------------------------------------------------
    // Error message #43
    // The state component L"%1" can only be used in read mode
    //-------------------------------------------------------
    GenErr(nm, ERR, 43, mk_sequence(PrintName(nm)));
    return Nil ();
  }

  if (this->StateEnv.DomExists(nm))
  {
    bool exists = false;
    size_t len = this->SEnv.Length();
    for (size_t idx = 1; (idx <= len) && !exists; idx++)
    {
      Map m (this->SEnv[idx]);
      exists = m.DomExists(nm);
    }
    if (exists)
    {
      if (GetContext() != AS_PURE) {
        //----------------------------------------------------
        // Error message #18
        // The scope of the state component L"%1" is now hidden
        //----------------------------------------------------
        GenErr(nm, ERR, 18, mk_sequence(PrintName(nm)));
      }
    }
    else if (GetContext() == AS_PURE) {
      //-----------------------------------------------
      // Error message #37
      // The state component L"%1" must not be used here
      //-----------------------------------------------
      GenErr (nm, ERR, 37, mk_sequence(PrintName (nm)));
    }
    TYPE_SSENV_TypeRepElem rc (this->StateEnv[nm]);
    rc.SetField(pos_SSENV_TypeRepElem_used, Bool(true));
    this->StateEnv.ImpModify (nm, rc);
    return rc.GetField(pos_SSENV_TypeRepElem_tp);
  }
  else
  {
    //-----------------------------
    // Error message #40
    // Unknown state reference L"%1"
    //-----------------------------
    GenErr(nm, ERR, 40, mk_sequence(PrintName(nm)));
    return Nil ();
  }
  return Nil (); // To avoid warnings
}
#endif // VDMSL

#ifdef VDMPP
// LookUpState
// nm : AS`Name
// use : bool
// kind : (<CUR>|<OLD>)
// ==> [REP`TypeRep|AccessType]
Generic StatSem::LookUpState (const TYPE_AS_Name & nm, const Bool & use, const Int & kind)
{
  if (nm.GetSequence(pos_AS_Name_ids).Length() == 2)
  {
    TYPE_AS_Name ncls (ASTAUX::GetFirstName(nm));
    TYPE_AS_Name nnm (ASTAUX::GetSecondName(nm));

    if (ncls == GetCurClass ())
    {
      if (kind == CUR)
      {
        if (GetStatic().GetValue() && IsStaticInstanceVar(nm))
        {
          Generic tp (LookUpInHierarchy(nnm, ncls, STATE, LOCAL));
          if (!tp.IsNil ())
            return tp;
        }
      } 
      return LookUpState (nnm, use, kind);
    }
    else if (IsLocalSuperSuper (ncls))
    {
      Generic tp (LookUpInHierarchy(nnm, ncls, STATE, LOCAL));
      if (tp.IsNil ())
      {
        ////------------------------
        //// Error message #34
        //// Unknown identifier L"%1"
        ////------------------------
        //GenErr(nm, ERR, 34, mk_sequence(PrintName(nm)));
        //-----------------------------
        // Error message #40
        // Unknown state reference L"%1"
        //-----------------------------
        GenErr(nm, ERR, 40, mk_sequence(PrintName(nm)));
        return Nil ();
      }
      return tp;
    }
    else
    {
      if (IsStaticInstanceVar(nm))
      {
        Generic tp (LookUpInHierarchy(nnm, ncls, STATE, LOCAL));
        if (!tp.IsNil ())
          return tp;
      }
      //----------------------------------------------
      // Error message #44
      // Instance variables are not visible from class
      // L"%1" so "%2" cannot be used here
      //----------------------------------------------
      GenErr (nm, ERR, 44, mk_sequence(PrintName(ncls), PrintName(nnm)));
      return Nil();
    }
  }
  else
  {
    if (kind == CUR)
    {
      size_t depth = this->LocalEnv.Length();
      for (size_t i = 1 ; i <= depth ; i++)
      {
        MAP<TYPE_AS_Name,Tuple> currentEnv (this->LocalEnv[i]);
        if (currentEnv.DomExists (nm))
        {
          size_t len_SEnv = this->SEnv.Length ();
          for (size_t j = 1; j <= len_SEnv; j++)
          {
            if (this->SEnv[j].DomExists (nm))
              return currentEnv[nm].GetRecord(1);
          }
        }
      }
    }
    else if (!(GetContext() == POST) || this->ExtEnv.DomExists (nm))
    {
      if (this->StateEnv.DomExists(nm))
      {
        TYPE_SSENV_TypeRepElem rc (this->StateEnv[nm]);
        rc.SetField(pos_SSENV_TypeRepElem_used, use);
        this->StateEnv.ImpModify (nm, rc);

        if ((kind == Int (OLD)) && (this->ExtEnv[nm] == Int (READ)))
        {
          //-------------------------------------------------------
          // Error message #43
          // The state component L"%1" can only be used in read mode
          //-------------------------------------------------------
          GenErr (nm, ERR, 43, mk_sequence(PrintName(nm)));
          return Nil ();
        }
        else
          return rc.get_tp();
      }
      else
      {
        Generic tp (LookUpInHierarchy (nm, Nil(), STATE,LOCAL));
        if (tp.IsNil())
        {
          //-----------------------------
          // Error message #40
          // Unknown state reference L"%1"
          //-----------------------------
          GenErr(nm, ERR, 40, mk_sequence(PrintName(nm)));
          return Nil ();
        }
        else
          return tp;
      }
    }

    if (this->StateEnv.DomExists(nm))
    {
      bool exists = false;
      size_t len = this->SEnv.Length();
      for (size_t idx = 1; (idx <= len) && !exists; idx++)
      {
        Map m (this->SEnv[idx]);
        exists = m.DomExists(nm);
      }
      if (exists)
      {
        if (GetContext() != AS_PURE) {
          //----------------------------------------------------
          // Error message #18
          // The scope of the state component L"%1" is now hidden
          //----------------------------------------------------
          GenErr(nm, ERR, 18, mk_sequence(PrintName(nm)));
        }
      }
      else if (GetContext() == AS_PURE) {
        //-----------------------------------------------
        // Error message #37
        // The state component L"%1" must not be used here
        //-----------------------------------------------
        GenErr (nm, ERR, 37, mk_sequence(PrintName (nm)));
      }

      TYPE_SSENV_TypeRepElem rc (this->StateEnv[nm]);
      rc.SetField(pos_SSENV_TypeRepElem_used, use);
      this->StateEnv.ImpModify (nm, rc);
      if (GetStatic() && !rc.GetBoolValue(pos_SSENV_TypeRepElem_stat))
      {
        //-----------------------------
        // Error message #422
        // Error L"%1"
        //-----------------------------
        GenErr(nm, ERR, 422, mk_sequence(PrintName(nm)));
        return Nil();
      }
      return rc.get_tp();
    }
    else
    {
      Generic tpii (FoundAsInstanceInSEnv(nm));
      if (!tpii.IsNil())
        return tpii;
      Generic tp (LookUpInHierarchy (nm, Nil(), STATE,LOCAL));
      if (tp.IsNil()) {
        size_t len_SEnv = this->SEnv.Length ();
        for (size_t j = 1; j <= len_SEnv; j++)
        {
            if (this->SEnv[j].DomExists (nm)) {
          
            Map pm (this->SEnv[j]);
            TYPE_SSENV_TypeRepElem tre (pm[nm]);
            TYPE_REP_TypeRep tr (tre.GetRecord(pos_SSENV_TypeRepElem_tp));
            switch (tr.GetTag()) {
              case TAG_TYPE_REP_ObjRefTypeRep: {
                tre.SetField(pos_SSENV_TypeRepElem_used, Bool(true));
                pm.ImpModify(nm, tre);
                this->SEnv.ImpModify(j, pm);
                return tr;
              }
              case TAG_TYPE_REP_UnionTypeRep: {
                SET<TYPE_REP_TypeRep> tp_s (tr.GetSet(pos_REP_UnionTypeRep_tps));
                bool exists = false;
                Generic tp;
                for (bool bb = tp_s.First(tp); bb && !exists; bb = tp_s.Next(tp)) {
                  exists = tp.Is(TAG_TYPE_REP_ObjRefTypeRep);
                }
                if (exists) {
                  tre.SetField(pos_SSENV_TypeRepElem_used, Bool(true));
                  pm.ImpModify(nm, tre);
                  this->SEnv.ImpModify(j, pm);
                  return tr;
                }
                break;
              }
            }
          }
        }
      }
      if (tp.IsNil())
      {
        //-----------------------------
        // Error message #40
        // Unknown state reference L"%1"
        //-----------------------------
        GenErr(nm, ERR, 40, mk_sequence(PrintName(nm)));
        return Nil ();
      }
      else
        return tp;
    }
  }
  return Nil(); // To avoid warnings
}

// FoundAsInstanceInSEnv
// nm : AS`Name
// ==> [REP`TypeRep]
Generic StatSem::FoundAsInstanceInSEnv(const TYPE_AS_Name & nm)
{
  size_t depth = this->SEnv.Length();
  for (size_t i = 1; i <= depth; i++)
  {
    Map currentEnv (this->SEnv[i]); // map AS`Name to TypeRepElem
    if (currentEnv.DomExists(nm))
    {
      TYPE_SSENV_TypeRepElem tre (currentEnv[nm]);
      TYPE_REP_TypeRep tp (tre.GetRecord(pos_SSENV_TypeRepElem_tp));
      if (tp.Is(TAG_TYPE_REP_TypeNameRep))
      {
        if (IsClassName(tp.GetRecord(pos_REP_TypeNameRep_nm)))
        {
          tre.SetField(pos_SSENV_TypeRepElem_used, Bool(true));
          currentEnv.ImpModify(nm, tre);
          this->SEnv.ImpModify(i, currentEnv); 
          return tp;
        }
      }
    }
  }
  return Nil();
}

// LookUpClass
// nm : AS`Name
// ==> ParseTypeInfo
TYPE_SSENV_ParseTypeInfo StatSem::LookUpClass (const TYPE_AS_Name & nm)
{
  Generic cls (GetClassTypeRep(nm));
  if (cls.IsNil())
  {
    //----------------------------------------
    // Error message #1
    // The class L"%1" has not been pre-checked
    //----------------------------------------
    GenErr(nm, ERR, 1, mk_sequence(PrintName(nm)));
    return MakeEmptyClass(nm);
  }
  else
    return cls;
}

// DefaultConstructorAccessible
// p_nm : AS`Name
// ==> bool
bool StatSem::DefaultConstructorAccessible(const TYPE_AS_Name & p_nm)
{
  Generic l_classInfo (GetClassTypeRep(p_nm));
  SET<TYPE_SSENV_AccessOpTypeRep> l_constrs;
  if (!l_classInfo.IsNil())
    l_constrs.ImpUnion(Record(l_classInfo).GetSet(pos_SSENV_ParseTypeInfo_constructors));

  SET<TYPE_SSENV_AccessOpTypeRep> l_defConstrs;
  Generic b_constr;
  for (bool bb = l_constrs.First(b_constr); bb; bb = l_constrs.Next(b_constr))
  {
    if (Record(b_constr).GetRecord(pos_SSENV_AccessOpTypeRep_tp)
                        .GetSequence(pos_REP_OpTypeRep_Dom)
                        .IsEmpty())
      l_defConstrs.Insert(b_constr);
  }

  if (l_defConstrs.IsEmpty())
    return true;
  else
  {
    TYPE_SSENV_AccessOpTypeRep l_defConstr (l_defConstrs.GetElem());
    if (GetCurClass() == p_nm)
      return true;
    else if (IsSubClass(GetCurClass(), p_nm))
      return (l_defConstr.get_a() != Int(PRIVATE_AS));
    else
      return (l_defConstr.get_a() == Int(PUBLIC_AS));
  }
}

// ConstructorExists
// p_i : TYPE`Ind
// p_nm : AS`Name
// p_argTps : seq of REP`TypeRep
// p_expr : AS`Expr
// local : bool (not in spec) ... for check non default constructor is defined locally
// ==> bool
bool StatSem::ConstructorExists (const Int & p_i,
                                      const TYPE_AS_Name & p_nm,
                                      const SEQ<TYPE_REP_TypeRep> & p_argTps,
                                      const TYPE_AS_Expr & p_expr,
                                      bool local)
{
  // Note that the implementation of this function is quite different
  // to the specificiation, since at the specification level there is
  // a series of O(n^3) set comprehensions. Here, using a kind of lazy
  // evaluation, as soon as a match is found a result can be returned.

  MAP<Int, Set> l_constrs (LookUpConstructors(p_nm)); // map nat to set of set of AccessOpTypeRep

  SET<Int> dom_constrs (l_constrs.Dom()); // set of nat
  int l_max = 0;
  if (!local) {
    Generic num;
    for (bool bb = dom_constrs.First(num); bb; bb = dom_constrs.Next(num))
      if (Int(num).GetValue() > l_max)
        l_max = Int(num).GetValue();
  }

  bool local_constr (GetCurClass() == p_nm);
  bool super_constr (IsSubClass(GetCurClass(), p_nm));

  Set b_s_public;
  for (int level = 0; level <= l_max; level++)
  {
    if (l_constrs.DomExists(Int(level)))
    {
      Set l_iConstrsCl (l_constrs[Int(level)].DUnion()); // set of AccessOpTypeRep
      Generic aotr; // ENV`AccessOpTypeRep
      for (bool dd = l_iConstrsCl.First(aotr); dd; dd = l_iConstrsCl.Next(aotr))
      {
        TYPE_SSENV_AccessOpTypeRep b_acc (aotr);
        if ((b_acc.get_a() == Int(PUBLIC_AS)) ||
            local_constr ||
            (super_constr && b_acc.get_a() == Int(PROTECTED_AS)))
        {
          const TYPE_REP_OpTypeRep & l_otr (b_acc.GetRecord(pos_SSENV_AccessOpTypeRep_tp));
          if (l_otr.get_Dom().Length() == p_argTps.Length())
          {
            const TYPE_REP_TypeRep & rng (l_otr.GetRecord(pos_REP_OpTypeRep_Rng));
            if (rng.Is(TAG_TYPE_REP_TypeNameRep))
            {
              const SEQ<TYPE_REP_TypeRep> & dom (l_otr.GetSequence(pos_REP_OpTypeRep_Dom));
              const TYPE_AS_Name & nm (rng.GetRecord(pos_REP_TypeNameRep_nm));
              bool forall = true;
              Generic t;
              for (bool ee = b_s_public.First(t); ee && forall; ee = b_s_public.Next(t))
              {
                TYPE_REP_TotalFnTypeRep tftr (t);
                const TYPE_REP_TypeRep & ptr (tftr.GetRecord(pos_REP_TotalFnTypeRep_fnrng));
                if (ptr.Is(TAG_TYPE_REP_TypeNameRep))
                {
                  const SEQ<TYPE_REP_TypeRep> & fndom (tftr.GetSequence(pos_REP_TotalFnTypeRep_fndom));
                  bool exists = !IsSubClass(ptr.GetRecord(pos_REP_TypeNameRep_nm), nm);
                  size_t len_fndom = fndom.Length(); 
                  for (size_t idx = 1; idx <= len_fndom && !exists; idx++)
                  {
                    //exists = !IsSubType(dom[idx], fndom[idx], Set());
                    exists = !IsCompatible(p_i, dom[idx], fndom[idx]);
                  }
                  forall = exists;
                }
              }
              if (forall)
                b_s_public.Insert(mk_REP_TotalFnTypeRep(l_otr.get_Dom(), rng));
            }
          }
        }
      }
    }
  }
  Tuple t (CheckOverloadedApply(p_i, b_s_public, p_argTps, p_expr, false, false));
  return t.GetBoolValue(1);
}

// LookUpConstructors
// p_nm : AS`Name
// ==> map nat to set of set of AccessOpTypeRep
Map StatSem::LookUpConstructors(const TYPE_AS_Name & p_nm)
{
  Map l_result; // map nat to set of set of AccessOpTypeRep

  // Local Constructor for the class
  Generic l_classInfo (GetClassTypeRep(p_nm)); // [ParseTypeInfo]

  SET<Set> l_constructors; // set of set of AccessOpTypeRep
  if (!l_classInfo.IsNil())
  {
    l_constructors.Insert(Record(l_classInfo).GetSet(pos_SSENV_ParseTypeInfo_constructors));
  }
  l_result.Insert(Int(0), l_constructors);

// 20091021 -->
// TODO:
// <-- 20091021

  // Constructor for super class
  Map l_indexedSupers (TransClosIndex(p_nm));  // map nat to set of AS`Name
  SET<TYPE_AS_Name> dom_l_indexedSupers (l_indexedSupers.Dom());
  Generic b_i;
  for (bool bb = dom_l_indexedSupers.First(b_i); bb; bb = dom_l_indexedSupers.Next(b_i))
  {
    SET<Set> l_theseConstructors;
    SET<TYPE_AS_Name> l_theseSupers (l_indexedSupers[b_i]);
    Generic b_cl; // AS`Name
    for (bool cc = l_theseSupers.First(b_cl); cc; cc = l_theseSupers.Next(b_cl))
    {
      Generic l_theseClassInfo (GetClassTypeRep(b_cl));
      if (!l_theseClassInfo.IsNil())
      {
        l_theseConstructors.Insert(Record(l_theseClassInfo).GetSet(pos_SSENV_ParseTypeInfo_constructors));
      }
    }
    l_result.Insert(b_i, l_theseConstructors);
  }

  return l_result;
}

// CheckClassName
// nm : AS`Name
// ==> bool
bool StatSem::CheckClassName (const TYPE_AS_Name & nm)
{
  return !GetClassTypeRep(nm).IsNil();
}

// HasThread
// nm : AS`Name
// ==> bool
bool StatSem::HasThread(const TYPE_AS_Name & nm)
{
  Generic classti (GetClassTypeRep(nm));
  if (classti.IsNil())
    return false;
  else
  {
    TYPE_SSENV_ParseTypeInfo pti (classti);
    if (pti.GetIntValue(pos_SSENV_ParseTypeInfo_thread) != NONE)
      return true;
    else {
      SET<TYPE_AS_Name> super (pti.GetSet(pos_SSENV_ParseTypeInfo_super));
      bool exists = false;
      Generic cls;
      for (bool bb = super.First(cls); bb && !exists; bb = super.Next(cls))
        exists = HasThread(cls);
      return exists;
    }
  }
}

// CheckTraceName
// nm : AS`Name
// ==> bool
bool StatSem::CheckTraceName (const TYPE_AS_Name & nm) const
{
  return bool (this->TraceEnv.InSet (nm));
}

// LookUpInHierarchy
// nm : AS`Name
// classid : [AS`Name]
// kind : (<VAL>|<TYPE>|<TAG>|<STATE>)
// local : (<OBJECT>|<LOCAL>|<GLOBAL>)
// ==> [REP`TypeRep | AccessType | (AS`Name * AccessFieldRep) | set of AccessType]
Generic StatSem::LookUpInHierarchy (const TYPE_AS_Name & nm, const Generic & classid, int kind, int local)
{
// 20070309
  this->FoundClass = Nil();

  Map binds; // map AS`Name to (REP`TypeRep | AccessType | (AS`Name * AccessFieldRep) | set of AccessType)
  SET<TYPE_AS_Name> super;
  if (classid.IsNil())
    super = this->Super;
  else
    super.Insert (classid);

  Generic sup;
  for (bool bb = super.First(sup); bb; bb = super.Next(sup))
  {
    binds.ImpOverride (LookUpInHierarchyAux (nm, sup, kind, local, Set()));
  }

  if (binds.IsEmpty())
  {
    return Nil();
  }
  else if (binds.Size() == 1)
  {
    // 20070307
//    SetDefClass(binds.Dom ().GetElem ());
//    Generic res (binds[this->DefiningClass.Hd()]);
//    UnSetDefClass();
//    Generic res (binds[this->FoundClass]);
//    return res;
// 20070309    
    this->FoundClass = binds.Dom().GetElem();
    return binds[this->FoundClass];
  }
  else
  {
    Set rng_binds (binds.Rng());
    bool exists = false;
    Generic s;
    for (bool cc = rng_binds.First(s); cc && !exists; cc = rng_binds.Next(s))
    {
      exists = IsSetOfAccessType(s);
    }

    if (exists || IsAccessTypeSet(rng_binds))
    {
      Map l_setBinds;
      Set dom_binds (binds.Dom());
      Generic b_nm;
      for (bool ee = dom_binds.First(b_nm); ee; ee = dom_binds.Next(b_nm))
      {
        Generic acs (binds[b_nm]);
        if (IsSetOfAccessType(acs))
          l_setBinds.Insert(b_nm, acs);
        else
          l_setBinds.Insert(b_nm, Set().Insert(acs));
      }
      Set l_tps (l_setBinds.Rng().DInter());

      if (!l_tps.IsEmpty())
      {
// 20120905 -->
        Set dom_l_setBinds (l_setBinds.Dom());
        Generic n;
        for (bool ii = dom_l_setBinds.First(n); ii; ii = dom_l_setBinds.Next(n))
        {
          Generic l_pte (GetClassTypeRep(n));
          if (!l_pte.IsNil())
          {
            const TYPE_SSENV_ParseTypeInfo & pti = l_pte;
            Map subresps (pti.GetMap(pos_SSENV_ParseTypeInfo_subresps));
            TYPE_AS_Name nn (ASTAUX::Combine2Names(n, nm));
            if (subresps.DomExists(nn))
            {
              Generic atp (subresps[nn]);
              Set atps (atp.IsSet() ? Set(atp) : mk_set(atp));
              if (l_tps.Diff(atps).IsEmpty())
                l_setBinds.RemElem(n); 
            }
          }
        }
        if (l_setBinds.Size() == 1)
        {
          this->FoundClass = l_setBinds.Dom().GetElem();
          return binds[this->FoundClass];
        }
// <-- 20120905

        //--------------------------------------------
        //-- Error message #45
        //-- L"%1" is multiple defined in super classes
        //--------------------------------------------
        GenErr(nm, ERR, 45, mk_sequence(PrintName(nm)));
        return Nil();
      }
      else
      {
        // ------------------------------------------------------
        // -- split on polymorphic or not
        // -------------------------------------------------------
        Set flatBindsRng (l_setBinds.Rng().DUnion());

        Set poly;
        Set non_poly;
        Generic b_acc;
        for (bool ff = flatBindsRng.First(b_acc); ff; ff = flatBindsRng.Next(b_acc))
        {
          if (b_acc.Is(TAG_TYPE_SSENV_AccessPolyTypeRep))
            poly.Insert(b_acc);
          else
            non_poly.Insert(b_acc);
        }

        //  ------------------------------------------------------
        //  -- split polymorphic types on the number of parameters
        //  -------------------------------------------------------
        Map param_lengths_m; //  map nat to set of SS`AccessPolyTypeRep
        Generic bd;
        for (bool gg = poly.First(bd); gg; gg = poly.Next(bd))
        {
          TYPE_REP_PolyTypeRep tp (Record(bd).GetField(pos_SSENV_AccessPolyTypeRep_tp));
          Int param_length (tp.GetSequence(pos_REP_PolyTypeRep_vars).Length());

          Set tmp_s;
          if (param_lengths_m.DomExists(param_length))
            tmp_s = param_lengths_m[param_length];
          tmp_s.Insert(bd);
          param_lengths_m.ImpModify(param_length, tmp_s);
        }

        Set overlapping_poly (param_lengths_m.Rng());

        Set non_poly_consistency (TypesAreConsistent(non_poly));

        Set poly_consistency_set;
        Generic dd;
        for (bool hh = overlapping_poly.First(dd); hh; hh = overlapping_poly.Next(dd)) {
          poly_consistency_set.Insert(TypesAreConsistent(dd));
        }

        Set consistency_sets (Set(poly_consistency_set)
                                 .Insert(non_poly_consistency)
                                 .DUnion());

        if (non_poly.IsEmpty() || !non_poly_consistency.IsEmpty())
        {
          if (poly.IsEmpty() || !poly_consistency_set.InSet(Set()))
            return consistency_sets;
        /*
        Set l_res (TypesAreConsistent(binds.Rng()));
        if (!l_res.IsEmpty())
          return l_res;
        */
        }
        //------------------------------------------
        // Error message #45
        // L"%1" is multiple defined in super classes
        //------------------------------------------
        GenErr(nm, ERR, 45, mk_sequence(PrintName(nm)));
        return Nil();
      }
    }
    else
    {
      //------------------------------------------
      // Error message #45
      // L"%1" is multiple defined in super classes
      //------------------------------------------
      GenErr(nm, ERR, 45, mk_sequence(PrintName(nm)));
      return Nil();
    }
  }
}

// TypesAreConsistent
// p_s : (set of (REP`TypeRep | AccessType | (AS`Name * AccessFieldRep) | set of AccessType))
// -> set of AccessType
SET<TYPE_SSENV_AccessType> StatSem::TypesAreConsistent(const Set & p_s_)
{
  if (p_s_.IsEmpty())
    return SET<TYPE_SSENV_AccessType>();

  Set p_s (p_s_); // for safe
  Generic b_tp;
  for (bool bb = p_s.First(b_tp); bb; bb = p_s.Next(b_tp))
  {
    if (b_tp.IsRecord()) // REP`TypeRep | AccessType
    {
      if (!IsAccessType(b_tp)) // REP`TypeRep
        return SET<TYPE_SSENV_AccessType>();
    }
    else if(b_tp.IsTuple()) // (AS`Name * AccessFieldRep)
      return SET<TYPE_SSENV_AccessType>();
  }

  // p_s : set of (AccessType | set of AccessType)
  SET<TYPE_SSENV_AccessType> l_flatAccs;
  Generic b_acc;
  for(bool cc = p_s.First(b_acc); cc; cc = p_s.Next(b_acc))
  {
    if (IsAccessOpFnPolyTypeRep(b_acc))
      l_flatAccs.Insert(NormaliseAccessType(b_acc));
    else if (IsAccessTypeSet(b_acc))
      l_flatAccs.ImpUnion(NormaliseAccessTypeSet(b_acc));
    else
      return SET<TYPE_SSENV_AccessType>();
  }

  // l_flatAccs : set of (AccessOpTypeRep | AccessFnTypeRep | AccessPolyTypeRep)
  SET<TYPE_REP_TypeRep> l_retTps;
  SET<TYPE_SSENV_AccessType> l_retVal;
  SET<Sequence> l_paramTps;
  Generic g;
  for (bool dd = l_flatAccs.First(g); dd; dd = l_flatAccs.Next(g))
  {
    SEQ<TYPE_REP_TypeRep> b_tp1;
    TYPE_SSENV_AccessType b_tp (g);
    switch(b_tp.GetTag()) {
      case TAG_TYPE_SSENV_AccessOpTypeRep: {
        TYPE_REP_OpTypeRep otr (b_tp.GetRecord(pos_SSENV_AccessOpTypeRep_tp));
        b_tp1 = SEQ<TYPE_REP_TypeRep>(otr.get_Dom());
        l_retTps.Insert(otr.get_Rng());
        break;
      }
      case TAG_TYPE_SSENV_AccessPolyTypeRep: {
        TYPE_REP_PolyTypeRep ptr (b_tp.GetRecord(pos_SSENV_AccessPolyTypeRep_tp));
        b_tp1 = SEQ<TYPE_REP_TypeRep>(ptr.get_tp().GetSequence(1));
        l_retTps.Insert(ptr.get_tp().GetRecord(2));
        break;
      }
      case TAG_TYPE_SSENV_AccessFnTypeRep: {
        TYPE_REP_FnTypeRep ftr (b_tp.GetRecord(pos_SSENV_AccessFnTypeRep_tp));
        b_tp1 = SEQ<TYPE_REP_TypeRep>(ftr.GetSequence(1));
        l_retTps.Insert(ftr.GetRecord(2));
        break;
      }
      default:
        break;
    }
    l_retVal.Insert(b_tp);

    Generic h;
    for (bool ee = l_paramTps.First(h); ee; ee = l_paramTps.Next(h))
    {
      bool isOverlapping = true;
      SEQ<TYPE_REP_TypeRep> b_tp2 (h);
      if ((b_tp1 != b_tp2) && (b_tp1.Length() == b_tp2.Length()))
      {
        size_t len = b_tp1.Length();
        for (size_t i = 1; i <= len && isOverlapping; i++)
          isOverlapping = IsOverlapping(b_tp1[i], b_tp2[i], Set());

        if( isOverlapping )
          return SET<TYPE_SSENV_AccessType>();
      }
    }
    l_paramTps.Insert(b_tp1);
  }

// 20121016 -->
//  if( l_retTps.Card() > 1 )
//    return SET<TYPE_SSENV_AccessType>();
// <-- 20121016

  if( l_retVal.Card() != l_paramTps.Card() )
    return SET<TYPE_SSENV_AccessType>();

  return l_retVal;
}

// LookUpInHierarchyAux
// nm : AS`Name
// sup : AS`Name
// kind : (<VAL>|<TYPE>|<TAG>|<STATE>)
// where : (<OBJECT>|<LOCAL>|<GLOBAL>)
// checked : set of AS`Name
// return : map AS`Name to (REP`TypeRep | AccessType | (AS`Name * AccessFieldRep) |
//                          set of (AccessOpTypeRep | AccessFnTypeRep | AccessPolyTypeRep))
Map StatSem::LookUpInHierarchyAux (const TYPE_AS_Name & nm,
                                   const TYPE_AS_Name & sup,
                                   int kind,
                                   int where,
                                   const Set & checked)
{
  if (checked.InSet(sup))
    return Map();

  TYPE_SSENV_ParseTypeInfo classinfo (LookUpClass (sup));
//  if (ctr.Is(TAG_TYPE_SSENV_ParseTypeInfo))
//  {
//    TYPE_SSENV_ParseTypeInfo pti (ctr);
    TYPE_AS_Name qname (ExtName(sup, nm));
    Set ss (checked.Union(mk_set(sup)));

    switch(kind) {
      case VAL: {
        Set valMaps;
        valMaps.Insert(classinfo.GetMap(pos_SSENV_ParseTypeInfo_insts));
//
// TODO:
//        valMaps.Insert(classinfo.GetMap(pos_SSENV_ParseTypeInfo_fcts));
        valMaps.Insert(Settings.VDM10() ? ChangeToStatic(classinfo.GetMap(pos_SSENV_ParseTypeInfo_fcts))
                                           : classinfo.GetMap(pos_SSENV_ParseTypeInfo_fcts));
//        valMaps.Insert(classinfo.GetMap(pos_SSENV_ParseTypeInfo_polys));
        valMaps.Insert(Settings.VDM10() ? ChangeToStatic(classinfo.GetMap(pos_SSENV_ParseTypeInfo_polys))
                                           : classinfo.GetMap(pos_SSENV_ParseTypeInfo_polys));
//
        valMaps.Insert(classinfo.GetMap(pos_SSENV_ParseTypeInfo_ops));
        valMaps.Insert(classinfo.GetMap(pos_SSENV_ParseTypeInfo_overloaded));

        Map valBd (ExtractValue(valMaps, where == GLOBAL, qname, sup));

        Set valMaps2;
        valMaps2.Insert(classinfo.GetMap(pos_SSENV_ParseTypeInfo_vals));

        valBd.ImpOverride(ExtractValue(valMaps2, false, qname, sup));

        Set super (classinfo.GetSet(pos_SSENV_ParseTypeInfo_super));
        Map binds;
        Generic s;
        for (bool bb = super.First(s); bb; bb = super.Next(s))
          binds.ImpOverride (LookUpInHierarchyAux (nm, s, VAL, where, ss));

        return RemoveHidden(valBd, binds);
        break;
      }
      case TYPE: {
        Map binds;
        Generic rng;
        if (classinfo.GetMap(pos_SSENV_ParseTypeInfo_tps).DomExists (qname, rng))
          binds.Insert (sup, rng);
        else {
          Set super (classinfo.GetSet(pos_SSENV_ParseTypeInfo_super));
          Generic s;
          for (bool bb = super.First(s); bb; bb = super.Next(s))
            binds.ImpOverride (LookUpInHierarchyAux (nm, s, TYPE, where, ss));
        }
        return TransBinds (sup, binds);
        break;
      }
      case TAG: {
        Map binds;
        Generic rng;
        if (classinfo.GetMap(pos_SSENV_ParseTypeInfo_tags).DomExists (qname, rng)) {
          binds.Insert (sup, mk_(qname, rng));
        }
// 20120703 -->
        else if (classinfo.GetMap(pos_SSENV_ParseTypeInfo_tps).DomExists (qname, rng)) {
          TYPE_REP_TypeRep tp (StripAccessType(rng));
          if (tp.Is(TAG_TYPE_REP_TypeNameRep))
          {
            const TYPE_AS_Name & tag (tp.GetRecord(pos_REP_TypeNameRep_nm));
            TYPE_AS_Name ncls ((tag.GetSequence(pos_AS_Name_ids).Length() == 2)
                                 ? ASTAUX::GetFirstName(tag) : sup);
            TYPE_AS_Name nnm ((tag.GetSequence(pos_AS_Name_ids).Length() == 2)
                                 ? ASTAUX::GetSecondName(tag) : tag);
            return LookUpInHierarchyAux(nnm, ncls, kind, where, checked);
          }
          //binds.Insert (sup, mk_(qname, rng));
        }
// <-- 20120703
        else {
          Set super (classinfo.GetSet(pos_SSENV_ParseTypeInfo_super));
          Generic s;
          for (bool bb = super.First(s); bb; bb = super.Next(s))
            binds.ImpOverride (LookUpInHierarchyAux (nm, s, TAG, where, ss));
        }
        return TransBinds (sup, binds);
        break;
      }
      case STATE: {
        if (where != LOCAL)
          return Map();

        Map binds; // map As`name to ENV`AccessTypeRep
        Generic rng;
        if (classinfo.GetMap(pos_SSENV_ParseTypeInfo_insts).DomExists (qname, rng))
          binds.Insert (sup, rng);
        else {
          Set super (classinfo.GetSet(pos_SSENV_ParseTypeInfo_super));
          Generic s;
          for (bool bb = super.First(s); bb; bb = super.Next(s))
            binds.ImpOverride (LookUpInHierarchyAux (nm, s, STATE, where, ss));
        }
        return TransBinds (sup, binds);
        break;
      }
    } // end of case
//  }
  return Map ();
}

// ExtractValue
// p_defms : set of (map AS`Name to (AccessType | set of (AccessOpTypeRep | AccessFnTypeRep | AccessPolyTypeRep)))
// p_isGlobal : bool
// p_qname : AS`Name
// p_sup : AS`Name
// -> map AS`Name to (AccessType | set of (AccessOpTypeRep | AccessFnTypeRep | AccessPolyTypeRep))
Map StatSem::ExtractValue(const Set & p_defms_,
                               bool p_isGlobal,
                               const TYPE_AS_Name & p_qname,
                               const TYPE_AS_Name & p_sup)
{
  Set p_defms (p_defms_); // for safe
  Set l_matchMaps;
  Map l_match;

  Generic b_m; // map AS`Name to (AccessType | set of (AccessOpTypeRep | AccessFnTypeRep | AccessPolyTypeRep)))
  bool found = false;
  for (bool bb = p_defms.First(b_m); bb && !found; bb = p_defms.Next(b_m))
  {
    if (Map(b_m).DomExists(p_qname))
    {
      l_matchMaps.Insert(b_m);
// 20090413 -->
//      if (p_isGlobal)
      if (
#ifdef VICE
           this->staticrequired &&
#endif // VICE
          p_isGlobal)
// <-- 20090413
      {
        Map b_mMap (b_m);
        Generic b_m_p_qname (b_mMap[p_qname]);
        if (IsAccessTypeSet(b_m_p_qname)) // b_m_p_qname.IsSet()
        {
          Set b_m_p_qname_s (b_m_p_qname);
          Generic b_atr;
          bool exists = false;
          for (bool cc = b_m_p_qname_s.First(b_atr); cc && !exists; cc = b_m_p_qname_s.Next(b_atr))
          {
            if (GetSSStatic(b_atr))
              exists = true;
          }
          if (exists)
          {
            l_match = b_mMap;
            found = true;
          }
        }
        else {
          if (GetSSStatic(b_m_p_qname))
          {
            l_match = b_mMap;
            found = true;
          }
        }
      }
      else
      {
        l_match = Map(b_m);
        found = true;
      }
    }
  }

  if (!found)
    return Map();

  Generic resVal;
  if (IsAccessTypeSet(l_match[p_qname]))
  {
    Set resValSet;
    Set l_match_p_qname(l_match[p_qname]);
    Generic b_meth_g;
    for (bool dd = l_match_p_qname.First(b_meth_g); dd; dd = l_match_p_qname.Next(b_meth_g))
    {
      if ((
#ifdef VICE
           this->staticrequired &&
#endif // VICE
           p_isGlobal) && !GetSSStatic(b_meth_g))
        continue;

      TYPE_SSENV_AccessType b_meth (b_meth_g);
      b_meth.SetField(1, SubSuperClType(p_sup, StripAccessType(b_meth)));  
      resValSet.Insert(b_meth);
    }
    return Map().Insert(p_sup, resValSet);
  }
  else
  {
    TYPE_SSENV_AccessType b_meth (l_match[p_qname]);
// 20070301
    b_meth.SetField(1, SubSuperClType(p_sup, StripAccessType(b_meth)));  
    return Map().Insert(p_sup, b_meth);
  }
}

// RemoveHidden
// p_subMap : map AS`Name to (AccessType| set of (AccessOpTypeRep | AccessFnTypeRep | AccessPolyTypeRep))
// p_supMap : map AS`Name to (AccessType| set of (AccessOpTypeRep | AccessFnTypeRep | AccessPolyTypeRep))
// ==> map AS`Name to (AccessType| set of (AccessOpTypeRep | AccessFnTypeRep | AccessPolyTypeRep))
Map StatSem::RemoveHidden(const Map& p_subMap, const Map& p_supMap)
{
  Map l_subMap(p_subMap);  // local subclass map
  Map l_supMap(p_supMap);  // local superclass map

  Set rng_p_subMap (p_subMap.Rng());
  Set l_flatSubs;
  Generic b_atp;
  for (bool bb = rng_p_subMap.First(b_atp); bb; bb = rng_p_subMap.Next(b_atp))
  {
    if (IsAccessType(b_atp))
      l_flatSubs.Insert(b_atp);
    else // overloaded
      l_flatSubs.ImpUnion(Set(b_atp));
  }

  Set dom_p_supMap (p_supMap.Dom());
  Generic b_nm;
  for (bool cc = dom_p_supMap.First(b_nm); cc; cc = dom_p_supMap.Next(b_nm))
  {
    Generic b_bd (p_supMap[b_nm]);
    if (IsAccessType(b_bd))
    {
      Set l_subOccs;
      Generic b_atp;
      for (bool dd = l_flatSubs.First(b_atp); dd; dd = l_flatSubs.Next(b_atp))
      {
        if (IsSubType(StripAccessType(NormaliseAccessType(b_atp)), StripAccessType(NormaliseAccessType(b_bd)), Set()))
          l_subOccs.Insert(b_atp);
      }
      TYPE_AS_Access acc (GetSSAccess(b_bd)); // AS`Access
      if ((!(acc == Int(PUBLIC_AS) || acc == Int(PROTECTED_AS))) || !l_subOccs.IsEmpty())
        l_supMap.RemElem(b_nm);
    }
    else
    { // overloaded
      Set b_bd_s (b_bd);  // overloaded
      Set l_pubpro;
      Generic b_atp;
      for (bool dd = b_bd_s.First(b_atp); dd; dd = b_bd_s.Next(b_atp))
      {
        TYPE_AS_Access acc (GetSSAccess(b_atp)); // AS`Access
        if (acc == Int(PUBLIC_AS) || acc == Int(PROTECTED_AS))
          l_pubpro.Insert(b_atp);
      }
      if (l_pubpro.IsEmpty())
        l_supMap.RemElem(b_nm);
      else
        l_supMap.ImpModify(b_nm, l_pubpro);
    }
  }

  Set dom_p_subMap (p_subMap.Dom());
  Generic b_subNm;
  for (bool dd = dom_p_subMap.First(b_subNm); dd; dd = dom_p_subMap.Next(b_subNm))
  {
    Generic b_bd (p_subMap[b_subNm]);
    Set dom_l_supMap (l_supMap.Dom());
    Generic b_supNm;
    for (bool ee = dom_l_supMap.First(b_supNm); ee; ee = dom_l_supMap.Next(b_supNm))
    {
      Generic b_supBd (l_supMap[b_supNm]);
      //if (b_supBd.IsSet()) // set of (AccessOpTypeRep | AccessFnTypeRep | AccessPolyTypeRep))
      if (IsAccessTypeSet(b_supBd))
      { // overloaded
        Set b_supBd_s (b_supBd);
        Set l_matches;
        Generic b_atp2;
        for (bool ff = b_supBd_s.First(b_atp2); ff; ff = b_supBd_s.Next(b_atp2))
        {
          //if (b_bd.IsSet()) // set of (AccessOpTypeRep | AccessFnTypeRep | AccessPolyTypeRep))
          if (IsAccessTypeSet(b_bd))
          { // overloaded
            Set b_bd_s (b_bd);
            bool exists = false;
            Generic b_atp1;
            for (bool gg = b_bd_s.First(b_atp1); gg && !exists; gg = b_bd_s.Next(b_atp1))
            {
              exists = IsSubType(StripAccessType(NormaliseAccessType(b_atp1)),
                                 StripAccessType(NormaliseAccessType(b_atp2)), Set());
            }
            if (exists)
              l_matches.Insert(b_atp2);
          }
          else
          {
            if (IsSubType(StripAccessType(NormaliseAccessType(b_bd)),
                          StripAccessType(NormaliseAccessType(b_atp2)), Set()))
              l_matches.Insert(b_atp2);
          }
        }
        if (l_matches == b_supBd)
        {
          l_supMap.RemElem(b_supNm);
        }
        else
        {
          Set s (l_supMap[b_supNm]);
          s.ImpDiff(l_matches);
          l_supMap.ImpModify(b_supNm, s);
        }
      }
    } 
  }
  return l_subMap.ImpOverride(l_supMap);
}

// SubSuperClType
// clsnm : AS`Name
// typerep : REP`TypeRep
// ==> REP`TypeRep
TYPE_REP_TypeRep StatSem::SubSuperClType(const TYPE_AS_Name & clsnm, const TYPE_REP_TypeRep & typerep)
{
  switch (typerep.GetTag()) {
    case TAG_TYPE_REP_NilTypeRep:
    case TAG_TYPE_REP_BooleanTypeRep:
    case TAG_TYPE_REP_NumericTypeRep:
    case TAG_TYPE_REP_TokenTypeRep:
    case TAG_TYPE_REP_CharTypeRep:
    case TAG_TYPE_REP_TypeVarRep:
    case TAG_TYPE_REP_CompositeTypeRep:
    case TAG_TYPE_REP_QuoteTypeRep:
      return typerep;
    case TAG_TYPE_REP_TypeNameRep: {
      const TYPE_AS_Name & nm (typerep.GetRecord(pos_REP_TypeNameRep_nm));
      if(nm.GetSequence(pos_AS_Name_ids).Length() == 1)
      {
// 20141202 -->
//        if (IsClassName(nm))
//          return mk_REP_ObjRefTypeRep(nm);
// <-- 20141202
        Generic newnm (LookUpTypeInSuper(nm, clsnm));
        if( !newnm.IsNil() )
        {
          return mk_REP_TypeNameRep(newnm);
        }
      }
// 20120226 -->
      else
      {
        TYPE_AS_Name ncls (ASTAUX::GetFirstName(nm));
        TYPE_AS_Name nnm (ASTAUX::GetSecondName(nm));
        Generic gpti (GetClassTypeRep(ncls));
        if (!gpti.IsNil())
        {
          Set dom_tps (Record(gpti).GetMap(pos_SSENV_ParseTypeInfo_tps).Dom());
          if (dom_tps.InSet(nm))
            return typerep;
        }
        if (IsClassName(nnm))
          return mk_REP_ObjRefTypeRep(nnm);
      }
// <-- 20120226
      return typerep;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (typerep.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> new_tp_s;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb; bb = tp_s.Next(tp))
        new_tp_s.Insert(SubSuperClType(clsnm, tp));
      return mk_REP_UnionTypeRep(new_tp_s);
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & tp_l (typerep.GetSequence(pos_REP_ProductTypeRep_tps));
      SEQ<TYPE_REP_TypeRep> new_tp_l;
      size_t len_tp_l = tp_l.Length();
      for (size_t idx = 1; idx <= len_tp_l; idx++)
        new_tp_l.ImpAppend(SubSuperClType(clsnm, tp_l[idx]));
      return mk_REP_ProductTypeRep(new_tp_l);
    }
    case TAG_TYPE_REP_SetTypeRep: {
      return mk_REP_SetTypeRep(SubSuperClType(clsnm, typerep.GetRecord(pos_REP_SetTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_EmptySetTypeRep: {
      return mk_REP_EmptySetTypeRep(SubSuperClType(clsnm, typerep.GetRecord(pos_REP_EmptySetTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return mk_REP_SeqTypeRep(SubSuperClType(clsnm, typerep.GetRecord(pos_REP_SeqTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_EmptySeqTypeRep: {
      return mk_REP_EmptySeqTypeRep(SubSuperClType(clsnm, typerep.GetRecord(pos_REP_EmptySeqTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return mk_REP_GeneralMapTypeRep(SubSuperClType(clsnm, typerep.GetRecord(pos_REP_GeneralMapTypeRep_mapdom)),
                                      SubSuperClType(clsnm, typerep.GetRecord(pos_REP_GeneralMapTypeRep_maprng)));
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return mk_REP_InjectiveMapTypeRep(SubSuperClType(clsnm, typerep.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom)),
                                        SubSuperClType(clsnm, typerep.GetRecord(pos_REP_InjectiveMapTypeRep_maprng)));
    }
    case TAG_TYPE_REP_EmptyMapTypeRep: {
      return mk_REP_EmptyMapTypeRep(SubSuperClType(clsnm, typerep.GetRecord(pos_REP_EmptyMapTypeRep_mapdom)),
                                    SubSuperClType(clsnm, typerep.GetRecord(pos_REP_EmptyMapTypeRep_maprng)));
    }
    case TAG_TYPE_REP_OpTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & dtpl (typerep.GetSequence(pos_REP_OpTypeRep_Dom));
      SEQ<TYPE_REP_TypeRep> new_dtpl;
      size_t len_dtpl = dtpl.Length();
      for (size_t idx = 1; idx <= len_dtpl; idx++)
        new_dtpl.ImpAppend(SubSuperClType(clsnm, dtpl[idx]));
      return mk_REP_OpTypeRep(new_dtpl, SubSuperClType(clsnm, typerep.GetRecord(pos_REP_OpTypeRep_Rng)));
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & dtpl (typerep.GetSequence(pos_REP_PartialFnTypeRep_fndom));
      SEQ<TYPE_REP_TypeRep> new_dtpl;
      size_t len_dtpl = dtpl.Length();
      for (size_t idx = 1; idx <= len_dtpl; idx++)
        new_dtpl.ImpAppend(SubSuperClType(clsnm, dtpl[idx]));
      return mk_REP_PartialFnTypeRep(new_dtpl,
                                     SubSuperClType(clsnm, typerep.GetRecord(pos_REP_PartialFnTypeRep_fnrng)));
    }
    case TAG_TYPE_REP_TotalFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & dtpl (typerep.GetSequence(pos_REP_TotalFnTypeRep_fndom));
      SEQ<TYPE_REP_TypeRep> new_dtpl;
      size_t len_dtpl = dtpl.Length();
      for (size_t idx = 1; idx <= len_dtpl; idx++)
        new_dtpl.ImpAppend(SubSuperClType(clsnm, dtpl[idx]));
      return mk_REP_TotalFnTypeRep(new_dtpl, SubSuperClType(clsnm, typerep.GetRecord(pos_REP_TotalFnTypeRep_fnrng)));
    }
    case TAG_TYPE_REP_InvTypeRep: {
      const TYPE_REP_TypeRep & shape (typerep.GetRecord(pos_REP_InvTypeRep_shape));
      return mk_REP_InvTypeRep(typerep.GetRecord(pos_REP_InvTypeRep_name),
                               SubSuperClType(clsnm, shape),
                               typerep.GetField(pos_REP_InvTypeRep_invariant));
    }
    default:
      return typerep;
  }
}

// LookUpTypeInSuper
// tpnm : AS`Name
// clsnm : AS`Name
// ==> [AS`Name]
Generic StatSem::LookUpTypeInSuper(const TYPE_AS_Name & tpnm_, const TYPE_AS_Name & clsnm_)
{
  if (IsClassName(tpnm_))
    return Nil();
// 20141202 -->
  TYPE_AS_Name tpnm ((tpnm_.get_ids().Length() == 1) ? tpnm_ : ASTAUX::GetSecondName(tpnm_));
  TYPE_AS_Name clsnm ((tpnm_.get_ids().Length() == 1) ? clsnm_ : ASTAUX::GetFirstName(tpnm_));
// <-- 20141202
  TYPE_SSENV_ParseTypeInfo classinfo (LookUpClass(clsnm));
// 20141202 -->
  TYPE_AS_Name extnm (ExtName(clsnm, tpnm));
//  TYPE_AS_Name extnm ((tpnm.get_ids().Length() == 1) ? ExtName(clsnm, tpnm) : tpnm); // 20080826
// <-- 20141202
  if(classinfo.get_tps().DomExists(extnm)) {
// 20141202 -->
    const Map & tps (classinfo.GetMap(pos_SSENV_ParseTypeInfo_tps));
     
    if (CheckAccessCurClass (tps[extnm]).IsNil())
      return Nil();
// <-- 20141202
    return extnm;
  }
  else
  {
    SET<TYPE_AS_Name> supers (classinfo.get_super());
    Generic sup;
    for (bool bb = supers.First(sup); bb; bb = supers.Next(sup))
    {
      Generic nm (LookUpTypeInSuper(tpnm, sup));
      if (!nm.IsNil())
        return nm;
    }
    return Nil();
  }
}
#endif // VDMPP

#ifdef VDMSL
// LookUpPolyFn
// nm : AS`Name
// ==> [(seq of REP`TypeVarRep) * REP`FnTypeRep]
Generic StatSem::LookUpPolyFn (const TYPE_AS_Name & nm)
{
  size_t depth = this->SEnv.Length();
  for (size_t i = 1 ; i <= depth ; i++)
  {
    Map currentEnv (this->SEnv[i]); // map AS`Name to TypeRepElem
    if (currentEnv.DomExists (nm))
    {
      TYPE_SSENV_TypeRepElem rc (currentEnv[nm]);
      TYPE_REP_TypeRep tp (rc.GetRecord(pos_SSENV_TypeRepElem_tp));
      if (tp.Is(TAG_TYPE_REP_PolyTypeRep))
      {
        rc.SetField(pos_SSENV_TypeRepElem_used, Bool(true));
        currentEnv.ImpModify (nm, rc);
        this->SEnv.ImpModify (i, currentEnv);
        return mk_(tp.GetSequence(pos_REP_PolyTypeRep_vars), tp.GetRecord(pos_REP_PolyTypeRep_tp));
      }
    }
  }

  if (this->PolyEnv.DomExists(nm))
  {
    TYPE_SSENV_PolyTypeRepElem rc (this->PolyEnv[nm]);
    rc.SetField(pos_SSENV_PolyTypeRepElem_used, Bool(true));
    this->PolyEnv.ImpModify (nm, rc);
    return mk_(rc.GetSequence(pos_SSENV_PolyTypeRepElem_poly), rc.GetRecord(pos_SSENV_PolyTypeRepElem_tp));
  }
  else {
    return Nil ();
  }
}
#endif // VDMSL

// CheckTypeVar
// nm : REP`TypeVarRep
// ==> bool
bool StatSem::CheckTypeVar (const TYPE_REP_TypeVarRep & nm)
{
  bool exists = false;
  size_t len_TypeVars = this->TypeVars.Length();
  for (size_t i = 1; (i <= len_TypeVars) && !exists; i++) {
    exists = Set(this->TypeVars[i]).InSet(nm);
  }
  return exists;
}

// CheckTag
// nm : AS`Name
// return : bool
bool StatSem::CheckTag (const TYPE_AS_Name & nm)
{
  if (this->TagEnv.DomExists (nm))
    return true;
  else
#ifdef VDMPP
    return (!LookUpInHierarchy (nm, Nil (), TAG, GLOBAL).IsNil ());
#endif //VDMPP
#ifdef VDMSL
  return (this->LocTagEnv.DomExists(nm));
#endif //VDMSL
}

// CheckLocalTag
// nm : AS`Name
// ==> bool
bool StatSem::CheckLocalTag (const TYPE_AS_Name & nm)
{
  return (this->TagEnv.DomExists(nm));
}

#ifdef VDMSL
// CheckImportedTag
// nm : AS`Name
// ==> bool
bool StatSem::CheckImportedTag(const TYPE_AS_Name & nm)
{
  return (this->LocTagEnv.DomExists(nm));
}
#endif //VDMSL

// CheckTypeName
// nm : AS`Name
// ==> bool
bool StatSem::CheckTypeName (const TYPE_AS_Name & nm)
{
#ifdef VDMPP
  if (nm.GetSequence(pos_AS_Name_ids).Length() == 2)
  {
    TYPE_AS_Name ncls (ASTAUX::GetFirstName(nm));
    TYPE_AS_Name nnm (ASTAUX::GetSecondName(nm));

    if (ncls == GetCurClass ())
      return CheckTypeName (nnm);
    else {
      Generic looktp (LookUpInHierarchy (nnm, ncls, TYPE, GLOBAL));
      if (!looktp.IsNil ()) {
        Generic acs (CheckAccessCurClass (looktp));
        if (acs.IsNil ()) {
          //--------------------
          // Error message #368
          // Access violation
          // -------------------
          GenErr (nm, ERR, 368, mk_sequence(PrintName (nm)));
          return false;
        }
        else
          return true;
      }
      else
        return true;
    }
  }
#endif //VDMPP

  if( this->TypeEnv.DomExists (nm) )
  {
    TYPE_SSENV_TypeRepElem tpre (this->TypeEnv[nm]);
    TYPE_REP_TypeRep tp (tpre.get_tp());
    if( !tp.Is(TAG_TYPE_REP_AllTypeRep) )
    {
      tpre.SetField(pos_SSENV_TypeRepElem_used, Bool(true));
      this->TypeEnv.ImpModify(nm, tpre);
      return true;
    }
  }
#ifdef VDMPP
  else // if(nm.GetSequence(pos_AS_Name_ids).Length() == 1)
  {
    Generic looktp (LookUpInHierarchy (nm, Nil(), TYPE, GLOBAL));
    if (!looktp.IsNil ())
    {
      Generic acs (CheckAccessCurClass (looktp));
      if (acs.IsNil ())
      {
        //--------------------
        // Error message #368
        // Access violation
        // -------------------
        GenErr (nm, ERR, 368, mk_sequence(PrintName (nm)));
        return false;
      }
      else
        return true;
    }
    else
      return CheckClassName(nm);
  }
#endif //VDMPP
#ifdef VDMSL
//  else if (this->TypeParEnv.DomExists (nm))
//    return true;
  else if (this->RenamedTypes.DomExists (nm))
    return CheckTypeName (this->RenamedTypes [nm]);
  else
    return false;
#endif //VDMSL
  return false; // dummy
}

// CheckOperationName
// nm : AS`Name
// ==> bool
bool StatSem::CheckOperationName (const TYPE_AS_Name& nm)
{
  if (this->OperationEnv.DomExists (nm))
  {
    TYPE_SSENV_OpTypeRepElem tre (this->OperationEnv[nm]);
    tre.SetField(pos_SSENV_TypeRepElem_used, Bool(true));
    this->OperationEnv.ImpModify (nm, tre);
    return true;
  }
  else
#ifdef VDMSL
    return false;
#endif // VDMSL
#ifdef VDMPP
    return ! LookUpOperationName(Nil(), nm, Nil()).IsEmpty ();
#endif // VDMPP
}

#ifdef VDMSL
// SetCurMod
// modnm : AS`Name
// ==> ()
void StatSem::SetCurMod (const TYPE_AS_Name& modnm)
{
  this->CheckModule = modnm;
}

// GetCurMod
// ==> AS`Name
TYPE_AS_Name StatSem::GetCurMod () const
{
  return this->CheckModule;
}
#endif // VDMSL

// GetCurModOrNil
// ==> [AS`Name]
Generic StatSem::GetCurModOrNil () const
{
#ifdef VDMSL
  return this->CheckModule;
#endif // VDMSL
#ifdef VDMPP
  return Nil();
#endif // VDMPP
}

#ifdef VDMPP
// InstallCurClass
// classnm : AS`Name
// super : seq of AS`Name
// ==> bool
bool StatSem::InstallCurClass (const TYPE_AS_Name & classnm, const SEQ<TYPE_AS_Name> & super)
{
  this->CheckClass = classnm;

  if (!CheckClassName(classnm))
    return false;

  this->Super = super.Elems ();
  this->SuperSuper = TransClos (classnm);
  return true;
}

// SetCurClass
// cls : AS`Name
// ==> ()
void StatSem::SetCurClass (const TYPE_AS_Name& cls)
{
  this->CheckClass = cls;
}

// GetCurClass
// ==> AS`Name
TYPE_AS_Name StatSem::GetCurClass () const
{
  return this->CheckClass;
}

// IsAccessible
// cls_q : AS`Name
// acs : AccessType
// ==> bool
bool StatSem::IsAccessible ( const TYPE_AS_Name & cls_q, const TYPE_SSENV_AccessType & acs)
{
  TYPE_AS_Name cls (ASTAUX::GetFirstName(cls_q));

  Int acsa (acs.GetInt (2));
  switch (acsa.GetValue ()) {
    case PUBLIC_AS: {
      return true;
    }
    case PROTECTED_AS: {
      return cls == GetCurClass () ||
             this->Super.InSet (cls) ||
             this->SuperSuper.InSet (cls);
    }
    case PRIVATE_AS:
    case NOT_INITIALISED_AS: {
// 20070310
//      return cls == this->FoundClass;
      return cls == GetCurClass ();
    }
    default: {
      InternalError (L"IsAccessible");
      return false;
    }
  }
}

// IsAccessibleCurClass
// acs : AccessType
// ==> bool
bool StatSem::IsAccessibleCurClass (const TYPE_SSENV_AccessType & acs)
{
  return IsAccessible (GetCurClass (), acs);
}

//// GenAccessError
//// a : AS`Name
////==> [bool]
//Generic StatSem::GenAccessError ( const TYPE_AS_Name & a)
//{
//  GenErr(a,ERR,368,mk_sequence(PrintName (a)));
//  return Nil ();
//}

// CheckAccess
// cls : AS`Name
// tp : [AccessType|REP`TypeRep|TagRepElem|
//      set of (AccessOpTypeRep | AccessFnTypeRep | AccessPolyTypeRep) |
//      set of REP`TypeRep ]
// -> [REP`TypeRep | TagRepElem | set of (REP`FnTypeRep | REP`OpTypeRep  | REP`PolyTypeRep)];
Generic StatSem::CheckAccess (const TYPE_AS_Name & cls, const Generic & tp)
{
  if (IsAccessTypeSet(tp))
  {
    SET<TYPE_SSENV_AccessType> tps (tp);
    Generic l_defcl (LookUpDefClass());
    TYPE_AS_Name l_actcl (l_defcl.IsNil() ? cls : TYPE_AS_Name(l_defcl));

    Generic b_atp;
    SET<TYPE_SSENV_AccessType> l_tps;
    for (bool bb = tps.First(b_atp); bb; bb = tps.Next(b_atp))
    {
      if (IsAccessible(l_actcl, b_atp))
        l_tps.Insert(StripAccessType(b_atp));
    }

    if (l_tps.IsEmpty())
      return Nil(); // GenAccessError(cls);
    else
      return l_tps;
  }
  else if (IsAccessType (tp))
  {
    Generic defcl (LookUpDefClass ());

    // 20090930-->
    // check cls is class or not
    Generic gtp = Nil();
    if (cls != GetCurClass ())
    {
      SetDefClass(GetCurClass ());
      gtp = LookUp(cls, false);
      SetDefClass(Nil());

      if (IsAccessType(gtp))
        CheckLookupStatic(cls, GetSSStatic(gtp));
    // <--20090930

    }
// 20090413 -->
//    if ((!defcl.IsNil () && IsAccessible (defcl, tp)) ||
    if (
#ifdef VICE
        !this->staticrequired ||
#endif // VICE
//        (!defcl.IsNil () && IsAccessible (defcl, tp) && (!IsClassName(cls) || IsAccessible (cls, tp))) ||
        (!defcl.IsNil () && IsAccessible (defcl, tp) && (!gtp.IsNil() || IsAccessible (cls, tp))) ||
// <-- 20090413
        (defcl.IsNil () && IsAccessible (cls, tp)))
      return StripAccessType(tp);
    else
      return Nil(); //GenAccessError (cls);
  }
  else
    return tp;
}

//// ConvertAccessMethToRep
//// p_meth : (REP`OpTypeRep | REP`FnTypeRep | REP`PolyTypeRep)
//// -> REP`FnTypeRep
//TYPE_REP_TotalFnTypeRep StatSem::ConvertAccessMethToRep(
//                                              const TYPE_REP_TypeRep & p_meth)
//{
//  if (p_meth.Is(TAG_TYPE_REP_OpTypeRep))
//    return mk_REP_TotalFnTypeRep(p_meth.GetField(1),
//                                 p_meth.GetField(2));
//  else
//    return (TYPE_REP_TotalFnTypeRep) p_meth;
//}

// CheckAccessCurClass
// tp : [ AccessType  | REP`TypeRep | TagRepElem  |
//      set of (AccessOpTypeRep | AccessFnTypeRep | AccessPolyTypeRep) |
//      set of REP`TypeRep]
// -> [ REP`TypeRep | TagRepElem  |
//    set of (REP`FnTypeRep | REP`OpTypeRep | REP`PolyTypeRep) ]
Generic StatSem::CheckAccessCurClass (const Generic & tp)
{
  Generic defcl (LookUpDefClass ());
  return (defcl.IsNil () ? CheckAccess (GetCurClass (), tp) : CheckAccess (defcl, tp));
}

// IsAccessType
// a : [AccessType|REP`TypeRep|TagRepElem|
//      set of (AccessOpTypeRep | AccessFnTypeRep | AccessPolyTypeRep)|
//      set of (REP`OpTypeRep | REP`FnTypeRep)]
// -> bool
bool StatSem::IsAccessType (const Generic & a) const
{
  if (a.IsRecord ())
  {
    switch(Record(a).GetTag()) {
      case TAG_TYPE_SSENV_AccessTypeRep:
      case TAG_TYPE_SSENV_AccessFnTypeRep:
      case TAG_TYPE_SSENV_AccessPolyTypeRep:
      case TAG_TYPE_SSENV_AccessOpTypeRep:
      case TAG_TYPE_SSENV_AccessFieldRep:
        return true;
      default:
        return false;
    }
  }
  else
    return false;
}

// IsSetOfAccessType (not in spec)
// a : [AccessType|REP`TypeRep|TagRepElem| set of (REP`OpTypeRep |
//     REP`FnTypeRep | REP`PolyTypeRep | AccessOpTypeRep |
//     AccessFnTypeRep | AccessPolyTypeRep)]
// -> bool
bool StatSem::IsSetOfAccessType (const Generic & a) const
{
  if (a.IsSet())
  {
    Set tp_s (a);
    bool forall = true;
    Generic tp;
    for (bool bb = tp_s.First(tp); bb && forall; bb = tp_s.Next(tp))
      forall = IsAccessType(tp);
    return forall;
  }
  else
    return false;
}

// IsAccessTypeSet
// a : [AccessType|REP`TypeRep|TagRepElem| set of (REP`OpTypeRep |
//     REP`FnTypeRep | REP`PolyTypeRep | AccessOpTypeRep |
//     AccessFnTypeRep | AccessPolyTypeRep)]
// -> bool
bool StatSem::IsAccessTypeSet(const Generic & a) const
{
  if (a.IsSet())
  {
    Set tp_s (a);
    bool forall = true;
    Generic tp;
    for (bool bb = tp_s.First(tp); bb && forall; bb = tp_s.Next(tp))
      forall = IsAccessOpFnPolyTypeRep(tp);
    return forall;
  }
  else
    return false;
}

// GetSSStatic (Not in spec)
// a : [AccessType|REP`TypeRep|TagRepElem| set of (REP`OpTypeRep |
//     REP`FnTypeRep | REP`PolyTypeRep | AccessOpTypeRep |
//     AccessFnTypeRep | AccessPolyTypeRep)]
// -> bool
Bool StatSem::GetSSStatic(const Generic & a) const
{
  if (a.IsRecord()) {
    TYPE_SSENV_AccessType tp (a);
    switch(tp.GetTag()) {
      case TAG_TYPE_SSENV_AccessTypeRep:
        return tp.GetBool(pos_SSENV_AccessTypeRep_stat);
      case TAG_TYPE_SSENV_AccessFnTypeRep:
        return tp.GetBool(pos_SSENV_AccessFnTypeRep_stat);
      case TAG_TYPE_SSENV_AccessPolyTypeRep:
        return tp.GetBool(pos_SSENV_AccessPolyTypeRep_stat);
      case TAG_TYPE_SSENV_AccessOpTypeRep:
        return tp.GetBool(pos_SSENV_AccessOpTypeRep_stat);
    }
  }
  return Bool(false);
}

// GetSSAccess (Not in spec)
// a : [AccessType|REP`TypeRep|TagRepElem| set of (REP`OpTypeRep |
//     REP`FnTypeRep | REP`PolyTypeRep | AccessOpTypeRep |
//     AccessFnTypeRep | AccessPolyTypeRep)]
// -> AS`Access
TYPE_AS_Access StatSem::GetSSAccess(const Generic & a) const
{
  if (a.IsRecord()) {
    TYPE_SSENV_AccessType tp (a);
    switch(tp.GetTag()) {
      case TAG_TYPE_SSENV_AccessTypeRep:
        return tp.GetField(pos_SSENV_AccessTypeRep_a);
      case TAG_TYPE_SSENV_AccessFnTypeRep:
        return tp.GetField(pos_SSENV_AccessFnTypeRep_a);
      case TAG_TYPE_SSENV_AccessPolyTypeRep:
        return tp.GetField(pos_SSENV_AccessPolyTypeRep_a);
      case TAG_TYPE_SSENV_AccessOpTypeRep:
        return tp.GetField(pos_SSENV_AccessOpTypeRep_a);
      case TAG_TYPE_SSENV_AccessFieldRep:
        return tp.GetField(pos_SSENV_AccessFieldRep_a);
    }
  }
  return Nil();
}

// IsAccessOpFnPolyTypeRep (Not in spec)
// a : [AccessType|REP`TypeRep|TagRepElem| set of (REP`OpTypeRep |
//     REP`FnTypeRep | REP`PolyTypeRep | AccessOpTypeRep |
//     AccessFnTypeRep | AccessPolyTypeRep)]
// -> bool
bool StatSem::IsAccessOpFnPolyTypeRep(const Generic & a) const
{
  if (a.IsRecord ())
  {
    switch(Record(a).GetTag()) {
      case TAG_TYPE_SSENV_AccessFnTypeRep:
      case TAG_TYPE_SSENV_AccessPolyTypeRep:
      case TAG_TYPE_SSENV_AccessOpTypeRep:
        return true;
      default:
        return false;
    }
  }
  else
    return false;
}

// IsOpFnTypeRep (Not in spec)
// a : [AccessType|REP`TypeRep|TagRepElem| set of (REP`OpTypeRep |
//     REP`FnTypeRep | REP`PolyTypeRep | AccessOpTypeRep |
//     AccessFnTypeRep | AccessPolyTypeRep)]
// -> bool
bool StatSem::IsOpFnTypeRep(const Generic & a) const
{
  if (a.IsRecord ())
  {
    switch(Record(a).GetTag()) {
      case TAG_TYPE_REP_OpTypeRep:
      case TAG_TYPE_REP_PartialFnTypeRep:
      case TAG_TYPE_REP_TotalFnTypeRep:
        return true;
      default:
        return false;
    }
  }
  else
    return false;
}

// IsOpFnTypeRepSet (Not in spec)
// a : [AccessType|REP`TypeRep|TagRepElem| set of (REP`OpTypeRep |
//     REP`FnTypeRep | REP`PolyTypeRep | AccessOpTypeRep |
//     AccessFnTypeRep | AccessPolyTypeRep)]
// -> bool
bool StatSem::IsOpFnTypeRepSet(const Generic & a) const
{
  if (a.IsSet())
  {
    Set tp_s (a);
    bool forall = true;
    Generic tp;
    for (bool bb = tp_s.First(tp); bb && forall; bb = tp_s.Next(tp))
      forall = IsOpFnTypeRep(tp);
    return forall;
  }
  else
    return false;
}

// IsTypeRep (Not in spec)
// a : 
// -> bool
bool StatSem::IsTypeRep(const Generic & a) const
{
  if (a.IsRecord ())
  {
    switch(Record(a).GetTag()) {
      case TAG_TYPE_REP_InvTypeRep:
      case TAG_TYPE_REP_BooleanTypeRep:
      case TAG_TYPE_REP_NumericTypeRep:
      case TAG_TYPE_REP_TokenTypeRep:
      case TAG_TYPE_REP_CharTypeRep:
      case TAG_TYPE_REP_QuoteTypeRep:
      case TAG_TYPE_REP_CompositeTypeRep:
      case TAG_TYPE_REP_UnionTypeRep:
      case TAG_TYPE_REP_ProductTypeRep:
      case TAG_TYPE_REP_SetTypeRep:
      case TAG_TYPE_REP_SeqTypeRep:
      case TAG_TYPE_REP_GeneralMapTypeRep:
      case TAG_TYPE_REP_InjectiveMapTypeRep:
      case TAG_TYPE_REP_TypeVarRep:
      case TAG_TYPE_REP_TypeParRep:
      case TAG_TYPE_REP_TypeNameRep:
      case TAG_TYPE_REP_PartialFnTypeRep:
      case TAG_TYPE_REP_TotalFnTypeRep:
      case TAG_TYPE_REP_ObjRefTypeRep:
#ifdef VICE
      case TAG_TYPE_REP_AnyOpTypeRep:
#endif // VICE
      case TAG_TYPE_REP_OpTypeRep:
      case TAG_TYPE_REP_ExitTypeRep:
      case TAG_TYPE_REP_PolyTypeRep:
      case TAG_TYPE_REP_AllTypeRep:
      case TAG_TYPE_REP_TmpTypeRep:
      case TAG_TYPE_REP_UnitTypeRep:
      case TAG_TYPE_REP_RetTypeRep:
      case TAG_TYPE_REP_NilTypeRep:
      case TAG_TYPE_REP_EmptySetTypeRep:
      case TAG_TYPE_REP_EmptySeqTypeRep:
      case TAG_TYPE_REP_EmptyMapTypeRep:
        return true;
      default: 
        return false;
    }
  }
  else
    return false;
}

// StripAccessType
// tp : [AccessType|REP`TypeRep|TagRepElem]
// -> [REP`TypeRep|seq of REP`FieldRep]
Generic StatSem::StripAccessType (const Generic & tp)
{
  if (IsAccessType (tp))
  {
    TYPE_SSENV_AccessType tpr (tp);
    switch(tpr.GetTag()) {
      case TAG_TYPE_SSENV_AccessTypeRep:
        return tpr.GetField(pos_SSENV_AccessTypeRep_tp);
      case TAG_TYPE_SSENV_AccessFnTypeRep:
        return tpr.GetField(pos_SSENV_AccessFnTypeRep_tp);
      case TAG_TYPE_SSENV_AccessPolyTypeRep:
        return tpr.GetField(pos_SSENV_AccessPolyTypeRep_tp);
      case TAG_TYPE_SSENV_AccessOpTypeRep:
        return tpr.GetField(pos_SSENV_AccessOpTypeRep_tp);
      case TAG_TYPE_SSENV_AccessFieldRep:
        return tpr.GetField(pos_SSENV_AccessFieldRep_tp);
      default:
        return TYPE_REP_TypeRep(); // dummy
    }
  }
  else
    return tp;
}

// CheckInstAccess
// p_acc : AS`Access
// p_nm : AS`Name
// ==> bool
bool StatSem::CheckInstAccess(const TYPE_AS_Access & p_acc, const TYPE_AS_Name & p_nm)
{
  bool l_res = true;
  Set ss (this->SuperSuper);
  Generic b_clnm;
  for (bool bb = ss.First(b_clnm); bb; bb = ss.Next(b_clnm))
  {
    Generic l_acs (LookUpInObject(b_clnm, p_nm, true, true));
    if (IsAccessType(l_acs) && !NotNarrowedAccess(GetSSAccess(l_acs), p_acc))
    {
      //--------------------------------------
      //-- Error 369
      //-- Scope narrowed by L"%1"
      //-------------------------------------
      GenErr(p_nm, ERR, 369, mk_sequence(PrintName(p_nm)));
      l_res = false;
    }
  }
  return l_res;
}

// CheckTypeDefAccess
// p_acc : AS`Access
// p_nm : AS`Name
// ==> bool
bool StatSem::CheckTypeDefAccess(const TYPE_AS_Access & p_acc, const TYPE_AS_Name & p_nm)
{
  bool l_res = true;
  Set ss (this->SuperSuper);
  Generic b_clnm;
  for (bool bb = ss.First(b_clnm); bb; bb = ss.Next(b_clnm))
  {
// 20100802 -->
// for cid 
    if (CheckClassName(b_clnm))
    {
// <-- 20100802
      Generic tp_acs (LookUpTypeName(ExtName(b_clnm, p_nm), false));
      if (IsAccessType(tp_acs) && !NotNarrowedAccess(GetSSAccess(tp_acs), p_acc))
      {
        //--------------------------------------
        //-- Error 369
        //-- Scope narrowed by L"%1"
        //-------------------------------------
        GenErr(p_nm, ERR, 369, mk_sequence(PrintName(p_nm)));
        l_res = false;
      }
    } // 20100802
  }
  return l_res;
}

// CheckDefAccess
// p_acc : AS`Access
// p_nm : AS`Name
// ==> bool
bool StatSem::CheckDefAccess(const TYPE_SSENV_AccessType & p_act, const TYPE_AS_Name & p_nm)
{
  bool l_res = true;
  
  TYPE_AS_Access p_acc (GetSSAccess(p_act));
 
  Set ss (this->SuperSuper);
  Generic b_clnm;
  for (bool bb = ss.First(b_clnm); bb; bb = ss.Next(b_clnm))
  {
    if (CheckClassName(b_clnm))
    {
      Generic tp_acs (LookUp(ExtName(b_clnm, p_nm), false));
      Set tp_acs_s;
      if (IsAccessType(tp_acs))
        tp_acs_s.Insert(tp_acs);
      else if (IsAccessTypeSet(tp_acs))
        tp_acs_s.ImpUnion(tp_acs);
      if (!tp_acs_s.IsEmpty()) {
        bool forallScopeOk = true; 
        bool forallPureOk = true; 
        Generic acs;
        for (bool bb = tp_acs_s.First(acs); bb; bb = tp_acs_s.Next(acs))
        {
          if (IsCompatible(POS, StripAccessType(p_act), StripAccessType(acs))) {
            forallScopeOk = NotNarrowedAccess(GetSSAccess(acs), p_acc) && forallScopeOk;
            if (p_act.Is(TAG_TYPE_SSENV_AccessOpTypeRep)) {
              if (!p_act.GetBoolValue(pos_SSENV_AccessOpTypeRep_oppure)) {
                forallPureOk = !Record(acs).GetBoolValue(pos_SSENV_AccessOpTypeRep_oppure) && forallPureOk;
              }
            }
          }
        }
  
        if (!forallScopeOk)
        {
          //--------------------------------------
          //-- Error 369
          //-- Scope narrowed by L"%1"
          //-------------------------------------
          GenErr(p_nm, ERR, 369, mk_sequence(PrintName(p_nm)));
          l_res = false;
        }
        if (!forallPureOk)
        {
          //--------------------------------------
          //-- Error 461
          //-- Impure operation "%1" override pure operation
          //-------------------------------------
          GenErr(p_nm, ERR, 461, mk_sequence(PrintName(p_nm)));
          l_res = false;
        }
      }
    }
  }
  return l_res;
}

// CheckPatternBindAccess
// acc : AS`Access
// bd : map AS`Name to (REP`TypeRep * nat1)
// ==> bool
bool StatSem::CheckPatternBindAccess (const TYPE_AS_Access & acc, const MAP<TYPE_AS_Name,Tuple> & bd)
{
  TYPE_AS_Name curcls (GetCurClass ());
  bool res (true);
  SET<TYPE_AS_Name> dom_bd (bd.Dom());
  Generic nm;
  for (bool bb = dom_bd.First(nm); bb; bb = dom_bd.Next (nm))
  {
    TYPE_REP_TypeRep tp (bd[nm].GetRecord(1));
    Generic acs (LookUpInObject (curcls, nm, false, true));
    Set acsSet;
    if (acs.IsSet())
      acsSet = acs;
    else
      acsSet.Insert(acs);

    Generic b_acs;
    for (bool cc = acsSet.First(b_acs); cc; cc = acsSet.Next(b_acs))
    {
      switch(tp.GetTag()) {
        case TAG_TYPE_REP_TypeNameRep: {
          res &= CheckTypeAccessAux (GetSSAccess(b_acs),
                                     TYPE_AS_TypeName().Init(tp.GetRecord(pos_REP_TypeNameRep_nm), NilContextId));
          break;
        }
        case TAG_TYPE_REP_CompositeTypeRep: {
          res &= CheckTypeAccessAux (GetSSAccess(b_acs),
                                     TYPE_AS_TypeName().Init(tp.GetRecord(pos_REP_TypeNameRep_nm), NilContextId));
          break;
        }
      }
    }

    Set ss (this->SuperSuper);
    Generic b_clnm;
    for (bool dd = ss.First(b_clnm); dd; dd = ss.Next(b_clnm))
    {
      Generic acs (LookUpInObject (b_clnm, nm, false, true));
      bool narrowed = false;
      if (IsAccessType(acs))
      {
        narrowed = !NotNarrowedAccess(GetSSAccess(acs), acc);
      }
      else if (IsAccessTypeSet(acs))
      {
        Set acsS (acs);
        Generic b_acs;
        bool exists = false;
        for (bool ee = acsS.First(b_acs); ee && !exists; ee = acsS.Next(b_acs))
        {
          exists = !NotNarrowedAccess(GetSSAccess(b_acs), acc);
        }
        narrowed = exists;
      }
      
      if (narrowed)
      {
        //--------------------------------------
        //-- Error 369
        //-- Scope narrowed by L"%1"
        //-------------------------------------
        GenErr(nm, ERR, 369, mk_sequence(PrintName(nm)));
        res = false;
      }
    }
  }
  return res;
}

// CheckTypeListAccess
// nm : AS`Name
// tp_l : seq of AS`Type
// return : bool
bool StatSem::CheckTypeListAccess (const TYPE_AS_Name & nm, const SEQ<TYPE_AS_Type> & tp_l)
{
  bool res (true);
  size_t len_tp_l = tp_l.Length();
  for (size_t idx = 1; idx <= len_tp_l; idx++)
    res = CheckTypeAccess (nm, tp_l[idx]) && res;
  return res;
}

// CheckTypeAccess
// nm : AS`Name
// tp : AS`Type
// return : bool
bool StatSem::CheckTypeAccess (const TYPE_AS_Name & nm, const TYPE_AS_Type & tp)
{
  Generic local_tp (LookUpInHierarchy (nm, GetCurClass (), TYPE, LOCAL));

  Generic nm_acs (local_tp.IsNil () ? LookUpInHierarchy (nm, GetCurClass (), VAL, LOCAL) : local_tp);

  if (IsAccessType(nm_acs))
  {
    return CheckTypeAccessAux (GetSSAccess(nm_acs), tp);
  }
  else if (IsAccessTypeSet(nm_acs))
  {
    Set nm_acsS (nm_acs);
    bool reswf = true;
    Generic l_atr;
    for (bool bb = nm_acsS.First(l_atr); bb; bb = nm_acsS.Next(l_atr))
      reswf = CheckTypeAccessAux(GetSSAccess(l_atr), tp) && reswf;
    return reswf;
  }
  else
    return true;
}

// GetMethRng
// p_atr : (REP`OpTypeRep | REP`FnTypeRep)
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::GetMethRng(const TYPE_REP_TypeRep & p_atr)
{
  switch(p_atr.GetTag()) {
    case TAG_TYPE_REP_OpTypeRep: {
      return p_atr.GetRecord(pos_REP_OpTypeRep_Rng);
    }
    case TAG_TYPE_REP_TotalFnTypeRep: {
      return p_atr.GetRecord(pos_REP_TotalFnTypeRep_fnrng);
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      return p_atr.GetRecord(pos_REP_PartialFnTypeRep_fnrng);
    }
    default: {
      return TYPE_REP_TypeRep();
    }
  }
}

// GetMethDom
// p_atr : (REP`OpTypeRep | REP`FnTypeRep)
// -> seq of REP`TypeRep
SEQ<TYPE_REP_TypeRep> StatSem::GetMethDom(const TYPE_REP_TypeRep & p_atr)
{
  switch(p_atr.GetTag()) {
    case TAG_TYPE_REP_OpTypeRep: {
      return p_atr.GetSequence(pos_REP_OpTypeRep_Dom);
    }
    case TAG_TYPE_REP_TotalFnTypeRep: {
      return p_atr.GetSequence(pos_REP_TotalFnTypeRep_fndom);
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      return p_atr.GetSequence(pos_REP_PartialFnTypeRep_fndom);
    }
    default: {
      return SEQ<TYPE_REP_TypeRep>();
    }
  }
}

// CheckTypeAccessAux
// main_acs : AS`Access
// tp : AS`Type
// ==> bool
bool StatSem::CheckTypeAccessAux (const TYPE_AS_Access & main_acs, const TYPE_AS_Type & tp)
{
  switch (tp.GetTag ()) {
    case TAG_TYPE_AS_BooleanType:
    case TAG_TYPE_AS_NumericType:
    case TAG_TYPE_AS_TokenType:
    case TAG_TYPE_AS_CharType:
      return true;
    case TAG_TYPE_AS_VoidType:
      return true;
    case TAG_TYPE_AS_QuoteType:
      return true;
    case TAG_TYPE_AS_ProductType: {
      const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence (pos_AS_ProductType_tps));
      bool forall (true);
      size_t len_tp_l = tp_l.Length();
      for (size_t idx = 1; idx <= len_tp_l; idx++)
        forall &= CheckTypeAccessAux (main_acs, tp_l[idx]);
      return forall;
    }
    case TAG_TYPE_AS_UnionType: {
      const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence(pos_AS_UnionType_tps));
      bool forall (true);
      size_t len_tp_l = tp_l.Length();
      for (size_t idx = 1; idx <= len_tp_l; idx++)
        forall &= CheckTypeAccessAux (main_acs, tp_l[idx]);
      return forall;
    }
    case TAG_TYPE_AS_CompositeType: {
      const SEQ<TYPE_AS_Field> & flds (tp.GetSequence(pos_AS_CompositeType_fields));
      bool forall (true);
      size_t len_flds = flds.Length();
      for (size_t idx = 1; idx <= len_flds; idx++)
        forall &= CheckTypeAccessAux (main_acs, flds[idx].GetRecord(pos_AS_Field_type));
      return forall;
    }
    case TAG_TYPE_AS_OptionalType:
      return CheckTypeAccessAux (main_acs, tp.GetRecord(pos_AS_OptionalType_tp));
    case TAG_TYPE_AS_BracketedType:
      return CheckTypeAccessAux (main_acs, tp.GetRecord(pos_AS_BracketedType_tp));
    case TAG_TYPE_AS_Seq0Type:
      return CheckTypeAccessAux (main_acs, tp.GetRecord(pos_AS_Seq0Type_elemtp));
    case TAG_TYPE_AS_Seq1Type:
      return CheckTypeAccessAux (main_acs, tp.GetRecord(pos_AS_Seq1Type_elemtp));
    case TAG_TYPE_AS_Set0Type:
      return CheckTypeAccessAux (main_acs, tp.GetRecord(pos_AS_Set0Type_elemtp));
    case TAG_TYPE_AS_Set1Type:
      return CheckTypeAccessAux (main_acs, tp.GetRecord(pos_AS_Set1Type_elemtp));
    case TAG_TYPE_AS_GeneralMap0Type: {
      return CheckTypeAccessAux (main_acs, tp.GetRecord(pos_AS_GeneralMap0Type_mapdom)) &&
             CheckTypeAccessAux (main_acs, tp.GetRecord(pos_AS_GeneralMap0Type_maprng));
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      return CheckTypeAccessAux (main_acs, tp.GetRecord(pos_AS_GeneralMap1Type_mapdom)) &&
             CheckTypeAccessAux (main_acs, tp.GetRecord(pos_AS_GeneralMap1Type_maprng));
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      return CheckTypeAccessAux (main_acs, tp.GetRecord(pos_AS_InjectiveMap0Type_mapdom)) &&
             CheckTypeAccessAux (main_acs, tp.GetRecord(pos_AS_InjectiveMap0Type_maprng));
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      return CheckTypeAccessAux (main_acs, tp.GetRecord(pos_AS_InjectiveMap1Type_mapdom)) &&
             CheckTypeAccessAux (main_acs, tp.GetRecord(pos_AS_InjectiveMap1Type_maprng));
    }
    case TAG_TYPE_AS_OpType: {
      const SEQ<TYPE_AS_Type> & dtp_l (tp.GetSequence(pos_AS_OpType_opdom));
      bool forall (CheckTypeAccessAux (main_acs, tp.GetRecord(pos_AS_OpType_oprng)));
      size_t len_dtp_l = dtp_l.Length();
      for (size_t idx = 1; idx <= len_dtp_l; idx++)
        forall &= CheckTypeAccessAux (main_acs, dtp_l[idx]);
      return forall;
    }
    case TAG_TYPE_AS_PartialFnType: {
      const SEQ<TYPE_AS_Type> & dtp_l (tp.GetSequence(pos_AS_PartialFnType_fndom));
      bool forall (CheckTypeAccessAux (main_acs, tp.GetRecord(pos_AS_PartialFnType_fnrng)));
      size_t len_dtp_l = dtp_l.Length();
      for (size_t idx = 1; idx <= len_dtp_l; idx++)
        forall &= CheckTypeAccessAux (main_acs, dtp_l[idx]);
      return forall;
    }
    case TAG_TYPE_AS_TotalFnType: {
      const SEQ<TYPE_AS_Type> & dtp_l (tp.GetSequence(pos_AS_TotalFnType_fndom));
      bool forall (CheckTypeAccessAux (main_acs, tp.GetRecord(pos_AS_TotalFnType_fnrng)));
      size_t len_dtp_l = dtp_l.Length();
      for (size_t idx = 1; idx <= len_dtp_l; idx++)
        forall &= CheckTypeAccessAux (main_acs, dtp_l[idx]);
      return forall;
    }
    case TAG_TYPE_AS_TypeName: {
      const TYPE_AS_Name & nm (tp.GetRecord(pos_AS_TypeName_name));
      Generic lookup (LookUpTypeName (nm, true));
      if (lookup.Is(TAG_TYPE_REP_ObjRefTypeRep))
        return true;
// 20070626 -->
// special case return type name is op/fn name and op/fn is overloaded
      else if (lookup.Is(TAG_TYPE_REP_AllTypeRep))
        return true;
// <-- 20070626
      else
      {
        Generic acc (CheckAccessCurClass (lookup));
        if (acc.IsNil ()) {
          //---------------------
          // Error #368
          // Access violation for ..
          //-------------------
          GenErr (nm, ERR, 368, mk_sequence(PrintName (nm)));
          return false;
        }
        else {
          TYPE_AS_Access tp_acs (LookUpAccessForType (lookup,nm));
          if (NotNarrowedAccess (main_acs, tp_acs))
            return true;
          else {
            //----------------------
            // Error #369
            // Scope narrowed by ..
            //-----------------------
            GenErr (nm, ERR, 369, mk_sequence(PrintName (nm)));
          return false;
          }
        }
      }
    }
    case TAG_TYPE_AS_TypeVar:
      return true;
    case TAG_TYPE_AS_AllType:
      return true;
    default:
      InternalError (L"CheckTypeAccessAux");
      return false;
  }
  return true; // VC++
}

// CheckThreadInherit
// clsnm : AS`Name
// ==> bool
bool StatSem::CheckThreadInherit(const TYPE_AS_Name & clsnm)
{
  SET<TYPE_AS_Name> cl_s;
  Set ss (this->SuperSuper);
  Generic b_clnm;
  for (bool bb = ss.First(b_clnm); bb; bb = ss.Next(b_clnm))
  {
    Generic clrep (GetClassTypeRep(b_clnm));
    if (!clrep.IsNil()) 
    {
      TYPE_SSENV_ParseTypeInfo pti (clrep);
      if (pti.GetInt(pos_SSENV_ParseTypeInfo_thread) != Int(NONE))
      {
        cl_s.Insert(b_clnm);
      }
    }
  }
  bool wf (cl_s.Card() <= 1);
  if (!wf)
  {
    SEQ<Char> nm_l;
    bool first = true;
    Generic nm;
    for (bool cc = cl_s.First(nm); cc; cc = cl_s.Next(nm))
    {
      if (first)
        first = false;
      else
        nm_l.ImpConc(SEQ<Char>(L", "));

      nm_l.ImpConc(PrintName(nm));
    }
    //------------------------------------------
    // Error message #427
    // Multiple threads are inherited from "%1"
    //------------------------------------------
    GenErr(clsnm, ERR, 427, mk_sequence(nm_l));
  }
  return wf;
}

// LookUpAccessForType
// tp : (AccessType|REP`TypeRep)
// nm : AS`Name
// return : AS`Access
TYPE_AS_Access StatSem::LookUpAccessForType (const Record & tp, const TYPE_AS_Name & nm)
{
  if (IsAccessType (tp))
    return GetSSAccess(tp);
  else {
    Generic lu (LookUpInHierarchy (nm, GetCurClass (), TYPE, LOCAL));
    if (lu.IsNil ()) {
      Generic lu_q (LookUpInHierarchy (nm, GetCurClass (), VAL, LOCAL));
      if (!lu_q.IsNil ())
      {
        if (IsAccessTypeSet(lu_q)) // IsSetOfOpOrFn(lu_q)
        {
          Set l_s (lu_q);
          const TYPE_CI_ContextId & cid (tp.GetRecord(pos_REP_ObjRefTypeRep_nm)
                                           .GetInt(pos_AS_Name_cid));
          Generic b_atr;
          for (bool bb = l_s.First(b_atr); bb; bb = l_s.Next(b_atr))
          {
            TYPE_REP_ObjRefTypeRep l_ort (GetMethRng(StripAccessType(b_atr)));

            if (cid == l_ort.get_nm().get_cid())
              return GetSSAccess(b_atr);
          }
          return Int(NOT_INITIALISED_AS);
        }
        else
          return GetSSAccess(lu_q);
      }
      else
        return Int(NOT_INITIALISED_AS);
    }
    else
      return GetSSAccess(lu);
  }
}

// NotNarrowedAccess
// master : AS`Access
// slave : AS`Access
// +> bool
bool StatSem::NotNarrowedAccess (const TYPE_AS_Access & master, const TYPE_AS_Access & slave)
{
  if (slave == Int(NOT_INITIALISED_AS))
    return true;

  switch (Int(master).GetValue ()) {
    case PUBLIC_AS:
      return (slave == master);
    case PROTECTED_AS:
      return ((slave == master) || (slave == Int(PUBLIC_AS)));
    case PRIVATE_AS:
    case NOT_INITIALISED_AS:
      return true;
    default:
      return true;
  }
  return true; // VC++
}

// CheckSubRespIsImplemented (not in spec)
// i : TYPE`Ind
// nm : AS`Name
bool StatSem::CheckSubRespIsImplemented(const Int & i, const TYPE_AS_Name & nm)
{
  Tuple t (GetSubResps(nm));
  Map notimplemented (t.GetMap(1));
  if (!notimplemented.IsEmpty())
  {
    Set dom_notimplemented (notimplemented.Dom());
    Generic n;
    for (bool bb = dom_notimplemented.First(n); bb; bb = dom_notimplemented.Next(n))
    {
      //-----------------------------------------------
      // Error message #439
      // Function/operation "%1" must be implemented
      //-----------------------------------------------
      GenErr (nm, ERR, 439, mk_sequence(PrintName(ASTAUX::GetSecondName(n))));
    }
  }
  return notimplemented.IsEmpty();
}

// GetNotImplemented
// nm : AS`Name
// ==> map AS`Name to ENV`AccessFnTypeRep | ENV`AccessPolyTypeRep | ENV`AccessOpTypeRep |
//                      set of (ENV`AccessOpTypeRep | ENV`AccessPolyTypeRep | ENV`AccessFnTypeRep) *
//     map AS`Name to ENV`AccessFnTypeRep | ENV`AccessPolyTypeRep | ENV`AccessOpTypeRep |
//                      set of (ENV`AccessOpTypeRep | ENV`AccessPolyTypeRep | ENV`AccessFnTypeRep)
Tuple StatSem::GetSubResps(const TYPE_AS_Name & nm)
{
  if (this->SubrespsEnv.DomExists(nm))
    return this->SubrespsEnv[nm];

  Generic l_pte (GetClassTypeRep(nm));
  if (l_pte.IsNil())
    return mk_(Map(), Map()); // class is`t found
  else
  {
    const TYPE_SSENV_ParseTypeInfo & pti = l_pte;
    Set super (pti.GetSet(pos_SSENV_ParseTypeInfo_super));
    Map notimplemented (pti.GetMap(pos_SSENV_ParseTypeInfo_subresps));
    Map implemented;
    Map impl;

    if (!super.IsEmpty())
    {
      // remove subresp methods
      const Map & fcts (pti.GetMap(pos_SSENV_ParseTypeInfo_fcts));
      Set dom_fcts (fcts.Dom());
      Generic n;
      for (bool bb = dom_fcts.First(n); bb; bb = dom_fcts.Next(n))
      {
        if (!notimplemented.DomExists(n))
          implemented.ImpModify(n, fcts[n]);
      }
      const Map & polys (pti.GetMap(pos_SSENV_ParseTypeInfo_polys));
      Set dom_polys (polys.Dom());
      for (bool cc = dom_polys.First(n); cc; cc = dom_polys.Next(n))
      {
        if (!notimplemented.DomExists(n))
          implemented.ImpModify(n, polys[n]);
      }
      const Map & ops (pti.GetMap(pos_SSENV_ParseTypeInfo_ops));
      Set dom_ops (ops.Dom());
      for (bool dd = dom_ops.First(n); dd; dd = dom_ops.Next(n))
      {
        if (!notimplemented.DomExists(n))
          implemented.ImpModify(n, ops[n]);
      }
      const Map & overloaded (pti.GetMap(pos_SSENV_ParseTypeInfo_overloaded));
      Set dom_overloaded (overloaded.Dom());
      for (bool ee = dom_overloaded.First(n); ee; ee = dom_overloaded.Next(n))
      {
        if (!notimplemented.DomExists(n))
          implemented.ImpModify(n, overloaded[n]);
        else
        {
          Set over (overloaded[n]);
          const Generic & atp (notimplemented[n]);
          over.ImpDiff(atp.IsSet() ? Set(atp) : mk_set(atp));
          switch (over.Card()) {
            case 0: {
              break;
            }
            case 1: {
              implemented.ImpModify(n, over.GetElem());
              break;
            }
            default: {
              implemented.ImpModify(n, over);
              break;
            }
          } 
        }
      }

      // merge subresp of all super classes
      Map sb;
      Generic clnm;
      for (bool ff = super.First(clnm); ff; ff = super.Next(clnm))
      {
        //Map sb (GetSubResps(clnm));
        Tuple t (GetSubResps(clnm));
        sb.ImpOverride(t.GetMap(1));
        impl.ImpOverride(t.GetMap(2));
      }

// 20120905 -->
      // Mearg impl method
      Map mergedimpl (implemented);  

      Set dom_impl (impl.Dom());
      Generic ng;
      for (bool ff = dom_impl.First(ng); ff; ff = dom_impl.Next(ng))
      {
        TYPE_AS_Name nn (ASTAUX::Combine2Names(nm, ng));
        if (mergedimpl.DomExists(nn))
        {
          Generic atp1 (mergedimpl[nn]);
          Generic atp2 (impl[ng]);
          Set atp ((atp1.IsSet() ? Set(atp1) : mk_set(atp1)).Union(atp2.IsSet() ? Set(atp2) : mk_set(atp2)));
          mergedimpl.ImpModify(nn, atp); 
        }
        else
        {
          mergedimpl.Insert(nn, impl[ng]);
        }
      }
// <-- 20120905

      SetDefClass(nm); // 20101006 for IsEquivalen
      Set dom_sb (sb.Dom());
      Generic mnm;
      for (bool cc = dom_sb.First(mnm); cc; cc = dom_sb.Next(mnm))
      {
        TYPE_AS_Name nn (ASTAUX::Combine2Names(nm, mnm));
        Generic atp2 (sb[mnm]);
        Set atp2_s (atp2.IsSet() ? Set(atp2) : mk_set(atp2));

        if (mergedimpl.DomExists(nn))
        {
          const Generic & atp1 (mergedimpl[nn]);
          Set atp1_s (atp1.IsSet() ? Set(atp1) : mk_set(atp1));
          Set eqs;
          Generic atp2g;
          for (bool dd = atp2_s.First(atp2g); dd; dd = atp2_s.Next(atp2g)) 
          {
            bool exists = false;
            Generic atp1g;
            for (bool ee = atp1_s.First(atp1g); ee && !exists; ee = atp1_s.Next(atp1g)) 
              exists = IsEquivalent(POS, StripAccessType(atp1g), StripAccessType(atp2g));
            if (exists)
              eqs.Insert(atp2g);
          }
          atp2_s.ImpDiff(eqs);
        }

        switch(atp2_s.Card()) {
          case 0: {
            continue;
          }
          case 1 : {
            notimplemented.ImpModify(mnm, atp2_s.GetElem());
            break;
          }
          default: {
            notimplemented.ImpModify(mnm, atp2_s);
            break;
          }
        }
      }
      SetDefClass(Nil()); // 20101006 for IsEquivalent
    }

    Set dom_implemented (implemented.Dom());
    Generic n;
    for (bool gg = dom_implemented.First(n); gg; gg = dom_implemented.Next(n))
    {
      Generic atp (implemented[n]);
      Set atps (atp.IsSet() ? Set(atp) : mk_set(atp));
      Set atps2;
      Generic a;
      for (bool hh = atps.First(a); hh; hh = atps.Next(a))
      {
        Generic acc = GetSSAccess(a);
        if ((acc == Int(PUBLIC_AS)) || (acc == Int(PROTECTED_AS)))
          atps2.Insert(a);
      }
      switch (atps2.Card()) {
        case 0: {
          break;
        }
        case 1: {
          impl.Insert(n, atps2.GetElem());
          break;
        }
        default: {
          impl.Insert(n, atps2);
          break;
        }
      }
    }
    this->SubrespsEnv.ImpModify(nm, mk_(notimplemented, impl));
    return mk_(notimplemented, impl);
  }
}

// ChangeToStatic (not in spec)
// m : map AS`Name to AccessFnTypeRep | map AS`Name to AccessPolyTypeRep
// ==> map AS`Name to AccessFnTypeRep | map AS`Name to AccessPolyTypeRep
Map StatSem::ChangeToStatic(const Map & m)
{
  Map new_m;
  Set dom_m (m.Dom());
  Generic nm;
  for (bool bb = dom_m.First(nm); bb; bb = dom_m.Next(nm))
  {
    TYPE_SSENV_AccessType tp (m[nm]);
    switch(tp.GetTag()) {
      case TAG_TYPE_SSENV_AccessFnTypeRep: {
        tp.SetField(pos_SSENV_AccessFnTypeRep_stat, Bool(true));
        break;
      }
      case TAG_TYPE_SSENV_AccessPolyTypeRep: {
        tp.SetField(pos_SSENV_AccessPolyTypeRep_stat, Bool(true));
        break;
      }
    }
    new_m.ImpModify(nm, tp);
  }
  return new_m;
}
#endif // VDMPP

#ifdef VDMSL
// InstallParEnv
// parenv : map AS`Name to REP`TypeRep
void StatSem::InstallParEnv (const MAP<TYPE_AS_Name,TYPE_REP_TypeRep> & parenv)
{
  this->TypeParEnv = parenv;
}

// LeaveCurModule
// modid : AS`Name
// exp : [AS`ExportSig]
void StatSem::LeaveCurModule (const TYPE_AS_Name & modid, const Generic & exp)
{
  Map tps, val, fns, ops;

  if (!exp.IsNil ())
  {
    TYPE_AS_ExportSig es (exp);
    tps = es.get_tps ();
    val = es.get_val ();
    fns = es.get_fns ();
    ops = es.get_ops ();
  }

//  Set inter (val.Dom ());
//  inter.ImpIntersect (this->ConstEnv.Dom ());
  Set inter (val.Dom ().ImpIntersect (this->ConstEnv.Dom ()));

  Generic nm, envval;
  for (bool bb = inter.First (nm); bb; bb = inter.Next (nm))
  {
    if(! this->ConstEnv[nm].get_used() && !val.DomExists(nm))
    {
      //------------------------------------------
      // Error message #53
      // The value L"%1" is not used in module "%2"
      //------------------------------------------
      GenErr(nm, WRN1, 53, mk_sequence(PrintName(nm), PrintName(modid)));
    }
  }

  MAP<TYPE_AS_Name,TYPE_SSENV_TypeRepElem> se (this->StateEnv);
  for (bool cc = se.First(nm); cc; cc = se.Next(nm))
  {
    if (! se[nm].get_used())
    {
      //----------------------------------------------------
      // Error message #48
      // The state component L"%1" is not used in module "%2"
      //----------------------------------------------------
      GenErr (nm, WRN1, 48, mk_sequence(PrintName (nm), PrintName (modid)));
    }
  }

  MAP<TYPE_AS_Name,TYPE_SSENV_TypeRepElem> te (this->TypeEnv);
  Set thetypes;
  for (bool dd = te.First(nm); dd; dd = te.Next(nm)) {
    thetypes.Insert( te[nm].get_tp());
  }

  for (bool ee = te.First(nm); ee; ee = te.Next(nm))
  {
    if (!te[nm].get_used() && !tps.DomExists (nm) && !TypeUsedImplicitly(nm, thetypes))
    {
      //-----------------------------------------
      // Error message #49
      // The type L"%1" is not used in module "%2"
      //-----------------------------------------
      GenErr (nm, WRN1, 49, mk_sequence(PrintName (nm), PrintName (modid)));
    }
  }

  MAP<TYPE_AS_Name,TYPE_SSENV_TypeRepElem> fe (this->FunctionEnv);
  for (bool ff = fe.First(nm); ff; ff = fe.Next(nm))
  {
    if (!fe[nm].get_used() && !fns.DomExists (nm) && !IsPrePostFn(nm))
    {
      //---------------------------------------------
      // Error message #50
      // The function L"%1" is not used in module "%2"
      //---------------------------------------------
      GenErr (nm, WRN1, 50, mk_sequence(PrintName (nm), PrintName (modid)));
    }
  }

  MAP<TYPE_AS_Name,TYPE_SSENV_PolyTypeRepElem> pe (this->PolyEnv);
  for (bool gg = pe.First(nm); gg; gg = pe.Next(nm))
  {
    if (!pe[nm].get_used() && !fns.DomExists (nm))
    {
      //---------------------------------------------
      // Error message #50
      // The function L"%1" is not used in module "%2"
      //---------------------------------------------
      GenErr (nm, WRN1, 50, mk_sequence(PrintName (nm), PrintName (modid)));
    }
  }

  MAP<TYPE_AS_Name,TYPE_SSENV_TypeRepElem> oe (this->OperationEnv);
  for (bool hh = oe.First(nm); hh; hh = oe.Next(nm))
  {
    if (!oe[nm].get_used() && !ops.DomExists (nm) && !IsPrePostFn(nm))
    {
      //----------------------------------------------
      // Error message #52
      // The operation L"%1" is not used in module "%2"
      //----------------------------------------------
      GenErr (nm, WRN1, 52, mk_sequence(PrintName (nm), PrintName (modid)));
    }
  }

  MAP<TYPE_AS_Name,TYPE_SSENV_TypeRepElem> ce (this->ConstEnv);
  for (bool ii = ce.First(nm); ii; ii = ce.Next(nm))
  {
    if (!ce[nm].get_used() && !val.DomExists (nm))
    {
      //------------------------------------------
      // Error message #53
      // The value L"%1" is not used in module "%2"
      //------------------------------------------
      GenErr (nm, WRN1, 53, mk_sequence(PrintName (nm), PrintName (modid)));
    }
  }
}
#endif // VDMSL

#ifdef VDMPP
// ExpandClass
// i : TYPE`Ind
// cls : AS`Class
// ==> bool
bool StatSem::ExpandClass (const Int & i, const TYPE_AS_Class & cls)
{
  this->WhileChecking = L"";

  const TYPE_AS_Name & nm         (cls.GetRecord(pos_AS_Class_nm));
  const SEQ<TYPE_AS_Name> & super (cls.GetSequence(pos_AS_Class_supercls));
  const Generic & defs            (cls.GetField(pos_AS_Class_defs));
  const Bool & sys                (cls.GetBool(pos_AS_Class_sys));

  InitEnv ();

  if (!InstallCurClass(nm, super))
    return false;

  if (defs.IsNil ()) {
    return true;
  }
  else {
    bool expanded = ExpandDefinitions (i, nm, defs);
    if (expanded) {
      Map vals;
      MAP<TYPE_AS_Name,TYPE_SSENV_TypeRepElem> cenv (this->ConstEnv);
      Generic gnm;
      for (bool bb = cenv.First(gnm); bb; bb = cenv.Next(gnm))
      {
        if (this->ValueIds.InSet(gnm))
          vals.Insert(ExtName(nm,gnm), QualifyTypeNames(cenv[gnm].get_tp(), nm));
      }
      UpdateValuesInParseEnv(nm, vals);
    }
    if (sys && !super.IsEmpty())
    {
      //-----------------------------------------------
      // Error message #433
      // The system class "%1" cannot inherit from other classes
      //-----------------------------------------------
      GenErr (nm, ERR, 433, mk_sequence(PrintName(nm)));
      return false;
    }
    else
      return expanded;
  }
}
#endif // VDMPP

#ifdef VDMSL
// ExpandDLModule
// i : TYPE`Ind
// imod : AS`DLModule
// ==> bool
bool StatSem::ExpandDLModule (const Int & i, const TYPE_AS_DLModule & imod)
{
  const TYPE_AS_Name & modid     (imod.GetRecord(pos_AS_DLModule_nm));
  const TYPE_AS_Interface & intf (imod.GetRecord(pos_AS_DLModule_intf));

  SetCurMod(modid);

  bool reswf = true;

  Map imp_m (intf.GetMap(pos_AS_Interface_imp));
  const TYPE_AS_DLExportSig & exp (intf.GetRecord(pos_AS_Interface_exp));

  Generic nm;
  for (bool bb = imp_m.First (nm); bb; bb = imp_m.Next (nm))
    if (!(nm == modid))
      reswf = ExpandDLImport( nm, imp_m[nm], modid) && reswf;

  Map val (exp.GetMap(pos_AS_DLExportSig_val));
  Map fns (exp.GetMap(pos_AS_DLExportSig_fns));
  Map ops (exp.GetMap(pos_AS_DLExportSig_ops));

  for (bool cc = val.First (nm); cc; cc = val.Next (nm))
    reswf = wf_Type(i, val[nm]) && reswf;

  for (bool dd = fns.First (nm); dd; dd = fns.Next (nm))
    reswf = wf_Type(i, fns[nm]) && reswf;

  for (bool ee = ops.First (nm); ee; ee = ops.Next (nm))
    reswf = wf_Type(i, ops[nm]) && reswf;

  return reswf;
}

// ExpandDLImport
// fromid : AS`Name
// imp : AS`DLImportSig
// : AS`Name
// ==> bool
bool StatSem::ExpandDLImport (const TYPE_AS_Name & fromid,
                              const TYPE_AS_DLImportSig & imp,
                              const TYPE_AS_Name & /* curmod */)
{
  bool reswf = true;

  Tuple tmp (GetModuleRep(fromid));
//  const Bool & parsed (tmp.GetBool (1));
  const TYPE_SSENV_ExpSigRep & exp (tmp.GetRecord (2));

  const Map & tpsr (exp.GetMap(pos_SSENV_ExpSigRep_tps));
  Map imp_tps (imp.GetMap(pos_AS_DLImportSig_tps));
  Map tp_imps;

  Generic nm;
  for (bool bb = imp_tps.First (nm); bb; bb = imp_tps.Next (nm)) {
    if (Record(nm).GetSequence(pos_AS_Name_ids).Length () == 1)
      tp_imps.ImpModify (ExtName (fromid,nm), imp_tps[nm]);
    else
      tp_imps.ImpModify (nm, imp_tps[nm]);
  }

  Set unknowns (tp_imps.Dom ());
  Set knowns (tp_imps.Dom ());
  unknowns.ImpDiff (tpsr.Dom ());
  knowns.ImpDiff (unknowns);

  Generic tpnm;
  for (bool cc = unknowns.First (tpnm); cc; cc = unknowns.Next (tpnm)) {
    //-----------------------------------------------
    // Error message #55
    // The type L"%1" is not exported from module "%2"
    //-----------------------------------------------
    GenErr (tpnm, ERR, 55, mk_sequence(PrintName(fromid), PrintName(tpnm)));
    reswf = false;
  }

  for (bool dd = knowns.First (nm); dd; dd = knowns.Next (nm)) {
    this->TypeEnv.ImpModify (nm, mk_SSENV_TypeRepElem(tpsr[nm], Bool(true), Bool(false)));
  }
  for (bool ee = unknowns.First (nm); ee; ee = unknowns.Next (nm)) {
    this->TypeEnv.ImpModify (nm, mk_SSENV_TypeRepElem(mk_REP_TypeParRep( nm ), Bool(true), Bool(false)));
  }
  return reswf;
}

// ExpandModule
// i : TYPE`Ind
// mod : (AS`Module | AS`DLModule)
// ==> bool
bool StatSem::ExpandModule (const Int & i, const Record & mod)
{
  this->WhileChecking = L"";

  if (mod.Is(TAG_TYPE_AS_DLModule)) {
    return ExpandDLModule(i,(const TYPE_AS_DLModule &)mod);
  }
  else
  {
    const TYPE_AS_Name & nm          (mod.GetRecord(pos_AS_Module_nm));
    const TYPE_AS_Interface & intf   (mod.GetRecord(pos_AS_Module_intf));
    const TYPE_AS_Definitions & defs (mod.GetRecord(pos_AS_Module_defs));

    SetCurMod(nm);

    InsideInter = true;
    bool reswf (ExpandTypeImports (i, nm, intf));
    InsideInter = false;

    reswf &= ExpandDefinitions (i, nm, defs);

    InsideInter = true;
    reswf &= ExpandInterface (i, nm, intf);
    InsideInter = false;

    reswf &= (ExtractValueDefs(i, defs.GetSequence(pos_AS_Definitions_valuem)));

    return reswf;
  }
}

// ExpandTypeImports
// i : TYPE`Ind
// modif : AS`Name
// intf : AS`Interface
// ==> bool
bool StatSem::ExpandTypeImports (const Int & i, const TYPE_AS_Name & modid, const TYPE_AS_Interface & intf)
{
  const Map & imp_m (intf.GetMap(pos_AS_Interface_imp)); // map Name to [ImportSig]

  bool reswf (true);

  Set dom_imp_m (imp_m.Dom());
  Generic nm;
  for (bool bb = dom_imp_m.First(nm); bb; bb = dom_imp_m.Next(nm))
    if (!(nm == modid))
      reswf = ExpandTypeImport (i, nm, imp_m[nm], modid) && (reswf);

  return reswf;
}

// ExpandTypeImport
// i : TYPE`Ind
// modid : AS`Name
// sig : [AS`ImportSig]
// curmod : AS`Name
// ==> bool
bool StatSem::ExpandTypeImport (const Int & i,
                                const TYPE_AS_Name & modid,
                                const Generic & sig,
                                const TYPE_AS_Name & curmod)
{
  bool reswf (true);

  Tuple infer (GetModuleRep (modid));
  const Bool & parsed (infer.GetBool (1));
  const TYPE_SSENV_ExpSigRep & exp (infer.GetRecord(2));

  if (sig.IsNil ())
  {
    if (parsed.GetValue ())
    {
      const Map & tpsr (exp.GetMap(pos_SSENV_ExpSigRep_tps)); // map AS`Name to REP`TypeRep

      Set dom_tpsr (tpsr.Dom());
      Generic nm;
      for (bool bb = dom_tpsr.First(nm); bb; bb = dom_tpsr.Next(nm))
      {
        this->TypeEnv.ImpModify (nm, mk_SSENV_TypeRepElem(tpsr[nm], Bool(true), Bool(false)));
      }
    }
  }
  else
  {
    TYPE_AS_ImportSig is (sig);
    const Map & sigtps (is.GetMap(pos_AS_ImportSig_tps)); // map AS`Name to [AS`TypeDef]
    const Map & sigren (is.GetMap(pos_AS_ImportSig_ren)); // map AS`Name to AS`Name
    Map tps;

    Set dom_sigtps (sigtps.Dom());
    Generic nm_g;
    for (bool bb = dom_sigtps.First (nm_g); bb; bb = dom_sigtps.Next (nm_g))
    {
      TYPE_AS_Name nm(nm_g);
      if (nm.GetSequence(pos_AS_Name_ids).Length () == 1)
        tps.Insert (ExtName (modid, nm), sigtps[nm]);
      else
        tps.Insert (nm, sigtps[nm]);
    }
    Tuple infer (CheckTypeRename (tps, sigren, modid));
    const Map & tps_ (infer.GetMap (1));
    const Map & ren_ (infer.GetMap (2));

    reswf &= ImportTypes (i, ren_, tps_, exp.GetMap(pos_SSENV_ExpSigRep_tps), modid, curmod);
  }
  return reswf;
}

// CheckImpTypeDef
// tp : REP`TypeRep
// modid : AS`Name
// ==> bool
bool StatSem::CheckImpTypeDef(const TYPE_REP_TypeRep & tp, const TYPE_AS_Name & modid)
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
    case TAG_TYPE_REP_TypeVarRep:
    case TAG_TYPE_REP_TypeParRep:
      return true;
    case TAG_TYPE_REP_TypeNameRep: {
      const TYPE_AS_Name & nm (tp.GetRecord(pos_REP_TypeNameRep_nm));
      if (this->TypeEnv.DomExists(nm) || this->RenamedTypes.DomExists(nm))
        return true;
      else {
        //--------------------------------------
        // Error message #57
        // The type L"%1" is not imported in "%2"
        //--------------------------------------
        GenErr(nm, ERR, 57, mk_sequence(PrintName(nm), PrintName(modid)));
        this->TypeEnv.Insert(nm, mk_SSENV_TypeRepElem(rep_alltp, Bool(true), Bool(false)));
        return false;
      }
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
      const SEQ<TYPE_REP_FieldRep> & fields (tp.GetSequence(pos_REP_CompositeTypeRep_fields));
      bool reswf = true;
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++)
        reswf &= CheckImpTypeDef(fields[idx].GetRecord(pos_REP_FieldRep_tp), modid);
      return reswf;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (tp.GetSet(pos_REP_UnionTypeRep_tps));
      bool reswf = true;
      Generic type;
      for (bool bb = tp_s.First(type); bb; bb = tp_s.Next(type))
        reswf &= CheckImpTypeDef(type, modid);
      return reswf;
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & tp_l (tp.GetSequence(pos_REP_ProductTypeRep_tps));
      bool reswf = true;
      size_t len_tp_l = tp_l.Length();
      for (size_t idx = 1; idx <= len_tp_l; idx++)
        reswf &= CheckImpTypeDef(tp_l[idx], modid);
      return reswf;
    }
    case TAG_TYPE_REP_InvTypeRep:
      return CheckImpTypeDef(tp.GetRecord(pos_REP_InvTypeRep_shape), modid);
    case TAG_TYPE_REP_SetTypeRep:
      return CheckImpTypeDef(tp.GetRecord(pos_REP_SetTypeRep_elemtp), modid);
    case TAG_TYPE_REP_SeqTypeRep:
      return CheckImpTypeDef(tp.GetRecord(pos_REP_SeqTypeRep_elemtp), modid);
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return
      CheckImpTypeDef(tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom), modid) &&
      CheckImpTypeDef(tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng), modid);
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return
      CheckImpTypeDef(tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom), modid) &&
      CheckImpTypeDef(tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng), modid);
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & tp_l (tp.GetSequence(pos_REP_PartialFnTypeRep_fndom));
      bool reswf = true;
      size_t len_tp_l = tp_l.Length();
      for (size_t idx = 1; idx <= len_tp_l; idx++)
        reswf &= CheckImpTypeDef(tp_l[idx], modid);
      return CheckImpTypeDef(tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng), modid) && reswf;
    }
    case TAG_TYPE_REP_TotalFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & tp_l (tp.GetSequence(pos_REP_TotalFnTypeRep_fndom));
      bool reswf = true;
      size_t len_tp_l = tp_l.Length();
      for (size_t idx = 1; idx <= len_tp_l; idx++)
        reswf &= CheckImpTypeDef(tp_l[idx], modid);
      return CheckImpTypeDef(tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng), modid) && reswf;
    }
    default:
      return true;
  }
}

// CheckTypeRename
// tps : (map AS`Name to [AS`TypeDef])
// ren : (map AS`Name to AS`Name)
// modnm : AS`Name
// ==> (map AS`Name to [AS`TypeDef]) * (map AS`Name to AS`Name)
Tuple StatSem::CheckTypeRename (const Map & tps,
                                const MAP<TYPE_AS_Name, TYPE_AS_Name> & ren,
                                const TYPE_AS_Name & modnm)
{
  Map tps_q (tps);

  SET<TYPE_AS_Name> dom_ren (ren.Dom());
  Generic nm;
  for (bool bb = dom_ren.First(nm); bb; bb = dom_ren.Next(nm))
  {
    TYPE_AS_Name fullnm (ExtName (modnm, ren[nm]));
// 20140321 -->
    //this->RenamedTypes.ImpModify (fullnm, nm);
    this->RenamedTypes.ImpModify (nm, fullnm);
// <-- 20140321
    if (tps.DomExists (fullnm))
    {
      tps_q.ImpModify (nm, tps_q[fullnm]);
      tps_q.RemElem (fullnm);
    }
  }

  MAP<TYPE_AS_Name, TYPE_AS_Name> ren_q;
  for (bool cc = dom_ren.First(nm); cc; cc = dom_ren.Next(nm))
    ren_q.ImpModify (nm, ExtName (modnm, ren[nm]));

  return mk_(tps_q, ren_q);
}

// ExpandInterface
// i : TYPE`Ind
// modid : AS`Name
// intf : AS`Interface
// ==> bool
bool StatSem::ExpandInterface (const Int & i, const TYPE_AS_Name & modid, const TYPE_AS_Interface & intf)
{
  const Map & imp (intf.GetMap(pos_AS_Interface_imp)); // map Name to [ImportSig]
  const Generic & exp (intf.GetField(pos_AS_Interface_exp)); // [ExportSig]

  bool reswf (ExpandImports (i, modid, imp));

  Set rng_TypeEnv (this->TypeEnv.Rng());
  Generic te; // 
  for (bool bb = rng_TypeEnv.First(te); bb; bb = rng_TypeEnv.Next(te))
    reswf &= CheckImpTypeDef(Record(te).GetRecord(pos_SSENV_TypeRepElem_tp), modid);

  if (!exp.IsNil ())
    reswf &= ExpandExport (exp);

  return reswf;
}

// ExpandImports
// i : TYPE`Ind
// modid : AS`Name
// imp_m : map AS`Name to [AS`ImportSig]
// ==> bool
bool StatSem::ExpandImports (const Int & i, const TYPE_AS_Name & modid, const Map & imp_m)
{
  bool reswf = true;

  SET<TYPE_AS_Name> dom_imp_m (imp_m.Dom());
  if (dom_imp_m.InSet(modid))
  {
    //------------------------------------------
    // Error message #56
    // The module L"%1" cannot instantiate itself
    //------------------------------------------
    GenErr (modid, ERR, 56, mk_sequence(PrintName(modid)));
    reswf = false;
    dom_imp_m.RemElem(modid);
  }

  Generic nm;
  // imp_m : map AS`Name to [AS`ImportSig]
  for (bool bb = dom_imp_m.First(nm); bb; bb = dom_imp_m.Next(nm))
    reswf &= ExpandImport (i, nm, imp_m[nm], modid);

  return reswf;
}

// ExpandImport
// i : TYPE`Ind
// modid : AS`Name
// sig : [AS`ImportSig]
// surmod : AS`Name
// ==> bool
bool StatSem::ExpandImport (const Int & i,
                            const TYPE_AS_Name & modid,
                            const Generic & sig,
                            const TYPE_AS_Name & curmod)
{
  Tuple infer (GetModuleRep (modid));
  const Bool & parsed (infer.GetBool (1));
  const TYPE_SSENV_ExpSigRep & exp (infer.GetRecord (2)); // ExpSigRep

  if (!parsed)
  {
    //-----------------------------------------------------------------
    // Error message #63
    // The interface to module L"%1" is not found and will not be tested
    //-----------------------------------------------------------------
    GenErr (modid, WRN2, 63, mk_sequence(PrintName (modid)));
  }

  if (sig.IsNil ())
  {
    if (parsed.GetValue ())
    { //  Map tpsr (exp.GetField (1)),
      const Map & valr  (exp.GetMap(pos_SSENV_ExpSigRep_val));   // map AS`Name to REP`TypeRep
      const Map & fnsr  (exp.GetMap(pos_SSENV_ExpSigRep_fns));   // map AS`Name to REP`FnTypeRep
      const Map & polyr (exp.GetMap(pos_SSENV_ExpSigRep_polys)); // map AS`Name to REP`PolyTypeRep
      const Map & opsr  (exp.GetMap(pos_SSENV_ExpSigRep_ops));   // map AS`Name to REP`OpTypeRep
      const Map & tag   (exp.GetMap(pos_SSENV_ExpSigRep_tag));   // map AS`Name to seq of REP`FieldRep

      this->TagEnv.ImpOverride (tag);

      Generic nm;
      Set dom_valr (valr.Dom());
      for (bool bb = dom_valr.First(nm); bb; bb = dom_valr.Next(nm))
      {
        this->ConstEnv.ImpModify (nm, mk_SSENV_TypeRepElem(valr[nm], Bool(true), Bool(true)));
      }

      Set dom_fnsr (fnsr.Dom());
      for (bool cc = dom_fnsr.First(nm); cc; cc = dom_fnsr.Next(nm))
      {
        this->FunctionEnv.ImpModify (nm, mk_SSENV_TypeRepElem(fnsr[nm], Bool(true), Bool(false)));
      }

      Set dom_polyr (polyr.Dom());
      for (bool dd = dom_polyr.First(nm); dd; dd = dom_polyr.Next(nm))
      {
        const TYPE_REP_PolyTypeRep & ptr (polyr[nm]);
        this->PolyEnv.ImpModify (nm, mk_SSENV_PolyTypeRepElem(ptr.GetSequence (pos_REP_PolyTypeRep_vars),
                                                              ptr.GetRecord (pos_REP_PolyTypeRep_tp),
                                                              Bool(true)));
      }

      Set dom_opsr (opsr.Dom());
      for (bool ee = dom_opsr.First(nm); ee; ee = dom_opsr.Next(nm))
      {
        this->OperationEnv.ImpModify (nm, mk_SSENV_OpTypeRepElem(opsr[nm], Bool(true), Bool(false), Bool(false)));
      }
    }
    return true;
  }
  else
  {
    return ImportExpl (i, sig, exp, parsed, modid, curmod);
  }
}

// ImportExpl
// i : TYPE`Ind
// is : AS`ImportSig
// exp : VCM`ExpSigRep
// parsed : bool
// modid : AS`Name
// curmod : AS`Name
// ==> bool
bool StatSem::ImportExpl (const Int & i,
                          const TYPE_AS_ImportSig & is,
                          const TYPE_SSENV_ExpSigRep & exp,
                          const Bool & parsed,
                          const TYPE_AS_Name & modid,
                          const TYPE_AS_Name & curmod)
{
  const Map & tps (is.GetMap(pos_AS_ImportSig_tps)); // map Name to [TypeDef]
  const Map & val (is.GetMap(pos_AS_ImportSig_val)); // map Name to [Type]
  const Map & fns (is.GetMap(pos_AS_ImportSig_fns)); // map Name to [(TypeVarList * FnType)]
  const Map & ops (is.GetMap(pos_AS_ImportSig_ops)); // map Name to [OpType]
  const Map & ren (is.GetMap(pos_AS_ImportSig_ren)); // map Name to Name
  const TYPE_CI_ContextId & cid (is.GetInt(pos_AS_ImportSig_cid));

  const Map & expval   (exp.GetMap(pos_SSENV_ExpSigRep_val));     // map AS`Name to REP`TypeRep
  const Map & expfns   (exp.GetMap(pos_SSENV_ExpSigRep_fns));     // map AS`Name to REP`FnTypeRep
  const Map & exppolys (exp.GetMap(pos_SSENV_ExpSigRep_polys)); // map AS`Name to REP`PolyTypeRep
  const Map & expops   (exp.GetMap(pos_SSENV_ExpSigRep_ops));     // map AS`Name to REP`OpTypeRep
  const Map & exptag   (exp.GetMap(pos_SSENV_ExpSigRep_tag));     // map AS`Name to seq of REP`FieldRep

  Map tps_q, val_q, fns_q, ops_q;

  Generic nm;
  Set dom_tps (tps.Dom());
  for (bool bb = dom_tps.First(nm); bb; bb = dom_tps.Next(nm))
    tps_q.Insert (ExtName (modid, nm), tps[nm]);

  Set dom_val (val.Dom());
  for (bool cc = dom_val.First(nm); cc; cc = dom_val.Next(nm))
    val_q.Insert (ExtName (modid, nm), val[nm]);

  Set dom_fns (fns.Dom());
  for (bool dd = dom_fns.First(nm); dd; dd = dom_fns.Next(nm))
    fns_q.Insert (ExtName (modid, nm), fns[nm]);

  Set dom_ops (ops.Dom());
  for (bool ee = dom_ops.First(nm); ee; ee = dom_ops.Next(nm))
    ops_q.Insert (ExtName (modid, nm), ops[nm]);

  TYPE_AS_ImportSig newsig (TYPE_AS_ImportSig().Init( tps_q, val_q, fns_q, ops_q, ren, cid ));

  Tuple infer (CheckRename (newsig, modid));
  const TYPE_AS_ImportSig & ims (infer.GetRecord (1));
  const Bool & wf_ren (infer.GetBool (2));
  const Map & ren_q (infer.GetMap (3));

  const Map & tp_qq (ims.GetMap (1));
  const Map & val_qq (ims.GetMap (2));
  const Map & fns_qq (ims.GetMap (3));
  const Map & ops_qq (ims.GetMap (4));

//  Map TL;
  Set dom_exptag (exptag.Dom());
  for (bool ff = dom_exptag.First(nm); ff; ff = dom_exptag.Next(nm))
  {
    TYPE_AS_Name nm_ (nm);
    TYPE_AS_Ids ids (nm_.get_ids());
    if (ids.Length() == 2) {
      ids.ImpTl();
      nm_.set_ids(ids);
    }
//    TL.Insert(nm_, exptag[nm]);
    this->LocTagEnv.Insert(nm_, exptag[nm]);
  }
//  this->LocTagEnv = TL;

  bool reswf = true;
  Set dom_tp_qq (tp_qq.Dom());
  Generic tpnm;
  for (bool gg = dom_tp_qq.First(tpnm); gg; gg = dom_tp_qq.Next(tpnm))
  {
    Generic t (tp_qq[tpnm]);
    if (!t.IsNil()) {
      Map m;
      m.Insert(tpnm, t);
      reswf &= wf_TypeDefs(i,m);
    }
  }

  reswf &= wf_ren && ImportValues (i, ren_q, val_qq, expval, parsed, modid, curmod);

  reswf &= ImportFunctions (i, ren_q, fns_qq, expfns, exppolys, parsed, modid, curmod);

  reswf &= ImportOperations (i, ren_q, ops_qq, expops, expfns, parsed, modid, curmod);

  reswf &= ImportTags (exptag, ren_q);

  this->LocTagEnv = Map();

  return reswf;
}

// CheckRename
// is : AS`ImportSig
// modid : AS`Name
// ==> AS`ImportSig * bool * map AS`Name to AS`Name
Tuple StatSem::CheckRename (const TYPE_AS_ImportSig & is, const TYPE_AS_Name & modid)
{
  Bool reswf (true);

  const MAP<TYPE_AS_Name,Generic> & tps (is.GetMap(pos_AS_ImportSig_tps));
  const MAP<TYPE_AS_Name,Generic> & val (is.GetMap(pos_AS_ImportSig_val));
  const MAP<TYPE_AS_Name,Generic> & fns (is.GetMap(pos_AS_ImportSig_fns));
  const MAP<TYPE_AS_Name,Generic> & ops (is.GetMap(pos_AS_ImportSig_ops));
  const MAP<TYPE_AS_Name,TYPE_AS_Name> & ren (is.GetMap(pos_AS_ImportSig_ren));
  const TYPE_CI_ContextId & cid (is.GetInt(pos_AS_ImportSig_cid));

  Map tps_q (tps);
  Map val_q (val);
  Map fns_q (fns);
  Map ops_q (ops);

  SET<TYPE_AS_Name> used;
//  SET<TYPE_AS_Name> ignored;
  MAP<TYPE_AS_Name,TYPE_AS_Name> not_ignored;

  this->Renamed.ImpUnion(ren.Dom());

  Generic nm;
  Set dom_ren (ren.Dom());
  for (bool bb = dom_ren.First(nm); bb; bb = dom_ren.Next(nm))
  {
    if (used.InSet (ren[nm]))
    {
      //-------------------------
      // Error message #73
      // L"%1" is multiple defined
      //-------------------------
      GenErr (nm, ERR, 73, mk_sequence(PrintName (ren[nm])));
      reswf = Bool(false);
//      ignored.Insert (nm);
    }
    else
    {
      used.Insert (nm);
      TYPE_AS_Name fullnm (ExtName (modid, ren[nm]));
      not_ignored.Insert (nm, fullnm);

      if (!CheckName (nm))
      {
        //--------------------------------------------------------------------
        // Error message #66
        // L"%1" cannot be renamed to "%2" because that name is already used in
        // this module
        //--------------------------------------------------------------------
        GenErr(nm, ERR, 66, mk_sequence(PrintName(ren[nm]), PrintName(nm)));
        reswf = Bool(false);
      }
      else if (tps_q.DomExists (fullnm))
      {
        tps_q.ImpModify (nm, tps_q[fullnm]);
        tps_q.RemElem (fullnm);
      }
      else if (val_q.DomExists (fullnm))
      {
        val_q.ImpModify (nm, val_q[fullnm]);
        val_q.RemElem (fullnm);
      }
      else if (fns_q.DomExists (fullnm))
      {
        fns_q.ImpModify (nm, fns_q[fullnm]);
        fns_q.RemElem (fullnm);
      }
      else if (ops_q.DomExists (fullnm))
      {
        ops_q.ImpModify (nm, ops_q[fullnm]);
        ops_q.RemElem (fullnm);
      }
      else
        InternalError (L"CheckRename"); //renamed construct not imported
    }
  }
  return mk_(TYPE_AS_ImportSig().Init(tps_q, val_q, fns_q, ops_q, ren, cid), reswf, not_ignored);
}

// ImportTypes
// i : TYPE`Ind
// ren : (map AS`Name to AS`Name)
// imptps : (map AS`Name to [AS`TypeDef])
// exptps : (map AS`Name to REP`TypeRep)
// modid : AS`Name
// curmod : AS`Name
// ==> bool
bool StatSem::ImportTypes (const Int & /*i*/ ,
                           const MAP<TYPE_AS_Name, TYPE_AS_Name> & ren,
                           const Map & imptps,
                           const MAP<TYPE_AS_Name, TYPE_REP_TypeRep> & exptps,
                           const TYPE_AS_Name & modid,
                           const TYPE_AS_Name & curmod)
{
  bool reswf = true;

  SET<TYPE_AS_Name> dom_imptps (imptps.Dom());
  Generic nm;
  for (bool bb = dom_imptps.First(nm); bb; bb = dom_imptps.Next(nm))
  {
//    UsedName.Insert (nm);

    TYPE_AS_Name realnm (ren.DomExists (nm) ? ExtName(modid, ren[nm]) : ExtName(modid, nm));

    bool exported = (exptps.DomExists (realnm));
    if (!exported)
    {
      //-----------------------------------------------
      // Error message #55
      // The type L"%1" is not exported from module "%2"
      //-----------------------------------------------
      GenErr (realnm, ERR, 55, mk_sequence(PrintName (realnm), PrintName (modid)));
      reswf = false;
    }
// 20140321 -->
    else
    {
      if (nm != realnm) {
        TYPE_SSENV_TypeRepElem tre (mk_SSENV_TypeRepElem(exptps[realnm], Bool(true), Bool(false)));
        this->TypeEnv.ImpModify (realnm, tre);
      }
    }
// <-- 20140321

    if (imptps[nm].IsNil ())
    {
      //TYPE_SSENV_TypeRepElem tre (mk_SSENV_TypeRepElem(mk_REP_TypeParRep(nm), Bool(false), Bool(false)));
// 20151221 -->
      if ( exptps.DomExists(realnm)) {
// <-- 20151221
        TYPE_SSENV_TypeRepElem tre (mk_SSENV_TypeRepElem(exptps[realnm], Bool(false), Bool(false)));
        this->TypeEnv.ImpModify (nm, tre);
      //this->TypeEnv.ImpModify (ExtName(curmod, nm), tre);
// 20151221 -->
      } else {
        // TODO:
      }
// <-- 20151221
    }
    else
    {
      TYPE_AS_TypeDef td (imptps[nm]);
      const TYPE_AS_Type & tp (td.GetRecord(pos_AS_TypeDef_shape));
      const Generic & Invar (td.GetField(pos_AS_TypeDef_Inv));

      TYPE_AS_Type tpnew (tp);
      if (tp.Is(TAG_TYPE_AS_CompositeType)) {
        TYPE_AS_CompositeType ct (tp);
        TYPE_AS_Name name (ct.get_name ());
        if (name.GetSequence(pos_AS_Name_ids).Length () == 1)
          ct.set_name (ExtName(modid, name));
        tpnew = ct;
      }

      TYPE_REP_TypeRep tpr (TransTypeDef (modid, nm, tpnew, Invar));
      if (exported && !(tpr == exptps[realnm]))
      {
        //-------------------------------------------
        // Error message #67
        // Imported type different from exported type
        //-------------------------------------------
        GenErrTp (nm, ERR, 67, tpr, exptps[realnm], Sequence());
        reswf = false;
      }

      //TYPE_REP_TypeRep tr (exported ? RenameTag (exptps[realnm], ren.Inverse())
      TYPE_REP_TypeRep tr (exported ? RenameTag (exptps[realnm], ren)
                                    : TransTypeDef (Nil (), nm, tp, Invar));
      TYPE_SSENV_TypeRepElem trep (mk_SSENV_TypeRepElem(tr, Bool(false), Bool(false)));

      if (!Invar.IsNil ())
      {
        SEQ<TYPE_REP_TypeRep> domtp;
        domtp.ImpAppend (TransType (modid, tp));
        TYPE_REP_TotalFnTypeRep invfn (mk_REP_TotalFnTypeRep(domtp, btp_bool));
        this->FunctionEnv.ImpModify (Inv (nm), mk_SSENV_TypeRepElem(invfn, Bool(true), Bool(true)));
      }

      this->TypeEnv.ImpModify (nm, trep);
      //this->TypeEnv.ImpModify (ExtName(curmod,nm), trep);
    }
  }
  return reswf;
}

// ImportValues
// i : TYPE`Ind
// ren : (map AS`Name to AS`Name)
// impval : (map AS`Name to [AS`Type])
// expval : (map AS`Name to REP`TypeRep)
// parsed : bool
// modid : AS`Name
// curmod : AS`Name
// ==> bool
bool StatSem::ImportValues (const Int & i,
                                 const MAP<TYPE_AS_Name, TYPE_AS_Name> & ren,
                                 const Map & impval,
                                 const MAP<TYPE_AS_Name, TYPE_REP_TypeRep> & expval,
        			 const Bool & parsed,
                                 const TYPE_AS_Name & modid,
                                 const TYPE_AS_Name & curmod)
{
  bool reswf (true);

  SET<TYPE_AS_Name> dom_impval (impval.Dom());
  Generic nm;
  for (bool bb = dom_impval.First(nm); bb; bb = dom_impval.Next(nm))
  {
    this->UsedName.Insert (nm);

    TYPE_AS_Name realnm (ren.DomExists (nm) ? ren[nm] : TYPE_AS_Name(nm));

    int exported = (expval.DomExists (realnm));
    if (!exported && parsed.GetValue ())
    {
      //------------------------------------------------
      // Error message #68
      // The value L"%1" is not exported from module "%2"
      //------------------------------------------------
      GenErr (realnm, ERR, 68, mk_sequence(PrintName (modid), PrintName (realnm)));
      reswf = false;
    }

    if (impval[nm].IsNil ())
    {
      if (exported) {
        this->ConstEnv.ImpModify (nm, mk_SSENV_TypeRepElem(expval[realnm], Bool(true), Bool(true)));
      }
      else {
// 20120118 -->
//        this->ConstEnv.ImpModify (nm, mk_SSENV_TypeRepElem(rep_alltp, Bool(true), Bool(false)));
        this->ConstEnv.ImpModify (nm, mk_SSENV_TypeRepElem(rep_alltp, Bool(true), Bool(true)));
// <--20120118
      }
    }
    else
    {
      if (exported && !(TransType (curmod, impval[nm]) == expval[realnm]))
      {
        //-------------------------------------------
        // Error message #67
        // Imported type different from exported type
        //-------------------------------------------
        GenErrTp (nm, ERR, 67, TransType(Nil(), impval[nm]), expval[realnm], Sequence());
        reswf = false;
      }

      TYPE_REP_TypeRep tp (exported ? expval[realnm] : TransType(Nil (), impval[nm]));

      reswf = wf_Type(i, impval[nm]) && reswf;
      this->ConstEnv.ImpModify (nm, mk_SSENV_TypeRepElem(tp, Bool(false), Bool(true)));
    }
  }
  return reswf;
}

// ImportFunctions
// i : TYPE`Ind
// ren : (map AS`Name to AS`Name)
// impfns : (map AS`Name to [AS`TypeVarList * AS`FnType])
// expfns : (map AS`Name to REP`FnTypeRep)
// polyfns : (map AS`Name to REP`PolyTypeRep)
// parsed : bool
// modid : AS`Name
// curmod : AS`Name
// ==> bool
bool StatSem::ImportFunctions (const Int & i,
                                    const MAP<TYPE_AS_Name, TYPE_AS_Name> & ren,
                                    const Map & impfns,
                                    const MAP<TYPE_AS_Name, TYPE_REP_FnTypeRep> & expfns,
                                    const MAP<TYPE_AS_Name, TYPE_REP_PolyTypeRep> & polyfns,
                                    const Bool & parsed,
                                    const TYPE_AS_Name & modid,
                                    const TYPE_AS_Name & curmod)
{
  bool reswf (true);

  SET<TYPE_AS_Name> dom_impfns (impfns.Dom());
  Generic nm;
  for (bool bb = dom_impfns.First(nm); bb; bb = dom_impfns.Next(nm))
  {
    this->UsedName.Insert (nm);

    TYPE_AS_Name realnm (ren.DomExists (nm) ? ren[nm] : (const TYPE_AS_Name &)nm);

    bool exportedfn = (expfns.DomExists (realnm));
    bool exportedpoly = (polyfns.DomExists (realnm));
    if (!(exportedfn || exportedpoly) && parsed.GetValue ())
    {
      //---------------------------------------------------
      // Error message #69
      // The function L"%1" is not exported from module "%2"
      //---------------------------------------------------
      GenErr (realnm, ERR, 69, mk_sequence(PrintName (realnm), PrintName (modid)));
      reswf = false;
    }

    if (impfns[nm].IsNil ())
    {
      if (exportedfn)
      {
        this->FunctionEnv.ImpModify (nm, mk_SSENV_TypeRepElem(expfns[realnm], Bool(false), Bool(false)));

        if (expfns.DomExists (Pre (realnm)))
        {
          this->FunctionEnv.ImpModify (Pre (nm), mk_SSENV_TypeRepElem(expfns[Pre (realnm)], Bool(true), Bool(true)));
        }
        if (expfns.DomExists (Post (realnm)))
        {
          this->FunctionEnv.ImpModify (Post (nm), mk_SSENV_TypeRepElem(expfns[Post (realnm)], Bool(true), Bool(true)));
        }
      }
      else if (exportedpoly)
      {
        TYPE_REP_PolyTypeRep polyfns_realnm (polyfns[realnm]);
        this->PolyEnv.ImpModify (nm,
                           mk_SSENV_PolyTypeRepElem(polyfns_realnm.get_vars(), polyfns_realnm.get_tp(), Bool(false)));

        if (polyfns.DomExists(Pre (realnm)))
        {
          TYPE_REP_PolyTypeRep polyfns_Pre_realnm (polyfns[Pre (realnm)]);
          this->PolyEnv.ImpModify(Pre (nm),
                  mk_SSENV_PolyTypeRepElem(polyfns_Pre_realnm.get_vars(), polyfns_Pre_realnm.get_tp(), Bool(true)));
        }
        if (polyfns.DomExists (Post (realnm)))
        {
          TYPE_REP_PolyTypeRep polyfns_Post_realnm (polyfns[Post (realnm)]);
          this->PolyEnv.ImpModify(Post (nm),
                  mk_SSENV_PolyTypeRepElem(polyfns_Post_realnm.get_vars(), polyfns_Post_realnm.get_tp(), Bool(true)));
        }
      }
      else
      {
        this->FunctionEnv.ImpModify (nm, mk_SSENV_TypeRepElem(rep_alltp, Bool(false), Bool(false)));
      }
    }
    else if (exportedpoly)
    {
      Map bind;

      SET<TYPE_AS_Name> dom_ren (ren.Dom());
      Generic nmg;
      for (bool cc = dom_ren.First(nmg); cc; cc = dom_ren.Next(nmg))
      {
        bind.ImpModify (nmg, TYPE_AS_TypeName().Init(ren[nmg], NilContextId));
      }

      TYPE_REP_PolyTypeRep polyfns_realnm (polyfns[realnm]);
      TYPE_AS_FnType fntp (((Tuple)(impfns[nm])).GetField (2));
      if (TransType(curmod, InstFnAS(fntp, bind)) != polyfns_realnm.get_tp())
      {
        //---------------------------------------------------------
        // Error message #70
        // Imported function signature different from exported type
        //---------------------------------------------------------
        GenErrTp (nm, ERR, 70, TransType(Nil (), fntp), polyfns_realnm.get_tp(), Sequence());
        reswf = false;
      }

      SEQ<TYPE_AS_TypeVar> tp_l (((Tuple)(impfns[nm])).GetField (1));
      SEQ<TYPE_REP_TypeVarRep> poly_l (polyfns_realnm.get_vars ());
      bool wrong_sig (tp_l.Length() != poly_l.Length ());
      size_t len_tp_l = tp_l.Length();
      for (size_t index = 1; (index <= len_tp_l) && !wrong_sig; index++)
        wrong_sig = (TransType (curmod, InstFnAS(tp_l[index], bind)) != poly_l[index]);
      if (wrong_sig)
      {
        //----------------------------------------------------
        // Error message #94
        // Imported type variable list different from exported
        //----------------------------------------------------
        GenErrTp (nm, ERR, 94,
                  Nil (), // Cannot print TypeVarList
                  Nil (), Sequence());
        reswf = false;
      }

      PushTypeVarContext(polyfns_realnm.get_vars());
      reswf = wf_Type(i, fntp) && reswf;
      PopTypeVarContext();
      this->PolyEnv.ImpModify(nm,
                              mk_SSENV_PolyTypeRepElem(
                                polyfns_realnm.get_vars(),
                                CurMod(REM, curmod, polyfns_realnm.get_tp()),
                                Bool(false)));

      if (polyfns.DomExists (Pre (realnm)))
      {
        TYPE_REP_PolyTypeRep polyfns_Pre_realnm (polyfns[Pre (realnm)]);
        this->PolyEnv.ImpModify(Pre (nm),
                                mk_SSENV_PolyTypeRepElem(
                                  polyfns_realnm.get_vars(),
                                  CurMod(REM, curmod, polyfns_Pre_realnm.get_tp()),
                                  Bool(true)));
      }
      if (polyfns.DomExists (Post (realnm)))
      {
        TYPE_REP_PolyTypeRep polyfns_Post_realnm (polyfns[Post (realnm)]);
        this->PolyEnv.ImpModify(Post (nm),
                                mk_SSENV_PolyTypeRepElem(
                                  polyfns_realnm.get_vars(),
                                  CurMod(REM, curmod, polyfns_Post_realnm.get_tp()),
                                  Bool(true)));
      }
    }
    else
    {
      MAP<TYPE_AS_Name, TYPE_AS_Name> bind;

      SET<TYPE_AS_Name> dom_ren (ren.Dom());
      Generic nmg;
      for (bool dd = dom_ren.First(nmg); dd; dd = dom_ren.Next(nmg))
      {
        bind.ImpModify (nmg, TYPE_AS_TypeName().Init(ren[nmg], NilContextId));
      }

      TYPE_AS_FnType fntp (((Tuple)(impfns[nm])).GetField (2));
      if (exportedfn && !(TransType (curmod, InstFnAS(fntp, bind)) == expfns[realnm]))
      {
        //---------------------------------------------------------
        // Error message #70
        // Imported function signature different from exported type
        //---------------------------------------------------------
        GenErrTp (nm, ERR, 70, TransType(Nil (), fntp), expfns[realnm], Sequence());
        reswf = false;
      }

      TYPE_REP_TypeRep tp (exportedfn ? CurMod (REM, curmod, expfns[realnm]) : TransType(Nil (), fntp));

      reswf = wf_Type(i, fntp) && (reswf);

      this->FunctionEnv.ImpModify (nm, mk_SSENV_TypeRepElem(tp, Bool(false), Bool(false)));

      if (expfns.DomExists (Pre (realnm))) {
        this->FunctionEnv.ImpModify (Pre (nm),
                      mk_SSENV_TypeRepElem(CurMod (REM, curmod, expfns[Pre (realnm)]), Bool(false), Bool(false)));
      }

      if (expfns.DomExists (Post (realnm))) {
        this->FunctionEnv.ImpModify (Post (nm),
                      mk_SSENV_TypeRepElem(CurMod (REM, curmod, expfns[Post (realnm)]), Bool(false), Bool(false)));
      }
    }
  }
  return reswf;
}

// ImportOperations
// i : TYPE`Ind
// ren (map AS`Name to AS`Name)
// impops : (map AS`Name to [AS`OpType])
// expops : (map AS`Name to REP`OpTypeRep)
// expfns (map AS`Name to REP`FnTypeRep)
// parsed : bool
// modid : AS`Name
// curmod : AS`Name
// ==> bool
bool StatSem::ImportOperations (const Int & i,
                                const MAP<TYPE_AS_Name, TYPE_AS_Name> & ren,
                                const Map & impops,
                                const MAP<TYPE_AS_Name, TYPE_REP_OpTypeRep> & expops,
                                const MAP<TYPE_AS_Name, TYPE_REP_FnTypeRep> & expfns,
                                const Bool & parsed,
                                const TYPE_AS_Name & modid,
                                const TYPE_AS_Name & curmod)
{
  bool reswf (true);

  SET<TYPE_AS_Name> dom_impops (impops.Dom());
  Generic nm;
  for (bool bb = dom_impops.First(nm); bb; bb = dom_impops.Next(nm))
  {
    this->UsedName.Insert (nm);

    TYPE_AS_Name realnm (ren.DomExists (nm) ? ren[nm] : TYPE_AS_Name(nm));

    int exported = (expops.DomExists (realnm));
    if (!exported && parsed.GetValue ())
    {
      //----------------------------------------------------
      // Error message #71
      // The operation L"%1" is not exported from module "%2"
      //----------------------------------------------------
      GenErr (realnm, ERR, 71, mk_sequence(PrintName (realnm), PrintName (modid)));
      reswf = false;
    }

    if (impops[nm].IsNil ())
    {
      if (exported) {
        this->OperationEnv.ImpModify (nm,
                 mk_SSENV_OpTypeRepElem(CurMod (REM, curmod, expops[realnm]), Bool(false), Bool(false), Bool(false)));

        if (expfns.DomExists (Pre (realnm))) {
          this->FunctionEnv.ImpModify (Pre (nm),
                        mk_SSENV_TypeRepElem(CurMod (REM, curmod, expfns[Pre (realnm)]), Bool(false), Bool(false)));
        }
        if (expfns.DomExists (Post (realnm))) {
          this->FunctionEnv.ImpModify (Post (nm),
                        mk_SSENV_TypeRepElem(CurMod (REM, curmod, expfns[Post (realnm)]), Bool(false), Bool(false)));
        }
      }
      else {
        this->OperationEnv.ImpModify (nm, mk_SSENV_OpTypeRepElem(rep_alltp, Bool(true), Bool(false), Bool(false)));
      }
    }
    else
    {
      // QUESTION  (52/136)
      if (exported && !(TransType (curmod, impops[nm]) == expops[realnm]))
      {
        //----------------------------------------------------------
        // Error message #72
        // Imported operation signature different from exported type
        //----------------------------------------------------------
        GenErrTp (nm, ERR, 72, TransType(curmod, impops[nm]), expops[realnm], Sequence());
        reswf = false;
      }

      TYPE_REP_TypeRep tp (exported ? CurMod(REM,curmod,expops[realnm]) : TransType(Nil (), impops[nm]));

      reswf = wf_Type(i, impops[nm]) && reswf;

      this->OperationEnv.ImpModify (nm, mk_SSENV_OpTypeRepElem(tp, Bool(false), Bool(false), Bool(false)));

      if (expfns.DomExists (Pre (realnm)))
      {
        this->FunctionEnv.ImpModify (Pre (nm),
                      mk_SSENV_TypeRepElem(CurMod(REM,curmod,expfns[Pre (realnm)]), Bool(false), Bool(false)));
      }
      if (expfns.DomExists (Post (realnm)))
      {
        this->FunctionEnv.ImpModify (Post (nm),
                      mk_SSENV_TypeRepElem(CurMod(REM,curmod,expfns[Post (realnm)]), Bool(false), Bool(false)));
      }
    }
  }
  return reswf;
}

// ImportTags
// exptag : (map AS`Name to seq of REP`FieldRep)
// ren : (map AS`Name to AS`Name)
// ==> bool
bool StatSem::ImportTags (const Map & exptag, const MAP<TYPE_AS_Name, TYPE_AS_Name> & ren)
{
  bool reswf (true);

  MAP<TYPE_AS_Name, TYPE_AS_Name> inv_ren (ren.Inverse());

  SET<TYPE_AS_Name> dom_exptag (exptag.Dom());
  Generic nm;
  for (bool bb = dom_exptag.First(nm); bb; bb = dom_exptag.Next(nm))
  {
    TYPE_AS_Name realnm (inv_ren.DomExists (nm) ? inv_ren[nm] : TYPE_AS_Name(nm));

    if (this->TagEnv.DomExists (realnm))
    {
      //-------------------------
      // Error message #73
      // L"%1" is multiple defined
      //-------------------------
      GenErr (realnm, ERR, 73, mk_sequence(PrintName(realnm)));
      reswf = false;
    }
    else
      this->TagEnv.ImpModify (realnm, exptag[nm]);
  }
  return reswf;
}

// ExpandExport
// es : AS`ExportSig
// ==> bool
bool StatSem::ExpandExport (const TYPE_AS_ExportSig & es)
{
  Map tps (es.get_tps ()); // map Name to [<STRUCT>]
  MAP<TYPE_AS_Name,TYPE_AS_Type> val (es.get_val ());
  Map fns (es.get_fns ()); // map Name to (TypeVarList * FnType)
  MAP<TYPE_AS_Name,TYPE_AS_OpType> ops (es.get_ops ());


  bool reswf (true);

  Generic tp_id;
  for (bool bb = tps.First (tp_id); bb; bb = tps.Next (tp_id))
  {
    if (!(this->TypeEnv.DomExists (tp_id)) || this->Renamed.InSet(tp_id))
    {
      //------------------------------
      // Error message #89
      // L"%1" is not defined in module
      //------------------------------
      GenErr (tp_id, ERR, 89, mk_sequence(PrintName (tp_id)));
      reswf = false;
    }
  }

  Generic val_id;
  for (bool cc = val.First (val_id); cc; cc = val.Next (val_id))
  {
    if (!(this->ValueIds.InSet (val_id)) || this->Renamed.InSet(val_id))
    {
      //------------------------------
      // Error message #89
      // L"%1" is not defined in module
      //------------------------------
      GenErr (val_id, ERR, 89, mk_sequence(PrintName (val_id)));
      reswf = false;
    }
  }

  Generic fn_id;
  for (bool dd = fns.First (fn_id); dd; dd = fns.Next (fn_id))
  {
    if (!(this->FunctionEnv.DomExists (fn_id) || this->PolyEnv.DomExists (fn_id)) || this->Renamed.InSet(fn_id))
    {
        //------------------------------
        // Error message #89
        // L"%1" is not defined in module
        //------------------------------
      GenErr (fn_id, ERR, 89, mk_sequence(PrintName (fn_id)));
      reswf = false;
    }
    else
    {
      SEQ<TYPE_AS_TypeVar> tp_l (((Tuple) (fns[fn_id])).GetSequence (1));
      TYPE_AS_FnType fntp (((Tuple) (fns[fn_id])).GetRecord (2));

      TYPE_REP_TypeRep gr;
      if (this->FunctionEnv.DomExists(fn_id))
        gr = this->FunctionEnv[fn_id].get_tp();
      else
        gr = this->PolyEnv[fn_id].get_tp();

      //TYPE_REP_TypeRep instfn (InstFn (PAR, gr, this->TypeParEnv));
      TYPE_REP_TypeRep instfn (gr);
      if (TransType(Nil (), fntp) != instfn)
      {
        //--------------------------------------
        // Error message #92
        // L"%1" is exported with wrong signature
        //--------------------------------------
        GenErrTp (fn_id, ERR, 92, TransType(Nil (), fntp), gr, mk_sequence(PrintName (fn_id)));
        reswf = false;
      }

      if (this->PolyEnv.DomExists(fn_id))
      {
        SEQ<TYPE_REP_TypeVarRep> poly_l (this->PolyEnv[fn_id].get_poly());
//        gr = PolyEnv[g].get_tp();
        bool wrong_sig (tp_l.Length () != poly_l.Length ());
        size_t len_tp_l = tp_l.Length();
        for (size_t ii = 1; (ii <= len_tp_l) && !wrong_sig; ii++ )
          //wrong_sig = (TransType(Nil (), tp_l[ii]) != InstFn (PAR, poly_l[ii], this->TypeParEnv));
          wrong_sig = (TransType(Nil (), tp_l[ii]) != poly_l[ii]);

        if (wrong_sig)
        {
          //------------------------------------------------------
          // Error message #93
          // Type variable list for L"%1" exported with wrong types
          //------------------------------------------------------
          GenErrTp (fn_id, ERR, 93, Nil (), Nil (), mk_sequence(PrintName (fn_id)));
          reswf = false;
        }
      }
    }
  }

  Generic op_id;
  for (bool ee = ops.First (op_id); ee; ee = ops.Next (op_id))
  {
    if (!(this->OperationEnv.DomExists (op_id)) || this->Renamed.InSet(op_id))
    {
      //------------------------------
      // Error message #89
      // L"%1" is not defined in module
      //------------------------------
      GenErr (op_id, ERR, 89, mk_sequence(PrintName (op_id)));
      reswf = false;
    }
    //else if (TransType(Nil (), ops[op_id]) != InstFn(PAR, this->OperationEnv[op_id].get_tp(), this->TypeParEnv))
    else if (TransType(Nil (), ops[op_id]) != this->OperationEnv[op_id].get_tp())
    {
      //--------------------------------------
      // Error message #92
      // L"%1" is exported with wrong signature
      //--------------------------------------
      GenErrTp (op_id, ERR, 92, TransType(Nil (), ops[op_id]),
                this->OperationEnv[op_id].get_tp(), mk_sequence(PrintName (op_id)));
      reswf = false;
    }
  }
  return reswf;
}
#endif // VDMSL

// ExpandDefinitions
// i : TYPE`Ind
// modid : AS`Name
// defs : AS`Definitions
// ==> bool
bool StatSem::ExpandDefinitions (const Int & i, const TYPE_AS_Name & modid, const TYPE_AS_Definitions & defs)
{
  const MAP<TYPE_AS_Name,TYPE_AS_TypeDef> & tm (defs.GetMap(pos_AS_Definitions_typem));
  const MAP<TYPE_AS_Name,TYPE_AS_FnDef> & fnm (defs.GetMap(pos_AS_Definitions_fnm));
  const MAP<TYPE_AS_Name,TYPE_AS_OpDef> & opm (defs.GetMap(pos_AS_Definitions_opm));
  const SEQ<TYPE_AS_ValueDef> & vm (defs.GetSequence(pos_AS_Definitions_valuem));
#ifdef VDMSL
  const Generic & state (defs.GetField(pos_AS_Definitions_State));        // [ StateDef ]
#endif // VDMSL
#ifdef VDMPP
  const SEQ<TYPE_AS_InstanceVarDef> & instvars (defs.GetSequence(pos_AS_Definitions_instvars));
//  const SEQ<TYPE_AS_SyncDef> & syncs (defs.GetSequence(pos_AS_Definitions_syncs));
#endif // VDMPP

  bool reswf (ExpandTypes (modid, tm));

#ifdef VDMSL
  reswf &= ExpandStateIdentifiers (modid, state);
  reswf &= ExtractOperationSignatures (opm, state);
#endif // VDMSL
#ifdef VDMPP
  reswf &= ExpandInstanceVars (instvars);
  reswf &= ExtractOperationSignatures (opm);
#endif // VDMPP
  reswf &= ExtractFunctionSignatures (fnm);
  reswf &= ExtractValueDefsFirst (i, vm);
#ifdef VDMPP
  reswf &= ExtractOverloadedSignatures(opm, fnm);
  reswf &= ExtractValueDefs (i, vm);
#endif // VDMPP

  return reswf;
}

#ifdef VDMSL
// ExpandStateIdentifiers
// modid : AS`Name
// stdef : [AS`StateDef]
// ==> bool
bool StatSem::ExpandStateIdentifiers (const TYPE_AS_Name & modid, const Generic & stdef)
{
  bool reswf (true);

  if (stdef.IsNil ())
    return true;
  else
  {
    TYPE_AS_StateDef st (stdef);
    const TYPE_AS_CompositeType & ct (st.GetRecord(pos_AS_StateDef_tp));
    const TYPE_AS_Name & tag (ct.GetRecord(pos_AS_CompositeType_name));
    const SEQ<TYPE_AS_Field> & flds (ct.GetSequence(pos_AS_CompositeType_fields)); // seq of AS`Field
    const Generic & Invar (st.GetField(pos_AS_StateDef_Inv));
    const Generic & Initvar (st.GetField(pos_AS_StateDef_Init));

    if (!CheckName (tag))
    {
      //-------------------------------
      // Error message #95
      // State name L"%1" is disregarded
      //-------------------------------
      GenErr (tag, WRN2, 95, mk_sequence(PrintName (tag)));
      reswf = false;
    }
    else
    {
      //TYPE_REP_TypeRep sttprep (TransStateType(GetCurMod(),stdef));
      //TYPE_REP_TypeRep sttprep (TransStateType(modid,stdef));
      TYPE_REP_TypeRep sttprep (TransStateType(Nil(),stdef));
      TYPE_SSENV_TypeRepElem tre (mk_SSENV_TypeRepElem(sttprep, Bool(true), Bool(false)));

      this->TypeEnv.ImpModify (tag, tre);
      this->TagEnv.ImpModify (tag, TransFields (Nil (), flds));
      this->StateEnv.ImpModify (tag,tre);

      SEQ<TYPE_REP_TypeRep> tp_l;
      tp_l.ImpAppend(sttprep);

      if (!Invar.IsNil()) {
        TYPE_REP_TypeRep invfn (mk_REP_TotalFnTypeRep(tp_l, btp_bool));
        this->FunctionEnv.ImpModify (Inv(tag), mk_SSENV_TypeRepElem(invfn, Bool(true), Bool(true)));
      }

      if (!Initvar.IsNil()) {
        TYPE_REP_TypeRep initfn (mk_REP_TotalFnTypeRep(tp_l, btp_bool));
        this->FunctionEnv.ImpModify (Init(tag), mk_SSENV_TypeRepElem(initfn, Bool(true), Bool(true)));
      }
    }

    // flds : seq of AS`Field
    size_t len_flds = flds.Length();
    for (size_t idx = 1; idx <= len_flds; idx++)
    {
      const TYPE_AS_Field & fld (flds[idx]);
      const Generic & nm (fld.GetField(pos_AS_Field_sel));
      const TYPE_AS_Type & tp (fld.GetRecord(pos_AS_Field_type));

      if (!nm.IsNil ())
      {
        if (!CheckName (nm))
        {
          //-------------------------------
          // Error message #95
          // State name L"%1" is disregarded
          //-------------------------------
          GenErr (nm, WRN2, 95, mk_sequence(PrintName (tag)));
          reswf = false;
        }
        else
        {
          TYPE_SSENV_TypeRepElem tre (mk_SSENV_TypeRepElem(TransType(Nil (), tp),
                                             Bool(false), Bool(false)));
          this->StateEnv.ImpModify (nm, tre);
// 20140318 -->
          GetCI().SetTypeInfo(Record(nm).GetInt(pos_AS_Name_cid), TransType(GetCurMod(), tp));
// <-- 20140318
        }
      }
    }
  }
  return reswf;
}
#endif // VDMSL

// UsedStateIds
// ids_org : set of AS`Name
// ==> set of AS`Name
SET<TYPE_AS_Name> StatSem::UsedStateIds(const SET<TYPE_AS_Name> & ids_org)
{
  SET<TYPE_AS_Name> ids (ids_org);
  return ids.ImpIntersect (this->StateEnv.Dom ());
}

// ExpandTypes
// modid : AS`Name
// tp_m: map AS`Name to AS`TypeDef
// ==> bool
bool StatSem::ExpandTypes (const TYPE_AS_Name & modid, const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & tp_m)
{
  bool reswf (true);

  SET<TYPE_AS_Name> dom_tp_m (tp_m.Dom());
  Generic g;
  for (bool bb = dom_tp_m.First(g) ; bb ; bb = dom_tp_m.Next(g))
  {
    TYPE_AS_TypeDef td (tp_m[g]);
    const TYPE_AS_Name & nm (td.GetRecord(pos_AS_TypeDef_nm));
    const TYPE_AS_Type & Tp (td.GetRecord(pos_AS_TypeDef_shape));
    const Generic & Invar (td.GetField(pos_AS_TypeDef_Inv));
    Generic localinv = Nil ();
    if (!CheckName (nm))
    {
      //------------------------------------
      // Error message #97
      // Type definition L"%1" is disregarded
      //------------------------------------
      GenErr(nm, WRN2, 97, mk_sequence(PrintName(nm)));
      reswf = false;
    }
    else
    {
      this->WhileChecking = L"while expanding type " + PrintName(nm).GetString();
      reswf = (ColRecs(Tp)) && reswf;
      localinv = Invar;

      
      TYPE_REP_TypeRep stp (TransTypeDef(Nil(),nm,Tp,localinv));
      this->TypeEnv.ImpModify(nm, mk_SSENV_TypeRepElem(stp, Bool(false), Bool(false)));
      this->TypeEnv.ImpModify(ExtName(modid,nm), mk_SSENV_TypeRepElem(stp, Bool(true), Bool(false)));

      if (!Invar.IsNil())
      {
        SEQ<TYPE_REP_TypeRep> sq;
        sq.ImpAppend (TransType(Nil (), Tp));
        TYPE_REP_TypeRep invfn (mk_REP_TotalFnTypeRep( sq, btp_bool ));
        this->FunctionEnv.ImpModify (Inv (nm), mk_SSENV_TypeRepElem(invfn, Bool(true), Bool(true)));
      }
    }
  }
  return reswf;
}

// ColRecs
// tp : AS`Type
// ==> bool
bool StatSem::ColRecs (const TYPE_AS_Type & tp)
{
  switch (tp.GetTag()) {
    case TAG_TYPE_AS_BooleanType:
    case TAG_TYPE_AS_NumericType:
    case TAG_TYPE_AS_TokenType:
    case TAG_TYPE_AS_CharType:
    case TAG_TYPE_AS_VoidType:
    case TAG_TYPE_AS_QuoteType:
    case TAG_TYPE_AS_TypeName:
    case TAG_TYPE_AS_TypeVar: {
      return true;
    }
    case TAG_TYPE_AS_CompositeType: {
      const SEQ<TYPE_AS_Field> & fld_l (tp.GetSequence(pos_AS_CompositeType_fields));
      bool nested = ExpandCompositeType(tp);
      size_t len_fld_l = fld_l.Length();
      for (size_t idx = 1; (idx <= len_fld_l) && nested; idx++)
        nested = ColRecs (fld_l[idx].GetField(pos_AS_Field_type));
      return nested;
    }
    case TAG_TYPE_AS_UnionType: {
      const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence(pos_AS_UnionType_tps));
      bool nested = true;
      size_t len_tp_l = tp_l.Length();
      for (size_t idx = 1; (idx <= len_tp_l) && nested; idx++)
        nested = ColRecs (tp_l[idx]);
      return nested;
    }
    case TAG_TYPE_AS_ProductType: {
      const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence(pos_AS_ProductType_tps));
      bool nested = true;
      size_t len_tp_l = tp_l.Length();
      for (size_t idx = 1; (idx <= len_tp_l) && nested; idx++)
        nested = ColRecs (tp_l[idx]);
      return nested;
    }
    case TAG_TYPE_AS_BracketedType: {
      return ColRecs(tp.GetRecord(pos_AS_BracketedType_tp));
    }
    case TAG_TYPE_AS_OptionalType: {
      return ColRecs(tp.GetRecord(pos_AS_OptionalType_tp));
    }
    case TAG_TYPE_AS_Set0Type: {
      return ColRecs(tp.GetRecord(pos_AS_Set0Type_elemtp));
    }
    case TAG_TYPE_AS_Set1Type: {
      return ColRecs(tp.GetRecord(pos_AS_Set1Type_elemtp));
    }
    case TAG_TYPE_AS_Seq0Type: {
      return ColRecs(tp.GetRecord(pos_AS_Seq0Type_elemtp));
    }
    case TAG_TYPE_AS_Seq1Type: {
      return ColRecs(tp.GetRecord(pos_AS_Seq1Type_elemtp));
    }
    case TAG_TYPE_AS_GeneralMap0Type: {
      return (ColRecs(tp.GetRecord(pos_AS_GeneralMap0Type_mapdom))) &&
             (ColRecs(tp.GetRecord(pos_AS_GeneralMap0Type_maprng)));
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      return (ColRecs(tp.GetRecord(pos_AS_GeneralMap1Type_mapdom))) &&
             (ColRecs(tp.GetRecord(pos_AS_GeneralMap1Type_maprng)));
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      return (ColRecs(tp.GetRecord(pos_AS_InjectiveMap0Type_mapdom))) &&
             (ColRecs(tp.GetRecord(pos_AS_InjectiveMap0Type_maprng)));
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      return (ColRecs(tp.GetRecord(pos_AS_InjectiveMap1Type_mapdom))) &&
             (ColRecs(tp.GetRecord(pos_AS_InjectiveMap1Type_maprng)));
    }
    case TAG_TYPE_AS_PartialFnType: {
      const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence(pos_AS_PartialFnType_fndom));
      bool nested (ColRecs(tp.GetRecord(pos_AS_PartialFnType_fnrng)));
      size_t len_tp_l = tp_l.Length();
      for (size_t idx = 1; (idx <= len_tp_l) && nested; idx++)
        nested = ColRecs (tp_l[idx]);
      return nested;
    }
    case TAG_TYPE_AS_TotalFnType: {
      const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence(pos_AS_TotalFnType_fndom));
      bool nested (ColRecs(tp.GetRecord(pos_AS_TotalFnType_fnrng)));
      size_t len_tp_l = tp_l.Length();
      for (size_t idx = 1; (idx <= len_tp_l) && nested; idx++)
        nested = ColRecs (tp_l[idx]);
      return nested;
    }
// 20070830
    case TAG_TYPE_AS_OpType: {
      const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence(pos_AS_OpType_opdom));
      bool nested (ColRecs(tp.GetRecord(pos_AS_OpType_oprng)));
      size_t len_tp_l = tp_l.Length();
      for (size_t idx = 1; (idx <= len_tp_l) && nested; idx++)
        nested = ColRecs (tp_l[idx]);
      return nested;
    }
    default: {
      InternalError(L"ColRecs");
      return true;
    }
  }
}

// ExpandCompositeType
// ctp : AS`CompositeType
// ==> bool
bool StatSem::ExpandCompositeType (const TYPE_AS_CompositeType & ctp)
{
  const TYPE_AS_Name & tag (ctp.GetRecord(pos_AS_CompositeType_name));
  const SEQ<TYPE_AS_Field> & flds (ctp.GetSequence(pos_AS_CompositeType_fields));
  SEQ<TYPE_REP_FieldRep> tflds (TransFields(Nil (), flds));

  bool reswf (true);
  if (CheckLocalTag(tag))
  {
    if (!(TagEnv[tag] == tflds))
    {
      //-------------------------
      // Error message #73
      // L"%1" is multiple defined
      //-------------------------
      GenErr(tag, ERR, 73, mk_sequence(PrintName(tag)));
      reswf = false;
    }
  }
  else
  {
    this->TagEnv.Insert(tag, tflds);
//#ifdef VDMSL
//    this->TagEnv.Insert(ExtName(GetCurMod(), tag), tflds);
//#endif // VDMSL
  }
  return reswf;
}

// ExtractFunctionSignatures
// fn_m : map AS`Name to AS`FnDef
// ==> bool
bool StatSem::ExtractFunctionSignatures (const MAP<TYPE_AS_Name,TYPE_AS_FnDef> & fn_m)
{
  bool reswf (true);

  SET<TYPE_AS_Name> dom_fn_m (fn_m.Dom());
  Generic fnnm;
  for (bool bb = dom_fn_m.First(fnnm) ; bb ; bb = dom_fn_m.Next(fnnm))
  {
#ifdef VDMPP
    if (!MANGLE::IsMangled(fnnm))
    {
#endif //VDMPP
      TYPE_AS_FnDef fn (fn_m[fnnm]);
      switch(fn.GetTag()) {
        case TAG_TYPE_AS_ExplFnDef: {
          reswf = (ExtractExplicitFunctionSignature(fn)) && reswf;
          break;
        }
        case TAG_TYPE_AS_ExtExplFnDef: {
          reswf = (ExtractExtExplicitFunctionSignature(fn)) && reswf;
          break;
        }
        case TAG_TYPE_AS_ImplFnDef: {
          reswf = (ExtractImplicitFunctionSignature(fn)) && reswf;
          break;
        }
        default:
          break;
      }
#ifdef VDMPP
    }
#endif //VDMPP
  }
  return reswf;
}

// ExtractExplicitFunctionSignature
// fndef : AS`ExplFnDef
// ==> bool
bool StatSem::ExtractExplicitFunctionSignature (const TYPE_AS_ExplFnDef & fndef)
{
  const TYPE_AS_Name & nm           (fndef.GetRecord(pos_AS_ExplFnDef_nm));
  const SEQ<TYPE_AS_TypeVar> & tv_l (fndef.GetSequence(pos_AS_ExplFnDef_tpparms));
  const TYPE_AS_FnType & tp         (fndef.GetRecord(pos_AS_ExplFnDef_tp));
  const Generic & fnpre             (fndef.GetField(pos_AS_ExplFnDef_fnpre));
  const Generic & fnpost            (fndef.GetField(pos_AS_ExplFnDef_fnpost));
  Bool stat                         (Settings.VDM10() ? Bool(true) : fndef.GetBool(pos_AS_ExplFnDef_stat));

  MarkUsedType (tp);

  if (!CheckName (nm))
  {
    //-------------------------------------
    // Error message #98
    // Ignoring function signature for L"%1"
    //-------------------------------------
    GenErr (nm, WRN2, 98, mk_sequence(PrintName(nm)));
    return false;
  }
  else
  {
    this->WhileChecking = L"while expanding function " + PrintName(nm).GetString();
    TYPE_REP_TypeRep fntp (TransType(Nil (), tp));
    if (!tv_l.IsEmpty ())
    {
      SEQ<TYPE_REP_TypeRep> r_l (TransTypeList (Nil (), tv_l));
      this->PolyEnv.ImpModify (nm, mk_SSENV_PolyTypeRepElem(r_l, fntp, Bool(false)));

      if (!fnpre.IsNil ())
      {
        Tuple infer (MakePolyPreType (r_l, fntp));
        const SEQ<TYPE_REP_TypeVarRep> & tv_l (infer.GetSequence (1));
        const TYPE_REP_FnTypeRep & pfn (infer.GetRecord (2));
        this->PolyEnv.ImpModify (Pre (nm), mk_SSENV_PolyTypeRepElem(tv_l, pfn, Bool(true)));
      }

      if (!fnpost.IsNil ())
      {
        Tuple infer (MakePolyPreType (r_l, fntp));
        const SEQ<TYPE_REP_TypeVarRep> & tv_l (infer.GetSequence (1));
        const TYPE_REP_FnTypeRep & pfn (infer.GetRecord (2));
        this->PolyEnv.ImpModify (Post (nm), mk_SSENV_PolyTypeRepElem(tv_l, pfn, Bool(true)));
      }
    }
    else
    {
      this->FunctionEnv.ImpModify (nm, mk_SSENV_TypeRepElem(fntp, Bool(false), stat));

      if (!fnpre.IsNil())
          this->FunctionEnv.ImpModify (Pre (nm), mk_SSENV_TypeRepElem(MakePreType (fntp), Bool(true), stat));
      if (!fnpost.IsNil())
          this->FunctionEnv.ImpModify (Post (nm), mk_SSENV_TypeRepElem(MakePostType (fntp), Bool(true), stat));
    }
  }
  return true;
}

// ExtractImplicitFunctionSignature
// fndef : AS`ImplFnDef
// ==> bool
bool StatSem::ExtractImplicitFunctionSignature (const TYPE_AS_ImplFnDef & fndef)
{
  const TYPE_AS_Name & nm                 (fndef.GetRecord(pos_AS_ImplFnDef_nm));
  const SEQ<TYPE_AS_TypeVar> & tv_l       (fndef.GetSequence(pos_AS_ImplFnDef_params));
  const SEQ<TYPE_AS_PatTypePair> & partps (fndef.GetSequence(pos_AS_ImplFnDef_partps));
  const SEQ<TYPE_AS_NameType> & resnmtps  (fndef.GetSequence(pos_AS_ImplFnDef_resnmtps));
  const Generic & fnpre                   (fndef.GetField(pos_AS_ImplFnDef_fnpre));
  Bool stat                               (Settings.VDM10() ? Bool (true) : fndef.GetBool(pos_AS_ImplFnDef_stat));

  TYPE_AS_Type fnrng (ConstructImplRngType(resnmtps));
  MarkUsedType (fnrng);

  if (!CheckName (nm))
  {
    //-------------------------------------
    // Error message #98
    // Ignoring function signature for L"%1"
    //-------------------------------------
    GenErr (nm, WRN2, 98, mk_sequence(PrintName(nm)));
    return false;
  }
  else
  {
    this->WhileChecking = L"while expanding function " + PrintName(nm).GetString();

    SEQ<TYPE_AS_Type> domtps (ConstructImplDomType(partps));
    Generic t;
    for (bool bb = domtps.First(t); bb; bb = domtps.Next(t))
      MarkUsedType (t);

    SEQ<TYPE_REP_TypeRep> domtp (TransTypeList (Nil (), domtps));
    TYPE_REP_TypeRep restp (TransType(Nil (), fnrng));

    TYPE_REP_TotalFnTypeRep fnsig (mk_REP_TotalFnTypeRep(domtp, restp));
    TYPE_REP_TotalFnTypeRep fnpresig (mk_REP_TotalFnTypeRep(domtp, btp_bool));
    TYPE_REP_TotalFnTypeRep fnpostsig (mk_REP_TotalFnTypeRep(domtp.ImpAppend (restp), btp_bool));

    if (!tv_l.IsEmpty ())
    {
      SEQ<TYPE_REP_TypeRep> r_l (TransTypeList (Nil (), tv_l));
      this->PolyEnv.ImpModify (nm, mk_SSENV_PolyTypeRepElem(r_l, fnsig, Bool(false)));

      if (!fnpre.IsNil ())
        this->PolyEnv.ImpModify (Pre (nm), mk_SSENV_PolyTypeRepElem(r_l, fnpresig, Bool(true)));

      this->PolyEnv.ImpModify (Post (nm), mk_SSENV_PolyTypeRepElem(r_l, fnpostsig, Bool(true)));
    }
    else
    {
      this->FunctionEnv.Insert (nm, mk_SSENV_TypeRepElem(fnsig, Bool(false), stat));

      if (!fnpre.IsNil())
        this->FunctionEnv.ImpModify (Pre(nm), mk_SSENV_TypeRepElem(fnpresig, Bool(true), stat));

      this->FunctionEnv.ImpModify (Post(nm), mk_SSENV_TypeRepElem(fnpostsig, Bool(true), stat));
    }
  }
  return true;
}

// ExtractExtExplicitFunctionSignature
// fndef : AS`ExtExplFnDef
// ==> bool
bool StatSem::ExtractExtExplicitFunctionSignature (const TYPE_AS_ExtExplFnDef & fndef)
{
  const TYPE_AS_Name & nm                 (fndef.GetRecord(pos_AS_ExtExplFnDef_nm));
  const SEQ<TYPE_AS_TypeVar> & tv_l       (fndef.GetSequence(pos_AS_ExtExplFnDef_params));
  const SEQ<TYPE_AS_PatTypePair> & partps (fndef.GetSequence(pos_AS_ExtExplFnDef_partps));
  const SEQ<TYPE_AS_NameType> & resnmtps  (fndef.GetSequence(pos_AS_ExtExplFnDef_resnmtps));
  const Generic & fnpre                   (fndef.GetField(pos_AS_ExtExplFnDef_fnpre));
  const Generic & fnpost                  (fndef.GetField(pos_AS_ExtExplFnDef_fnpost));
  Bool stat                               (Settings.VDM10() ? Bool(true) : fndef.GetBool(pos_AS_ExtExplFnDef_stat));

  TYPE_AS_Type fnrng (ConstructImplRngType(resnmtps));
  MarkUsedType (fnrng);

  if (!CheckName (nm))
  {
    //-------------------------------------
    // Error message #98
    // Ignoring function signature for L"%1"
    //-------------------------------------
    GenErr (nm, WRN2, 98, mk_sequence(PrintName(nm)));
    return false;
  }
  else
  {
    this->WhileChecking = L"while expanding function " + PrintName(nm).GetString();

    SEQ<TYPE_AS_Type> domtps (ConstructImplDomType(partps));
    Generic t;
    for (bool bb = domtps.First(t); bb; bb = domtps.Next(t))
      MarkUsedType (t);

    SEQ<TYPE_REP_TypeRep> domtp (TransTypeList (Nil (), domtps));
    TYPE_REP_TypeRep restp (TransType(Nil (), fnrng));

    TYPE_REP_TotalFnTypeRep fnsig (mk_REP_TotalFnTypeRep(domtp, restp));
    TYPE_REP_TotalFnTypeRep fnpresig (mk_REP_TotalFnTypeRep(domtp, btp_bool));
    TYPE_REP_TotalFnTypeRep fnpostsig (mk_REP_TotalFnTypeRep(domtp.ImpAppend (restp), btp_bool));

    if (!tv_l.IsEmpty ())
    {
      SEQ<TYPE_REP_TypeRep> r_l (TransTypeList (Nil (), tv_l));
      this->PolyEnv.ImpModify (nm, mk_SSENV_PolyTypeRepElem(r_l, fnsig, Bool(false)));

      if (!fnpre.IsNil ())
        this->PolyEnv.ImpModify (Pre (nm), mk_SSENV_PolyTypeRepElem(r_l, fnpresig, Bool(true)));

      if (!fnpost.IsNil ())
        this->PolyEnv.ImpModify (Post (nm), mk_SSENV_PolyTypeRepElem(r_l, fnpostsig, Bool(true)));
    }
    else
    {
      this->FunctionEnv.Insert (nm, mk_SSENV_TypeRepElem(fnsig, Bool(false), stat));

      if (!fnpre.IsNil())
        this->FunctionEnv.ImpModify (Pre (nm), mk_SSENV_TypeRepElem(fnpresig, Bool(true), stat));

      if (!fnpost.IsNil())
        this->FunctionEnv.ImpModify (Post(nm), mk_SSENV_TypeRepElem(fnpostsig, Bool(true), stat));
    }
  }
  return true;
}

#ifdef VDMSL
// ExtractOperationSignatures
// fn_l : (map AS`Name to AS`OpDef)
// stdef : [AS`StateDef]
// ==> bool
bool StatSem::ExtractOperationSignatures (const MAP<TYPE_AS_Name,TYPE_AS_OpDef> & opdefmap, const Generic & stdef)
#endif // VDMSL
#ifdef VDMPP
// ExtractOperationSignatures
// opdefmap : (map AS`Name to AS`OpDef)
// ==> bool
bool StatSem::ExtractOperationSignatures (const MAP<TYPE_AS_Name,TYPE_AS_OpDef> & opdefmap)
#endif // VDMPP
{
  bool reswf (true);

  SET<TYPE_AS_Name> dom_opdefmap (opdefmap.Dom());
  Generic opnm;
  for (bool bb = dom_opdefmap.First(opnm) ; bb ; bb = dom_opdefmap.Next(opnm))
  {
    TYPE_AS_OpDef op (opdefmap[opnm]);
#ifdef VDMSL
    switch(op.GetTag()) {
      case TAG_TYPE_AS_ExplOpDef: {
        reswf &= ExtractExplicitOperationSignature(op, stdef);
        break;
      }
      case TAG_TYPE_AS_ImplOpDef: {
        reswf &= ExtractImplicitOperationSignature(op, stdef);
        break;
      }
      case TAG_TYPE_AS_ExtExplOpDef: {
        reswf &= ExtractExtExplicitOperationSignature(op, stdef);
        break;
      }
      default:
        break;
    }
#endif // VDMSL
#ifdef VDMPP
    if (!MANGLE::IsMangled(opnm))
    {
      switch(op.GetTag()) {
        case TAG_TYPE_AS_ExplOpDef: {
          reswf &= ExtractExplicitOperationSignature(op);
          break;
        }
        case TAG_TYPE_AS_ImplOpDef: {
          reswf &= ExtractImplicitOperationSignature(op);
          break;
        }
        case TAG_TYPE_AS_ExtExplOpDef: {
          reswf &= ExtractExtExplicitOperationSignature(op);
          break;
        }
        default:
          break;
      }
    }
#endif // VDMPP
  }
  return reswf;
}

#ifdef VDMSL
// ExtractExplicitOperationSignature
// opdef : AS`ExplOpDef
// stdef : [AS`StateDef]
// ==> bool
bool StatSem::ExtractExplicitOperationSignature (const TYPE_AS_ExplOpDef & opdef, const Generic & stdef)
#endif // VDMSL
#ifdef VDMPP
// ExtractExplicitOperationSignature
// opdef : AS`ExplOpDef
// ==> bool
bool StatSem::ExtractExplicitOperationSignature (const TYPE_AS_ExplOpDef & opdef)
#endif // VDMPP
{
  const TYPE_AS_Name & nm   (opdef.GetRecord(pos_AS_ExplOpDef_nm));
  const TYPE_AS_OpType & tp (opdef.GetRecord(pos_AS_ExplOpDef_tp));
//#ifdef VDMSL
  const Generic & fnpre     (opdef.GetField(pos_AS_ExplOpDef_oppre));
  const Generic & fnpost    (opdef.GetField(pos_AS_ExplOpDef_oppost));
//#endif // VDMSL
  const Bool & stat         (opdef.GetBool(pos_AS_ExplOpDef_stat));
  const Bool & oppure       (opdef.GetBool(pos_AS_ExplOpDef_oppure));

  MarkUsedType (tp);

  if (!CheckName (nm))
  {
    //----------------------------------
    // Error message #99
    // Ignoring operation signature L"%1"
    //----------------------------------
    GenErr (nm, WRN2, 99, mk_sequence(PrintName(nm)));
    return false;
  }
  else
  {
    this->WhileChecking = L"while expanding operation " + PrintName(nm).GetString();
    TYPE_REP_OpTypeRep fntp (TransType(Nil (), tp));

    TYPE_SSENV_TypeRepElem tpe (mk_SSENV_OpTypeRepElem(fntp, Bool(false), stat, oppure));
    this->OperationEnv.ImpModify (nm, tpe);
#ifdef VDMSL
    if (!fnpre.IsNil())
    {
      this->FunctionEnv.ImpModify (Pre (nm),
                    mk_SSENV_TypeRepElem(MakeOpPreType (Nil(), fntp, stdef), Bool(true), Bool(true)));
    }

    if (!fnpost.IsNil())
    {
      this->FunctionEnv.ImpModify (Post (nm),
                    mk_SSENV_TypeRepElem(MakeOpPostType (Nil(), fntp, stdef), Bool(true), Bool(true)));
    }
#endif // VDMSL
#ifdef VDMPP
    if (!fnpre.IsNil())
    {
      this->FunctionEnv.ImpModify (Pre (nm),
                    mk_SSENV_TypeRepElem(MakeOpPreType (Nil(), fntp, Nil()), Bool(true), Bool(true)));
    }

    if (!fnpost.IsNil())
    {
      this->FunctionEnv.ImpModify (Post (nm),
                    mk_SSENV_TypeRepElem(MakeOpPostType (Nil(), fntp, Nil()), Bool(true), Bool(true)));
    }
#endif // VDMPP
  }
  return true;
}

#ifdef VDMSL
// ExtractImplicitOperationSignature
// opdef : AS`ImplOpDef
// stdef : [AS`StateDef]
// ==> bool
bool StatSem::ExtractImplicitOperationSignature (const TYPE_AS_ImplOpDef & opdef, const Generic & stdef)
#endif // VDMSL
#ifdef VDMPP
// ExtractImplicitOperationSignature
// opdef : AS`ImplOpDef
// ==> bool
bool StatSem::ExtractImplicitOperationSignature (const TYPE_AS_ImplOpDef & opdef)
#endif // VDMPP
{
  const TYPE_AS_Name & nm                 (opdef.GetRecord(pos_AS_ImplOpDef_nm));
  const SEQ<TYPE_AS_PatTypePair> & partps (opdef.GetSequence(pos_AS_ImplOpDef_partps));
  const SEQ<TYPE_AS_NameType> & resnmtps  (opdef.GetSequence(pos_AS_ImplOpDef_resnmtps));
  const Generic & fnpre                   (opdef.GetField(pos_AS_ImplOpDef_oppre));
  const Bool & stat                       (opdef.GetBool(pos_AS_ImplOpDef_stat));
  const Bool & oppure                     (opdef.GetBool(pos_AS_ImplOpDef_oppure));

  TYPE_AS_Type oprng (ConstructImplRngType(resnmtps));
  MarkUsedType (oprng);

  if (!CheckName (nm))
  {
    //----------------------------------
    // Error message #99
    // Ignoring operation signature L"%1"
    //----------------------------------
    GenErr (nm, WRN2, 99, mk_sequence(PrintName(nm)));
    return false;
  }
  else
  {
    this->WhileChecking = L"while expanding function " + PrintName(nm).GetString();

    SEQ<TYPE_AS_Type> domtps (ConstructImplDomType(partps));
    Generic t;
    for (bool bb = domtps.First(t); bb; bb = domtps.Next(t))
      MarkUsedType (t);

    TYPE_REP_TypeRep rngtp (resnmtps.IsEmpty () ? rep_unittp : TransType(Nil (), oprng));

    SEQ<TYPE_REP_TypeRep> domtp (TransTypeList (Nil (), domtps));

    TYPE_REP_OpTypeRep opsig (mk_REP_OpTypeRep(domtp, rngtp));

    this->OperationEnv.ImpModify (nm, mk_SSENV_OpTypeRepElem( opsig, Bool(false), stat, oppure ));

#ifdef VDMSL
    if (!fnpre.IsNil())
    {
      this->FunctionEnv.ImpModify (Pre(nm),
                    mk_SSENV_TypeRepElem(MakeOpPreType (Nil(), opsig, stdef), Bool(true), Bool(true)));
    }
    this->FunctionEnv.ImpModify (Post(nm),
                  mk_SSENV_TypeRepElem(MakeOpPostType (Nil(), opsig, stdef), Bool(true), Bool(true)));
#endif // VDMSL
#ifdef VDMPP
    if (!fnpre.IsNil())
    {
      this->FunctionEnv.ImpModify (Pre(nm),
                    mk_SSENV_TypeRepElem(MakeOpPreType (Nil(), opsig, Nil()), Bool(true), Bool(true)));
    }
    this->FunctionEnv.ImpModify (Post(nm),
                  mk_SSENV_TypeRepElem(MakeOpPostType (Nil(), opsig, Nil()), Bool(true), Bool(true)));
#endif // VDMPP
  }
  return true;
}

#ifdef VDMSL
// ExtractExtExplicitOperationSignature
// opdef : AS`ExtExplOpDef
// stdef : [AS`StateDef]
// ==> bool
bool StatSem::ExtractExtExplicitOperationSignature (const TYPE_AS_ExtExplOpDef & opdef, const Generic & stdef)
#endif // VDMSL
#ifdef VDMPP
// ExtractExtExplicitOperationSignature
// opdef : AS`ExtExplOpDef
// ==> bool
bool StatSem::ExtractExtExplicitOperationSignature (const TYPE_AS_ExtExplOpDef & opdef)
#endif // VDMPP
{
  const TYPE_AS_Name & nm                 (opdef.GetRecord(pos_AS_ExtExplOpDef_nm));
  const SEQ<TYPE_AS_PatTypePair> & partps (opdef.GetSequence(pos_AS_ExtExplOpDef_partps));
  const SEQ<TYPE_AS_NameType> & resnmtps  (opdef.GetSequence(pos_AS_ExtExplOpDef_resnmtps));
  const Generic & oppre                   (opdef.GetField(pos_AS_ExtExplOpDef_oppre));
  const Generic & oppost                  (opdef.GetField(pos_AS_ExtExplOpDef_oppost));
  const Bool & stat                       (opdef.GetBool(pos_AS_ExtExplOpDef_stat));
  const Bool & oppure                     (opdef.GetBool(pos_AS_ExtExplOpDef_oppure));
//#ifdef VDMPP
//  const Bool & constr                     (opdef.GetBool(pos_AS_ExtExplOpDef_constr));
//#endif // VDMPP

  TYPE_AS_Type oprng (ConstructImplRngType(resnmtps));
  MarkUsedType (oprng);

  if (!CheckName (nm))
  {
    //----------------------------------
    // Error message #99
    // Ignoring operation signature L"%1"
    //----------------------------------
    GenErr (nm, WRN2, 99, mk_sequence(PrintName(nm)));
    return false;
  }
  else
  {
    bool reswf (true);
    this->WhileChecking = L"while expanding function " + PrintName(nm).GetString();

    SEQ<TYPE_AS_Type> domtps (ConstructImplDomType(partps));
    Generic t;
    for (bool bb = domtps.First(t); bb; bb = domtps.Next(t))
      MarkUsedType (t);

    SEQ<TYPE_REP_TypeRep> domtp (TransTypeList (Nil (), domtps));

    TYPE_REP_TypeRep rngtp (resnmtps.IsEmpty () ? rep_unittp : TransType(Nil (), oprng));

    TYPE_REP_OpTypeRep opsig (mk_REP_OpTypeRep( domtp, rngtp ));

    this->OperationEnv.ImpModify (nm, mk_SSENV_OpTypeRepElem(opsig, Bool(false), stat, oppure));

#ifdef VDMSL
    if (!oppre.IsNil()) {
      this->FunctionEnv.ImpModify (Pre(nm),
                    mk_SSENV_TypeRepElem(MakeOpPreType (Nil (), opsig, stdef), Bool(true), Bool(true)));
    }

    if (!oppost.IsNil()) {
      this->FunctionEnv.ImpModify (Post(nm),
                    mk_SSENV_TypeRepElem(MakeOpPostType (Nil (), opsig, stdef), Bool(true), Bool(true)));
    }
#endif // VDMSL
#ifdef VDMPP
    if (!oppre.IsNil()) {
      this->FunctionEnv.ImpModify (Pre(nm),
                    mk_SSENV_TypeRepElem(MakeOpPreType (Nil (), opsig, Nil()), Bool(true), Bool(true)));
    }

    if (!oppost.IsNil()) {
      this->FunctionEnv.ImpModify (Post(nm),
                    mk_SSENV_TypeRepElem(MakeOpPostType (Nil (), opsig, Nil()), Bool(true), Bool(true)));
    }
#endif // VDMPP
    return reswf;
  }
}
#ifdef VDMPP

// ExtractOverloadedSignatures
// p_opm : map AS`Name to AS`OpDef
// p_fnm : map AS`Name to AS`FnDef
// ==> bool
bool StatSem::ExtractOverloadedSignatures(const MAP<TYPE_AS_Name,TYPE_AS_OpDef> & p_opm,
                                          const MAP<TYPE_AS_Name,TYPE_AS_FnDef> & p_fnm)
{
  bool l_reswf = true;

  Set dom_p_opm (p_opm.Dom());
  Generic b_opnm;
  for (bool bb = dom_p_opm.First(b_opnm); bb; bb = dom_p_opm.Next(b_opnm))
  {
    TYPE_AS_OpDef l_opdef (p_opm[b_opnm]);
    if (MANGLE::IsMangled(b_opnm))
    {
      TYPE_AS_Name l_nm (MANGLE::GetUnmangledName(b_opnm));
      Tuple l_opsig (GetOperationSignature(l_opdef)); // (seq of REP`TypeRep) * REP`TypeRep
      const SEQ<TYPE_REP_TypeRep> & l_opdom (l_opsig.GetSequence(1));
      const TYPE_REP_TypeRep & l_oprng (l_opsig.GetRecord(2));
      bool l_static = GetOperationStaticStatus(l_opdef);

      if (this->OverloadedEnv.DomExists(l_nm))
      {
        if (OverloadedTypeConsistent(l_nm, l_opdom, true))
          InsertOverloadedName(l_nm, l_opdom, l_oprng, true, l_static);
        else
          l_reswf = false;
      }
      else
        InsertOverloadedName(l_nm, l_opdom, l_oprng, true, l_static);
    }
  }

  Set dom_p_fnm (p_fnm.Dom());
  Generic b_fnm;
  for (bool dd = dom_p_fnm.First(b_fnm); dd; dd = dom_p_fnm.Next(b_fnm))
  {
    TYPE_AS_FnDef l_fndef (p_fnm[b_fnm]);
    if (MANGLE::IsMangled(b_fnm))
    {
      TYPE_AS_Name l_nm (MANGLE::GetUnmangledName(b_fnm));
      Tuple l_fnsig (GetFunctionSignature(l_fndef));
      const SEQ<TYPE_REP_TypeRep> & l_fndom (l_fnsig.GetSequence(1));
      const TYPE_REP_TypeRep & l_fnrng (l_fnsig.GetRecord(2));
      bool l_static = GetFunctionStaticStatus(l_fndef);

      if (this->OverloadedEnv.DomExists(l_nm))
      {
        if (OverloadedTypeConsistent(l_nm, l_fndom, false))
          InsertOverloadedName(l_nm, l_fndom, l_fnrng, false, l_static);
        else
          l_reswf = false;
      }
      else
        InsertOverloadedName(l_nm, l_fndom, l_fnrng, false, l_static);
    }
  }
  return l_reswf;
}

// OverloadedTypeConsistent
// p_nm : AS`Name
// p_domtp : seq of REP`TypeRep
// p_isop : bool
// ==> bool
bool StatSem::OverloadedTypeConsistent(const TYPE_AS_Name & p_nm,
                                       const SEQ<TYPE_REP_TypeRep> & p_domtp,
                                       bool p_isop)
{
  SET<TYPE_SSENV_TypeRepElem> l_typereps (this->OverloadedEnv[p_nm]);

  Set l_domtps; // set of seq of REP`TypeRep
  Generic b_tp;
  for (bool bb = l_typereps.First(b_tp); bb; bb = l_typereps.Next(b_tp))
  {
    TYPE_SSENV_TypeRepElem l_tre (b_tp); //l_tre:TypeRepElem
    const TYPE_REP_TypeRep & r (l_tre.GetRecord(pos_SSENV_TypeRepElem_tp));
    switch(r.GetTag()) {
      case TAG_TYPE_REP_OpTypeRep: {
        l_domtps.Insert(r.GetSequence(pos_REP_OpTypeRep_Dom));
        break;
      }
      case TAG_TYPE_REP_PartialFnTypeRep: {
        l_domtps.Insert(r.GetSequence(pos_REP_PartialFnTypeRep_fndom));
        break;
      }
      case TAG_TYPE_REP_TotalFnTypeRep: {
        l_domtps.Insert(r.GetSequence(pos_REP_TotalFnTypeRep_fndom));
        break;
      }
      default: {
        break;
      }
    }
  }

  bool l_reswf = true;
  Generic b_tpseqg;
  for (bool cc = l_domtps.First(b_tpseqg); cc; cc = l_domtps.Next(b_tpseqg))
  {
    SEQ<TYPE_REP_TypeRep> b_tpseq (b_tpseqg);
    bool forall = (b_tpseq.Length() == p_domtp.Length());
    size_t len_b_tpseq = b_tpseq.Length();
    for (size_t i = 1; (i <= len_b_tpseq) && forall; i++)
    {
      forall = IsOverlapping(b_tpseq[i], p_domtp[i], Set());
    }
    if (forall)
    {
      // ---------------------------------
      // -- Error message #382/#384
      // -- Parameter types of overloaded operations/functions overlap
      // ---------------------------------
      GenErr (p_nm, ERR, p_isop ? 382 : 384, Sequence());
      l_reswf = false;
    }
  }
  return l_reswf;
}

// InsertOverloadedName
// p_nm : AS`Name
// p_domtps : seq of REP`TypeRep
// p_rngtp : REP`TypeRep
// p_isop : bool
// p_static : bool
void StatSem::InsertOverloadedName(const TYPE_AS_Name & p_nm,
                                   const SEQ<TYPE_REP_TypeRep> & p_domtps,
                                   const TYPE_REP_TypeRep & p_rngtp,
                                   bool p_isop,
	                           bool p_static)
{
  SEQ<TYPE_REP_TypeRep> l_domtps (NormaliseTypeNameList(p_domtps));
  TYPE_REP_TypeRep l_rngtp (NormaliseTypeName(p_rngtp));

  TYPE_REP_TypeRep l_methtp;
  if (p_isop)
    l_methtp = mk_REP_OpTypeRep(l_domtps, l_rngtp);
  else
    l_methtp = mk_REP_TotalFnTypeRep(l_domtps, l_rngtp);

  TYPE_SSENV_TypeRepElem l_tre (mk_SSENV_TypeRepElem(l_methtp, Bool(false), Bool(p_static)));

  if (this->OverloadedEnv.DomExists(p_nm))
  {
    Set oe_pnm (this->OverloadedEnv[p_nm]);
    oe_pnm.Insert(l_tre);
    this->OverloadedEnv.ImpModify(p_nm, oe_pnm);
  }
  else
    this->OverloadedEnv.Insert(p_nm, Set().Insert(l_tre));
}

// NormaliseTypeNameList
// p_tpL : seq of REP`TypeRep
// ==> seq of REP`TypeRep
SEQ<TYPE_REP_TypeRep> StatSem::NormaliseTypeNameList(const SEQ<TYPE_REP_TypeRep> & p_tpL)
{
  SEQ<TYPE_REP_TypeRep> l_tpL;
  size_t len_p_tpL = p_tpL.Length();
  for (size_t idx = 1; idx <= len_p_tpL; idx++)
    l_tpL.ImpAppend (NormaliseTypeName(p_tpL[idx]));

  return l_tpL;
}

// NormaliseAccessTypeSet
// p_acctpS : set of AccessType
// ==> set of AccessType
SET<TYPE_SSENV_AccessType> StatSem::NormaliseAccessTypeSet(const SET<TYPE_SSENV_AccessType> & p_acctpS)
{
  SET<TYPE_SSENV_AccessType> p_acctpS_q (p_acctpS);
  SET<TYPE_SSENV_AccessType> l_acctpS;
  Generic l_acctp;
  for (bool bb = p_acctpS_q.First(l_acctp); bb; bb = p_acctpS_q.Next(l_acctp))
    l_acctpS.Insert (NormaliseAccessType(l_acctp));

  return l_acctpS;
}

// NormaliseAccessType
// p_acctpS : AccessType
// ==> AccessType
TYPE_SSENV_AccessType StatSem::NormaliseAccessType(const TYPE_SSENV_AccessType & p_acctp)
{
  TYPE_SSENV_AccessType l_acctp (p_acctp);

  l_acctp.SetField(pos_SSENV_AccessTypeRep_tp, NormaliseTypeName(StripAccessType(p_acctp)));

  return l_acctp;
}

// NormaliseTypeName
// p_tp : REP`TypeRep
// ==> REP`TypeRep
TYPE_REP_TypeRep StatSem::NormaliseTypeName(const TYPE_REP_TypeRep & p_tp)
{
  switch (p_tp.GetTag()) {
    case TAG_TYPE_REP_TypeNameRep: {
      const TYPE_AS_Name & nm (p_tp.GetRecord(pos_REP_TypeNameRep_nm));
  
      if (nm.GetSequence(pos_AS_Name_ids).Length() == 2)
      {
        TYPE_AS_Name l_clnm (ASTAUX::GetFirstName(nm));
        TYPE_AS_Name l_elmnm (ASTAUX::GetSecondName(nm));
  
        if (IsClassName(l_elmnm)) {
  
          Generic l_pte (GetClassTypeRep(l_clnm));
          Map l_tps;
  
          if (!l_pte.IsNil())
          {
            const TYPE_SSENV_ParseTypeInfo & pti = l_pte;
            l_tps = pti.get_tps();
          }
  
          if (!l_tps.DomExists(l_elmnm)) {	   	
            return mk_REP_TypeNameRep(l_elmnm);
          }
        }
// 20061213 for inherit types (not in spec)
        else
        {
          // check inherited types
          Map sup_tpm (LookUpInHierarchyAux(l_elmnm, l_clnm, TYPE, LOCAL, Set()));
          if( sup_tpm.Size() == 1 )
          {
            TYPE_AS_Name sup_nm (sup_tpm.Dom().GetElem());
            TYPE_AS_Ids new_ids (sup_nm.get_ids());
            new_ids.ImpConc(l_elmnm.get_ids());
            TYPE_AS_Name newnm (nm);
            newnm.set_ids(new_ids);
            return mk_REP_TypeNameRep(newnm);
          }
        }
//
      }
      return p_tp;
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
      const SEQ<TYPE_REP_FieldRep> & fields (p_tp.GetSequence(pos_REP_CompositeTypeRep_fields));
      SEQ<TYPE_REP_FieldRep> l_fields;
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++)
      {
        const TYPE_REP_FieldRep & fld (fields[idx]);
        l_fields.ImpAppend (mk_REP_FieldRep(fld.GetField(pos_REP_FieldRep_sel),
                                            NormaliseTypeName(fld.GetRecord(pos_REP_FieldRep_tp)),
                                            fld.GetBool(pos_REP_FieldRep_dc)));
      }
      return mk_REP_CompositeTypeRep(p_tp.GetRecord(pos_REP_CompositeTypeRep_nm), l_fields);
                     
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (p_tp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> l_tps;
      Generic g;
      for (bool bb = tp_s.First(g); bb; bb = tp_s.Next(g)) {
        l_tps.Insert (NormaliseTypeName(g));
      }
      return mk_REP_UnionTypeRep(l_tps);
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      return mk_REP_ProductTypeRep(NormaliseTypeNameList(p_tp.GetSequence(pos_REP_ProductTypeRep_tps)));
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return mk_REP_InvTypeRep(p_tp.GetRecord(pos_REP_InvTypeRep_name),
                               NormaliseTypeName(p_tp.GetRecord(pos_REP_InvTypeRep_shape)),
                               p_tp.GetRecord(pos_REP_InvTypeRep_invariant));
    }
#ifdef VICE
    case TAG_TYPE_REP_ObjRefTypeRep: {
      TYPE_AS_Name nm (p_tp.GetRecord(pos_REP_ObjRefTypeRep_nm));
      if (nm.GetSequence(pos_AS_Name_ids).IsEmpty())
        return mk_REP_TypeNameRep(nm);
      else
        return p_tp;
    }
#endif // VICE
    case TAG_TYPE_REP_SetTypeRep: {
      return mk_REP_SetTypeRep(NormaliseTypeName(p_tp.GetRecord(pos_REP_SetTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_EmptySetTypeRep: {
      return mk_REP_EmptySetTypeRep(NormaliseTypeName(p_tp.GetRecord(pos_REP_EmptySetTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return mk_REP_SeqTypeRep(NormaliseTypeName(p_tp.GetRecord(pos_REP_SeqTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_EmptySeqTypeRep: {
      return mk_REP_EmptySeqTypeRep(NormaliseTypeName(p_tp.GetRecord(pos_REP_EmptySeqTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return mk_REP_GeneralMapTypeRep(NormaliseTypeName(p_tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom)),
                                      NormaliseTypeName(p_tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng)));
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return mk_REP_InjectiveMapTypeRep(NormaliseTypeName(p_tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom)),
                                        NormaliseTypeName(p_tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng)));
    }
    case TAG_TYPE_REP_EmptyMapTypeRep: {
      return mk_REP_EmptyMapTypeRep(NormaliseTypeName(p_tp.GetRecord(pos_REP_EmptyMapTypeRep_mapdom)),
                                    NormaliseTypeName(p_tp.GetRecord(pos_REP_EmptyMapTypeRep_maprng)));
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      return mk_REP_PartialFnTypeRep(NormaliseTypeNameList(p_tp.GetSequence(pos_REP_PartialFnTypeRep_fndom)),
                                     NormaliseTypeName(p_tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng)));
    }
    case TAG_TYPE_REP_TotalFnTypeRep: {
      return mk_REP_TotalFnTypeRep(NormaliseTypeNameList(p_tp.GetSequence(pos_REP_TotalFnTypeRep_fndom)),
                                   NormaliseTypeName(p_tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng)));
    }
    case TAG_TYPE_REP_OpTypeRep: {
#ifdef VICE
      const SEQ<TYPE_REP_TypeRep> & dom (p_tp.GetSequence(pos_REP_OpTypeRep_Dom));
      const TYPE_REP_TypeRep & rng (p_tp.GetRecord(pos_REP_OpTypeRep_Rng));

      if ((dom.Length() == 2) &&
          //(dom[1] == TYPE_REP_AnyOpTypeRep()) &&
          (dom[1].Is(TAG_TYPE_REP_AnyOpTypeRep)) &&
          (dom[2] == btp_nat) &&
          //(rng == TYPE_REP_UnitTypeRep()))
          (rng.Is(TAG_TYPE_REP_UnitTypeRep)))
        {
          SEQ<TYPE_REP_TypeRep> l_dtp;
          l_dtp.ImpAppend(mk_REP_TypeNameRep(ASTAUX::MkNameFromId(ASTAUX::MkId(L"top"), NilContextId)));
          l_dtp.ImpAppend(btp_nat); 
          return mk_REP_OpTypeRep(l_dtp, rng);
        }
#endif // VICE
      return mk_REP_OpTypeRep(NormaliseTypeNameList(p_tp.GetSequence(pos_REP_OpTypeRep_Dom)),
                              NormaliseTypeName(p_tp.GetRecord(pos_REP_OpTypeRep_Rng))); 
    }
    case TAG_TYPE_REP_PolyTypeRep: {
      return mk_REP_PolyTypeRep(p_tp.GetSequence(pos_REP_PolyTypeRep_vars),
                                NormaliseTypeName(p_tp.GetRecord(pos_REP_PolyTypeRep_tp)));
    }
    default: {
      return p_tp;
    }
  }
//  return p_tp;
}

// GetOperationSignature
// opdef : AS`OpDef
// -> (seq of REP`TypeRep) * REP`TypeRep
Tuple StatSem::GetOperationSignature(const TYPE_AS_OpDef & opdef)
{
  switch(opdef.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef: {
      const TYPE_AS_OpType & tp (opdef.GetRecord(pos_AS_ExplOpDef_tp));
      TYPE_REP_TypeRep rtp (TransType(Nil(), tp.GetRecord(pos_AS_OpType_oprng)));
      return mk_(TransTypeList(Nil(), tp.GetSequence(pos_AS_OpType_opdom)), rtp);
    }
    case TAG_TYPE_AS_ImplOpDef: {
      const SEQ<TYPE_AS_NameType> & resnmtps (opdef.GetSequence(pos_AS_ImplOpDef_resnmtps));
      TYPE_REP_TypeRep rtp (TransType(Nil(), ConstructImplRngType(resnmtps)));
      return mk_(TransTypeList(Nil(), ConstructImplDomType(opdef.GetSequence(pos_AS_ImplOpDef_partps))), rtp);
    }
    case TAG_TYPE_AS_ExtExplOpDef: {
      const SEQ<TYPE_AS_NameType> & resnmtps (opdef.GetSequence(pos_AS_ExtExplOpDef_resnmtps));
      TYPE_REP_TypeRep rtp (TransType(Nil(), ConstructImplRngType(resnmtps)));
      return mk_(TransTypeList(Nil(), ConstructImplDomType(opdef.GetSequence(pos_AS_ExtExplOpDef_partps))), rtp);
    }
    default: {
      InternalError (L"GetOperationSignature");
      return Tuple(); // dummy
    }
  }
}

// GetFunctionSignature
// fndef : AS`FnDef
// -> (seq of REP`TypeRep) * REP`TypeRep
Tuple StatSem::GetFunctionSignature(const TYPE_AS_FnDef & fndef)
{
  switch(fndef.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef: {
      return mk_(TransTypeList(Nil(), fndef.GetRecord(pos_AS_ExplFnDef_tp).GetSequence(1)),
                 TransType(Nil(), fndef.GetRecord(pos_AS_ExplFnDef_tp).GetRecord(2)));
    }
    case TAG_TYPE_AS_ImplFnDef: {
      return mk_(TransTypeList(Nil(), ConstructImplDomType(fndef.GetSequence(pos_AS_ImplFnDef_partps))),
                 TransType(Nil(), ConstructImplRngType(fndef.GetSequence(pos_AS_ImplFnDef_resnmtps))));
    }
    case TAG_TYPE_AS_ExtExplFnDef: {
      return mk_(TransTypeList(Nil(), ConstructImplDomType(fndef.GetSequence(pos_AS_ExtExplFnDef_partps))),
                 TransType(Nil(), ConstructImplRngType(fndef.GetSequence(pos_AS_ExtExplFnDef_resnmtps))));
    }
    default: {
      InternalError (L"GetFunctionSignature");
      return Tuple(); // dummy
    }
  }
}

// GetOperationStaticStatus
// opdef : AS`OpDef
// -> bool
bool StatSem::GetOperationStaticStatus(const TYPE_AS_OpDef & opdef)
{
  switch(opdef.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef: {
      return opdef.GetBool(pos_AS_ExplOpDef_stat).GetValue();
    }
    case TAG_TYPE_AS_ImplOpDef: {
      return opdef.GetBool(pos_AS_ImplOpDef_stat).GetValue();
    }
    case TAG_TYPE_AS_ExtExplOpDef: {
      return opdef.GetBool(pos_AS_ExtExplOpDef_stat).GetValue();
    }
    default:
      return false; // dummy
  }
}

// GetFunctionStaticStatus
// fndef : AS`FnDef
// -> bool
bool StatSem::GetFunctionStaticStatus(const TYPE_AS_FnDef & fndef)
{
  switch(fndef.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef: {
      return (Settings.VDM10() ? true : fndef.GetBool(pos_AS_ExplFnDef_stat).GetValue());
    }
    case TAG_TYPE_AS_ImplFnDef: {
      return (Settings.VDM10() ? true : fndef.GetBool(pos_AS_ImplFnDef_stat).GetValue());
    }
    case TAG_TYPE_AS_ExtExplFnDef: {
      return (Settings.VDM10() ? true : fndef.GetBool(pos_AS_ExtExplFnDef_stat).GetValue());
    }
    default:
      return false; // dummy
  }
}

// ExpandInstanceVars
// inst_l : seq of AS`InstanceVarDef
// ==> bool
bool StatSem::ExpandInstanceVars (const SEQ<TYPE_AS_InstanceVarDef> & inst_l)
{
  bool reswf(true);

  size_t len_inst_l = inst_l.Length();
  for (size_t idx = 1; idx <= len_inst_l; idx++)
  {
    const TYPE_AS_InstanceVarDef & ivd (inst_l[idx]);
    if (ivd.Is (TAG_TYPE_AS_InstAssignDef))
    {
      const TYPE_AS_AssignDef & ad (ivd.GetRecord(pos_AS_InstAssignDef_ad));
      const TYPE_AS_Name & nm (ad.GetRecord(pos_AS_AssignDef_var));
      if (!CheckName (nm))
      {
        //-------------------------------
        // Error message #95
        // State name L"%1" is disregarded
        //-------------------------------
        GenErr(nm, WRN2, 95, mk_sequence(PrintName(nm)));
        reswf = false;
      }
      else
      {
        const TYPE_AS_Type & tp (ad.GetRecord(pos_AS_AssignDef_tp));
        const Bool & stat (ivd.GetBool(pos_AS_InstAssignDef_stat));

        this->WhileChecking = L"while expanding instance variable " + PrintName(nm).GetString();
        this->StateEnv.Insert(nm, mk_SSENV_TypeRepElem(TransType(Nil(),tp), Bool(false), stat));
      }
    }
  }
  return reswf;
}
#endif //VDMPP

// ExtractValueDefsFirst
// i : TYPE`Ind
// vals : seq of AS`ValueDef
// ==> bool
bool StatSem::ExtractValueDefsFirst (const Int & i, const SEQ<TYPE_AS_ValueDef> & vals)
{
// TODO : memory leak here

  //this->ValueIds = SET<TYPE_AS_Name>();
  this->ValueIds.Clear();

  size_t len_vals = vals.Length();
  for (size_t j = 1; j <= len_vals; j++)
  {
    this->ValueIds.ImpUnion (ExtractPatternName (vals[j].GetRecord(pos_AS_ValueDef_pat)).Dom());
  }

  bool reswf (true);
  for (size_t k = 1; k <= len_vals; k++)
  {
    const TYPE_AS_ValueDef & vd (vals[k]);
    const Generic & tp (vd.GetField(pos_AS_ValueDef_tp));

    if (!tp.IsNil ())
    {
      const TYPE_AS_Pattern & pat   (vd.GetRecord(pos_AS_ValueDef_pat));
      const Bool & stat             (vd.GetBool(pos_AS_ValueDef_stat));
#ifdef VDMPP
      const TYPE_AS_Access & access (vd.GetField(pos_AS_ValueDef_access));
#endif // VDMPP

      Tuple infer (wf_Pattern (i, pat, TransType(Nil (), tp)));
      bool wf_def (infer.GetField (1) == Bool(true)); // [bool]
      const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap (2));

      reswf = reswf && (wf_def);

      Set dom_bd (bd.Dom());
      Generic nm;
      for (bool dd = dom_bd.First(nm); dd; dd = dom_bd.Next(nm))
      {
#ifdef VDMPP
// 20101004 -->
        //reswf = CheckDefAccess(access, nm) && reswf;
        reswf = CheckDefAccess(MakeTpRng(bd[nm].GetRecord(1), access, Bool(true)), nm) && reswf;
// <-- 20101004
#endif // VDMPP
        Bool used (this->ConstEnv.DomExists (nm)
                     ? this->ConstEnv[nm].GetBool(pos_SSENV_TypeRepElem_used)
                     : Bool(false));
        this->ConstEnv.ImpModify (nm, mk_SSENV_TypeRepElem(bd[nm].GetRecord(1), used, stat));
      }
    }
  }

  if (!this->ValueIds.IsEmpty())
  {
    Set ids (this->ValueIds);
    Generic nm;
    for (bool ee = ids.First(nm); ee; ee = ids.Next(nm))
      reswf &= CheckName (nm);
  }

  return reswf;
}

// ExtractValueDefs
// i : TYPE`Ind
// vals : seq of AS`ValueDef
// ==> bool
bool StatSem::ExtractValueDefs (const Int & i, const SEQ<TYPE_AS_ValueDef> & vals)
{
  Tuple infer (ExtractPatterns (i, vals));
  Bool reswf (infer.GetBool (1)); // not const &
  const Map & valbds (infer.GetMap (2)); // map AS`Name to (REP`TypeRep * bool) // not ref

  Set dom_valbds (valbds.Dom());
  Generic nm;
  for (bool bb = dom_valbds.First (nm); bb; bb = dom_valbds.Next (nm))
  {
    Tuple t (valbds[nm]); // (REP`TypeRep * bool)
    this->ConstEnv.ImpModify (nm, mk_SSENV_TypeRepElem(t.GetRecord(1), Bool(false), t.GetBool(2)));
  }

  size_t len_vals = vals.Length();
  for (size_t j = 1; j <= len_vals; j++)
  {
    Tuple infer2 (wf_ValueDef (i, vals[j]));
    const Bool & wf_def (infer2.GetBool (1));
    const MAP<TYPE_AS_Name,Tuple> & bd (infer2.GetMap (2));

    reswf = wf_def && reswf;

    Set dom_bd (bd.Dom());
    Generic nm;
    for (bool dd = dom_bd.First (nm); dd; dd = dom_bd.Next (nm))
    {
      if (this->ConstEnv.DomExists (nm))
      {
        TYPE_SSENV_TypeRepElem tre (this->ConstEnv[nm]);
        this->ConstEnv.ImpModify (nm, mk_SSENV_TypeRepElem(bd[nm].GetRecord(1), tre.get_used (), tre.get_stat ()));
      }
      else {
        this->ConstEnv.ImpModify (nm, mk_SSENV_TypeRepElem(bd[nm].GetRecord(1), Bool(false), Bool(false)));
      }
    }
  }
  return reswf.GetValue();
}

// ExtractPatterns
// i : TYPE`Ind
// vals : seq of (AS`ValueDef | AS`FnDef)
// ==> bool * map AS`Name to (REP`TypeRep * bool) renamed ExtractPatterns
Tuple StatSem::ExtractPatterns (const Int & i, const SEQ<TYPE_AS_LocalDef> & vals)
{
  Map valbds;         // map AS`Name to (REP`TypeRep * bool)
  Set valNames;       // set of AS`Name

  Bool reswf (true);
  size_t len = vals.Length();
  for (size_t index = 1; index <= len; index++)
  {
    TYPE_AS_LocalDef def (vals[index]);

    switch(def.GetTag()) {
      case TAG_TYPE_AS_ValueDef: {
        const TYPE_AS_Pattern & pat (def.GetRecord(pos_AS_ValueDef_pat));
        const Generic & tp          (def.GetField(pos_AS_ValueDef_tp));
        const Bool & stat           (def.GetBool(pos_AS_ValueDef_stat));
  
        SET<TYPE_AS_Name> newids (ExtractPatternName (pat).Dom());
  
        Generic id;
        for (bool bb = newids.First (id); bb; bb = newids.Next (id))
        {
          if (valbds.DomExists (id))
          {
            //-------------------------------------------------------
            // Error message #100
            // L"%1" is multiple defined, and must have the same value
            //-------------------------------------------------------
            GenErr (id, WRN1, 100, mk_sequence(PrintName (id)));
          }
        }
  
        if (!tp.IsNil ())
        {
          bool wftp (wf_Type (i, tp));
  
          reswf &= wftp;
  
          MarkUsedType (tp);
          Tuple infer (wf_Pattern (i, pat, TransType(Nil (), tp)));
          Bool wf (infer.GetField (1) == Bool (true)); // [bool]
          const MAP<TYPE_AS_Name,Tuple> & bd (infer.GetMap (2));
  
          if (!wf)
          {
            //-------------------------------------
            // Error message #101
            // Rhs of value definition cannot match
            //-------------------------------------
            GenErr (pat, ERR, 101, Sequence());
            reswf = Bool(false);
          }
  
          SET<TYPE_AS_Name> st (newids);
          for (bool cc = st.First (id); cc; cc = st.Next (id))
          {
            TYPE_REP_TypeRep tp (bd[id].GetRecord(1));
            if (valbds.DomExists (id))
            {
              Tuple t (valbds[id]); // REP`TypeRep * bool
              if (t.GetRecord(1).Is(TAG_TYPE_REP_TmpTypeRep))
              {
                valbds.ImpModify (id, mk_(tp, stat));
                valNames.Insert(id);
              }
              else
              {
                if (t.GetRecord(1) != tp)
                {
                  //----------------------------------------
                  // Error message #102
                  // L"%1" is assigned to two different types
                  //----------------------------------------
                  GenErrTp (id, ERR, 102, t.GetRecord(1), tp, mk_sequence(PrintName (id)));
                  reswf = Bool(false);
                }
              }
            }
            else
            {
              valbds.ImpModify (id, mk_(bd[id].GetRecord(1), stat));
              valNames.Insert(id);
            }
          }
        }
        else
        {
          for (bool dd = newids.First (id); dd; dd = newids.Next (id))
          {
            if (!valbds.DomExists (id))
            {
              valbds.ImpModify (id, mk_(rep_tmptp, stat));
              valNames.Insert(id);
            }
          }
        }
        break;
      } // End of Is(ValueDef)
      case TAG_TYPE_AS_ExplFnDef: {
        const TYPE_AS_Name & nm   (def.GetRecord  (pos_AS_ExplFnDef_nm));
        const Sequence & tv_l     (def.GetSequence(pos_AS_ExplFnDef_tpparms));
        const TYPE_AS_FnType & tp (def.GetRecord  (pos_AS_ExplFnDef_tp));
        const Generic & fnpre     (def.GetField   (pos_AS_ExplFnDef_fnpre));
        const Generic & fnpost    (def.GetField   (pos_AS_ExplFnDef_fnpost));
        Bool stat                 (Settings.VDM10() ? Bool(true) : def.GetBool (pos_AS_ExplFnDef_stat));
  
        TYPE_REP_TypeRep fntp (TransType(Nil (), tp));
  
        if (valbds.DomExists (nm))
        {
          //--------------------------------------------
          // Error message #103
          // L"%1" cannot be used in multiple definitions
          //--------------------------------------------
          GenErr (nm, ERR, 103, mk_sequence(PrintName (nm)));
          reswf = Bool(false);
        }
        else if (!tv_l.IsEmpty ())
        {
          SEQ<TYPE_REP_TypeRep> r_l (TransTypeList (Nil (), tv_l));
          valbds.ImpModify (nm, mk_(mk_REP_PolyTypeRep(r_l, fntp), stat));
          valNames.Insert(nm);
  
          if (!fnpre.IsNil ())
          {
            Tuple infer (MakePolyPreType (r_l, fntp)); // seq of REP`TypeVarRep * REP`FnTypeRep
            valbds.ImpModify (Pre (nm), mk_(mk_REP_PolyTypeRep(infer.GetSequence (1), infer.GetRecord (2)), stat));
          }
  
          if (!fnpost.IsNil ())
          {
            Tuple infer (MakePolyPostType (r_l, fntp));
            valbds.ImpModify (Post (nm), mk_(mk_REP_PolyTypeRep(infer.GetSequence (1), infer.GetRecord (2)), stat));
          }
        }
        else
        {
          valNames.Insert(nm);
          valbds.ImpModify (nm, mk_(fntp, stat));
          if (!fnpre.IsNil ())
            valbds.ImpModify (Pre (nm), mk_(MakePreType (fntp), stat));
  
          if (!fnpost.IsNil ())
            valbds.ImpModify (Post (nm), mk_(MakePostType (fntp), stat));
        }
        break;
      } // End of Is(ExplFnDef)
      case TAG_TYPE_AS_ImplFnDef: {
        const TYPE_AS_Name & nm   (def.GetRecord  (pos_AS_ImplFnDef_nm));
        const Sequence & tv_l     (def.GetSequence(pos_AS_ImplFnDef_params));
        const Sequence & partps   (def.GetSequence(pos_AS_ImplFnDef_partps));
        const Sequence & resnmtps (def.GetSequence(pos_AS_ImplFnDef_resnmtps));
        const Generic & fnpre     (def.GetField   (pos_AS_ImplFnDef_fnpre));
        Bool stat                 (Settings.VDM10() ? Bool(true) : def.GetBool (pos_AS_ImplFnDef_stat));
  
        SEQ<TYPE_REP_TypeRep> domtp (TransTypeList (Nil (), ConstructImplDomType(partps)));
        TYPE_REP_TypeRep restp (TransType(Nil (), ConstructImplRngType(resnmtps)));
  
        TYPE_REP_TotalFnTypeRep fnsig (mk_REP_TotalFnTypeRep(domtp, restp));
        TYPE_REP_TotalFnTypeRep fnpresig (mk_REP_TotalFnTypeRep(domtp, btp_bool));
        SEQ<TYPE_REP_TypeRep> postdom (domtp);
        postdom.ImpAppend (restp);
        TYPE_REP_TotalFnTypeRep fnpostsig (mk_REP_TotalFnTypeRep(postdom, btp_bool));
  
        SEQ<TYPE_REP_TypeRep> r_l (TransTypeList(Nil (), tv_l));
  
        if (valbds.DomExists (nm))
        {
          //--------------------------------------------
          // Error message #103
          // L"%1" cannot be used in multiple definitions
          //--------------------------------------------
          GenErr (nm, ERR, 103, mk_sequence(PrintName (nm)));
          reswf = Bool(false);
        }
        else if (!tv_l.IsEmpty ())
        {
          valbds.ImpModify (nm, mk_(mk_REP_PolyTypeRep(r_l, fnsig), stat));
          valNames.Insert(nm);
  
          if (!fnpre.IsNil ())
            valbds.ImpModify (Pre (nm), mk_(mk_REP_PolyTypeRep(r_l, fnpresig), stat));
  
          valbds.ImpModify (Post (nm), mk_(mk_REP_PolyTypeRep(r_l, fnpostsig), stat));
        }
        else
        {
          valNames.Insert(nm);
          valbds.ImpModify (nm, mk_(fnsig, stat));
          if (!fnpre.IsNil ())
            valbds.ImpModify (Pre (nm), mk_(fnpresig, stat));
  
          valbds.ImpModify (Post (nm), mk_(fnpostsig, stat));
        }
        break;
      } // End of Is(ImplFnDef)
      case TAG_TYPE_AS_ExtExplFnDef: {
        const TYPE_AS_Name & nm   (def.GetRecord  (pos_AS_ExtExplFnDef_nm));
        const Sequence & tv_l     (def.GetSequence(pos_AS_ExtExplFnDef_params));
        const Sequence & partps   (def.GetSequence(pos_AS_ExtExplFnDef_partps));
        const Sequence & resnmtps (def.GetSequence(pos_AS_ExtExplFnDef_resnmtps));
        const Generic & fnpre     (def.GetField   (pos_AS_ExtExplFnDef_fnpre));
        const Generic & fnpost    (def.GetField   (pos_AS_ExtExplFnDef_fnpost));
        Bool stat                 (Settings.VDM10() ? Bool(true) : def.GetBool (pos_AS_ExtExplFnDef_stat));
  
        SEQ<TYPE_REP_TypeRep> domtp (TransTypeList (Nil (), ConstructImplDomType(partps)));
        TYPE_REP_TypeRep restp (TransType(Nil (), ConstructImplRngType(resnmtps)));
  
        TYPE_REP_TotalFnTypeRep fnsig (mk_REP_TotalFnTypeRep(domtp, restp));
  
        TYPE_REP_TotalFnTypeRep fnpresig (mk_REP_TotalFnTypeRep(domtp, btp_bool));
  
        SEQ<TYPE_REP_TypeRep> postdom (domtp);
        postdom.ImpAppend (restp);
        TYPE_REP_TotalFnTypeRep fnpostsig (mk_REP_TotalFnTypeRep(postdom, btp_bool));
  
        SEQ<TYPE_REP_TypeRep> r_l (TransTypeList(Nil (), tv_l));
  
        if (valbds.DomExists (nm))
        {
          //--------------------------------------------
          // Error message #103
          // L"%1" cannot be used in multiple definitions
          //--------------------------------------------
          GenErr (nm, ERR, 103, mk_sequence(PrintName (nm)));
          reswf = Bool(false);
        }
        else if (!tv_l.IsEmpty ())
        {
          valbds.ImpModify (nm, mk_(mk_REP_PolyTypeRep(r_l, fnsig), stat));
          valNames.Insert(nm);
  
          if (!fnpre.IsNil ())
            valbds.ImpModify (Pre (nm), mk_(mk_REP_PolyTypeRep(r_l, fnpresig), stat));
  
          if (!fnpost.IsNil ())
            valbds.ImpModify (Post (nm), mk_(mk_REP_PolyTypeRep(r_l, fnpostsig), stat));
        }
        else
        {
          valbds.ImpModify (nm, mk_(fnsig, stat));
          valNames.Insert(nm);
  
          if (!fnpre.IsNil ())
          {
            valbds.ImpModify (Pre (nm), mk_(fnpresig, stat));
          }
  
          if (!fnpost.IsNil ())
          {
            valbds.ImpModify (Post (nm), mk_(fnpostsig, stat));
          }
        }
        break;
      } // End of Is(ExtExplFnDef)
      default: {
        InternalError (L"ExtractPatterns");
        break;
      }
    }
  }

  Generic nm;
  for (bool cc = valNames.First (nm); cc; cc = valNames.Next (nm))
  {
    if (ReservedPrefix (nm))
    {
      //--------------------------------
      // Error message #110
      // L"%1" contains a reserved prefix
      //--------------------------------
      GenErr(nm, ERR, 110, mk_sequence(PrintName(nm)));
      reswf = Bool(false);
    }
  }
  return mk_(reswf, valbds);
}

// wf_ValueDef
// i : TYPE`Ind
// def AS`ValueDef
// ==> bool * map AS`Name to (REP`TypeRep * nat1)
Tuple StatSem::wf_ValueDef (const Int & i, const TYPE_AS_ValueDef & def)
{
  const TYPE_AS_Pattern & pat   (def.GetRecord(pos_AS_ValueDef_pat));
  const Generic & tp            (def.GetField (pos_AS_ValueDef_tp));
  const TYPE_AS_Expr & val      (def.GetRecord(pos_AS_ValueDef_val));
#ifdef VDMPP
  const TYPE_AS_Access & access (def.GetField (pos_AS_ValueDef_access));
//  const Bool & stat             (def.GetBool  (pos_AS_ValueDef_stat));
#endif // VDMPP
  const TYPE_CI_ContextId & cid (def.GetInt   (pos_AS_ValueDef_cid));

// 20120423 -->
  TYPE_REP_TypeRep exptp (tp.IsNil() ? Pattern2TypeRep(pat) : TransType(Nil(), tp)); //
// <-- 20120423

  // TODO: check operation
// 20130724 -->
  PushVDContext(cid);
// <-- 20130724
  Tuple infer (wf_Expr (i, val, exptp));
// 20130724 -->
  PopVDContext();
// <-- 20130724
  const Bool & wf_e (infer.GetBool (1));
  const TYPE_REP_TypeRep & tp_e (infer.GetRecord (2));

  TYPE_AS_PatternBind patbind (pat);
  if (!tp.IsNil ())
    patbind = TYPE_AS_TypeBind().Init(pat, tp, cid);

  Tuple infer2 (wf_PatternBind (i, patbind, tp_e));
  Bool wf_pb (infer2.GetField (1) == Bool(true)); // [bool]
  const MAP<TYPE_AS_Name,Tuple> & bd (infer2.GetMap (2));

// 20100827 -->
#ifdef VDMSL
  Set mult (bd.Dom().Intersect(this->Renamed));
  if (!mult.IsEmpty())
  {
    Generic nm;
    for (bool bb = mult.First(nm); bb; bb = mult.Next(nm))
    {
      //-------------------------
      // Error message #73
      // L"%1" is multiple defined
      //-------------------------
      GenErr (nm, ERR, 73, mk_sequence(PrintName (nm)));
    }
  }
#endif // VDMSL
// <-- 20100827

  if (!wf_pb)
  {
    //---------------------
    // Error message #104
    // Pattern cannot match
    //---------------------
    GenErrTp (pat, ERR, 104, tp_e, exptp, Sequence());
  }

  bool wf_acc (true);
#ifdef VDMPP
  if (access != Int(NOT_INITIALISED_AS)) // not let stmt nor let expr
    wf_acc = CheckPatternBindAccess (access, bd);
#endif //VDMPP

  return mk_(wf_e && wf_pb && wf_acc, bd);
}

#ifdef VDMPP
// IsSubClass
// return true if nm1 is subclass of nm2
// nm1 : AS`Name
// nm2 : AS`Name
// return : bool
bool StatSem::IsSubClass (const TYPE_AS_Name & nm1, const TYPE_AS_Name & nm2)
{
  return TransClos(nm1).InSet(nm2);
}

// IsLocalSuperSuper
// nm : AS`Name
// ==> bool
bool StatSem::IsLocalSuperSuper (const TYPE_AS_Name & nm) const
{
  return this->SuperSuper.InSet (nm);
}

// TransClos
// nm : AS`Name
// ==> set of AS`Name
SET<TYPE_AS_Name> StatSem::TransClos(const TYPE_AS_Name & nm)
{
  return TransClosAux (nm, SET<TYPE_AS_Name>());
}

// TransClosIndex
// nm : AS`Name
// ==> map nat to set of AS`Name
Map StatSem::TransClosIndex(const TYPE_AS_Name & nm)
{
  return TransClosIndexAux(nm, 1, Set().Insert(nm));
}

// TransClosAux
// nm : AS`Name
// done : set of AS`Name
// ==> set of AS`Name
SET<TYPE_AS_Name> StatSem::TransClosAux (const TYPE_AS_Name & nm, const SET<TYPE_AS_Name> & done)
{
  SET<TYPE_AS_Name> supers (CheckClassName (nm) ? LookUpClass (nm).GetSet(pos_SSENV_ParseTypeInfo_super) 
                                                : Set());

  SET<TYPE_AS_Name> dones (done);
  while (!supers.SubSet(dones))
  {
    SET<TYPE_AS_Name> st (supers.Diff (dones));

    TYPE_AS_Name super (st.GetElem ());

    dones.Insert (super);
    supers.ImpUnion (TransClosAux (super, dones));
  }
  return supers;
}

// TransClosIndexAux
// nm : AS`Name
// base : nat
// dine :set of AS`Name
// ==> map nat to set of AS`Name
Map StatSem::TransClosIndexAux(const TYPE_AS_Name& nm, int base, const SET<TYPE_AS_Name> & done)
{
  SET<TYPE_AS_Name> l_supers (CheckClassName(nm)
                                ? Set(LookUpClass(nm).GetSet(pos_SSENV_ParseTypeInfo_super)).ImpDiff(done)
                                : Set()); // set of AS`Name

  SET<TYPE_AS_Name> l_newDone (Set(l_supers).ImpUnion(done));

  Map resmap; // map nat to set of AS`Name
  resmap.Insert(Int(base), l_supers);

  Generic b_s;
  for (bool bb = l_supers.First(b_s); bb; bb = l_supers.Next(b_s))
  {
    Map indexentry (TransClosIndexAux(b_s, base + 1, l_newDone));
    SET<Int> intersect (indexentry.Dom().ImpIntersect(resmap.Dom()));
    if(!intersect.IsEmpty())
    {
      Generic entry;
      for (bool cc = intersect.First(entry); cc; cc = intersect.Next(entry))
      {
//        SET<TYPE_AS_Name> unionset (Set(resmap[entry]).ImpUnion(indexentry[entry]));
        SET<TYPE_AS_Name> unionset (resmap[entry]);
        unionset.ImpUnion(indexentry[entry]);
        resmap.ImpModify(entry, unionset);
      }
    } else {
// 20091021 -->
//      resmap.ImpOverride(indexentry);
      Set rng_indexentry (indexentry.Rng());
      bool exists = false;
      Generic nm_s;
      for (bool dd = rng_indexentry.First(nm_s); dd && !exists; dd = rng_indexentry.Next(nm_s))
        exists = !Set(nm_s).IsEmpty();
      if (exists)
        resmap.ImpOverride(indexentry);
// <-- 20091021
    }
  }
  return resmap;
}

// CheckParmsInHierarchy
// nm : AS`Name
// ptps : seq of REP`TypeRep
// p_acc : AS`Access
// op_or_fn : bool
// ==> bool
Bool StatSem::CheckParmsInHierarchy(const TYPE_AS_Name & nm,
                                         const SEQ<TYPE_REP_TypeRep> & ptps,
                                         const TYPE_AS_Access & p_acc,
                                         const Bool & op_or_fn)
{
  bool reswf (true);
  Map superparms ( LookUpParmsInHierarchy(nm, op_or_fn));

  Set dom_superparms (superparms.Dom());
  Generic b_clnm;
  for (bool bb = dom_superparms.First(b_clnm); bb; bb = dom_superparms.Next(b_clnm))
  {
    TYPE_SSENV_AccessType b_supOcc (superparms[b_clnm]);
    if ((GetMethDom(StripAccessType(b_supOcc)) == ptps) && !NotNarrowedAccess(GetSSAccess(b_supOcc), p_acc))
    {
      //--------------------------------------
      //-- Error 369
      //-- Scope narrowed by L"%1"
      //-------------------------------------
      GenErr(nm, ERR, 369, mk_sequence(PrintName(nm)));
      reswf = false;
    }
  }
  return Bool(reswf);
}

// LookUpParmsInHierarchy
// nm : AS`Name
// op_or_fn : bool
// ==> map AS`Name to (AccessOpTypeRep | AccessFnTypeRep)
Map StatSem::LookUpParmsInHierarchy(const TYPE_AS_Name & nm, const Bool & op_or_fn)
{
  Map class_m;
  Set ss (this->SuperSuper);
  Generic classnm;
  for (bool bb = ss.First(classnm); bb; bb = ss.Next(classnm))
  {
    Generic pti (GetClassTypeRep(classnm));
    if (!pti.IsNil()) {
//
// TODO:
      Map meths (op_or_fn ? Record(pti).GetMap(pos_SSENV_ParseTypeInfo_ops)
//                          : Record(pti).GetMap(pos_SSENV_ParseTypeInfo_fcts));
                          : (Settings.VDM10() ? ChangeToStatic(Record(pti).GetMap(pos_SSENV_ParseTypeInfo_fcts))
                                                 : Record(pti).GetMap(pos_SSENV_ParseTypeInfo_fcts)));
//
      class_m.ImpOverride(ExtractParmsMap(nm, classnm, meths));
    }
  }
  return class_m;
}

// ExtractParmsMap
// nm : AS`Name
// classnm : AS`Name
// meths : map AS`Name to (AccessOpTypeRep | AccessFnTypeRep)
// ==> map AS`Name to (AccessOpTypeRep | AccessFnTypeRep)
Map StatSem::ExtractParmsMap(const TYPE_AS_Name & nm, const TYPE_AS_Name & classnm, const Map & meths)
{
  TYPE_AS_Name nm1 (ASTAUX::GetFirstName(nm));
  SET<TYPE_AS_Name> dom_meths (meths.Dom());
  Generic meth;
  for (bool bb = dom_meths.First(meth); bb; bb = dom_meths.Next(meth))
  {
    if (Record(meth).GetSequence(pos_AS_Name_ids).Length() == 2)
    {
      TYPE_AS_Name nm2 (ASTAUX::GetSecondName(meth));
      if (nm1 == nm2) {
        Map result;
        result.Insert(classnm, meths[meth]);
        return result;
      }
    }
  }
  return Map();
}

/*
// FindSuperClassPath
// clsnm : AS`Name
// supernm : AS`Name
// ==> seq of char * nat
Tuple StatSem::FindSuperClassPath(const TYPE_AS_Name& clsnm,
                                       const TYPE_AS_Name& supernm)
{
  Tuple auxpath (FindSuperClassPathAux(clsnm, supernm));
  Sequence clspath (auxpath.GetField(1));
  Bool res (auxpath.GetField(2));
  wstring msg = L"";
  size_t len_clspath = clspath.Length();
  for (size_t i = 1; i < len_clspath; i++) {
    msg += PrintName(Record(clspath[i])) + L" -> ";
  }
  msg += PrintName(Record(clspath[len_clspath]));
   // FIXME: Is this length ever used??
  return mk_(Sequence(msg),Int(len_clspath));
}

// FindSuperClassPathAux
// clsnm : AS`Name
// supernm : AS`Name
// ==> seq of AS`Name * bool
Tuple StatSem::FindSuperClassPathAux(const TYPE_AS_Name& clsnm,
                                          const TYPE_AS_Name& supernm)
{
  Set supercls (LookUpClass(clsnm).get_super());

  if (supercls.InSet(supernm)) {
    SEQ<TYPE_AS_Name> s;
    s.ImpAppend(clsnm);
    s.ImpAppend(supernm);
    return mk_(s, Bool(true));
  }
  else {
    Generic g;
    for (bool bb = supercls.First(g); bb; bb = supercls.Next(g)) {
      Tuple tempTup (FindSuperClassPathAux(g, supernm));
      SEQ<TYPE_AS_Name> clspath (tempTup.GetField(1));
      Bool res (tempTup.GetField(2));
      if (res.GetValue()){
        SEQ<TYPE_AS_Name> s;
        s.ImpAppend(clsnm);
        s.ImpConc(clspath);
        return mk_(s, Bool(true)); // 20061213
      }
    }
    return mk_(SEQ<TYPE_AS_Name>(), Bool(false));
  }
}
*/
#endif // VDMPP

// InitEnv
// ==> ()
void StatSem::InitEnv ()
{
#ifdef VICE
  this->staticrequired = true;
#endif // VICE

  //this->ValueIds = SET<TYPE_AS_Name>();
  this->ValueIds.Clear();
  this->WhereAmI.Clear();
  this->WhereAmI.ImpPrepend(Int(AS_PURE)); //
  this->ExtEnv = Map();
  this->TypeEnv = MAP<TYPE_AS_Name,TYPE_SSENV_TypeRepElem>();

#ifdef VDMSL
  this->InsideInter = false;
  this->RenamedTypes = MAP<TYPE_AS_Name,TYPE_AS_Name>();
  this->TypeParEnv = MAP<TYPE_AS_Name,TYPE_REP_TypeRep>();
#endif // VDMSL

  this->TypeVars.Clear();
  this->TagEnv = Map();

#ifdef VDMSL
  this->LocTagEnv = Map();
#endif // VDMSL

  this->ConstEnv = MAP<TYPE_AS_Name,TYPE_SSENV_TypeRepElem>();
  this->StateEnv = MAP<TYPE_AS_Name,TYPE_SSENV_TypeRepElem> ();
  this->LocalEnv = Sequence ();
  this->FunctionEnv = MAP<TYPE_AS_Name,TYPE_SSENV_TypeRepElem>();
  this->PolyEnv = MAP<TYPE_AS_Name,TYPE_SSENV_PolyTypeRepElem>();
  this->OperationEnv = MAP<TYPE_AS_Name,TYPE_SSENV_OpTypeRepElem>();

#ifdef VDMPP
  this->OverloadedEnv = Map();
  this->SubrespsEnv = Map();
#endif //VDMPP

#ifdef VDMSL
  this->Renamed = SET<TYPE_AS_Name>();
  this->CheckModule = Nil();
#endif //VDMSL

  this->UsedName = SET<TYPE_AS_Name>();
  this->SEnv = SEQ<Map>();

#ifdef VDMPP
  this->CheckClass = Nil();
  this->Super = SET<TYPE_AS_Name>();
  this->SuperSuper = SET<TYPE_AS_Name>();
  this->TraceEnv = SET<TYPE_AS_Name>();
  this->CurrentMethod = Nil();
  this->FoundClass = Nil();
  this->DefiningClass = SEQ<TYPE_AS_Name>();
  this->isStatic = Bool(false);
#endif //VDMPP
//  this->RecMap = Map(); // not init for POG

  // cache for IsUnionRecursive
  this->UnionRecursiveResults = MAP<TYPE_REP_TypeRep, Bool>();

  this->ValueDefContext.Clear();

  this->lastline = 0;
  this->lastcol  = 0;
  this->lastfile = 0;
#ifdef VDMPP
  this->LookUpInObjectCache = Map();
  this->LookUpTypeNameCache = Map();
#endif //VDMPP
}

// SetExtAll
// nm_s : set of AS`Name
// ==> bool
bool StatSem::SetExtAll (const SET<TYPE_AS_Name> & nm_s)
{
  bool reswf = true;

  TYPE_AS_Mode rw ((Int)READWRITE);
//  this->ExtEnv = Map ();
  this->ExtEnv.Clear();
  SET<TYPE_AS_Name> dom_se (this->StateEnv.Dom());
  Generic nm;
  for (bool bb = dom_se.First (nm); bb; bb = dom_se.Next (nm))
  {
    this->ExtEnv.Insert (nm, rw);
    if (nm_s.InSet (nm)) {
      //----------------------------------------------------------------
      // Error message #105
      // L"%1" is used in both parameter list and in the state definition
      //----------------------------------------------------------------
// 20110830 -->
      //GenErr (nm, ERR, 105, mk_sequence(PrintName(nm)));
      TYPE_AS_Name pnm (Set(nm_s).ImpIntersect(mk_set(nm)).GetElem());
      GenErr (pnm, ERR, 105, mk_sequence(PrintName(nm)));
// <-- 20110830
      reswf = false;
    }
  }
  return reswf;
}

// SetExt
// ext_l : seq of AS`ExtInf
// nm_s : set of AS`Name
// ==> bool
bool StatSem::SetExt (const SEQ<TYPE_AS_ExtInf> & ext_l, const SET<TYPE_AS_Name> & nm_s)
{
  bool reswf (true);

//  this->ExtEnv = Map ();
  this->ExtEnv.Clear();

  size_t len = ext_l.Length();
  for (size_t index = 1; index <= len; index++)
  {
    TYPE_AS_ExtInf ei (ext_l[index]);
    const TYPE_AS_Mode & md         (ei.GetField(pos_AS_ExtInf_mode));
    const SEQ<TYPE_AS_Name>  & vars (ei.GetSequence(pos_AS_ExtInf_vars));
    const Generic & tp              (ei.GetField(pos_AS_ExtInf_tp));

    if (!tp.IsNil ())
      MarkUsedType (tp);

    size_t len_vars = vars.Length();
    for (size_t j = 1; j <= len_vars; j++)
    {
      TYPE_AS_Name var (vars[j]);
      Generic sttp (LookUpState (var, Bool(false), CUR));
      if (sttp.IsNil ())
        reswf = false;
      else if (!tp.IsNil () &&
#ifdef VDMSL
               //! (sttp == InstFn(PAR, TransType(Nil (),tp), this->TypeParEnv))
               ! (sttp == TransType(Nil (),tp))
#endif // VDMSL
#ifdef VDMPP
               ! (sttp == TransType(Nil (),tp))
#endif //VDMPP
                )
      {
        //--------------------------------------------------------------------
        // Error message #106
        // The type listed for L"%1" in the external clause is not the correct
        // type from the state definition
        //--------------------------------------------------------------------
        GenErr(var, ERR, 106, mk_sequence(PrintName(var)));
        reswf = false;
      }

      if (this->ExtEnv.DomExists (var)) {
        //-----------------------------------------
        // Error message #107
        // L"%1" is multiple used in this ext clause
        //-----------------------------------------
        GenErr (var, ERR, 107, mk_sequence(PrintName (var)));
        reswf = false;
      }
      else if (nm_s.InSet(var)) {
        //-----------------------------------------------------------
        // Error message #108
        // L"%1" is used in both parameter list and in this ext clause
        //-----------------------------------------------------------
        GenErr(var, ERR, 108, mk_sequence(PrintName (var)));
        reswf = false;
      }
      else
        this->ExtEnv.ImpModify (var, md);
    }
  }

#ifdef VDMPP
  Sequence le (this->LocalEnv); // seq of map AS`Name to REP`TypeRep
  size_t len_le = le.Length();
  for (size_t i = 1; i <= len_le; i++)
  {
    Map m (le[i]);
    Set dom_m (m.Dom());
    Generic nm;
    for (bool ee = dom_m.First(nm); ee; ee = dom_m.Next(nm))
      this->ExtEnv.ImpModify (nm, Int (READWRITE));
  }
#endif //VDMPP

  return reswf;
}

// PushTypeVarContext
// tv_l : seq of REP`TypeVarRep
void StatSem::PushTypeVarContext (const SEQ<TYPE_REP_TypeVarRep> & tv_l)
{
  this->TypeVars.ImpPrepend(tv_l.Elems());
}

// PopTypeVarContext
// tv_l : seq of REP`TypeVarRep
void StatSem::PopTypeVarContext ()
{
  this->TypeVars.ImpTl();
}

// GetContext
// for VDMSL
// ==> <PURE>|<ALL>|<PRE>|<POST>|<IMPL>|<VALUE>
// for VDMPP
// ==> <PURE>|<ALL>|<PRE>|<POST>|<IMPL>|<VALUE>|<PERM>|<PTHREAD>
Int StatSem::GetContext ()
{
  return this->WhereAmI.Hd();
}

// PushContext
// VDMSL
// kind : <PURE>|<ALL>|<PRE>|<POST>|<IMPL>|<VALUE>
// VDMPP
// kind : <PURE>|<ALL>|<PRE>|<POST>|<IMPL>|<VALUE>|<PERM>|<PTHREAD>
void StatSem::PushContext (const Int & kind)
{
  this->WhereAmI.ImpPrepend(kind);
}

// PopContext
void StatSem::PopContext ()
{
  this->WhereAmI.ImpTl();
}

#ifdef VDMPP
// clsnm : AS`Name
// binds : map AS`Name to (REP`TypeRep| AccessType| (AS`Name * AccessFieldRep) | set of AccessOpTypeRep)
// ==> map AS`Name to (REP`TypeRep|AccessType| (AS`Name * AccessFieldRep) | set of AccessOpTypeRep)
Map StatSem::TransBinds (const TYPE_AS_Name & clsnm, const Map & binds)
{
  SET<TYPE_AS_Name> dom_binds (binds.Dom());
  Map res_binds (binds);
  Generic nm;
  for (bool bb = dom_binds.First(nm); bb; bb = dom_binds.Next(nm))
  {
    Generic tp (binds[nm]);
    if (tp.IsTuple()) // (AS`Name * AccessFieldRep)
    {
      Tuple tpl (tp); // (AS`Name * AccessFieldRep)
      const TYPE_SSENV_AccessFieldRep & fl (tpl.GetRecord (2)); // AccessFieldRep
      const SEQ<TYPE_REP_FieldRep> & fr_l (fl.GetSequence(pos_SSENV_AccessFieldRep_tp));
      SEQ<TYPE_REP_FieldRep> new_fr_l;
      size_t len = fr_l.Length();
      for (size_t i = 1; i <= len; i++)
      {
        TYPE_REP_FieldRep fr (fr_l[i]);
        new_fr_l.ImpAppend (mk_REP_FieldRep(fr.GetField(pos_REP_FieldRep_sel),
                                            PrependClassName (clsnm, fr.GetRecord(pos_REP_FieldRep_tp)),
                                            fr.GetBool(pos_REP_FieldRep_dc)));
      }
      tpl.SetField (2, mk_SSENV_AccessFieldRep(new_fr_l, fl.GetField(pos_SSENV_AccessFieldRep_a)));
      res_binds.ImpModify (nm, tpl);
    }
    else if (tp.IsRecord()) { // REP`TypeRep | AccessType
      Record tpr (tp);
      if (IsAccessType (tpr)) { // AccessType
        // case TAG_TYPE_SSENV_AccessTypeRep:
        // case TAG_TYPE_SSENV_AccessFnTypeRep:
        // case TAG_TYPE_SSENV_AccessPolyTypeRep:
        // case TAG_TYPE_SSENV_AccessOpTypeRep:
        // case TAG_TYPE_SSENV_AccessFieldRep:
        tpr.SetField (1, PrependClassName (clsnm, StripAccessType(tpr)));
        res_binds.ImpModify (nm, tpr);
      }
      else // REP`TypeRep
        res_binds.ImpModify (nm, PrependClassName (clsnm, tpr));
    }
    else
    { // if (tp.IsSet()) { // set of AccessOpTypeRep
      // Note that the implementation is slightly
      // different here from the spec, since the implementation level
      // offers and easier way to express this IsSet test
      SET<TYPE_SSENV_AccessOpTypeRep> tps (tp);
      SET<TYPE_SSENV_AccessOpTypeRep> rb_nm;
      Generic tp;
      for (bool bb = tps.First(tp); bb; bb = tps.Next(tp))
      {
        TYPE_SSENV_AccessOpTypeRep tpe (tp);
        tpe.SetField(pos_SSENV_AccessOpTypeRep_tp,
                     PrependClassName(clsnm, tpe.GetRecord(pos_SSENV_AccessOpTypeRep_tp)));
        rb_nm.Insert(tpe);
      }
      res_binds.ImpModify(nm, rb_nm);
    }
  }
  return res_binds;
}
#endif // VDMPP

// CheckName
// nm : AS`Name
// ==> bool
bool StatSem::CheckName (const TYPE_AS_Name & nm)
{
  if (this->UsedName.InSet (nm))
  {
    //-------------------------
    // Error message #73
    // L"%1" is multiple defined
    //-------------------------
    GenErr (nm, ERR, 73, mk_sequence(PrintName(nm)));
    return false;
  }
  else if (!ReservedPrefix (nm))
  {
    this->UsedName.Insert (nm);
    return true;
  }
  else
  {
    //--------------------------------
    // Error message #110
    // L"%1" contains a reserved prefix
    //--------------------------------
    GenErr (nm, ERR, 110, mk_sequence(PrintName(nm)));
    return false;
  }
}

// MarkUsedType
// tp : AS`Type
void StatSem::MarkUsedType (const TYPE_AS_Type & tp)
{
  switch (tp.GetTag()) {
    case TAG_TYPE_AS_TypeName: {
      const TYPE_AS_Name & nm (tp.GetRecord(pos_AS_TypeName_name));
//#ifdef VDMSL
//      if (!this->TypeParEnv.DomExists (nm))
//#endif //VDMSL
      if (this->TypeEnv.DomExists (nm))
      {
        TYPE_SSENV_TypeRepElem rc (this->TypeEnv[nm]);
        rc.SetField(pos_SSENV_TypeRepElem_used, Bool(true));
        this->TypeEnv.ImpModify (nm, rc);
      }
      break;
    }
    case TAG_TYPE_AS_CompositeType: {
      const SEQ<TYPE_AS_Field> & fields (tp.GetSequence(pos_AS_CompositeType_fields));
      size_t len_fields = fields.Length();
      for (size_t i = 1; i <= len_fields; i++)
        MarkUsedType (fields[i].GetRecord(pos_AS_Field_type));
      break;
    }
    case TAG_TYPE_AS_UnionType: {
      const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence(pos_AS_UnionType_tps));
      size_t len_tp_l = tp_l.Length();
      for (size_t i = 1; i <= len_tp_l; i++)
        MarkUsedType (tp_l[i]);
      break;
    }
    case TAG_TYPE_AS_ProductType: {
      const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence(pos_AS_ProductType_tps));
      size_t len_tp_l = tp_l.Length();
      for (size_t i = 1; i <= len_tp_l; i++)
        MarkUsedType (tp_l[i]);
      break;
    }
    case TAG_TYPE_AS_BracketedType: {
      MarkUsedType (tp.GetRecord(pos_AS_BracketedType_tp));
      break;
    }
    case TAG_TYPE_AS_OptionalType: {
      MarkUsedType (tp.GetRecord(pos_AS_OptionalType_tp));
      break;
    }
    case TAG_TYPE_AS_Set0Type: {
      MarkUsedType (tp.GetRecord(pos_AS_Set0Type_elemtp));
      break;
    }
    case TAG_TYPE_AS_Set1Type: {
      MarkUsedType (tp.GetRecord(pos_AS_Set1Type_elemtp));
      break;
    }
    case TAG_TYPE_AS_Seq0Type: {
      MarkUsedType (tp.GetRecord(pos_AS_Seq0Type_elemtp));
      break;
    }
    case TAG_TYPE_AS_Seq1Type: {
      MarkUsedType (tp.GetRecord(pos_AS_Seq1Type_elemtp));
      break;
    }
    case TAG_TYPE_AS_GeneralMap0Type: {
      MarkUsedType (tp.GetRecord(pos_AS_GeneralMap0Type_mapdom));
      MarkUsedType (tp.GetRecord(pos_AS_GeneralMap0Type_maprng));
      break;
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      MarkUsedType (tp.GetRecord(pos_AS_GeneralMap1Type_mapdom));
      MarkUsedType (tp.GetRecord(pos_AS_GeneralMap1Type_maprng));
      break;
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      MarkUsedType (tp.GetRecord(pos_AS_InjectiveMap0Type_mapdom));
      MarkUsedType (tp.GetRecord(pos_AS_InjectiveMap0Type_maprng));
      break;
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      MarkUsedType (tp.GetRecord(pos_AS_InjectiveMap1Type_mapdom));
      MarkUsedType (tp.GetRecord(pos_AS_InjectiveMap1Type_maprng));
      break;
    }
    case TAG_TYPE_AS_OpType: {
      const SEQ<TYPE_AS_Type> & d (tp.GetSequence(pos_AS_OpType_opdom));
      size_t len_d = d.Length();
      for (size_t i = 1; i <= len_d; i++)
        MarkUsedType (d[i]);
      MarkUsedType (tp.GetRecord(pos_AS_OpType_oprng));
      break;
    }
    case TAG_TYPE_AS_PartialFnType: {
      const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence(pos_AS_PartialFnType_fndom));
      size_t len_tp_l = tp_l.Length();
      for (size_t i = 1; i <= len_tp_l; i++)
        MarkUsedType (tp_l[i]);
      MarkUsedType (tp.GetRecord(pos_AS_PartialFnType_fnrng));
      break;
    }
    case TAG_TYPE_AS_TotalFnType: {
      const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence(pos_AS_TotalFnType_fndom));
      size_t len_tp_l = tp_l.Length();
      for (size_t i = 1; i <= len_tp_l; i++)
        MarkUsedType (tp_l[i]);
      MarkUsedType (tp.GetRecord(pos_AS_TotalFnType_fnrng));
      break;
    }
  }
}

// TransTypeList
// modnm : [AS`Name]
// tp_l :  seq of AS`Type
// -> seq of REP`TypeRep
SEQ<TYPE_REP_TypeRep> StatSem::TransTypeList (const Generic & modnm, const SEQ<TYPE_AS_Type> & tp_l) const
{
  SEQ<TYPE_REP_TypeRep> res;
  size_t len_tp_l = tp_l.Length();
  for (size_t index = 1; index <= len_tp_l; index++)
    res.ImpAppend (TransType (modnm, tp_l[index]));
  return res;
}

// TransTypeDef
// modnm : [AS`Name]
// tp : AS`Type
// inv : [AS`Invariant]
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::TransTypeDef (const Generic & modnm, const TYPE_AS_Name & nm,
                                        const TYPE_AS_Type & tp, const Generic & inv)
{
  if (inv.IsNil())
    return TransType(modnm, tp);
  else
  {
    const TYPE_AS_Invariant asInv (inv);
    TYPE_AS_Name name (
      (modnm.IsNil() || (modnm == ASTAUX::MkNameFromId (ASTAUX::MkId(L"DefaultMod"), NilContextId)))
       ? nm : ExtName (modnm, nm));
    return mk_REP_InvTypeRep(name,
                             TransType(modnm, tp),
                             (Generic)mk_(asInv.GetRecord(pos_AS_Invariant_pat),
                                          asInv.GetRecord(pos_AS_Invariant_expr)));
  }
}

// TransType
// modnm : [AS`Name]
// tp :  AS`Type
// ==> REP`TypeRep
TYPE_REP_TypeRep StatSem::TransType (const Generic & modnm, const TYPE_AS_Type & tp) const
{
  switch (tp.GetTag()) {
    case TAG_TYPE_AS_BooleanType: {
      return mk_REP_BooleanTypeRep();
    }
    case TAG_TYPE_AS_NumericType: {
      return mk_REP_NumericTypeRep(tp.GetField(pos_AS_NumericType_qtp));
    }
    case TAG_TYPE_AS_TokenType: {
      return mk_REP_TokenTypeRep();
    }
    case TAG_TYPE_AS_CharType: {
      return mk_REP_CharTypeRep();
    }
    case TAG_TYPE_AS_VoidType: {
      return mk_REP_UnitTypeRep();
    }
    case TAG_TYPE_AS_QuoteType: {
      const TYPE_AS_QuoteLit & ql (tp.GetRecord(pos_AS_QuoteType_lit));
      return mk_REP_QuoteTypeRep(ASTAUX::MkNameFromId(ql.get_val(), NilContextId));
    }
    case TAG_TYPE_AS_TypeName: {
      const TYPE_AS_Name & nm (tp.GetRecord(pos_AS_TypeName_name));
//#ifdef VDMSL
//      if (this->TypeParEnv.DomExists (nm))
//        return this->TypeParEnv[nm];
//#endif //VDMSL
  //    MarkUsedType (tp); // 30.05.95 (HV) Not in spec!
      if (modnm.IsNil ())
        return mk_REP_TypeNameRep(nm);
      else if ( (nm.GetSequence(pos_AS_Name_ids).Length () == 1) 
#ifdef VDMPP
                && !IsClassName(nm)
                && GetTypes().InSet(nm)
#endif // VDMPP
              )
      {
        // TODO : 20071130
        // the case : nm is a type defined by super class
        //            modnm is't good
        return mk_REP_TypeNameRep(ExtName (modnm, nm));
      }
      else
        return mk_REP_TypeNameRep(nm);
    }
    case TAG_TYPE_AS_TypeVar: {
      return mk_REP_TypeVarRep(tp.GetRecord(pos_AS_TypeVar_name));
    }
    case TAG_TYPE_AS_CompositeType: {
      const TYPE_AS_Name & tag (tp.GetRecord(pos_AS_CompositeType_name));
      const SEQ<TYPE_AS_Field> & fields (tp.GetSequence(pos_AS_CompositeType_fields));
      if (modnm.IsNil ())
        return mk_REP_CompositeTypeRep(tag, TransFields(modnm, fields));
      else if (tag.GetSequence(pos_AS_Name_ids).Length() == 1)
        return mk_REP_CompositeTypeRep(ExtName (modnm, tag), TransFields(modnm, fields));
      else
        return mk_REP_CompositeTypeRep(tag, TransFields(modnm, fields));
    }
    case TAG_TYPE_AS_UnionType: {
      const SEQ<TYPE_AS_Type> & tp_l (tp.GetSequence(pos_AS_UnionType_tps));
      size_t len_tp_l = tp_l.Length();
      SET<TYPE_REP_TypeRep> tp_s;
      for (size_t i = 1; i <= len_tp_l; i++)
        tp_s.Insert(TransType(modnm, tp_l[i]));
      return mk_REP_UnionTypeRep( FlatternUnion(tp_s) );
    }
    case TAG_TYPE_AS_ProductType: {
      return mk_REP_ProductTypeRep(TransTypeList(modnm, tp.GetSequence(pos_AS_ProductType_tps)));
    }
    case TAG_TYPE_AS_BracketedType: {
      return TransType(modnm, tp.GetRecord(pos_AS_BracketedType_tp));
    }
    case TAG_TYPE_AS_OptionalType: {
      return mk_REP_UnionTypeRep( FlatternUnion(mk_set(rep_niltp,
                                                       TransType(modnm, tp.GetRecord(pos_AS_OptionalType_tp)))) );
    }
    case TAG_TYPE_AS_Set0Type: {
      TYPE_REP_TypeRep elemtp (TransType(modnm, tp.GetRecord(pos_AS_Set0Type_elemtp)));
      return mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySetTypeRep(elemtp), mk_REP_SetTypeRep(elemtp)));
    }
    case TAG_TYPE_AS_Set1Type: {
      return mk_REP_SetTypeRep(TransType(modnm, tp.GetRecord(pos_AS_Set1Type_elemtp)));
    }
    case TAG_TYPE_AS_Seq0Type: {
      TYPE_REP_TypeRep elemtp (TransType(modnm, tp.GetRecord(pos_AS_Seq0Type_elemtp)));
      return mk_REP_UnionTypeRep(mk_set(mk_REP_EmptySeqTypeRep(elemtp), mk_REP_SeqTypeRep(elemtp)));
    }
    case TAG_TYPE_AS_Seq1Type: {
      return mk_REP_SeqTypeRep(TransType(modnm, tp.GetRecord(pos_AS_Seq1Type_elemtp)));
    }
    case TAG_TYPE_AS_GeneralMap0Type: {
      TYPE_REP_TypeRep domtp (TransType(modnm, tp.GetRecord(pos_AS_GeneralMap0Type_mapdom)));
      TYPE_REP_TypeRep regtp (TransType(modnm, tp.GetRecord(pos_AS_GeneralMap0Type_maprng)));
      return mk_REP_UnionTypeRep(mk_set(mk_REP_EmptyMapTypeRep(domtp, regtp),
                                        mk_REP_GeneralMapTypeRep(domtp, regtp)));
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      return mk_REP_GeneralMapTypeRep(TransType(modnm, tp.GetRecord(pos_AS_GeneralMap1Type_mapdom)),
                                      TransType(modnm, tp.GetRecord(pos_AS_GeneralMap1Type_maprng)));
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      TYPE_REP_TypeRep domtp (TransType(modnm, tp.GetRecord(pos_AS_InjectiveMap0Type_mapdom)));
      TYPE_REP_TypeRep regtp (TransType(modnm, tp.GetRecord(pos_AS_InjectiveMap0Type_maprng)));
      return mk_REP_UnionTypeRep(mk_set(mk_REP_EmptyMapTypeRep(domtp, regtp),
                                        mk_REP_InjectiveMapTypeRep(domtp, regtp)));
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      return mk_REP_InjectiveMapTypeRep(TransType(modnm, tp.GetRecord(pos_AS_InjectiveMap1Type_mapdom)),
                                        TransType(modnm, tp.GetRecord(pos_AS_InjectiveMap1Type_maprng)));
    }
    case TAG_TYPE_AS_OpType: {
      return mk_REP_OpTypeRep(TransTypeList(modnm, tp.GetSequence(pos_AS_OpType_opdom)),
                              TransType(modnm, tp.GetRecord(pos_AS_OpType_oprng)));
    }
    case TAG_TYPE_AS_PartialFnType: {
      return mk_REP_PartialFnTypeRep(TransTypeList(modnm, tp.GetSequence(pos_AS_PartialFnType_fndom)),
                                     TransType(modnm, tp.GetRecord(pos_AS_PartialFnType_fnrng)));
    }
    case TAG_TYPE_AS_TotalFnType: {
      return mk_REP_TotalFnTypeRep(TransTypeList(modnm, tp.GetSequence(pos_AS_TotalFnType_fndom)),
                                   TransType(modnm, tp.GetRecord(pos_AS_TotalFnType_fnrng)));
    }
    case TAG_TYPE_AS_AllType: {
      return rep_alltp;
    }
    default: {
      InternalError(L"TransType: " + tp.ascii() );
      return rep_alltp;
    }
  }
}

#ifdef VDMSL
// CurMod
// kind : (<ADD>|<REM>)
// curmod : AS`Name
// tp_org REP`TypeRep
// +> REP`TypeRep
TYPE_REP_TypeRep StatSem::CurMod (int kind, const TYPE_AS_Name & curmod, const TYPE_REP_TypeRep & tp)
{
  switch (tp.GetTag ()) {
    case TAG_TYPE_REP_TypeNameRep: {
      TYPE_AS_Name nm (tp.GetRecord(pos_REP_TypeNameRep_nm));
      TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
      TYPE_AS_Ids cids (curmod.GetSequence(pos_AS_Name_ids));

      switch(kind) {
        case REM: {
          if (ids.Length () == 2 && (ids.Hd () == cids.Hd ())) {
            nm.set_ids (ids.Tl ());
            return mk_REP_TypeNameRep(nm);
          }
          else
            return tp;
        }
        case ADD: {
          if (ids.Length () == 1) {
            cids.ImpConc (ids);
            nm.set_ids (cids);
            return mk_REP_TypeNameRep(nm);
          }
          else
            return tp;
        }
      }
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
      const TYPE_AS_Name & tag (tp.GetRecord(pos_REP_CompositeTypeRep_nm));
      const SEQ<TYPE_REP_FieldRep> & fields (tp.GetSequence(pos_REP_CompositeTypeRep_fields));
      SEQ<TYPE_REP_FieldRep> f_l;
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++)
      {
        const TYPE_REP_FieldRep & field (fields[idx]);
        f_l.ImpAppend (mk_REP_FieldRep(field.GetField(pos_REP_FieldRep_sel),
                                       CurMod(kind,curmod,field.GetRecord(pos_REP_FieldRep_tp)),
                                       field.GetBool(pos_REP_FieldRep_dc)));
      }
      TYPE_AS_Ids id_l (tag.GetSequence(pos_AS_Name_ids));
      TYPE_AS_Ids cur_l (curmod.GetSequence(pos_AS_Name_ids));

      TYPE_AS_Name newtag (tag);
      if (kind == Int (REM)) {
        if (id_l.Length() == 2 && (id_l.Hd() == cur_l.Hd())) {
          id_l.ImpTl();
          newtag.set_ids (id_l);
        }
      }
      else {
        if (id_l.Length() == 1) {
          cur_l.ImpConc(id_l);
          newtag.set_ids(cur_l);
        }
      }
      return mk_REP_CompositeTypeRep(newtag, f_l);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (tp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> s;
      Generic g;
      for (bool bb = tp_s.First (g); bb; bb = tp_s.Next (g))
        s.Insert (CurMod(kind, curmod, g));
      return mk_REP_UnionTypeRep(s);
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & tp_l (tp.GetSequence(pos_REP_ProductTypeRep_tps));
      SEQ<TYPE_REP_TypeRep> l;
      size_t len_tp_l = tp_l.Length();
      for (size_t idx = 1; idx <= len_tp_l; idx++)
        l.ImpAppend (CurMod(kind, curmod, tp_l[idx]));
      return mk_REP_ProductTypeRep(l);
    }
    case TAG_TYPE_REP_SetTypeRep: {
      TYPE_REP_TypeRep type (tp.GetRecord(pos_REP_SetTypeRep_elemtp));
      return mk_REP_SetTypeRep(CurMod(kind, curmod, type));
    }
    case TAG_TYPE_REP_EmptySetTypeRep: {
      TYPE_REP_TypeRep type (tp.GetRecord(pos_REP_EmptySetTypeRep_elemtp));
      return mk_REP_EmptySetTypeRep(CurMod(kind, curmod, type));
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      TYPE_REP_TypeRep type (tp.GetRecord(pos_REP_SeqTypeRep_elemtp));
      return mk_REP_SeqTypeRep(CurMod(kind, curmod, type));
    }
    case TAG_TYPE_REP_EmptySeqTypeRep: {
      TYPE_REP_TypeRep type (tp.GetRecord(pos_REP_EmptySeqTypeRep_elemtp));
      return mk_REP_EmptySeqTypeRep(CurMod(kind, curmod, type));
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return mk_REP_GeneralMapTypeRep(CurMod(kind, curmod, tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom)),
                                      CurMod(kind, curmod, tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng)));
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return mk_REP_InjectiveMapTypeRep(CurMod(kind, curmod, tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom)),
                                        CurMod(kind, curmod, tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng)));
    }
    case TAG_TYPE_REP_EmptyMapTypeRep: {
      return mk_REP_EmptyMapTypeRep(CurMod(kind, curmod, tp.GetRecord(pos_REP_EmptyMapTypeRep_mapdom)),
                                    CurMod(kind, curmod, tp.GetRecord(pos_REP_EmptyMapTypeRep_maprng)));
    }
    case TAG_TYPE_REP_OpTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & tp_l (tp.GetSequence(pos_REP_OpTypeRep_Dom));
      const TYPE_REP_TypeRep & tp_ (tp.GetRecord(pos_REP_OpTypeRep_Rng));
      SEQ<TYPE_REP_TypeRep> l;
      size_t len_tp_l = tp_l.Length();
      for (size_t idx = 1; idx <= len_tp_l; idx++)
        l.ImpAppend (CurMod(kind, curmod, tp_l[idx]));
      return mk_REP_OpTypeRep(l, CurMod(kind, curmod, tp_));
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      const TYPE_REP_TypeRep & fnrng (tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng));
// 20140326 -->
      //const SEQ<TYPE_REP_TypeRep> & fndom (tp.GetSequence(pos_REP_PartialFnTypeRep_fndom));
      const Generic & dom (tp.GetField(pos_REP_PartialFnTypeRep_fndom));
      if (dom.Is(TAG_TYPE_REP_AllTypeRep))
        return mk_REP_PartialFnTypeRep(dom, CurMod(kind, curmod, fnrng));
      const SEQ<TYPE_REP_TypeRep> & fndom (dom);
// <-- 20140326
      SEQ<TYPE_REP_TypeRep> l;
      size_t len_fndom = fndom.Length();
      for (size_t idx = 1; idx <= len_fndom; idx++)
        l.ImpAppend (CurMod(kind,curmod, fndom[idx]));
      return mk_REP_PartialFnTypeRep(l, CurMod(kind, curmod, fnrng));
    }
    case TAG_TYPE_REP_TotalFnTypeRep: {
      const TYPE_REP_TypeRep & fnrng (tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng));
// 20140326 -->
      //const SEQ<TYPE_REP_TypeRep> & fndom (tp.GetSequence(pos_REP_TotalFnTypeRep_fndom));
      const Generic & dom (tp.GetField(pos_REP_TotalFnTypeRep_fndom));
      if (dom.Is(TAG_TYPE_REP_AllTypeRep))
        return mk_REP_TotalFnTypeRep(dom, CurMod(kind, curmod, fnrng));
      const SEQ<TYPE_REP_TypeRep> & fndom (dom);
// <-- 20140326
      SEQ<TYPE_REP_TypeRep> l;
      size_t len_fndom = fndom.Length();
      for (size_t idx = 1; idx <= len_fndom; idx++)
        l.ImpAppend (CurMod(kind, curmod, fndom[idx]));
      return mk_REP_TotalFnTypeRep(l, CurMod(kind, curmod, fnrng));
    }
    default:
      return tp;
  }
}
#endif // VDMSL

#ifdef VDMPP
// PrependClassName
// clsnm : AS`Name
// tp_q : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::PrependClassName (const TYPE_AS_Name & clsnm, const TYPE_REP_TypeRep & tp_q)
{
  TYPE_REP_TypeRep tp (StripAccessType(tp_q));

  TYPE_AS_Id cnm (clsnm.get_ids ().Hd());

  switch (tp.GetTag()) {
    case TAG_TYPE_REP_TypeNameRep: {
      TYPE_AS_Name tpn (tp.GetRecord(pos_REP_TypeNameRep_nm));
      TYPE_AS_Ids nm_l (tpn.get_ids ());
// 20120117 -->
//      if (nm_l.Length () == 1) {
      if ((nm_l.Length () == 1) && !IsClassName(tpn)) {
// <--20120117
        nm_l.ImpPrepend (cnm);
        tpn.set_ids (nm_l);
      }
      return mk_REP_TypeNameRep(tpn);
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return mk_REP_InvTypeRep(tp.GetRecord(pos_REP_InvTypeRep_name),
                               PrependClassName (clsnm, tp.GetRecord(pos_REP_InvTypeRep_shape)),
                               tp.GetField(pos_REP_InvTypeRep_invariant));
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
      TYPE_AS_Name tag (tp.GetRecord(pos_REP_CompositeTypeRep_nm));
      TYPE_AS_Ids nm_l (tag.get_ids ());
      if (nm_l.Length () == 1) {
        nm_l.ImpPrepend (cnm);
        tag.set_ids (nm_l);
      }
      const SEQ<TYPE_REP_FieldRep> & f_l (tp.GetSequence(pos_REP_CompositeTypeRep_fields));
      SEQ<TYPE_REP_FieldRep> res_f_l;
      size_t len_f_l = f_l.Length();
      for (size_t idx = 1; idx <= len_f_l; idx++)
      {
        const TYPE_REP_FieldRep & fld (f_l[idx]);
//        fld.set_tp (PrependClassName (clsnm, fld.get_tp ()));
//        res_f_l.ImpAppend (fld);
        res_f_l.ImpAppend (mk_REP_FieldRep(fld.GetField(pos_REP_FieldRep_sel),
                                           PrependClassName (clsnm, fld.GetRecord(pos_REP_FieldRep_tp)),
                                           fld.GetBool(pos_REP_FieldRep_dc)));
      }
// 20070305
      TYPE_AS_Name tagext (tag);
      if (tag.GetSequence(pos_AS_Name_ids).Length() == 1)
        tagext = ExtName(clsnm, tag);

      return mk_REP_CompositeTypeRep(tagext, res_f_l);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> t_s (tp.GetSet(pos_REP_UnionTypeRep_tps)), res_t_s;
      Generic t;
      for (bool bb = t_s.First(t); bb; bb = t_s.Next(t))
        res_t_s.Insert (PrependClassName (clsnm, t));

      return mk_REP_UnionTypeRep(res_t_s);
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & f_l (tp.GetSequence(pos_REP_ProductTypeRep_tps));
      SEQ<TYPE_REP_TypeRep> res_f_l;
      size_t len_f_l = f_l.Length();
      for (size_t idx = 1; idx <= len_f_l; idx++)
        res_f_l.ImpAppend (PrependClassName (clsnm, f_l[idx]));

      return mk_REP_ProductTypeRep(res_f_l);
    }
    case TAG_TYPE_REP_SetTypeRep: {
      return mk_REP_SetTypeRep(PrependClassName (clsnm, tp.GetRecord(pos_REP_SetTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_EmptySetTypeRep: {
      return mk_REP_EmptySetTypeRep(PrependClassName (clsnm, tp.GetRecord(pos_REP_EmptySetTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return mk_REP_SeqTypeRep(PrependClassName (clsnm, tp.GetRecord(pos_REP_SeqTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_EmptySeqTypeRep: {
      return mk_REP_EmptySeqTypeRep(PrependClassName (clsnm, tp.GetRecord(pos_REP_EmptySeqTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return mk_REP_GeneralMapTypeRep(PrependClassName (clsnm, tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom)),
                                      PrependClassName (clsnm, tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng)));
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return mk_REP_InjectiveMapTypeRep(PrependClassName (clsnm, tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom)),
                                        PrependClassName (clsnm, tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng)));
    }
    case TAG_TYPE_REP_EmptyMapTypeRep: {
      return mk_REP_EmptyMapTypeRep(PrependClassName (clsnm, tp.GetRecord(pos_REP_EmptyMapTypeRep_mapdom)),
                                    PrependClassName (clsnm, tp.GetRecord(pos_REP_EmptyMapTypeRep_maprng)));
    }
    case TAG_TYPE_REP_TotalFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & sq (tp.GetSequence(pos_REP_TotalFnTypeRep_fndom));
      SEQ<TYPE_REP_TypeRep> res_sq;
      size_t len_sq = sq.Length();
      for (size_t idx = 1; idx <= len_sq; idx++)
        res_sq.ImpAppend (PrependClassName (clsnm, sq[idx]));

      return mk_REP_TotalFnTypeRep(res_sq, PrependClassName (clsnm, tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng)));
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & sq (tp.GetSequence(pos_REP_PartialFnTypeRep_fndom));
      SEQ<TYPE_REP_TypeRep> res_sq;
      size_t len_sq = sq.Length();
      for (size_t idx = 1; idx <= len_sq; idx++)
        res_sq.ImpAppend (PrependClassName (clsnm, sq[idx]));

      return mk_REP_PartialFnTypeRep(res_sq, PrependClassName (clsnm, tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng)));
    }
    case TAG_TYPE_REP_OpTypeRep: {
      return tp;
    }
    default: {
      return tp;
    }
  }
}
#endif // VDMPP

// FlatternUnion
// tps : set of REP`TypeRep
// -> set of REP`TypeRep
SET<TYPE_REP_TypeRep> StatSem::FlatternUnion (const SET<TYPE_REP_TypeRep> & s) const
{
  SET<TYPE_REP_TypeRep> tps (s); // for safe
  SET<TYPE_REP_TypeRep> res;
  Generic t;
  for(bool bb = tps.First(t); bb; bb = tps.Next(t))
  {
    TYPE_REP_TypeRep tp (t);
    switch(tp.GetTag()) {
      case TAG_TYPE_REP_UnionTypeRep: {
        SET<TYPE_REP_TypeRep> tmp_tps (tp.GetSet(pos_REP_UnionTypeRep_tps));
        res.ImpUnion(FlatternUnion(tmp_tps));
        break;
      }
      default: {
        res.Insert(tp);
        break;
      }
    }
  }
  return res;
}

// TransFields
// modnm : [AS`Name]
// f_l : seq of AS`Field
// -> seq of REP`FieldRep
SEQ<TYPE_REP_FieldRep> StatSem::TransFields (const Generic & modnm, const SEQ<TYPE_AS_Field> & f_l) const
{
  SEQ<TYPE_REP_FieldRep> res;
  size_t len_f_l = f_l.Length();
  for (size_t i = 1; i <= len_f_l; i++)
  {
    const TYPE_AS_Field & f (f_l[i]);
    res.ImpAppend(mk_REP_FieldRep(f.GetField(pos_AS_Field_sel),
                                  TransType(modnm, f.GetRecord(pos_AS_Field_type)),
                                  f.GetBool(pos_AS_Field_dc)));
  }
  return res;
}

#ifdef VDMSL
// TransStateType
// modid : [AS`Name]
// stdef : AS`StateDef
// -> REP`TypeRep
TYPE_REP_TypeRep StatSem::TransStateType (const Generic & modid, const TYPE_AS_StateDef & stdef)
{
  const TYPE_AS_CompositeType & ct (stdef.GetRecord(pos_AS_StateDef_tp));
  return TransTypeDef (modid/*Nil ()*/,
                       ct.GetRecord(pos_AS_CompositeType_name),
                       ct,
                       stdef.GetField(pos_AS_StateDef_Inv));
}
#endif //VDMSL

// ConstructImplParms
// partps : AS`ParameterTypes
// -> seq of AS`Pattern
SEQ<TYPE_AS_Pattern> StatSem::ConstructImplParms (const SEQ<TYPE_AS_PatTypePair> & partps) const
{
  SEQ<TYPE_AS_Pattern> res;

  size_t len_partps = partps.Length();
  for (size_t i = 1; i <= len_partps; i++)
  {
    const TYPE_AS_PatTypePair & ptp (partps[i]);
    const SEQ<TYPE_AS_Pattern> & pats (ptp.GetSequence(pos_AS_PatTypePair_pats));
    res.ImpConc(pats);
  }
  return res;
}

// PrintName
// name : AS`Name
// -> seq of char
SEQ<Char> StatSem::PrintName(const TYPE_AS_Name & name) const
{
  const TYPE_AS_Ids & ids (name.GetSequence(pos_AS_Name_ids));
  switch (ids.Length()) {
    case 0:
      return SEQ<Char>(L"");
    case 1:
      return ids[1];
    case 2:
      return ids[1].Conc(SEQ<Char>(L"`")).Conc(ids[2]);
    default: {
      SEQ<Char> res (ids[1]);
      size_t len_ids = ids.Length();
      for (size_t idx = 2; idx <= len_ids; idx++)
      {
        res.ImpAppend(Char(L'`'));
        res.ImpConc(ids[idx]);
      }
      return res;
    }
  }
}

// ExtractValueBindings
// valtps : seq of AS`NameType
// -> map AS`Name to (REP`TypeRep * nat1)
MAP<TYPE_AS_Name, Tuple> StatSem::ExtractValueBindings (const SEQ<TYPE_AS_NameType> & valtps)
{
  MAP<TYPE_AS_Name, Tuple> res;

  if (!valtps.IsEmpty()) {
    size_t len = valtps.Length();
    for (size_t index = 1; index <= len; index++)
    {
      TYPE_AS_NameType valtp (valtps[index]);
      res.Insert (valtp.GetRecord(pos_AS_NameType_nm),
                  mk_(TransType(Nil (), valtp.GetRecord(pos_AS_NameType_tp)),Int(1)));
    }
  }
  return res;
}

#ifdef VDMPP
// MakeEmptyClass
// nm : AS`Name
// -> ParseTypeInfo
TYPE_SSENV_ParseTypeInfo StatSem::MakeEmptyClass (const TYPE_AS_Name & nm)
{
  TYPE_SSENV_ParseTypeInfo pti;
  pti.SetField(pos_SSENV_ParseTypeInfo_nm,           nm);
  pti.SetField(pos_SSENV_ParseTypeInfo_sys,          Bool(false));
  pti.SetField(pos_SSENV_ParseTypeInfo_super,        Set());
  pti.SetField(pos_SSENV_ParseTypeInfo_tps,          Map());
  pti.SetField(pos_SSENV_ParseTypeInfo_vals,         Map());
  pti.SetField(pos_SSENV_ParseTypeInfo_insts,        Map());
  pti.SetField(pos_SSENV_ParseTypeInfo_fcts,         Map());
  pti.SetField(pos_SSENV_ParseTypeInfo_polys,        Map());
  pti.SetField(pos_SSENV_ParseTypeInfo_ops,          Map());
  pti.SetField(pos_SSENV_ParseTypeInfo_tags,         Map());
  pti.SetField(pos_SSENV_ParseTypeInfo_overloaded,   Map());
  pti.SetField(pos_SSENV_ParseTypeInfo_thread,       Int(NONE));
  pti.SetField(pos_SSENV_ParseTypeInfo_constructors, Set());
  pti.SetField(pos_SSENV_ParseTypeInfo_fndefs,       Map());
  pti.SetField(pos_SSENV_ParseTypeInfo_subresps,     Map());
  pti.SetField(pos_SSENV_ParseTypeInfo_instinv,      Bool(false));
  return pti;
}
#endif //VDMPP

// ConstructImplDomType
// partps : AS`ParameterTypes (seq of PatTypePair)
// -> seq of AS`Type
SEQ<TYPE_AS_Type> StatSem::ConstructImplDomType (const SEQ<TYPE_AS_PatTypePair> & partps) const
{
  SEQ<TYPE_AS_Type> tp_l;

  size_t len_partps = partps.Length();
  for (size_t i = 1; i <= len_partps; i++)
  {
    const TYPE_AS_PatTypePair & ptp (partps[i]);
    const TYPE_AS_Type & tp (ptp.GetRecord(pos_AS_PatTypePair_tp));
    size_t len = ptp.GetSequence(pos_AS_PatTypePair_pats).Length();
    for(size_t j = 1; j <= len; j++)
      tp_l.ImpAppend(tp);
  }
  return tp_l;
}

// ConstructImplRngType
// nmtps : seq of AS`NameType
// -> AS`Type
TYPE_AS_Type StatSem::ConstructImplRngType (const SEQ<TYPE_AS_NameType> & nmtps) const
{
  switch(nmtps.Length()) {
    case 0:
      return TYPE_AS_VoidType().Init(NilContextId);
    case 1:
      return (nmtps[1].GetRecord(pos_AS_NameType_tp));
    default: {
      SEQ<TYPE_AS_Type> tps;
      size_t len = nmtps.Length();
      for (size_t i = 1; i <= len; i++)
        tps.ImpAppend(nmtps[i].GetRecord(pos_AS_NameType_tp));

      return TYPE_AS_ProductType().Init(tps, NilContextId);
    }
  }
}

#ifdef VDMPP
// SetStatic
// p_b : bool
void StatSem::SetStatic(const Bool & p_b)
{
  this->isStatic = p_b;
}

// UnsetStatic
void StatSem::UnsetStatic()
{
  this->isStatic = Bool(false);
}

// GetStatic
// ==> bool
Bool StatSem::GetStatic() const
{
  return this->isStatic;
}
#endif // VDMPP

//=======================================
// Not in spec 
//=======================================

#ifdef VDMSL
// PublicLookUpTypeName (not in spe)
// nm : AS`Name
// clnm : AS`Name
// ==> [REP`TypeRep]
Generic StatSem::PublicLookUpTypeName (const TYPE_AS_Name & nm, const TYPE_AS_Name & clnm)
{
  return LookUpTypeName (nm);
}
#endif // VDMSL

#ifdef VDMPP
// PublicLookUpTypeName (not in spe)
// nm : AS`Name
// clnm : AS`Name
// ==> [REP`TypeRep|AccessType] * [AS`Name]
Tuple StatSem::PublicLookUpTypeName (const TYPE_AS_Name & nm, const TYPE_AS_Name & clnm)
{
  SetCurClass(clnm);
  SetDefClass(clnm);
  Generic tp (LookUpTypeName (nm, true)); // [REP`TypeRep|ENV`AccessType]
  Generic foundCls (this->FoundClass); // bestoryed by CheckAccessCurClass

  Tuple res (mk_(CheckAccessCurClass (tp), foundCls));
  SetDefClass(Nil());
  return res;
}
#endif // VDMPP

// LookUpStateEnv
// nm : AS`Name
// ==> [REP`TypeRep]
Generic StatSem::LookUpStateEnv(const TYPE_AS_Name & nm)
{
  if (this->StateEnv.DomExists(nm))
  {
    const TYPE_SSENV_TypeRepElem & rc (this->StateEnv[nm]);
    const TYPE_REP_TypeRep & rsttp (rc.GetRecord(pos_SSENV_TypeRepElem_tp));
    if (rsttp.Is(TAG_TYPE_REP_InvTypeRep))
      return rsttp.GetRecord(pos_REP_InvTypeRep_shape);
    else
      return rsttp;
  }
  return Nil();
}

#ifdef VDMPP
// LookUpOpOrFnName
// classnm : [AS`Name]
// metnm : AS`Name
// ==> [REP`TypeRep]
Generic StatSem::LookUpOpOrFnName (const Generic & classnm, const TYPE_AS_Name & metnm)
{
// 20120721 -->
//  Generic tp (classnm.IsNil() ? StripAccessType (LookUp(metnm, true))
//                              : StripAccessType (LookUpInObject(classnm, metnm, false, true)));
  // TODO: Overload type returned by LookUpInObject
  Generic tp = Nil();
  if (classnm.IsNil())
    tp = StripAccessType (LookUp(metnm, true));
  else
  {
    Generic luio (LookUpInObject(classnm, metnm, false, true));
    if (luio.IsRecord())
      tp = StripAccessType(luio);
    else if (IsSetOfAccessType(luio))
    {
      Set atp_s (luio);
      Set tp_s;
      Generic atp;
      for (bool bb = atp_s.First(atp); bb; bb = atp_s.Next(atp))
        tp_s.Insert(StripAccessType(atp));
      tp = tp_s;
    }
  }
// <--20120721

  if (tp.Is(TAG_TYPE_REP_OpTypeRep) ||
      tp.Is(TAG_TYPE_REP_TotalFnTypeRep) ||
      tp.Is(TAG_TYPE_REP_PartialFnTypeRep))
  {
    return tp;
  }
// 20121221 -->
  else if (tp.Is(TAG_TYPE_REP_PolyTypeRep))
  {
    return Record(tp).GetRecord(pos_REP_PolyTypeRep_tp);
  }
// <-- 20121221
// 20120721 -->
  else if (tp.IsSet())
  {
    return tp;
  }
// <--20120721
  else
    return Nil();
}

// ExpandClassName
// name : AS`Name
// nm_s : set of AS`Name
// ==> bool * AS`Name
Tuple StatSem::ExpandClassName(const TYPE_AS_Name & name, const SET<TYPE_AS_Name> & nm_s)
{
  if (nm_s.InSet(name))
    return mk_(Bool(false), name);
  if (IsClassName(name))
    return mk_(Bool(true), name);
  else
  {
    Generic newtp (StripAccessType (LookUpTypeName(name, false)));
    if (newtp.Is(TAG_TYPE_REP_TypeNameRep))
    {
      TYPE_REP_TypeNameRep tnr (newtp);
      return ExpandClassName(tnr.GetRecord(pos_REP_TypeNameRep_nm), Set(nm_s).Insert(name));
    }
    return mk_(Bool(false), name);
  }
}
#endif //VDMPP

// GetTCState
// ==>
Tuple StatSem::GetTCState ()
{
#ifdef VDMSL
  return mk_(this->TypeEnv, this->RenamedTypes, this->TypeParEnv);
#endif // VDMSL
#ifdef VDMPP
  return mk_(this->TypeEnv, Nil(), Nil());
#endif // VDMPP
}

// SetTCState
// st :
void StatSem::SetTCState (const Tuple & st)
{
  this->TypeEnv      = st.GetField (1);
#ifdef VDMSL
  this->RenamedTypes = st.GetField (2);
  this->TypeParEnv   = st.GetField (3);
#endif // VDMSL
}

#ifdef VDMPP
// IsInstanceVar
// nm : AS`Name
// ==> bool
bool StatSem::IsInstanceVar(const TYPE_AS_Name & nm)
{
  return IsInstanceVar_q(nm, false);
}

// IsStaticInstanceVar
// nm : AS`Name
// ==> bool
bool StatSem::IsStaticInstanceVar(const TYPE_AS_Name & nm)
{
  return IsInstanceVar_q(nm, true);
}

// IsInstanceVari_q
// nm : AS`Name
// checkStatic : bool
// ==> bool
bool StatSem::IsInstanceVar_q(const TYPE_AS_Name & nm, bool checkStatic)
{
  if (nm.GetSequence(pos_AS_Name_ids).Length() == 2)
  {
    TYPE_AS_Name ncls (ASTAUX::GetFirstName(nm));
    TYPE_AS_Name nnm (ASTAUX::GetSecondName(nm));

    Generic cls (GetClassTypeRep(ncls));
    if (cls.IsNil())
      return false;
    else {
      TYPE_SSENV_ParseTypeInfo pti (cls);
      //MAP<TYPE_AS_Name,TYPE_SSENV_AccessTypeRep> vals (pti.get_insts ());
      const MAP<TYPE_AS_Name,TYPE_SSENV_AccessTypeRep> & insts (pti.GetMap(pos_SSENV_ParseTypeInfo_insts));
      const MAP<TYPE_AS_Name,TYPE_SSENV_AccessTypeRep> & vals (pti.GetMap(pos_SSENV_ParseTypeInfo_vals));
      if (insts.DomExists(nm)) {
        if (checkStatic)
          //return insts[nm].get_stat();
          return insts[nm].GetBoolValue(pos_SSENV_AccessTypeRep_stat);
        else
          return true;
      }
      else
        return vals.DomExists(nm);
    }
  }
  else {
// 20111222 -->
    //return this->StateEnv.DomExists(nm);
    if (this->StateEnv.DomExists(nm))
    {
      TYPE_SSENV_TypeRepElem tre (this->StateEnv[nm]);
      if (checkStatic)
        //return insts[nm].get_stat();
        return tre.GetBoolValue(pos_SSENV_TypeRepElem_stat);
      else
        return true;
    }
    else
      return false;
// <-- 20111222
  }
}

// LookUpInstanceVar
// nm : AS`Name
// ==> [REP`TypeRep]
Generic StatSem::LookUpInstanceVar(const TYPE_AS_Name & nm)
{
  if (nm.GetSequence(pos_AS_Name_ids).Length() == 2)
  {
    TYPE_AS_Name ncls (ASTAUX::GetFirstName(nm));
    TYPE_AS_Name nnm (ASTAUX::GetSecondName(nm));

    Generic cls (GetClassTypeRep(ncls));
    if (cls.IsNil())
      return Nil();
    else {
      TYPE_SSENV_ParseTypeInfo pti (cls);
      const MAP<TYPE_AS_Name,TYPE_SSENV_AccessTypeRep> & insts (pti.GetMap(pos_SSENV_ParseTypeInfo_insts));
      const MAP<TYPE_AS_Name,TYPE_SSENV_AccessTypeRep> & vals (pti.GetMap(pos_SSENV_ParseTypeInfo_vals));
      if (insts.DomExists(nm))
        return StripAccessType(insts[nm]);
      else if (vals.DomExists(nm))
        return StripAccessType(vals[nm]);
      else
        return Nil();
    }
  }
  else {
    if (this->StateEnv.DomExists(nm))
    {
      TYPE_SSENV_TypeRepElem tre (this->StateEnv[nm]);
      return tre.GetRecord(pos_SSENV_TypeRepElem_tp);
    }
    else
      return Nil();
  }
}

// HasInstInv
// nm : AS`Name
// ==> bool
bool StatSem::HasInstInv(const TYPE_AS_Name & nm)
{
  Generic pti (GetClassTypeRep(nm));
  if (pti.IsNil())
    return false;
  else
    return Record(pti).GetBoolValue(pos_SSENV_ParseTypeInfo_instinv);
}
#endif //VDMPP

void StatSem::PushVDContext(const TYPE_CI_ContextId & cid)
{
  this->ValueDefContext.ImpPrepend(cid);
}

void StatSem::PopVDContext()
{
  this->ValueDefContext.ImpTl();
}

bool StatSem::IsInVDContext()
{
  return !this->ValueDefContext.IsEmpty();
}

#endif //!NOSS
