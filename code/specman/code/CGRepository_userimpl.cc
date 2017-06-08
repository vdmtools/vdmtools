/***
*  * WHAT
*  *    Implementation of preliminary methods in class CGRepository
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/specman/code/CGRepository_userimpl.cc,v $
*  * VERSION
*  *    $Revision: 1.4 $
*  * DATE
*  *    $Date: 2005/01/21 03:16:06 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    IFAD-VDM28
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "CGRepository.h"
#include "projectval.h"
#include "tbutils.h"
#include "rt_errmsg.h"
#include "tb_exceptions.h"
#include "tb_wstring.h"
#include <fstream>
#include <string>

static std::string CGReposFile ("m4tag_rep");

void vdm_CGRepository::vdm_WriteCGInfo (const TYPE_ProjectTypes_CGInfo &cg_info)
{
  // Write new TagRepository to m4tag_rep
  std::ofstream ostr;
  ostr.open(CGReposFile.c_str(), ios::out);
  if (!ostr) {
    vdm_err << L"Couldn't open file '" << TBWSTR::string2wstring(CGReposFile) << L"'\n";
    return;
  }
  Tuple info (PTAUX::ExtractCGInfo (cg_info));
  info.WriteVal(ostr);
  ostr.close();
}

TYPE_ProjectTypes_CGInfo vdm_CGRepository::vdm_GetCGInfo ()
{
  Tuple info(3);
  std::ifstream istr (CGReposFile.c_str());
  
  if (istr) {
    // Read existing TagRepository.
    Generic rep;
    rep = ReadVal (istr);
    if (rep.IsTuple ()) {
      info = rep;
      istr.close();
    }
    else {
      vdm_log << TBWSTR::string2wstring(CGReposFile) << L" is not a tag repository\n";
      istr.close();
      throw TB_Exception(ERR_CG); 
      return (Generic) Record (); // to avoid warnings
    }
  }      
  else  {
    // Construct an empty tag repository.
    info.SetField(1, Map());
    info.SetField(2, Set());
    info.SetField(3, Int(0));
  }
  return (Generic) PTAUX::mk_CGInfo (info);
}
