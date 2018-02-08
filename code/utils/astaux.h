/***
*  * WHAT
*  *    Functions working on the AST nodes
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/utils/astaux.h,v $
*  * VERSION
*  *    $Revision: 1.25 $
*  * DATE
*  *    $Date: 2006/07/03 05:21:15 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    Toolbox
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
*  * SPECIFICATION VERSION
*  *    None -- This file does not correspond to any specifications
***/

#ifndef __astaux_h__
#define __astaux_h__

#include "AS.h"
#include "contextinfo.h"

class ASTAUX
{
public:
  static TYPE_AS_Id GetFirstId(const TYPE_AS_Name & nm)
  { return nm.GetSequence(pos_AS_Name_ids).Index(1); }

  static TYPE_AS_Id GetSecondId(const TYPE_AS_Name & nm)
  { return nm.GetSequence(pos_AS_Name_ids).Index(2); };

  static TYPE_AS_Id GetLastId(const TYPE_AS_Name & nm)
  { return nm.GetSequence(pos_AS_Name_ids).Last(); };

  static TYPE_AS_Id MkId(const wstring & str) { return SEQ<Char>(str); };

  static TYPE_AS_Name MkNameFromVoid (void)
  { return TYPE_AS_Name().Init(TYPE_AS_Ids(), NilContextId); };

  static TYPE_AS_Name MkNameFromId (const TYPE_AS_Id & id, const TYPE_CI_ContextId & cid)
  {return TYPE_AS_Name().Init(TYPE_AS_Ids().ImpAppend(id), cid); };

  static TYPE_AS_Name MkNameFromIds (const TYPE_AS_Ids & ids, const TYPE_CI_ContextId & cid)
  { return TYPE_AS_Name().Init(ids, cid); };

  static TYPE_AS_Name MkName (const wstring & name);

  static wstring Id2String(const TYPE_AS_Id &);
  static wstring ASName2String(const TYPE_AS_Name & asname);

  static TYPE_CI_ContextId GetCid(const Record &);
  static bool IsASTRec(const Record &);
  static TYPE_AS_Name GetFirstName(const TYPE_AS_Name &);
  static TYPE_AS_Name GetSecondName(const TYPE_AS_Name &);
  static TYPE_AS_Name Combine2Names(const TYPE_AS_Name &, const TYPE_AS_Name &);
  static TYPE_AS_Access GetOpAccess(const TYPE_AS_OpDef &);
  static TYPE_AS_Access GetFnAccess(const TYPE_AS_FnDef &);
  static TYPE_AS_OpBody GetOpBody(const TYPE_AS_OpDef &);
  static TYPE_AS_FnBody GetFnBody(const TYPE_AS_FnDef &);
  static TYPE_AS_Name GetOpName(const TYPE_AS_OpDef &);
  static TYPE_AS_Name GetFnName(const TYPE_AS_FnDef &);
  static Bool GetOpStatic(const TYPE_AS_OpDef &);
  static Bool GetOpPureOp(const TYPE_AS_OpDef &);
  static Bool GetFnStatic(const TYPE_AS_FnDef &);
  static bool GetConstr (const TYPE_AS_OpDef & opdef);
  static Generic GetOpPrecond (const TYPE_AS_OpDef & opdef);
  static Generic GetFnPrecond (const TYPE_AS_FnDef & fndef);
  static Generic GetOpPostcond (const TYPE_AS_OpDef & opdef);
  static Generic GetFnPostcond (const TYPE_AS_FnDef & fndef);
  static TYPE_AS_Name UnqualiName(const TYPE_AS_Name &);
  static SEQ<TYPE_AS_Type> GetFnParms(const TYPE_AS_FnDef &);
  static SEQ<TYPE_AS_Type> GetOpParms(const TYPE_AS_OpDef &);
  static TYPE_AS_Type GetFnRestype(const TYPE_AS_FnDef &);
  static SEQ<TYPE_AS_Type> ConstructImplDomType(const SEQ<TYPE_AS_PatTypePair>&);
  static TYPE_AS_Type ConstructImplRngType(const SEQ<TYPE_AS_NameType> &);
  static Generic GetFnMeasu(const TYPE_AS_FnDef &);

  static Sequence SetToSequenceR(const Set & s);

  static TYPE_AS_MultBind BindToMultBind(const TYPE_AS_Bind & bind);
  static SEQ<TYPE_AS_MultBind> BindToBindList(const TYPE_AS_Bind & bind);
  static SEQ<TYPE_AS_Bind> MultBindToBindSeq(const TYPE_AS_MultBind & bind);
  static SEQ<TYPE_AS_Bind> BindListToBindSeq(const SEQ<TYPE_AS_MultBind> & bind_l);
  static SEQ<TYPE_AS_MultBind> MargeBindList(const SEQ<TYPE_AS_MultBind> & bind_l);

#ifdef VDMSL
  static wstring GetDefaultModName();
  static TYPE_AS_Name GetDefaultModASName();
  static Generic DefaultModASName;
#endif // VDMSL
#ifdef VDMPP
  static TYPE_AS_Document ChangeDocumentToStatic(const TYPE_AS_Document & cs);
  static TYPE_AS_Class ChangeClassToStatic(const TYPE_AS_Class & cl);
  static bool IsSubrespFnOp(const Record & def);
#endif // VDMPP
};

#endif // __astaux_h__
