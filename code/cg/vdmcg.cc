/***
*  * WHAT
*  *    Implementation of Code Generator Front-End.
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/vdmcg.cc,v $
*  * VERSION
*  *    $Revision: 1.35 $
*  * DATE
*  *    $Date: 2006/06/23 05:34:19 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    IDERES/AFRODITE
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <stdlib.h>
#include "vdmcg.h"
#include "statsem.h"
#include "rt_errmsg.h"
#include "astaux.h"  // to get GetCid()
#include "tokenlist.h" // to get TokenPos
#include "contextinfo.h" // to get GetFilePos
#include "tb_exceptions.h"
#include "backend.h"
#include "tbutils.h"
#include "BC.h"
#include "CPP.h"
#include <string>

TYPE_CGMAIN_VT vdmcg::mk_CG_VT(const TYPE_CPP_Expr & id, const Generic & ti)
{
  TYPE_CGMAIN_VT vt;
  vt.set_name(id);
  vt.set_type(ti);
  return vt;
}

void vdmcg::ReportUndefined(const std::wstring & fct)
{
  vdm_err << L"An internal error occurred in: " <<  fct << endl << flush;
  throw TB_Exception(ERR_CG); 
}

void vdmcg::ReportError(const std::wstring & fct)
{
  vdm_err << L"An internal error occurred in: " <<  fct << endl << flush;
  throw TB_Exception(ERR_CG); 
}     

#ifdef VDMSL
void vdmcg::cg(const TYPE_AS_Document & ast,
               const Map & tc_state,
               const Set & /*cl_nms*/,
               std::wstring (*_GetSourceFile) (int fileid),
               const Generic & m,
               const Bool & s,
               const Bool & p,
               const Bool & t,
               const Bool & st,
               const Generic & package_name,
               const Bool & cop,
               const Bool & testcond)
#endif //VDMSL
#ifdef VDMPP
void vdmcg::cg(const TYPE_AS_Document & ast,
               const SET<TYPE_AS_Name> & cl_nms,
               const Generic & m,
               const Bool & s,
               const Bool & p,
               const Bool & t,
               const Bool & st,
               const Generic & package_name,
               const Bool & cop,
               const Bool & testcond)
#endif //VDMPP
{
  vdm_CPP_setmode(m);

  Quote kind = m;

#ifdef VDMPP
  SET<TYPE_CPP_File> ProgAS (GenClasses(ast, cl_nms, s, p, t, st, package_name, cop, testcond));
#endif // VDMPP

#ifdef VDMSL
  GetSourceFile = _GetSourceFile;
  SET<TYPE_CPP_File> ProgAS (cg_CG(ast, tc_state, testcond));
#endif // VDMSL

  if (!ProgAS.IsEmpty()) {
    CGBackEnd be;
    be.GenFiles(ProgAS, ((kind == ((Quote) L"CPP")) ? CGBackEnd::CPP : CGBackEnd::JAVA));
  }
}

// GenUpdatePosInfo
// rc : AS`Expr | AS`Stmt
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::GenUpdatePosInfo(const Record & rc)
{
  SEQ<TYPE_CPP_Stmt> res;
  int no_of_fields = rc.Length ();

  if (no_of_fields < 1) {
    return res;
  }

#ifdef VDMSL
  int f = -1;
#endif //VDMSL
  Tuple gfp (GetCI().GetFilePos(ASTAUX::GetCid(rc)));
  if (gfp.GetBoolValue(1)) {
#ifdef VDMSL
    const TYPE_CI_FileId & fid (gfp.GetInt(2)); 
    f = fid.GetValue();
#endif //VDMSL
    const TYPE_CI_TokenPos & astpos (gfp.GetRecord(4));
    const Int & line (astpos.GetInt(pos_CI_TokenPos_abs_uline));
    const Int & col (astpos.GetInt(pos_CI_TokenPos_column));
    SEQ<TYPE_CPP_Expr> arg_l;
    arg_l.ImpAppend(vdm_BC_GenIntegerLit(line)); // line
    arg_l.ImpAppend(vdm_BC_GenIntegerLit(col)); // column
    res.ImpAppend(GenPushPosInfo(arg_l));
  }
  
#ifdef VDMSL
  /* -1 signals unknown position so dont change the last position */
  if (f != this->cur_file) {
    this->cur_file = f;
    std::wstring file (GetSourceFile(this->cur_file));
    if (file == L"") {
      SetCurFileName(vdm_BC_GenStringLit(GiveCurCName()));
    }
    else {
      SetCurFileName(vdm_BC_GenStringLit(SEQ<Char>(file)));
    }
  }
#endif //VDMSL

#ifdef VDMPP
  SetCurFileName(vdm_BC_GenStringLit(GiveCurCName()));
#endif //VDMPP

  return res;
}

void vdmcg::SetCurFileName(const TYPE_CPP_Expr & file)
{
  this->cur_file_name_l.Clear();
  this->cur_file_name_l.ImpAppend(file);
}

SEQ<TYPE_CPP_Expr> vdmcg::GetCurFileName()
{
  return this->cur_file_name_l;
}

TYPE_CPP_Stmt vdmcg::GenPushFile()
{
  SEQ<TYPE_CPP_Expr> arg_l (GetCurFileName());
  if (arg_l.IsEmpty()) {
    arg_l.ImpAppend(vdm_BC_GenStringLit(SEQ<Char>(L"??")));
  }
  return vdm_BC_GenExpressionStmt(vdm_BC_GenFctCall(vdm_BC_GenIdentifier(SEQ<Char>(L"CGUTIL::PushFile")), arg_l));
}

TYPE_CPP_Stmt vdmcg::GenPopFile()
{
  return vdm_BC_GenExpressionStmt(vdm_BC_GenFctCall(vdm_BC_GenIdentifier(SEQ<Char>(L"CGUTIL::PopFile")), type_dL()));
}

TYPE_CPP_Stmt vdmcg::GenPushPosInfo(const SEQ<TYPE_CPP_Expr> & arg_l)
{
  return vdm_BC_GenExpressionStmt(vdm_BC_GenFctCall(vdm_BC_GenIdentifier(SEQ<Char>(L"CGUTIL::PushPosInfo")), arg_l));
}

TYPE_CPP_Stmt vdmcg::GenPopPosInfo()
{
  return vdm_BC_GenExpressionStmt(vdm_BC_GenFctCall(vdm_BC_GenIdentifier(SEQ<Char>(L"CGUTIL::PopPosInfo")), type_dL()));
}

#ifdef VDMSL
void vdmcg::Reset(const TYPE_AS_Document & doc_l)
{
  init_BC();
  init_BCCI();

  InitState_CGAUX(Nil());

  GetCI().ResetCGTable();
  
  // Initiailze TPGEN and Declare types 
  InitState_TPGEN(Nil());

  TYPE_AS_Document mod_l;  // seq of (AS`Module | AS`DLModule)
  if ((doc_l.Length() == 1) && doc_l[1].Is(TAG_TYPE_AS_Definitions)) {
    TYPE_AS_Module mod;
    mod.Init(ASTAUX::GetDefaultModASName(),
             TYPE_AS_Interface().Init(type_7AS_NameCUM(), Nil(), NilContextId),
             doc_l[1],
             NilContextId);

    mod_l.ImpAppend(mod);
  }
  else {
    mod_l.ImpConc(doc_l);
  }
  size_t len_mod_l = mod_l.Length();
  for (size_t idx = 1; idx <= len_mod_l; idx++) {
    const Record & md (mod_l[idx]);
    if (md.Is (TAG_TYPE_AS_Module)) {
      AddModuleTypes(md);
    }
  }
  InitState_FCTDEF();
}
#endif // VDMSL

#ifdef VDMPP
//
// vdmcg::Reset
//
// This function takes a sequence containing all of the ASTs in the
// project, and performs the preprocessing of the classes, which at
// the specification level is performed in CLASS`GenClasses.
void vdmcg::Reset(const TYPE_AS_Document & allClasses)
{
  init_BC();
  init_BCCI();

  InitState_CGAUX(Nil());

  GetCI().ResetCGTable();
  
  // Initiailze TPGEN and Declare types 
  InitState_TPGEN(Nil());

  InitState_FCTDEF();

  SEQ<TYPE_AS_Class> nonCPPModules;
  size_t len_allClasses = allClasses.Length();
  for (size_t idx = 1; idx <= len_allClasses; idx++) {
    if( !allClasses[idx].Is(TAG_TYPE_CPP_Module) ) {
      nonCPPModules.ImpAppend(allClasses[idx]);
    }
  }

  SET<TYPE_AS_Name> as_nms;
  size_t len_nonCPPModules = nonCPPModules.Length();
  for (size_t idx2 = 1; idx2 <= len_nonCPPModules; idx2++) {
    const TYPE_AS_Class & cs(nonCPPModules[idx2]);
    InitState_CGAUX(cs.get_nm());
    AddSuperClasses(cs.get_supercls());
    AddAbstractMethods(cs);
    as_nms.Insert(cs.get_nm());

    AddClassTypes(cs);
  }
  GenerateOrderedSupers();
  ProcessMethodNames();

  if (vdm_CPP_isJAVA()) {
    ProcessAbstractMethods();
  }
  for (size_t idx3 = 1; idx3 <= len_nonCPPModules; idx3++) {
    const TYPE_AS_Class & cs(nonCPPModules[idx3]);    
    Generic optdefs(cs.get_defs());
    if(!optdefs.IsNil()) {
      TYPE_AS_Definitions defs(optdefs);
      InitState_TPGEN(cs.get_nm());
      GetStatSem().ExpandClass (POS, cs);
      AddInstanceVars(cs.get_nm(), defs.get_instvars());
      if (vdm_CPP_isJAVA()) {
        AddPackageName(cs.get_nm(), defs.get_valuem());
      }
    }
  }

  InitSpecState();                             

  ExtractGlobalInformation(nonCPPModules, as_nms /*GetAllVDMModuleNames()*/ );
}
#endif //VDMPP
