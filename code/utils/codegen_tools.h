/***
*  * WHAT
*  *    Top level interface to the tools used for C++ and Java code generation
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/utils/codegen_tools.h,v $
*  * VERSION
*  *    $Revision: 1.2 $
*  * DATE
*  *    $Date: 2001/10/22 12:14:26 $
*  * AUTHOR
*  *    $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __CODEGEN_TOOLS_H__
#define __CODEGEN_TOOLS_H__

#include "metaiv.h"
#include "ProjectTypes.h"
#include "vdmcg.h"

class CodeGenTools
{
public:
  CodeGenTools() {}
  ~CodeGenTools() {}
//  void CleanupCG();
  bool ResetCG(const Quote & kind);
  void SetCGOptions(const Bool & skeletons,
                    const Bool & preAndPost, 
                    const Bool & onlyTypes,
                    const Bool & smallTypes,
                    const Generic & package_name, 
                    const Bool & concurrency,
                    const Bool & testPreAndPost);
  void ResetJCGInterfaces();
  Set EvalPossibleInterfaces ();
  bool EvalCodeGen (const TYPE_ProjectTypes_ModuleName & modnm,
                    const Quote & kind,
                    const Bool & s,
                    const Bool & p,
                    const Bool & t,
                    const Bool & st, 
                    const Generic & package_name,
                    const Bool & cop,
                    const Bool & testcond);
  vdmcg & GetCGi() { return this->CGi; }; 
private:
  void InitCG();
  bool CheckAllowed(const TYPE_ProjectTypes_ModuleName & modnm,
                    bool& typeCheckerUsed);
//  bool FeatureOk(const Quote & kind,
//                 const TYPE_ProjectTypes_ModuleName & modnm,
//                 char*& feature);

  vdmcg CGi;
};

CodeGenTools & GetCGTools();
vdmcg & GetVDMCG();

#endif //__CODEGEN_TOOLS_H__
