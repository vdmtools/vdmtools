//
// THIS FILE IS AUTOMATICALLY GENERATED!!
//
// Generated at Wed 05-Apr-00 by the VDM-SLtoC++ Code Generator
// (v3.3.2 - Thu 02-Dec-99)
//
// Supported compilers:
// 	egcs version 1.1 on SunOS4, SunOS5, Linux, HP-UX10
// 	VC++ version 6.0 on Windows95, Windows NT
//

#ifndef _TRL_uTAR_uDEF_h
#define _TRL_uTAR_uDEF_h

#include <math.h>
#include "metaiv.h"
#include "cg.h"
#include "cg_aux.h"
#include "AS.h"
#include "CI.h"
#include "DEP_uANA.h"
#include "DER_uCON.h"
#include "DER_uINV.h"
#include "HOL.h"
#include "REP.h"
#include "TRANS.h"
#include "TRL_uTAR_uEXPR.h"
#include "TRL_uTAR_uPAT.h"
#include "TRL_uTAR_uTYPE.h"
#include "TRL_uTAR_uDEF_anonym.h"
enum  {
  vdm_TRL_uTAR_uDEF_ValueDef = TAG_TYPE_TRL_uTAR_uDEF_ValueDef,
  length_TRL_uTAR_uDEF_ValueDef = 2,
  pos_TRL_uTAR_uDEF_ValueDef_var = 1,
  pos_TRL_uTAR_uDEF_ValueDef_expr = 2,
  vdm_TRL_uTAR_uDEF_const_udef = TAG_TYPE_TRL_uTAR_uDEF_const_udef,
  length_TRL_uTAR_uDEF_const_udef = 4,
  pos_TRL_uTAR_uDEF_const_udef_name = 1,
  pos_TRL_uTAR_uDEF_const_udef_type = 2,
  pos_TRL_uTAR_uDEF_const_udef_expr = 3,
  pos_TRL_uTAR_uDEF_const_udef_fv = 4,
  vdm_TRL_uTAR_uDEF_datatype_udef = TAG_TYPE_TRL_uTAR_uDEF_datatype_udef,
  length_TRL_uTAR_uDEF_datatype_udef = 5,
  pos_TRL_uTAR_uDEF_datatype_udef_name = 1,
  pos_TRL_uTAR_uDEF_datatype_udef_type = 2,
  pos_TRL_uTAR_uDEF_datatype_udef_fnsel = 3,
  pos_TRL_uTAR_uDEF_datatype_udef_fninv = 4,
  pos_TRL_uTAR_uDEF_datatype_udef_fv = 5,
  vdm_TRL_uTAR_uDEF_func_udef = TAG_TYPE_TRL_uTAR_uDEF_func_udef,
  length_TRL_uTAR_uDEF_func_udef = 9,
  pos_TRL_uTAR_uDEF_func_udef_name = 1,
  pos_TRL_uTAR_uDEF_func_udef_fntype = 2,
  pos_TRL_uTAR_uDEF_func_udef_parms = 3,
  pos_TRL_uTAR_uDEF_func_udef_parmstp = 4,
  pos_TRL_uTAR_uDEF_func_udef_body = 5,
  pos_TRL_uTAR_uDEF_func_udef_bodytp = 6,
  pos_TRL_uTAR_uDEF_func_udef_fnpre = 7,
  pos_TRL_uTAR_uDEF_func_udef_fnpost = 8,
  pos_TRL_uTAR_uDEF_func_udef_fv = 9,
  vdm_TRL_uTAR_uDEF_po_udef = TAG_TYPE_TRL_uTAR_uDEF_po_udef,
  length_TRL_uTAR_uDEF_po_udef = 3,
  pos_TRL_uTAR_uDEF_po_udef_name = 1,
  pos_TRL_uTAR_uDEF_po_udef_expr = 2,
  pos_TRL_uTAR_uDEF_po_udef_fv = 3,
  vdm_TRL_uTAR_uDEF_pred_udef = TAG_TYPE_TRL_uTAR_uDEF_pred_udef,
  length_TRL_uTAR_uDEF_pred_udef = 6,
  pos_TRL_uTAR_uDEF_pred_udef_name = 1,
  pos_TRL_uTAR_uDEF_pred_udef_fntype = 2,
  pos_TRL_uTAR_uDEF_pred_udef_parms = 3,
  pos_TRL_uTAR_uDEF_pred_udef_parmstp = 4,
  pos_TRL_uTAR_uDEF_pred_udef_body = 5,
  pos_TRL_uTAR_uDEF_pred_udef_fv = 6,
  vdm_TRL_uTAR_uDEF_select_udef = TAG_TYPE_TRL_uTAR_uDEF_select_udef,
  length_TRL_uTAR_uDEF_select_udef = 6,
  pos_TRL_uTAR_uDEF_select_udef_name = 1,
  pos_TRL_uTAR_uDEF_select_udef_fntype = 2,
  pos_TRL_uTAR_uDEF_select_udef_parms = 3,
  pos_TRL_uTAR_uDEF_select_udef_parmstp = 4,
  pos_TRL_uTAR_uDEF_select_udef_body = 5,
  pos_TRL_uTAR_uDEF_select_udef_fv = 6,
  vdm_TRL_uTAR_uDEF_type_uabbrev_udef = TAG_TYPE_TRL_uTAR_uDEF_type_uabbrev_udef,
  length_TRL_uTAR_uDEF_type_uabbrev_udef = 4,
  pos_TRL_uTAR_uDEF_type_uabbrev_udef_name = 1,
  pos_TRL_uTAR_uDEF_type_uabbrev_udef_type = 2,
  pos_TRL_uTAR_uDEF_type_uabbrev_udef_fninv = 3,
  pos_TRL_uTAR_uDEF_type_uabbrev_udef_fv = 4
};

class TYPE_TRL_uTAR_uDEF_ValueDef : public Record {
public:

  TYPE_TRL_uTAR_uDEF_ValueDef() : Record(TAG_TYPE_TRL_uTAR_uDEF_ValueDef, 2) {}
  void Init(TYPE_TRL_uTAR_uEXPR_Expr p2, TYPE_TRL_uTAR_uEXPR_Expr p3);

  TYPE_TRL_uTAR_uDEF_ValueDef(const Generic &c) : Record(c) {}

  const char * GetTypeName() const { return "TYPE_TRL_uTAR_uDEF_ValueDef"; }
  TYPE_TRL_uTAR_uEXPR_Expr get_var() const;
  void set_var(const TYPE_TRL_uTAR_uEXPR_Expr &p);
  TYPE_TRL_uTAR_uEXPR_Expr get_expr() const;
  void set_expr(const TYPE_TRL_uTAR_uEXPR_Expr &p);
} ;

class TYPE_TRL_uTAR_uDEF_const_udef : public Record {
public:

  TYPE_TRL_uTAR_uDEF_const_udef() : Record(TAG_TYPE_TRL_uTAR_uDEF_const_udef, 4) {}
  void Init(TYPE_TRL_uTAR_uDEF_string p2, TYPE_HOL_term_utype p3, TYPE_HOL_term p4, type_cLS p5);

  TYPE_TRL_uTAR_uDEF_const_udef(const Generic &c) : Record(c) {}

  const char * GetTypeName() const { return "TYPE_TRL_uTAR_uDEF_const_udef"; }
  TYPE_TRL_uTAR_uDEF_string get_name() const;
  void set_name(const TYPE_TRL_uTAR_uDEF_string &p);
  TYPE_HOL_term_utype get_type() const;
  void set_type(const TYPE_HOL_term_utype &p);
  TYPE_HOL_term get_expr() const;
  void set_expr(const TYPE_HOL_term &p);
  type_cLS get_fv() const;
  void set_fv(const type_cLS &p);
} ;

class TYPE_TRL_uTAR_uDEF_datatype_udef : public Record {
public:

  TYPE_TRL_uTAR_uDEF_datatype_udef() : Record(TAG_TYPE_TRL_uTAR_uDEF_datatype_udef, 5) {}
  void Init(TYPE_TRL_uTAR_uDEF_string p2, TYPE_HOL_datatype p3, Generic p4, Generic p5, type_cLS p6);

  TYPE_TRL_uTAR_uDEF_datatype_udef(const Generic &c) : Record(c) {}

  const char * GetTypeName() const { return "TYPE_TRL_uTAR_uDEF_datatype_udef"; }
  TYPE_TRL_uTAR_uDEF_string get_name() const;
  void set_name(const TYPE_TRL_uTAR_uDEF_string &p);
  TYPE_HOL_datatype get_type() const;
  void set_type(const TYPE_HOL_datatype &p);
  Generic get_fnsel() const;
  void set_fnsel(const Generic &p);
  Generic get_fninv() const;
  void set_fninv(const Generic &p);
  type_cLS get_fv() const;
  void set_fv(const type_cLS &p);
} ;

class TYPE_TRL_uTAR_uDEF_func_udef : public Record {
public:

  TYPE_TRL_uTAR_uDEF_func_udef() : Record(TAG_TYPE_TRL_uTAR_uDEF_func_udef, 9) {}
  void Init(TYPE_TRL_uTAR_uDEF_string p2, TYPE_HOL_term_utype p3, type_FL p4, type_FL p5, TYPE_HOL_term p6, TYPE_HOL_term_utype p7, Generic p8, Generic p9, type_cLS p10);

  TYPE_TRL_uTAR_uDEF_func_udef(const Generic &c) : Record(c) {}

  const char * GetTypeName() const { return "TYPE_TRL_uTAR_uDEF_func_udef"; }
  TYPE_TRL_uTAR_uDEF_string get_name() const;
  void set_name(const TYPE_TRL_uTAR_uDEF_string &p);
  TYPE_HOL_term_utype get_fntype() const;
  void set_fntype(const TYPE_HOL_term_utype &p);
  type_FL get_parms() const;
  void set_parms(const type_FL &p);
  type_FL get_parmstp() const;
  void set_parmstp(const type_FL &p);
  TYPE_HOL_term get_body() const;
  void set_body(const TYPE_HOL_term &p);
  TYPE_HOL_term_utype get_bodytp() const;
  void set_bodytp(const TYPE_HOL_term_utype &p);
  Generic get_fnpre() const;
  void set_fnpre(const Generic &p);
  Generic get_fnpost() const;
  void set_fnpost(const Generic &p);
  type_cLS get_fv() const;
  void set_fv(const type_cLS &p);
} ;

class TYPE_TRL_uTAR_uDEF_po_udef : public Record {
public:

  TYPE_TRL_uTAR_uDEF_po_udef() : Record(TAG_TYPE_TRL_uTAR_uDEF_po_udef, 3) {}
  void Init(TYPE_TRL_uTAR_uDEF_string p2, TYPE_HOL_term p3, type_cLS p4);

  TYPE_TRL_uTAR_uDEF_po_udef(const Generic &c) : Record(c) {}

  const char * GetTypeName() const { return "TYPE_TRL_uTAR_uDEF_po_udef"; }
  TYPE_TRL_uTAR_uDEF_string get_name() const;
  void set_name(const TYPE_TRL_uTAR_uDEF_string &p);
  TYPE_HOL_term get_expr() const;
  void set_expr(const TYPE_HOL_term &p);
  type_cLS get_fv() const;
  void set_fv(const type_cLS &p);
} ;

class TYPE_TRL_uTAR_uDEF_pred_udef : public Record {
public:

  TYPE_TRL_uTAR_uDEF_pred_udef() : Record(TAG_TYPE_TRL_uTAR_uDEF_pred_udef, 6) {}
  void Init(TYPE_TRL_uTAR_uDEF_string p2, TYPE_HOL_term_utype p3, type_FL p4, type_FL p5, TYPE_HOL_term p6, type_cLS p7);

  TYPE_TRL_uTAR_uDEF_pred_udef(const Generic &c) : Record(c) {}

  const char * GetTypeName() const { return "TYPE_TRL_uTAR_uDEF_pred_udef"; }
  TYPE_TRL_uTAR_uDEF_string get_name() const;
  void set_name(const TYPE_TRL_uTAR_uDEF_string &p);
  TYPE_HOL_term_utype get_fntype() const;
  void set_fntype(const TYPE_HOL_term_utype &p);
  type_FL get_parms() const;
  void set_parms(const type_FL &p);
  type_FL get_parmstp() const;
  void set_parmstp(const type_FL &p);
  TYPE_HOL_term get_body() const;
  void set_body(const TYPE_HOL_term &p);
  type_cLS get_fv() const;
  void set_fv(const type_cLS &p);
} ;

class TYPE_TRL_uTAR_uDEF_select_udef : public Record {
public:

  TYPE_TRL_uTAR_uDEF_select_udef() : Record(TAG_TYPE_TRL_uTAR_uDEF_select_udef, 6) {}
  void Init(TYPE_TRL_uTAR_uDEF_string p2, TYPE_HOL_term_utype p3, type_FL p4, type_FL p5, TYPE_HOL_term p6, type_cLS p7);

  TYPE_TRL_uTAR_uDEF_select_udef(const Generic &c) : Record(c) {}

  const char * GetTypeName() const { return "TYPE_TRL_uTAR_uDEF_select_udef"; }
  TYPE_TRL_uTAR_uDEF_string get_name() const;
  void set_name(const TYPE_TRL_uTAR_uDEF_string &p);
  TYPE_HOL_term_utype get_fntype() const;
  void set_fntype(const TYPE_HOL_term_utype &p);
  type_FL get_parms() const;
  void set_parms(const type_FL &p);
  type_FL get_parmstp() const;
  void set_parmstp(const type_FL &p);
  TYPE_HOL_term get_body() const;
  void set_body(const TYPE_HOL_term &p);
  type_cLS get_fv() const;
  void set_fv(const type_cLS &p);
} ;

class TYPE_TRL_uTAR_uDEF_type_uabbrev_udef : public Record {
public:

  TYPE_TRL_uTAR_uDEF_type_uabbrev_udef() : Record(TAG_TYPE_TRL_uTAR_uDEF_type_uabbrev_udef, 4) {}
  void Init(TYPE_TRL_uTAR_uDEF_string p2, TYPE_HOL_term_utype p3, Generic p4, type_cLS p5);

  TYPE_TRL_uTAR_uDEF_type_uabbrev_udef(const Generic &c) : Record(c) {}

  const char * GetTypeName() const { return "TYPE_TRL_uTAR_uDEF_type_uabbrev_udef"; }
  TYPE_TRL_uTAR_uDEF_string get_name() const;
  void set_name(const TYPE_TRL_uTAR_uDEF_string &p);
  TYPE_HOL_term_utype get_type() const;
  void set_type(const TYPE_HOL_term_utype &p);
  Generic get_fninv() const;
  void set_fninv(const Generic &p);
  type_cLS get_fv() const;
  void set_fv(const type_cLS &p);
} ;
void init_TRL_uTAR_uDEF();
type_FLL vdm_TRL_uTAR_uDEF_ConvertFuncs2Hol(const type_dLL &);
type_11HOL_po_udefCL vdm_TRL_uTAR_uDEF_ConvertProofObligations2Hol(const type_21TRL_uTAR_uDEF_po_udefCL &);
type_FLL vdm_TRL_uTAR_uDEF_ConvertTypes2Hol(const type_dLL &);
TYPE_TRL_uTAR_uDEF_TypeDef vdm_TRL_uTAR_uDEF_DatatypeDef(const TYPE_TRL_uTAR_uEXPR_NameName &, const TYPE_TRL_uTAR_uTYPE_Type &, const Generic &);
Bool vdm_TRL_uTAR_uDEF_pre_DatatypeDef(const TYPE_TRL_uTAR_uEXPR_NameName &, const TYPE_TRL_uTAR_uTYPE_Type &, const Generic &);
type_7AS_NameC23TRL_uTAR_uDEF_pred_udefCM vdm_TRL_uTAR_uDEF_ExtractPredicatesInFuncs(const type_7AS_NameCdM &);
type_7AS_NameC23TRL_uTAR_uDEF_pred_udefCM vdm_TRL_uTAR_uDEF_ExtractPredicatesInTypes(const type_7AS_NameCdM &);
type_21TRL_uTAR_uDEF_po_udefCL vdm_TRL_uTAR_uDEF_ExtractProofObligationsInFuncs(const type_7AS_NameCdM &);
type_7AS_NameC25TRL_uTAR_uDEF_select_udefCM vdm_TRL_uTAR_uDEF_ExtractSelectorsInTypes(const type_7AS_NameCdM &);
type_FF2P vdm_TRL_uTAR_uDEF_MakeConstructor(const type_cL &, const TYPE_HOL_record &);
type_7AS_NameC23TRL_uTAR_uDEF_func_udefCM vdm_TRL_uTAR_uDEF_RemoveProofObligationsInFuncs(const type_7AS_NameCdM &);
type_typevariableL vdm_TRL_uTAR_uDEF_SetToSeq(const type_typevariableS &);
TYPE_TRL_uTAR_uDEF_TypeDef vdm_TRL_uTAR_uDEF_TypeAbbrevDef(const TYPE_TRL_uTAR_uEXPR_NameName &, const TYPE_TRL_uTAR_uTYPE_Type &, const Generic &);
Bool vdm_TRL_uTAR_uDEF_pre_TypeAbbrevDef(const TYPE_TRL_uTAR_uEXPR_NameName &, const TYPE_TRL_uTAR_uTYPE_Type &, const Generic &);
type_7AS_NameC24TRL_uTAR_uDEF_const_udefCM vdm_TRL_uTAR_uDEF_ValueSeq2Map(const type_22TRL_uTAR_uDEF_ValueDefCL &);
TYPE_TRL_uTAR_uDEF_AstCI vdm_TRL_uTAR_uDEF_tar_uAstCI(const Record &, const type_19CI_TokenContextInfoCL &, const TYPE_CI_FileIdMap &);
TYPE_TRL_uTAR_uDEF_Definitions vdm_TRL_uTAR_uDEF_tar_uDefinitions(const type_7AS_NameCdM &, const type_22TRL_uTAR_uDEF_ValueDefCL &, const type_7AS_NameCdM &, const type_7AS_NameCFM &, const Generic &, const TYPE_CI_ContextId &);
TYPE_TRL_uTAR_uDEF_Document vdm_TRL_uTAR_uDEF_tar_uDocument(const type_UL &);
Bool vdm_TRL_uTAR_uDEF_pre_tar_uDocument(const type_UL &);
TYPE_TRL_uTAR_uDEF_ExplFnDef vdm_TRL_uTAR_uDEF_tar_uExplFunction(const TYPE_TRL_uTAR_uEXPR_NameName &, const TYPE_TRL_uTAR_uDEF_TypeVarList &, const TYPE_TRL_uTAR_uTYPE_FnType &, const TYPE_TRL_uTAR_uDEF_ParametersList &, const TYPE_TRL_uTAR_uDEF_FnBody &, const Generic &, const Generic &, const TYPE_CI_ContextId &);
TYPE_TRL_uTAR_uDEF_ExtExplFnDef vdm_TRL_uTAR_uDEF_tar_uExtExplFunction(const TYPE_TRL_uTAR_uEXPR_NameName &, const TYPE_TRL_uTAR_uDEF_TypeVarList &, const TYPE_TRL_uTAR_uDEF_ParameterTypes &, const type_19TRL_uTAR_uEXPR_EXPRCL &, const TYPE_TRL_uTAR_uDEF_FnBody &, const Generic &, const Generic &, const TYPE_CI_ContextId &);
TYPE_TRL_uTAR_uDEF_FnBody vdm_TRL_uTAR_uDEF_tar_uFnBody(const Generic &, const TYPE_CI_ContextId &);
type_22TRL_uTAR_uDEF_ValueDefCL vdm_TRL_uTAR_uDEF_tar_uGlobalValueDef(const type_22TRL_uTAR_uDEF_ValueDefCL &);
TYPE_TRL_uTAR_uDEF_ImplFnDef vdm_TRL_uTAR_uDEF_tar_uImplFunction(const TYPE_TRL_uTAR_uEXPR_NameName &, const TYPE_TRL_uTAR_uDEF_TypeVarList &, const TYPE_TRL_uTAR_uDEF_ParameterTypes &, const type_19TRL_uTAR_uEXPR_EXPRCL &, const Generic &, const TYPE_TRL_uTAR_uEXPR_Expr &, const TYPE_CI_ContextId &);
TYPE_TRL_uTAR_uDEF_Invariant vdm_TRL_uTAR_uDEF_tar_uInvariant(const TYPE_TRL_uTAR_uPAT_Pattern &, const TYPE_TRL_uTAR_uEXPR_Expr &, const TYPE_CI_ContextId &);
TYPE_TRL_uTAR_uDEF_Module vdm_TRL_uTAR_uDEF_tar_uModule(const TYPE_TRL_uTAR_uEXPR_NameName &, const TYPE_TRL_uTAR_uDEF_Interface &, const TYPE_TRL_uTAR_uDEF_Definitions &, const TYPE_CI_ContextId &);
TYPE_TRL_uTAR_uDEF_NameType vdm_TRL_uTAR_uDEF_tar_uNameType(const TYPE_TRL_uTAR_uEXPR_NameName &, const TYPE_TRL_uTAR_uTYPE_Type &, const TYPE_CI_ContextId &);
TYPE_TRL_uTAR_uDEF_PatTypePair vdm_TRL_uTAR_uDEF_tar_uPatTypePair(const type_19TRL_uTAR_uEXPR_EXPRCL &, const TYPE_TRL_uTAR_uTYPE_Type &, const TYPE_CI_ContextId &);
TYPE_TRL_uTAR_uDEF_TypeDef vdm_TRL_uTAR_uDEF_tar_uTypeDef(const TYPE_TRL_uTAR_uEXPR_NameType &, const TYPE_AS_Type &, const TYPE_TRL_uTAR_uTYPE_Type &, const Generic &, const TYPE_CI_ContextId &);
TYPE_TRL_uTAR_uDEF_ValueDef vdm_TRL_uTAR_uDEF_tar_uValueDef(const TYPE_TRL_uTAR_uPAT_Pattern &, const Generic &, const TYPE_TRL_uTAR_uEXPR_Expr &, const TYPE_CI_ContextId &);

#endif

