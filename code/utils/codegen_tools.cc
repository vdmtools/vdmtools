/***
*  * WHAT
*  *    Implementation of the tools used for C++ and Java code generation
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/utils/codegen_tools.cc,v $
*  * VERSION
*  *    $Revision: 1.6 $
*  * DATE
*  *    $Date: 2006/06/06 01:09:54 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "codegen_tools.h"
#include "tbutils.h"
#include "Repository.h"
#include "projectval.h" //For ExtractAstVal
#include "statsem.h"
#include "tools.h"      //For EvalTypeCheck
#include "tb_exceptions.h"
#include "astaux.h"
#include "settings.h"

static CodeGenTools cgtools;

CodeGenTools & GetCGTools()
{
  return cgtools;
}

vdmcg & GetVDMCG()
{
  return GetCGTools().GetCGi();
}

void CodeGenTools::InitCG()
{
  this->CGi.InitState_CGMAIN();
  // Get the CG_RTI option from the options class
  this->CGi.SetRTI(Settings.GetCGRTI());

#ifdef VDMPP
  this->CGi.set_longs_option(Settings.GetJCGLongs());
  this->CGi.set_interfaces(PTAUX::ModuleNameSet2ASNameSet(Settings.GetJCGInterfaces()));
  this->CGi.set_java_rename_option(Settings.GetJCGVDMPrefix());
#endif // VDMPP
}

bool CodeGenTools::ResetCG(const Quote & kind)
{
  try {
    vdm_CPP_setmode(kind);
    InitCG();
    this->CGi.Reset(ToolMediator::GetVDMASTs());
  }
  catch (TB_Exception & e) {
    return false;
  }
  return true;
}

bool CodeGenTools::CheckAllowed(const TYPE_ProjectTypes_ModuleName & modnm, bool& typeCheckerUsed)
{
  Tuple allowres (ToolMediator::Repos()->vdm_Allowed (modnm, action_cg));

  if (!allowres.GetBoolValue(1)) {
    const SEQ<Char> & mes (allowres.GetSequence (2));
    if (!mes.IsEmpty()) {
      vdm_log << PTAUX::Seq2Str (mes) << endl << flush;
      return false;
    }
    else { 
#ifdef VDMSL    
      vdm_log << L"The module must be possibly well-formed." << endl << flush;
#endif //VDMSL
#ifdef VDMPP
      vdm_log << L"The class must be possibly well-formed." << endl << flush;
#endif //VDMPP    
      TOOLS::EvalTypeCheck (modnm, POS, vdm_log);
      typeCheckerUsed = true;
    }
  }
  Tuple allowres2 (ToolMediator::Repos()->vdm_Allowed (modnm, action_cg));
  return allowres2.GetBoolValue(1);
}

//bool CodeGenTools::FeatureOk(const Quote & kind,
//                             const TYPE_ProjectTypes_ModuleName & modnm,
//                             char*& feature)
//{
//  return true;
//}

// The following function is used by the code generator (parsed
// as a parameter to vdmcg::cg) to lookup a filename
#ifdef VDMSL
static wstring GetSourceFile (int fileid)
{
  Generic file = ToolMediator::Repos()->vdm_GetFileName (fileid);
  if (file.IsNil()) {
    return L""; // unknown file
  }
  else {
    return PTAUX::ExtractFileName (TYPE_ProjectTypes_FileName(file));
  }
}
#endif // VDMSL

bool CodeGenTools::EvalCodeGen (const TYPE_ProjectTypes_ModuleName & modnm, 
                                const Quote & kind, 
                                const Bool & skeletonOption, 
                                const Bool & prePostOption, 
                                const Bool & onlyTypesOption, 
                                const Bool & smallTypesOption, 
                                const Generic & package_name, 
                                const Bool & concurrencyOption, 
                                const Bool & testPrePostOption)
{
  bool typeCheckerUsed = false;
  bool allowed = CheckAllowed(modnm, typeCheckerUsed);
  if (!allowed) {
    return false;
  }
  
  try {
    TBUTILS::ClearErrorsCount();
    if (!typeCheckerUsed || TOOLS::isBatchMode()) {
      GetStatSem().ResetErrors();
    }

    vdm_CPP_setmode(kind);

    // Initialize the code generator
    InitCG();

    Record ast_val (PTAUX::ExtractAstOrFlat (ToolMediator::Repos()->vdm_GetAST (modnm)));

    Tuple ti (TOOLS::InitStatSem (ast_val));

    vdm_log << L"Generating code ... " << endl << flush;
    if (TBUTILS::GetErrorsCount() == 0) {
#ifdef VDMSL
      Map TypeCheckState;
      TYPE_AS_Name nm (ASTAUX::MkNameFromId (ASTAUX::MkId(PTAUX::ExtractModuleName (modnm)), NilContextId));
      TypeCheckState.Insert (nm, ti);

      this->CGi.cg(mk_sequence(ast_val), 
                   TypeCheckState, 
                   ToolMediator::GetAllVDMModuleNames(), 
                   GetSourceFile, 
                   kind, 
                   skeletonOption, 
                   prePostOption, 
                   onlyTypesOption, 
                   smallTypesOption, 
                   package_name, 
                   concurrencyOption,
                   testPrePostOption);
#endif // VDMSL
#ifdef VDMPP
      this->CGi.cg(mk_sequence(ast_val), 
                   ToolMediator::GetAllVDMModuleNames(), 
                   kind, 
                   skeletonOption, 
                   prePostOption, 
                   onlyTypesOption, 
                   smallTypesOption, 
                   package_name, 
                   concurrencyOption,
                   testPrePostOption);
#endif // VDMPP
    }
  }
  catch (TB_Exception & e) {
    vdm_err << L"Internal Error" << endl;
    TBUTILS::IncrementErrorsCount();
  }
  
  if (TBUTILS::GetErrorsCount() > 0) {
    vdm_log << L"done (Errors detected)" << endl << flush;
    ToolMediator::UpdateCG(modnm, kind, Bool(false));
  }
  else {
    vdm_log << L"done" << endl << flush;   
    ToolMediator::UpdateCG(modnm, kind, Bool(true));
  }
  TOOLS::OutputStatSemErrors(Nil());

  return (TBUTILS::GetErrorsCount() == 0);
}

void CodeGenTools::SetCGOptions(const Bool & skeletons,
                                const Bool & preAndPost, 
                                const Bool & onlyTypes,
                                const Bool & smallTypes,
                                const Generic & package_name, 
                                const Bool & concurrency,
                                const Bool & testPreAndPost)
{
#ifdef VDMPP
  this->CGi.set_skeleton_option(skeletons.GetValue());
  this->CGi.set_preandpost_option(preAndPost.GetValue());
  this->CGi.set_onlytypes_option(onlyTypes.GetValue());
  this->CGi.set_smalltypes_option(smallTypes.GetValue());
  this->CGi.set_package_option(package_name);
  this->CGi.set_conc_option(concurrency.GetValue());
#endif // VDMPP
  this->CGi.set_testpreandpost_option(testPreAndPost.GetValue());
}

#ifdef VDMPP
void CodeGenTools::ResetJCGInterfaces() {
  Settings.SetJCGInterfaces(SET<TYPE_ProjectTypes_ModuleName>());
}

Set CodeGenTools::EvalPossibleInterfaces ()
{
  if (!ToolMediator::Repos()->vdm_IsAllTypeCorrect()) {
    vdm_log << L"All classes must be [pos] type correct to compute possible interfaces." << endl << flush;
    return Set();
  }
  
  Set possibleInterfaces;
  try {
    TBUTILS::ClearErrorsCount();
    GetStatSem().ResetErrors();

    //InitCG(); // 20110603
    SetCGOptions(Settings.GetJCGSkel(),
                 Settings.GetJCGGenPrePost(), 
                 Settings.GetJCGTypes(),
                 Settings.GetJCGSmallTypes(), 
                 Settings.GetJCGPackage(),
                 Settings.GetJCGConcur(), 
                 Settings.GetJCGCheckPrePost());
    if (TBUTILS::GetErrorsCount() == 0) {
      SEQ<TYPE_AS_Class> cl_l (ToolMediator::GetVDMASTs()); 
      SET<TYPE_AS_Name> mnm_s (PTAUX::ModuleNameSet2ASNameSet(Settings.GetJCGInterfaces()));
      this->CGi.set_interfaces(mnm_s); 
      possibleInterfaces = this->CGi.GetPossibleInterfaces(cl_l);
      this->CGi.set_interfaces(Set()); 
    }
  }
  catch (TB_Exception & e) {
    vdm_err << L"Internal Error" << endl;
    TBUTILS::IncrementErrorsCount();
  }
  
  if (TBUTILS::GetErrorsCount() > 0) {
    vdm_log << L"done (Errors detected)" << endl << flush;
  }
  TOOLS::OutputStatSemErrors(Nil());

  return possibleInterfaces;
}
#endif //VDMPP

