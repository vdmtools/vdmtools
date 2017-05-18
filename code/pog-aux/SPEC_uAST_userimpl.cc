/***
*  * WHAT
*  *    
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/pog-aux/SPEC_uAST_userimpl.cc,v $
*  * VERSION
*  *    $Revision: 1.5 $
*  * DATE
*  *    $Date: 2001/11/08 12:49:44 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    IDERS-
*  * PROJECT
*  *    IDERS Project EP8593
*  * AUTHOR
*  *    $Author: mikhail $
*  * LOG
*  *    $Log: SPEC_uAST_userimpl.cc,v $
*  *    Revision 1.5  2001/11/08 12:49:44  mikhail
*  *    vdm_SPEC_uAST_GetASTs optimized
*  *
*  *    Revision 1.4  2001/11/02 13:34:11  mikhail
*  *    dead code removed
*  *
*  *    Revision 1.3  2001/10/22 12:20:20  paulm
*  *    Use of GetVDMASTs instead of GetASTs
*  *
*  *    Revision 1.2  2001/09/06 10:09:58  mikhail
*  *    ci after CVS crash
*  *
*  *    Revision 1.1.1.1  2001/08/09 11:52:16  sunesen
*  *    Import of code/pog-aux
*  *
*  *    Revision 1.1.1.1  2000/04/18 14:07:18  sunesen
*  *    directory prosper-common
*  *
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "astaux.h"
#include "tools.h"
#include "intconvquotes.h"

#ifdef DEF_SPEC_uAST_GetModuleName
Generic vdm_SPEC_uAST_GetModuleName ()
{
  return SPECAST::GetModuleName();
}
#endif // DEF_SPEC_uAST_GetModuleName

#ifdef DEF_SPEC_uAST_GetDefs
type_14AS_DefinitionsCL vdm_SPEC_uAST_GetDefs (const Generic & nm)
{
  return SPECAST::GetDefs(nm);
}
#endif // DEF_SPEC_uAST_GetDefs

#ifdef DEF_SPEC_uAST_GetTypeDefMap
type_7AS_NameC10AS_TypeDefCM vdm_SPEC_uAST_GetTypeDefMap ()
{
  return SPECAST::GetTypeDefMap();
}
#endif // DEF_SPEC_uAST_GetTypeDefMap

#ifdef DEF_SPEC_uAST_GetTypeDefMap_q
type_7AS_NameC10AS_TypeDefCM vdm_SPEC_uAST_GetTypeDefMap_q (const TYPE_AS_Name & mnm)
{
  return SPECAST::GetTypeDefMap_q(mnm);
}
#endif // DEF_SPEC_uAST_GetTypeDefMap_q

#ifdef DEF_SPEC_uAST_GetTypeDef
#ifdef VDMSL
Record vdm_SPEC_uAST_GetTypeDef (const TYPE_AS_Name & nm)
#endif // VDMSL
#ifdef VDMPP
TYPE_AS_TypeDef vdm_SPEC_uAST_GetTypeDef (const TYPE_AS_Name & nm)
#endif // VDMPP
{
  return SPECAST::GetTypeDef(nm);
}
#endif // DEF_SPEC_uAST_GetTypeDef

#ifdef VDMSL
#ifdef DEF_SPEC_uAST_GetStateDef
Generic vdm_SPEC_uAST_GetStateDef ()
{
  return SPECAST::GetStateDef();
}
#endif // DEF_SPEC_uAST_GetStateDef
#ifdef DEF_SPEC_uAST_GetStateDef_q
Generic vdm_SPEC_uAST_GetStateDef_q (const TYPE_AS_Name & mnm)
{
  return SPECAST::GetStateDef_q(mnm);
}
#endif // DEF_SPEC_uAST_GetStateDef_q
#endif // VDMSL

#ifdef DEF_SPEC_uAST_GetFnDefMap
type_7AS_NameCdM vdm_SPEC_uAST_GetFnDefMap (const TYPE_AS_Name & modnm)
{
  return SPECAST::GetFnDefMap(modnm);
}
#endif // DEF_SPEC_uAST_GetFnDefMap

#ifdef DEF_SPEC_uAST_GetFnDef
TYPE_AS_FnDef vdm_SPEC_uAST_GetFnDef (const TYPE_AS_Name & nm)
{
  return SPECAST::GetFnDef(nm);
}
#endif // DEF_SPEC_uAST_GetFnDef

#ifdef DEF_SPEC_uAST_GetOpDefMap
type_7AS_NameCdM vdm_SPEC_uAST_GetOpDefMap (const TYPE_AS_Name & modnm)
{
  return SPECAST::GetOpDefMap(modnm);
}
#endif // DEF_SPEC_uAST_GetOpDefMap

#ifdef DEF_SPEC_uAST_GetOpDef
TYPE_AS_OpDef vdm_SPEC_uAST_GetOpDef (const TYPE_AS_Name & nm)
{
  return SPECAST::GetOpDef(nm);
}
#endif // DEF_SPEC_uAST_GetOpDef

#ifdef DEF_SPEC_uAST_GetValueDefSeq
type_11AS_ValueDefCL vdm_SPEC_uAST_GetValueDefSeq ()
{
  return SPECAST::GetValueDefSeq();
}
#endif // DEF_SPEC_uAST_GetOpDef

#ifdef VDMPP
#ifdef DEF_SPEC_uAST_GetInstVars
type_dL vdm_SPEC_uAST_GetInstVars (const TYPE_AS_Name & modnm)
{
  return SPECAST::GetInstVars(modnm);
}
#endif // DEF_SPEC_uAST_GetInstVars
#endif // VDMPP

#ifdef DEF_SPEC_uAST_GetASTs
Generic vdm_SPEC_uAST_GetASTs ()
{
  return SPECAST::GetASTs();
}
#endif // DEF_SPEC_uAST_GetASTs

#ifdef DEF_SPEC_uAST_GetTypeDefNames
type_7AS_NameCL vdm_SPEC_uAST_GetTypeDefNames (const Generic & mnm)
{
  return SPECAST::GetTypeDefNames(mnm);
}
#endif // DEF_SPEC_uAST_GetTypeDefNames

#ifdef DEF_SPEC_uAST_GetFnDefNames
type_7AS_NameCL vdm_SPEC_uAST_GetFnDefNames (const Generic & mnm)
{
  return SPECAST::GetFnDefNames(mnm);
}
#endif // DEF_SPEC_uAST_GetFnDefNames

// GetModuleName
// ==> [AS`Name]
Generic SPECAST::GetModuleName ()
{
  return vdm_SPEC_uAST_currModName;
}

// GetDefs
// nm : [AS`Name]
// ==> seq of AS`Definitions
SEQ<TYPE_AS_Definitions> SPECAST::GetDefs (const Generic & nm)
{
  if (vdm_SPEC_uAST_defs.DomExists(nm))
  {
    return vdm_SPEC_uAST_defs[nm];
  }
  else
  {
    SEQ<TYPE_AS_Definitions> l_defs;
    Sequence l_asts (GetASTs());
#ifdef VDMSL
    if ((l_asts.Length () == 1) &&
        (Record (l_asts.Hd ()).Is (TAG_TYPE_AS_Definitions)))
    {
      l_defs.ImpConc(l_asts); 
    }
    else
    {
      Generic l_mod (GetModule(nm, l_asts));
      if (!l_mod.IsNil())
      {
        Generic defs (Record(l_mod).GetField(pos_AS_Module_defs));
        if (!defs.IsNil())
          l_defs.ImpAppend(defs);
      }
    }
#endif // VDMSL
#ifdef VDMPP
    Generic l_cls_g (GetModule(nm, l_asts));
    if (!l_cls_g.IsNil())
    {
      TYPE_AS_Class l_cls (l_cls_g);
      const SEQ<TYPE_AS_Name> & supercls (l_cls.GetSequence(pos_AS_Class_supercls)); 
      Map typem;
      Sequence valuem;
      Map fnm;
      Map opm;
      Sequence instvars;
      int len_supercls = supercls.Length();
      for (int index = 1; index <= len_supercls; index++)
      {
        SEQ<TYPE_AS_Definitions> lldefs (GetDefs(supercls[index]));
        if (!lldefs.IsEmpty())
        {
          const TYPE_AS_Definitions & ldefs (lldefs[1]);
          typem.ImpOverride(ldefs.GetMap(pos_AS_Definitions_typem));
          valuem.ImpConc(ldefs.GetSequence(pos_AS_Definitions_valuem));
          fnm.ImpOverride(ldefs.GetMap(pos_AS_Definitions_fnm));
          opm.ImpOverride(ldefs.GetMap(pos_AS_Definitions_opm));
          instvars.ImpConc(ldefs.GetSequence(pos_AS_Definitions_instvars));
        }
      }
      const Generic & l_cls_defs_g (l_cls.GetField(pos_AS_Class_defs));
      if (!l_cls_defs_g.IsNil())
      {
        TYPE_AS_Definitions l_cls_defs (l_cls_defs_g);
        typem.ImpOverride(l_cls_defs.GetMap(pos_AS_Definitions_typem));
        valuem.ImpConc(l_cls_defs.GetSequence(pos_AS_Definitions_valuem));
        fnm.ImpOverride(l_cls_defs.GetMap(pos_AS_Definitions_fnm));
        opm.ImpOverride(l_cls_defs.GetMap(pos_AS_Definitions_opm));
        instvars.ImpConc(l_cls_defs.GetSequence(pos_AS_Definitions_instvars));
      }
      TYPE_AS_Definitions defs;
      defs.Init(typem, valuem,fnm, opm, instvars,
                Sequence(),
                Nil(),
                Map(),
                NilContextId);
      l_defs.ImpAppend(defs);
    }
#endif // VDMPP
    vdm_SPEC_uAST_defs.ImpModify(nm, l_defs);
    return l_defs;
  }
}

// GetTypeDefMap
// ==> map AS`Name to AS`TypeDef
MAP<TYPE_AS_Name, TYPE_AS_TypeDef> SPECAST::GetTypeDefMap ()
{
  return GetTypeDefMap_q(vdm_SPEC_uAST_currModName);
}

// GetTypeDefMap'
// mnm : AS`Name
// ==> map AS`Name to AS`TypeDef
MAP<TYPE_AS_Name, TYPE_AS_TypeDef> SPECAST::GetTypeDefMap_q (const TYPE_AS_Name & mnm)
{
  SEQ<TYPE_AS_Definitions> defs (GetDefs(mnm));
  int len_defs = defs.Length();
  Map typem;
  for (int index = 1; index <= len_defs; index++)
  {
    const TYPE_AS_Definitions & ldefs (defs[index]);
    typem.ImpOverride(ldefs.GetMap(pos_AS_Definitions_typem));
  }
  return typem;
}

#ifdef VDMSL
// GetTypeDef
// nm : AS`Name
// ==> AS`TypeDef | AS`StateDef
Record SPECAST::GetTypeDef (const TYPE_AS_Name & nm)
#endif // VDMSL
#ifdef VDMPP
// GetTypeDef
// nm : AS`Name
// ==> AS`TypeDef
TYPE_AS_TypeDef SPECAST::GetTypeDef (const TYPE_AS_Name & nm)
#endif // VDMPP
{
  Tuple t (SplitName(nm));
  SEQ<TYPE_AS_Definitions> l_defs_q (GetDefs (t.GetRecord(1)));
  const TYPE_AS_Name & l_tnm (t.GetRecord(2));

  int len_l_defs_q = l_defs_q.Length();
  for (int index = 1; index <= len_l_defs_q; index++)
  {
    const TYPE_AS_Definitions & ldefs (l_defs_q[index]);
    const Map & typem (ldefs.GetMap(pos_AS_Definitions_typem));
    if (typem.DomExists(l_tnm))
      return typem[l_tnm];
  }
#ifdef VDMSL
  if (!l_defs_q.IsEmpty())
  {
    Generic state (l_defs_q[1].GetField(pos_AS_Definitions_State));
    if (!state.IsNil())
      return state;
  }
#endif // VDMSL
  wcout << L"TypeDef is't found" << endl;
  return TYPE_AS_TypeDef();
}

#ifdef VDMSL
// GetStateDef
// ==> [AS`StateDef]
Generic SPECAST::GetStateDef ()
{
  return GetStateDef_q(vdm_SPEC_uAST_currModName);
}

// GetStateDef'
// mnm : AS`Name
// ==> [AS`StateDef]
Generic SPECAST::GetStateDef_q (const TYPE_AS_Name & mnm)
{
  SEQ<TYPE_AS_Definitions> l_defs (GetDefs (mnm));
  if (!l_defs.IsEmpty())
  {
    return l_defs[1].GetField(pos_AS_Definitions_State);
  }
  return Nil();
}
#endif // VDMSL

// GetFnDefMap
// modnm : AS`Name
// ==> map AS`Name to AS`FnDef
MAP<TYPE_AS_Name, TYPE_AS_FnDef> SPECAST::GetFnDefMap (const TYPE_AS_Name & modnm)
{
  SEQ<TYPE_AS_Definitions> l_defs (GetDefs (modnm));
  MAP<TYPE_AS_Name, TYPE_AS_FnDef> res;
  int len_l_defs = l_defs.Length();
  for (int index = 1; index <= len_l_defs; index++)
  {
    res.ImpOverride(l_defs[index].GetMap(pos_AS_Definitions_fnm));
  }
  return res;
}

// GetFnDef
// nm : AS`Name
// ==> AS`FnDef
TYPE_AS_FnDef SPECAST::GetFnDef (const TYPE_AS_Name & nm)
{
  Tuple t (SplitName(nm));
  SEQ<TYPE_AS_Definitions> defs (GetDefs (t.GetRecord(1)));
  const TYPE_AS_Name & l_tnm (t.GetRecord(2));
  
  int len_defs = defs.Length();
  for (int index = 1; index <= len_defs; index++)
  {
    const TYPE_AS_Definitions & ldefs (defs[index]);
    const Map & fnm (ldefs.GetMap(pos_AS_Definitions_fnm));
    if (fnm.DomExists(l_tnm))
      return fnm[l_tnm];
  }
  wcout << L"fndef is't found" << endl;
  return TYPE_AS_FnDef();
}

// GetOpDefMap
// modnm : AS`Name
// ==> map AS`Name to AS`OpDef
MAP<TYPE_AS_Name, TYPE_AS_OpDef> SPECAST::GetOpDefMap (const TYPE_AS_Name & modnm)
{
  SEQ<TYPE_AS_Definitions> l_defs (GetDefs (modnm));
  MAP<TYPE_AS_Name, TYPE_AS_OpDef> res;
  int len_l_defs = l_defs.Length();
  for (int index = 1; index <= len_l_defs; index++)
  {
    res.ImpOverride(l_defs[index].GetMap(pos_AS_Definitions_opm));
  }
  return res;
}

// GetOpDef
// nm : AS`Name
//  ==> AS`OpDef
TYPE_AS_OpDef SPECAST::GetOpDef (const TYPE_AS_Name & nm)
{
  Tuple t (SplitName(nm));
  SEQ<TYPE_AS_Definitions> defs (GetDefs (t.GetRecord(1)));
  const TYPE_AS_Name & l_tnm (t.GetRecord(2));
  
  int len_defs = defs.Length();
  for (int index = 1; index <= len_defs; index++)
  {
    const TYPE_AS_Definitions & ldefs (defs[index]);
    const Map & opm (ldefs.GetMap(pos_AS_Definitions_opm));
    if (opm.DomExists(l_tnm))
      return opm[l_tnm];
  }
  wcout << L"opdef is't found" << endl;
  return TYPE_AS_OpDef();
}

// GetValueDefSeq
//  ==> seq of AS`ValueDef
SEQ<TYPE_AS_ValueDef> SPECAST::GetValueDefSeq ()
{
  SEQ<TYPE_AS_Definitions> l_defs (GetDefs (vdm_SPEC_uAST_currModName));
  SEQ<TYPE_AS_ValueDef> res;
  int len_l_defs = l_defs.Length();
  for (int index = 1; index <= len_l_defs; index++)
  {
    const TYPE_AS_Definitions & ldefs (l_defs[index]);
    res.ImpConc(ldefs.GetSequence(pos_AS_Definitions_valuem));
  }
  return res;
}

// GetTypeDefNames (not same as spec)
// mnm : [AS`Name]
// ==> seq of AS`Name
SEQ<TYPE_AS_Name> SPECAST::GetTypeDefNames(const Generic & mnm)
{
  Generic l_umnm;
  if (mnm.IsNil()) {
    l_umnm = vdm_SPEC_uAST_currModName;
  } else {
    l_umnm = mnm;
  }
  return MapToSeq(vdm_SPEC_uAST_GetTypeDefMap_q(l_umnm));
}

// GetFnDefNames (not same as spec)
// mnm : [AS`Name]
// ==> seq of AS`Name
SEQ<TYPE_AS_Name> SPECAST::GetFnDefNames(const Generic & mnm)
{
  Generic l_umnm;
  if (mnm.IsNil()) {
    l_umnm = vdm_SPEC_uAST_currModName;
  } else {
    l_umnm = mnm;
  }
  return MapToSeq(vdm_SPEC_uAST_GetFnDefMap(l_umnm));
}

#ifdef VDMPP
// GetInstVars
// modnm : AS`Name
// ==> seq of AS`InstanceVarDef
SEQ<TYPE_AS_InstanceVarDef> SPECAST::GetInstVars (const TYPE_AS_Name & modnm)
{
  SEQ<TYPE_AS_Definitions> l_defs (GetDefs (modnm));
  SEQ<TYPE_AS_InstanceVarDef> res;
  int len_l_defs = l_defs.Length();
  for (int index = 1; index <= len_l_defs; index++)
  {
    const TYPE_AS_Definitions & ldefs (l_defs[index]);
    res.ImpConc(ldefs.GetSequence(pos_AS_Definitions_instvars));
  }
  return res;
}
#endif // VDMPP

// SplitName
// nm : AS`Name
// ==> [AS`Name] * AS`Name
Tuple SPECAST::SplitName (const TYPE_AS_Name & nm)
{
  if (nm.GetSequence(pos_AS_Name_ids).Length() > 1)
  {
    return mk_(ASTAUX::GetFirstName(nm), ASTAUX::GetSecondName(nm)); 
  }
  else
  {
#ifdef VDMSL
    return mk_(vdm_SPEC_uAST_currModName, nm);
#endif // VDMSL
#ifdef VDMPP
    Generic defcls (GetStatSem().LookUpDefClass());
    if (defcls.IsNil())
      return mk_(vdm_SPEC_uAST_currModName, nm);
    else
      return mk_(defcls, nm);
#endif // VDMPP
  }
}

// GetCurrModName: () ==> [AS`Name]
// SetCurrModName: [AS`Name] ==> ()

// SetAst: AS`AstCI ==> ()

// GetASTs (not same as spec)
// ==> AS`Document | AS`Expr
Generic SPECAST::GetASTs ()
{
  if(vdm_SPEC_uAST_ast.IsNil())
  {
    SEQ<Record> ast_l (INT2Q::h2gAS(ToolMediator::GetVDMASTs ()));
#ifdef VICE
    ast_l.ImpAppend(INT2Q::h2gAS(GetCPUClassDef()));
    ast_l.ImpAppend(INT2Q::h2gAS(GetBUSClassDef()));
#endif // VICE
    vdm_SPEC_uAST_ast = ast_l;
  }
  return  vdm_SPEC_uAST_ast;
}

// GetAST: () ==> AS`AstCI

Sequence SPECAST::MapToSeq (const Map & m)
{
  return m.Dom().ToSequence();
}

Generic SPECAST::GetModule (const TYPE_AS_Name & nm, const Sequence & mods)
{
  int len_mods = mods.Length();
  for (int index = 1; index <= len_mods; index++)
  {
    const Record & r (mods[index]);
    switch(r.GetTag()) {
#ifdef VDMSL
      case TAG_TYPE_AS_Module: {
        if (r.GetRecord(pos_AS_Module_nm) == nm)
          return r;
      }
#endif // VDMSL
#ifdef VDMPP
      case TAG_TYPE_AS_Class: {
        if (r.GetRecord(pos_AS_Class_nm) == nm)
          return r;
      }
#endif // VDMPP
    }
  }
  return Nil();
}

#ifdef VICE
TYPE_AS_ExplOpDef SPECAST::GetCPUConstructor(const TYPE_AS_Name & opnm)
{
  TYPE_AS_NumericType btpnat;
  btpnat.Init(Int(NAT), NilContextId);
//  TYPE_AS_TypeName kindtn;
//  kindtn.Init(ASTAUX::MkNameFromId(ASTAUX::MkId(L"Kind"), NilContextId), NilContextId);
  TYPE_AS_QuoteLit fcfs_lit;
  fcfs_lit.Init(ASTAUX::MkId(L"FCFS"), NilContextId);
  TYPE_AS_QuoteType fcfs;
  fcfs.Init(fcfs_lit, NilContextId);
  TYPE_AS_QuoteLit fp_lit;
  fp_lit.Init(ASTAUX::MkId(L"FP"), NilContextId);
  TYPE_AS_QuoteType fp;
  fp.Init(fp_lit, NilContextId);
  SEQ<TYPE_AS_Type> tp_l;
  tp_l.ImpAppend(fcfs);
  tp_l.ImpAppend(fp);
  TYPE_AS_UnionType ut;
  ut.Init(tp_l, NilContextId);

  SEQ<TYPE_AS_Type> opdom;
//  opdom.ImpAppend(kindtn);
  opdom.ImpAppend(ut);
  opdom.ImpAppend(btpnat);

  TYPE_AS_TypeName cputn;
  cputn.Init(opnm, NilContextId);

  TYPE_AS_OpType tp;
  tp.Init(opdom, cputn, NilContextId);

  TYPE_AS_OpBody body;
  body.Init(Int(NOTYETSPEC), NilContextId); //

  TYPE_AS_PatternName pn1;
  pn1.Init(ASTAUX::MkNameFromId(ASTAUX::MkId(L"kind"), NilContextId), Nil(), NilContextId);
  TYPE_AS_PatternName pn2;
  pn2.Init(ASTAUX::MkNameFromId(ASTAUX::MkId(L"capacity"), NilContextId), Nil(), NilContextId);
  TYPE_AS_Parameters parms;
  parms.ImpAppend(pn1);
  parms.ImpAppend(pn2);

  TYPE_AS_ExplOpDef op;
  op.Init(opnm,
          Bool(false),
          Bool(true),
          tp,
          parms,
          body,
          Nil(),
          Nil(),
          Int(PUBLIC_AS), // 
          Bool(false),
          Bool(true),     // constructor
          NilContextId);
  return op;
}

TYPE_AS_ExplOpDef SPECAST::GetDeployOp(const TYPE_AS_Name & opnm)
{
  TYPE_AS_TypeName ortn;
  ortn.Init(ASTAUX::MkNameFromId(ASTAUX::MkId(L"OBJRef"), NilContextId), NilContextId);
  SEQ<TYPE_AS_Type> opdom;
  opdom.ImpAppend(ortn);

  TYPE_AS_OpType tp;
  tp.Init(opdom, Nil(), NilContextId);

  TYPE_AS_OpBody body;
  body.Init(Int(NOTYETSPEC), NilContextId); //

  TYPE_AS_PatternName pn1;
  pn1.Init(ASTAUX::MkNameFromId(ASTAUX::MkId(L"obj"), NilContextId), Nil(), NilContextId);
  TYPE_AS_Parameters parms;
  parms.ImpAppend(pn1);

  TYPE_AS_ExplOpDef op;
  op.Init(opnm,
          Bool(false),
          Bool(true),
          tp,
          parms,
          body,
          Nil(),
          Nil(),
          Int(PUBLIC_AS), //
          Bool(false),
          Bool(false),
          NilContextId);
  return op;
}

TYPE_AS_ExplOpDef SPECAST::GetSetPriorityOp(const TYPE_AS_Name & opnm)
{
  TYPE_AS_NumericType btpnat;
  btpnat.Init(Int(NAT), NilContextId);
  TYPE_AS_TypeName ortn;
  ortn.Init(ASTAUX::MkNameFromId(ASTAUX::MkId(L"OBJRef"), NilContextId), NilContextId);
  SEQ<TYPE_AS_Type> opdom;
  opdom.ImpAppend(ortn);
  opdom.ImpAppend(btpnat);

  TYPE_AS_OpType tp;
  tp.Init(opdom, Nil(), NilContextId);

  TYPE_AS_OpBody body;
  body.Init(Int(NOTYETSPEC), NilContextId); //

  TYPE_AS_PatternName pn1;
  pn1.Init(ASTAUX::MkNameFromId(ASTAUX::MkId(L"obj"), NilContextId), Nil(), NilContextId);
  TYPE_AS_PatternName pn2;
  pn2.Init(ASTAUX::MkNameFromId(ASTAUX::MkId(L"prio"), NilContextId), Nil(), NilContextId);
  TYPE_AS_Parameters parms;
  parms.ImpAppend(pn1);
  parms.ImpAppend(pn2);

  TYPE_AS_ExplOpDef op;
  op.Init(opnm,
          Bool(false),
          Bool(true),
          tp,
          parms,
          body,
          Nil(),
          Nil(),
          Int(PUBLIC_AS), //
          Bool(false),
          Bool(false),
          NilContextId);
  return op;
}

MAP<TYPE_AS_Name, TYPE_AS_OpDef> SPECAST::GetCPUOpDef()
{
  TYPE_AS_Name cpunm (ASTAUX::MkNameFromId(ASTAUX::MkId(L"CPU"), NilContextId));
  TYPE_AS_Name deploynm (ASTAUX::MkNameFromId(ASTAUX::MkId(L"deploy"), NilContextId));
  TYPE_AS_Name setPrioritynm (ASTAUX::MkNameFromId(ASTAUX::MkId(L"setPriority"), NilContextId));
  MAP<TYPE_AS_Name, TYPE_AS_OpDef> opm;
  opm.ImpModify(cpunm, GetCPUConstructor(cpunm));
  opm.ImpModify(deploynm, GetDeployOp(deploynm));
  opm.ImpModify(setPrioritynm, GetSetPriorityOp(setPrioritynm));
  return opm;
}

TYPE_AS_Definitions SPECAST::GetCPUDefinitions()
{
  TYPE_AS_Name kindnm (ASTAUX::MkNameFromId(ASTAUX::MkId(L"Kind"), NilContextId));
  TYPE_AS_TokenType btptkn;
  btptkn.Init(NilContextId);
  TYPE_AS_TypeDef td;
  td.Init(kindnm, btptkn, Nil(), Nil(), Nil(), Int(PUBLIC_AS), NilContextId); //

  MAP<TYPE_AS_Name, TYPE_AS_TypeDef> typem;
  typem.ImpModify(kindnm, td);
  
  TYPE_AS_Definitions def;
  def.Init(typem,
           SEQ<TYPE_AS_ValueDef>(),
           MAP<TYPE_AS_Name, TYPE_AS_FnDef>(),
           GetCPUOpDef(),
           SEQ<TYPE_AS_InstanceVarDef>(),
           SEQ<TYPE_AS_SyncDef>(),
           Nil(),
           Map(),
           NilContextId);
  return def;
}

TYPE_AS_Class SPECAST::GetCPUClassDef()
{
  TYPE_AS_Class cls;
  cls.Init(ASTAUX::MkNameFromId(ASTAUX::MkId(L"CPU"), NilContextId), // name
           Bool(false),                              // sys
           SEQ<TYPE_AS_Name>(),                      // supercls
           GetCPUDefinitions(),                      // defs
           Nil(),                                    // useslib
           NilContextId);
  return cls;
}

TYPE_AS_ExplOpDef SPECAST::GetBUSConstructor(const TYPE_AS_Name & opnm)
{
  TYPE_AS_NumericType btpnat;
  btpnat.Init(Int(NAT), NilContextId); //
//  TYPE_AS_TypeName kindtn;
//  kindtn.Init(ASTAUX::MkNameFromId(ASTAUX::MkId(L"Kind"), NilContextId), NilContextId);
  TYPE_AS_TypeName cputn;
  cputn.Init(ASTAUX::MkNameFromId(ASTAUX::MkId(L"CPU"), NilContextId), NilContextId);
  TYPE_AS_Set0Type st;
  st.Init(cputn, NilContextId);

  TYPE_AS_QuoteLit fcfs_lit;
  fcfs_lit.Init(ASTAUX::MkId(L"FCFS"), NilContextId);
  TYPE_AS_QuoteType fcfs;
  fcfs.Init(fcfs_lit, NilContextId);
  TYPE_AS_QuoteLit tdma_lit;
  tdma_lit.Init(ASTAUX::MkId(L"TDMA"), NilContextId);
  TYPE_AS_QuoteType tdma;
  tdma.Init(tdma_lit, NilContextId);
  TYPE_AS_QuoteLit csmacd_lit;
  csmacd_lit.Init(ASTAUX::MkId(L"FP"), NilContextId);
  TYPE_AS_QuoteType csmacd;
  csmacd.Init(csmacd_lit, NilContextId);

  SEQ<TYPE_AS_Type> tp_l;
  tp_l.ImpAppend(fcfs);
  tp_l.ImpAppend(tdma);
  tp_l.ImpAppend(csmacd);
  TYPE_AS_UnionType ut;
  ut.Init(tp_l, NilContextId);

  SEQ<TYPE_AS_Type> opdom;
//  opdom.ImpAppend(kindtn);
  opdom.ImpAppend(ut);
  opdom.ImpAppend(btpnat);
  opdom.ImpAppend(st);

  TYPE_AS_TypeName bustn;
  bustn.Init(opnm, NilContextId);

  TYPE_AS_OpType tp;
  tp.Init(opdom, bustn, NilContextId);

  TYPE_AS_OpBody body;
  body.Init(Int(NOTYETSPEC), NilContextId); //

  TYPE_AS_PatternName pn1;
  pn1.Init(ASTAUX::MkNameFromId(ASTAUX::MkId(L"kind"), NilContextId), Nil(), NilContextId);
  TYPE_AS_PatternName pn2;
  pn2.Init(ASTAUX::MkNameFromId(ASTAUX::MkId(L"capacity"), NilContextId), Nil(), NilContextId);
  TYPE_AS_PatternName pn3;
  pn3.Init(ASTAUX::MkNameFromId(ASTAUX::MkId(L"cpus"), NilContextId), Nil(), NilContextId);
  TYPE_AS_Parameters parms;
  parms.ImpAppend(pn1);
  parms.ImpAppend(pn2);
  parms.ImpAppend(pn3);

  TYPE_AS_ExplOpDef op;
  op.Init(opnm,
          Bool(false),
          Bool(true),
          tp,
          parms,
          body,
          Nil(),
          Nil(),
          Int(PUBLIC_AS), //
          Bool(false),
          Bool(true),       // constructor
          NilContextId);
  return op;
}

MAP<TYPE_AS_Name, TYPE_AS_OpDef> SPECAST::GetBUSOpDef()
{
  TYPE_AS_Name busnm (ASTAUX::MkNameFromId(ASTAUX::MkId(L"BUS"), NilContextId));
  Map opm;
  opm.ImpModify(busnm, GetBUSConstructor(busnm));
  return opm;
}

TYPE_AS_Definitions SPECAST::GetBUSDefinitions()
{
  TYPE_AS_Name kindnm (ASTAUX::MkNameFromId(ASTAUX::MkId(L"Kind"), NilContextId));
  TYPE_AS_TokenType btptkn;
  btptkn.Init(NilContextId); //
  TYPE_AS_TypeDef td;
  td.Init(kindnm, btptkn, Nil(), Nil(), Nil(), Int(PUBLIC_AS), NilContextId); //

  MAP<TYPE_AS_Name, TYPE_AS_TypeDef> typem;
  typem.ImpModify(kindnm, td);
  
  TYPE_AS_Definitions def;
  def.Init(typem,
           SEQ<TYPE_AS_ValueDef>(),
           MAP<TYPE_AS_Name, TYPE_AS_FnDef>(),
           GetBUSOpDef(),
           SEQ<TYPE_AS_InstanceVarDef>(),
           SEQ<TYPE_AS_SyncDef>(),
           Nil(),
           Map(),
           NilContextId);
  return def;
}

TYPE_AS_Class SPECAST::GetBUSClassDef()
{
  TYPE_AS_Class cls;
  cls.Init(ASTAUX::MkNameFromId(ASTAUX::MkId(L"BUS"), NilContextId), // nm
           Bool(false),                              // sys
           SEQ<TYPE_AS_Name>(),                      // supercls
           GetBUSDefinitions(),                      // defs
           Nil(),                                    // useslib
           NilContextId);
  return cls;
}
#endif // VICE

