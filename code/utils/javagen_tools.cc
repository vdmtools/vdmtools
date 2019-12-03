/***
*  * WHAT
*  *    Implementation of the tools used for manipulating java source files
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/utils/javagen_tools.cc,v $
*  * VERSION
*  *    $Revision: 1.7 $
*  * DATE
*  *    $Date: 2006/06/06 01:09:54 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "javagen_tools.h"
#include "projectval.h"    // for ExtractFileName
#include "tbutils.h"       // for vdm_log
#include "java2vdm.h"      // for java_parse
#include "tools.h"         // for Repos/GetCI
#include "BaseTools.h"
#include "astaux.h"        // for MkNameFromId
#include "JSSVCM1.h"
#include "JSSDEF.h"
#include "J2V.h"
#include "XF.h"
#include "as2ascii.h"
#include "tb_wstring.h"
#include "tb_exceptions.h"
#include <sys/stat.h>
#include <sstream>
#include <string>

#ifdef _MSC_VER
#include <direct.h>    // For getcwd
#endif // _MSC_VER

#ifdef VDMPP

bool JavaGenTools::EvalJavaParse(const TYPE_ProjectTypes_FileName & filename)
{
  std::wstring fn (PTAUX::ExtractFileName(filename));
  TYPE_CPP_File ast;
  std::ifstream inp;
  inp.open (TBWSTR::wstring2fsstr(fn).c_str());
  inp.seekg(0);
  SetInput(inp);

  // Initialize the fileId if necessary
  TYPE_ProjectTypes_FileId fileId (ToolMediator::GetFileId(filename).GetValue());
  if (fileId.GetValue() == 0) {
    SET<TYPE_ProjectTypes_FileName> s;
    s.Insert(filename);
    ToolMediator::AddFiles(s);
    fileId = ToolMediator::GetFileId(filename).GetValue();
  }

  // Parse the file
  vdm_log << L"Parsing \"" << fn << L"\" (Java)" << flush;
  bool parse_error = java_parse((wchar_t*) fn.c_str(),
                                true,
                                ToolMediator::Errs(),
                                ast,
                                GetCI(),
                                true,
                                TYPE_CI_FileId(fileId)
                                );

  // Check result of the parse
  if (parse_error) {
    // Update status in spec manager
    ToolMediator::UpdateSC(filename, Sequence());
    vdm_log << L"done (Errors detected)" << endl << flush;
    return false;
  }
  else {
    // First, update parse env in JSS

    // Need to decompose the generated AST, since one java file could
    // contain several classes
    Sequence moduleAsts;
    Generic header (ast.get_h());
    Sequence cppas (ast.get_ast());
    Generic g;
    for (int i = cppas.First(g); i; i = cppas.Next(g)) {
      TYPE_CPP_IdentDeclaration classDecl(g);
      Sequence declSpecifiers(classDecl.get_ds());

      Generic h;
      for (int j = declSpecifiers.First(h); j; j = declSpecifiers.Next(h)) {
        TYPE_CPP_TypeSpecifier ts (h);

        TYPE_CPP_IdentDeclaration thisIdent(classDecl);
        thisIdent.set_ds((Generic) Sequence().ImpAppend(ts));

        TYPE_CPP_Module m;
        std::wstring nameStr;
        if (Record(ts.get_tp()).GetTag() == TAG_TYPE_CPP_ClassSpecifier) {
          TYPE_CPP_ClassSpecifier cls (ts.get_tp());
          TYPE_CPP_ClassHead ch (cls.get_ch());
          TYPE_CPP_Identifier classNm (ch.get_name());
          Sequence classNmSeq (classNm.get_id());
          classNmSeq.GetString(nameStr);
        }
        else {
          // InterfaceSpecifier
          TYPE_CPP_InterfaceSpecifier isp (ts.get_tp());
          TYPE_CPP_InterfaceHead ih (isp.get_ih());
          TYPE_CPP_Identifier iName (ih.get_name());
          Sequence interfaceNmSeq (iName.get_id());
          interfaceNmSeq.GetString(nameStr);
        }

	// Internally java classes are distinguished from VDM++
	// classes using the prefix "java|"
        nameStr = L"java|" + nameStr;
        m.set_name(ASTAUX::MkName(nameStr));
        m.set_header(header);
        m.set_ast(thisIdent);
        moduleAsts.ImpAppend(m);
        vdm_JSSVCM1_UpdateParseEnvWithModule(m);
      }
    }

    ToolMediator::UpdateRepository (filename, fileId, ast, moduleAsts, true);

    // Set/update timestamp in repository
    struct stat buf;
    long mtime = 0L;
    int s = stat (TBWSTR::wstring2string(fn).c_str (), &buf);
    if (!s) {
      mtime = buf.st_mtime;
    }
    ToolMediator::SetFileTimestamp (filename, Int (mtime));

    ToolMediator::UpdateRepository (filename, fileId, ast, moduleAsts, true);
    vdm_log << L"done" << endl << flush;
    return true;
  }
}

void JavaGenTools::UpdateJSS(const TYPE_CPP_Module& m)
{
}

bool JavaGenTools::EvalJavaTypeCheck(const TYPE_ProjectTypes_ModuleName& nm)
{
  // Run the java static semantics check on the given module

  // Inform the user what is happening
  std::wstring moduleName (PTAUX::ExtractModuleName(nm));
  vdm_log << L"Type checking \"" << moduleName.substr(5) << L"\" (Java)" << flush;

  // Extract the java module from the repository
  TYPE_ProjectTypes_AstVal ast_val (ToolMediator::GetAST (nm));
  TYPE_CPP_Module module (PTAUX::ExtractAstVal(ast_val));

  // Initialize the static semantics module
  ClearJavaErrors();

  // Run the static semantics check
  bool tcOk = true;
  try {
    tcOk = vdm_JSSDEF_jwf_uModule(module).GetValue();
  }
  catch (TB_Exception &e) {
    vdm_err << L"Please report this error." << endl << flush;
  }

  // Check for errors
  int no_of_errors = GetNumJavaErrs();

  // If errors were found, output them to the interface
  if (no_of_errors > 0) {
    vdm_log << L"done (Errors detected)" << endl << flush;
    SEQ<TYPE_ProjectTypes_Message> err_l (PTAUX::QS (GetJavaErrors(), false));;
    size_t len_err_l = err_l.Length();
    for (size_t idx = 1; idx <= len_err_l; idx++)
      ToolMediator::Errs()->vdm_AddMsg(err_l[idx]);

    ToolMediator::Errs()->vdm_Done();

    // Update the status of the class in the repository
    ToolMediator::UpdateTC(nm, Nil());
  }
  else {
    vdm_log << L"done" << endl << flush;
    // Update the status of the class in the repository
    ToolMediator::UpdateTC(nm, tc_def);
  }
  // Reset the errors
  ClearJavaErrors();

  // Update the interface
  ToolMediator::UpdateSelections();
  return tcOk;
}

void JavaGenTools::AfterJavaTypeCheck()
{
   vdm_JSSENV_RemoveUnknownClasses();
}

bool JavaGenTools::EvalJavaGenerateVDM(const TYPE_ProjectTypes_ModuleName & nm,
                                       const Bool & stubsOpt,
                                       const Bool & renameOpt,
                                       const Bool & trans)
{
//    // Check that this action is allowed
//    Tuple allowres = ToolMediator::Repos()->vdm_Allowed (nm, action_cg);
//    Bool allowed = allowres.GetField (1);
//    Sequence mes = allowres.GetField (2);

//    // If not allowed, try type checking
//    if (!allowed.GetValue ()) {
//      if (mes.Length ()) {
//        vdm_log << Seq2Str (mes) << L"\n" << flush;
//        return false;
//      }
//      else {
//        vdm_log << L"The class must be type correct.\n" << flush;
//        EvalJavaTypeCheck (nm);
//      }
//    }

//    allowres = ToolMediator::Repos()->vdm_Allowed (nm, action_cg);
//    allowed = allowres.GetField (1);

//    // If still not allowed, give up
//    if (!allowed.GetValue ())
//      return false;


  if (!EvalJavaTypeCheck (nm)) {
    return false;
  }
  Generic ast_val_g (ToolMediator::GetAST (nm));
  if (ast_val_g.IsNil()) {
    return false;
  }
  SET<TYPE_XF_SXFId> strans;
  SET<TYPE_XF_EXFId> etrans;
  if (trans.GetValue()) {
    // etrans.Insert(TYPE_XF_EXFId().Init(Int(1)));
    // etrans.Insert(TYPE_XF_EXFId().Init(Int(2)));
    etrans.Insert(TYPE_XF_EXFId().Init(Int(3)));

    strans.Insert(TYPE_XF_SXFId().Init(Int(1)));
    strans.Insert(TYPE_XF_SXFId().Init(Int(2)));
    strans.Insert(TYPE_XF_SXFId().Init(Int(3)));
    strans.Insert(TYPE_XF_SXFId().Init(Int(4)));
    strans.Insert(TYPE_XF_SXFId().Init(Int(5)));
    strans.Insert(TYPE_XF_SXFId().Init(Int(6)));
    strans.Insert(TYPE_XF_SXFId().Init(Int(7)));
    strans.Insert(TYPE_XF_SXFId().Init(Int(8)));
    strans.Insert(TYPE_XF_SXFId().Init(Int(9)));
    strans.Insert(TYPE_XF_SXFId().Init(Int(10)));
    //    strans.Insert(TYPE_XF_SXFId().Init(Int(11)));
    strans.Insert(TYPE_XF_SXFId().Init(Int(12)));
  }

  TYPE_ProjectTypes_AstVal ast_val (ast_val_g);
  std::wstring outputDir (TBUTILS::tb_getcwd());

  vdm_J2V_J2VSetTransMethods(strans, etrans);

  TYPE_CPP_Module module (PTAUX::ExtractAstVal(ast_val));
  TYPE_AS_SpecFile sf (vdm_J2V_J2VModule(module, stubsOpt, renameOpt));

  TYPE_CI_TokenContextInfo tci (sf.get_tokenci());
  Sequence classes (sf.get_vdm());

  Generic myG (sf.GetField(1));
  std::wstring packageName;
  if (!myG.IsNil()) {
    Sequence sfName (myG);
    sfName.GetString(packageName);
  }
  if(packageName.compare(L"nil")==0) {
    packageName.erase();
  }
  std::wstring packageDir(outputDir+L"/"+packageName);
#ifndef _MSC_VER
  mkdir(TBWSTR::wstring2string(packageDir).c_str(), 0777);
#else
  _mkdir(TBWSTR::wstring2string(packageDir).c_str());
#endif // _MSC_VER

  SEQ<TYPE_ProjectTypes_FileName> newFiles;
  Generic g;
  for (int j = classes.First(g); j; j = classes.Next(g)) {
    TYPE_AS_Class cls (g);
    TYPE_AS_Name nm (cls.get_nm());
    std::wstring clsName (ASTAUX::GetFirstId(nm).GetString());

    std::wostream* os;
    std::wstring fileName(outputDir + L"/" );
    if (packageName != L"") {
      fileName = fileName + packageName + L"/";
    }
    fileName = fileName + clsName + L".vpp";

    // On Linux, wofstreams are written as binary data, so the ascii
    // is generated in a wstringstream, then converted to an ascii
    // string and finally written to an ofstream
#ifdef _MSC_VER
    os = (std::wostream*) new wofstream(TBWSTR::wstring2fsstr(fileName).c_str());
#else
    os = (std::wostream*) new wstringstream();
#endif //_MSC_VER

    AS2ASCII conv;
    vdm_log << L"Writing " << fileName << endl << flush;
    conv.BeginDocument(*os);
    conv.Class2ASCII(cls, *os, tci);
    conv.EndDocument(*os);

    // Tidy up after generation of VDM++ file
#ifndef _MSC_VER
    std::ofstream wof(TBWSTR::wstring2string(fileName).c_str());
    wof << TBWSTR::wstring2string(((wstringstream*) os)->str()) << std::flush;
    wof.close();
#endif //_MSC_VER

    vdm_log << L"Finished "<< fileName  << endl << flush;
    newFiles.ImpAppend(PTAUX::mk_FileName(fileName));
  }

  if (!newFiles.IsEmpty()) {
    ToolMediator::AddFiles(newFiles.Elems());
    ToolMediator::BTools()->vdm_SyntaxCheck(newFiles);
  }

  int no_of_errors = GetNumJavaErrs();;
  Quote kind (L"CPP");
  if (no_of_errors > 0) {
    vdm_log << L"done (Errors detected)" << endl << flush;
    ToolMediator::UpdateCG(nm, kind, Bool(false));
  }
  else {
    vdm_log << L"done" << endl << flush;
    ToolMediator::UpdateCG(nm, kind, Bool(true));
  }

  Sequence err_l = GetJavaErrors();
  for (int i = err_l.First(g); i; i=err_l.Next(g)) {
    Record r = g;
    ToolMediator::Errs()->vdm_AddMsg(g);
  }
  ToolMediator::Errs()->vdm_Done();
  ClearJavaErrors();
  ToolMediator::UpdateSelections();

  return (no_of_errors == 0);
}

#endif //VDMPP
