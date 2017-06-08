/***
*  * WHAT
*  *    Implementation of mod_valdef.vdm
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_valdef.h,v $
*  * VERSION
*  *    $Revision: 1.10 $
*  * DATE
*  *    $Date: 2001/12/17 09:44:36 $
*  * STATUS
*  *    Under development $State: Exp $
*  * PROJECT
*  *    AFRODITE
*  * AUTHOR
*  *    Poul Boegh Lassen (PBL) + $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __mod_valdef_h__
#define __mod_valdef_h__

private:
  Set loc_s;                 // set of (AS`Name | AS`OldName)
  Sequence sl_l;             // seq of (set of AS`Name)
  SET<TYPE_AS_Name> directdefinedValues;
#ifdef VDMPP
  Sequence decltemp;         // seq of CPP`IdentDeclaration
#endif //VDMPP

private:
#ifdef VDMSL
  Tuple GenValDef (const SEQ<TYPE_AS_ValueDef> & vd_l);
  Sequence DeclValId (const Map &);
#endif //VDMSL
#ifdef VDMPP
  Tuple GenValDef(const TYPE_AS_Name &, const SEQ<TYPE_AS_ValueDef> &);
  Tuple DeclValId(const TYPE_AS_Name &, const Map &);

  bool AreDefinedDirectly(const SEQ<TYPE_AS_ValueDef> & vd_l);
  bool IsDefinedDirectly(const TYPE_AS_ValueDef & vdef, const Map & pid_m);

  SEQ<TYPE_CPP_IdentDeclaration> DeclValJava(const TYPE_AS_Name &, const Map &, const SEQ<TYPE_AS_ValueDef> &);
  Map GenValDefAccessMap (const SEQ<TYPE_AS_ValueDef> &);
  SEQ<TYPE_CPP_Stmt> GenAssignTemp(const SET<TYPE_AS_Name> &);
#endif //VDMPP
  SEQ<TYPE_CPP_Stmt> GenLocalValDef(const SEQ<TYPE_AS_ValueDef> &);
  bool InsertLoc(const TYPE_AS_PatternName &);
  void PushLoc(const Set &);
  void DeleteLoc();
  void PushFctLoc(const Set &);

#endif // __mod_valdef_h__
